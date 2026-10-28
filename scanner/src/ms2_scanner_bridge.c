/* Copyright (c) 2026 S7venYoung; SPDX-License-Identifier: MIT */
#include "s7789_update.h"
#include "fault_recovery.h"
#include "prospector_layouts.h"
#include "dashboard_layouts.h"
#include <string.h>
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
#include "dya_settings.h"
#endif
volatile int8_t ble_signal_rssi=-100;
volatile int32_t ble_signal_rate_x100=-1;
static bool empty_notified,source_known;
static int selected=-1;
static uint8_t source_address[6];
void scanner_core_process_alive(void){ble_core_process_alive();}
bool ble_get_pending_update(struct pending_display_data *out) {
    if(!out)return false;
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    int wanted=prospector_dya_scanner_keyboard();struct zmk_keyboard_status wanted_status;
    if(wanted>=0&&wanted!=scanner_get_selected_keyboard()&&scanner_copy_keyboard_status(wanted,&wanted_status))
        scanner_set_selected_keyboard(wanted);
#endif
    bool fresh=scanner_get_pending_update(out);
    if(!fresh&&scanner_get_active_keyboard_count()>0)return false;
    if(!fresh){if(empty_notified)return false;memset(out,0,sizeof(*out));out->no_keyboards=true;}
    if(out->no_keyboards) {
        struct prospector_keyboard_data missing={0};memcpy(missing.current_layer_name,"WAIT",5);
        prospector_layouts_update(&missing);dashboard_layout_reset_history();empty_notified=true;source_known=false;
        return true;
    }
    int current=scanner_get_selected_keyboard();struct zmk_keyboard_status status;
    if(scanner_copy_keyboard_status(current,&status)) {
        if(!source_known||selected!=current||memcmp(source_address,status.ble_addr,6))dashboard_layout_reset_history();
        selected=current;memcpy(source_address,status.ble_addr,6);source_known=true;
    }
    empty_notified=false;
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    int batteries[2]={out->bat[0],out->bat[1]};
    int left=prospector_dya_hand_slot(0),right=prospector_dya_hand_slot(1);
    out->bat[0]=left>=0&&left<2?batteries[left]:0;out->bat[1]=right>=0&&right<2?batteries[right]:0;
#endif
    return true;
}
bool ble_is_signal_pending(void) {
    if(!scanner_is_signal_pending())return false;
    ble_signal_rssi=scanner_signal_rssi;ble_signal_rate_x100=scanner_signal_rate_x100;return true;
}
bool ble_get_kb_version(uint8_t *a,uint8_t *b,uint8_t *c,bool *dev,char *name,size_t len){return scanner_get_kb_version(a,b,c,dev,name,len);}
int ble_get_selected_keyboard(void){return scanner_get_selected_keyboard();}
int ble_set_selected_keyboard(int index){return scanner_set_selected_keyboard(index);}
bool ble_copy_keyboard_status(int index,struct zmk_keyboard_status *out){return scanner_copy_keyboard_status(index,out);}
int ble_msg_send_display_refresh(void){return scanner_msg_send_display_refresh();}
void ble_signal_update(int8_t rssi,int32_t rate){(void)rssi;(void)rate;}
