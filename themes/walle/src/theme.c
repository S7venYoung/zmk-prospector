/* WALL-E dashboard for the rotated 280 x 240 Prospector LCD. */
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/split_central_status_changed.h>
#include <zmk/keymap.h>

LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_56);

#define INK 0x101411
#define YELLOW 0xFFBF18
#define PAPER 0xF3EEE5
#define MUTED 0x6D746E
#define GREEN 0x58E85D
#define RED 0xE23B2E

static lv_obj_t *layer_value, *wpm_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *battery_segment[ZMK_SPLIT_BLE_PERIPHERAL_COUNT][6];
static lv_obj_t *connection_dot[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
struct layer_state { uint8_t index; };
struct battery_state { uint8_t source, level; };
struct connection_state { uint8_t source; bool connected; };
struct wpm_state { uint16_t wpm; };

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o); lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0); lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0); lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *box(lv_obj_t *p, int x, int y, int w, int h, uint32_t bg, int radius) {
    lv_obj_t *o = lv_obj_create(p); plain(o, bg); lv_obj_set_size(o, w, h); lv_obj_set_pos(o, x, y);
    lv_obj_set_style_radius(o, radius, 0); return o;
}
static lv_obj_t *text(lv_obj_t *p, const char *s, const lv_font_t *font, uint32_t color) {
    lv_obj_t *o = lv_label_create(p); lv_label_set_text(o, s);
    lv_obj_set_style_text_font(o, font, 0); lv_obj_set_style_text_color(o, lv_color_hex(color), 0);
    return o;
}
static void stroke(lv_obj_t *o, uint32_t color, int width, int radius) {
    lv_obj_set_style_border_width(o, width, 0); lv_obj_set_style_border_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_radius(o, radius, 0);
}

static void layer_update(struct layer_state s) {
    const char *name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(s.index));
    if (!layer_value) return;
    lv_label_set_text(layer_value, name && name[0] ? name : "BASE");
}
static struct layer_state layer_get(const zmk_event_t *eh) { return (struct layer_state){zmk_keymap_highest_layer_active()}; }
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_layer, struct layer_state, layer_update, layer_get)
ZMK_SUBSCRIPTION(theme_walle_layer, zmk_layer_state_changed);

static void battery_update(struct battery_state s) {
    if (s.source >= ARRAY_SIZE(battery_value) || !battery_value[s.source]) return;
    lv_label_set_text_fmt(battery_value[s.source], "%u%%", s.level);
    for (uint8_t i = 0; i < 6; i++) {
        uint32_t color = s.level > i * 100 / 6 ? (s.level < 20 ? RED : GREEN) : 0xA8A8A4;
        lv_obj_set_style_bg_color(battery_segment[s.source][i], lv_color_hex(color), 0);
    }
}
static struct battery_state battery_get(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *e = eh ? as_zmk_peripheral_battery_state_changed(eh) : NULL;
    return e ? (struct battery_state){e->source, e->state_of_charge} : (struct battery_state){0, 0};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_battery, struct battery_state, battery_update, battery_get)
ZMK_SUBSCRIPTION(theme_walle_battery, zmk_peripheral_battery_state_changed);

static void connection_update(struct connection_state s) {
    if (s.source >= ARRAY_SIZE(connection_dot) || !connection_dot[s.source]) return;
    lv_obj_set_style_bg_color(connection_dot[s.source], lv_color_hex(s.connected ? GREEN : RED), 0);
}
static struct connection_state connection_get(const zmk_event_t *eh) {
    const struct zmk_split_central_status_changed *e = eh ? as_zmk_split_central_status_changed(eh) : NULL;
    return e ? (struct connection_state){e->slot, e->connected} : (struct connection_state){0, false};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_connection, struct connection_state, connection_update, connection_get)
ZMK_SUBSCRIPTION(theme_walle_connection, zmk_split_central_status_changed);

static uint32_t last_key_time; static uint16_t smoothed_wpm;
static void wpm_update(struct wpm_state s) { if (wpm_value) lv_label_set_text_fmt(wpm_value, "%u", s.wpm); }
static struct wpm_state wpm_get(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *e = eh ? as_zmk_position_state_changed(eh) : NULL;
    if (e && e->state) { uint32_t now = k_uptime_get_32(); if (last_key_time) { uint32_t dt = now - last_key_time;
        uint16_t instant = dt ? MIN(240U, 12000U / dt) : 240U; smoothed_wpm = smoothed_wpm ? (smoothed_wpm * 3 + instant) / 4 : instant; } last_key_time = now; }
    return (struct wpm_state){smoothed_wpm};
}
ZMK_DISPLAY_WIDGET_LISTENER(theme_walle_wpm, struct wpm_state, wpm_update, wpm_get)
ZMK_SUBSCRIPTION(theme_walle_wpm, zmk_position_state_changed);

