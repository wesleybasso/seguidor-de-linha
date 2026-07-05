#include "calibration.h"
#include <string.h>

void calibration_begin(calibration_state_t *cal, sensor_state_t *sensors, uint32_t now_ms) {
    memset(cal, 0, sizeof(*cal));
    cal->active = true;
    cal->started_ms = now_ms;
    for (uint8_t i = 0; i < 8; ++i) {
        sensors->min[i] = 4095;
        sensors->max[i] = 0;
        sensors->noise[i] = 0;
    }
    sensors->lat_l_min = 4095;
    sensors->lat_l_max = 0;
    sensors->lat_r_min = 4095;
    sensors->lat_r_max = 0;
}

void calibration_sample(calibration_state_t *cal, sensor_state_t *s) {
    if (!cal->active) return;
    cal->samples++;
    for (uint8_t i = 0; i < 8; ++i) {
        if (s->filtered[i] < s->min[i]) s->min[i] = s->filtered[i];
        if (s->filtered[i] > s->max[i]) s->max[i] = s->filtered[i];
    }
    if (s->lat_l_filtered < s->lat_l_min) s->lat_l_min = s->lat_l_filtered;
    if (s->lat_l_filtered > s->lat_l_max) s->lat_l_max = s->lat_l_filtered;
    if (s->lat_r_filtered < s->lat_r_min) s->lat_r_min = s->lat_r_filtered;
    if (s->lat_r_filtered > s->lat_r_max) s->lat_r_max = s->lat_r_filtered;
}

bool calibration_finish(calibration_state_t *cal, sensor_state_t *s) {
    bool ok = true;
    for (uint8_t i = 0; i < 8; ++i) {
        const uint16_t range = s->max[i] > s->min[i] ? (uint16_t)(s->max[i] - s->min[i]) : 0u;
        cal->sensor_fault[i] = range < SENSOR_MIN_RANGE;
        if (cal->sensor_fault[i]) ok = false;
    }
    cal->sensor_fault[8] = (uint16_t)(s->lat_l_max - s->lat_l_min) < SENSOR_MIN_RANGE;
    cal->sensor_fault[9] = (uint16_t)(s->lat_r_max - s->lat_r_min) < SENSOR_MIN_RANGE;
    cal->active = false;
    return ok;
}
