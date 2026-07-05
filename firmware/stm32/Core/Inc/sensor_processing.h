#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef struct {
    uint16_t raw[8];
    uint16_t filtered[8];
    uint16_t min[8];
    uint16_t max[8];
    uint16_t noise[8];
    uint16_t norm[8];

    uint16_t lat_l_raw;
    uint16_t lat_l_filtered;
    uint16_t lat_l_min;
    uint16_t lat_l_max;
    uint16_t lat_l_norm;

    uint16_t lat_r_raw;
    uint16_t lat_r_filtered;
    uint16_t lat_r_min;
    uint16_t lat_r_max;
    uint16_t lat_r_norm;

    uint8_t active_count;
    uint8_t strong_count;
    uint8_t first_active;
    uint8_t last_active;
    uint8_t span;

    uint32_t front_sum;
    int16_t position;
    int16_t error;
    int16_t last_valid_error;

    bool line_detected;
    bool line_lost;
    bool line_lost_confirmed;
    bool crossing_detected;
    bool lat_l_active;
    bool lat_r_active;
    bool left_curve_valid;
    bool right_marker_valid;
} sensor_state_t;

void sensor_state_init(sensor_state_t *state);
void sensor_set_raw_from_adc(sensor_state_t *state, const uint16_t *adc_values, uint8_t count);
void sensor_filter(sensor_state_t *state, uint8_t alpha_num, uint8_t alpha_den);
void sensor_normalize(sensor_state_t *state, const robot_config_t *config, bool invert_line);
void sensor_extract_features(sensor_state_t *state, const robot_config_t *config, uint32_t dt_ms);
