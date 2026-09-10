/*
 * Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 */

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/sys/util.h>

#include <cormoran/zmk/custom_settings.h>
#include <zmk/display_settings.h>
#include <zmk/event_manager.h>

#if IS_ENABLED(CONFIG_ZMK_STUDIO_RPC)
#include <zmk/studio/custom.h>
#endif

/*
 * Custom Settings serializes a setting by looking up this identifier in the
 * registered DYA RPC subsystem table. Register a dedicated display subsystem
 * so Studio presents these values separately from the module's own settings.
 */
#define DISPLAY_SETTINGS_SUBSYSTEM "dongle_display_settings"

#if IS_ENABLED(CONFIG_ZMK_STUDIO_RPC)
static struct zmk_rpc_custom_subsystem_meta dongle_display_settings_meta = {
    ZMK_RPC_CUSTOM_SUBSYSTEM_UI_URLS(),
    .security = ZMK_STUDIO_RPC_HANDLER_UNSECURED,
};

static bool dongle_display_settings_handle_request(const zmk_custom_CallRequest *req,
                                                   pb_callback_t *res) {
    ARG_UNUSED(req);
    ARG_UNUSED(res);
    return false;
}

ZMK_RPC_CUSTOM_SUBSYSTEM(dongle_display_settings, &dongle_display_settings_meta,
                         dongle_display_settings_handle_request);
#endif

ZMK_CUSTOM_SETTING_DEFINE(
    display_key_stats_enabled, DISPLAY_SETTINGS_SUBSYSTEM, "key_stats_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(true),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_key_stats_x, DISPLAY_SETTINGS_SUBSYSTEM, "key_stats_x",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32, ZMK_CUSTOM_SETTING_VALUE_INT32(42),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_RANGE_INT32(0, 78));

ZMK_CUSTOM_SETTING_DEFINE(
    display_key_stats_y, DISPLAY_SETTINGS_SUBSYSTEM, "key_stats_y",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32, ZMK_CUSTOM_SETTING_VALUE_INT32(0),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_RANGE_INT32(0, 46));

ZMK_CUSTOM_SETTING_DEFINE(
    display_layer_alignment, DISPLAY_SETTINGS_SUBSYSTEM, "layer_alignment",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32, ZMK_CUSTOM_SETTING_VALUE_INT32(1),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_RANGE_INT32(0, 2));

ZMK_CUSTOM_SETTING_DEFINE(
    display_layer_width, DISPLAY_SETTINGS_SUBSYSTEM, "layer_width",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32, ZMK_CUSTOM_SETTING_VALUE_INT32(50),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_RANGE_INT32(20, 78));

ZMK_CUSTOM_SETTING_DEFINE(
    display_mac_modifiers, DISPLAY_SETTINGS_SUBSYSTEM, "mac_modifiers",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(true),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_dongle_battery, DISPLAY_SETTINGS_SUBSYSTEM, "dongle_battery_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(false),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_bongo_cat, DISPLAY_SETTINGS_SUBSYSTEM, "bongo_cat_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(true),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_modifiers, DISPLAY_SETTINGS_SUBSYSTEM, "modifiers_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(true),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_layer, DISPLAY_SETTINGS_SUBSYSTEM, "layer_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(true),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_wpm, DISPLAY_SETTINGS_SUBSYSTEM, "wpm_enabled",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BOOL, ZMK_CUSTOM_SETTING_VALUE_BOOL(false),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(
    display_wpm_disabled_layers, DISPLAY_SETTINGS_SUBSYSTEM, "wpm_disabled_layers",
    ZMK_CUSTOM_SETTING_VALUE_TYPE_STRING, ZMK_CUSTOM_SETTING_VALUE_STRING(""),
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
    ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE,
    ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

bool zmk_display_settings_key_stats_enabled(void) {
    bool value = true;
    zmk_custom_setting_get_bool(&display_key_stats_enabled, &value);
    return value;
}

int32_t zmk_display_settings_key_stats_x(void) {
    int32_t value = 42;
    zmk_custom_setting_get_int32(&display_key_stats_x, &value);
    return value;
}

int32_t zmk_display_settings_key_stats_y(void) {
    int32_t value = 0;
    zmk_custom_setting_get_int32(&display_key_stats_y, &value);
    return value;
}

int32_t zmk_display_settings_layer_alignment(void) {
    int32_t value = 1;
    zmk_custom_setting_get_int32(&display_layer_alignment, &value);
    return value;
}

int32_t zmk_display_settings_layer_width(void) {
    int32_t value = 50;
    zmk_custom_setting_get_int32(&display_layer_width, &value);
    return value;
}

#define BOOL_SETTING_GETTER(_name, _setting, _default)                                            \
    bool _name(void) {                                                                             \
        bool value = _default;                                                                     \
        zmk_custom_setting_get_bool(&_setting, &value);                                            \
        return value;                                                                              \
    }

BOOL_SETTING_GETTER(zmk_display_settings_mac_modifiers, display_mac_modifiers, true)
BOOL_SETTING_GETTER(zmk_display_settings_dongle_battery_enabled, display_dongle_battery, false)
BOOL_SETTING_GETTER(zmk_display_settings_bongo_cat_enabled, display_bongo_cat, true)
BOOL_SETTING_GETTER(zmk_display_settings_modifiers_enabled, display_modifiers, true)
BOOL_SETTING_GETTER(zmk_display_settings_layer_enabled, display_layer, true)
BOOL_SETTING_GETTER(zmk_display_settings_wpm_enabled, display_wpm, false)

const char *zmk_display_settings_wpm_disabled_layers(void) {
    static char value[CONFIG_ZMK_CUSTOM_SETTINGS_VALUE_MAX_SIZE + 1];
    size_t size = 0;

    if (zmk_custom_setting_read_into(&display_wpm_disabled_layers, value, sizeof(value) - 1,
                                     &size, NULL) != 0) {
        value[0] = '\0';
        return value;
    }

    value[MIN(size, sizeof(value) - 1)] = '\0';
    return value;
}

__attribute__((weak)) void zmk_display_settings_runtime_changed(void) {}

static int display_settings_listener_cb(const zmk_event_t *eh) {
    if (as_zmk_custom_settings_initialized(eh) != NULL) {
        zmk_display_settings_runtime_changed();
        return ZMK_EV_EVENT_BUBBLE;
    }

    const struct zmk_custom_setting_changed *ev = as_zmk_custom_setting_changed(eh);
    if (ev == NULL || ev->setting == NULL || ev->setting->custom_subsystem_id == NULL ||
        strcmp(ev->setting->custom_subsystem_id, DISPLAY_SETTINGS_SUBSYSTEM) != 0) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    zmk_display_settings_runtime_changed();
    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(display_settings, display_settings_listener_cb);
ZMK_SUBSCRIPTION(display_settings, zmk_custom_setting_changed);
ZMK_SUBSCRIPTION(display_settings, zmk_custom_settings_initialized);
