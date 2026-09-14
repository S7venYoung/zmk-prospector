/* Keyboard-first Codex status theme for the rotated 280x240 Prospector display. */
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
#include <zmk/codex_metrics.h>
#include <zmk/events/codex_metrics_changed.h>
#include <prospector_touch.h>

LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_48);
LV_FONT_DECLARE(impact_56);

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

static void format_tokens(char *buffer, size_t size, uint64_t tokens) {
    if (tokens >= 1000000) {
        snprintk(buffer, size, "%llu.%lluM", (unsigned long long)(tokens / 1000000),
                 (unsigned long long)((tokens % 1000000) / 100000));
    } else if (tokens >= 1000) {
        snprintk(buffer, size, "%llu.%lluK", (unsigned long long)(tokens / 1000),
                 (unsigned long long)((tokens % 1000) / 100));
    } else {
        snprintk(buffer, size, "%llu", (unsigned long long)tokens);
    }
}

static void metrics_update(struct zmk_codex_metrics_changed state) {
    if (!codex_used_value || !codex_tokens_value) return;
    if (state.updated_at == 0) {
        lv_label_set_text(codex_used_value, "--%");
        lv_label_set_text(codex_tokens_value, "--");
        return;
    }
    /* The host reports consumption; the dashboard intentionally foregrounds what remains. */
    lv_label_set_text_fmt(codex_used_value, "%u%%", 100 - state.five_hour_used_percent);
    char tokens[16];
    format_tokens(tokens, sizeof(tokens), state.today_total_tokens);
    lv_label_set_text(codex_tokens_value, tokens);
}
static struct zmk_codex_metrics_changed metrics_get(const zmk_event_t *eh) {
    if (eh) return *as_zmk_codex_metrics_changed(eh);
    struct zmk_codex_metrics current = zmk_codex_metrics_get();
    return (struct zmk_codex_metrics_changed){current.five_hour_used_percent,
                                              current.today_total_tokens, current.updated_at};
}
ZMK_DISPLAY_WIDGET_LISTENER(codex_status_metrics, struct zmk_codex_metrics_changed,
                            metrics_update, metrics_get)
ZMK_SUBSCRIPTION(codex_status_metrics, zmk_codex_metrics_changed);

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
    lv_obj_set_width(battery_fill[state.source], MAX(2, (int32_t)state.level * 34 / 100));
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
    plain(card, INK);
    lv_obj_set_size(card, 122, 40);
    lv_obj_set_pos(card, x, 190);
    lv_obj_set_style_radius(card, 12, 0);
    lv_obj_set_style_border_width(card, 2, 0);
    lv_obj_set_style_border_color(card, lv_color_hex(MUTED), 0);
    lv_obj_t *side_label = text(card, side, &impact_20, MUTED);
    lv_obj_set_pos(side_label, 8, 7);
    battery_value[source] = text(card, "--%", &impact_20, PAPER);
    lv_obj_set_pos(battery_value[source], 28, 6);
    connection_dot[source] = lv_obj_create(card);
    plain(connection_dot[source], GREEN);
    lv_obj_set_size(connection_dot[source], 10, 10);
    lv_obj_set_pos(connection_dot[source], 104, 14);
    lv_obj_set_style_radius(connection_dot[source], LV_RADIUS_CIRCLE, 0);
    lv_obj_t *track = lv_obj_create(card);
    plain(track, PAPER);
    lv_obj_set_size(track, 38, 14);
    lv_obj_set_pos(track, 63, 12);
    lv_obj_set_style_radius(track, 3, 0);
    lv_obj_set_style_border_width(track, 1, 0);
    lv_obj_set_style_border_color(track, lv_color_hex(PAPER), 0);
    battery_fill[source] = lv_obj_create(card);
    plain(battery_fill[source], GREEN);
    lv_obj_set_size(battery_fill[source], 4, 9);
    lv_obj_set_pos(battery_fill[source], 65, 14);
    lv_obj_set_style_radius(battery_fill[source], 2, 0);
}

