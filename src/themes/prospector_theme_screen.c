/*
 * Fixed WALL-E / Codex screen for the Prospector Dongle.
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>

#include "widgets/battery_bar.h"
#include "widgets/layer_roller.h"
#include "widgets/modifier_indicator.h"

static struct zmk_widget_layer_roller layer_widget;
static struct zmk_widget_battery_bar battery_widget;
static struct zmk_widget_modifier_indicator modifier_widget;

static void set_panel_style(lv_obj_t *obj, lv_color_t color) {
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
}

lv_obj_t *zmk_display_status_screen(void) {
    const lv_color_t charcoal = lv_color_hex(0x101411);
    const lv_color_t yellow = lv_color_hex(0xFFBF18);

    lv_obj_t *screen = lv_obj_create(NULL);
    set_panel_style(screen, charcoal);

    lv_obj_t *header = lv_obj_create(screen);
    lv_obj_set_size(header, 240, 42);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    set_panel_style(header, yellow);

    lv_obj_t *eyes = lv_label_create(header);
    lv_label_set_text(eyes, "[o][o]");
    lv_obj_set_style_text_color(eyes, charcoal, 0);
    lv_obj_align(eyes, LV_ALIGN_LEFT_MID, 8, 0);

    lv_obj_t *title = lv_label_create(header);
    lv_label_set_text(title, "SOFLE // CODEX");
    lv_obj_set_style_text_color(title, charcoal, 0);
    lv_obj_align(title, LV_ALIGN_RIGHT_MID, -8, 0);

    lv_obj_t *stripe = lv_obj_create(screen);
    lv_obj_set_size(stripe, 240, 6);
    lv_obj_align(stripe, LV_ALIGN_TOP_MID, 0, 42);
    set_panel_style(stripe, yellow);

    zmk_widget_layer_roller_init(&layer_widget, screen);
    lv_obj_set_size(zmk_widget_layer_roller_obj(&layer_widget), 224, 112);
    lv_obj_align(zmk_widget_layer_roller_obj(&layer_widget), LV_ALIGN_TOP_MID, 0, 50);

    zmk_widget_modifier_indicator_init(&modifier_widget, screen);
    lv_obj_align(zmk_widget_modifier_indicator_obj(&modifier_widget), LV_ALIGN_BOTTOM_RIGHT, -8,
                 -54);

    zmk_widget_battery_bar_init(&battery_widget, screen);
    lv_obj_set_size(zmk_widget_battery_bar_obj(&battery_widget), 240, 48);
    lv_obj_align(zmk_widget_battery_bar_obj(&battery_widget), LV_ALIGN_BOTTOM_MID, 0, 0);

    return screen;
}
