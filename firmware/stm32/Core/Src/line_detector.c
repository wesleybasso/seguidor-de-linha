#include "line_detector.h"
#include <string.h>

void line_detector_init(line_detector_t *detector) {
    memset(detector, 0, sizeof(*detector));
}

void line_detector_update(line_detector_t *d, sensor_state_t *s, const robot_config_t *config, uint32_t now_ms, uint32_t dt_ms) {
    const bool crossing_candidate =
        s->strong_count >= config->crossing_min_count &&
        s->span >= config->crossing_min_span &&
        s->front_sum >= config->crossing_sum_threshold;

    if (crossing_candidate) {
        d->crossing_ms += dt_ms;
    } else {
        d->crossing_ms = 0;
    }
    s->crossing_detected = d->crossing_ms >= config->crossing_debounce_ms;

    if (s->line_lost) {
        d->line_lost_ms += dt_ms;
    } else {
        d->line_lost_ms = 0;
    }
    s->line_lost_confirmed = d->line_lost_ms >= config->line_lost_timeout_ms;

    if (s->crossing_detected) {
        s->left_curve_valid = false;
        s->right_marker_valid = false;
        d->lateral_lockout_until_ms = now_ms + config->lateral_lockout_after_crossing_ms;
        return;
    }

    const bool lateral_locked = now_ms < d->lateral_lockout_until_ms;
    s->left_curve_valid = s->lat_l_active && !lateral_locked;
    s->right_marker_valid = s->lat_r_active && !lateral_locked;
}
