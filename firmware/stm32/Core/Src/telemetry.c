#include "telemetry.h"
#include "board_pins.h"
#include "pegasus_hal.h"
#include <string.h>

static uint32_t last_basic_ms = 0;
static uint32_t last_sensor_ms = 0;

static void put_u16(uint8_t *p, uint16_t v) {
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)(v >> 8);
}

static void put_i16(uint8_t *p, int16_t v) {
    put_u16(p, (uint16_t)v);
}

static void put_u32(uint8_t *p, uint32_t v) {
    p[0] = (uint8_t)(v & 0xFFu);
    p[1] = (uint8_t)((v >> 8) & 0xFFu);
    p[2] = (uint8_t)((v >> 16) & 0xFFu);
    p[3] = (uint8_t)((v >> 24) & 0xFFu);
}

static uint8_t esp_state(robot_state_t state) {
    switch (state) {
    case ST_BOOT: return 0;
    case ST_IDLE:
    case ST_SELF_TEST: return 1;
    case ST_CALIBRATION: return 2;
    case ST_READY: return 3;
    case ST_ARMED: return 4;
    case ST_RUNNING:
    case ST_FAN_SPINUP: return 5;
    case ST_CROSSING: return 6;
    case ST_CURVE_LEFT: return 7;
    case ST_FINISH_DETECTED: return 8;
    case ST_LINE_LOST: return 9;
    case ST_SAFE_STOP: return 10;
    case ST_ERROR:
    case ST_HARDWARE_TEST: return 11;
    }
    return 11;
}

bool telemetry_build_basic(const robot_context_t *ctx, const motor_state_t *motor, const fan_state_t *fan, uint8_t *payload, uint8_t *len) {
    memset(payload, 0, 24);
    payload[0] = esp_state(ctx->state);
    put_u16(&payload[1], 0); /* VBAT optional in Rev A0 */
    put_i16(&payload[3], ctx->sensors.position);
    put_i16(&payload[5], ctx->sensors.error);
    put_i16(&payload[7], 0); /* PID correction is exposed in future telemetry */
    put_i16(&payload[9], motor->left_output);
    put_i16(&payload[11], motor->right_output);
    put_u16(&payload[13], fan->output);
    payload[15] = (ctx->sensors.line_detected ? 0x01u : 0u) |
                  (ctx->sensors.crossing_detected ? 0x02u : 0u) |
                  (ctx->sensors.lat_l_active ? 0x04u : 0u) |
                  (ctx->sensors.lat_r_active ? 0x08u : 0u) |
                  (ctx->sensors.line_lost_confirmed ? 0x10u : 0u);
    put_u32(&payload[16], ctx->run_time_ms);
    put_u32(&payload[20], ctx->error_flags);
    *len = 24;
    return true;
}

bool telemetry_build_sensors(const sensor_state_t *s, uint8_t *payload, uint8_t *len) {
    uint8_t offset = 0;
    for (uint8_t i = 0; i < 8; ++i, offset += 2) put_u16(&payload[offset], s->raw[i]);
    for (uint8_t i = 0; i < 8; ++i, offset += 2) put_u16(&payload[offset], s->norm[i]);
    put_u16(&payload[offset], s->lat_l_raw); offset += 2;
    put_u16(&payload[offset], s->lat_l_norm); offset += 2;
    put_u16(&payload[offset], s->lat_r_raw); offset += 2;
    put_u16(&payload[offset], s->lat_r_norm); offset += 2;
    *len = offset;
    return true;
}

static void send_packet(uint8_t type, const uint8_t *payload, uint8_t len) {
    uint8_t frame[PEGASUS_UART_MAX_PAYLOAD + 4u];
    size_t frame_len = 0;
    if (pegasus_encode_packet(type, payload, len, frame, sizeof(frame), &frame_len)) {
        pegasus_hal_uart_write(frame, (uint16_t)frame_len);
    }
}

void telemetry_send_periodic(robot_context_t *ctx, const motor_state_t *motor, const fan_state_t *fan, uint32_t now_ms) {
    uint8_t payload[PEGASUS_UART_MAX_PAYLOAD];
    uint8_t len = 0;
    if (now_ms - last_basic_ms >= 1000u / PEGASUS_TELEMETRY_HZ) {
        last_basic_ms = now_ms;
        telemetry_build_basic(ctx, motor, fan, payload, &len);
        send_packet(MSG_TELEMETRY_BASIC, payload, len);
    }
    if (now_ms - last_sensor_ms >= 100u) {
        last_sensor_ms = now_ms;
        telemetry_build_sensors(&ctx->sensors, payload, &len);
        send_packet(MSG_SENSOR_VALUES, payload, len);
    }
}
