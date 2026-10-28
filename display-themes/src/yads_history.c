/* Copyright (c) 2026 S7venYoung
 * SPDX-License-Identifier: MIT
 * Portable model of the referenced YADS five-letter/shortcut/idle behavior.
 */
#include "yads_history.h"
#include <stdio.h>
#include <string.h>
static void clear(struct yads_history *h) {
    h->length = 0; h->letters[0] = '\0'; h->layer_letter = '\0';
}
void yads_history_reset(struct yads_history *h) {
    memset(h, 0, sizeof(*h)); h->idle = true;
}
void yads_history_layer(struct yads_history *h, uint8_t layer) {
    if (layer != h->layer) { h->layer_letter = '\0'; h->layer = layer; }
}
void yads_history_key(struct yads_history *h, uint16_t key, bool pressed,
                      uint8_t layer, uint32_t now) {
    yads_history_layer(h, layer);
    if (pressed) { h->idle = false; h->last_press = now; }
    if (key >= 0xe0 && key <= 0xe7) {
        uint8_t mask = 1u << (key - 0xe0);
        if (pressed) { clear(h); h->modifiers |= mask; }
        else h->modifiers &= ~mask;
    } else if (!pressed && h->shortcut_down) {
        clear(h); h->shortcut_down = false;
    } else if (pressed && key >= 4 && key <= 0x1d) {
        char letter = 'A' + key - 4;
        if (h->length == 5) { memmove(h->letters, h->letters + 1, 4); h->length--; }
        h->letters[h->length++] = letter; h->letters[h->length] = '\0';
        h->shortcut_down = h->modifiers != 0;
        if (layer) h->layer_letter = letter;
    } else if (pressed && h->modifiers) {
        h->shortcut_down = true;
    } else if (pressed && key == 0x2a && h->length) {
        h->letters[--h->length] = '\0';
    }
}
void yads_history_text(struct yads_history *h, uint8_t layer, const char *name,
                       uint32_t now, char out[32]) {
    yads_history_layer(h, layer);
    if (!h->idle && (uint32_t)(now - h->last_press) >= 5000) {
        clear(h); h->idle = true;
    }
    char fallback[8]; snprintf(fallback, sizeof(fallback), "L%u", layer);
    if (!name || !*name) name = fallback;
    if (layer || h->idle) {
        if (h->layer_letter) snprintf(out, 32, "%.27s %c", name, h->layer_letter);
        else snprintf(out, 32, "%.31s", name);
    } else snprintf(out, 32, "%s", h->letters);
}
