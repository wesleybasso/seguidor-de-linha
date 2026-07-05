#include "motor_control.h"
#include "pegasus_hal.h"
#include <stdlib.h>
#include <string.h>

static int16_t clamp_i16(int32_t value, int16_t lo, int16_t hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return (int16_t)value;
}

static int16_t apply_min(int16_t value, int16_t min_abs) {
    if (value > 0 && value < min_abs) return min_abs;
    if (value < 0 && value > -min_abs) return (int16_t)-min_abs;
    return value;
}

static int16_t slew(int16_t target, int16_t previous, int16_t accel, int16_t brake) {
    const int16_t limit = abs(target) < abs(previous) ? brake : accel;
    const int16_t delta = target - previous;
    if (delta > limit) return previous + limit;
    if (delta < -limit) return previous - limit;
    return target;
}

void motor_init(motor_state_t *motor) {
    memset(motor, 0, sizeof(*motor));
    pegasus_hal_tb6612_stby(false);
    pegasus_hal_motor_coast();
}

void motor_stop_coast(motor_state_t *motor) {
    motor->left_target = 0;
    motor->right_target = 0;
    motor->left_output = 0;
    motor->right_output = 0;
    pegasus_hal_motor_coast();
    pegasus_hal_tb6612_stby(false);
}

void motor_stop_brake(motor_state_t *motor) {
    motor->left_target = 0;
    motor->right_target = 0;
    motor->left_output = 0;
    motor->right_output = 0;
    pegasus_hal_motor_brake();
}

void motor_set_targets_from_pid(motor_state_t *motor, const robot_config_t *config, float correction) {
    int32_t left = (int32_t)config->base_speed - (int32_t)correction;
    int32_t right = (int32_t)config->base_speed + (int32_t)correction;
    left = clamp_i16(left, (int16_t)-config->pwm_max_left, config->pwm_max_left);
    right = clamp_i16(right, (int16_t)-config->pwm_max_right, config->pwm_max_right);
    left = apply_min((int16_t)left, config->pwm_min_left);
    right = apply_min((int16_t)right, config->pwm_min_right);
    if (config->left_motor_inverted) left = -left;
    if (config->right_motor_inverted) right = -right;
    motor->left_target = (int16_t)left;
    motor->right_target = (int16_t)right;
}

void motor_apply(motor_state_t *motor, const robot_config_t *config) {
    motor->left_output = slew(motor->left_target, motor->left_output, config->accel_limit, config->brake_limit);
    motor->right_output = slew(motor->right_target, motor->right_output, config->accel_limit, config->brake_limit);
    pegasus_hal_tb6612_stby(motor->left_output != 0 || motor->right_output != 0);
    pegasus_hal_motor_left(motor->left_output);
    pegasus_hal_motor_right(motor->right_output);
}