static lv_obj_t *panel(lv_obj_t *screen, int x, int y, int w, int h, uint32_t color,
                       uint32_t border, int radius) {
    lv_obj_t *o = lv_obj_create(screen);
    plain(o, color);
    lv_obj_set_size(o, w, h);
    lv_obj_set_pos(o, x, y);
    lv_obj_set_style_radius(o, radius, 0);
    lv_obj_set_style_border_width(o, 1, 0);
    lv_obj_set_style_border_color(o, lv_color_hex(border), 0);
    return o;
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    plain(screen, INK);
    lv_obj_set_size(screen, 280, 240);
    /* Waveshare 1.69in panel: keep all artwork inside the rounded glass safe area. */
    lv_obj_set_style_radius(screen, 24, 0);

    lv_obj_t *brand = text(screen, "CODEX", &impact_20, YELLOW);
    lv_obj_set_pos(brand, 18, 8);
    lv_obj_t *brand_suffix = text(screen, "// SOFLE", &impact_20, PAPER);
    lv_obj_set_pos(brand_suffix, 83, 8);
    lv_obj_t *usb = text(screen, "USB", &impact_16, PAPER);
    lv_obj_set_pos(usb, 226, 8);
    lv_obj_t *usb_dot = lv_obj_create(screen);
    plain(usb_dot, GREEN);
    lv_obj_set_size(usb_dot, 10, 10);
    lv_obj_set_pos(usb_dot, 258, 14);
    lv_obj_set_style_radius(usb_dot, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *header_rule = lv_obj_create(screen);
    plain(header_rule, YELLOW);
    lv_obj_set_size(header_rule, 244, 2);
    lv_obj_set_pos(header_rule, 18, 37);

    lv_obj_t *used_card = panel(screen, 14, 48, 122, 96, INK, YELLOW, 14);
    lv_obj_t *used_caption = text(used_card, "5 HOUR LEFT", &impact_16, PAPER);
    lv_obj_set_pos(used_caption, 8, 8);
    lv_obj_t *used_marks = text(used_card, "///", &impact_16, YELLOW);
    lv_obj_set_pos(used_marks, 101, 8);
    codex_used_value = text(used_card, "--%", &impact_56, YELLOW);
    lv_obj_set_pos(codex_used_value, 4, 34);
    lv_obj_set_width(codex_used_value, 114);
    lv_obj_set_style_text_align(codex_used_value, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *token_card = panel(screen, 144, 48, 122, 96, INK, YELLOW, 14);
    lv_obj_t *token_caption = text(token_card, "TODAY TOTAL", &impact_16, PAPER);
    lv_obj_set_pos(token_caption, 8, 8);
    lv_obj_t *token_marks = text(token_card, "///", &impact_16, YELLOW);
    lv_obj_set_pos(token_marks, 101, 8);
    codex_tokens_value = text(token_card, "--", &impact_48, PAPER);
    lv_obj_set_pos(codex_tokens_value, 2, 38);
    lv_obj_set_width(codex_tokens_value, 118);
    lv_obj_set_style_text_align(codex_tokens_value, LV_TEXT_ALIGN_CENTER, 0);

    lv_obj_t *keyboard_bar = panel(screen, 14, 150, 252, 32, INK, MUTED, 10);
    lv_obj_t *layer_caption = text(keyboard_bar, "LAYER", &impact_16, MUTED);
    lv_obj_set_pos(layer_caption, 10, 8);
    layer_value = text(keyboard_bar, "BASE", &impact_20, YELLOW);
    lv_obj_set_pos(layer_value, 76, 3);
    lv_obj_set_width(layer_value, 65);
    lv_obj_t *bar_divider = lv_obj_create(keyboard_bar);
    plain(bar_divider, MUTED);
    lv_obj_set_size(bar_divider, 1, 22);
    lv_obj_set_pos(bar_divider, 136, 5);
    lv_obj_t *wpm_caption = text(keyboard_bar, "WPM", &impact_16, MUTED);
    lv_obj_set_pos(wpm_caption, 164, 8);
    wpm_value = text(keyboard_bar, "0", &impact_20, YELLOW);
    lv_obj_set_pos(wpm_value, 214, 3);
    lv_obj_set_width(wpm_value, 30);
    lv_obj_set_style_text_align(wpm_value, LV_TEXT_ALIGN_RIGHT, 0);

    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) battery_card(screen, 0, 14, "L");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) battery_card(screen, 1, 144, "R");

    codex_status_layer_init();
    codex_status_battery_init();
    codex_status_connection_init();
    codex_status_wpm_init();
    codex_status_metrics_init();
    prospector_touch_attach(screen);
    return screen;
}
