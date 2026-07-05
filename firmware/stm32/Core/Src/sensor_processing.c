#include "sensor_processing.h"
#include <string.h>

static const int16_t kWeights[8] = {-3500, -2500, -1500, -500, 500, 1500, 2500, 3500};

static uint16_t clamp_u16(int32_t value, uint16_t lo, uint16_t hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return (uint16_t)value;
}

void sensor_state_init(sensor_state_t *s) {
    memset(s, 0, sizeof(*s));
    for (uint8_t i = 0; i < 8; ++i) {
        s->min[i] = 4095;
        s->max[i] = 0;
    }
    s->lat_l_min = 4095;
    s->lat_r_min = 4095;
}

void sensor_set_raw_from_adc(sensor_state_t *s, const uint16_t *adc_values, uint8_t count) {
    for (uint8_t i = 0; i < 8 && i < count; ++i) {
        s->raw[i] = adc_values[i];
    }
    if (count > 8) s->lat_l_raw = adc_values[8];
    if (count > 9) s->lat_r_raw = adc_values[9];
}

void sensor_filter(sensor_state_t *s, uint8_t alpha_num, uint8_t alpha_den) {
    if (alpha_den == 0) alpha_den = 10;
    for (uint8_t i = 0; i < 8; ++i) {
        s->filtered[i] = (uint16_t)(((uint32_t)s->filtered[i] * (alpha_den - alpha_num) + (uint32_t)s->raw[i] * alpha_num) / alpha_den);
    }
    s->lat_l_filtered = (uint16_t)(((uint32_t)s->lat_l_filtered * (alpha_den - alpha_num) + (uint32_t)s->lat_l_raw * alpha_num) / alpha_den);
    s->lat_r_filtered = (uint16_t)(((uint32_t)s->lat_r_filtered * (alpha_den - alpha_num) + (uint32_t)s->lat_r_raw * alpha_num) / alpha_den);
}

static uint16_t normalize_one(uint16_t raw, uint16_t min_v, uint16_t max_v, bool invert) {
    const uint16_t range = max_v > min_v ? (uint16_t)(max_v - min_v) : 1u;
    int32_t norm = ((int32_t)raw - min_v) * 1000 / range;
    norm = clamp_u16(norm, 0, 1000);
    return invert ? (uint16_t)(1000 - norm) : (uint16_t)norm;
}

void sensor_normalize(sensor_state_t *s, const robot_config_t *config, bool invert_line) {
    (void)config;
    for (uint8_t i = 0; i < 8; ++i) {
        s->norm[i] = normalize_one(s->filtered[i], s->min[i], s->max[i], invert_line);
    }
    s->lat_l_norm = normalize_one(s->lat_l_filtered, s->lat_l_min, s->lat_l_max, invert_line);
    s->lat_r_norm = normalize_one(s->lat_r_filtered, s->lat_r_min, s->lat_r_max, invert_line);
}

void sensor_extract_features(sensor_state_t *s, const robot_config_t *config, uint32_t dt_ms) {
    (void)dt_ms;
    s->active_count = 0;
    s->strong_count = 0;
    s->first_active = 255;
    s->last_active = 0;
    s->span = 0;
    s->front_sum = 0;

    int32_t weighted = 0;
    for (uint8_t i = 0; i < 8; ++i) {
        const uint16_t norm = s->norm[i];
        s->front_sum += norm;
        weighted += (int32_t)norm * kWeights[i];
        if (norm > config->sensor_threshold[i]) {
            if (s->first_active == 255) s->first_active = i;
            s->last_active = i;
            s->active_count++;
        }
        if (norm > config->sensor_strong_threshold[i]) {
            s->strong_count++;
        }
    }

    if (s->active_count > 0) {
        s->span = (uint8_t)(s->last_active - s->first_active + 1u);
        s->line_detected = true;
    } else {
        s->first_active = 0;
        s->last_active = 0;
        s->line_detected = false;
    }

    if (s->front_sum > 0) {
        s->position = (int16_t)(weighted / (int32_t)s->front_sum);
        s->error = (int16_t)(0 - s->position);
        s->last_valid_error = s->error;
    }

    s->line_lost = s->front_sum < config->lost_line_sum_threshold;
    s->lat_l_active = s->lat_l_norm > config->lat_l_threshold;
    s->lat_r_active = s->lat_r_norm > config->lat_r_threshold;
}
