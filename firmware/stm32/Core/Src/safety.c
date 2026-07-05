#include "safety.h"

bool safety_has_critical_fault(const robot_context_t *ctx, uint32_t now_ms) {
    if (ctx->error_flags & (ERR_CONFIG_INVALID | ERR_SENSOR_FAULT | ERR_ADC_FAULT | ERR_BATTERY_LOW)) {
        return true;
    }
    if (ctx->config.stop_on_esp_timeout && ctx->last_esp_packet_ms != 0 && (now_ms - ctx->last_esp_packet_ms) > 1000u) {
        return true;
    }
    return false;
}

void safety_apply_stop(robot_context_t *ctx, uint32_t error_flags) {
    ctx->error_flags |= error_flags;
    ctx->stop_requested = true;
}
