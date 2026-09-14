#include <pb_decode.h>
#include <pb_encode.h>
#include <zephyr/logging/log.h>
#include <zmk/codex_metrics.h>
#include <zmk/studio/custom.h>
#include <s7venyoung/codex_metrics/codex_metrics.pb.h>

LOG_MODULE_REGISTER(codex_metrics_rpc, CONFIG_ZMK_LOG_LEVEL);

static bool handle_request(const zmk_custom_CallRequest *raw_request, pb_callback_t *encode_response);

static struct zmk_rpc_custom_subsystem_meta metrics_meta = {
    .security = ZMK_STUDIO_RPC_HANDLER_UNSECURED,
};

ZMK_RPC_CUSTOM_SUBSYSTEM(s7venyoung__codex_metrics, &metrics_meta, handle_request);
ZMK_RPC_CUSTOM_SUBSYSTEM_RESPONSE_BUFFER(s7venyoung__codex_metrics,
                                         s7venyoung_codex_metrics_Response);

static bool handle_request(const zmk_custom_CallRequest *raw_request, pb_callback_t *encode_response) {
    s7venyoung_codex_metrics_Response *response =
        ZMK_RPC_CUSTOM_SUBSYSTEM_RESPONSE_BUFFER_ALLOCATE(s7venyoung__codex_metrics,
                                                          encode_response);
    s7venyoung_codex_metrics_Request request = s7venyoung_codex_metrics_Request_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(raw_request->payload.bytes,
                                                  raw_request->payload.size);
    if (!pb_decode(&stream, s7venyoung_codex_metrics_Request_fields, &request)) return false;

    if (request.which_request_type == s7venyoung_codex_metrics_Request_set_metrics_tag) {
        const s7venyoung_codex_metrics_Metrics *incoming =
            &request.request_type.set_metrics.metrics;
        int ret = zmk_codex_metrics_set((struct zmk_codex_metrics){
            .five_hour_used_percent = incoming->five_hour_used_percent,
            .today_total_tokens = incoming->today_total_tokens,
            .updated_at = incoming->updated_at,
        });
        response->which_response_type = s7venyoung_codex_metrics_Response_status_tag;
        response->response_type.status.ok = ret == 0;
        return true;
    }

    if (request.which_request_type == s7venyoung_codex_metrics_Request_get_metrics_tag) {
        struct zmk_codex_metrics current = zmk_codex_metrics_get();
        response->which_response_type = s7venyoung_codex_metrics_Response_metrics_tag;
        response->response_type.metrics.five_hour_used_percent = current.five_hour_used_percent;
        response->response_type.metrics.today_total_tokens = current.today_total_tokens;
        response->response_type.metrics.updated_at = current.updated_at;
        return true;
    }
    return false;
}
