#include "pid_controller.h"
#include <string.h>

static float clampf(float v, float lo, float hi) {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

void pid_init(pid_controller_t *pid) {
    memset(pid, 0, sizeof(*pid));
}

void pid_reset(pid_controller_t *pid) {
    pid_init(pid);
}

float pid_update(pid_controller_t *pid, const robot_config_t *config, float error, float dt_s, bool freeze_integral) {
    if (dt_s <= 0.0f) dt_s = 0.002f;

    if (!freeze_integral) {
        pid->integral += error * dt_s;
        pid->integral = clampf(pid->integral, -config->integral_limit, config->integral_limit);
    }

    const float derivative = (error - pid->last_error) / dt_s;
    pid->derivative_filtered =
        pid->derivative_filtered * config->derivative_filter +
        derivative * (1.0f - config->derivative_filter);

    pid->last_error = error;
    pid->last_correction =
        config->kp * error +
        config->ki * pid->integral +
        config->kd * pid->derivative_filtered;
    return pid->last_correction;
}
