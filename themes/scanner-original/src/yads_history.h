/* SPDX-License-Identifier: MIT
 * Input semantics adapted from S7venYoung/zmk-sofle-dongle-dya,
 * 4.1-display-themes, widgets/typed_keys_status.c (Copyright 2026 S7venYoung).
 */
#pragma once
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
struct yads_history {
    char letters[6];
    size_t length;
    char layer_letter;
    uint8_t modifiers, layer;
    bool shortcut_down, idle;
    uint32_t last_press;
};
void yads_history_reset(struct yads_history *h);
void yads_history_layer(struct yads_history *h, uint8_t layer);
void yads_history_key(struct yads_history *h, uint16_t key, bool pressed,
                      uint8_t layer, uint32_t now);
void yads_history_text(struct yads_history *h, uint8_t layer, const char *name,
                       uint32_t now, char out[32]);
