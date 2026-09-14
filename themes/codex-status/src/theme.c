/* Keyboard-first Codex status theme for the rotated 280x240 Prospector display. */
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <fonts.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/split_central_status_changed.h>
#include <zmk/keymap.h>

#define INK 0x101411
#define YELLOW 0xFFBF18
#define PAPER 0xF3EEE5
#define MUTED 0x7C847D
#define GREEN 0x58E85D
#define RED 0xE23B2E

static lv_obj_t *layer_value, *wpm_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *battery_fill[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *connection_dot[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *codex_used_value, *codex_tokens_value;

struct layer_state { uint8_t index; };
struct battery_state { uint8_t source, level; };
struct connection_state { uint8_t source; bool connected; };
struct wpm_state { uint16_t wpm; };

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o);
    lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0);
    lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}

static lv_obj_t *text(lv_obj_t *parent, const char *value, const lv_font_t *font, uint32_t color) {
    lv_obj_t *label = lv_label_create(parent);
    lv_label_set_text(label, value);
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_style_text_color(label, lv_color_hex(color), 0);
    return label;
}

static void layer_update(struct layer_state state) {
    if (!layer_value) return;
    const char *name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(state.index));
    if (name && name[0]) lv_label_set_text(layer_value, name);
    else lv_label_set_text_fmt(layer_value, "LAYER %u", state.index);
}
static struct layer_state layer_get(const zmk_event_t *eh) {
    return (struct layer_state){zmk_keymap_highest_layer_active()};
}
ZMK_DISPLAY_WIDGET_LISTENER(codex_status_layer, struct layer_state, layer_update, layer_get)
ZMK_SUBSCRIPTION(codex_status_layer, zmk_layer_state_changed);

static void battery_update(struct battery_state state) {
    if (state.source >= ARRAY_SIZE(battery_value) || !battery_value[state.source]) return;
    lv_label_set_text_fmt(battery_value[state.source], "%u%%", state.level);
    lv_obj_set_width(battery_fill[state.source], MAX(2, (int32_t)state.level * 40 / 100));
    lv_obj_set_style_bg_color(battery_fill[state.source],
                              lv_color_hex(state.level < 20 ? RED : GREEN), 0);
}
static struct battery_state battery_get(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *event =
        eh ? as_zmk_peripheral_battery_state_changed(eh) : NULL;
    return event ? (struct battery_state){event->source, event->state_of_charge}
                 : (struct battery_state){0, 0};
}
ZMK_DISPLAY_WIDGET_LISTENER(codex_status_battery, struct battery_state, battery_update, battery_get)
ZMK_SUBSCRIPTION(codex_status_battery, zmk_peripheral_battery_state_changed);

static void connection_update(struct connection_state state) {
    if (state.source >= ARRAY_SIZE(connection_dot) || !connection_dot[state.source]) return;
    lv_obj_set_style_bg_color(connection_dot[state.source],
                              lv_color_hex(state.connected ? GREEN : RED), 0);
}
static struct connection_state connection_get(const zmk_event_t *eh) {
    const struct zmk_split_central_status_changed *event =
        eh ? as_zmk_split_central_status_changed(eh) : NULL;
    return event ? (struct connection_state){event->slot, event->connected}
                 : (struct connection_state){0, false};
}
ZMK_DISPLAY_WIDGET_LISTENER(codex_status_connection, struct connection_state, connection_update,
                            connection_get)
ZMK_SUBSCRIPTION(codex_status_connection, zmk_split_central_status_changed);

