#pragma once

#include <stdint.h>
#include "config.h"

typedef struct {
    uint16_t output;
} fan_state_t;

void fan_init(fan_state_t *fan);
void fan_off(fan_state_t *fan);
void fan_spinup(fan_state_t *fan, const robot_config_t *config);
void fan_run(fan_state_t *fan, const robot_config_t *config);
