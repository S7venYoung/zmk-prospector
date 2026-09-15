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

LV_FONT_DECLARE(impact_16);
LV_FONT_DECLARE(impact_20);
LV_FONT_DECLARE(impact_56);
LV_FONT_DECLARE(Symbols_Semibold_28);
LV_FONT_DECLARE(chinese_date_18);

#define INK 0x101411
#define GOLD 0xE4B52C
#define PAPER 0xF3EEE5
#define GRAPHITE 0x202522
#define GREEN 0x58E85D

static lv_obj_t *time_value, *date_value, *temperature_value, *rain_value;
static lv_obj_t *sun_parts[5], *cloud_parts[3], *weather_rain[3];
static lv_obj_t *battery_value[ZMK_SPLIT_BLE_PERIPHERAL_COUNT];
static lv_obj_t *wpm_value;
static lv_obj_t *modifier_key[4];
static lv_obj_t *modifier_symbol[4];

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
static void metal(lv_obj_t *o, uint32_t highlight, uint32_t shadow) {
    lv_obj_set_style_bg_color(o, lv_color_hex(highlight), 0);
    lv_obj_set_style_bg_grad_color(o, lv_color_hex(shadow), 0);
    lv_obj_set_style_bg_grad_dir(o, LV_GRAD_DIR_VER, 0);
}
static lv_obj_t *label(lv_obj_t *p, const char *value, const lv_font_t *font, uint32_t color) {
    lv_obj_t *o = lv_label_create(p); lv_label_set_text(o, value);
    lv_obj_set_style_text_font(o, font, 0); lv_obj_set_style_text_color(o, lv_color_hex(color), 0); return o;
}
static void stroke(lv_obj_t *o, uint32_t color, int width, int radius) {
    lv_obj_set_style_border_width(o, width, 0); lv_obj_set_style_border_color(o, lv_color_hex(color), 0);
    lv_obj_set_style_radius(o, radius, 0);
}
static void visible(lv_obj_t *o, bool show) {
    if (show) lv_obj_clear_flag(o, LV_OBJ_FLAG_HIDDEN);
    else lv_obj_add_flag(o, LV_OBJ_FLAG_HIDDEN);
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
    if (!state.unix_time) { lv_label_set_text(time_value, "--:--"); lv_label_set_text(date_value, "WAIT HOST"); lv_label_set_text(temperature_value, "--C"); lv_label_set_text(rain_value, "--%"); return; }
    /* unix_time is UTC; the companion supplies the host's local offset. */
    uint32_t seconds = state.unix_time + state.timezone_offset_minutes * 60;
    uint32_t seconds_day = seconds % 86400;
    lv_label_set_text_fmt(time_value, "%02u:%02u", seconds_day / 3600, (seconds_day / 60) % 60);
    int temperature_abs = state.temperature_deci_c < 0 ? -state.temperature_deci_c : state.temperature_deci_c;
    lv_label_set_text_fmt(temperature_value, "%d.%dC", state.temperature_deci_c / 10, temperature_abs % 10);
    lv_label_set_text_fmt(rain_value, "%u%%", state.rain_probability);
    bool rainy = state.weather_code >= 51 && state.weather_code <= 82;
    bool cloudy = !rainy && state.weather_code >= 1 && state.weather_code <= 48;
    for (int i = 0; i < 5; i++) visible(sun_parts[i], !rainy);
    for (int i = 0; i < 3; i++) { visible(cloud_parts[i], cloudy || rainy); visible(weather_rain[i], rainy); }
    static const char *days[] = {"æ¥", "ä¸", "äº", "ä¸", "å", "äº", "å­"};
    int year; unsigned month, day, weekday; civil_date(seconds, &year, &month, &day, &weekday);
    lv_label_set_text_fmt(date_value, "ææ%s %uæ%uæ¥", days[weekday], month, day);
}
static struct zmk_host_status_changed host_get(const zmk_event_t *eh) {
    if (eh) return *as_zmk_host_status_changed(eh);
    struct zmk_host_status s = zmk_host_status_get();
    return (struct zmk_host_status_changed){
        .unix_time = s.unix_time, .temperature_deci_c = s.temperature_deci_c,
        .weather_code = s.weather_code, .observed_at = s.observed_at,
        .high_temperature_deci_c = s.high_temperature_deci_c,
        .low_temperature_deci_c = s.low_temperature_deci_c,
        .rain_probability = s.rain_probability,
        .timezone_offset_minutes = s.timezone_offset_minutes,
    };
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
    if (e && e->state) { uint32_t now = k_uptime_get_32(); if (last_key) { uint32_t dt = now - last_key; uint16_t instant = dt ? MIN(240U, 12000U / dt) : 240U; wpm = wpm ? (wpm * 3 + instant) / 4 : instant; } last_key = now; }
    return (struct wpm_state){wpm};
}
ZMK_DISPLAY_WIDGET_LISTENER(weather_clock_wpm, struct wpm_state, wpm_update, wpm_get)
ZMK_SUBSCRIPTION(weather_clock_wpm, zmk_position_state_changed);

/* The status row reflects the explicit modifier state emitted by ZMK itself.
 * This works for both local and split-keyboard keypresses and needs no macOS helper. */
