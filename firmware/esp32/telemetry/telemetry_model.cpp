#include "telemetry_model.h"
#include "../communication/uart_protocol.h"

static uint16_t u16le(const uint8_t *p) {
    return static_cast<uint16_t>(p[0] | (p[1] << 8));
}

static int16_t i16le(const uint8_t *p) {
    return static_cast<int16_t>(u16le(p));
}

static uint32_t u32le(const uint8_t *p) {
    return static_cast<uint32_t>(p[0]) |
           (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) |
           (static_cast<uint32_t>(p[3]) << 24);
}

const char *robot_state_name(robot_state_t state) {
    switch (state) {
    case ROBOT_BOOT: return "BOOT";
    case ROBOT_IDLE: return "IDLE";
    case ROBOT_CALIBRATION: return "CALIBRATION";
    case ROBOT_READY: return "READY";
    case ROBOT_ARMED: return "ARMED";
    case ROBOT_RUNNING: return "RUNNING";
    case ROBOT_CROSSING: return "CROSSING";
    case ROBOT_CURVE_LEFT: return "CURVE_LEFT";
    case ROBOT_FINISH_DETECTED: return "FINISH_DETECTED";
    case ROBOT_LINE_LOST: return "LINE_LOST";
    case ROBOT_SAFE_STOP: return "SAFE_STOP";
    case ROBOT_ERROR: return "ERROR";
    }
    return "UNKNOWN";
}

void telemetry_apply_packet(const uint8_t type, const uint8_t *payload, uint8_t len, pegasus_telemetry_t &t) {
    t.last_update_ms = millis();
    t.stm32_online = true;

    if (type == pegasus::MSG_TELEMETRY_BASIC && len >= 24) {
        t.state = static_cast<robot_state_t>(payload[0]);
        t.battery_mv = u16le(&payload[1]);
        t.line_position = i16le(&payload[3]);
        t.pid_error = i16le(&payload[5]);
        t.pid_correction = i16le(&payload[7]);
        t.pwm_left = i16le(&payload[9]);
        t.pwm_right = i16le(&payload[11]);
        t.fan_pwm = u16le(&payload[13]);
        const uint8_t flags = payload[15];
        t.line_detected = flags & 0x01;
        t.crossing_detected = flags & 0x02;
        t.lat_l_active = flags & 0x04;
        t.lat_r_active = flags & 0x08;
        t.line_lost = flags & 0x10;
        t.run_time_ms = u32le(&payload[16]);
        t.error_flags = u32le(&payload[20]);
    } else if (type == pegasus::MSG_SENSOR_VALUES && len >= 40) {
        uint8_t offset = 0;
        for (uint8_t i = 0; i < 8; ++i, offset += 2) {
            t.front_raw[i] = u16le(&payload[offset]);
        }
        for (uint8_t i = 0; i < 8; ++i, offset += 2) {
            t.front_norm[i] = u16le(&payload[offset]);
        }
        t.lat_l_raw = u16le(&payload[offset]); offset += 2;
        t.lat_l_norm = u16le(&payload[offset]); offset += 2;
        t.lat_r_raw = u16le(&payload[offset]); offset += 2;
        t.lat_r_norm = u16le(&payload[offset]);
    }
}

String telemetry_to_json(const pegasus_telemetry_t &t) {
    String s = "{";
    s += "\"state\":\"" + String(robot_state_name(t.state)) + "\",";
    s += "\"stm32_online\":" + String(t.stm32_online ? "true" : "false") + ",";
    s += "\"battery_mv\":" + String(t.battery_mv) + ",";
    s += "\"line_detected\":" + String(t.line_detected ? "true" : "false") + ",";
    s += "\"crossing_detected\":" + String(t.crossing_detected ? "true" : "false") + ",";
    s += "\"lat_l_active\":" + String(t.lat_l_active ? "true" : "false") + ",";
    s += "\"lat_r_active\":" + String(t.lat_r_active ? "true" : "false") + ",";
    s += "\"line_lost\":" + String(t.line_lost ? "true" : "false") + ",";
    s += "\"line_position\":" + String(t.line_position) + ",";
    s += "\"pid_error\":" + String(t.pid_error) + ",";
    s += "\"pid_correction\":" + String(t.pid_correction) + ",";
    s += "\"pwm_left\":" + String(t.pwm_left) + ",";
    s += "\"pwm_right\":" + String(t.pwm_right) + ",";
    s += "\"fan_pwm\":" + String(t.fan_pwm) + ",";
    s += "\"run_time_ms\":" + String(t.run_time_ms) + ",";
    s += "\"error_flags\":" + String(t.error_flags) + ",";
    s += "\"last_update_age_ms\":" + String(t.last_update_ms == 0 ? 0 : millis() - t.last_update_ms) + ",";
    s += "\"front_raw\":[";
    for (uint8_t i = 0; i < 8; ++i) {
        if (i) s += ",";
        s += String(t.front_raw[i]);
    }
    s += "],\"front_norm\":[";
    for (uint8_t i = 0; i < 8; ++i) {
        if (i) s += ",";
        s += String(t.front_norm[i]);
    }
    s += "],\"lat_l_raw\":" + String(t.lat_l_raw) + ",";
    s += "\"lat_l_norm\":" + String(t.lat_l_norm) + ",";
    s += "\"lat_r_raw\":" + String(t.lat_r_raw) + ",";
    s += "\"lat_r_norm\":" + String(t.lat_r_norm);
    s += "}";
    return s;
}
