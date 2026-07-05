#pragma once

#include <Arduino.h>
#include "../app_config.h"

struct pegasus_telemetry_t {
    robot_state_t state = ROBOT_BOOT;
    bool stm32_online = false;
    bool line_detected = false;
    bool crossing_detected = false;
    bool lat_l_active = false;
    bool lat_r_active = false;
    bool line_lost = false;

    uint16_t battery_mv = 0;
    uint16_t front_raw[8] = {};
    uint16_t front_norm[8] = {};
    uint16_t lat_l_raw = 0;
    uint16_t lat_l_norm = 0;
    uint16_t lat_r_raw = 0;
    uint16_t lat_r_norm = 0;

    int16_t line_position = 0;
    int16_t pid_error = 0;
    int16_t pid_correction = 0;
    int16_t pwm_left = 0;
    int16_t pwm_right = 0;
    uint16_t fan_pwm = 0;
    uint32_t run_time_ms = 0;
    uint32_t error_flags = 0;
    uint32_t last_update_ms = 0;
};

const char *robot_state_name(robot_state_t state);
void telemetry_apply_packet(const uint8_t type, const uint8_t *payload, uint8_t len, pegasus_telemetry_t &telemetry);
String telemetry_to_json(const pegasus_telemetry_t &telemetry);
