/* Exact 280x240 RGB565 Retro Workstation artwork for Prospector receivers. */
#include <lvgl.h>
#include <zmk/display.h>

#include "reference_pixels.h"

static const lv_img_dsc_t retro_workstation_art = {
    .header = {.cf = LV_IMG_CF_TRUE_COLOR, .w = 280, .h = 240},
    .data_size = 280 * 240 * sizeof(uint16_t),
    .data = (const uint8_t *)reference_pixels,
};

static void plain(lv_obj_t *obj, uint32_t color) {
    lv_obj_remove_style_all(obj);
    lv_obj_set_style_bg_color(obj, lv_color_hex(color), 0);
    lv_obj_set_style_bg_opa(obj, LV_OPA_COVER, 0);
    lv_obj_set_style_border_width(obj, 0, 0);
    lv_obj_set_style_pad_all(obj, 0, 0);
    lv_obj_remove_flag(obj, LV_OBJ_FLAG_SCROLLABLE | LV_OBJ_FLAG_CLICKABLE);
}

lv_obj_t *zmk_display_status_screen(void) {
    lv_obj_t *screen = lv_obj_create(NULL);
    plain(screen, 0x141414);
    lv_obj_set_size(screen, 280, 240);
    /* Matches the Prospector panel's physical rounded glass safety area. */
    lv_obj_set_style_radius(screen, 43, 0);
    lv_obj_set_style_clip_corner(screen, true, 0);

    lv_obj_t *art = lv_img_create(screen);
    lv_img_set_src(art, &retro_workstation_art);
    lv_obj_set_pos(art, 0, 0);
    lv_obj_remove_flag(art, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
    return screen;
}
