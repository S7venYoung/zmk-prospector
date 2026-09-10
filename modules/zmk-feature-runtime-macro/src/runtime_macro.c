/*
 * Copyright (c) 2026 cormoran
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/modifiers.h>
#include <cormoran/zmk/custom_settings.h>
#include <cormoran/zmk/runtime_macro.h>
#include <zmk/behavior.h>
#include <zmk/behavior_queue.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#define RUNTIME_MACRO_WAIT_BEHAVIOR_NAME "runtime_macro_wait"

struct runtime_macro_queue_item {
    bool is_delay;
    bool is_tap;
    bool use_tap_ms;
    bool pressed;
    uint32_t delay_ms;
    struct zmk_behavior_binding binding;
};

struct runtime_macro_player {
    struct k_mutex lock;
    struct k_work play_work;
    struct k_work_delayable clear_active_work;
    bool active;
    uint32_t slot;
    struct zmk_behavior_binding_event event;
    uint8_t encoded[CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES];
    struct runtime_macro_queue_item items[CONFIG_ZMK_RUNTIME_MACRO_QUEUE_SIZE];
};

static void runtime_macro_play_work_handler(struct k_work *work);
static void runtime_macro_clear_active_work_handler(struct k_work *work);
static int queue_runtime_macro(const struct runtime_macro_queue_item *items, size_t count,
                               const struct zmk_behavior_binding_event *event,
                               uint32_t *queued_duration_ms);

static struct runtime_macro_player player = {
    .lock = Z_MUTEX_INITIALIZER(player.lock),
    .play_work = Z_WORK_INITIALIZER(runtime_macro_play_work_handler),
    .clear_active_work = Z_WORK_DELAYABLE_INITIALIZER(runtime_macro_clear_active_work_handler),
};

static int on_runtime_macro_wait_pressed(struct zmk_behavior_binding *binding,
                                         struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_runtime_macro_wait_released(struct zmk_behavior_binding *binding,
                                          struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api runtime_macro_wait_driver_api = {
    .binding_pressed = on_runtime_macro_wait_pressed,
    .binding_released = on_runtime_macro_wait_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .get_parameter_metadata = zmk_behavior_get_empty_param_metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

DEVICE_DEFINE(runtime_macro_wait_behavior, RUNTIME_MACRO_WAIT_BEHAVIOR_NAME, NULL, NULL, NULL, NULL,
              POST_KERNEL, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT, &runtime_macro_wait_driver_api);

static const STRUCT_SECTION_ITERABLE(zmk_behavior_ref, runtime_macro_wait_behavior_ref) = {
    .device = DEVICE_GET(runtime_macro_wait_behavior),
};

BUILD_ASSERT(CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES <= CONFIG_ZMK_CUSTOM_SETTINGS_LARGE_VALUE_MAX_SIZE,
             "ZMK_RUNTIME_MACRO_MAX_BYTES must not exceed "
             "ZMK_CUSTOM_SETTINGS_LARGE_VALUE_MAX_SIZE");
BUILD_ASSERT(CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES <= CONFIG_ZMK_RUNTIME_MACRO_POOL_BYTES,
             "ZMK_RUNTIME_MACRO_MAX_BYTES must not exceed ZMK_RUNTIME_MACRO_POOL_BYTES");

/* A macro's whole identity+content is one runtime_macros keyspace entry:
 * "macro/<name>" -> encoded body. max_key_len leaves room for the "macro/"
 * prefix, up to NAME_MAX_LEN name bytes, and the NUL terminator. The pool
 * budget is the module's own POOL_BYTES config (not the worst-case
 * COUNT * (max_key_len + MAX_BYTES)) - see
 * ZMK_CUSTOM_SETTING_KEYSPACE_DEFINE_WITH_POOL_SIZE - so a handful of large
 * macros or many small ones share one budget, matching the pre-migration
 * shared-pool property (docs/design/large-macros-shared-pool.md). */
#define RUNTIME_MACRO_MAX_KEY_LEN                                                                  \
    (sizeof(ZMK_RUNTIME_MACRO_KEY_PREFIX) - 1 + CONFIG_ZMK_RUNTIME_MACRO_NAME_MAX_LEN + 1)

