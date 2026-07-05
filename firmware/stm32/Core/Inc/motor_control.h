#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

typedef struct {
    int16_t left_target;
    int16_t right_target;
    int16_t left_output;
    int16_t right_output;
} motor_state_t;

void motor_init(motor_state_t *motor);
void motor_stop_coast(motor_state_t *motor);
void motor_stop_brake(motor_state_t *motor);
void motor_set_targets_from_pid(motor_state_t *motor, const robot_config_t *config, float correction);
void motor_apply(motor_state_t *motor, const robot_config_t *config);
