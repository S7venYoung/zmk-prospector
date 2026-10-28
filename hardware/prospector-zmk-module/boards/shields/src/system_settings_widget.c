/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#include "system_settings_widget.h"
#include "display_settings.h"
#include <zephyr/logging/log.h>
#include <zephyr/sys/reboot.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

// ========== Runtime Channel Storage (backed by NVS via display_settings) ==========

static bool runtime_channel_initialized = false;
static uint8_t runtime_scanner_channel = 0;

static void init_runtime_channel(void) {
    if (!runtime_channel_initialized) {
        runtime_scanner_channel = display_settings_get_channel();
        runtime_channel_initialized = true;
        LOG_INF("Runtime channel initialized to %d (from NVS)", runtime_scanner_channel);
    }
}

uint8_t scanner_get_runtime_channel(void) {
    init_runtime_channel();
    return runtime_scanner_channel;
}

void scanner_set_runtime_channel(uint8_t channel) {
    init_runtime_channel();
    runtime_scanner_channel = channel;
    display_settings_set_channel(channel);
    LOG_INF("Scanner channel set to %d (%s, saved to NVS)",
            channel, channel == 0 ? "All" : "Filtered");
}
