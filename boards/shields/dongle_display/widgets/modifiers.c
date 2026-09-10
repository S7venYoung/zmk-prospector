/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/services/bas.h>

#include <zephyr/logging/log.h>
LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/display.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/hid.h>
#include <dt-bindings/zmk/modifiers.h>

#include "modifiers.h"

#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
#include <zmk/display_settings.h>
#endif

struct modifiers_state {    
    uint8_t modifiers;
};

struct modifier_symbol {    
    uint8_t modifier;
    const lv_img_dsc_t *symbol_dsc;
};

LV_IMG_DECLARE(control_icon);
struct modifier_symbol ms_control = {
    .modifier = MOD_LCTL | MOD_RCTL,
    .symbol_dsc = &control_icon,
};

LV_IMG_DECLARE(shift_icon);
struct modifier_symbol ms_shift = {
    .modifier = MOD_LSFT | MOD_RSFT,
    .symbol_dsc = &shift_icon,
};

LV_IMG_DECLARE(opt_icon);
struct modifier_symbol ms_opt = {
    .modifier = MOD_LALT | MOD_RALT,
    .symbol_dsc = &opt_icon,
};

LV_IMG_DECLARE(cmd_icon);
struct modifier_symbol ms_cmd = {
    .modifier = MOD_LGUI | MOD_RGUI,
    .symbol_dsc = &cmd_icon,
};

static struct modifier_symbol *mac_modifier_symbols[] = {
    // this order determines the order of the symbols
    &ms_control,
    &ms_opt,
    &ms_cmd,
    &ms_shift
};

LV_IMG_DECLARE(alt_icon);
struct modifier_symbol ms_alt = {
    .modifier = MOD_LALT | MOD_RALT,
    .symbol_dsc = &alt_icon,
};

LV_IMG_DECLARE(win_icon);
struct modifier_symbol ms_win = {
    .modifier = MOD_LGUI | MOD_RGUI,
    .symbol_dsc = &win_icon,
};

static struct modifier_symbol *win_modifier_symbols[] = {
    // this order determines the order of the symbols
    &ms_win,
    &ms_alt,
    &ms_control,
    &ms_shift
};

static struct modifier_symbol **modifier_symbols;

static sys_slist_t widgets = SYS_SLIST_STATIC_INIT(&widgets);

static void anim_y_cb(void *var, int32_t v) {
    lv_obj_set_y(var, v);
}

static void move_object_y(void *obj, int32_t from, int32_t to) {
    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, obj);
    lv_anim_set_duration(&a, 200);
    lv_anim_set_exec_cb(&a, anim_y_cb);
    lv_anim_set_path_cb(&a, lv_anim_path_overshoot);
    lv_anim_set_values(&a, from, to);
    lv_anim_start(&a);
}

static void set_modifiers(struct zmk_widget_modifiers *widget, struct modifiers_state state) {
    for (int i = 0; i < ZMK_MODIFIER_SYMBOL_COUNT; i++) {
        bool mod_is_active = state.modifiers & modifier_symbols[i]->modifier;

        if (mod_is_active && !widget->is_active[i]) {
            move_object_y(widget->symbols[i], 1, 0);
            move_object_y(widget->selection_lines[i], SIZE_SYMBOLS + 4, SIZE_SYMBOLS + 2);
            widget->is_active[i] = true;
        } else if (!mod_is_active && widget->is_active[i]) {
            move_object_y(widget->symbols[i], 0, 1);
            move_object_y(widget->selection_lines[i], SIZE_SYMBOLS + 2, SIZE_SYMBOLS + 4);
            widget->is_active[i] = false;
        }
    }
}

void modifiers_update_cb(struct modifiers_state state) {
    struct zmk_widget_modifiers *widget;
    SYS_SLIST_FOR_EACH_CONTAINER(&widgets, widget, node) { set_modifiers(widget, state); }
}

static struct modifiers_state modifiers_get_state(const zmk_event_t *eh) {
    return (struct modifiers_state) {
        .modifiers = zmk_hid_get_explicit_mods()
    };
}

ZMK_DISPLAY_WIDGET_LISTENER(widget_modifiers, struct modifiers_state,
                            modifiers_update_cb, modifiers_get_state)

ZMK_SUBSCRIPTION(widget_modifiers, zmk_keycode_state_changed);

void zmk_widget_modifiers_refresh(struct zmk_widget_modifiers *widget) {
    bool use_mac = IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MAC_MODIFIERS);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
    use_mac = zmk_display_settings_mac_modifiers();
#endif
    modifier_symbols = use_mac ? mac_modifier_symbols : win_modifier_symbols;

    for (int i = 0; i < ZMK_MODIFIER_SYMBOL_COUNT; i++) {
        lv_img_set_src(widget->symbols[i], modifier_symbols[i]->symbol_dsc);
        widget->is_active[i] = false;
        lv_obj_set_y(widget->symbols[i], 1);
        lv_obj_set_y(widget->selection_lines[i], SIZE_SYMBOLS + 4);
    }

    set_modifiers(widget, modifiers_get_state(NULL));
}

int zmk_widget_modifiers_init(struct zmk_widget_modifiers *widget, lv_obj_t *parent) {
    bool use_mac = IS_ENABLED(CONFIG_ZMK_DONGLE_DISPLAY_MAC_MODIFIERS);
#if IS_ENABLED(CONFIG_ZMK_CUSTOM_SETTINGS)
    use_mac = zmk_display_settings_mac_modifiers();
#endif
    modifier_symbols = use_mac ? mac_modifier_symbols : win_modifier_symbols;

    widget->obj = lv_obj_create(parent);

    lv_obj_set_size(widget->obj, ZMK_MODIFIER_SYMBOL_COUNT * (SIZE_SYMBOLS + 1) + 1,
                    SIZE_SYMBOLS + 3);
    
    static lv_style_t style_line;
    lv_style_init(&style_line);
    lv_style_set_line_width(&style_line, 2);

    static const lv_point_precise_t selection_line_points[] = { {0, 0}, {SIZE_SYMBOLS, 0} };

    for (int i = 0; i < ZMK_MODIFIER_SYMBOL_COUNT; i++) {
        widget->symbols[i] = lv_img_create(widget->obj);
        lv_obj_align(widget->symbols[i], LV_ALIGN_TOP_LEFT, 1 + (SIZE_SYMBOLS + 1) * i, 1);

        widget->selection_lines[i] = lv_line_create(widget->obj);
        lv_line_set_points(widget->selection_lines[i], selection_line_points, 2);
        lv_obj_add_style(widget->selection_lines[i], &style_line, 0);
        lv_obj_align_to(widget->selection_lines[i], widget->symbols[i], LV_ALIGN_OUT_BOTTOM_LEFT, 0,
                        3);
    }

    zmk_widget_modifiers_refresh(widget);

    sys_slist_append(&widgets, &widget->node);

    widget_modifiers_init();

    return 0;
}

lv_obj_t *zmk_widget_modifiers_obj(struct zmk_widget_modifiers *widget) {
    return widget->obj;
}
