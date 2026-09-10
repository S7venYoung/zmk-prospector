/*
 * Copyright (c) 2026 S7venYoung
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <stdio.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/key_stats_changed.h>
#include <zmk/key_stats.h>

#include "key_stats_status.h"

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct key_stats_status_state {
    uint32_t total;
    uint32_t today;
};

static struct key_stats_status_state get_state(const zmk_event_t *_eh) {
    const struct zmk_key_stats_changed *ev = as_zmk_key_stats_changed(_eh);

    if (ev != NULL) {
        return (struct key_stats_status_state) {
            .total = ev->total,
            .today = ev->today,
        };
    }

    return (struct key_stats_status_state) {
        .total = zmk_key_stats_total(),
        .today = zmk_key_stats_today(),
    };
}

static void format_count(char *buf, size_t len, uint32_t value) {
    if (value < 1000U) {
        snprintf(buf, len, "%u", value);
    } else if (value < 10000U) {
        snprintf(buf, len, "%u.%uk", value / 1000U, (value % 1000U) / 100U);
    } else if (value < 1000000U) {
        snprintf(buf, len, "%uk", value / 1000U);
    } else if (value < 10000000U) {
        snprintf(buf, len, "%u.%um", value / 1000000U, (value % 1000000U) / 100000U);
    } else if (value < 1000000000U) {
        snprintf(buf, len, "%um", value / 1000000U);
    } else {
        snprintf(buf, len, "%u.%ub", value / 1000000000U, (value % 1000000000U) / 100000000U);
    }
}

static void set_key_stats(struct zmk_widget_key_stats_status *widget,
                          struct key_stats_status_state state) {
    char count[8];

    format_count(count, sizeof(count), state.total);
    lv_label_set_text(widget->total_value_label, count);

    format_count(count, sizeof(count), state.today);
    lv_label_set_text(widget->today_value_label, count);
}

static void key_stats_status_update_cb(struct key_stats_status_state state) {
    struct zmk_widget_key_stats_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        set_key_stats(widget, state);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_key_stats_status, struct key_stats_status_state,
                            key_stats_status_update_cb, get_state)
ZMK_SUBSCRIPTION(widget_key_stats_status, zmk_key_stats_changed);

int zmk_widget_key_stats_status_init(struct zmk_widget_key_stats_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);
    lv_obj_set_size(widget->obj, 50, 18);

    widget->total_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->total_label, "T");
    lv_obj_align(widget->total_label, LV_ALIGN_TOP_LEFT, 0, 0);

    widget->total_value_label = lv_label_create(widget->obj);
    lv_obj_set_width(widget->total_value_label, 38);
    lv_obj_set_style_text_align(widget->total_value_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(widget->total_value_label, LV_ALIGN_TOP_RIGHT, 0, 0);

    widget->today_label = lv_label_create(widget->obj);
    lv_label_set_text(widget->today_label, "D");
    lv_obj_align(widget->today_label, LV_ALIGN_TOP_LEFT, 0, 9);

    widget->today_value_label = lv_label_create(widget->obj);
    lv_obj_set_width(widget->today_value_label, 38);
    lv_obj_set_style_text_align(widget->today_value_label, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_align(widget->today_value_label, LV_ALIGN_TOP_RIGHT, 0, 9);

    sys_slist_append(&widgets, &widget->node);

    widget_key_stats_status_init();
    return 0;
}

lv_obj_t *zmk_widget_key_stats_status_obj(struct zmk_widget_key_stats_status *widget) {
    return widget->obj;
}
