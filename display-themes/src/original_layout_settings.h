/* SPDX-License-Identifier: MIT */
#pragma once
#include <stdint.h>
#include <zephyr/sys/util.h>
#include "display_settings.h"
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
#include <zmk/keymap.h>
#endif
static inline uint8_t original_layout_layer_count(void) {
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
    uint8_t count = 0;
    while (count < ZMK_KEYMAP_LAYERS_LEN && zmk_keymap_layer_index_to_id(count) != ZMK_KEYMAP_LAYER_ID_INVAL)
        count++;
    return count ? count : 1;
#else
    return display_settings_get_max_layers();
#endif
}
