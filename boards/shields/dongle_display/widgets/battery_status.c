/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/battery.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/usb_conn_state_changed.h>
#include <zmk/event_manager.h>
#include <zmk/usb.h>

#include "battery_status.h"

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
#include <zmk/display_settings.h>
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY)
    #define SOURCE_OFFSET 1
#else
    #define SOURCE_OFFSET 0
#endif

#ifndef ZMK_SPLIT_BLE_PERIPHERAL_COUNT
#  define ZMK_SPLIT_BLE_PERIPHERAL_COUNT 0
#endif

#define BUFFER_SIZE LV_CANVAS_BUF_SIZE(5, 8, LV_COLOR_FORMAT_GET_BPP(LV_COLOR_FORMAT_L8), LV_DRAW_BUF_STRIDE_ALIGN)

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

struct battery_state {
    uint8_t source;
    uint8_t level;
    bool usb_present;
    bool central;
};

struct battery_object {
    lv_obj_t *symbol;
    lv_obj_t *label;
} battery_objects[ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET];

static struct battery_state central_state;
static bool central_state_valid;
static struct battery_state peripheral_states[MAX(1, ZMK_SPLIT_BLE_PERIPHERAL_COUNT)];
static bool peripheral_state_valid[MAX(1, ZMK_SPLIT_BLE_PERIPHERAL_COUNT)];
    
static lv_color_t battery_image_buffer[ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET][BUFFER_SIZE];

static void draw_battery(lv_obj_t *canvas, uint8_t level, bool usb_present) {
    lv_canvas_fill_bg(canvas, lv_color_black(), LV_OPA_COVER);
    
    lv_layer_t layer;
    lv_canvas_init_layer(canvas, &layer);

    lv_draw_rect_dsc_t rect_fill_dsc;
    lv_draw_rect_dsc_init(&rect_fill_dsc);
    rect_fill_dsc.bg_color = lv_color_white();

    if (usb_present) {
        rect_fill_dsc.bg_opa = LV_OPA_TRANSP;
        rect_fill_dsc.border_color = lv_color_white();
        rect_fill_dsc.border_width = 1;
    }

    lv_canvas_set_px(canvas, 0, 0, lv_color_white(), LV_OPA_COVER);
    lv_canvas_set_px(canvas, 4, 0, lv_color_white(), LV_OPA_COVER);

    lv_area_t rect_coords;
    bool rect_draw = true;
    
    if (level <= 10 || usb_present) {
        rect_coords = (lv_area_t){1, 2, 3, 6};
    } else if (level <= 30) {
        rect_coords = (lv_area_t){1, 2, 3, 5};
    } else if (level <= 50) {
        rect_coords = (lv_area_t){1, 2, 3, 4};
    } else if (level <= 70) {
        rect_coords = (lv_area_t){1, 2, 3, 3};
    } else if (level <= 90) {
        rect_coords = (lv_area_t){1, 2, 3, 2};
    } else {
        rect_draw = false;
    }

    if (rect_draw) {
        lv_draw_rect(&layer, &rect_fill_dsc, &rect_coords);
    }

    lv_canvas_finish_layer(canvas, &layer);
}

static void set_battery_symbol(uint8_t object_index, struct battery_state state) {
    if (object_index >= ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET) {
        return;
    }
    LOG_DBG("source: %d, level: %d, usb: %d", object_index, state.level, state.usb_present);
    lv_obj_t *symbol = battery_objects[object_index].symbol;
    lv_obj_t *label = battery_objects[object_index].label;

    draw_battery(symbol, state.level, state.usb_present);
    lv_label_set_text_fmt(label, "%4u%% ", state.level);
    
    if (state.level > 0 || state.usb_present) {
        lv_obj_clear_flag(symbol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(symbol);
        lv_obj_clear_flag(label, LV_OBJ_FLAG_HIDDEN);
        lv_obj_move_foreground(label);
    } else {
        lv_obj_add_flag(symbol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(label, LV_OBJ_FLAG_HIDDEN);
    }
}

void zmk_widget_dongle_battery_status_refresh(struct zmk_widget_dongle_battery_status *widget) {
    ARG_UNUSED(widget);

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET; i++) {
        lv_obj_add_flag(battery_objects[i].symbol, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_objects[i].label, LV_OBJ_FLAG_HIDDEN);
    }

    bool show_central = IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
    show_central = show_central && zmk_display_settings_dongle_battery_enabled();
#endif

    uint8_t offset = show_central ? 1U : 0U;
    if (show_central && central_state_valid) {
        set_battery_symbol(0, central_state);
    }

    for (uint8_t i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT; i++) {
        if (peripheral_state_valid[i]) {
            set_battery_symbol(i + offset, peripheral_states[i]);
        }
    }
}

void battery_status_update_cb(struct battery_state state) {
    if (state.central) {
        central_state = state;
        central_state_valid = true;
    } else if (state.source < ZMK_SPLIT_BLE_PERIPHERAL_COUNT) {
        peripheral_states[state.source] = state;
        peripheral_state_valid[state.source] = true;
    }

    struct zmk_widget_dongle_battery_status *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) {
        zmk_widget_dongle_battery_status_refresh(widget);
    }
}

