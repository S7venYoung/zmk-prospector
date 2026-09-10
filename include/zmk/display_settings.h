/*
 * Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stdint.h>

bool zmk_display_settings_key_stats_enabled(void);
int32_t zmk_display_settings_key_stats_x(void);
int32_t zmk_display_settings_key_stats_y(void);
int32_t zmk_display_settings_layer_alignment(void);
int32_t zmk_display_settings_layer_width(void);
bool zmk_display_settings_mac_modifiers(void);
bool zmk_display_settings_dongle_battery_enabled(void);
bool zmk_display_settings_bongo_cat_enabled(void);
bool zmk_display_settings_modifiers_enabled(void);
bool zmk_display_settings_layer_enabled(void);
bool zmk_display_settings_wpm_enabled(void);
const char *zmk_display_settings_wpm_disabled_layers(void);

/* Implemented by the active status screen. Called after a runtime setting changes. */
void zmk_display_settings_runtime_changed(void);
