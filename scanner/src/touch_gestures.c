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
struct gesture { int theme, brightness; };
K_MSGQ_DEFINE(gestures, sizeof(struct gesture), 8, 4);
static int x, y, start_x, start_y;
static bool pressed;
static int64_t started, last_swipe;
static lv_timer_t *timer;
static atomic_t pointer_x, pointer_y, pointer_down;
static lv_obj_t *menu, *slider;
static int page; /* 0: themes, 1: main/navigation, 2: display settings */
static bool dragging;
static int64_t last_tap;
static void read_pointer(lv_indev_t *indev, lv_indev_data_t *data) {
    ARG_UNUSED(indev);
    data->point.x = CLAMP(atomic_get(&pointer_y), 0, 279);
    data->point.y = CLAMP(239 - atomic_get(&pointer_x), 0, 239);
    data->state = atomic_get(&pointer_down) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}
static void slider_event(lv_event_t *event) {
    lv_event_code_t code = lv_event_get_code(event);
    if (code == LV_EVENT_PRESSED) dragging = true;
    if (code == LV_EVENT_RELEASED || code == LV_EVENT_PRESS_LOST) dragging = false;
    if (code == LV_EVENT_VALUE_CHANGED)
        prospector_dya_touch_brightness(lv_slider_get_value(slider));
}
static void show_page(int next) {
    if (menu) lv_obj_delete(menu);
    menu = NULL; slider = NULL; page = next; dragging = false;
    if (!page) return;
    menu = lv_obj_create(lv_layer_top());
    lv_obj_set_size(menu, 280, 240); lv_obj_set_pos(menu, 0, 0);
    lv_obj_remove_flag(menu, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_style_bg_color(menu, lv_color_hex(0x0a0a0a), 0);
    lv_obj_set_style_bg_opa(menu, LV_OPA_COVER, 0);
    lv_obj_t *label = lv_label_create(menu);
    lv_label_set_text(label, page == 1 ? "DISPLAY\n\nSwipe down: brightness\nSwipe left: themes" : "BRIGHTNESS\n\nSwipe up: return");
    lv_obj_set_style_text_font(label, &lv_font_montserrat_16, 0);
    lv_obj_set_style_text_color(label, lv_color_white(), 0);
    if (page == 2) {
        slider = lv_slider_create(menu); lv_obj_set_size(slider, 220, 24);
        lv_obj_set_pos(slider, 10, 130); lv_slider_set_range(slider, 1, 100);
        lv_slider_set_value(slider, prospector_dya_touch_get_brightness(), LV_ANIM_OFF);
        lv_obj_add_event_cb(slider, slider_event, LV_EVENT_ALL, NULL);
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
