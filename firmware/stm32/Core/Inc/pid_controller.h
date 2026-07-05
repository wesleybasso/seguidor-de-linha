#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

typedef struct {
    float integral;
    float last_error;
    float derivative_filtered;
    float last_correction;
} pid_controller_t;

void pid_init(pid_controller_t *pid);
void pid_reset(pid_controller_t *pid);
float pid_update(pid_controller_t *pid, const robot_config_t *config, float error, float dt_s, bool freeze_integral);