static uint32_t last_key_time;
static uint16_t smoothed_wpm;
static void wpm_update(struct wpm_state state) {
    if (wpm_value) lv_label_set_text_fmt(wpm_value, "%u", state.wpm);
}
static struct wpm_state wpm_get(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *event = eh ? as_zmk_position_state_changed(eh) : NULL;
    if (event && event->state) {
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
ZMK_DISPLAY_WIDGET_LISTENER(codex_status_wpm, struct wpm_state, wpm_update, wpm_get)
ZMK_SUBSCRIPTION(codex_status_wpm, zmk_position_state_changed);

static void battery_card(lv_obj_t *screen, uint8_t source, int x, const char *side) {
    lv_obj_t *card = lv_obj_create(screen);
    plain(card, PAPER);
    lv_obj_set_size(card, 112, 29);
    lv_obj_set_pos(card, x, 205);
    lv_obj_set_style_radius(card, 6, 0);
    lv_obj_t *side_label = text(card, side, LV_FONT_DEFAULT, INK);
    lv_obj_set_pos(side_label, 7, 6);
    battery_value[source] = text(card, "--%", &FoundryGridnikMedium_20, INK);
    lv_obj_set_pos(battery_value[source], 25, 1);
    connection_dot[source] = lv_obj_create(card);
    plain(connection_dot[source], RED);
    lv_obj_set_size(connection_dot[source], 8, 8);
    lv_obj_set_pos(connection_dot[source], 96, 6);
    lv_obj_set_style_radius(connection_dot[source], LV_RADIUS_CIRCLE, 0);
    lv_obj_t *track = lv_obj_create(card);
    plain(track, 0xA9A59D);
    lv_obj_set_size(track, 40, 4);
    lv_obj_set_pos(track, 64, 19);
    lv_obj_set_style_radius(track, 2, 0);
    battery_fill[source] = lv_obj_create(card);
    plain(battery_fill[source], GREEN);
    lv_obj_set_size(battery_fill[source], 2, 4);
    lv_obj_set_pos(battery_fill[source], 64, 19);
    lv_obj_set_style_radius(battery_fill[source], 2, 0);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    plain(screen, INK);
    lv_obj_set_size(screen, 280, 240);

    lv_obj_t *header = lv_obj_create(screen);
    plain(header, YELLOW);
    lv_obj_set_size(header, 280, 39);
    lv_obj_t *brand = text(header, "CODEX // SOFLE", &FoundryGridnikMedium_20, INK);
    lv_obj_set_pos(brand, 25, 7);
    lv_obj_t *usb = lv_obj_create(header);
    plain(usb, YELLOW);
    lv_obj_set_size(usb, 48, 25);
    lv_obj_set_pos(usb, 210, 7);
    lv_obj_set_style_border_width(usb, 2, 0);
    lv_obj_set_style_border_color(usb, lv_color_hex(INK), 0);
    lv_obj_set_style_radius(usb, 5, 0);
    lv_obj_t *usb_label = text(usb, "USB", LV_FONT_DEFAULT, INK);
    lv_obj_center(usb_label);

    lv_obj_t *used_caption = text(screen, "5 HOUR USED", LV_FONT_DEFAULT, YELLOW);
    lv_obj_set_pos(used_caption, 24, 47);
    codex_used_value = text(screen, "--%", &FRAC_Regular_48, PAPER);
    lv_obj_set_pos(codex_used_value, 18, 58);
    lv_obj_set_width(codex_used_value, 244);
    lv_obj_set_style_text_align(codex_used_value, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *divider = lv_obj_create(screen);
    plain(divider, YELLOW);
    lv_obj_set_size(divider, 238, 3);
    lv_obj_set_pos(divider, 21, 111);

    lv_obj_t *token_caption = text(screen, "TODAY TOTAL TOKEN", LV_FONT_DEFAULT, MUTED);
    lv_obj_set_pos(token_caption, 24, 119);
    codex_tokens_value = text(screen, "--", &FRAC_Regular_48, YELLOW);
    lv_obj_set_pos(codex_tokens_value, 18, 130);
    lv_obj_set_width(codex_tokens_value, 244);
    lv_obj_set_style_text_align(codex_tokens_value, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *keyboard_bar = lv_obj_create(screen);
    plain(keyboard_bar, 0x242A25);
    lv_obj_set_size(keyboard_bar, 244, 30);
    lv_obj_set_pos(keyboard_bar, 18, 172);
    lv_obj_set_style_radius(keyboard_bar, 7, 0);
    lv_obj_t *layer_caption = text(keyboard_bar, "LAYER", LV_FONT_DEFAULT, MUTED);
    lv_obj_set_pos(layer_caption, 8, 7);
    layer_value = text(keyboard_bar, "BASE", &FoundryGridnikMedium_20, PAPER);
    lv_obj_set_pos(layer_value, 52, 2);
    lv_obj_set_width(layer_value, 105);
    lv_obj_t *wpm_caption = text(keyboard_bar, "WPM", LV_FONT_DEFAULT, MUTED);
    lv_obj_set_pos(wpm_caption, 164, 7);
    wpm_value = text(keyboard_bar, "0", &FoundryGridnikMedium_20, YELLOW);
    lv_obj_set_pos(wpm_value, 205, 2);
    lv_obj_set_width(wpm_value, 31);
    lv_obj_set_style_text_align(wpm_value, LV_TEXT_ALIGN_RIGHT, 0);

    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) battery_card(screen, 0, 22, "L");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) battery_card(screen, 1, 146, "R");

    codex_status_layer_init();
    codex_status_battery_init();
    codex_status_connection_init();
    codex_status_wpm_init();
    return screen;
}