static struct battery_state peripheral_battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *ev = as_zmk_peripheral_battery_state_changed(eh);
    return (struct battery_state){
        .source = ev->source,
        .level = ev->state_of_charge,
        .central = false,
    };
}

static struct battery_state central_battery_status_get_state(const zmk_event_t *eh) {
    const struct zmk_battery_state_changed *ev = as_zmk_battery_state_changed(eh);
    return (struct battery_state) {
        .source = 0,
        .level = (ev != NULL) ? ev->state_of_charge : zmk_battery_state_of_charge(),
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
        .usb_present = zmk_usb_is_powered(),
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
        .central = true,
    };
}

static struct battery_state battery_status_get_state(const zmk_event_t *eh) { 
    if (as_zmk_peripheral_battery_state_changed(eh) != NULL) {
        return peripheral_battery_status_get_state(eh);
    } else {
        return central_battery_status_get_state(eh);
    }
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_dongle_battery_status, struct battery_state,
                            battery_status_update_cb, battery_status_get_state)

ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_peripheral_battery_state_changed);

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY)
#if !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL)

ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_battery_state_changed);
#if IS_ENABLED(CONFIG_USB_DEVICE_STACK)
ZMK_SUBSCRIPTION(widget_dongle_battery_status, zmk_usb_conn_state_changed);
#endif /* IS_ENABLED(CONFIG_USB_DEVICE_STACK) */
#endif /* !IS_ENABLED(CONFIG_ZMK_SPLIT) || IS_ENABLED(CONFIG_ZMK_SPLIT_ROLE_CENTRAL) */
#endif /* IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_DONGLE_BATTERY) */

int zmk_widget_dongle_battery_status_init(struct zmk_widget_dongle_battery_status *widget, lv_obj_t *parent) {
    widget->obj = lv_obj_create(parent);

    lv_obj_set_size(widget->obj, LV_SIZE_CONTENT, LV_SIZE_CONTENT);

    for (int i = 0; i < ZMK_SPLIT_BLE_PERIPHERAL_COUNT + SOURCE_OFFSET; i++) {
        lv_obj_t *image_canvas = lv_canvas_create(widget->obj);
        lv_obj_t *battery_label = lv_label_create(widget->obj);

        lv_canvas_set_buffer(image_canvas, battery_image_buffer[i], 5, 8, LV_COLOR_FORMAT_L8);

        lv_obj_align(image_canvas, LV_ALIGN_TOP_RIGHT, 0, i * 10);
        lv_obj_align_to(battery_label, image_canvas, LV_ALIGN_OUT_LEFT_MID, 0, 0);

        lv_obj_add_flag(image_canvas, LV_OBJ_FLAG_HIDDEN);
        lv_obj_add_flag(battery_label, LV_OBJ_FLAG_HIDDEN);
        
        battery_objects[i] = (struct battery_object){
            .symbol = image_canvas,
            .label = battery_label,
        };
    }

    sys_slist_append(&widgets, &widget->node);

    widget_dongle_battery_status_init();

    return 0;
}

lv_obj_t *zmk_widget_dongle_battery_status_obj(struct zmk_widget_dongle_battery_status *widget) {
    return widget->obj;
}
