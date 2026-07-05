#pragma once

#include <stddef.h>
#include <stdint.h>
#include "fan_control.h"
#include "motor_control.h"
#include "robot_state.h"
#include "uart_protocol.h"

bool telemetry_build_basic(const robot_context_t *ctx, const motor_state_t *motor, const fan_state_t *fan, uint8_t *payload, uint8_t *len);
bool telemetry_build_sensors(const sensor_state_t *sensors, uint8_t *payload, uint8_t *len);
void telemetry_send_periodic(robot_context_t *ctx, const motor_state_t *motor, const fan_state_t *fan, uint32_t now_ms);
