/*
 * Copyright (c) 2026 S7venYoung
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/event_manager.h>

struct zmk_key_stats_changed {
    uint32_t total;
    uint32_t today;
};

ZMK_EVENT_DECLARE(zmk_key_stats_changed);
