/*
 * Multi-theme screen shell for the Prospector Dongle.
 * SPDX-License-Identifier: MIT
 */

#include <lvgl.h>
#include <zephyr/dt-bindings/input/cst816s-gesture-codes.h>
#include <zephyr/dt-bindings/input/input-event-codes.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/settings/settings.h>

#include "widgets/battery_bar.h"
#include "widgets/layer_roller.h"
#include "widgets/modifier_indicator.h"

static struct zmk_widget_layer_roller layer_widget;
static struct zmk_widget_battery_bar battery_widget;
static struct zmk_widget_modifier_indicator modifier_widget;

enum prospector_theme {
    PROSPECTOR_THEME_FIELD,
    PROSPECTOR_THEME_OPERATOR,
    PROSPECTOR_THEME_RADII,
    PROSPECTOR_THEME_WALLE,
    PROSPECTOR_THEME_COUNT,
};

#define THEME_SETTINGS_KEY "prospector_theme/selected"
#define SWIPE_THRESHOLD 30
#define GESTURE_COOLDOWN_MS 400
#define TOUCH_FALLBACK_MS 350

static uint8_t current_theme = PROSPECTOR_THEME_WALLE;
static lv_obj_t *theme_screen;
static lv_obj_t *theme_header;
static lv_obj_t *theme_stripe;
static lv_obj_t *theme_eyes;
static lv_obj_t *theme_title;
static int16_t touch_start_x;
static int16_t touch_start_y;
static bool touch_active;
static bool hardware_gesture_handled;
static int64_t last_gesture_time;
static volatile int8_t pending_direction;
static volatile bool touch_fallback_pending;
static volatile uint32_t touch_fallback_deadline;
static struct k_work_delayable theme_save_work;

static void queue_theme_switch(int8_t direction);

static void set_panel_style(lv_obj_t *obj, lv_color_t color) {
    lv_obj_set_style_bg_color(obj, color, LV_PART_MAIN);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, LV_PART_MAIN);
    lv_obj_set_style_border_width(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_radius(obj, 0, LV_PART_MAIN);
    lv_obj_set_style_pad_all(obj, 0, LV_PART_MAIN);
}

static int theme_settings_set(const char *name, size_t len, settings_read_cb read_cb,
                              void *cb_arg) {
    const char *next;

    if (!settings_name_steq(name, "selected", &next) || next) {
        return -ENOENT;
    }
    if (len != sizeof(current_theme)) {
        return -EINVAL;
    }

    int err = read_cb(cb_arg, &current_theme, sizeof(current_theme));
    if (err < 0) {
        return err;
    }
    if (current_theme >= PROSPECTOR_THEME_COUNT) {
        current_theme = PROSPECTOR_THEME_WALLE;
    }
    return 0;
}

SETTINGS_STATIC_HANDLER_DEFINE(prospector_theme, "prospector_theme", NULL, theme_settings_set,
                               NULL, NULL);

static void theme_save_work_cb(struct k_work *work) {
    ARG_UNUSED(work);
    settings_save_one(THEME_SETTINGS_KEY, &current_theme, sizeof(current_theme));
}

static void apply_theme(void) {
    static const uint32_t backgrounds[] = {0x10251C, 0x071A24, 0x170E22, 0x101411};
    static const uint32_t accents[] = {0x79D98C, 0x45D6FF, 0xD981FF, 0xFFBF18};
    static const char *const marks[] = {"[+]", "//", "(O)", "[o][o]"};
    static const char *const names[] = {"FIELD", "OPERATOR", "RADII", "SOFLE // CODEX"};
    lv_color_t background = lv_color_hex(backgrounds[current_theme]);
    lv_color_t accent = lv_color_hex(accents[current_theme]);

    set_panel_style(theme_screen, background);
    set_panel_style(theme_header, accent);
    set_panel_style(theme_stripe, accent);
    lv_label_set_text(theme_eyes, marks[current_theme]);
    lv_label_set_text(theme_title, names[current_theme]);
    lv_obj_set_style_text_color(theme_eyes, background, 0);
    lv_obj_set_style_text_color(theme_title, background, 0);

    lv_obj_t *layer = zmk_widget_layer_roller_obj(&layer_widget);
    if (current_theme == PROSPECTOR_THEME_RADII) {
        lv_obj_set_size(layer, 208, 104);
        lv_obj_align(layer, LV_ALIGN_CENTER, 0, -4);
    } else if (current_theme == PROSPECTOR_THEME_FIELD) {
        lv_obj_set_size(layer, 224, 100);
        lv_obj_align(layer, LV_ALIGN_TOP_MID, 0, 58);
    } else {
        lv_obj_set_size(layer, 224, 112);
        lv_obj_align(layer, LV_ALIGN_TOP_MID, 0, 50);
    }
}

static void theme_timer_cb(lv_timer_t *timer) {
    ARG_UNUSED(timer);

    if (touch_fallback_pending &&
        (int32_t)(k_uptime_get_32() - touch_fallback_deadline) >= 0) {
        touch_fallback_pending = false;
        queue_theme_switch(1);
    }

    int8_t direction = pending_direction;

    if (direction == 0) {
        return;
    }
    pending_direction = 0;

    int next = (int)current_theme + direction;
    if (next < 0) {
        next = PROSPECTOR_THEME_COUNT - 1;
    }
    current_theme = next % PROSPECTOR_THEME_COUNT;
    apply_theme();
    k_work_reschedule(&theme_save_work, K_MSEC(750));
}

