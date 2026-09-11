/* Fixed WALL-E / Codex screen for the 240x280 Prospector Dongle. */
#include <lvgl.h>
#include <zephyr/sys/util.h>
#include <fonts.h>
#include <zmk/ble.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/layer_state_changed.h>
#include <zmk/events/split_central_status_changed.h>
#include <zmk/keymap.h>

static lv_obj_t *layer_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *battery_fill[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *connection_dot[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];

struct walle_layer_state { uint8_t index; };
struct walle_battery_state { uint8_t source; uint8_t level; };
struct walle_connection_state { uint8_t source; bool connected; };

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
    lv_obj_set_width(battery_fill[state.source], MAX(2, (int32_t)state.level * 72 / 100));
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
    lv_obj_set_size(eye, 42, 30); lv_obj_set_pos(eye, x, 10); lv_obj_set_style_radius(eye, 12, 0);
    lv_obj_t *iris = lv_obj_create(eye); plain(iris, 0xFFBF18);
    lv_obj_set_size(iris, 20, 20); lv_obj_center(iris); lv_obj_set_style_radius(iris, LV_RADIUS_CIRCLE, 0);
    lv_obj_t *pupil = lv_obj_create(iris); plain(pupil, 0x111612);
    lv_obj_set_size(pupil, 7, 7); lv_obj_center(pupil); lv_obj_set_style_radius(pupil, LV_RADIUS_CIRCLE, 0);
}
static void make_battery_card(lv_obj_t *screen, uint8_t source, int x, const char *side) {
    lv_obj_t *card = lv_obj_create(screen); plain(card, 0xECE7DC);
    lv_obj_set_size(card, 108, 65); lv_obj_set_pos(card, x, 169); lv_obj_set_style_radius(card, 7, 0);
    lv_obj_t *side_label = label(card, side, LV_FONT_DEFAULT, 0x111612);
    lv_obj_set_pos(side_label, 8, 5);
    connection_dot[source] = lv_obj_create(card); plain(connection_dot[source], 0xE23B2E);
    lv_obj_set_size(connection_dot[source], 8, 8); lv_obj_set_pos(connection_dot[source], 91, 8);
    lv_obj_set_style_radius(connection_dot[source], LV_RADIUS_CIRCLE, 0);
    battery_value[source] = label(card, "--%", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(battery_value[source], 8, 24);
    lv_obj_t *track = lv_obj_create(card); plain(track, 0xA9A59D);
    lv_obj_set_size(track, 76, 6); lv_obj_set_pos(track, 8, 52); lv_obj_set_style_radius(track, 2, 0);
    battery_fill[source] = lv_obj_create(card); plain(battery_fill[source], 0xFFBF18);
    lv_obj_set_size(battery_fill[source], 2, 6); lv_obj_set_pos(battery_fill[source], 8, 52);
    lv_obj_set_style_radius(battery_fill[source], 2, 0);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL); plain(screen, 0x101411); lv_obj_set_size(screen, 240, 280);
    lv_obj_t *header = lv_obj_create(screen); plain(header, 0xFFBF18);
    lv_obj_set_size(header, 240, 54); lv_obj_set_pos(header, 0, 0);
    make_eye(header, 10); make_eye(header, 55);
    lv_obj_t *title = label(header, "WALL-E", &FoundryGridnikMedium_20, 0x111612);
    lv_obj_set_pos(title, 105, 7);
    lv_obj_t *subtitle = label(header, "// CODEX", LV_FONT_DEFAULT, 0x111612);
    lv_obj_set_pos(subtitle, 105, 30);
    for (int i = 0; i < 6; i++) {
        lv_obj_t *stripe = lv_obj_create(screen); plain(stripe, (i % 2) == 0 ? 0xFFBF18 : 0x101411);
        lv_obj_set_size(stripe, 40, 7); lv_obj_set_pos(stripe, i * 40, 54);
    }
    lv_obj_t *caption = label(screen, "ACTIVE LAYER", LV_FONT_DEFAULT, 0xFFBF18);
    lv_obj_set_pos(caption, 12, 72);
    layer_value = label(screen, "BASE", &FRAC_Regular_48, 0xF3EEE5);
    lv_obj_set_width(layer_value, 216); lv_obj_set_style_text_align(layer_value, LV_TEXT_ALIGN_CENTER, 0);
    lv_obj_set_pos(layer_value, 12, 90);
    lv_obj_t *rule = lv_obj_create(screen); plain(rule, 0xFFBF18);
    lv_obj_set_size(rule, 216, 4); lv_obj_set_pos(rule, 12, 153);
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0) make_battery_card(screen, 0, 8, "LEFT");
    if (ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1) make_battery_card(screen, 1, 124, "RIGHT");
    lv_obj_t *footer = lv_obj_create(screen); plain(footer, 0x242A25);
    lv_obj_set_size(footer, 224, 32); lv_obj_set_pos(footer, 8, 242); lv_obj_set_style_radius(footer, 7, 0);
    lv_obj_t *footer_text = label(footer, "BLE  ONLINE  //  SYNC", LV_FONT_DEFAULT, 0x58E85D);
    lv_obj_center(footer_text);
    walle_layer_init(); walle_battery_init(); walle_connection_init();
    return screen;
}
