/*
 * Copyright (c) 2024 The ZMK Contributors
 * SPDX-License-Identifier: MIT
 *
 * YADS2 Layout for Scanner Mode
 *
 * A second YADS-style layout, modelled on the original YADS status screen from
 * janpfischer/zmk-dongle-screen (MIT License):
 * https://github.com/janpfischer/zmk-dongle-screen/tree/main/boards/shields/dongle_screen
 *
 * Widget arrangement (adapted to the 280x240 scanner panel):
 * - Top corners: hand connection status (left/right half) with the BLE indicator
 *   beside it - "L ✓ BLE 1" on the left, "BLE 2 R ✓" on the right. The BLE
 *   values are placeholders ("BLE 1" / "BLE 2") until updated through
 *   yads2_layout_set_ble(); the hand status follows the keyboard's halves.
 * - Keyboard name (top centre)
 * - Layer roller (centre): exactly 3 entries with the current layer always in
 *   the middle row (its neighbours above/below), all drawn at the same size;
 *   the current one is highlighted in white, the neighbours dimmed. Rows
 *   outside the keymap are hidden, so the highlight stays centred even on the
 *   first/last layer. The names come from this firmware's own keymap, so full
 *   display-names are available (the advertisement only carries 4 characters).
 *   Before any update the first layer is highlighted;
 *   yads2_layout_set_layer() moves the highlight to the current layer.
 * - Modifier icons (NerdFont row below the layer list)
 * - Battery level per half along the bottom edge
 * - RSSI ("-62dBm") just above the battery row on the right; it comes from the
 *   same source as the Classic status screen (s7789_update's ble_signal_rssi +
 *   ble_is_signal_pending()) and falls back to a dim "--dBm" placeholder while
 *   no value has been reported.
 *
 * WPM is intentionally not shown on this layout.
 *
 * Unlike the Classic (YADS) screen this layout is self-contained: it does not
 * touch the loose widget set in custom_status_screen.c but follows the
 * create/update/destroy contract of the other Prospector Display layouts.
 */

#pragma once

#include <lvgl.h>
#include <stdbool.h>

/* Battery row capacity: slot 0 = keyboard (central), slots 1..3 = peripherals */
#define YADS2_MAX_PERIPHERALS 3
#define YADS2_MAX_BATTERIES (YADS2_MAX_PERIPHERALS + 1)

/* YADS2 layout API */
lv_obj_t *yads2_layout_create(lv_obj_t *parent);

/**
 * @brief Highlight one layer in the layer list
 *
 * The names themselves are read from this firmware's own keymap
 * (zmk_keymap_layer_name()), so the full display-name is available - the status
 * advertisement only carries 4 characters of a layer name and is therefore not
 * used for the list. Call with 0 for the first layer (which is also the default
 * before any call).
 *
 * @param index Layer index (clamped to the number of keymap layers)
 */
void yads2_layout_set_layer(uint8_t index);

/**
 * @brief Update one BLE indicator in the top corners
 *
 * Two indicators are shown side by side: slot 0 in the left corner, slot 1 in
 * the right corner. They start as placeholders ("BLE 1" / "BLE 2"); pass the
 * real profile number (1..5) to show "BLE n", or 0/anything else for "BLE -".
 *
 * @param slot 0 = left indicator, 1 = right indicator
 * @param profile BLE profile number (1..5) to display
 */
void yads2_layout_set_ble(uint8_t slot, uint8_t profile);

void yads2_layout_update(uint8_t active_layer, const char *layer_name,
                         uint8_t battery_level, bool battery_connected,
                         const uint8_t peripheral_battery[YADS2_MAX_PERIPHERALS],
                         const bool peripheral_connected[YADS2_MAX_PERIPHERALS],
                         uint8_t wpm, uint8_t modifier_flags,
                         bool usb_connected, uint8_t ble_profile,
                         bool ble_connected, bool ble_bonded,
                         const char *keyboard_name);

/**
 * @brief Take a newly pushed RSSI, if there is one, and redraw the label
 *
 * The layout's main refresh is data driven (yads2_layout_update() only runs
 * when the central has something new to report), so the RSSI would otherwise
 * only follow along with key presses and the periodic battery polls. The
 * display timer (custom_status_screen.c, 100 ms) calls this instead: it returns
 * immediately when nothing new was pushed (ble_is_signal_pending()) or when the
 * value did not change.
 */
void yads2_layout_refresh_rssi(void);

void yads2_layout_destroy(void);
