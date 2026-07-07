#include "config.h"
#include <string.h>

#define PEGASUS_CONFIG_VERSION 0xA0000001u
#define PEGASUS_PWM_LIMIT 1000
#define PEGASUS_FAN_DELAY_LIMIT_MS 5000u

static float clamp_float(float value, float lo, float hi) {
    if (value != value) return lo;
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static int16_t clamp_i16(int16_t value, int16_t lo, int16_t hi) {
    if (value < lo) return lo;
    if (value > hi) return hi;
    return value;
}

static uint16_t clamp_u16(uint16_t value, uint16_t hi) {
    return value > hi ? hi : value;
}

robot_config_t robot_default_config(void) {
    robot_config_t c;
    memset(&c, 0, sizeof(c));
    c.kp = 0.55f;
    c.ki = 0.0f;
    c.kd = 0.10f;
    c.integral_limit = 500.0f;
    c.derivative_filter = 0.65f;
    for (uint8_t i = 0; i < 8; ++i) {
        c.sensor_threshold[i] = 350;
        c.sensor_strong_threshold[i] = 700;
    }
    c.lat_l_threshold = 500;
    c.lat_r_threshold = 500;
    c.crossing_sum_threshold = 3500;
    c.crossing_min_count = 5;
    c.crossing_min_span = 5;
    c.lost_line_sum_threshold = 250;
    c.base_speed = 350;
    c.pwm_min_left = 120;
    c.pwm_min_right = 120;
    c.pwm_max_left = 750;
    c.pwm_max_right = 750;
    c.accel_limit = 25;
    c.brake_limit = 60;
    c.left_motor_inverted = false;
    c.right_motor_inverted = false;
    c.fan_pwm_start = 300;
    c.fan_pwm_run = 500;
    c.fan_pwm_max = 800;
    c.fan_start_delay_ms = 400;
    c.fan_mode = FAN_MODE_OFF;
    c.finish_min_run_time_ms = 3000;
    c.line_lost_timeout_ms = 80;
    c.crossing_debounce_ms = 30;
    c.lateral_debounce_ms = 50;
    c.lateral_lockout_after_crossing_ms = 100;
    c.stop_on_esp_timeout = false;
    robot_config_finalize(&c);
    return c;
}

uint32_t robot_config_checksum(const robot_config_t *config) {
    robot_config_t copy = *config;
    copy.checksum = 0;
    const uint8_t *bytes = (const uint8_t *)&copy;
    uint32_t sum = 2166136261u;
    for (uint32_t i = 0; i < sizeof(copy); ++i) {
        sum ^= bytes[i];
        sum *= 16777619u;
    }
    return sum;
}

bool robot_config_valid(const robot_config_t *config) {
    return config->config_version == PEGASUS_CONFIG_VERSION &&
           config->checksum == robot_config_checksum(config);
}

void robot_config_finalize(robot_config_t *config) {
    config->kp = clamp_float(config->kp, 0.0f, 20.0f);
    config->ki = clamp_float(config->ki, 0.0f, 10.0f);
    config->kd = clamp_float(config->kd, 0.0f, 20.0f);
    config->integral_limit = clamp_float(config->integral_limit, 0.0f, 5000.0f);
    config->derivative_filter = clamp_float(config->derivative_filter, 0.0f, 1.0f);

    config->base_speed = clamp_i16(config->base_speed, -PEGASUS_PWM_LIMIT, PEGASUS_PWM_LIMIT);
    config->pwm_min_left = clamp_i16(config->pwm_min_left, 0, PEGASUS_PWM_LIMIT);
    config->pwm_min_right = clamp_i16(config->pwm_min_right, 0, PEGASUS_PWM_LIMIT);
    config->pwm_max_left = clamp_i16(config->pwm_max_left, 0, PEGASUS_PWM_LIMIT);
    config->pwm_max_right = clamp_i16(config->pwm_max_right, 0, PEGASUS_PWM_LIMIT);
    if (config->pwm_min_left > config->pwm_max_left) config->pwm_min_left = config->pwm_max_left;
    if (config->pwm_min_right > config->pwm_max_right) config->pwm_min_right = config->pwm_max_right;

    config->fan_pwm_max = clamp_u16(config->fan_pwm_max, PEGASUS_PWM_LIMIT);
    config->fan_pwm_start = clamp_u16(config->fan_pwm_start, config->fan_pwm_max);
    config->fan_pwm_run = clamp_u16(config->fan_pwm_run, config->fan_pwm_max);
    config->fan_start_delay_ms = clamp_u16(config->fan_start_delay_ms, PEGASUS_FAN_DELAY_LIMIT_MS);
    if (config->fan_mode > FAN_MODE_ESC_SERVO) config->fan_mode = FAN_MODE_OFF;

    config->config_version = PEGASUS_CONFIG_VERSION;
    config->checksum = 0;
    config->checksum = robot_config_checksum(config);
}
