/* SPDX-License-Identifier: MIT */
#pragma once
#include "prospector_layouts.h"
lv_obj_t *yads_layout_create(lv_obj_t *parent);
void yads_layout_destroy(void);
void yads_layout_update(const struct prospector_keyboard_data *data);
