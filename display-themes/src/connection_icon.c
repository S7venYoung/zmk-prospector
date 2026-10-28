/* SPDX-License-Identifier: MIT */
#include "connection_icon.h"
#include <string.h>
LV_FONT_DECLARE(connection_icons_20);
lv_obj_t *connection_icon_create(lv_obj_t *parent,int x,int y,uint32_t color) {
    lv_obj_t *o=lv_label_create(parent);lv_obj_remove_style_all(o);lv_obj_set_pos(o,x,y);lv_obj_set_size(o,18,20);
    lv_obj_set_style_text_font(o,&connection_icons_20,0);lv_obj_set_style_text_color(o,lv_color_hex(color),0);
    lv_label_set_text_static(o,LV_SYMBOL_CLOSE);return o;
}
void connection_icon_update(lv_obj_t *icon,bool usb,bool ble) {
    const char *glyph=usb?LV_SYMBOL_USB:ble?LV_SYMBOL_BLUETOOTH:LV_SYMBOL_CLOSE;
    if(strcmp(lv_label_get_text(icon),glyph))lv_label_set_text_static(icon,glyph);
}
