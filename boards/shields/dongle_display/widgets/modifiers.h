/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */
 
#pragma once

#include <stdbool.h>
#include <lvgl.h>
#include <zephyr/kernel.h>

#define SIZE_SYMBOLS 14 // 14 x 14 pixel
#define ZMK_MODIFIER_SYMBOL_COUNT 4

struct zmk_widget_modifiers {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *symbols[ZMK_MODIFIER_SYMBOL_COUNT];
    lv_obj_t *selection_lines[ZMK_MODIFIER_SYMBOL_COUNT];
    bool is_active[ZMK_MODIFIER_SYMBOL_COUNT];
};

int zmk_widget_modifiers_init(struct zmk_widget_modifiers *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_modifiers_obj(struct zmk_widget_modifiers *widget);
void zmk_widget_modifiers_refresh(struct zmk_widget_modifiers *widget);
