/* WALL-E / Codex theme for the rotated 280x240 Prospector display. */
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

static lv_obj_t *layer_value, *wpm_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *battery_fill[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *connection_dot[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
struct layer_state { uint8_t index; };
struct battery_state { uint8_t source, level; };
struct connection_state { uint8_t source; bool connected; };
struct wpm_state { uint16_t wpm; };

static void layer_update(struct layer_state s) {
    const char *name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(s.index));
    if (!layer_value) return;
    if (name && name[0]) lv_label_set_text(layer_value, name);
    else lv_label_set_text_fmt(layer_value, "LAYER %u", s.index);
}
static struct layer_state layer_get(const zmk_event_t *eh) {
    return (struct layer_state){zmk_keymap_highest_layer_active()};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_layer, struct layer_state, layer_update, layer_get)
ZMK_SUBSCRIPTION(theme_walle_layer, zmk_layer_state_changed);

static void battery_update(struct battery_state s) {
    if (s.source >= ARRAY_SIZE(battery_value) || !battery_value[s.source]) return;
    lv_label_set_text_fmt(battery_value[s.source], "%u%%", s.level);
    lv_obj_set_width(battery_fill[s.source], MAX(2, (int32_t)s.level * 112 / 100));
    lv_obj_set_style_bg_color(battery_fill[s.source],
        s.level < 20 ? lv_color_hex(0xE23B2E) : lv_color_hex(0xFFBF18), 0);
}
static struct battery_state battery_get(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *e =
        eh ? as_zmk_peripheral_battery_state_changed(eh) : NULL;
    return e ? (struct battery_state){e->source, e->state_of_charge} : (struct battery_state){0, 0};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_battery, struct battery_state, battery_update, battery_get)
ZMK_SUBSCRIPTION(theme_walle_battery, zmk_peripheral_battery_state_changed);

static void connection_update(struct connection_state s) {
    if (s.source >= ARRAY_SIZE(connection_dot) || !connection_dot[s.source]) return;
    lv_obj_set_style_bg_color(connection_dot[s.source],
        lv_color_hex(s.connected ? 0x58E85D : 0xE23B2E), 0);
}
static struct connection_state connection_get(const zmk_event_t *eh) {
    const struct zmk_split_central_status_changed *e =
        eh ? as_zmk_split_central_status_changed(eh) : NULL;
    return e ? (struct connection_state){e->slot, e->connected} : (struct connection_state){0, false};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_connection, struct connection_state, connection_update,
                            connection_get)
ZMK_SUBSCRIPTION(theme_walle_connection, zmk_split_central_status_changed);

static uint32_t last_key_time;
static uint16_t smoothed_wpm;
static void wpm_update(struct wpm_state s) {
    if (wpm_value) lv_label_set_text_fmt(wpm_value, "%u", s.wpm);
}
static struct wpm_state wpm_get(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *e = eh ? as_zmk_position_state_changed(eh) : NULL;
    if (e && e->state) {
        uint32_t now = k_uptime_get_32();
        if (last_key_time) {
            uint32_t elapsed = now - last_key_time;
            uint16_t instant = elapsed ? MIN(240U, 12000U / elapsed) : 240U;
            smoothed_wpm = smoothed_wpm ? (smoothed_wpm * 3 + instant) / 4 : instant;
        }
        last_key_time = now;
    }
    return (struct wpm_state){smoothed_wpm};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_wpm, struct wpm_state, wpm_update, wpm_get)
ZMK_SUBSCRIPTION(theme_walle_wpm, zmk_position_state_changed);

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *text(lv_obj_t *p, const char *s, const lv_font_t *font, uint32_t color) {
    lv_obj_t *o = lv_label_create(p); lv_label_set_text(o, s);
    lv_obj_set_style_text_font(o, font, 0); lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    return o;
}
static void eye(lv_obj_t *p, int x) {
    lv_obj_t *e = lv_obj_create(p); plain(e, 0x111612); lv_obj_set_size(e, 31, 26);
    lv_obj_set_pos(e, x, 7); lv_obj_set_style_radius(e, 9, 0);
    lv_obj_t *i = lv_obj_create(e); plain(i, 0xFFBF18); lv_obj_set_size(i, 17, 17);
    lv_obj_center(i); lv_obj_set_style_radius(i, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *u = lv_obj_create(i); plain(u, 0x111612); lv_obj_set_size(u, 7, 7);
    lv_obj_center(u); lv_obj_set_style_radius(u, LV_RADIUS_CIRCLE, 0);
}
static void battery_card(lv_obj_t *s, uint8_t source, int x, const char *side) {
    lv_obj_t *c = lv_obj_create(s); plain(c, 0xECE7DC); lv_obj_set_size(c, 132, 57);
    lv_obj_set_pos(c, x, 142); lv_obj_set_style_radius(c, 8, 0);
    lv_obj_t *side_text = text(c, side, LV_FONT_DEFAULT, 0x111612); lv_obj_set_pos(side_text, 8, 5);
    connection_dot[source] = lv_obj_create(c); plain(connection_dot[source], 0xE23B2E);
    lv_obj_set_size(connection_dot[source], 8, 8); lv_obj_set_pos(connection_dot[source], 116, 8);
    lv_obj_set_style_radius(connection_dot[source], LV_RADIUS_CIRCLE, 0);
    battery_value[source] = text(c, "--%", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(battery_value[source], 27, 4);
    lv_obj_t *track = lv_obj_create(c); plain(track, 0xA9A59D); lv_obj_set_size(track, 112, 15);
    lv_obj_set_pos(track, 8, 35); lv_obj_set_style_radius(track, 3, 0);
    battery_fill[source] = lv_obj_create(c); plain(battery_fill[source], 0xFFBF18);
    lv_obj_set_size(battery_fill[source], 2, 15); lv_obj_set_pos(battery_fill[source], 8, 35);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *s = lv_obj_create(NULL); plain(s, 0x101411); lv_obj_set_size(s, 280, 240);
    lv_obj_t *h = lv_obj_create(s); plain(h, 0xFFBF18); lv_obj_set_size(h, 280, 40);
    eye(h, 7); eye(h, 41);
    lv_obj_t *title = text(h, "SOFLE // CODEX", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(title, 78, 8);
    lv_obj_t *usb = lv_obj_create(h); plain(usb, 0xFFBF18); lv_obj_set_size(usb, 55, 28);
    lv_obj_set_pos(usb, 218, 6); lv_obj_set_style_border_width(usb, 3, 0);
    lv_obj_set_style_border_color(usb, lv_color_hex(0x111612), 0); lv_obj_set_style_radius(usb, 5, 0);
    lv_obj_t *usb_text = text(usb, "USB", LV_FONT_DEFAULT, 0x111612); lv_obj_center(usb_text);
    for (int i = 0; i < 7; i++) { lv_obj_t *m = lv_obj_create(s); plain(m, i % 2 ? 0x101411 : 0xFFBF18);
        lv_obj_set_size(m, 40, 7); lv_obj_set_pos(m, i * 40, 40); }
    layer_value = text(s, "BASE", &FRAC_Regular_48, 0xF3EEE5); lv_obj_set_width(layer_value, 264);
    lv_obj_set_style_text_align(layer_value, LV_TEXT_ALIGN_CENTER, 0); lv_obj_set_pos(layer_value, 8, 49);
    wpm_value = text(s, "0", &FRAC_Regular_48, 0xFFBF18); lv_obj_set_width(wpm_value, 92);
    lv_obj_set_style_text_align(wpm_value, LV_TEXT_ALIGN_RIGHT, 0); lv_obj_set_pos(wpm_value, 64, 94);
    lv_obj_t *wpm = text(s, "WPM", &FoundryGridnikMedium_20, 0xF3EEE5); lv_obj_set_pos(wpm, 160, 108);
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) battery_card(s, 0, 5, "L");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) battery_card(s, 1, 143, "R");
    lv_obj_t *f = lv_obj_create(s); plain(f, 0x242A25); lv_obj_set_size(f, 270, 31);
    lv_obj_set_pos(f, 5, 204); lv_obj_set_style_radius(f, 8, 0);
    lv_obj_t *status = text(f, "<     BLE      SYNC     >", &FoundryGridnikMedium_20, 0xF3EEE5);
    lv_obj_center(status);
    theme_walle_layer_init(); theme_walle_battery_init();
    theme_walle_connection_init(); theme_walle_wpm_init();
    return s;
}
