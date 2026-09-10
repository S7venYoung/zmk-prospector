/*
 * Copyright (c) 2026 cormoran
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT cormoran_runtime_macro_default

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/settings/settings.h>
#include <zephyr/sys/util.h>

#include <drivers/behavior.h>
#include <dt-bindings/zmk/hid_usage.h>
#include <dt-bindings/zmk/hid_usage_pages.h>
#include <dt-bindings/zmk/modifiers.h>
#include <cormoran/zmk/custom_settings.h>
#include <cormoran/zmk/runtime_macro.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT)

#define RUNTIME_MACRO_DEFAULT_ASSERT_HAS_CONTENT(n)                                                \
    BUILD_ASSERT(DT_NODE_HAS_PROP(n, text) || DT_NODE_HAS_PROP(n, bindings),                       \
                 "cormoran,runtime-macro-default node must set `text` and/or `bindings`");

DT_FOREACH_STATUS_OKAY(cormoran_runtime_macro_default, RUNTIME_MACRO_DEFAULT_ASSERT_HAS_CONTENT)

/* &macro_tap / &macro_press / &macro_release / &macro_wait_time and &kp are singleton control
 * behaviors from ZMK's behaviors.dtsi. Resolve their device names once, guarded so this file still
 * compiles for keymaps that reference none of them (no `bindings` default at all). */
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_macro_control_mode_tap)
#define RUNTIME_MACRO_DEFAULT_TAP_MODE_NAME DEVICE_DT_NAME(DT_INST(0, zmk_macro_control_mode_tap))
#else
#define RUNTIME_MACRO_DEFAULT_TAP_MODE_NAME NULL
#endif
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_macro_control_mode_press)
#define RUNTIME_MACRO_DEFAULT_PRESS_MODE_NAME                                                      \
    DEVICE_DT_NAME(DT_INST(0, zmk_macro_control_mode_press))
#else
#define RUNTIME_MACRO_DEFAULT_PRESS_MODE_NAME NULL
#endif
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_macro_control_mode_release)
#define RUNTIME_MACRO_DEFAULT_RELEASE_MODE_NAME                                                    \
    DEVICE_DT_NAME(DT_INST(0, zmk_macro_control_mode_release))
#else
#define RUNTIME_MACRO_DEFAULT_RELEASE_MODE_NAME NULL
#endif
#if DT_HAS_COMPAT_STATUS_OKAY(zmk_macro_control_wait_time)
#define RUNTIME_MACRO_DEFAULT_WAIT_TIME_NAME DEVICE_DT_NAME(DT_INST(0, zmk_macro_control_wait_time))
#else
#define RUNTIME_MACRO_DEFAULT_WAIT_TIME_NAME NULL
#endif
#if DT_NODE_EXISTS(DT_NODELABEL(kp))
#define RUNTIME_MACRO_DEFAULT_KP_NAME DEVICE_DT_NAME(DT_NODELABEL(kp))
#else
#define RUNTIME_MACRO_DEFAULT_KP_NAME NULL
#endif

struct runtime_macro_default_config {
    const char *name;
    const char *text;
    uint32_t wait_ms;
    const struct zmk_behavior_binding *bindings;
    uint32_t bindings_len;
};

#define TRANSFORMED_BEHAVIORS(n)                                                                   \
    {LISTIFY(DT_PROP_LEN(n, bindings), ZMK_KEYMAP_EXTRACT_BINDING, (, ), n)}

#define RUNTIME_MACRO_DEFAULT_BINDINGS_ARRAY(n)                                                    \
    COND_CODE_1(DT_NODE_HAS_PROP(n, bindings),                                                     \
                (static const struct zmk_behavior_binding runtime_macro_default_bindings_##n[] =   \
                     TRANSFORMED_BEHAVIORS(n);),                                                   \
                ())

