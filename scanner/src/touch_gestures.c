/* SPDX-License-Identifier: MIT
 * CST816 coordinate gestures. Input thread only queues; LVGL timer applies.
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/atomic.h>
#include "prospector_layouts.h"
#include <lvgl.h>
#include "dya_settings.h"
LOG_MODULE_REGISTER(scanner_touch, CONFIG_ZMK_LOG_LEVEL);
#define TOUCH_NODE DT_NODELABEL(touch_sensor)
#define UI_BG_COLOR 0x090B0F
#define UI_PANEL_COLOR 0x171C24
#define UI_PANEL_BORDER 0x2B3440
#define UI_ACCENT_COLOR 0x4A90E2
#define UI_TEXT_MUTED 0xA7B0BC
#define UI_MARGIN 24
struct gesture { int theme, brightness; };
K_MSGQ_DEFINE(gestures, sizeof(struct gesture), 8, 4);
static int x, y, start_x, start_y;
static bool pressed;
static int64_t started, last_swipe;
static lv_timer_t *timer;
static atomic_t pointer_x, pointer_y, pointer_down;
static lv_obj_t *menu, *slider, *brightness_value;
static int page; /* 0: themes, 1: main/navigation, 2: display settings */
static bool dragging;
static int64_t last_tap;

static int32_t display_width(void) {
    lv_display_t *display = lv_display_get_default();
    return display ? lv_display_get_horizontal_resolution(display) : 280;
}

static int32_t display_height(void) {
    lv_display_t *display = lv_display_get_default();
    return display ? lv_display_get_vertical_resolution(display) : 240;
}

static lv_obj_t *create_label(lv_obj_t *parent, const char *text, int32_t x, int32_t y,
                              int32_t width, lv_color_t color, lv_text_align_t align) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, text);
    lv_obj_set_pos(label, x, y);
    lv_obj_set_width(label, width);
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, color, 0);
    lv_obj_set_style_text_align(label, align, 0);
    return label;
}

