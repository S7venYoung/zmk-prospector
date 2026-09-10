/*
 * Copyright (c) 2026 cormoran
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <cormoran/zmk/custom_settings.h>
#include <zmk/behavior.h>

#define ZMK_RUNTIME_MACRO_SUBSYSTEM_ID "cormoran__runtime_macro"

/* Every macro is one entry in the runtime_macros keyspace: its NAME is the
 * keyspace key suffix appended to this prefix ("macro/hello"), and its
 * payload is the macro's encoded body - see docs/design/keyspace-macros.md.
 * There is no separate name storage/table any more. */
#define ZMK_RUNTIME_MACRO_KEY_PREFIX "macro/"
#define ZMK_RUNTIME_MACRO_TAP_MS_KEY "tap_ms"

#define ZMK_RUNTIME_MACRO_FORMAT_VERSION 1U

enum zmk_runtime_macro_opcode {
    ZMK_RUNTIME_MACRO_OP_DOWN = 1,
    ZMK_RUNTIME_MACRO_OP_UP = 2,
    ZMK_RUNTIME_MACRO_OP_TAP = 3,
    ZMK_RUNTIME_MACRO_OP_DELAY = 4,
    ZMK_RUNTIME_MACRO_OP_KEY_TAP_SEQUENCE = 5,
};

int zmk_runtime_macro_pack_key_tap(uint32_t keycode, uint8_t *packed_key);
int zmk_runtime_macro_unpack_key_tap(uint8_t packed_key, uint32_t *keycode);
int zmk_runtime_macro_validate_encoded(const uint8_t *encoded, size_t size);

/* Play whatever macro is currently bound to keyspace slot `slot`
 * (0..CONFIG_ZMK_RUNTIME_MACRO_COUNT-1) - this is the keymap behavior's
 * numeric parameter. Slot indices are stable across reboots (ordinal
 * persistence re-binds a persisted entry to the same slot every boot), but
 * are only known for certain by reading a macro's `slot` field from the RPC
 * list/get response after it is created. An empty/unbound slot plays
 * nothing (logged at debug), not an error - so an unused keymap position
 * bound to `&rmacro N` is silently inert until a macro is created there. */
int zmk_runtime_macro_play(uint32_t slot, const struct zmk_behavior_binding_event *event);

/* Create a new named macro (a fresh runtime_macros keyspace entry) with the
 * given encoded body, claiming whatever slot is currently free. Returns
 * -EEXIST if `name` already has a live macro, -ENOSPC if every slot is in
 * use or the shared name+body pool is full, -EMSGSIZE if name/encoded_size
 * exceed their configured limits, -EINVAL for a malformed encoded body.
 * `out_slot` (if non-NULL) receives the new macro's slot index. */
int zmk_runtime_macro_create(const char *name, const uint8_t *encoded, size_t encoded_size,
                             enum zmk_custom_setting_write_mode mode, uint32_t *out_slot);

/* Delete the named macro: erases its persisted record (if any) and frees its
 * slot for reuse. Returns -ENOENT if no live macro has this name. */
int zmk_runtime_macro_delete(const char *name);

/* Overwrite an existing named macro's body in place (the name/slot are
 * unchanged) - used by the step-level editing RPCs. Returns -ENOENT if no
 * live macro has this name, -EMSGSIZE if encoded_size exceeds
 * CONFIG_ZMK_RUNTIME_MACRO_MAX_BYTES, -ENOSPC if the shared pool is full,
 * -EINVAL for a malformed encoded body. */
int zmk_runtime_macro_write(const char *name, const uint8_t *encoded, size_t encoded_size,
                            enum zmk_custom_setting_write_mode mode);

/* Read the named macro's encoded body. Returns -ENOENT if no live macro has
 * this name. */
int zmk_runtime_macro_read(const char *name, uint8_t *encoded, size_t encoded_capacity,
                           size_t *encoded_size);

/* Rename a macro: creates a new entry under `new_name` with the current
 * body and deletes the old one. Returns -ENOENT if `old_name` has no live
 * macro, -EEXIST if `new_name` is already taken, and leaves the old entry
 * untouched if the create half fails (so a rename never loses data). The
 * macro's slot index is NOT preserved by a rename (the new entry claims
 * whatever slot is free, which is very likely, but not guaranteed to be,
 * the same slot the old entry occupied). */
int zmk_runtime_macro_rename(const char *old_name, const char *new_name,
                             enum zmk_custom_setting_write_mode mode);

/* Encode the compile-time (devicetree) default body for the macro named
 * `name` into `encoded` (capacity `encoded_capacity`), writing its size to
 * `*encoded_size`. This is the same byte body the boot-time seed installs, so
 * callers can restore a macro to its factory state. Returns -ENOENT if no
 * `cormoran,runtime-macro-default` node declares a macro with this name (i.e.
 * the macro has no compile-time default), -ENOSPC if the buffer is too small,
 * or another negative errno if the default fails to encode. */
int zmk_runtime_macro_default_encode(const char *name, uint8_t *encoded, size_t encoded_capacity,
                                     size_t *encoded_size);

/* Slot <-> name resolution, for the keymap binding UX and RPC list/get
 * responses. Returns -ENOENT if the slot is out of range or currently
 * unbound (no live macro), or (for slot_for_name) if no live macro has this
 * name. */
int zmk_runtime_macro_name_for_slot(uint32_t slot, char *name, size_t name_capacity);
int zmk_runtime_macro_slot_for_name(const char *name, uint32_t *out_slot);

/* Total bytes configured for the shared name+body pool
 * (CONFIG_ZMK_RUNTIME_MACRO_POOL_BYTES) and the bytes currently occupied by
 * every macro's name+body combined - for RPC/UI budget reporting. */
size_t zmk_runtime_macro_pool_total(void);
size_t zmk_runtime_macro_pool_used(void);

/* Iterate every currently bound slot in ascending order, calling `cb` with
 * the macro's slot index and name for each. Used by the RPC list handler;
 * exposed here so it stays in one place (the keyspace-slot iteration
 * convention - see docs/design/keyspace-macros.md) instead of being
 * duplicated at each caller. `has_unsaved_changes` is true when the macro has
 * an in-memory-only value not yet written to flash. `cb` returning a negative
 * value stops the iteration early and that value is returned; otherwise
 * returns 0. */
typedef int (*zmk_runtime_macro_iter_cb_t)(uint32_t slot, const char *name, size_t encoded_size,
                                           bool has_unsaved_changes, void *user_data);
int zmk_runtime_macro_for_each(zmk_runtime_macro_iter_cb_t cb, void *user_data);