static void modifier_paint(void) {
    zmk_mod_flags_t mods = zmk_hid_get_explicit_mods();
    const zmk_mod_flags_t masks[] = {MOD_LGUI | MOD_RGUI, MOD_LALT | MOD_RALT,
                                     MOD_LCTL | MOD_RCTL, MOD_LSFT | MOD_RSFT};
    for (int i = 0; i < 4; i++) {
        if (!modifier_key[i]) continue;
        bool active = (mods & masks[i]) != 0;
        metal(modifier_key[i], active ? 0xFFE679 : 0x353A37, active ? 0xAA7717 : 0x111513);
        lv_obj_set_style_border_color(modifier_key[i], lv_color_hex(active ? GOLD : PAPER), 0);
        lv_obj_set_style_text_color(modifier_symbol[i], lv_color_hex(active ? INK : PAPER), 0);
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
    lv_obj_t *s = lv_obj_create(NULL); plain(s, INK); lv_obj_set_size(s, 280, 240); lv_obj_set_style_radius(s, 24, 0); metal(s, 0x161A17, 0x030403);
    /* Left weather icon: sun, cloud, and rain are composed from crisp LVGL primitives. */
    sun_parts[0] = box(s, 25, 18, 16, 16, GOLD, LV_RADIUS_CIRCLE);
    sun_parts[1] = box(s, 30, 10, 5, 6, GOLD, 2); sun_parts[2] = box(s, 30, 38, 5, 6, GOLD, 2);
    sun_parts[3] = box(s, 17, 24, 6, 5, GOLD, 2); sun_parts[4] = box(s, 43, 24, 6, 5, GOLD, 2);
    cloud_parts[0] = box(s, 18, 27, 31, 10, PAPER, 5);
    cloud_parts[1] = box(s, 23, 21, 15, 15, PAPER, LV_RADIUS_CIRCLE);
    cloud_parts[2] = box(s, 34, 24, 12, 12, PAPER, LV_RADIUS_CIRCLE);
    weather_rain[0] = box(s, 23, 39, 3, 6, 0x73B9EE, 1);
    weather_rain[1] = box(s, 32, 39, 3, 6, 0x73B9EE, 1);
    weather_rain[2] = box(s, 41, 39, 3, 6, 0x73B9EE, 1);
    for (int i = 0; i < 3; i++) { visible(cloud_parts[i], false); visible(weather_rain[i], false); }
    temperature_value = label(s, "--C", &impact_56, GOLD); lv_obj_set_width(temperature_value, 140); lv_obj_set_style_text_align(temperature_value, LV_TEXT_ALIGN_CENTER, 0); lv_obj_set_pos(temperature_value, 72, 5);
    /* The droplets and percentage share one right-aligned baseline. */
    (void)box(s, 208, 20, 10, 12, GOLD, LV_RADIUS_CIRCLE);
    (void)box(s, 220, 13, 8, 10, GOLD, LV_RADIUS_CIRCLE);
    (void)box(s, 230, 22, 7, 9, GOLD, LV_RADIUS_CIRCLE);
    rain_value = label(s, "--%", &impact_20, GOLD); lv_obj_set_pos(rain_value, 238, 17);
    lv_obj_t *date = box(s, 28, 58, 224, 28, GOLD, 15); metal(date, 0xFFE980, 0xA87514); stroke(date, 0xFFE08A, 1, 15); date_value = label(date, "WAIT HOST", &chinese_date_18, INK); lv_obj_center(date_value);
    time_value = label(s, "00:00", &impact_56, PAPER); lv_obj_set_width(time_value, 260); lv_obj_set_style_text_align(time_value, LV_TEXT_ALIGN_CENTER, 0); lv_obj_set_pos(time_value, 10, 86);
    lv_obj_t *stats = box(s, 10, 151, 260, 80, GOLD, 16); metal(stats, 0xFFE77A, 0x9C6A10); stroke(stats, 0xF8C847, 1, 16);
    lv_obj_t *wpm_caption = label(stats, "WPM", &impact_20, INK); lv_obj_set_pos(wpm_caption, 15, 4); wpm_value = label(stats, "0", &impact_20, INK); lv_obj_set_pos(wpm_value, 32, 25);
    lv_obj_t *divider1 = box(stats, 86, 10, 1, 32, INK, 0); lv_obj_set_style_bg_opa(divider1, LV_OPA_30, 0);
    lv_obj_t *left = label(stats, "L", &impact_20, INK); lv_obj_set_pos(left, 122, 4);
#if ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 0
    battery_value[0] = label(stats, "--%", &impact_20, INK); lv_obj_set_pos(battery_value[0], 108, 25);
#endif
    lv_obj_t *divider2 = box(stats, 173, 10, 1, 32, INK, 0); lv_obj_set_style_bg_opa(divider2, LV_OPA_30, 0);
    lv_obj_t *right = label(stats, "R", &impact_20, INK); lv_obj_set_pos(right, 210, 4);
#if ZMK_SPLIT_BLE_PERIPHERAL_COUNT > 1
    battery_value[1] = label(stats, "--%", &impact_20, INK); lv_obj_set_pos(battery_value[1], 195, 25);
#endif
    lv_obj_t *mod = box(stats, 10, 49, 240, 28, GRAPHITE, 7); metal(mod, 0x303632, 0x0D100E); stroke(mod, 0x5C625E, 1, 7);
    for (int i = 0; i < 4; i++) {
        modifier_key[i] = box(mod, 4 + i * 59, 2, 54, 24, GRAPHITE, 5);
        metal(modifier_key[i], 0x353A37, 0x111513);
        stroke(modifier_key[i], PAPER, 1, 5);
        static const char *symbols[] = {"\xF4\x80\x86\x94", "\xF4\x80\x86\x95", "\xF4\x80\x86\x8D", "\xF4\x80\x86\x9D"};
        modifier_symbol[i] = label(modifier_key[i], symbols[i], &Symbols_Semibold_28, PAPER);
        lv_obj_center(modifier_symbol[i]);
    }
    weather_clock_host_init(); weather_clock_battery_init(); weather_clock_wpm_init(); modifier_paint();
    prospector_touch_attach(s); return s;
}
