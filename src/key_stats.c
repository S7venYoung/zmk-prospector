/*
 * Copyright (c) 2026 S7venYoung
 *
 * SPDX-License-Identifier: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/logging/log.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#include <zmk/event_manager.h>
#include <zmk/events/key_stats_changed.h>
#include <zmk/events/position_state_changed.h>
#include <zmk/key_stats.h>

#if IS_ENABLED(CONFIG_SETTINGS)
#include <zephyr/settings/settings.h>
#endif

#define KEY_STATS_SETTINGS_KEY "key_stats/total"

static uint32_t total_count;
static uint32_t today_count;
static bool settings_ready;
static struct k_work_delayable save_work;

ZMK_EVENT_IMPL(zmk_key_stats_changed);

uint32_t zmk_key_stats_total(void) {
    return total_count;
}

uint32_t zmk_key_stats_today(void) {
    return today_count;
}

#if IS_ENABLED(CONFIG_SETTINGS)
static int key_stats_handle_set(const char *name, size_t len, settings_read_cb read_cb,
                                void *cb_arg) {
    const char *next;

    if (!settings_name_steq(name, "total", &next) || next) {
        return 0;
    }

    if (len != sizeof(total_count)) {
        return -EINVAL;
    }

    int err = read_cb(cb_arg, &total_count, sizeof(total_count));
    if (err <= 0) {
        LOG_WRN("Failed to read key stats total: %d", err);
        return err;
    }

    return 0;
}

static int key_stats_handle_commit(void) {
    settings_ready = true;
    return 0;
}

static void key_stats_save_work_cb(struct k_work *work) {
    if (!settings_ready) {
        return;
    }

    int err = settings_save_one(KEY_STATS_SETTINGS_KEY, &total_count, sizeof(total_count));
    if (err) {
        LOG_WRN("Failed to save key stats total: %d", err);
    }
}

static struct settings_handler key_stats_settings_handler = {
    .name = "key_stats",
    .h_set = key_stats_handle_set,
    .h_commit = key_stats_handle_commit,
};
#else
static void key_stats_save_work_cb(struct k_work *work) {}
#endif

static int key_stats_listener_cb(const zmk_event_t *eh) {
    const struct zmk_position_state_changed *ev = as_zmk_position_state_changed(eh);

    if (ev == NULL || !ev->state) {
        return ZMK_EV_EVENT_BUBBLE;
    }

    total_count++;
    today_count++;

    raise_zmk_key_stats_changed((struct zmk_key_stats_changed) {
        .total = total_count,
        .today = today_count,
    });

    if (today_count == 1U || (today_count % CONFIG_ZMK_KEY_STATS_SAVE_PRESS_INTERVAL) == 0U) {
        k_work_reschedule(&save_work, K_SECONDS(CONFIG_ZMK_KEY_STATS_SAVE_DEBOUNCE_SECONDS));
    }

    return ZMK_EV_EVENT_BUBBLE;
}

ZMK_LISTENER(key_stats, key_stats_listener_cb);
ZMK_SUBSCRIPTION(key_stats, zmk_position_state_changed);

static int key_stats_init(void) {
    k_work_init_delayable(&save_work, key_stats_save_work_cb);

#if IS_ENABLED(CONFIG_SETTINGS)
    int err = settings_register(&key_stats_settings_handler);
    if (err) {
        LOG_WRN("Failed to register key stats settings handler: %d", err);
    } else {
        settings_ready = true;
    }
#endif

    return 0;
}

SYS_INIT(key_stats_init, APPLICATION, CONFIG_APPLICATION_INIT_PRIORITY);
