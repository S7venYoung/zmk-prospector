/* SPDX-License-Identifier: MIT
 * One-shot boot overlay; all work runs on the existing LVGL timer thread.
 * USB/RPC and BLE scanning never wait for this animation.
 */
#include "boot_animation.h"
LV_FONT_DECLARE(boot_title_56);
LV_FONT_DECLARE(impact_16);
static lv_obj_t *overlay, *reveal, *title, *subtitle, *beam, *rule[2];
static lv_timer_t *timer;
static uint32_t started;
static bool played;
#define AMBER 0xFFBD22

bool prospector_boot_active(void) { return overlay != NULL; }
void prospector_boot_stop(void) {
    if (timer) { lv_timer_delete(timer); timer = NULL; }
    if (overlay) { lv_obj_delete(overlay); overlay = NULL; }
}
static lv_obj_t *block(lv_obj_t *parent, int x, int y, int w, int h) {
    lv_obj_t *obj = lv_obj_create(parent);
    lv_obj_remove_style_all(obj);
    lv_obj_set_pos(obj, x, y); lv_obj_set_size(obj, w, h);
    lv_obj_set_style_bg_color(obj, lv_color_hex(AMBER), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    return obj;
}
static void frame(lv_timer_t *t) {
    (void)t;
    uint32_t ms = lv_tick_elaps(started);
    if (ms >= 1100) { prospector_boot_stop(); return; }
    if (ms < 200) {
        lv_obj_set_pos(beam, 38, 119);
        lv_obj_set_size(beam, 1 + ms * 102 / 200, 2);
    } else {
        lv_obj_remove_flag(reveal, LV_OBJ_FLAG_HIDDEN);
        int x = 38 + (int)LV_MIN(ms - 200, 450U) * 204 / 450;
        lv_obj_set_pos(beam, x, 68); lv_obj_set_size(beam, 2, 106);
        lv_obj_set_width(reveal, LV_MAX(1, x - 38));
        if (ms >= 650) {
            lv_obj_add_flag(beam, LV_OBJ_FLAG_HIDDEN);
            lv_obj_set_width(reveal, 204);
            lv_obj_set_style_opa(subtitle, LV_OPA_COVER, 0);
            for (int i = 0; i < 2; i++) lv_obj_set_style_opa(rule[i], LV_OPA_COVER, 0);
        }
    }
    if (ms >= 850)
        lv_obj_set_style_opa(overlay, (1100 - ms) * LV_OPA_COVER / 250, 0);
}
void prospector_boot_start(lv_obj_t *parent) {
    if (played) return;
    played = true;
    /* Separate top layer survives initial NVS/DYA theme restoration. */
    overlay = lv_obj_create(lv_display_get_layer_top(lv_obj_get_display(parent)));
    lv_obj_remove_style_all(overlay);
    lv_obj_set_size(overlay, 280, 240);
    lv_obj_set_style_bg_color(overlay, lv_color_black(), 0);
    lv_obj_set_style_bg_opa(overlay, LV_OPA_COVER, 0);
    lv_obj_remove_flag(overlay, LV_OBJ_FLAG_SCROLLABLE);
    reveal = lv_obj_create(overlay); lv_obj_remove_style_all(reveal);
    lv_obj_set_pos(reveal, 38, 80); lv_obj_set_size(reveal, 1, 64);
    lv_obj_remove_flag(reveal, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_flag(reveal, LV_OBJ_FLAG_HIDDEN);
    title = lv_label_create(reveal);
    lv_obj_set_style_text_font(title, &boot_title_56, 0);
    lv_obj_set_style_text_color(title, lv_color_hex(AMBER), 0);
    lv_label_set_text(title, "CORNE");
    lv_obj_update_layout(title);
    lv_obj_set_pos(title, (204 - lv_obj_get_width(title)) / 2, 0);
    subtitle = lv_label_create(overlay);
    lv_obj_set_style_text_font(subtitle, &impact_16, 0);
    lv_obj_set_style_text_color(subtitle, lv_color_hex(AMBER), 0);
    lv_obj_set_style_text_letter_space(subtitle, 4, 0);
    lv_label_set_text(subtitle, "SCANNER");
    lv_obj_align(subtitle, LV_ALIGN_TOP_MID, 0, 153);
    lv_obj_set_style_opa(subtitle, LV_OPA_TRANSP, 0);
    rule[0] = block(overlay, 66, 67, 148, 1);
    rule[1] = block(overlay, 66, 177, 148, 1);
    for (int i = 0; i < 2; i++) lv_obj_set_style_opa(rule[i], LV_OPA_TRANSP, 0);
    beam = block(overlay, 38, 119, 1, 2);
    started = lv_tick_get();
    timer = lv_timer_create(frame, 32, NULL);
    if (!timer) prospector_boot_stop();
}
