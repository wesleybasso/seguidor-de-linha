#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "robot_state.h"

#define ERR_CONFIG_INVALID     (1u << 0)
#define ERR_SENSOR_FAULT       (1u << 1)
#define ERR_ADC_FAULT          (1u << 2)
#define ERR_BATTERY_LOW        (1u << 3)
#define ERR_ESP_TIMEOUT        (1u << 4)
#define ERR_HARDWARE_TEST_TO   (1u << 5)

bool safety_has_critical_fault(const robot_context_t *ctx, uint32_t now_ms);
void safety_apply_stop(robot_context_t *ctx, uint32_t error_flags);
