/*
 * Copyright (c) 2026 The ZMK Contributors
 *
 * SPDX-License-Identifier: MIT
 *
 * Crash recovery for the scanner.
 *
 * Zephyr's default k_sys_fatal_error_handler() locks interrupts and spins
 * forever. On a display-only device that is indistinguishable from a
 * freeze: the last frame stays on the LCD, nothing responds, and there is
 * no record of what happened. Every stack overflow (MPU guard), NULL
 * dereference, HardFault or k_oops therefore reported as "the scanner
 * froze".
 *
 * This override records the fault in a __noinit RAM block (nRF52 keeps RAM
 * across a soft reset), then reboots. On the next boot the record is
 * logged and kept available via fault_recovery_get_last() so it can be
 * shown on the settings screen. A crash-loop guard halts the old way if
 * the device keeps faulting within the first minute after boot, so a
 * broken build cannot reboot endlessly.
 */

#include <zephyr/kernel.h>
#include <zephyr/init.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/reboot.h>
#include <zephyr/drivers/hwinfo.h>
#include <zephyr/logging/log.h>
#include <zephyr/logging/log_ctrl.h>
#include <string.h>

#if IS_ENABLED(CONFIG_PROSPECTOR_SCANNER_TASK_WATCHDOG)
#include <zephyr/task_wdt/task_wdt.h>
#endif

#include "fault_recovery.h"

LOG_MODULE_REGISTER(fault_recovery, LOG_LEVEL_INF);

#define CRASH_MAGIC 0x43524153u /* "CRAS" */

/* Consecutive faults within CRASH_LOOP_WINDOW_MS of boot before we give up
 * rebooting and halt like the stock handler would. */
#define CRASH_LOOP_LIMIT     5
#define CRASH_LOOP_WINDOW_MS 60000

struct crash_slot {
    uint32_t magic;
    struct fault_record rec;
};

/* Survives soft reset - deliberately NOT zero-initialised. */
static struct crash_slot crash __noinit;

static struct fault_record last_record;
static bool have_last_record;
static uint32_t boot_reset_cause;

static void crash_loop_clear_handler(struct k_work *work) {
    ARG_UNUSED(work);
    /* Device has been up long enough: this boot is healthy, forget the
     * consecutive-crash count (the last record itself is kept for display). */
    crash.magic = 0;
    crash.rec.consecutive = 0;
}
static K_WORK_DELAYABLE_DEFINE(crash_loop_clear_work, crash_loop_clear_handler);

void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf) {
    uint32_t uptime = k_uptime_get_32();
    uint32_t consecutive = 1;

    if (crash.magic == CRASH_MAGIC) {
        consecutive = crash.rec.consecutive + 1;
    }

    crash.magic = CRASH_MAGIC;
    crash.rec.reason = reason;
    crash.rec.pc = esf ? esf->basic.pc : 0;
    crash.rec.lr = esf ? esf->basic.lr : 0;
    crash.rec.uptime_ms = uptime;
    crash.rec.consecutive = consecutive;

    const char *name = k_thread_name_get(k_current_get());
    if (name == NULL || name[0] == '\0') {
        name = k_is_in_isr() ? "<isr>" : "<unnamed>";
    }
    strncpy(crash.rec.thread, name, sizeof(crash.rec.thread) - 1);
    crash.rec.thread[sizeof(crash.rec.thread) - 1] = '\0';

    /* Flush whatever is in the deferred log buffer synchronously. */
    LOG_PANIC();
    LOG_ERR("FATAL reason=%u thread=%s pc=0x%08x lr=0x%08x uptime=%ums (#%u)",
            reason, crash.rec.thread, crash.rec.pc, crash.rec.lr, uptime, consecutive);

    if (consecutive >= CRASH_LOOP_LIMIT && uptime < CRASH_LOOP_WINDOW_MS) {
        LOG_ERR("Crash loop (%u faults within %ums of boot) - halting", consecutive, uptime);
        (void)arch_irq_lock();
        for (;;) {
            /* Halt like the stock handler. */
        }
    }

    LOG_ERR("Rebooting to recover");
    sys_reboot(SYS_REBOOT_COLD);
    CODE_UNREACHABLE;
}

/* ---------- Software watchdog (task_wdt, no hardware backend) ----------
 *
 * Catches the other freeze class: a thread that stops running (deadlock,
 * livelock, priority starvation) while the kernel itself is still alive.
 * Pure software mode (task_wdt_init(NULL)) - the nRF52 hardware WDT keeps
 * running across a soft reset and would fire inside the UF2 bootloader
 * during firmware updates, so it is deliberately not used.
 *
 * Two channels, each with a WATCHDOG_PERIOD_MS budget:
 *   display - fed by the 100ms LVGL timer on the dedicated display thread
 *   core    - fed by s7789_update.c's periodic work on the system workqueue
 *             (原 scanner_core.c 的 process_work，该文件已移除)
 * On expiry the k_timer ISR records a synthetic reason and reboots; the
 * next boot reports it like any other crash.
 */
