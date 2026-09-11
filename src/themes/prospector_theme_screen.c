/* Fixed WALL-E / Codex screen for the 240x280 Prospector Dongle. */
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <fonts.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/split_central_status_changed.h>
#include <zmk/keymap.h>

static lv_obj_t *layer_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *battery_fill[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *connection_dot[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *wpm_value;

struct walle_layer_state { uint8_t index; };
struct walle_battery_state { uint8_t source; uint8_t level; };
struct walle_connection_state { uint8_t source; bool connected; };
struct walle_wpm_state { uint16_t wpm; };

static void layer_update_cb(struct walle_layer_state state) {
    const char *name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state.index));
    if (layer_value == NULL) return;
    if (name != NULL && name[0] != '\0') lv_label_set_text(layer_value, name);
    else lv_label_set_text_fmt(layer_value, "LAYER %u", state.index);
}
static struct walle_layer_state layer_get_state(const zmk_event_t *eh) {
    return (struct walle_layer_state){.index = zmk_keymap_highest_layer_active()};
}
ZMK_DISPLAY_WIDGET_LISTENER(walle_layer, struct walle_layer_state, layer_update_cb, layer_get_state)
ZMK_SUBSCRIPTION(walle_layer, zmk_layer_state_changed);

static void battery_update_cb(struct walle_battery_state state) {
    if (state.source >= ARRAY_SIZE(battery_value) || battery_value[state.source] == NULL) return;
    lv_label_set_text_fmt(battery_value[state.source], "%u%%", state.level);
    lv_obj_set_width(battery_fill[state.source], MAX(2, (int32_t)state.level * 112 / 100));
    lv_obj_set_style_bg_color(battery_fill[state.source],
        state.level < 20 ? lv_color_hex(0xE23B2E) : lv_color_hex(0xFFBF18), 0);
}
static struct walle_battery_state battery_get_state(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *event =
        eh == NULL ? NULL : as_zmk_peripheral_battery_state_changed(eh);
    return event == NULL ? (struct walle_battery_state){0, 0}
                         : (struct walle_battery_state){event->source, event->state_of_charge};
}
ZMK_DISPLAY_WIDGET_LISTENER(walle_battery, struct walle_battery_state, battery_update_cb,
                            battery_get_state)
ZMK_SUBSCRIPTION(walle_battery, zmk_peripheral_battery_state_changed);

static void connection_update_cb(struct walle_connection_state state) {
    if (state.source >= ARRAY_SIZE(connection_dot) || connection_dot[state.source] == NULL) return;
    lv_obj_set_style_bg_color(connection_dot[state.source],
        state.connected ? lv_color_hex(0x58E85D) : lv_color_hex(0xE23B2E), 0);
}
static struct walle_connection_state connection_get_state(const zmk_event_t *eh) {
    const struct zmk_split_central_status_changed *event =
        eh == NULL ? NULL : as_zmk_split_central_status_changed(eh);
    return event == NULL ? (struct walle_connection_state){0, false}
                         : (struct walle_connection_state){event->slot, event->connected};
}
ZMK_DISPLAY_WIDGET_LISTENER(walle_connection, struct walle_connection_state,
                            connection_update_cb, connection_get_state)
ZMK_SUBSCRIPTION(walle_connection, zmk_split_central_status_changed);

static uint32_t last_key_time;
static uint16_t smoothed_wpm;
static void wpm_update_cb(struct walle_wpm_state state) {
    if (wpm_value != NULL) lv_label_set_text_fmt(wpm_value, "%u", state.wpm);
}
static struct walle_wpm_state wpm_get_state(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event =
        eh == NULL ? NULL : as_zmk_position_state_changed(eh);
    if (event != NULL && event->state) {
        uint32_t now = k_uptime_get_32();
        if (last_key_time != 0) {
            uint32_t elapsed = now - last_key_time;
            uint16_t instant = elapsed > 0 ? MIN(240U, 12000U / elapsed) : 240U;
            smoothed_wpm = smoothed_wpm == 0 ? instant : (smoothed_wpm * 3 + instant) / 4;
        }
        last_key_time = now;
    }
    return (struct walle_wpm_state){smoothed_wpm};
}
ZMK_DISPLAY_WIDGET_LISTENER(walle_wpm, struct walle_wpm_state, wpm_update_cb, wpm_get_state)
ZMK_SUBSCRIPTION(walle_wpm, zmk_position_state_changed);

