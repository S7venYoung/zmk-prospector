/* SPDX-License-Identifier: MIT */
#pragma once
#include "prospector_layouts.h"
#define PROSPECTOR_LAYOUT_TAPE ((prospector_layout_t)10)
#define PROSPECTOR_LAYOUT_BLUEPRINT ((prospector_layout_t)11)
#define PROSPECTOR_LAYOUT_TERMINAL ((prospector_layout_t)12)
lv_obj_t *concept_layout_create(lv_obj_t *parent, prospector_layout_t style);
void concept_layout_update(const struct prospector_keyboard_data *data, const char *name);
void concept_layout_destroy(void);
void concept_layout_reset_history(void);