#if IS_ENABLED(CONFIG_PROSPECTOR_SCANNER_TASK_WATCHDOG)

/* 30s: comfortably above every legitimate stall on either fed path (NVS
 * settings save on the display thread during a screen transition, a 500ms
 * I2C timeout on the light sensor, touch-event floods that back up the
 * shared system workqueue) while still recovering a wedged device long
 * before anyone reaches for the power switch. */
#define WATCHDOG_PERIOD_MS 30000

static int wdt_display_channel = -1;
static int wdt_core_channel = -1;

static void record_hang_and_reboot(uint32_t reason, const char *what) {
    /* Runs in k_timer (ISR) context: RAM writes + reboot only. */
    uint32_t consecutive = (crash.magic == CRASH_MAGIC) ? crash.rec.consecutive + 1 : 1;
    crash.magic = CRASH_MAGIC;
    crash.rec.reason = reason;
    crash.rec.pc = 0;
    crash.rec.lr = 0;
    crash.rec.uptime_ms = k_uptime_get_32();
    crash.rec.consecutive = consecutive;
    strncpy(crash.rec.thread, what, sizeof(crash.rec.thread) - 1);
    crash.rec.thread[sizeof(crash.rec.thread) - 1] = '\0';

    LOG_PANIC();
    LOG_ERR("WATCHDOG: %s stopped for %dms at uptime %ums - rebooting",
            what, WATCHDOG_PERIOD_MS, crash.rec.uptime_ms);
    sys_reboot(SYS_REBOOT_COLD);
}

static void wdt_display_expired(int channel_id, void *user_data) {
    ARG_UNUSED(channel_id);
    ARG_UNUSED(user_data);
    record_hang_and_reboot(FAULT_REASON_HANG_DISPLAY, "display");
}

static void wdt_core_expired(int channel_id, void *user_data) {
    ARG_UNUSED(channel_id);
    ARG_UNUSED(user_data);
    record_hang_and_reboot(FAULT_REASON_HANG_CORE, "core");
}

void fault_recovery_display_alive(void) {
    if (wdt_display_channel >= 0) {
        task_wdt_feed(wdt_display_channel);
    }
}

/* 系统工作队列存活回调：由 s7789_update.c 的周期任务调用（原来由
 * scanner_core.c 的 process_work 调用，该文件已移除）。 */
void ble_core_process_alive(void) {
    if (wdt_core_channel >= 0) {
        task_wdt_feed(wdt_core_channel);
    }
}

static void watchdog_init(void) {
    int err = task_wdt_init(NULL);
    if (err) {
        LOG_ERR("task_wdt_init failed: %d (watchdog disabled)", err);
        return;
    }
    wdt_display_channel = task_wdt_add(WATCHDOG_PERIOD_MS, wdt_display_expired, NULL);
    wdt_core_channel = task_wdt_add(WATCHDOG_PERIOD_MS, wdt_core_expired, NULL);
    if (wdt_display_channel < 0 || wdt_core_channel < 0) {
        LOG_ERR("task_wdt_add failed (display=%d core=%d)", wdt_display_channel, wdt_core_channel);
    } else {
        LOG_INF("Software watchdog armed: display+core, %dms", WATCHDOG_PERIOD_MS);
    }
}
#else
void fault_recovery_display_alive(void) {}
void ble_core_process_alive(void) {}
static inline void watchdog_init(void) {}
#endif /* CONFIG_PROSPECTOR_SCANNER_TASK_WATCHDOG */

bool fault_recovery_get_last(struct fault_record *out) {
    if (!have_last_record) {
        return false;
    }
    if (out) {
        *out = last_record;
    }
    return true;
}

uint32_t fault_recovery_reset_cause(void) {
    return boot_reset_cause;
}

static int fault_recovery_init(void) {
    uint32_t cause = 0;
    if (hwinfo_get_reset_cause(&cause) == 0) {
        boot_reset_cause = cause;
        (void)hwinfo_clear_reset_cause();
    }

    if (crash.magic == CRASH_MAGIC) {
        last_record = crash.rec;
        have_last_record = true;
        LOG_WRN("Recovered from crash: reason=%u thread=%s pc=0x%08x lr=0x%08x "
                "uptime=%ums consecutive=%u (reset cause 0x%x)",
                last_record.reason, last_record.thread, last_record.pc,
                last_record.lr, last_record.uptime_ms, last_record.consecutive, cause);
        /* Keep magic + count until the boot proves healthy (crash-loop guard). */
        k_work_schedule(&crash_loop_clear_work, K_MSEC(CRASH_LOOP_WINDOW_MS));
    } else {
        LOG_INF("Clean boot (reset cause 0x%x)", cause);
        crash.magic = 0;
        crash.rec.consecutive = 0;
    }

    watchdog_init();
    return 0;
}

SYS_INIT(fault_recovery_init, APPLICATION, 50);