static void queue_theme_switch(int8_t direction) {
    int64_t now = k_uptime_get();

    if ((now - last_gesture_time) < GESTURE_COOLDOWN_MS) {
        return;
    }
    last_gesture_time = now;
    pending_direction = direction;
}

static void theme_touch_callback(struct input_event *event, void *user_data) {
    ARG_UNUSED(user_data);

    static int16_t touch_x;
    static int16_t touch_y;

    if (event->type == INPUT_EV_DEVICE) {
        hardware_gesture_handled = true;
        touch_fallback_pending = false;
        switch (event->code) {
        case CST816S_GESTURE_CODE_SWIPE_LEFT:
        case CST816S_GESTURE_CODE_SWIPE_UP:
            queue_theme_switch(-1);
            break;
        case CST816S_GESTURE_CODE_SWIPE_RIGHT:
        case CST816S_GESTURE_CODE_SWIPE_DOWN:
        case CST816S_GESTURE_CODE_SINGLE_CLICK:
        case CST816S_GESTURE_CODE_DOUBLE_CLICK:
            queue_theme_switch(1);
            break;
        default:
            break;
        }
    } else if (event->type == INPUT_EV_ABS && event->code == INPUT_ABS_X) {
        touch_x = event->value;
    } else if (event->type == INPUT_EV_ABS && event->code == INPUT_ABS_Y) {
        touch_y = event->value;
    } else if (event->type == INPUT_EV_KEY && event->code == INPUT_BTN_TOUCH) {
        if (event->value && !touch_active) {
            touch_start_x = touch_x;
            touch_start_y = touch_y;
            touch_active = true;
            hardware_gesture_handled = false;
            touch_fallback_deadline = k_uptime_get_32() + TOUCH_FALLBACK_MS;
            touch_fallback_pending = true;
        } else if (!event->value && touch_active) {
            int16_t raw_dx = touch_x - touch_start_x;
            int16_t raw_dy = touch_y - touch_start_y;

            /* Prospector rotates the 240x280 panel into a 280x240 LVGL display.
             * Match v2.2.3: touch Y maps to display X, touch X maps to inverted Y. */
            int16_t dx = raw_dy;
            int16_t dy = -raw_dx;
            int16_t abs_dx = dx < 0 ? -dx : dx;
            int16_t abs_dy = dy < 0 ? -dy : dy;

            touch_active = false;
            touch_fallback_pending = false;
            if (hardware_gesture_handled) {
                hardware_gesture_handled = false;
                return;
            }
            if (abs_dx > SWIPE_THRESHOLD && abs_dx > abs_dy) {
                queue_theme_switch(dx > 0 ? 1 : -1);
            } else if (abs_dx <= SWIPE_THRESHOLD && abs_dy <= SWIPE_THRESHOLD) {
                queue_theme_switch(1);
            } else {
                return;
            }
        }
    }
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(touch_sensor)), theme_touch_callback, NULL);

lv_obj_t *__wrap_zmk_display_status_screen(void) {
    const lv_color_t charcoal = lv_color_hex(0x101411);
    const lv_color_t yellow = lv_color_hex(0xFFBF18);

    k_work_init_delayable(&theme_save_work, theme_save_work_cb);

    lv_obj_t *screen = lv_obj_create(NULL);
    theme_screen = screen;
    set_panel_style(screen, charcoal);

    lv_obj_t *header = lv_obj_create(screen);
    theme_header = header;
    lv_obj_set_size(header, 240, 42);
    lv_obj_align(header, LV_ALIGN_TOP_MID, 0, 0);
    set_panel_style(header, yellow);

    lv_obj_t *eyes = lv_label_create(header);
    theme_eyes = eyes;
    lv_label_set_text(eyes, "[o][o]");
    lv_obj_set_style_text_color(eyes, charcoal, 0);
    lv_obj_align(eyes, LV_ALIGN_LEFT_MID, 8, 0);

    lv_obj_t *title = lv_label_create(header);
    theme_title = title;
    lv_label_set_text(title, "SOFLE // CODEX");
    lv_obj_set_style_text_color(title, charcoal, 0);
    lv_obj_align(title, LV_ALIGN_RIGHT_MID, -8, 0);

    lv_obj_t *stripe = lv_obj_create(screen);
    theme_stripe = stripe;
    lv_obj_set_size(stripe, 240, 6);
    lv_obj_align(stripe, LV_ALIGN_TOP_MID, 0, 42);
    set_panel_style(stripe, yellow);

    zmk_widget_layer_roller_init(&layer_widget, screen);
    lv_obj_set_size(zmk_widget_layer_roller_obj(&layer_widget), 224, 112);
    lv_obj_align(zmk_widget_layer_roller_obj(&layer_widget), LV_ALIGN_TOP_MID, 0, 50);

    zmk_widget_modifier_indicator_init(&modifier_widget, screen);
    lv_obj_align(zmk_widget_modifier_indicator_obj(&modifier_widget), LV_ALIGN_BOTTOM_RIGHT, -8,
                 -54);

    zmk_widget_battery_bar_init(&battery_widget, screen);
    lv_obj_set_size(zmk_widget_battery_bar_obj(&battery_widget), 240, 48);
    lv_obj_align(zmk_widget_battery_bar_obj(&battery_widget), LV_ALIGN_BOTTOM_MID, 0, 0);

    apply_theme();
    lv_timer_create(theme_timer_cb, 50, NULL);

    return screen;
}
