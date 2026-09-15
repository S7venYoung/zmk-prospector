#pragma once

#include <stdint.h>
#include <zmk/event_manager.h>

struct zmk_host_status_changed {
    uint32_t unix_time;
    int16_t temperature_deci_c;
    uint8_t weather_code;
    uint32_t observed_at;
};

ZMK_EVENT_DECLARE(zmk_host_status_changed);
