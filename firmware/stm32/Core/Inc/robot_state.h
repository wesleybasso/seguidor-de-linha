#pragma once

#include <stdbool.h>
#include <stdint.h>
#include "config.h"
#include "fan_control.h"
#include "motor_control.h"
#include "sensor_processing.h"

typedef enum {
    ST_BOOT = 0,
    ST_SELF_TEST,
    ST_IDLE,
    ST_CALIBRATION,
    ST_READY,
    ST_ARMED,
    ST_FAN_SPINUP,
    ST_RUNNING,
    ST_CROSSING,
    ST_CURVE_LEFT,
    ST_LINE_LOST,
    ST_FINISH_DETECTED,
    ST_SAFE_STOP,
    ST_HARDWARE_TEST,
    ST_ERROR
} robot_state_t;

typedef struct {
    robot_state_t state;
    uint32_t state_enter_ms;
    uint32_t run_start_ms;
    uint32_t run_time_ms;
    uint32_t last_control_ms;
    uint32_t last_esp_packet_ms;
    uint32_t error_flags;
    bool stop_requested;
    bool arm_requested;
    bool start_requested;
    bool disarm_requested;
    bool esp_online;
    sensor_state_t sensors;
    robot_config_t config;
} robot_context_t;

void robot_init(robot_context_t *ctx);
void robot_control_tick(robot_context_t *ctx, uint32_t now_ms);
void robot_set_state(robot_context_t *ctx, robot_state_t state, uint32_t now_ms);
void robot_request_stop(robot_context_t *ctx);
void robot_request_arm(robot_context_t *ctx);
void robot_request_disarm(robot_context_t *ctx);
void robot_request_start(robot_context_t *ctx);
/* Returns false when the state machine refuses the bench test (safety). */
bool robot_request_hardware_test(robot_context_t *ctx, uint32_t now_ms);
const motor_state_t *robot_motor_state(void);
const fan_state_t *robot_fan_state(void);
void robot_begin_calibration(robot_context_t *ctx, uint32_t now_ms);
void robot_finish_calibration(robot_context_t *ctx, uint32_t now_ms);