static void plain(lv_obj_t *obj, uint32_t color) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_radius(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *label(lv_obj_t *parent, const char *text, const lv_font_t *font, uint32_t color) {
    lv_obj_t *obj = lv_label_create(parent);
    lv_label_set_text(obj, text);
    lv_obj_set_style_text_font(obj, font, 0);
    lv_obj_set_style_text_color(obj, lv_color_hex(color), 0);
    return obj;
}
static void make_eye(lv_obj_t *parent, int x) {
    lv_obj_t *eye = lv_obj_create(parent); plain(eye, 0x111612);
    lv_obj_set_size(eye, 31, 26); lv_obj_set_pos(eye, x, 7); lv_obj_set_style_radius(eye, 9, 0);
    lv_obj_t *iris = lv_obj_create(eye); plain(iris, 0xFFBF18);
    lv_obj_set_size(iris, 17, 17); lv_obj_center(iris); lv_obj_set_style_radius(iris, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *pupil = lv_obj_create(iris); plain(pupil, 0x111612);
    lv_obj_set_size(pupil, 7, 7); lv_obj_center(pupil); lv_obj_set_style_radius(pupil, LV_RADIUS_CIRCLE, 0);
}
static void make_battery_card(lv_obj_t *screen, uint8_t source, int x, const char *side) {
    lv_obj_t *card = lv_obj_create(screen); plain(card, 0xECE7DC);
    lv_obj_set_size(card, 132, 57); lv_obj_set_pos(card, x, 142); lv_obj_set_style_radius(card, 8, 0);
    lv_obj_t *side_label = label(card, side, LV_FONT_DEFAULT, 0x111612);
    lv_obj_set_pos(side_label, 8, 5);
    connection_dot[source] = lv_obj_create(card); plain(connection_dot[source], 0xE23B2E);
    lv_obj_set_size(connection_dot[source], 8, 8); lv_obj_set_pos(connection_dot[source], 116, 8);
    lv_obj_set_style_radius(connection_dot[source], LV_RADIUS_CIRCLE, 0);
    battery_value[source] = label(card, "--%", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(battery_value[source], 27, 4);
    lv_obj_t *track = lv_obj_create(card); plain(track, 0xA9A59D);
    lv_obj_set_size(track, 112, 15); lv_obj_set_pos(track, 8, 35); lv_obj_set_style_radius(track, 3, 0);
    battery_fill[source] = lv_obj_create(card); plain(battery_fill[source], 0xFFBF18);
    lv_obj_set_size(battery_fill[source], 2, 15); lv_obj_set_pos(battery_fill[source], 8, 35);
    lv_obj_set_style_radius(battery_fill[source], 2, 0);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL); plain(screen, 0x101411); lv_obj_set_size(screen, 280, 240);
    lv_obj_t *header = lv_obj_create(screen); plain(header, 0xFFBF18);
    lv_obj_set_size(header, 280, 40); lv_obj_set_pos(header, 0, 0);
    make_eye(header, 7); make_eye(header, 41);
    lv_obj_t *title = label(header, "SOFLE // CODEX", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(title, 78, 8);
    lv_obj_t *usb = lv_obj_create(header); plain(usb, 0xFFBF18);
    lv_obj_set_size(usb, 55, 28); lv_obj_set_pos(usb, 218, 6);
    lv_obj_set_style_border_width(usb, 3, 0); lv_obj_set_style_border_color(usb, lv_color_hex(0x111612), 0);
    lv_obj_set_style_radius(usb, 5, 0);
    lv_obj_t *usb_text = label(usb, "USB", LV_FONT_DEFAULT, 0x111612); lv_obj_center(usb_text);
    for (int i = 0; i < 7; i++) {
        lv_obj_t *stripe = lv_obj_create(screen); plain(stripe, (i % 2) == 0 ? 0xFFBF18 : 0x101411);
        lv_obj_set_size(stripe, 40, 7); lv_obj_set_pos(stripe, i * 40, 40);
    }
    layer_value = label(screen, "BASE", &FRAC_Regular_48, 0xF3EEE5);
    lv_obj_set_width(layer_value, 264); lv_obj_set_style_text_align(layer_value, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(layer_value, 8, 49);
    wpm_value = label(screen, "0", &FRAC_Regular_48, 0xFFBF18);
    lv_obj_set_width(wpm_value, 92); lv_obj_set_style_text_align(wpm_value, LV_TEXT_ALIGN_RIGHT, 0);
    lv_obj_set_pos(wpm_value, 64, 94);
    lv_obj_t *wpm_text = label(screen, "WPM", &FoundryGridnikMedium_20, 0xF3EEE5);
    lv_obj_set_pos(wpm_text, 160, 108);
    for (int i = 0; i < 3; i++) {
        lv_obj_t *left_mark = lv_obj_create(screen); plain(left_mark, 0xFFBF18);
        lv_obj_set_size(left_mark, 20, 6); lv_obj_set_pos(left_mark, 10 + i * 25, 118);
        lv_obj_t *right_mark = lv_obj_create(screen); plain(right_mark, 0xFFBF18);
        lv_obj_set_size(right_mark, 20, 6); lv_obj_set_pos(right_mark, 205 + i * 25, 118);
    }
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) make_battery_card(screen, 0, 5, "L");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) make_battery_card(screen, 1, 143, "R");
    lv_obj_t *footer = lv_obj_create(screen); plain(footer, 0x242A25);
    lv_obj_set_size(footer, 270, 31); lv_obj_set_pos(footer, 5, 204); lv_obj_set_style_radius(footer, 8, 0);
    lv_obj_t *left_arrow = label(footer, "<", &FoundryGridnikMedium_20, 0xFFBF18); lv_obj_set_pos(left_arrow, 10, 3);
    lv_obj_t *right_arrow = label(footer, ">", &FoundryGridnikMedium_20, 0xFFBF18); lv_obj_set_pos(right_arrow, 245, 3);
    lv_obj_t *footer_text = label(footer, "BLE      SYNC", &FoundryGridnikMedium_20, 0xF3EEE5);
    lv_obj_center(footer_text);
    walle_layer_init(); walle_battery_init(); walle_connection_init(); walle_wpm_init();
    return screen;
}