static void eye(lv_obj_t *p, int x) {
    lv_obj_t *rim = box(p, x, 8, 35, 28, INK, 9);
    lv_obj_t *iris = box(rim, 8, 5, 19, 19, YELLOW, LV_RADIUS_CIRCLE);
    (void)box(iris, 6, 6, 8, 8, INK, LV_RADIUS_CIRCLE);
}
static void battery_card(lv_obj_t *s, uint8_t source, int x, const char *side) {
    lv_obj_t *c = box(s, x, 155, 122, 48, PAPER, 12);
    lv_obj_t *side_text = text(c, side, &impact_20, INK); lv_obj_set_pos(side_text, 10, 8);
    battery_value[source] = text(c, "--%", &impact_20, INK); lv_obj_set_pos(battery_value[source], 38, 8);
    connection_dot[source] = box(c, 106, 11, 7, 7, RED, LV_RADIUS_CIRCLE);
    lv_obj_t *track = box(c, 10, 29, 96, 13, INK, 4); stroke(track, INK, 2, 4);
    (void)box(c, 106, 33, 4, 6, INK, 1);
    for (uint8_t i = 0; i < 6; i++) battery_segment[source][i] = box(c, 13 + i * 15, 32, 12, 7, 0xA8A8A4, 0);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *s = lv_obj_create(NULL); plain(s, INK); lv_obj_set_size(s, 280, 240);
    /* Important content remains inside the physical rounded-corner safe area. */
    lv_obj_set_style_radius(s, 24, 0);
    lv_obj_t *h = box(s, 12, 4, 256, 40, YELLOW, 18);
    eye(h, 9); eye(h, 48);
    lv_obj_t *title = text(h, "SOFLE // CODEX", &impact_20, INK); lv_obj_set_pos(title, 92, 12);
    lv_obj_t *usb = box(h, 203, 8, 45, 27, YELLOW, 6); stroke(usb, INK, 2, 6);
    lv_obj_t *usb_t = text(usb, "USB", &impact_16, INK); lv_obj_center(usb_t);
    for (int i = 0; i < 8; i++) (void)box(s, 12 + i * 32, 45, 20, 8, YELLOW, 0);

    layer_value = text(s, "BASE", &impact_56, PAPER); lv_obj_set_width(layer_value, 250);
    lv_obj_set_style_text_align(layer_value, LV_TEXT_ALIGN_CENTER, 0); lv_obj_set_pos(layer_value, 15, 53);
    for (int i = 0; i < 3; i++) { (void)box(s, 16 + i * 23, 122, 15, 7, YELLOW, 0); (void)box(s, 200 + i * 23, 122, 15, 7, YELLOW, 0); }
    wpm_value = text(s, "0", &impact_20, YELLOW); lv_obj_set_pos(wpm_value, 115, 116);
    lv_obj_t *wpm = text(s, "WPM", &impact_20, PAPER); lv_obj_set_pos(wpm, 145, 116);

    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) battery_card(s, 0, 14, "L");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) battery_card(s, 1, 144, "R");
    (void)box(s, 138, 155, 3, 48, MUTED, 2);
    lv_obj_t *footer = box(s, 14, 209, 252, 24, 0x1E2520, 9); stroke(footer, 0x404842, 2, 9);
    lv_obj_t *left = box(footer, 6, 3, 32, 18, YELLOW, 4); lv_obj_t *lt = text(left, "<", &impact_16, INK); lv_obj_center(lt);
    lv_obj_t *right = box(footer, 214, 3, 32, 18, YELLOW, 4); lv_obj_t *rt = text(right, ">", &impact_16, INK); lv_obj_center(rt);
    lv_obj_t *ble = text(footer, "BLE", &impact_20, PAPER); lv_obj_set_pos(ble, 91, 3);
    (void)box(footer, 135, 7, 10, 10, GREEN, LV_RADIUS_CIRCLE); (void)box(footer, 154, 7, 10, 10, GREEN, LV_RADIUS_CIRCLE);
    theme_walle_layer_init(); theme_walle_battery_init(); theme_walle_connection_init(); theme_walle_wpm_init();
    return s;
}
