/*
 * Copyright (c) 2026 S7venYoung
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <lvgl.h>
#include <zephyr/kernel.h>

struct zmk_widget_key_stats_status {
    sys_snode_t node;
    lv_obj_t *obj;
    lv_obj_t *total_label;
    lv_obj_t *total_value_label;
    lv_obj_t *today_label;
    lv_obj_t *today_value_label;
};

int zmk_widget_key_stats_status_init(struct zmk_widget_key_stats_status *widget, lv_obj_t *parent);
lv_obj_t *zmk_widget_key_stats_status_obj(struct zmk_widget_key_stats_status *widget);
