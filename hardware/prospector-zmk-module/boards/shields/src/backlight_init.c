/*
 * Simple backlight initialization - turns on backlight at boot
 * Uses PWM LED driver for brightness control
 *
 * This runs at SYS_INIT priority 50, BEFORE display initialization,
 * ensuring the backlight is on even if display init fails.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(backlight_init, LOG_LEVEL_INF);

/* Backlight PWM LED node */
#if DT_HAS_COMPAT_STATUS_OKAY(pwm_leds)
#define BACKLIGHT_NODE DT_COMPAT_GET_ANY_STATUS_OKAY(pwm_leds)
static const struct device *backlight_dev = DEVICE_DT_GET(BACKLIGHT_NODE);
#define HAS_PWM_BACKLIGHT 1
#else
#define HAS_PWM_BACKLIGHT 0
#endif

/* Default brightness (0-100) */
#define DEFAULT_BRIGHTNESS 65

/* The old 3s k_timer "heartbeat" was removed: it called
 * display_get_capabilities() from ISR context every 15s and logged 1200
 * records/hour. Liveness is now covered by fault_recovery.c. */

#if HAS_PWM_BACKLIGHT

static int backlight_init(void) {
    int ret;

    LOG_INF("=== BACKLIGHT INIT (PWM, priority 50) ===");

    if (!device_is_ready(backlight_dev)) {
        LOG_ERR("PWM backlight device not ready");
        return -ENODEV;
    }

    /* Set initial brightness to default value */
    ret = led_set_brightness(backlight_dev, 0, DEFAULT_BRIGHTNESS);
    if (ret < 0) {
        LOG_ERR("Failed to set backlight brightness: %d", ret);
        return ret;
    }

    LOG_INF("Backlight turned ON at %d%% brightness", DEFAULT_BRIGHTNESS);

    return 0;
}

SYS_INIT(backlight_init, APPLICATION, 50);

#else
#warning "PWM backlight LED node not found"
#endif
