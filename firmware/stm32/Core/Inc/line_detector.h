#pragma once

#include <stdint.h>
#include "config.h"
#include "sensor_processing.h"

typedef struct {
    uint32_t crossing_ms;
    uint32_t line_lost_ms;
    uint32_t lateral_lockout_until_ms;
} line_detector_t;

void line_detector_init(line_detector_t *detector);
void line_detector_update(line_detector_t *detector, sensor_state_t *sensors, const robot_config_t *config, uint32_t now_ms, uint32_t dt_ms);
