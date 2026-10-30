/*
 * Backlight initialization and first-frame gate
 * Uses PWM LED driver for brightness control
 *
 * This runs at SYS_INIT priority 50, BEFORE display initialization,
 * Scanner startup stays dark until LVGL has transferred a complete frame.
 */

#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/drivers/led.h>
#include <zephyr/drivers/display.h>
#include <zephyr/logging/log.h>
#include "backlight_init.h"

LOG_MODULE_REGISTER(backlight_init, LOG_LEVEL_INF);

/* Backlight PWM LED node */
#if DT_HAS_COMPAT_STATUS_OKAY(pwm_leds)
#define BACKLIGHT_LED_NODE DT_NODELABEL(backlight)
#define BACKLIGHT_NODE DT_PARENT(BACKLIGHT_LED_NODE)
#define BACKLIGHT_LED_INDEX DT_NODE_CHILD_IDX(BACKLIGHT_LED_NODE)
static const struct device *backlight_dev = DEVICE_DT_GET(BACKLIGHT_NODE);
#define HAS_PWM_BACKLIGHT 1
#else
#define HAS_PWM_BACKLIGHT 0
#endif

/* Default brightness (0-100) */
#if IS_ENABLED(CONFIG_ZMK_PROSPECTOR_DYA_SETTINGS)
/* Same user brightness as DYA's unsaved default; ms2 PWM is inverted. */
#define DEFAULT_BRIGHTNESS (100 - 60)
#else
#define DEFAULT_BRIGHTNESS 65
#endif

/* The old 3s k_timer "heartbeat" was removed: it called
 * display_get_capabilities() from ISR context every 15s and logged 1200
 * records/hour. Liveness is now covered by fault_recovery.c. */

#if HAS_PWM_BACKLIGHT

static uint8_t requested_brightness = DEFAULT_BRIGHTNESS;
static bool first_frame_ready = !IS_ENABLED(CONFIG_ZMK_PROSPECTOR_BROADCAST_SCANNER);

int prospector_backlight_request(uint8_t pwm_level) {
    if (pwm_level > 100) return -EINVAL;
    requested_brightness = pwm_level;
    if (!device_is_ready(backlight_dev)) return -ENODEV;
    return first_frame_ready ? led_set_brightness(backlight_dev, BACKLIGHT_LED_INDEX, pwm_level) : 0;
}

void prospector_backlight_first_frame(void) {
    if (first_frame_ready) return;
    first_frame_ready = true;
    if (device_is_ready(backlight_dev))
        led_set_brightness(backlight_dev, BACKLIGHT_LED_INDEX, requested_brightness);
}

static int backlight_init(void) {
    int ret;

    LOG_INF("=== BACKLIGHT INIT (PWM, priority 50) ===");

    if (!device_is_ready(backlight_dev)) {
        LOG_ERR("PWM backlight device not ready");
        return -ENODEV;
    }

    /* Set initial brightness to default value */
    ret = led_set_brightness(backlight_dev, BACKLIGHT_LED_INDEX,
                             first_frame_ready ? requested_brightness : 100);
    if (ret < 0) {
        LOG_ERR("Failed to set backlight brightness: %d", ret);
        return ret;
    }

    LOG_INF("Backlight initialized; first frame ready=%d", first_frame_ready);

    return 0;
}

SYS_INIT(backlight_init, APPLICATION, 50);

#else
int prospector_backlight_request(uint8_t pwm_level) { (void)pwm_level; return -ENODEV; }
void prospector_backlight_first_frame(void) {}
#warning "PWM backlight LED node not found"
#endif
