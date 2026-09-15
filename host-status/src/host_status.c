#include <errno.h>
#include <zephyr/kernel.h>
#include <zmk/event_manager.h>
#include <zmk/host_status.h>
#include <zmk/events/host_status_changed.h>

static struct zmk_host_status current_status;
K_MUTEX_DEFINE(host_status_mutex);

ZMK_EVENT_IMPL(zmk_host_status_changed);

struct zmk_host_status zmk_host_status_get(void) {
    struct zmk_host_status result;
    k_mutex_lock(&host_status_mutex, K_FOREVER);
    result = current_status;
    k_mutex_unlock(&host_status_mutex);
    return result;
}

int zmk_host_status_set(struct zmk_host_status status) {
    k_mutex_lock(&host_status_mutex, K_FOREVER);
    current_status = status;
    k_mutex_unlock(&host_status_mutex);
    return raise_zmk_host_status_changed((struct zmk_host_status_changed){
        .unix_time = status.unix_time,
        .temperature_deci_c = status.temperature_deci_c,
        .weather_code = status.weather_code,
        .observed_at = status.observed_at,
        .high_temperature_deci_c = status.high_temperature_deci_c,
        .low_temperature_deci_c = status.low_temperature_deci_c,
        .rain_probability = status.rain_probability,
        .timezone_offset_minutes = status.timezone_offset_minutes,
    });
}
