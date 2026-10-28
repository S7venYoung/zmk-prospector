/*
 * Copyright (c) 2024 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <zephyr/kernel.h>
#include <zmk/status_scanner.h>
#include <zmk/status_advertisement.h>

/**
 * @brief Liveness hook called at the start of every process_work run
 *
 * Weak no-op in the core. A shield may provide a strong definition to feed
 * a watchdog channel from the system-workqueue processing path.
 */
void scanner_core_process_alive(void);

#ifndef MAX_NAME_LEN
#define MAX_NAME_LEN 32
#endif

/**
 * @brief Data handed from the processing workqueue to the display thread
 *
 * SINGLE SOURCE OF TRUTH - do not redefine this struct in a .c file.
 * Historically scanner_stub.c and custom_status_screen.c each carried
 * their own copy and the two drifted apart (scanner_stub.c grew the
 * kb_version fields): scanner_get_pending_update()'s bulk copy was sized
 * by the larger definition and overwrote the caller's smaller stack
 * variable - stack corruption on the display thread every update.
 */
struct pending_display_data {
    volatile bool update_pending;
    volatile bool signal_update_pending;  /* Signal widget updates separately (1Hz) */
    volatile bool no_keyboards;           /* True when all keyboards timed out */

    char device_name[MAX_NAME_LEN];
    char layer_name[4];
    int layer;
    int wpm;
    bool usb_ready;
    bool ble_connected;
    bool ble_bonded;
    int profile;
    uint8_t modifiers;
    int bat[4];
    int8_t rssi;
    float rate_hz;
    int scanner_battery;
    bool scanner_battery_pending;

    /* Keyboard firmware version (decoded from version + profile_slot fields) */
    uint8_t kb_version_major;
    uint8_t kb_version_minor;
    uint8_t kb_version_patch;
    bool kb_version_dev;
    bool kb_version_valid;       /* True after first keyboard data received */
};

/**
 * @brief Consume the pending display update, if any (display thread only)
 *
 * @param out Destination for a consistent snapshot (copied under mutex)
 * @return true if an update was pending and copied
 */
bool scanner_get_pending_update(struct pending_display_data *out);

/**
 * @brief Consume the pending signal-widget update flag (display thread only)
 */
bool scanner_is_signal_pending(void);

/**
 * @brief Consume the pending scanner battery update, if any
 */
bool scanner_get_pending_battery(int *level);

/**
 * @brief Get last received keyboard firmware version
 */
bool scanner_get_kb_version(uint8_t *major, uint8_t *minor, uint8_t *patch,
                            bool *is_dev, char *name, size_t name_len);

/* Signal data written by the processing workqueue, read by display thread */
extern volatile int8_t scanner_signal_rssi;
extern volatile int32_t scanner_signal_rate_x100;  /* rate * 100 */

/**
 * @brief Send keyboard data received from BLE advertisement
 *
 * Lock-free: pushes data into SPSC ring buffer.
 * Called from BT RX thread. Data is processed by scanner_process_incoming()
 * in the LVGL timer context.
 *
 * @param adv_data Parsed advertisement data
 * @param rssi Signal strength
 * @param device_name Keyboard device name
 * @param ble_addr BLE MAC address (6 bytes)
 * @param ble_addr_type BLE address type
 * @return 0 on success, negative error code on failure
 */
int scanner_msg_send_keyboard_data(const struct zmk_status_adv_data *adv_data,
                                   int8_t rssi, const char *device_name,
                                   const uint8_t *ble_addr, uint8_t ble_addr_type);

/**
 * @brief Process incoming advertisements from ring buffer
 *
 * Must be called from LVGL timer context (main thread).
 * Drains ring buffer, manages keyboards[], calculates rates,
 * checks timeouts, and updates scanner battery.
 */
void scanner_process_incoming(void);

/**
 * @brief Trigger timeout check for keyboards
 *
 * Checks if any keyboards have timed out and updates display accordingly.
 *
 * @return 0 on success, negative error code on failure
 */
int scanner_msg_send_timeout_check(void);

/**
 * @brief Get keyboard data by index
 *
 * @param index Keyboard index
 * @param data Output: advertisement data
 * @param rssi Output: signal strength
 * @param name Output: keyboard name
 * @param name_len Size of name buffer
 * @return true if keyboard found, false otherwise
 */
bool scanner_get_keyboard_data(int index, struct zmk_status_adv_data *data,
                               int8_t *rssi, char *name, size_t name_len);

/**
 * @brief Get the count of active keyboards
 *
 * @return Number of active keyboards
 */
int scanner_get_active_keyboard_count(void);

/**
 * @brief Copy keyboard status by index (thread-safe snapshot)
 *
 * Copies the slot under the data mutex. keyboards[] is mutated on the
 * system workqueue while the display runs on its own thread, so callers
 * must never hold a pointer into the array - always use this copy API.
 *
 * @param index Keyboard index (0 to MAX_KEYBOARDS-1)
 * @param out Destination for the snapshot
 * @return true if the slot is active and copied, false otherwise
 */
bool scanner_copy_keyboard_status(int index, struct zmk_keyboard_status *out);

/**
 * @brief Get the selected keyboard index
 *
 * @return Selected keyboard index
 */
int scanner_get_selected_keyboard(void);

/**
 * @brief Set the selected keyboard index
 *
 * @param index Keyboard index to select
 */
int scanner_set_selected_keyboard(int index); /* 0 on success, -EBUSY if the core was busy */

/**
 * @brief Send display refresh request
 *
 * Triggers immediate display update after screen transitions to MAIN.
 *
 * @return 0 on success, negative error code on failure
 */
int scanner_msg_send_display_refresh(void);
