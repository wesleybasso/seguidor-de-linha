#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "sensor_processing.h"

#define SENSOR_MIN_RANGE 120u

typedef struct {
    bool active;
    uint32_t started_ms;
    uint32_t samples;
    bool sensor_fault[10];
} calibration_state_t;

void calibration_begin(calibration_state_t *cal, sensor_state_t *sensors, uint32_t now_ms);
void calibration_sample(calibration_state_t *cal, sensor_state_t *sensors);
bool calibration_finish(calibration_state_t *cal, sensor_state_t *sensors);
