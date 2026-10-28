/* Real receiver/core sources; kernel, clock and BLE driver mocked. */
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
uint32_t test_now=1000;
static uint8_t test_channel;
#include "s7789_update.h"
#include "prospector_layouts.h"
static unsigned history_resets,empty_views;
static int requested_slot=-1,hand_slots[2]={0,1};
int prospector_dya_scanner_keyboard(void){return requested_slot;}
int prospector_dya_hand_slot(int hand){return hand_slots[hand];}
void dashboard_layout_reset_history(void){history_resets++;}
void ble_core_process_alive(void){}
void prospector_layouts_update(const struct prospector_keyboard_data *data){if(!data->has_dynamic_data)empty_views++;}

#include "../src/scanner_core.c"
#include "../src/status_scanner.c"
uint8_t scanner_get_runtime_channel(void){return test_channel;}
_Static_assert(sizeof(struct zmk_status_adv_data)==26,"legacy packet size");
static void packet(const struct zmk_status_adv_data *d,uint8_t addr_byte) {
    uint8_t raw[2+sizeof(*d)];raw[0]=sizeof(*d)+1;raw[1]=BT_DATA_MANUFACTURER_DATA;memcpy(raw+2,d,sizeof(*d));
    struct net_buf_simple buf={.data=raw,.len=sizeof(raw)};
    bt_addr_le_t addr={.type=1,.a={{addr_byte,2,3,4,5,6}}};scan_callback(&addr,-55,0,&buf);
    scanner_process_incoming();
}
int main(void) {
    scanner_init_start();assert(!zmk_status_scanner_start());
    struct zmk_status_adv_data data={.manufacturer_id={255,255},.service_uuid={0xab,0xcd},.version=0x22,.battery_level=83,.active_layer=2,.profile_slot=3,.status_flags=ZMK_STATUS_FLAG_USB_HID_READY,.device_role=ZMK_DEVICE_ROLE_CENTRAL,.peripheral_battery={79,0,0},.layer_name={'B','A','S','E'},.keyboard_id={0xf1,2,3,4},.modifier_flags=0x88,.wpm_value=42,.channel=2};
    struct pending_display_data out;
    data.manufacturer_id[0]=0;packet(&data,1);assert(scanner_get_active_keyboard_count()==0);
    data.manufacturer_id[0]=255;test_channel=3;packet(&data,1);assert(scanner_get_active_keyboard_count()==0);
    test_channel=2;packet(&data,1);assert(scanner_get_active_keyboard_count()==1);
    assert(scanner_get_pending_update(&out));assert(out.layer==2&&out.wpm==42&&out.usb_ready&&out.modifiers==0x88);assert(out.bat[0]==83&&out.bat[1]==79);assert(!memcmp(out.layer_name,"BASE",4));
    /* Malformed AD lengths must neither over-read nor create a keyboard. */
    uint8_t bad[]={28,255,1,2,3};struct net_buf_simple shortbuf={.data=bad,.len=sizeof(bad)};
    bt_addr_le_t addr={.a={{9,8,7,6,5,4}}};scan_callback(&addr,-70,0,&shortbuf);scanner_process_incoming();assert(scanner_get_active_keyboard_count()==1);
    /* A second sender remains independently selectable. */
    data.keyboard_id[0]=9;data.wpm_value=80;data.status_flags=ZMK_STATUS_FLAG_BLE_CONNECTED;packet(&data,9);assert(scanner_get_active_keyboard_count()==2);
    assert(scanner_set_selected_keyboard(1)==0);assert(ble_get_pending_update(&out));assert(out.wpm==80&&out.ble_connected&&!out.usb_ready);assert(history_resets==1);
    assert(scanner_set_selected_keyboard(0)==0);assert(ble_get_pending_update(&out));assert(history_resets==2&&out.wpm==42);
    requested_slot=1;assert(ble_get_pending_update(&out));assert(history_resets==3&&out.wpm==80);
    hand_slots[0]=1;hand_slots[1]=0;test_now+=1000;packet(&data,9);
    assert(ble_get_pending_update(&out)&&out.bat[0]==79&&out.bat[1]==83);
    /* Timeout invalidates all stale state rather than retaining old WPM. */
    test_now+=CONFIG_PROSPECTOR_SCANNER_TIMEOUT_MS+1500;for(int i=0;i<10;i++)scanner_process_incoming();assert(scanner_get_active_keyboard_count()==0);assert(ble_get_pending_update(&out)&&out.no_keyboards);assert(empty_views==1&&history_resets==4);assert(!ble_get_pending_update(&out));
    puts("receiver: signature/channel/malformed AD/two senders/selection/timeout/source-reset/DYA-selection/hand-swap passed");
}
