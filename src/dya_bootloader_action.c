/* SPDX-License-Identifier: MIT
 * Adapted from S7venYoung/zmk-elashvelvet codex/nano-dongle.
 * Studio bootloader edits are transient actions, never stored key bindings.
 */
#include <errno.h>
#include <string.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include <zephyr/retention/bootmode.h>
#include <zephyr/sys/atomic.h>
#include <zephyr/sys/reboot.h>
#include <zmk/behavior.h>

LOG_MODULE_REGISTER(prospector_dya_bootloader, LOG_LEVEL_INF);
static atomic_t boot_pending;

static void enter_bootloader(struct k_work *work) {
    ARG_UNUSED(work);
    int err = bootmode_set(BOOT_MODE_TYPE_BOOTLOADER);
    if (err != 0) {
        LOG_ERR("Bootloader request failed: %d", err);
    } else {
        sys_reboot(SYS_REBOOT_WARM);
    }
    atomic_clear(&boot_pending);
}
K_WORK_DELAYABLE_DEFINE(boot_work, enter_bootloader);

int prospector_dya_binding_action(const struct zmk_behavior_binding *binding) {
    if (!binding || !binding->behavior_dev || strcmp(binding->behavior_dev, "bootload")) {
        return 0;
    }
    if (!atomic_cas(&boot_pending, 0, 1)) {
        return -EBUSY;
    }
    /* Allow the Studio response to leave USB before the reboot disconnects it. */
    int ret = k_work_reschedule(&boot_work, K_MSEC(500));
    if (ret < 0) {
        atomic_clear(&boot_pending);
        return ret;
    }
    return 1;
}
