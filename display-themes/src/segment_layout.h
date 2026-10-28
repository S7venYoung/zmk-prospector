/* SPDX-License-Identifier: MIT */
#pragma once
#include "prospector_layouts.h"
#define PROSPECTOR_LAYOUT_SEGMENT ((prospector_layout_t)6)
lv_obj_t *segment_layout_create(lv_obj_t *parent);
void segment_layout_update(const struct prospector_keyboard_data *data, const char *name);
void segment_layout_destroy(void);
