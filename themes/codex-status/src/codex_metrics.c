#include <errno.h>
#include <zephyr/kernel.h>
#include <zmk/codex_metrics.h>
#include <zmk/event_manager.h>
#include <zmk/events/codex_metrics_changed.h>

static struct zmk_codex_metrics current_metrics;
K_MUTEX_DEFINE(metrics_mutex);

ZMK_EVENT_IMPL(zmk_codex_metrics_changed);

struct zmk_codex_metrics zmk_codex_metrics_get(void) {
    struct zmk_codex_metrics result;
    k_mutex_lock(&metrics_mutex, K_FOREVER);
    result = current_metrics;
    k_mutex_unlock(&metrics_mutex);
    return result;
}

int zmk_codex_metrics_set(struct zmk_codex_metrics metrics) {
    if (metrics.five_hour_used_percent > 100) return -ERANGE;
    k_mutex_lock(&metrics_mutex, K_FOREVER);
    current_metrics = metrics;
    k_mutex_unlock(&metrics_mutex);
    return raise_zmk_codex_metrics_changed((struct zmk_codex_metrics_changed){
        .five_hour_used_percent = metrics.five_hour_used_percent,
        .today_total_tokens = metrics.today_total_tokens,
        .updated_at = metrics.updated_at,
    });
}
