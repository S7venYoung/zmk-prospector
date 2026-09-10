/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "custom_status_screen.h"
#include "widgets/battery_status.h"
#include "widgets/modifiers.h"
#include "widgets/bongo_cat.h"
#include "widgets/layer_status.h"
#include "widgets/output_status.h"
#include "widgets/hid_indicators.h"
#include "widgets/wpm_status.h"
#include "widgets/key_stats_status.h"

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
#include <zmk/display_settings.h>
#endif

#include <zephyr/logging/log.h>
#include <zmk/display.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

static struct zmk_widget_output_status output_status_widget;
static struct zmk_widget_dongle_battery_status dongle_battery_status_widget;

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
static struct zmk_widget_layer_status layer_status_widget;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
static struct zmk_widget_modifiers modifiers_widget;
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
static struct zmk_widget_hid_indicators hid_indicators_widget;
#endif

#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
static struct zmk_widget_bongo_cat bongo_cat_widget;
#endif

static struct zmk_widget_wpm_status wpm_status_widget;

#if IS_ENABLED(CONFIG_ZMK_KEY_STATS)
static struct zmk_widget_key_stats_status key_stats_status_widget;
#endif

lv_style_t global_style;
static bool status_screen_ready;

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
static void set_widget_visible(lv_obj_t *obj, bool visible) {
    if (visible) {
        lv_obj_clear_flag(obj, LV_OBJ_FLAG_HIDDEN);
    } else {
        lv_obj_add_flag(obj, LV_OBJ_FLAG_HIDDEN);
    }
}

static void apply_runtime_display_settings(struct k_work *work) {
    ARG_UNUSED(work);

    if (!status_screen_ready) {
        return;
    }

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
    set_widget_visible(zmk_widget_wpm_status_obj(&wpm_status_widget),
                       zmk_display_settings_wpm_enabled());
    zmk_widget_wpm_status_refresh(&wpm_status_widget);
#endif

#if IS_ENABLED(CONFIG_ZMK_KEY_STATS)
    set_widget_visible(zmk_widget_key_stats_status_obj(&key_stats_status_widget),
                       zmk_display_settings_key_stats_enabled());
    lv_obj_align(zmk_widget_key_stats_status_obj(&key_stats_status_widget), LV_ALIGN_TOP_LEFT,
                 MAX(0, zmk_display_settings_key_stats_x() - 4),
                 zmk_display_settings_key_stats_y());
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    bool bongo_cat_enabled = zmk_display_settings_bongo_cat_enabled();
    set_widget_visible(zmk_widget_bongo_cat_obj(&bongo_cat_widget), bongo_cat_enabled);
#else
    bool bongo_cat_enabled = false;
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
    set_widget_visible(zmk_widget_modifiers_obj(&modifiers_widget),
                       zmk_display_settings_modifiers_enabled());
    zmk_widget_modifiers_refresh(&modifiers_widget);
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
    set_widget_visible(zmk_widget_hid_indicators_obj(&hid_indicators_widget),
                       zmk_display_settings_modifiers_enabled());
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
    set_widget_visible(zmk_widget_layer_status_obj(&layer_status_widget),
                       zmk_display_settings_layer_enabled());
    zmk_widget_layer_status_refresh(&layer_status_widget);
#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    if (bongo_cat_enabled) {
        lv_obj_align_to(zmk_widget_layer_status_obj(&layer_status_widget),
                        zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 5);
    } else {
        lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0,
                     -3);
    }
#else
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, -3);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_BATTERY)
    zmk_widget_dongle_battery_status_refresh(&dongle_battery_status_widget);
#endif
}

K_WORK_DEFINE(runtime_display_settings_work, apply_runtime_display_settings);

void zmk_display_settings_runtime_changed(void) {
    if (!zmk_display_is_initialized()) {
        return;
    }
    k_work_submit_to_queue(zmk_display_work_q(), &runtime_display_settings_work);
}
#endif

lv_obj_t *zmk_display_status_screen() {
    lv_obj_t *screen;

    screen = lv_obj_create(NULL);
    lv_style_init(&global_style);
    lv_style_set_bg_color(&global_style, lv_color_white());
    lv_style_set_bg_opa(&global_style, LV_OPA_COVER);
    lv_style_set_text_color(&global_style, lv_color_black());
    lv_style_set_text_font(&global_style, &lv_font_unscii_8);
    lv_style_set_text_letter_space(&global_style, 1);
    lv_style_set_text_line_space(&global_style, 1);
    lv_obj_add_style(screen, &global_style, LV_PART_MAIN);
    
    zmk_widget_output_status_init(&output_status_widget, screen);
    lv_obj_align(zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_TOP_LEFT, 0, 0);

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_WPM)
    zmk_widget_wpm_status_init(&wpm_status_widget, screen);
    lv_obj_align_to(zmk_widget_wpm_status_obj(&wpm_status_widget),
                    zmk_widget_output_status_obj(&output_status_widget), LV_ALIGN_OUT_RIGHT_MID, 7,
                    0);
#endif

#if IS_ENABLED(CONFIG_ZMK_KEY_STATS)
    zmk_widget_key_stats_status_init(&key_stats_status_widget, screen);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
    lv_obj_align(zmk_widget_key_stats_status_obj(&key_stats_status_widget), LV_ALIGN_TOP_LEFT,
                 MAX(0, zmk_display_settings_key_stats_x() - 4),
                 zmk_display_settings_key_stats_y());
#else
    lv_obj_align(zmk_widget_key_stats_status_obj(&key_stats_status_widget), LV_ALIGN_TOP_LEFT, 42, 0);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    zmk_widget_bongo_cat_init(&bongo_cat_widget, screen);
    lv_obj_align(zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, -7);
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MODIFIERS)
    zmk_widget_modifiers_init(&modifiers_widget, screen);
    lv_obj_align(zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_BOTTOM_LEFT, 0, 0);
#if IS_ENABLED(CONFIG_ZMK_HID_INDICATORS)
    zmk_widget_hid_indicators_init(&hid_indicators_widget, screen);
    lv_obj_align_to(zmk_widget_hid_indicators_obj(&hid_indicators_widget),
                    zmk_widget_modifiers_obj(&modifiers_widget), LV_ALIGN_OUT_TOP_LEFT, 0, -2);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_LAYER)
    zmk_widget_layer_status_init(&layer_status_widget, screen);
#if IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_BONGO_CAT)
    if (
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
        zmk_display_settings_bongo_cat_enabled()
#else
        true
#endif
    ) {
        lv_obj_align_to(zmk_widget_layer_status_obj(&layer_status_widget),
                        zmk_widget_bongo_cat_obj(&bongo_cat_widget), LV_ALIGN_BOTTOM_RIGHT, 0, 5);
    } else {
        lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0,
                     -3);
    }
#else
    lv_obj_align(zmk_widget_layer_status_obj(&layer_status_widget), LV_ALIGN_BOTTOM_RIGHT, 0, -3);
#endif
#endif

#if IS_ENABLED(CONFIG_ZMK_BATTERY)
    zmk_widget_dongle_battery_status_init(&dongle_battery_status_widget, screen);
    lv_obj_align(zmk_widget_dongle_battery_status_obj(&dongle_battery_status_widget), LV_ALIGN_TOP_RIGHT, 0, 0);
#endif

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
    status_screen_ready = true;
    apply_runtime_display_settings(NULL);
#endif

    return screen;
}
