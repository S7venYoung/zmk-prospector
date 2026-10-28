/* SPDX-License-Identifier: MIT */
#pragma once
#include "prospector_layouts.h"
/* Local extension; existing layout IDs and DYA values remain unchanged. */
#define PROSPECTOR_LAYOUT_INDUSTRIAL ((prospector_layout_t)5)
lv_obj_t *industrial_layout_create(lv_obj_t *parent);
void industrial_layout_update(const struct prospector_keyboard_data *data, const char *layer_name);
void industrial_layout_destroy(void);
