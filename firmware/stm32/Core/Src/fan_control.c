#include "fan_control.h"
#include "pegasus_hal.h"
#include <string.h>

static uint16_t clamp_pwm(uint16_t value, uint16_t max) {
    return value > max ? max : value;
}

void fan_init(fan_state_t *fan) {
    memset(fan, 0, sizeof(*fan));
    pegasus_hal_fan_pwm(0);
}

void fan_off(fan_state_t *fan) {
    fan->output = 0;
    pegasus_hal_fan_pwm(0);
}

void fan_spinup(fan_state_t *fan, const robot_config_t *config) {
    if (config->fan_mode == FAN_MODE_OFF) {
        fan_off(fan);
        return;
    }
    fan->output = clamp_pwm(config->fan_pwm_start, config->fan_pwm_max);
    pegasus_hal_fan_pwm(fan->output);
}

void fan_run(fan_state_t *fan, const robot_config_t *config) {
    if (config->fan_mode == FAN_MODE_OFF) {
        fan_off(fan);
        return;
    }
    fan->output = clamp_pwm(config->fan_pwm_run, config->fan_pwm_max);
    pegasus_hal_fan_pwm(fan->output);
}
