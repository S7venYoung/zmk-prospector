#pragma once

#include <stdint.h>
#include <zmk/event_manager.h>

struct zmk_codex_metrics_changed {
    uint8_t five_hour_used_percent;
    uint64_t today_total_tokens;
    uint64_t updated_at;
};

ZMK_EVENT_DECLARE(zmk_codex_metrics_changed);
