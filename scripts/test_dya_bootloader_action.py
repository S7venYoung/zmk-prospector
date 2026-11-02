#!/usr/bin/env python3
"""Run the real action and patched setter against mocked reboot/work APIs."""
from pathlib import Path
import subprocess
import tempfile
from patch_dya_bootloader_action import patch, ANCHOR, HOOK, MARKER

ROOT = Path(__file__).resolve().parents[1]
STUB = r'''
#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#define CONFIG_PROSPECTOR_DYA_BOOTLOADER_ACTION 1
#define IS_ENABLED(x) (x)
#define ARG_UNUSED(x) ((void)(x))
#define LOG_MODULE_REGISTER(...)
#define LOG_ERR(...) ((void)0)
#define K_MSEC(x) (x)
#define BOOT_MODE_TYPE_BOOTLOADER 1
#define SYS_REBOOT_WARM 0
typedef int atomic_t;
static bool atomic_cas(atomic_t *v,int old,int next){if(*v!=old)return false;*v=next;return true;}
static void atomic_clear(atomic_t *v){*v=0;}
struct k_work { int unused; };
#define K_WORK_DELAYABLE_DEFINE(name,callback) static struct k_work name
struct zmk_behavior_binding {const char *behavior_dev;uint32_t param1,param2;};
static int schedule_result,boot_result,schedules,boots,reboots,delay_ms,pending_writes;
static int k_work_reschedule(struct k_work *work,int delay){(void)work;schedules++;delay_ms=delay;return schedule_result;}
static int bootmode_set(int mode){assert(mode==BOOT_MODE_TYPE_BOOTLOADER);boots++;return boot_result;}
static void sys_reboot(int type){assert(type==SYS_REBOOT_WARM);reboots++;}
static struct zmk_behavior_binding zmk_keymap[1][1];
'''
SETTER = '''#include <zmk/keymap.h>
static int set_binding(int idx,struct zmk_behavior_binding binding) {
    if(idx!=0)return -EINVAL;
    int layer_id=0,storage_binding_idx=idx;
''' + ANCHOR + '''
        return 0;
    }
    memcpy(&zmk_keymap[layer_id][storage_binding_idx],&binding,sizeof(binding));
    pending_writes++;
    return 0;
}
'''
MAIN = r'''
int main(void){
    struct zmk_behavior_binding boot={.behavior_dev="bootload"};
    struct zmk_behavior_binding normal={.behavior_dev="key_press",.param1=4};
    assert(set_binding(0,normal)==0&&pending_writes==1&&schedules==0);
    struct zmk_behavior_binding saved=zmk_keymap[0][0];
    assert(set_binding(1,boot)==-EINVAL&&schedules==0);
    assert(set_binding(0,boot)==0&&schedules==1&&delay_ms==500);
    assert(boots==0&&reboots==0&&pending_writes==1);
    assert(!memcmp(&saved,&zmk_keymap[0][0],sizeof(saved)));
    assert(set_binding(0,boot)==-EBUSY&&schedules==1);
    enter_bootloader(&boot_work);assert(boots==1&&reboots==1);
    /* Even if an existing saved key is already Bootloader, execute the action. */
    zmk_keymap[0][0]=boot;
    assert(set_binding(0,boot)==0&&schedules==2);
    boot_result=-ENODEV;enter_bootloader(&boot_work);assert(boots==2&&reboots==1);
    schedule_result=-ENOMEM;assert(set_binding(0,boot)==-ENOMEM);
    schedule_result=0;assert(set_binding(0,boot)==0);
    boot_result=0;enter_bootloader(&boot_work);assert(reboots==2);
    assert(prospector_dya_binding_action(NULL)==0);
    struct zmk_behavior_binding empty={0};assert(prospector_dya_binding_action(&empty)==0);
    assert(prospector_dya_binding_action(&normal)==0);
    struct zmk_behavior_binding bluetooth={.behavior_dev="bluetooth",.param1=0};
    int scheduled=schedules;assert(set_binding(0,bluetooth)==0&&schedules==scheduled);
    assert(pending_writes==2); /* Only normal edits were written. */
    return 0;
}
'''
with tempfile.TemporaryDirectory() as temp:
    root=Path(temp);target=root/'keymap.c';target.write_text(SETTER)
    patch(target);once=target.read_text();patch(target);assert target.read_text()==once
    for bad in ('unrecognised source',SETTER+SETTER,SETTER+MARKER):
        target.write_text(bad)
        try:patch(target)
        except SystemExit:pass
        else:raise AssertionError('Changed/duplicate/partial source must fail closed')
        assert target.read_text()==bad
    for n in ('zmk/keymap.h','zmk/behavior.h','zephyr/kernel.h','zephyr/logging/log.h',
              'zephyr/retention/bootmode.h','zephyr/sys/atomic.h','zephyr/sys/reboot.h'):
        p=root/n;p.parent.mkdir(parents=True,exist_ok=True);p.write_text('')
    c=root/'test.c';c.write_text(STUB+once+'\n'+(ROOT/'src/dya_bootloader_action.c').read_text()+MAIN)
    subprocess.run(['cc','-std=c11','-Wall','-Wextra','-Werror','-I',str(root),str(c),'-o',str(root/'test')],check=True)
    subprocess.run([str(root/'test')],check=True)
print('DYA BL: delayed action, no key persistence, duplicate suppression, failures, ordinary edits and patch guards passed')