static lv_obj_t *create_action_card(lv_obj_t *parent, int32_t x, int32_t y,
                                    int32_t width, int32_t height) {
    lv_obj_t *card = lv_obj_create(parent);
    lv_obj_set_pos(card, x, y);
    lv_obj_set_size(card, width, height);
    lv_obj_remove_flag(card, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(card, 10, 0);
    lv_obj_set_style_bg_color(card, lv_color_hex(UI_PANEL_COLOR), 0);
    lv_obj_set_style_bg_opa(card, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(card, 1, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(UI_PANEL_BORDER), 0);
    lv_obj_set_style_pad_all(card, 0, 0);
    return card;
}

static void read_pointer(lv_indev_t *indev, lv_indev_data_t *data) {
    ARG_UNUSED(indev);
    int32_t logical_x = atomic_get(&pointer_y);

    /* The CST816T reports the brightness slider's horizontal movement in
     * the opposite direction from the rotated display. The brightness page
     * contains no other pointer targets, so mirror X there while leaving the
     * raw gesture detector and all theme navigation untouched. */
    if (page == 2) logical_x = display_width() - 1 - logical_x;

    data->point.x = CLAMP(logical_x, 0, display_width() - 1);
    data->point.y = CLAMP(display_height() - 1 - atomic_get(&pointer_x),
                          0, display_height() - 1);
    data->state = atomic_get(&pointer_down) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

static void slider_event(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) dragging = true;
    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) dragging = false;
    if (code == LV_EVENT_VALUE_CHANGED) {
        int value = lv_slider_get_value(slider);
        prospector_dya_touch_brightness(value);
        if (brightness_value) lv_label_set_text_fmt(brightness_value, "%d%%", value);
    }
}

static void show_page(int next) {
    if (menu) lv_obj_delete(menu);
    menu = NULL; slider = NULL; brightness_value = NULL; page = next; dragging = false;
    if (!page) return;

    const int32_t width = display_width();
    const int32_t height = display_height();
    const int32_t content_width = width - (2 * UI_MARGIN);

    menu = lv_obj_create(lv_layer_top());
    lv_obj_set_size(menu, width, height);
    lv_obj_set_pos(menu, 0, 0);
    lv_obj_remove_flag(menu, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(menu, 0, 0);
    lv_obj_set_style_border_width(menu, 0, 0);
    lv_obj_set_style_pad_all(menu, 0, 0);
    lv_obj_set_style_bg_color(menu, lv_color_hex(UI_BG_COLOR), 0);
    lv_obj_set_style_bg_opa(menu, LV_OPA_COVER, 0);

    lv_obj_t *title = create_label(menu, page == 1 ? "TOUCH CONTROL" : "DISPLAY",
                                   UI_MARGIN, 14, content_width,
                                   lv_color_white(), LV_TEXT_ALIGN_CENTER);
    lv_obj_set_style_text_letter_space(title, 1, 0);

    lv_obj_t *accent = lv_obj_create(menu);
    lv_obj_set_size(accent, 64, 2);
    lv_obj_align(accent, LV_ALIGN_TOP_MID, 0, 42);
    lv_obj_remove_flag(accent, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_radius(accent, LV_RADIUS_CIRCLE, 0);
    lv_obj_set_style_border_width(accent, 0, 0);
    lv_obj_set_style_pad_all(accent, 0, 0);
    lv_obj_set_style_bg_color(accent, lv_color_hex(UI_ACCENT_COLOR), 0);
    lv_obj_set_style_bg_opa(accent, LV_OPA_COVER, 0);

    if (page == 1) {
        lv_obj_t *brightness = create_action_card(menu, UI_MARGIN, 58,
                                                  content_width, 66);
        create_label(brightness, "BRIGHTNESS", 16, 10, content_width - 64,
                     lv_color_white(), LV_TEXT_ALIGN_LEFT);
        create_label(brightness, "Swipe down to adjust", 16, 34, content_width - 64,
                     lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_LEFT);
        lv_obj_t *down = create_label(brightness, LV_SYMBOL_DOWN,
                                      content_width - 54, 18, 34,
                                      lv_color_hex(UI_ACCENT_COLOR), LV_TEXT_ALIGN_CENTER);
        lv_obj_set_style_text_font(down, &lv_font_montserrat_16, 0);

        lv_obj_t *themes = create_action_card(menu, UI_MARGIN, 138,
                                              content_width, 66);
        create_label(themes, "THEMES", 16, 10, content_width - 64,
                     lv_color_white(), LV_TEXT_ALIGN_LEFT);
        create_label(themes, "Swipe left to return", 16, 34, content_width - 64,
                     lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_LEFT);
        create_label(themes, LV_SYMBOL_LEFT, content_width - 54, 18, 34,
                     lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_CENTER);
    } else {
        brightness_value = create_label(menu, "", UI_MARGIN, 61, content_width,
                                        lv_color_hex(UI_ACCENT_COLOR), LV_TEXT_ALIGN_CENTER);
        lv_obj_set_style_text_font(brightness_value, &lv_font_montserrat_28, 0);
        lv_label_set_text_fmt(brightness_value, "%d%%", prospector_dya_touch_get_brightness());

        slider = lv_slider_create(menu);
        lv_obj_set_size(slider, content_width, 12);
        lv_obj_align(slider, LV_ALIGN_TOP_MID, 0, 115);
        lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_MAIN);
        lv_obj_set_style_bg_color(slider, lv_color_hex(0x3A424D), LV_PART_MAIN);
        lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_MAIN);
        lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
        lv_obj_set_style_bg_color(slider, lv_color_hex(UI_ACCENT_COLOR), LV_PART_INDICATOR);
        lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_INDICATOR);
        lv_obj_set_style_radius(slider, LV_RADIUS_CIRCLE, LV_PART_KNOB);
        lv_obj_set_style_bg_color(slider, lv_color_white(), LV_PART_KNOB);
        lv_obj_set_style_bg_opa(slider, LV_OPA_COVER, LV_PART_KNOB);
        lv_obj_set_style_pad_all(slider, 7, LV_PART_KNOB);
        lv_obj_set_style_shadow_width(slider, 5, LV_PART_KNOB);
        lv_obj_set_style_shadow_color(slider, lv_color_black(), LV_PART_KNOB);
        lv_obj_set_style_shadow_opa(slider, LV_OPA_40, LV_PART_KNOB);
        lv_obj_set_ext_click_area(slider, 18);
        lv_slider_set_range(slider, 1, 100);
        lv_slider_set_value(slider, prospector_dya_touch_get_brightness(), LV_ANIM_OFF);
        lv_obj_add_event_cb(slider, slider_event, LV_EVENT_ALL, NULL);

        create_label(menu, "1", UI_MARGIN, 139, 32,
                     lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_LEFT);
        create_label(menu, "100", width - UI_MARGIN - 44, 139, 44,
                     lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_RIGHT);

        lv_obj_t *hint = create_label(menu, LV_SYMBOL_UP "  BACK TO CONTROL",
                                      UI_MARGIN, 190, content_width,
                                      lv_color_hex(UI_TEXT_MUTED), LV_TEXT_ALIGN_CENTER);
        lv_obj_set_style_text_font(hint, &lv_font_montserrat_16, 0);
    }
}
static void input_cb(struct input_event *event, void *unused) {
    ARG_UNUSED(unused);
    if (event->type == INPUT_EV_ABS) {
        if (event->code == INPUT_ABS_X) { x = event->value; atomic_set(&pointer_x, x); }
        if (event->code == INPUT_ABS_Y) { y = event->value; atomic_set(&pointer_y, y); }
        return;
    }
    if (event->type != INPUT_EV_KEY || event->code != INPUT_BTN_TOUCH) return;
    int64_t now = k_uptime_get();
    if (event->value && !pressed) {
        start_x = x; start_y = y; started = now;
    } else if (!event->value && pressed && now - started <= 1500) {
        /* Original panel portrait -> ST7789 mdac=0x60 landscape. */
        int dx = y - start_y, dy = start_x - x;
        int ax = dx < 0 ? -dx : dx, ay = dy < 0 ? -dy : dy;
        struct gesture gesture = {0};
        if (ax > 30 && ax > ay) gesture.brightness = dx < 0 ? -1 : 1;
        else if (ay > 30 && ay > ax) gesture.theme = dy > 0 ? 1 : -1;
        else if (ax <= 10 && ay <= 10 && now - started < 350) {
            if (last_tap && now - last_tap < 350) { gesture.theme = 2; last_tap = 0; }
            else last_tap = now;
        }
        if ((gesture.theme || gesture.brightness) &&
            (!last_swipe || now - last_swipe >= 400)) {
            if (!k_msgq_put(&gestures, &gesture, K_NO_WAIT)) last_swipe = now;
        }
    }
    pressed = event->value != 0;
    atomic_set(&pointer_down, pressed);
}
INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(TOUCH_NODE), input_cb, NULL);
static void process(lv_timer_t *unused) {
    ARG_UNUSED(unused);
    struct gesture gesture;
    while (!k_msgq_get(&gestures, &gesture, K_NO_WAIT)) {
        if (dragging) continue;
        if (!page) {
            if (gesture.brightness > 0) show_page(1);
            else if (gesture.theme == 2) prospector_dya_touch_palette();
            else if (gesture.theme) prospector_dya_gesture(gesture.theme, 0);
        } else if (page == 1) {
            if (gesture.brightness < 0) show_page(0);
            else if (gesture.theme == 1) show_page(2);
        } else if (page == 2 && gesture.theme == -1) show_page(1);
    }
}
void prospector_touch_start(void) {
    if (timer) return;
    if (!device_is_ready(DEVICE_DT_GET(TOUCH_NODE))) {
        LOG_WRN("Touch unavailable: check CST816T wiring and chip ID");
        return;
    }
    lv_indev_t *indev = lv_indev_create();
    if (!indev) return;
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, read_pointer);
    timer = lv_timer_create(process, 30, NULL);
}
