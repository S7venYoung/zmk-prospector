/* Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 * Uses the same Custom Settings contract as the referenced DYA dongle project.
 */
#include <lvgl.h>
#include <string.h>
#include <zephyr/sys/atomic.h>
#include <cormoran/zmk/custom_settings.h>
#include <zmk/studio/custom.h>
#include <zmk/event_manager.h>
#include "prospector_layouts.h"
#include "field_layout.h"
#include "operator_layout.h"
#include "radii_layout.h"
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include "dya_settings.h"
#define SUBSYSTEM "dongle_display_settings"
static struct zmk_rpc_custom_subsystem_meta display_meta = {
    ZMK_RPC_CUSTOM_SUBSYSTEM_UI_URLS(),
    .security = ZMK_STUDIO_RPC_HANDLER_UNSECURED,
};
static bool unused_rpc(const zmk_custom_CallRequest *req, pb_callback_t *res) {
    ARG_UNUSED(req); ARG_UNUSED(res); return false;
}
ZMK_RPC_CUSTOM_SUBSYSTEM(dongle_display_settings, &display_meta, unused_rpc);
#define DISPLAY_SETTING(id, initial, lo, hi) \
    ZMK_CUSTOM_SETTING_DEFINE(id, SUBSYSTEM, #id, ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32, \
        ZMK_CUSTOM_SETTING_VALUE_INT32(initial), ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC, \
        ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE, ZMK_CUSTOM_SETTING_PERMISSION_SECURE, \
        ZMK_CUSTOM_SETTING_RANGE_INT32(lo, hi))
DISPLAY_SETTING(display_theme, 0, 0, 4);
DISPLAY_SETTING(display_palette, 0, 0, 3);
DISPLAY_SETTING(display_brightness, 60, 1, 100);
DISPLAY_SETTING(left_hand_slot, 0, 0, 1);
DISPLAY_SETTING(right_hand_slot, 1, 0, 1);
static atomic_t dirty = ATOMIC_INIT(1);
static lv_timer_t *apply_timer;
static int32_t read_value(const struct zmk_custom_setting *setting, int32_t fallback) {
    int32_t value = fallback;
    zmk_custom_setting_get_int32(setting, &value);
    return value;
}
static void apply(lv_timer_t *timer) {
    ARG_UNUSED(timer);
    if (!atomic_cas(&dirty, 1, 0)) return;
    int32_t theme = read_value(&display_theme, 0);
    int32_t palette = read_value(&display_palette, 0);
    int32_t brightness = read_value(&display_brightness, 60);
    if (theme < 0 || theme > 4 || palette < 0 || palette > 3) return;
    prospector_layouts_set_style((prospector_layout_t)(theme + 1));
    uint8_t current = theme == 0 ? field_layout_get_palette() : theme == 1
        ? operator_layout_get_palette() : theme == 2 ? radii_layout_get_palette() : 0;
    if (theme < 3) for (int i = 0; i < ((palette - current + 4) % 4); i++)
        prospector_layouts_cycle_palette();
#if DT_HAS_COMPAT_STATUS_OKAY(pwm_leds)
    const struct device *backlight = DEVICE_DT_GET(DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds));
    if (brightness >= 1 && brightness <= 100 && device_is_ready(backlight))
        led_set_brightness(backlight, 0, (uint8_t)(100 - brightness));
#endif
}
static int changed(const zmk_event_t *eh) {
    if (as_zmk_custom_settings_initialized(eh)) atomic_set(&dirty, 1);
    const struct zmk_custom_setting_changed *ev = as_zmk_custom_setting_changed(eh);
    if (ev && ev->setting && ev->setting->custom_subsystem_id &&
        !strcmp(ev->setting->custom_subsystem_id, SUBSYSTEM)) atomic_set(&dirty, 1);
    return ZMK_EV_EVENT_BUBBLE;
}
ZMK_LISTENER(ms2_dya_settings, changed);
ZMK_SUBSCRIPTION(ms2_dya_settings, zmk_custom_setting_changed);
ZMK_SUBSCRIPTION(ms2_dya_settings, zmk_custom_settings_initialized);
void prospector_dya_start(void) {
    if (!apply_timer) apply_timer = lv_timer_create(apply, 50, NULL);
    atomic_set(&dirty, 1);
}
void prospector_dya_stop(void) {
    if (apply_timer) lv_timer_delete(apply_timer);
    apply_timer = NULL;
}

int prospector_dya_hand_slot(int hand) {
    int left = read_value(&left_hand_slot, 0);
    int right = read_value(&right_hand_slot, 1);
    if (left == right) return -1;
    return hand ? right : left;
}