#define RUNTIME_MACRO_DEFAULT_INST(n)                                                              \
    RUNTIME_MACRO_DEFAULT_BINDINGS_ARRAY(n)                                                        \
    static const struct runtime_macro_default_config runtime_macro_default_config_##n = {          \
        .name = DT_PROP_OR(n, macro_name, DT_NODE_FULL_NAME(n)),                                   \
        .text = DT_PROP_OR(n, text, NULL),                                                         \
        .wait_ms = DT_PROP_OR(n, wait_ms, 0),                                                      \
        .bindings = COND_CODE_1(DT_NODE_HAS_PROP(n, bindings),                                     \
                                (runtime_macro_default_bindings_##n), (NULL)),                     \
        .bindings_len = DT_PROP_LEN_OR(n, bindings, 0),                                            \
    };

DT_FOREACH_STATUS_OKAY(cormoran_runtime_macro_default, RUNTIME_MACRO_DEFAULT_INST)

#define RUNTIME_MACRO_DEFAULT_CONFIG_PTR(n) &runtime_macro_default_config_##n,

static const struct runtime_macro_default_config *runtime_macro_default_configs[] = {
    DT_FOREACH_STATUS_OKAY(cormoran_runtime_macro_default, RUNTIME_MACRO_DEFAULT_CONFIG_PTR)};

#define RUNTIME_MACRO_DEFAULT_COUNT ARRAY_SIZE(runtime_macro_default_configs)

enum runtime_macro_default_step_mode {
    RUNTIME_MACRO_DEFAULT_MODE_TAP,
    RUNTIME_MACRO_DEFAULT_MODE_PRESS,
    RUNTIME_MACRO_DEFAULT_MODE_RELEASE,
};

struct runtime_macro_encode_state {
    uint8_t *buf;
    size_t capacity;
    size_t size;
    uint8_t pending_packed[CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES];
    size_t pending_packed_count;
};

static int emit_byte(struct runtime_macro_encode_state *s, uint8_t byte) {
    if (s->size >= s->capacity) {
        return -ENOSPC;
    }
    s->buf[s->size++] = byte;
    return 0;
}

static int emit_uvar(struct runtime_macro_encode_state *s, uint32_t value) {
    do {
        uint8_t byte = value & 0x7f;
        value >>= 7;
        int ret = emit_byte(s, value != 0 ? (byte | 0x80) : byte);
        if (ret < 0) {
            return ret;
        }
    } while (value != 0);
    return 0;
}

static int flush_packed_sequence(struct runtime_macro_encode_state *s) {
    if (s->pending_packed_count == 0) {
        return 0;
    }

    int ret = emit_byte(s, ZMK_RUNTIME_MACRO_OP_KEY_TAP_SEQUENCE);
    if (ret < 0) {
        return ret;
    }
    ret = emit_uvar(s, s->pending_packed_count);
    if (ret < 0) {
        return ret;
    }
    for (size_t i = 0; i < s->pending_packed_count; i++) {
        ret = emit_byte(s, s->pending_packed[i]);
        if (ret < 0) {
            return ret;
        }
    }
    s->pending_packed_count = 0;
    return 0;
}

static int try_append_packed_tap(struct runtime_macro_encode_state *s, uint32_t keycode) {
    if (s->pending_packed_count >= sizeof(s->pending_packed)) {
        return -ENOSPC;
    }

    uint8_t packed;
    int ret = zmk_runtime_macro_pack_key_tap(keycode, &packed);
    if (ret < 0) {
        return ret;
    }

    s->pending_packed[s->pending_packed_count++] = packed;
    return 0;
}

static int emit_delay_step(struct runtime_macro_encode_state *s, uint32_t delay_ms) {
    int ret = flush_packed_sequence(s);
    if (ret < 0) {
        return ret;
    }
    ret = emit_byte(s, ZMK_RUNTIME_MACRO_OP_DELAY);
    if (ret < 0) {
        return ret;
    }
    return emit_uvar(s, delay_ms);
}

static int emit_binding_step(struct runtime_macro_encode_state *s, uint8_t opcode,
                             const struct zmk_behavior_binding *binding) {
    int ret = flush_packed_sequence(s);
    if (ret < 0) {
        return ret;
    }

    zmk_behavior_local_id_t id = zmk_behavior_get_local_id(binding->behavior_dev);
    if (id == UINT16_MAX) {
        LOG_ERR("Runtime macro default: unknown behavior %s", binding->behavior_dev);
        return -ENODEV;
    }

    ret = emit_byte(s, opcode);
    if (ret < 0) {
        return ret;
    }
    ret = emit_uvar(s, id);
    if (ret < 0) {
        return ret;
    }
    ret = emit_uvar(s, binding->param1);
    if (ret < 0) {
        return ret;
    }
    return emit_uvar(s, binding->param2);
}

/* US-layout ASCII -> (HID keyboard usage, left-shift) for the packable range (usages 0x04-0x38),
 * matching what the Web UI's packed key-tap sequence encoding assumes. */
static int ascii_char_to_usage(char c, uint32_t *usage, bool *shift) {
    *shift = false;

    if (c >= 'a' && c <= 'z') {
        *usage = HID_USAGE_KEY_KEYBOARD_A + (c - 'a');
        return 0;
    }
    if (c >= 'A' && c <= 'Z') {
        *usage = HID_USAGE_KEY_KEYBOARD_A + (c - 'A');
        *shift = true;
        return 0;
    }
    if (c >= '1' && c <= '9') {
        *usage = HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION + (c - '1');
        return 0;
    }

    switch (c) {
    case '0':
        *usage = HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS;
        return 0;
    case '\n':
        *usage = HID_USAGE_KEY_KEYBOARD_RETURN_ENTER;
        return 0;
    case '\t':
        *usage = HID_USAGE_KEY_KEYBOARD_TAB;
        return 0;
    case ' ':
        *usage = HID_USAGE_KEY_KEYBOARD_SPACEBAR;
        return 0;
    case '-':
        *usage = HID_USAGE_KEY_KEYBOARD_MINUS_AND_UNDERSCORE;
        return 0;
    case '_':
        *usage = HID_USAGE_KEY_KEYBOARD_MINUS_AND_UNDERSCORE;
        *shift = true;
        return 0;
    case '=':
        *usage = HID_USAGE_KEY_KEYBOARD_EQUAL_AND_PLUS;
        return 0;
    case '+':
        *usage = HID_USAGE_KEY_KEYBOARD_EQUAL_AND_PLUS;
        *shift = true;
        return 0;
    case '[':
        *usage = HID_USAGE_KEY_KEYBOARD_LEFT_BRACKET_AND_LEFT_BRACE;
        return 0;
    case '{':
        *usage = HID_USAGE_KEY_KEYBOARD_LEFT_BRACKET_AND_LEFT_BRACE;
        *shift = true;
        return 0;
    case ']':
        *usage = HID_USAGE_KEY_KEYBOARD_RIGHT_BRACKET_AND_RIGHT_BRACE;
        return 0;
    case '}':
        *usage = HID_USAGE_KEY_KEYBOARD_RIGHT_BRACKET_AND_RIGHT_BRACE;
        *shift = true;
        return 0;
    case '\\':
        *usage = HID_USAGE_KEY_KEYBOARD_BACKSLASH_AND_PIPE;
        return 0;
    case '|':
        *usage = HID_USAGE_KEY_KEYBOARD_BACKSLASH_AND_PIPE;
        *shift = true;
        return 0;
    case ';':
        *usage = HID_USAGE_KEY_KEYBOARD_SEMICOLON_AND_COLON;
        return 0;
    case ':':
        *usage = HID_USAGE_KEY_KEYBOARD_SEMICOLON_AND_COLON;
        *shift = true;
        return 0;
    case '\'':
        *usage = HID_USAGE_KEY_KEYBOARD_APOSTROPHE_AND_QUOTE;
        return 0;
    case '"':
        *usage = HID_USAGE_KEY_KEYBOARD_APOSTROPHE_AND_QUOTE;
        *shift = true;
        return 0;
    case '`':
        *usage = HID_USAGE_KEY_KEYBOARD_GRAVE_ACCENT_AND_TILDE;
        return 0;
    case '~':
        *usage = HID_USAGE_KEY_KEYBOARD_GRAVE_ACCENT_AND_TILDE;
        *shift = true;
        return 0;
    case ',':
        *usage = HID_USAGE_KEY_KEYBOARD_COMMA_AND_LESS_THAN;
        return 0;
    case '<':
        *usage = HID_USAGE_KEY_KEYBOARD_COMMA_AND_LESS_THAN;
        *shift = true;
        return 0;
    case '.':
        *usage = HID_USAGE_KEY_KEYBOARD_PERIOD_AND_GREATER_THAN;
        return 0;
    case '>':
        *usage = HID_USAGE_KEY_KEYBOARD_PERIOD_AND_GREATER_THAN;
        *shift = true;
        return 0;
    case '/':
        *usage = HID_USAGE_KEY_KEYBOARD_SLASH_AND_QUESTION_MARK;
        return 0;
    case '?':
        *usage = HID_USAGE_KEY_KEYBOARD_SLASH_AND_QUESTION_MARK;
        *shift = true;
        return 0;
    case '!':
        *usage = HID_USAGE_KEY_KEYBOARD_1_AND_EXCLAMATION;
        *shift = true;
        return 0;
    case '@':
        *usage = HID_USAGE_KEY_KEYBOARD_2_AND_AT;
        *shift = true;
        return 0;
    case '#':
        *usage = HID_USAGE_KEY_KEYBOARD_3_AND_HASH;
        *shift = true;
        return 0;
    case '$':
        *usage = HID_USAGE_KEY_KEYBOARD_4_AND_DOLLAR;
        *shift = true;
        return 0;
    case '%':
        *usage = HID_USAGE_KEY_KEYBOARD_5_AND_PERCENT;
        *shift = true;
        return 0;
    case '^':
        *usage = HID_USAGE_KEY_KEYBOARD_6_AND_CARET;
        *shift = true;
        return 0;
    case '&':
        *usage = HID_USAGE_KEY_KEYBOARD_7_AND_AMPERSAND;
        *shift = true;
        return 0;
    case '*':
        *usage = HID_USAGE_KEY_KEYBOARD_8_AND_ASTERISK;
        *shift = true;
        return 0;
    case '(':
        *usage = HID_USAGE_KEY_KEYBOARD_9_AND_LEFT_PARENTHESIS;
        *shift = true;
        return 0;
    case ')':
        *usage = HID_USAGE_KEY_KEYBOARD_0_AND_RIGHT_PARENTHESIS;
        *shift = true;
        return 0;
    default:
        return -EINVAL;
    }
}

static int encode_text(struct runtime_macro_encode_state *s, const char *text) {
    for (const char *p = text; *p != '\0'; p++) {
        uint32_t usage;
        bool shift;
        int ret = ascii_char_to_usage(*p, &usage, &shift);
        if (ret < 0) {
            LOG_ERR("Runtime macro default: unsupported character 0x%02x in `text`",
                    (unsigned)(unsigned char)*p);
            return ret;
        }

        uint32_t keycode = ZMK_HID_USAGE(HID_USAGE_KEY, usage);
        if (shift) {
            keycode = LS(keycode);
        }

        ret = try_append_packed_tap(s, keycode);
        if (ret < 0) {
            return ret;
        }
    }

    return 0;
}

static bool name_matches(const char *name, const char *control_name) {
    return control_name != NULL && name != NULL && strcmp(name, control_name) == 0;
}

static int encode_bindings(struct runtime_macro_encode_state *s,
                           const struct runtime_macro_default_config *cfg) {
    enum runtime_macro_default_step_mode mode = RUNTIME_MACRO_DEFAULT_MODE_TAP;
    bool need_delay_before_next = false;

    for (uint32_t i = 0; i < cfg->bindings_len; i++) {
        const struct zmk_behavior_binding *binding = &cfg->bindings[i];

        if (name_matches(binding->behavior_dev, RUNTIME_MACRO_DEFAULT_TAP_MODE_NAME)) {
            mode = RUNTIME_MACRO_DEFAULT_MODE_TAP;
            continue;
        }
        if (name_matches(binding->behavior_dev, RUNTIME_MACRO_DEFAULT_PRESS_MODE_NAME)) {
            mode = RUNTIME_MACRO_DEFAULT_MODE_PRESS;
            continue;
        }
        if (name_matches(binding->behavior_dev, RUNTIME_MACRO_DEFAULT_RELEASE_MODE_NAME)) {
            mode = RUNTIME_MACRO_DEFAULT_MODE_RELEASE;
            continue;
        }
        if (name_matches(binding->behavior_dev, RUNTIME_MACRO_DEFAULT_WAIT_TIME_NAME)) {
            int ret = emit_delay_step(s, binding->param1);
            if (ret < 0) {
                return ret;
            }
            need_delay_before_next = false;
            continue;
        }

        if (mode == RUNTIME_MACRO_DEFAULT_MODE_TAP &&
            name_matches(binding->behavior_dev, RUNTIME_MACRO_DEFAULT_KP_NAME) &&
            try_append_packed_tap(s, binding->param1) == 0) {
            need_delay_before_next = false;
            continue;
        }

        if (need_delay_before_next && cfg->wait_ms > 0) {
            int ret = emit_delay_step(s, cfg->wait_ms);
            if (ret < 0) {
                return ret;
            }
        }

        uint8_t opcode;
        switch (mode) {
        case RUNTIME_MACRO_DEFAULT_MODE_PRESS:
            opcode = ZMK_RUNTIME_MACRO_OP_DOWN;
            break;
        case RUNTIME_MACRO_DEFAULT_MODE_RELEASE:
            opcode = ZMK_RUNTIME_MACRO_OP_UP;
            break;
        default:
            opcode = ZMK_RUNTIME_MACRO_OP_TAP;
            break;
        }

        int ret = emit_binding_step(s, opcode, binding);
        if (ret < 0) {
            return ret;
        }
        need_delay_before_next = true;
    }

    return 0;
}

/* Encode one DT default config's body (version byte + text + bindings) into
 * `buf`, writing its size to `*out_size`. Shared by the boot-time seed and the
 * public zmk_runtime_macro_default_encode() (RPC reset-to-default). */
static int encode_default_body(const struct runtime_macro_default_config *cfg, uint8_t *buf,
                               size_t capacity, size_t *out_size) {
    struct runtime_macro_encode_state state = {
        .buf = buf,
        .capacity = capacity,
    };

    int ret = emit_byte(&state, ZMK_RUNTIME_MACRO_FORMAT_VERSION);
    if (ret < 0) {
        return ret;
    }

    if (cfg->text) {
        ret = encode_text(&state, cfg->text);
        if (ret < 0) {
            return ret;
        }
    }

    if (cfg->bindings_len > 0) {
        ret = encode_bindings(&state, cfg);
        if (ret < 0) {
            return ret;
        }
    }

    ret = flush_packed_sequence(&state);
    if (ret < 0) {
        return ret;
    }

    ret = zmk_runtime_macro_validate_encoded(state.buf, state.size);
    if (ret < 0) {
        return ret;
    }

    *out_size = state.size;
    return 0;
}

/* Public: encode the DT default body for the macro named `name`. Returns
 * -ENOENT if no DT default declares that name. See the header for the full
 * contract. */
int zmk_runtime_macro_default_encode(const char *name, uint8_t *encoded, size_t encoded_capacity,
                                     size_t *encoded_size) {
    if (!name) {
        return -EINVAL;
    }

    for (size_t i = 0; i < RUNTIME_MACRO_DEFAULT_COUNT; i++) {
        const struct runtime_macro_default_config *cfg = runtime_macro_default_configs[i];
        if (strcmp(cfg->name, name) == 0) {
            return encode_default_body(cfg, encoded, encoded_capacity, encoded_size);
        }
    }

    return -ENOENT;
}

static int install_one_default(const struct runtime_macro_default_config *cfg) {
    uint8_t encoded[CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES];
    size_t size = 0;

    int ret = encode_default_body(cfg, encoded, sizeof(encoded), &size);
    if (ret < 0) {
        goto encode_failed;
    }

    /* Seed-if-absent: never overwrite a macro that already exists (a
     * persisted user entry with the same name, or - across a reboot without
     * settings persistence - a DT default this same function already
     * created earlier in this loop). MEMORY mode, so a user Delete of a DT
     * default is a per-session-only removal - it comes back next boot,
     * exactly like a factory default. To remove a DT default permanently,
     * remove it from the devicetree. */
    ret = zmk_runtime_macro_create(cfg->name, encoded, size, ZMK_CUSTOM_SETTING_WRITE_MODE_MEMORY,
                                   NULL);
    if (ret == -EEXIST) {
        LOG_DBG("Runtime macro default: \"%s\" already exists, not overwriting", cfg->name);
        return 0;
    }
    if (ret < 0) {
        LOG_ERR("Runtime macro default: \"%s\" failed to install: %d", cfg->name, ret);
        return ret;
    }

    LOG_DBG("Runtime macro default: installed \"%s\" (%u bytes)", cfg->name, (unsigned)size);
    return 0;

encode_failed:
    LOG_ERR("Runtime macro default: \"%s\" failed to encode: %d", cfg->name, ret);
    return ret;
}

/* Fires as the settings subsystem's commit callback, i.e. once settings_load()
 * has finished loading every persisted record for every registered handler
 * (see subsys/settings/src/settings_store.c: settings_load_subtree loads all
 * sources, THEN commits all handlers) - so every persisted macro is already
 * bound to its keyspace slot by the time this runs, and behavior local IDs
 * are long since resolved (behavior_local_id_init runs at
 * APPLICATION/CONFIG_APPLICATION_INIT_PRIORITY, well before settings_load()
 * is called from main()). Best-effort: one bad default must not prevent the
 * rest from installing. */
static int runtime_macro_seed_dt_defaults(void) {
    for (size_t i = 0; i < RUNTIME_MACRO_DEFAULT_COUNT; i++) {
        install_one_default(runtime_macro_default_configs[i]);
    }

    return 0;
}

/* No h_get/h_set/h_export: this handler exists purely to run
 * runtime_macro_seed_dt_defaults() as h_commit. Its registered name never
 * matches a real stored setting, so it never intercepts settings_load()'s
 * per-record dispatch - it only participates in the commit phase that runs
 * once after every source has loaded. */
SETTINGS_STATIC_HANDLER_DEFINE(runtime_macro_dt_defaults, "runtime_macro_dt_defaults", NULL, NULL,
                               runtime_macro_seed_dt_defaults, NULL);

#else /* !DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */

/* No `cormoran,runtime-macro-default` nodes in the devicetree: no macro has a
 * compile-time default, so a reset can only ever mean "delete". */
int zmk_runtime_macro_default_encode(const char *name, uint8_t *encoded, size_t encoded_capacity,
                                     size_t *encoded_size) {
    ARG_UNUSED(name);
    ARG_UNUSED(encoded);
    ARG_UNUSED(encoded_capacity);
    ARG_UNUSED(encoded_size);
    return -ENOENT;
}

#endif /* DT_HAS_COMPAT_STATUS_OKAY(DT_DRV_COMPAT) */
