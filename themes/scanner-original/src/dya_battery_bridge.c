/* SPDX-License-Identifier: MIT
 * DYA-core adapter for the existing ms2 left/right display cache.
 * Uses the public split transport API; no Bluetooth callback is replaced.
 */
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/iterable_sections.h>
#include <zmk/split/central.h>
#include <zmk/split/transport/central.h>
#include "s7789_update.h"
#include "dya_settings.h"
static struct k_work_delayable update_work;
static void update(struct k_work *work) {
    ARG_UNUSED(work);
    bool connected[ZMK_SPLIT_CENTRAL_PERIPHERAL_COUNT] = {false};
    STRUCT_SECTION_FOREACH(zmk_split_transport_central, transport) {
        if (!transport->api->get_available_source_ids) continue;
        uint8_t sources[ZMK_SPLIT_CENTRAL_PERIPHERAL_COUNT];
        int count = transport->api->get_available_source_ids(sources);
        for (int i = 0; i < count && i < ARRAY_SIZE(sources); i++)
            if (sources[i] < ARRAY_SIZE(connected)) connected[sources[i]] = true;
    }
    uint8_t hands[2] = {0};
    for (int hand = 0; hand < 2; hand++) {
        int slot = prospector_dya_hand_slot(hand);
        if (slot >= 0 && slot < ARRAY_SIZE(connected) && connected[slot]) {
            uint8_t level = 0;
            if (!zmk_split_central_get_peripheral_battery_level(slot, &level) && level <= 100)
                hands[hand] = level;
        }
    }
    ble_battery_update(hands[0], hands[1]);
    k_work_reschedule(&update_work, K_SECONDS(1));
}
static int start(void) {
    k_work_init_delayable(&update_work, update);
    k_work_reschedule(&update_work, K_SECONDS(1));
    return 0;
}
SYS_INIT(start, APPLICATION, 99);
