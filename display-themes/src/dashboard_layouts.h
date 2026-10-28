/* SPDX-License-Identifier: MIT */
#pragma once
#include "prospector_layouts.h"
#define PROSPECTOR_LAYOUT_ORBIT ((prospector_layout_t)7)
#define PROSPECTOR_LAYOUT_SPLIT ((prospector_layout_t)8)
#define PROSPECTOR_LAYOUT_MONO ((prospector_layout_t)9)
lv_obj_t *dashboard_layout_create(lv_obj_t *parent, prospector_layout_t style);
void dashboard_layout_update(const struct prospector_keyboard_data *data,const char *name);
void dashboard_layout_destroy(void);