ZMK_CUSTOM_SETTING_KEYSPACE_DEFINE_WITH_POOL_SIZE(
    runtime_macros, ZMK_RUNTIME_MACRO_SUBSYSTEM_ID, ZMK_RUNTIME_MACRO_KEY_PREFIX,
    ZMK_CUSTOM_SETTING_VALUE_TYPE_BYTES, CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES,
    RUNTIME_MACRO_MAX_KEY_LEN, CONFIG_ZMK_RUNTIME_MACRO_COUNT, CONFIG_ZMK_RUNTIME_MACRO_POOL_BYTES,
    ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC, ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
    ZMK_CUSTOM_SETTING_PERMISSION_SECURE, ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

ZMK_CUSTOM_SETTING_DEFINE(runtime_macro_tap_ms, ZMK_RUNTIME_MACRO_SUBSYSTEM_ID,
                          ZMK_RUNTIME_MACRO_TAP_MS_KEY, ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32,
                          ZMK_CUSTOM_SETTING_VALUE_INT32(CONFIG_ZMK_RUNTIME_MACRO_DEFAULT_TAP_MS),
                          ZMK_CUSTOM_SETTING_CONFIDENTIALITY_RPC_PUBLIC,
                          ZMK_CUSTOM_SETTING_PERMISSION_UNSECURE,
                          ZMK_CUSTOM_SETTING_PERMISSION_SECURE, ZMK_CUSTOM_SETTING_NO_CONSTRAINT);

#define RUNTIME_MACRO_PACKED_KEY_LSFT BIT(7)
#define RUNTIME_MACRO_PACKED_KEY_USAGE_MASK 0x7f
#define RUNTIME_MACRO_PACKED_KEY_MIN_USAGE 0x04
#define RUNTIME_MACRO_PACKED_KEY_MAX_USAGE 0x38

int zmk_runtime_macro_pack_key_tap(uint32_t keycode, uint8_t *packed_key) {
    uint32_t mods = SELECT_MODS(keycode);
    if ((mods & ~MOD_LSFT) != 0) {
        return -EINVAL;
    }

    uint32_t usage_keycode = STRIP_MODS(keycode);
    if (ZMK_HID_USAGE_PAGE(usage_keycode) != HID_USAGE_KEY) {
        return -EINVAL;
    }

    uint32_t usage = ZMK_HID_USAGE_ID(usage_keycode);
    if (usage < RUNTIME_MACRO_PACKED_KEY_MIN_USAGE || usage > RUNTIME_MACRO_PACKED_KEY_MAX_USAGE) {
        return -EINVAL;
    }

    *packed_key = usage | (mods == MOD_LSFT ? RUNTIME_MACRO_PACKED_KEY_LSFT : 0);
    return 0;
}

int zmk_runtime_macro_unpack_key_tap(uint8_t packed_key, uint32_t *keycode) {
    uint8_t usage = packed_key & RUNTIME_MACRO_PACKED_KEY_USAGE_MASK;
    if (usage < RUNTIME_MACRO_PACKED_KEY_MIN_USAGE || usage > RUNTIME_MACRO_PACKED_KEY_MAX_USAGE) {
        return -EINVAL;
    }

    uint32_t unpacked = ZMK_HID_USAGE(HID_USAGE_KEY, usage);
    *keycode = (packed_key & RUNTIME_MACRO_PACKED_KEY_LSFT) ? LS((unpacked)) : unpacked;
    return 0;
}

static int read_uvar(const uint8_t *encoded, size_t size, size_t *offset, uint32_t *value) {
    uint32_t result = 0;
    uint8_t shift = 0;

    while (*offset < size && shift <= 28) {
        uint8_t byte = encoded[(*offset)++];
        if (shift == 28 && (byte & 0xf0) != 0) {
            return -EINVAL;
        }
        result |= (uint32_t)(byte & 0x7f) << shift;

        if ((byte & 0x80) == 0) {
            *value = result;
            return 0;
        }

        shift += 7;
    }

    return -EINVAL;
}

static int read_binding(const uint8_t *encoded, size_t size, size_t *offset,
                        struct zmk_behavior_binding *binding) {
    uint32_t behavior_id;
    int ret = read_uvar(encoded, size, offset, &behavior_id);
    if (ret < 0) {
        return ret;
    }
    if (behavior_id > UINT16_MAX) {
        return -ERANGE;
    }

    const char *behavior_name =
        zmk_behavior_find_behavior_name_from_local_id((zmk_behavior_local_id_t)behavior_id);
    if (!behavior_name) {
        return -ENODEV;
    }

    uint32_t param1;
    uint32_t param2;
    ret = read_uvar(encoded, size, offset, &param1);
    if (ret < 0) {
        return ret;
    }
    ret = read_uvar(encoded, size, offset, &param2);
    if (ret < 0) {
        return ret;
    }

    *binding = (struct zmk_behavior_binding){
        .behavior_dev = behavior_name,
        .param1 = param1,
        .param2 = param2,
    };

    return zmk_behavior_validate_binding(binding);
}

static int append_item(struct runtime_macro_queue_item *items, size_t *count,
                       struct runtime_macro_queue_item item) {
    if (*count >= CONFIG_ZMK_RUNTIME_MACRO_QUEUE_SIZE) {
        return -ENOSPC;
    }

    if (items) {
        items[*count] = item;
    }
    (*count)++;
    return 0;
}

static int append_tap_item(struct runtime_macro_queue_item *items, size_t *count,
                           struct zmk_behavior_binding binding) {
    return append_item(items, count,
                       (struct runtime_macro_queue_item){
                           .is_tap = true,
                           .binding = binding,
                       });
}

static int read_key_tap_sequence(const uint8_t *encoded, size_t size, size_t *offset,
                                 struct runtime_macro_queue_item *items, size_t *count) {
    uint32_t sequence_size;
    int ret = read_uvar(encoded, size, offset, &sequence_size);
    if (ret < 0) {
        return ret;
    }
    if (sequence_size > size - *offset) {
        return -EINVAL;
    }

    zmk_behavior_local_id_t kp_id = zmk_behavior_get_local_id(DEVICE_DT_NAME(DT_NODELABEL(kp)));
    if (kp_id == UINT16_MAX) {
        return -ENODEV;
    }

    const char *kp_name =
        zmk_behavior_find_behavior_name_from_local_id((zmk_behavior_local_id_t)kp_id);
    if (!kp_name) {
        return -ENODEV;
    }

    for (uint32_t i = 0; i < sequence_size; i++) {
        uint32_t keycode;
        ret = zmk_runtime_macro_unpack_key_tap(encoded[*offset + i], &keycode);
        if (ret < 0) {
            return ret;
        }

        struct zmk_behavior_binding binding = {
            .behavior_dev = kp_name,
            .param1 = keycode,
            .param2 = 0,
        };

        ret = zmk_behavior_validate_binding(&binding);
        if (ret < 0) {
            return ret;
        }

        ret = append_tap_item(items, count, binding);
        if (ret < 0) {
            return ret;
        }
    }

    *offset += sequence_size;
    return 0;
}

static int decode_macro(const uint8_t *encoded, size_t size, struct runtime_macro_queue_item *items,
                        size_t *count) {
    *count = 0;

    if (size == 0) {
        return 0;
    }

    if (encoded[0] != ZMK_RUNTIME_MACRO_FORMAT_VERSION) {
        return -EINVAL;
    }

    size_t offset = 1;

    while (offset < size) {
        uint8_t opcode = encoded[offset++];

        if (opcode == ZMK_RUNTIME_MACRO_OP_DELAY) {
            uint32_t delay_ms;
            int ret = read_uvar(encoded, size, &offset, &delay_ms);
            if (ret < 0) {
                return ret;
            }

            ret = append_item(items, count,
                              (struct runtime_macro_queue_item){
                                  .is_delay = true,
                                  .delay_ms = delay_ms,
                              });
            if (ret < 0) {
                return ret;
            }
            continue;
        }

        if (opcode == ZMK_RUNTIME_MACRO_OP_KEY_TAP_SEQUENCE) {
            int ret = read_key_tap_sequence(encoded, size, &offset, items, count);
            if (ret < 0) {
                return ret;
            }
            continue;
        }

        struct zmk_behavior_binding binding;
        int ret = read_binding(encoded, size, &offset, &binding);
        if (ret < 0) {
            return ret;
        }

        switch (opcode) {
        case ZMK_RUNTIME_MACRO_OP_DOWN:
            ret = append_item(items, count,
                              (struct runtime_macro_queue_item){
                                  .binding = binding,
                                  .pressed = true,
                              });
            break;
        case ZMK_RUNTIME_MACRO_OP_UP:
            ret = append_item(items, count,
                              (struct runtime_macro_queue_item){
                                  .binding = binding,
                                  .pressed = false,
                              });
            break;
        case ZMK_RUNTIME_MACRO_OP_TAP:
            ret = append_tap_item(items, count, binding);
            break;
        default:
            return -EINVAL;
        }

        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

static uint32_t get_runtime_macro_tap_ms(void) {
    struct zmk_custom_setting_value value;
    int ret = zmk_custom_setting_read_by_key(ZMK_RUNTIME_MACRO_SUBSYSTEM_ID,
                                             ZMK_RUNTIME_MACRO_TAP_MS_KEY, &value);
    if (ret < 0 || value.type != ZMK_CUSTOM_SETTING_VALUE_TYPE_INT32 || value.int32_value < 0) {
        return CONFIG_ZMK_RUNTIME_MACRO_DEFAULT_TAP_MS;
    }

    return MIN(value.int32_value, 10000);
}

/* Build the keyspace key "macro/<name>" into a caller-owned scratch buffer. */
static int build_key(const char *name, char *out, size_t out_size) {
    if (!name) {
        return -EINVAL;
    }

    int n = snprintf(out, out_size, "%s%s", ZMK_RUNTIME_MACRO_KEY_PREFIX, name);
    if (n < 0 || (size_t)n >= out_size) {
        return -ENAMETOOLONG;
    }

    return 0;
}

/* The keyspace slot array is a fixed, address-stable static allocation (see
 * ZMK_CUSTOM_SETTING_KEYSPACE_DEFINE), so a pointer into it never moves -
 * this scan is only ever used to translate an already-resolved descriptor
 * back to its slot number (e.g. right after create()). */
static int slot_index_of(const struct zmk_custom_setting *setting) {
    for (uint32_t i = 0; i < CONFIG_ZMK_RUNTIME_MACRO_COUNT; i++) {
        if (&runtime_macros.slots[i].setting == setting) {
            return (int)i;
        }
    }
    return -ENOENT;
}

/* Strip the keyspace's own "macro/" prefix off a slot's public (storage) key
 * to recover the bare user-facing name. */
static const char *strip_prefix(const char *public_key) {
    size_t prefix_len = sizeof(ZMK_RUNTIME_MACRO_KEY_PREFIX) - 1;
    if (strncmp(public_key, ZMK_RUNTIME_MACRO_KEY_PREFIX, prefix_len) == 0) {
        return public_key + prefix_len;
    }
    return public_key;
}

static void runtime_macro_clear_active_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    k_mutex_lock(&player.lock, K_FOREVER);
    player.active = false;
    k_mutex_unlock(&player.lock);
}

static int runtime_macro_start_playback(uint32_t slot,
                                        const struct zmk_behavior_binding_event *event) {
    k_mutex_lock(&player.lock, K_FOREVER);
    if (player.active) {
        k_mutex_unlock(&player.lock);
        return -EBUSY;
    }

    k_work_cancel_delayable(&player.clear_active_work);
    player.slot = slot;
    player.event = *event;
    player.active = true;
    k_mutex_unlock(&player.lock);

    int ret = k_work_submit(&player.play_work);
    if (ret < 0) {
        runtime_macro_clear_active_work_handler(&player.clear_active_work.work);
        return ret;
    }

    return 0;
}

static void runtime_macro_finish_playback(uint32_t active_ms) {
    if (active_ms == 0) {
        runtime_macro_clear_active_work_handler(&player.clear_active_work.work);
        return;
    }

    uint32_t clear_delay_ms = active_ms == UINT32_MAX ? UINT32_MAX : active_ms + 1;
    k_work_schedule(&player.clear_active_work, K_MSEC(clear_delay_ms));
}

static void runtime_macro_play_work_handler(struct k_work *work) {
    ARG_UNUSED(work);

    k_mutex_lock(&player.lock, K_FOREVER);
    uint32_t slot = player.slot;
    struct zmk_behavior_binding_event event = player.event;
    k_mutex_unlock(&player.lock);

    if (!runtime_macros.slots[slot].in_use) {
        /* The slot was deleted between the keypress and this deferred work
         * running - nothing to play. */
        LOG_DBG("Runtime macro slot %u is empty at playback time", slot);
        runtime_macro_finish_playback(0);
        return;
    }

    size_t body_size = 0;
    int ret = zmk_custom_setting_read_into(&runtime_macros.slots[slot].setting, player.encoded,
                                           sizeof(player.encoded), &body_size, NULL);
    if (ret < 0) {
        LOG_WRN("Runtime macro slot %u body read failed: %d", slot, ret);
        runtime_macro_finish_playback(0);
        return;
    }

    size_t count = 0;
    ret = decode_macro(player.encoded, body_size, player.items, &count);
    if (ret < 0) {
        LOG_WRN("Runtime macro slot %u decode failed: %d", slot, ret);
        runtime_macro_finish_playback(0);
        return;
    }

    uint32_t queued_duration_ms = 0;
    ret = queue_runtime_macro(player.items, count, &event, &queued_duration_ms);
    if (ret < 0) {
        LOG_WRN("Runtime macro slot %u queue failed: %d", slot, ret);
        runtime_macro_finish_playback(0);
        return;
    }

    runtime_macro_finish_playback(queued_duration_ms);
}

static int queue_behavior(const struct zmk_behavior_binding_event *event,
                          const struct zmk_behavior_binding *binding, bool pressed,
                          uint32_t wait_ms) {
    int ret = zmk_behavior_queue_add(event, *binding, pressed, wait_ms);
    if (ret < 0) {
        LOG_WRN("Runtime macro behavior queue add failed: %d", ret);
    }

    return ret;
}

static void add_saturating_delay(uint32_t *total_ms, uint32_t delay_ms) {
    if (UINT32_MAX - *total_ms < delay_ms) {
        *total_ms = UINT32_MAX;
    } else {
        *total_ms += delay_ms;
    }
}

static int queue_runtime_macro(const struct runtime_macro_queue_item *items, size_t count,
                               const struct zmk_behavior_binding_event *event,
                               uint32_t *queued_duration_ms) {
    uint32_t pending_delay_ms = 0;
    *queued_duration_ms = 0;

    struct zmk_behavior_binding none_binding = {
        .behavior_dev = RUNTIME_MACRO_WAIT_BEHAVIOR_NAME,
    };

    for (size_t i = 0; i < count; i++) {
        const struct runtime_macro_queue_item *item = &items[i];

        if (item->is_delay) {
            uint32_t delay_ms = item->use_tap_ms ? get_runtime_macro_tap_ms() : item->delay_ms;
            add_saturating_delay(&pending_delay_ms, delay_ms);
            continue;
        }

        if (pending_delay_ms > 0) {
            int ret = queue_behavior(event, &none_binding, true, pending_delay_ms);
            if (ret < 0) {
                return ret;
            }
            add_saturating_delay(queued_duration_ms, pending_delay_ms);
            pending_delay_ms = 0;
        }

        if (item->is_tap) {
            uint32_t tap_ms = get_runtime_macro_tap_ms();
            int ret = queue_behavior(event, &item->binding, true, tap_ms);
            if (ret < 0) {
                return ret;
            }
            add_saturating_delay(queued_duration_ms, tap_ms);

            uint32_t wait_ms = 0;
            if (i + 1 < count && items[i + 1].is_delay) {
                wait_ms =
                    items[i + 1].use_tap_ms ? get_runtime_macro_tap_ms() : items[i + 1].delay_ms;
                i++;
            }

            ret = queue_behavior(event, &item->binding, false, wait_ms);
            if (ret < 0) {
                return ret;
            }
            add_saturating_delay(queued_duration_ms, wait_ms);
            continue;
        }

        uint32_t wait_ms = 0;
        if (i + 1 < count && items[i + 1].is_delay) {
            wait_ms = items[i + 1].use_tap_ms ? get_runtime_macro_tap_ms() : items[i + 1].delay_ms;
            i++;
        }

        int ret = queue_behavior(event, &item->binding, item->pressed, wait_ms);
        if (ret < 0) {
            return ret;
        }
        add_saturating_delay(queued_duration_ms, wait_ms);
    }

    if (pending_delay_ms > 0) {
        int ret = queue_behavior(event, &none_binding, true, pending_delay_ms);
        if (ret < 0) {
            return ret;
        }
        add_saturating_delay(queued_duration_ms, pending_delay_ms);
    }

    return 0;
}

int zmk_runtime_macro_validate_encoded(const uint8_t *encoded, size_t size) {
    size_t count;

    return decode_macro(encoded, size, NULL, &count);
}

int zmk_runtime_macro_play(uint32_t slot, const struct zmk_behavior_binding_event *event) {
    if (slot >= CONFIG_ZMK_RUNTIME_MACRO_COUNT) {
        return -ERANGE;
    }

    if (!runtime_macros.slots[slot].in_use) {
        /* A deep-sleep wake is a fresh boot, but on some split configurations the
         * custom-settings keyspace can still be empty when the first relayed key
         * event reaches the central. Reload only the runtime-macro subtree here;
         * this one-shot recovery path leaves unrelated DYA settings untouched. */
        int ret = settings_load_subtree("custom_settings/" ZMK_RUNTIME_MACRO_SUBSYSTEM_ID);
        if (ret < 0) {
            LOG_WRN("Runtime macro settings reload failed: %d", ret);
        }

        if (!runtime_macros.slots[slot].in_use) {
            LOG_DBG("Runtime macro slot %u is empty", slot);
            return 0;
        }
    }

    return runtime_macro_start_playback(slot, event);
}

int zmk_runtime_macro_create(const char *name, const uint8_t *encoded, size_t encoded_size,
                             enum zmk_custom_setting_write_mode mode, uint32_t *out_slot) {
    if (!name || name[0] == '\0') {
        return -EINVAL;
    }

    int ret = zmk_runtime_macro_validate_encoded(encoded, encoded_size);
    if (ret < 0) {
        return ret;
    }
    if (encoded_size > CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES) {
        return -EMSGSIZE;
    }

    char key[RUNTIME_MACRO_MAX_KEY_LEN];
    ret = build_key(name, key, sizeof(key));
    if (ret < 0) {
        return ret;
    }

    /* Create with an empty body first (fits any keyspace's small-value
     * carrier unconditionally), then grow it with write_bytes exactly like
     * any other pooled BYTES setting - mirrors the pattern
     * zmk-feature-custom-settings' own keyspace tests use for large
     * entries. */
    struct zmk_custom_setting_value initial_value = {
        .type = ZMK_CUSTOM_SETTING_VALUE_TYPE_BYTES,
        .size = 0,
    };
    const struct zmk_custom_setting *created = NULL;
    ret = zmk_custom_setting_keyspace_create(&runtime_macros, key, &initial_value, mode, &created);
    if (ret < 0) {
        return ret;
    }

    if (encoded_size > 0) {
        ret = zmk_custom_setting_write_bytes(created, encoded, encoded_size, mode);
        if (ret < 0) {
            /* Don't leave a stray empty macro behind on a failed body write
             * (e.g. -ENOSPC from a full pool). */
            zmk_custom_setting_keyspace_delete(&runtime_macros, key);
            return ret;
        }
    }

    if (out_slot) {
        int slot = slot_index_of(created);
        *out_slot = slot >= 0 ? (uint32_t)slot : 0;
    }

    return 0;
}

int zmk_runtime_macro_delete(const char *name) {
    char key[RUNTIME_MACRO_MAX_KEY_LEN];
    int ret = build_key(name, key, sizeof(key));
    if (ret < 0) {
        return ret;
    }

    return zmk_custom_setting_keyspace_delete(&runtime_macros, key);
}

int zmk_runtime_macro_write(const char *name, const uint8_t *encoded, size_t encoded_size,
                            enum zmk_custom_setting_write_mode mode) {
    int ret = zmk_runtime_macro_validate_encoded(encoded, encoded_size);
    if (ret < 0) {
        return ret;
    }
    if (encoded_size > CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES) {
        return -EMSGSIZE;
    }

    char key[RUNTIME_MACRO_MAX_KEY_LEN];
    ret = build_key(name, key, sizeof(key));
    if (ret < 0) {
        return ret;
    }

    const struct zmk_custom_setting *setting =
        zmk_custom_setting_keyspace_find(&runtime_macros, key);
    if (!setting) {
        return -ENOENT;
    }

    /* -ENOSPC here means the shared name+body pool
     * (CONFIG_ZMK_RUNTIME_MACRO_POOL_BYTES) is exhausted; propagate it
     * unchanged so the Studio RPC handler can surface a clear "pool full"
     * message. */
    return zmk_custom_setting_write_bytes(setting, encoded, encoded_size, mode);
}

int zmk_runtime_macro_read(const char *name, uint8_t *encoded, size_t encoded_capacity,
                           size_t *encoded_size) {
    char key[RUNTIME_MACRO_MAX_KEY_LEN];
    int ret = build_key(name, key, sizeof(key));
    if (ret < 0) {
        return ret;
    }

    const struct zmk_custom_setting *setting =
        zmk_custom_setting_keyspace_find(&runtime_macros, key);
    if (!setting) {
        return -ENOENT;
    }

    size_t body_size = 0;
    ret = zmk_custom_setting_read_into(setting, encoded, encoded_capacity, &body_size, NULL);
    if (ret < 0) {
        return ret;
    }
    if (encoded_size) {
        *encoded_size = body_size;
    }

    return 0;
}

int zmk_runtime_macro_rename(const char *old_name, const char *new_name,
                             enum zmk_custom_setting_write_mode mode) {
    char old_key[RUNTIME_MACRO_MAX_KEY_LEN];
    int ret = build_key(old_name, old_key, sizeof(old_key));
    if (ret < 0) {
        return ret;
    }

    const struct zmk_custom_setting *existing =
        zmk_custom_setting_keyspace_find(&runtime_macros, old_key);
    if (!existing) {
        return -ENOENT;
    }

    uint8_t encoded[CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES];
    size_t encoded_size = 0;
    ret = zmk_custom_setting_read_into(existing, encoded, sizeof(encoded), &encoded_size, NULL);
    if (ret < 0) {
        return ret;
    }

    ret = zmk_runtime_macro_create(new_name, encoded, encoded_size, mode, NULL);
    if (ret < 0) {
        return ret;
    }

    return zmk_custom_setting_keyspace_delete(&runtime_macros, old_key);
}

int zmk_runtime_macro_name_for_slot(uint32_t slot, char *name, size_t name_capacity) {
    if (slot >= CONFIG_ZMK_RUNTIME_MACRO_COUNT) {
        return -ERANGE;
    }
    if (!runtime_macros.slots[slot].in_use) {
        return -ENOENT;
    }

    const char *public_key = zmk_custom_setting_public_key(&runtime_macros.slots[slot].setting);
    snprintf(name, name_capacity, "%s", strip_prefix(public_key));
    return 0;
}

int zmk_runtime_macro_slot_for_name(const char *name, uint32_t *out_slot) {
    char key[RUNTIME_MACRO_MAX_KEY_LEN];
    int ret = build_key(name, key, sizeof(key));
    if (ret < 0) {
        return ret;
    }

    const struct zmk_custom_setting *setting =
        zmk_custom_setting_keyspace_find(&runtime_macros, key);
    if (!setting) {
        return -ENOENT;
    }

    int slot = slot_index_of(setting);
    if (slot < 0) {
        return -ENOENT;
    }
    if (out_slot) {
        *out_slot = (uint32_t)slot;
    }
    return 0;
}

size_t zmk_runtime_macro_pool_total(void) { return CONFIG_ZMK_RUNTIME_MACRO_POOL_BYTES; }

size_t zmk_runtime_macro_pool_used(void) {
    return zmk_custom_setting_large_pool_used(runtime_macros.large_pool);
}

int zmk_runtime_macro_for_each(zmk_runtime_macro_iter_cb_t cb, void *user_data) {
    for (uint32_t slot = 0; slot < CONFIG_ZMK_RUNTIME_MACRO_COUNT; slot++) {
        if (!runtime_macros.slots[slot].in_use) {
            continue;
        }

        char name[CONFIG_ZMK_RUNTIME_MACRO_NAME_MAX_LEN + 1];
        int ret = zmk_runtime_macro_name_for_slot(slot, name, sizeof(name));
        if (ret < 0) {
            continue;
        }

        size_t encoded_size = 0;
        zmk_custom_setting_value_size(&runtime_macros.slots[slot].setting, &encoded_size);

        bool has_unsaved_changes =
            zmk_custom_setting_has_unsaved_value(&runtime_macros.slots[slot].setting);

        ret = cb(slot, name, encoded_size, has_unsaved_changes, user_data);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}
