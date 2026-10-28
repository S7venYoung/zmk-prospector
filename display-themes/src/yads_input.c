/* SPDX-License-Identifier: MIT
 * Event callbacks touch the model only; LVGL stays on the display thread.
 */
#include "yads_input.h"
#include "yads_history.h"
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/events/keycode_state_changed.h>
#include <zmk/keymap.h>
static struct k_spinlock model_lock;
static struct yads_history history;
static bool active;
void yads_input_enable(bool enabled) {
    k_spinlock_key_t key = k_spin_lock(&model_lock);
    active = enabled; yads_history_reset(&history);
    k_spin_unlock(&model_lock, key);
}
void yads_input_text(uint8_t layer, const char *name, char out[32]) {
    k_spinlock_key_t key = k_spin_lock(&model_lock);
    const char *live_name = zmk_keymap_layer_name(zmk_keymap_layer_index_to_id(layer));
    yads_history_text(&history, layer, live_name ? live_name : name, k_uptime_get_32(), out);
    k_spin_unlock(&model_lock, key);
}
static int key_event(const zmk_event_t *eh) {
    const struct zmk_keycode_state_changed *ev = as_zmk_keycode_state_changed(eh);
    if (!ev || ev->usage_page != 0x07) return ZMK_EV_EVENT_BUBBLE;
    k_spinlock_key_t key = k_spin_lock(&model_lock);
    if (active) yads_history_key(&history, ev->keycode, ev->state,
        zmk_keymap_highest_layer_active(), k_uptime_get_32());
    k_spin_unlock(&model_lock, key);
    return ZMK_EV_EVENT_BUBBLE;
}
ZMK_LISTENER(ms2_yads_history, key_event);
ZMK_SUBSCRIPTION(ms2_yads_history, zmk_keycode_state_changed);
