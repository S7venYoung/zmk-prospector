#pragma once

#include <stdint.h>

/* Generic host data. No theme owns or renders these fields. */
struct zmk_host_status {
    uint32_t unix_time;
    int16_t temperature_deci_c;
    uint8_t weather_code;
    uint32_t observed_at;
};

struct zmk_host_status zmk_host_status_get(void);
int zmk_host_status_set(struct zmk_host_status status);
