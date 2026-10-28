/* Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 * Industrial robot-eye dashboard, adapted from the supplied portrait reference
 * to the ms2 280x240 landscape panel. All drawing is native LVGL; no full-screen
 * raster or transform buffer is allocated.
 */
#include "industrial_layout.h"
#include "fonts_carrefinho.h"
#include <draw/lv_draw_triangle.h>
#include <stdio.h>
#include <string.h>

LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(industrial_display_56);
LV_FONT_DECLARE(industrial_brand_28);
LV_FONT_DECLARE(industrial_numeric_32);
LV_FONT_DECLARE(industrial_battery_32);

#define INK 0x0D1310
#define GOLD 0xFFCA13
#define PAPER 0xF3EFE3
#define GREEN 0x43DF44
#define AMBER 0xFFAE22
#define MUTED 0x777C76
#define PANEL 0x1B2420

static lv_obj_t *root, *layer, *wpm, *wpm_unit, *output, *battery[2], *segments[2][8];
static lv_obj_t *modifier_keys[4], *modifier_symbols[4];
static char layer_text[32], wpm_text[4], output_text[4], battery_text[2][6];

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *box(lv_obj_t *parent, int x, int y, int w, int h,
                     uint32_t color, int radius) {
    lv_obj_t *o = lv_obj_create(parent); plain(o, color);
    lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_set_style_radius(o, radius, 0);
    return o;
}
static lv_obj_t *text(lv_obj_t *parent, int x, int y, int w,
                      const lv_font_t *font, uint32_t color, const char *value) {
    lv_obj_t *o = lv_label_create(parent); lv_label_set_text(o, "");
    lv_obj_set_style_text_font(o, font, 0);
    lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_text_align(o, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_size(o, w, font->line_height); lv_obj_set_pos(o, x, y);
    lv_label_set_long_mode(o, LV_LABEL_LONG_DOT);
    lv_label_set_text(o, value);
    return o;
}
static void eye(lv_obj_t *parent, int x) {
    lv_obj_t *rim = box(parent, x, 15, 30, 30, INK, 7);
    lv_obj_t *iris = box(rim, 4, 4, 22, 22, GOLD, LV_RADIUS_CIRCLE);
    lv_obj_t *pupil = box(iris, 5, 5, 14, 14, INK, LV_RADIUS_CIRCLE);
    box(pupil, 8, 1, 5, 5, GOLD, LV_RADIUS_CIRCLE);
}
static void quad(lv_layer_t *draw, int x, int y, int width, int height,
                  int skew, uint32_t color) {
    lv_draw_triangle_dsc_t triangle; lv_draw_triangle_dsc_init(&triangle);
    triangle.bg_color = lv_color_hex(color);
    int lean = skew >= 0 ? skew : -skew;
    lv_point_precise_t top_left = {x + (skew >= 0 ? lean : 0), y};
    lv_point_precise_t top_right = {x + (skew >= 0 ? width : width - lean), y};
    lv_point_precise_t bottom_right = {x + (skew >= 0 ? width - lean : width), y + height};
    lv_point_precise_t bottom_left = {x + (skew >= 0 ? 0 : lean), y + height};
    triangle.p[0] = top_left; triangle.p[1] = top_right; triangle.p[2] = bottom_right;
    lv_draw_triangle(draw, &triangle);
    triangle.p[1] = bottom_right; triangle.p[2] = bottom_left;
    lv_draw_triangle(draw, &triangle);
}
static void artwork(lv_event_t *event) {
    lv_layer_t *draw = lv_event_get_layer(event);
    lv_area_t area; lv_obj_get_coords(lv_event_get_target(event), &area);
    int x = area.x1, y = area.y1;
    for (int i = 0; i < 10; i++) quad(draw, x + i * 30 - 8, y + 47, 23, 8, 8, INK);
    for (int i = 0; i < 3; i++) {
        quad(draw, x + 16 + i * 19, y + 128, 16, 10, 5, GOLD);
        quad(draw, x + 209 + i * 19, y + 128, 16, 10, -5, GOLD);
    }
}
static void battery_card(int hand, int x) {
    lv_obj_t *card = box(root, x, 151, 120, 42, PAPER, 9);
    lv_obj_t *track = box(card, 7, 9, 60, 24, INK, 3);
    box(card, 67, 16, 3, 10, INK, 1);
    for (int i = 0; i < 8; i++)
        segments[hand][i] = box(track, 3 + i * 7, 3, 6, 18, 0xB1B4AC, 0);
    battery[hand] = text(card, 72, 4, 44, &industrial_battery_32, INK, "--");
}
lv_obj_t *industrial_layout_create(lv_obj_t *parent) {
    root = box(parent, 0, 0, 280, 240, INK, 0);
    box(root, 0, 0, 280, 55, GOLD, 0);
    eye(root, 20); eye(root, 56);
    text(root, 92, 16, 112, &industrial_brand_28, INK, "CORNE");
    lv_obj_t *badge = box(root, 210, 15, 54, 30, GOLD, 6);
    lv_obj_set_style_border_width(badge, 3, 0);
    lv_obj_set_style_border_color(badge, lv_color_hex(INK), 0);
    output = text(badge, 0, 0, 42, &impact_20, INK, "OFF");
    lv_obj_center(output);
    /* Transparent drawing layer sits behind all foreground text. */
    lv_obj_t *decoration = box(root, 0, 0, 280, 240, INK, 0);
    lv_obj_set_style_bg_opa(decoration, LV_OPA_TRANSP, 0);
    lv_obj_add_event_cb(decoration, artwork, LV_EVENT_DRAW_MAIN, NULL);
    layer = text(root, 18, 56, 244, &industrial_display_56, PAPER, "BASE");
    wpm = text(root, 88, 116, 64, &industrial_numeric_32, GOLD, "0");
    lv_obj_set_style_text_align(wpm, LV_TEXT_ALIGN_RIGHT, 0);
    wpm_unit = text(root, 146, 123, 45, &impact_20, PAPER, "WPM");
    lv_obj_set_style_text_align(wpm_unit, LV_TEXT_ALIGN_LEFT, 0);
    battery_card(0, 16); battery_card(1, 144);
    box(root, 138, 154, 3, 36, 0x354039, 1);
    lv_obj_t *footer = box(root, 18, 197, 244, 37, PANEL, 10);
    lv_obj_set_style_border_color(footer, lv_color_hex(0x3C4640), 0);
    lv_obj_set_style_border_width(footer, 2, 0);
    static const char *symbols[] = {SYMBOL_COMMAND, SYMBOL_OPTION, SYMBOL_CONTROL, SYMBOL_SHIFT};
    for (int i = 0; i < 4; i++) {
        modifier_keys[i] = box(footer, 12 + i * 56, 0, 48, 33, PANEL, 5);
        modifier_symbols[i] = text(modifier_keys[i], 0, 0, 48,
            &Symbols_Semibold_32, MUTED, symbols[i]);
    }
    layer_text[0] = '\0';
    struct prospector_keyboard_data empty = {0};
    industrial_layout_update(&empty, "BASE");
    return root;
}
void industrial_layout_update(const struct prospector_keyboard_data *data, const char *name) {
    if (!root || !data) return;
    if (!name || !*name) name = "BASE";
    char upper[32]; size_t n = 0;
    for (; name[n] && n < sizeof(upper) - 1; n++)
        upper[n] = name[n] >= 'a' && name[n] <= 'z' ? name[n] - 'a' + 'A' : name[n];
    upper[n] = '\0';
    /* Keep stock verbose layer labels readable in the display font. */
    if (!strcmp(upper, "DEFAULT LAYER") || !strcmp(upper, "BASE LAYER"))
        snprintf(upper, sizeof(upper), "BASE");
    else if (!strcmp(upper, "LOWER LAYER"))
        snprintf(upper, sizeof(upper), "LOWER");
    else if (!strcmp(upper, "RAISE LAYER"))
        snprintf(upper, sizeof(upper), "RAISE");
    n = strlen(upper);
    if (strcmp(layer_text, upper)) {
        memcpy(layer_text, upper, n + 1);
        const lv_font_t *font = &industrial_display_56; lv_point_t size;
        lv_text_get_size(&size, layer_text, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
        if (size.x > 244) {
            font = &FR_Regular_36;
            lv_text_get_size(&size, layer_text, font, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
            if (size.x > 244) font = &impact_20;
        }
        lv_obj_set_style_text_font(layer, font, 0);
        lv_obj_set_height(layer, font->line_height);
        lv_obj_set_y(layer, 56 + (58 - font->line_height) / 2);
        lv_label_set_text_static(layer, layer_text);
    }
    snprintf(wpm_text, sizeof(wpm_text), "%u", data->wpm_value);
    lv_label_set_text_static(wpm, wpm_text);
    lv_point_t number_size, unit_size;
    lv_text_get_size(&number_size, wpm_text, &industrial_numeric_32, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    lv_text_get_size(&unit_size, "WPM", &impact_20, 0, 0, LV_COORD_MAX, LV_TEXT_FLAG_NONE);
    int start = (280 - number_size.x - 8 - unit_size.x) / 2;
    lv_obj_set_width(wpm, number_size.x); lv_obj_set_x(wpm, start);
    lv_obj_set_width(wpm_unit, unit_size.x); lv_obj_set_x(wpm_unit, start + number_size.x + 8);
    snprintf(output_text, sizeof(output_text), "%s", data->usb_connected ? "USB" : data->ble_connected ? "BLE" : "OFF");
    lv_label_set_text_static(output, output_text);
    uint8_t levels[2] = {data->battery_level, data->peripheral_battery[0]};
    for (int hand = 0; hand < 2; hand++) {
        bool valid = data->has_dynamic_data && levels[hand] > 0 && levels[hand] <= 100;
        if (valid) snprintf(battery_text[hand], sizeof(battery_text[hand]), "%u", levels[hand]);
        else snprintf(battery_text[hand], sizeof(battery_text[hand]), "--");
        lv_label_set_text_static(battery[hand], battery_text[hand]);
        uint32_t color = valid ? (levels[hand] < 20 ? AMBER : GREEN) : MUTED;
        for (int i = 0; i < 8; i++)
            lv_obj_set_style_bg_color(segments[hand][i],
                lv_color_hex(valid && levels[hand] > i * 100 / 8 ? color : 0xB1B4AC), 0);
    }
    static const uint8_t masks[] = {0x88, 0x44, 0x11, 0x22};
    for (int i = 0; i < 4; i++) {
        bool active = (data->modifier_flags & masks[i]) != 0;
        lv_obj_set_style_bg_color(modifier_keys[i], lv_color_hex(active ? GOLD : PANEL), 0);
        lv_obj_set_style_text_color(modifier_symbols[i], lv_color_hex(active ? INK : MUTED), 0);
    }
}
void industrial_layout_destroy(void) {
    if (root) lv_obj_delete(root);
    root = layer = wpm = wpm_unit = output = NULL;
    for (int i = 0; i < 4; i++) modifier_keys[i] = modifier_symbols[i] = NULL;
    for (int i = 0; i < 2; i++) {
        battery[i] = NULL;
        for (int j = 0; j < 8; j++) segments[i][j] = NULL;
    }
}
