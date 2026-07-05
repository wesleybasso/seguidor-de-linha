#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    FAN_MODE_OFF = 0,
    FAN_MODE_PWM_DC_MOSFET = 1,
    FAN_MODE_ESC_SERVO = 2
} fan_mode_t;

typedef struct {
    float kp;
    float ki;
    float kd;
    float integral_limit;
    float derivative_filter;

    uint16_t sensor_threshold[8];
    uint16_t sensor_strong_threshold[8];
    uint16_t lat_l_threshold;
    uint16_t lat_r_threshold;

    uint16_t crossing_sum_threshold;
    uint8_t crossing_min_count;
    uint8_t crossing_min_span;
    uint16_t lost_line_sum_threshold;

    int16_t base_speed;
    int16_t pwm_min_left;
    int16_t pwm_min_right;
    int16_t pwm_max_left;
    int16_t pwm_max_right;
    int16_t accel_limit;
    int16_t brake_limit;

    bool left_motor_inverted;
    bool right_motor_inverted;

    uint16_t fan_pwm_start;
    uint16_t fan_pwm_run;
    uint16_t fan_pwm_max;
    uint16_t fan_start_delay_ms;
    uint8_t fan_mode;

    uint16_t finish_min_run_time_ms;
    uint16_t line_lost_timeout_ms;
    uint16_t crossing_debounce_ms;
    uint16_t lateral_debounce_ms;
    uint16_t lateral_lockout_after_crossing_ms;

    bool stop_on_esp_timeout;

    uint32_t config_version;
    uint32_t checksum;
} robot_config_t;

robot_config_t robot_default_config(void);
uint32_t robot_config_checksum(const robot_config_t *config);
bool robot_config_valid(const robot_config_t *config);
void robot_config_finalize(robot_config_t *config);
