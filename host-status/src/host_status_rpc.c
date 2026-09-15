#include <pb_decode.h>
#include <pb_encode.h>
#include <zmk/host_status.h>
#include <zmk/studio/custom.h>
#include <s7venyoung/host_status/host_status.pb.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(prospector_host_status_rpc, CONFIG_ZMK_LOG_LEVEL);

static bool handle_request(const zmk_custom_CallRequest *raw_request, pb_callback_t *encode_response);
static struct zmk_rpc_custom_subsystem_meta status_meta = {
    .security = ZMK_STUDIO_RPC_HANDLER_UNSECURED,
};

ZMK_RPC_CUSTOM_SUBSYSTEM(s7venyoung__host_status, &status_meta, handle_request);
ZMK_RPC_CUSTOM_SUBSYSTEM_RESPONSE_BUFFER(s7venyoung__host_status,
                                         s7venyoung_host_status_Response);

static bool handle_request(const zmk_custom_CallRequest *raw_request, pb_callback_t *encode_response) {
    s7venyoung_host_status_Response *response =
        ZMK_RPC_CUSTOM_SUBSYSTEM_RESPONSE_BUFFER_ALLOCATE(s7venyoung__host_status,
                                                          encode_response);
    s7venyoung_host_status_Request request = s7venyoung_host_status_Request_init_zero;
    pb_istream_t stream = pb_istream_from_buffer(raw_request->payload.bytes,
                                                  raw_request->payload.size);
    if (!pb_decode(&stream, s7venyoung_host_status_Request_fields, &request)) return false;

    if (request.which_request_type == s7venyoung_host_status_Request_set_status_tag) {
        const s7venyoung_host_status_Status *incoming = &request.request_type.set_status.status;
        int ret = zmk_host_status_set((struct zmk_host_status){
            .unix_time = incoming->unix_time,
            .temperature_deci_c = incoming->temperature_deci_c,
            .weather_code = incoming->weather_code,
            .observed_at = incoming->observed_at,
            .high_temperature_deci_c = incoming->high_temperature_deci_c,
            .low_temperature_deci_c = incoming->low_temperature_deci_c,
            .rain_probability = incoming->rain_probability,
            .timezone_offset_minutes = incoming->timezone_offset_minutes,
        });
        response->which_response_type = s7venyoung_host_status_Response_ack_tag;
        response->response_type.ack.ok = ret == 0;
        return true;
    }
    if (request.which_request_type == s7venyoung_host_status_Request_get_status_tag) {
        struct zmk_host_status current = zmk_host_status_get();
        response->which_response_type = s7venyoung_host_status_Response_status_tag;
        response->response_type.status.unix_time = current.unix_time;
        response->response_type.status.temperature_deci_c = current.temperature_deci_c;
        response->response_type.status.weather_code = current.weather_code;
        response->response_type.status.observed_at = current.observed_at;
        response->response_type.status.high_temperature_deci_c = current.high_temperature_deci_c;
        response->response_type.status.low_temperature_deci_c = current.low_temperature_deci_c;
        response->response_type.status.rain_probability = current.rain_probability;
        response->response_type.status.timezone_offset_minutes = current.timezone_offset_minutes;
        return true;
    }
    return false;
}
