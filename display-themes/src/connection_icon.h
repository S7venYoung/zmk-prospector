/* SPDX-License-Identifier: MIT */
#pragma once
#include <lvgl.h>
lv_obj_t *connection_icon_create(lv_obj_t *parent,int x,int y,uint32_t color);
void connection_icon_update(lv_obj_t *icon,bool usb,bool ble);
