#pragma once

#include <stdint.h>

struct zmk_codex_metrics {
    uint8_t five_hour_used_percent;
    uint64_t today_total_tokens;
    uint64_t updated_at;
};

struct zmk_codex_metrics zmk_codex_metrics_get(void);
int zmk_codex_metrics_set(struct zmk_codex_metrics metrics);
