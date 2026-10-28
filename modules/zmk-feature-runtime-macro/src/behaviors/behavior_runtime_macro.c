/*
 * Copyright (c) 2026 cormoran
 *
 * SPDX-License-Identifier: MIT
 */

#define DT_DRV_COMPAT cormoran_zmk_behavior_runtime_macro

#include <zephyr/device.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <cormoran/zmk/runtime_macro.h>
#include <zmk/behavior.h>

LOG_MODULE_DECLARE(zmk, CONFIG_ZMK_LOG_LEVEL);

#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)

static const struct behavior_parameter_value_metadata param_values[] = {
    {
        .display_name = "Macro",
        .type = BEHAVIOR_PARAMETER_VALUE_TYPE_RANGE,
        .range =
            {
                .min = 0,
                .max = CONFIG_ZMK_RUNTIME_MACRO_COUNT - 1,
            },
    },
};

static const struct behavior_parameter_metadata_set param_metadata_set[] = {{
    .param1_values = param_values,
    .param1_values_len = ARRAY_SIZE(param_values),
}};

static const struct behavior_parameter_metadata metadata = {
    .sets_len = ARRAY_SIZE(param_metadata_set),
    .sets = param_metadata_set,
};

#endif

static int on_runtime_macro_pressed(struct zmk_behavior_binding *binding,
                                    struct zmk_behavior_binding_event event) {
    int ret = zmk_runtime_macro_play(binding->param1, &event);
    if (ret < 0) {
        LOG_WRN("Runtime macro %u failed: %d", binding->param1, ret);
    }

    return ZMK_BEHAVIOR_OPAQUE;
}

static int on_runtime_macro_released(struct zmk_behavior_binding *binding,
                                     struct zmk_behavior_binding_event event) {
    ARG_UNUSED(binding);
    ARG_UNUSED(event);

    return ZMK_BEHAVIOR_OPAQUE;
}

static const struct behavior_driver_api behavior_runtime_macro_driver_api = {
    .binding_pressed = on_runtime_macro_pressed,
    .binding_released = on_runtime_macro_released,
#if IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
    .parameter_metadata = &metadata,
#endif // IS_ENABLED(CONFIG_ZMK_BEHAVIOR_METADATA)
};

#define RUNTIME_MACRO_INST(n)                                                                      \
    BEHAVIOR_DT_INST_DEFINE(n, NULL, NULL, NULL, NULL, POST_KERNEL,                                \
                            CONFIG_KERNEL_INIT_PRIORITY_DEFAULT,                                   \
                            &behavior_runtime_macro_driver_api);

DT_INST_FOREACH_STATUS_OKAY(RUNTIME_MACRO_INST)
