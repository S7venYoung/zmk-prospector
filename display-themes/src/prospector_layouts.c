/* SPDX-License-Identifier: MIT
 * Scanner artwork, driven by the ms2 dongle's existing local-state snapshot.
 * Numeric layout IDs stay compatible with the upstream display settings.
 */
#include "prospector_layouts.h"
#include "field_layout.h"
#include "operator_layout.h"
#include "radii_layout.h"
#include "yads_layout.h"
#include "industrial_layout.h"
#include <string.h>
#include <zephyr/sys/util.h>
#include <stdio.h>
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
#include "dya_settings.h"
#include <zmk/keymap.h>
#endif

static lv_obj_t *parent_obj;
static bool initialized;
static prospector_layout_t current_layout = PROSPECTOR_LAYOUT_OPERATOR;
static struct prospector_keyboard_data cached;

static bool supported(prospector_layout_t style) {
    return style == PROSPECTOR_LAYOUT_FIELD || style == PROSPECTOR_LAYOUT_OPERATOR ||
           style == PROSPECTOR_LAYOUT_RADII || style == PROSPECTOR_LAYOUT_YADS2 ||
           style == PROSPECTOR_LAYOUT_INDUSTRIAL;
}
static void create_layout(void) {
    switch ((int)current_layout) {
    case PROSPECTOR_LAYOUT_FIELD: field_layout_create(parent_obj); break;
    case PROSPECTOR_LAYOUT_OPERATOR: operator_layout_create(parent_obj); break;
    case PROSPECTOR_LAYOUT_RADII: radii_layout_create(parent_obj); break;
    case PROSPECTOR_LAYOUT_YADS2: yads_layout_create(parent_obj); break;
    case PROSPECTOR_LAYOUT_INDUSTRIAL: industrial_layout_create(parent_obj); break;
    default: break;
    }
}
static void destroy_layout(void) {
    switch ((int)current_layout) {
    case PROSPECTOR_LAYOUT_FIELD: field_layout_destroy(); break;
    case PROSPECTOR_LAYOUT_OPERATOR: operator_layout_destroy(); break;
    case PROSPECTOR_LAYOUT_RADII: radii_layout_destroy(); break;
    case PROSPECTOR_LAYOUT_YADS2: yads_layout_destroy(); break;
    case PROSPECTOR_LAYOUT_INDUSTRIAL: industrial_layout_destroy(); break;
    default: break;
    }
}
static void paint(void) {
    /* ms2 already maps BLS identifiers to hands: battery_level is L,
     * peripheral_battery[0] is R. Its 0 sentinel means unknown/disconnected.
     * Do not reinterpret BT connection slots, or display a dongle battery. */
    uint8_t left = cached.battery_level;
    uint8_t right[OPERATOR_MAX_PERIPHERALS] = {cached.peripheral_battery[0], 0, 0};
    bool left_valid = cached.has_dynamic_data && left > 0 && left <= 100;
    bool right_valid[OPERATOR_MAX_PERIPHERALS] = {
        cached.has_dynamic_data && right[0] > 0 && right[0] <= 100, false, false};
    char name[32];
    memcpy(name, cached.current_layer_name, sizeof(cached.current_layer_name));
    name[sizeof(cached.current_layer_name)] = '\0';
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    const char *live_name = zmk_keymap_layer_name(
        zmk_keymap_layer_index_to_id(cached.active_layer));
    if (live_name) snprintf(name, sizeof(name), "%s", live_name);
#endif
    if (!name[0]) {
        snprintf(name, sizeof(name), "L%u", cached.active_layer);
    }
    switch ((int)current_layout) {
    case PROSPECTOR_LAYOUT_FIELD:
        field_layout_update(cached.active_layer, name, left, left_valid, right[0],
            right_valid[0], cached.wpm_value, cached.modifier_flags, cached.usb_connected,
            cached.profile_slot, cached.ble_connected, cached.ble_bonded);
        break;
    case PROSPECTOR_LAYOUT_OPERATOR:
        operator_layout_update(cached.active_layer, name, left, left_valid, right,
            right_valid, cached.wpm_value, cached.modifier_flags, cached.usb_connected,
            cached.profile_slot, cached.ble_connected, cached.ble_bonded);
        break;
    case PROSPECTOR_LAYOUT_RADII:
        radii_layout_update(cached.active_layer, name, left, left_valid, right[0],
            right_valid[0], cached.modifier_flags, cached.usb_connected, cached.profile_slot);
        break;
    case PROSPECTOR_LAYOUT_YADS2:
        yads_layout_update(&cached);
        break;
    case PROSPECTOR_LAYOUT_INDUSTRIAL:
        industrial_layout_update(&cached, name);
        break;
    default: break;
    }
}
void prospector_layouts_init(lv_obj_t *parent) {
    if (initialized) return;
    parent_obj = parent;
    current_layout = supported(CONFIG_PROSPECTOR_DEFAULT_LAYOUT)
        ? CONFIG_PROSPECTOR_DEFAULT_LAYOUT : PROSPECTOR_LAYOUT_OPERATOR;
    memset(&cached, 0, sizeof(cached));
    create_layout();
    initialized = true;
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    prospector_dya_start();
#endif
}
void prospector_layouts_destroy(void) {
    if (!initialized) return;
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    prospector_dya_stop();
#endif
    destroy_layout();
    initialized = false;
    parent_obj = NULL;
}
void prospector_layouts_set_style(prospector_layout_t style) {
    if (!initialized) return;
    if (!supported(style)) style = PROSPECTOR_LAYOUT_OPERATOR;
    if (style == current_layout) return;
    destroy_layout();
    current_layout = style;
    create_layout();
    paint();
}
prospector_layout_t prospector_layouts_get_style(void) { return current_layout; }
void prospector_layouts_update(const struct prospector_keyboard_data *data) {
    if (!initialized || !data) return;
    cached = *data;
    paint();
}
void prospector_layouts_next(void) {
    prospector_layouts_set_style(current_layout == PROSPECTOR_LAYOUT_INDUSTRIAL
        ? PROSPECTOR_LAYOUT_FIELD : (prospector_layout_t)(current_layout + 1));
}
void prospector_layouts_prev(void) {
    prospector_layouts_set_style(current_layout == PROSPECTOR_LAYOUT_FIELD
        ? PROSPECTOR_LAYOUT_INDUSTRIAL : (prospector_layout_t)(current_layout - 1));
}
void prospector_layouts_cycle_palette(void) {
    if (!initialized) return;
    switch ((int)current_layout) {
    case PROSPECTOR_LAYOUT_FIELD: field_layout_cycle_palette(); break;
    case PROSPECTOR_LAYOUT_OPERATOR: operator_layout_cycle_palette(); break;
    case PROSPECTOR_LAYOUT_RADII: radii_layout_cycle_palette(); break;
    default: break;
    }
}
const char *prospector_layouts_get_name(prospector_layout_t style) {
    switch ((int)style) {
    case PROSPECTOR_LAYOUT_FIELD: return "Field";
    case PROSPECTOR_LAYOUT_OPERATOR: return "Operator";
    case PROSPECTOR_LAYOUT_RADII: return "Radii";
    case PROSPECTOR_LAYOUT_YADS2: return "YADS";
    case PROSPECTOR_LAYOUT_INDUSTRIAL: return "Industrial";
    default: return "Classic";
    }
}
