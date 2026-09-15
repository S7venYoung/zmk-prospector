/* Weather Clock dashboard: a receiver-only 280x240 safe-area layout. */
#include <lvgl.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/util.h>
#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/battery_state_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/ble.h>
#include <zmk/hid.h>
#include <zmk/host_status.h>
#include <zmk/events/host_status_changed.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <prospector_touch.h>
#include <fonts.h>
#include <symbols.h>

#define INK 0x101411
#define GOLD 0xE4B52C
#define PAPER 0xF3EEE5
#define GRAPHITE 0x202522

static lv_obj_t *time_value, *date_value, *temperature_value;
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *wpm_value;
static lv_obj_t *modifier_key[4];

struct battery_state { uint8_t source, level; };
struct wpm_state { uint16_t wpm; };

static void plain(lv_obj_t *o, uint32_t color) {
    lv_obj_remove_style_all(o); lv_obj_set_style_bg_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(o, LV_OPA_COVER, 0); lv_obj_set_style_border_width(o, 0, 0);
    lv_obj_set_style_radius(o, 0, 0); lv_obj_set_style_pad_all(o, 0, 0);
    lv_obj_remove_flag(o, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
}
static lv_obj_t *box(lv_obj_t *p, int x, int y, int w, int h, uint32_t color, int radius) {
    lv_obj_t *o = lv_obj_create(p); plain(o, color); lv_obj_set_pos(o, x, y); lv_obj_set_size(o, w, h);
    lv_obj_set_style_radius(o, radius, 0); return o;
}
static lv_obj_t *label(lv_obj_t *p, const char *value, const lv_font_t *font, uint32_t color) {
    lv_obj_t *o = lv_label_create(p); lv_label_set_text(o, value);
    lv_obj_set_style_text_font(o, font, 0); lv_obj_set_style_text_color(o, lv_color_hex(color), 0); return o;
}
static void stroke(lv_obj_t *o, uint32_t color, int width, int radius) {
    lv_obj_set_style_border_width(o, width, 0); lv_obj_set_style_border_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_radius(o, radius, 0);
}

static void civil_date(uint32_t seconds, int *year, unsigned *month, unsigned *day, unsigned *weekday) {
    int64_t z = seconds / 86400 + 719468;
    int era = (z >= 0 ? z : z - 146096) / 146097;
    unsigned doe = (unsigned)(z - era * 146097);
    unsigned yoe = (doe - doe / 1460 + doe / 36524 - doe / 146096) / 365;
    int y = (int)yoe + era * 400;
    unsigned doy = doe - (365 * yoe + yoe / 4 - yoe / 100);
    unsigned mp = (5 * doy + 2) / 153;
    *day = doy - (153 * mp + 2) / 5 + 1;
    *month = mp + (mp < 10 ? 3 : -9);
    *year = y + (*month <= 2);
    *weekday = (unsigned)((seconds / 86400 + 4) % 7);
}
static void host_update(struct zmk_host_status_changed state) {
    if (!time_value || !date_value || !temperature_value) return;
    if (!state.unix_time) {
        lv_label_set_text(time_value, "--:--"); lv_label_set_text(date_value, "WAIT HOST");
        lv_label_set_text(temperature_value, "--C"); return;
    }
    uint32_t seconds = state.unix_time;
    uint32_t seconds_day = seconds % 86400;
    lv_label_set_text_fmt(time_value, "%02u:%02u", seconds_day / 3600, (seconds_day / 60) % 60);
    int temperature_abs = state.temperature_deci_c < 0 ? -state.temperature_deci_c : state.temperature_deci_c;
    lv_label_set_text_fmt(temperature_value, "%d.%dC", state.temperature_deci_c / 10, temperature_abs % 10);
    static const char *days[] = {"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"};
    static const char *months[] = {"JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"};
    int year; unsigned month, day, weekday; civil_date(seconds, &year, &month, &day, &weekday);
    lv_label_set_text_fmt(date_value, "%s %02u %s", days[weekday], day, months[month - 1]);
}
static struct zmk_host_status_changed host_get(const zmk_event_t *eh) {
    if (eh) return *as_zmk_host_status_changed(eh);
    struct zmk_host_status s = zmk_host_status_get();
    return (struct zmk_host_status_changed){s.unix_time, s.temperature_deci_c, s.weather_code, s.observed_at};
}
ZMK_DISPLAY_WIDGET_LISTENER(weather_clock_host, struct zmk_host_status_changed, host_update, host_get)
ZMK_SUBSCRIPTION(weather_clock_host, zmk_host_status_changed);

static void battery_update(struct battery_state s) {
    if (s.source < ARRAY_SIZE(battery_value) && battery_value[s.source]) lv_label_set_text_fmt(battery_value[s.source], "%u%%", s.level);
}
static struct battery_state battery_get(const zmk_event_t *eh) {
    const struct zmk_peripheral_battery_state_changed *e = eh ? as_zmk_peripheral_battery_state_changed(eh) : NULL;
    return e ? (struct battery_state){e->source, e->state_of_charge} : (struct battery_state){0, 0};
}
ZMK_DISPLAY_WIDGET_LISTENER(weather_clock_battery, struct battery_state, battery_update, battery_get)
ZMK_SUBSCRIPTION(weather_clock_battery, zmk_peripheral_battery_state_changed);

static uint32_t last_key; static uint16_t wpm;
static void wpm_update(struct wpm_state s) { if (wpm_value) lv_label_set_text_fmt(wpm_value, "%u", s.wpm); }
static struct wpm_state wpm_get(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *e = eh ? as_zmk_position_state_changed(eh) : NULL;
    if (e && e->state) {
        uint32_t now = k_uptime_get_32();
        if (last_key) {
            uint32_t dt = now - last_key;
            uint16_t instant = dt ? MIN(240U, 12000U / dt) : 240U;
            wpm = wpm ? (wpm * 3 + instant) / 4 : instant;
        }
        last_key = now;
    }
    return (struct wpm_state){wpm};
}
ZMK_DISPLAY_WIDGET_LISTENER(weather_clock_wpm, struct wpm_state, wpm_update, wpm_get)
ZMK_SUBSCRIPTION(weather_clock_wpm, zmk_position_state_changed);

static void modifier_paint(void) {
    zmk_mod_flags_t mods = zmk_hid_get_explicit_mods();
    const zmk_mod_flags_t masks[] = {MOD_LGUI | MOD_RGUI, MOD_LALT | MOD_RALT,
                                     MOD_LCTL | MOD_RCTL, MOD_LSFT | MOD_RSFT};
    for (int i = 0; i < 4; i++) {
        if (!modifier_key[i]) continue;
        bool active = (mods & masks[i]) != 0;
        lv_obj_set_style_bg_color(modifier_key[i], lv_color_hex(active ? GOLD : GRAPHITE), 0);
        lv_obj_set_style_border_color(modifier_key[i], lv_color_hex(active ? GOLD : PAPER), 0);
        lv_obj_t *glyph = lv_obj_get_child(modifier_key[i], 0);
        if (glyph) lv_obj_set_style_text_color(glyph, lv_color_hex(active ? INK : PAPER), 0);
    }
}
static int modifier_listener(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *event = as_zmk_keycode_state_changed(eh);
    if (event && is_mod(event->usage_page, event->keycode)) modifier_paint();
    return ZMK_EV_EVENT_BUBBLE;
}
ZMK_LISTENER(weather_clock_modifiers, modifier_listener);
ZMK_SUBSCRIPTION(weather_clock_modifiers, zmk_keycode_state_changed);

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *s = lv_obj_create(NULL); plain(s, INK); lv_obj_set_size(s, 280, 240); lv_obj_set_style_radius(s, 24, 0);
    lv_obj_t *sun = box(s, 18, 14, 30, 30, GOLD, LV_RADIUS_CIRCLE); (void)box(sun, 7, 7, 16, 16, INK, LV_RADIUS_CIRCLE);
    lv_obj_t *weather = label(s, "TODAY'S WEATHER", &FoundryGridnikMedium_20, PAPER); lv_obj_set_pos(weather, 72, 6);
    temperature_value = label(s, "--C", &FoundryGridnikMedium_20, GOLD); lv_obj_set_pos(temperature_value, 112, 28);
    lv_obj_t *humidity = label(s, "H 25  L 18   65", &FoundryGridnikMedium_20, PAPER); lv_obj_set_pos(humidity, 72, 48);
    lv_obj_t *date = box(s, 28, 68, 224, 29, GOLD, 15); date_value = label(date, "WAIT HOST", &FoundryGridnikMedium_20, INK); lv_obj_center(date_value);
    time_value = label(s, "--:--", &FRAC_Regular_48, PAPER); lv_obj_set_width(time_value, 260); lv_obj_set_style_text_align(time_value, LV_TEXT_ALIGN_CENTER, 0); lv_obj_set_pos(time_value, 10, 97);
    lv_obj_t *stats = box(s, 10, 151, 260, 80, GOLD, 16);
    lv_obj_t *wpm_caption = label(stats, "WPM", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(wpm_caption, 20, 5); wpm_value = label(stats, "0", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(wpm_value, 32, 25);
    lv_obj_t *divider1 = box(stats, 86, 10, 1, 32, INK, 0); lv_obj_set_style_bg_opa(divider1, LV_OPA_30, 0);
    lv_obj_t *left = label(stats, "L", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(left, 122, 5);
#if ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0
    battery_value[0] = label(stats, "--%", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(battery_value[0], 108, 25);
#endif
    lv_obj_t *divider2 = box(stats, 173, 10, 1, 32, INK, 0); lv_obj_set_style_bg_opa(divider2, LV_OPA_30, 0);
    lv_obj_t *right = label(stats, "R", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(right, 210, 5);
#if ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1
    battery_value[1] = label(stats, "--%", &FoundryGridnikMedium_20, INK); lv_obj_set_pos(battery_value[1], 195, 25);
#endif
    lv_obj_t *mod = box(stats, 10, 51, 240, 24, GRAPHITE, 7);
    const char *symbols[] = {SYMBOL_COMMAND, SYMBOL_OPTION, SYMBOL_CONTROL, SYMBOL_SHIFT};
    for (int i = 0; i < 4; i++) {
        modifier_key[i] = box(mod, 4 + i * 59, 3, 54, 18, GRAPHITE, 5);
        stroke(modifier_key[i], PAPER, 1, 5);
        lv_obj_t *symbol = label(modifier_key[i], symbols[i], &Symbols_Semibold_32, PAPER);
        lv_obj_center(symbol);
    }
    weather_clock_host_init(); weather_clock_battery_init(); weather_clock_wpm_init(); modifier_paint();
    prospector_touch_attach(s); return s;
}
