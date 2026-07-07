#include "robot_state.h"
#include "board_pins.h"
#include "calibration.h"
#include "line_detector.h"
#include "pegasus_hal.h"
#include "pid_controller.h"
#include "safety.h"
#include <string.h>

static motor_state_t g_motor;
static fan_state_t g_fan;
static pid_controller_t g_pid;
static line_detector_t g_line_detector;
static calibration_state_t g_calibration;

static uint32_t state_age(const robot_context_t *ctx, uint32_t now_ms) {
    return now_ms - ctx->state_enter_ms;
}

const motor_state_t *robot_motor_state(void) {
    return &g_motor;
}

const fan_state_t *robot_fan_state(void) {
    return &g_fan;
}

void robot_set_state(robot_context_t *ctx, robot_state_t state, uint32_t now_ms) {
    if (ctx->state == state) return;
    ctx->state = state;
    ctx->state_enter_ms = now_ms;
}

void robot_request_stop(robot_context_t *ctx) {
    ctx->stop_requested = true;
}

void robot_request_arm(robot_context_t *ctx) {
    ctx->arm_requested = true;
}

void robot_request_disarm(robot_context_t *ctx) {
    ctx->disarm_requested = true;
}

void robot_request_start(robot_context_t *ctx) {
    ctx->start_requested = true;
}

bool robot_request_hardware_test(robot_context_t *ctx, uint32_t now_ms) {
    if (ctx->state != ST_IDLE && ctx->state != ST_READY) {
        return false;
    }
    robot_set_state(ctx, ST_HARDWARE_TEST, now_ms);
    return true;
}

void robot_begin_calibration(robot_context_t *ctx, uint32_t now_ms) {
    calibration_begin(&g_calibration, &ctx->sensors, now_ms);
    robot_set_state(ctx, ST_CALIBRATION, now_ms);
}

void robot_finish_calibration(robot_context_t *ctx, uint32_t now_ms) {
    if (calibration_finish(&g_calibration, &ctx->sensors)) {
        robot_set_state(ctx, ST_READY, now_ms);
    } else {
        ctx->error_flags |= ERR_SENSOR_FAULT;
        robot_set_state(ctx, ST_ERROR, now_ms);
    }
}

void robot_init(robot_context_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));
    ctx->state = ST_BOOT;
    ctx->config = robot_default_config();
    sensor_state_init(&ctx->sensors);
    motor_init(&g_motor);
    fan_init(&g_fan);
    pid_init(&g_pid);
    line_detector_init(&g_line_detector);
}

static void update_sensors(robot_context_t *ctx, uint32_t dt_ms) {
    uint16_t adc[PEGASUS_SENSOR_TOTAL_COUNT] = {0};
    pegasus_hal_get_adc_snapshot(adc, PEGASUS_SENSOR_TOTAL_COUNT);
    sensor_set_raw_from_adc(&ctx->sensors, adc, PEGASUS_SENSOR_TOTAL_COUNT);
    sensor_filter(&ctx->sensors, 3, 10);
    sensor_normalize(&ctx->sensors, &ctx->config, false);
    sensor_extract_features(&ctx->sensors, &ctx->config, dt_ms);
}

static void run_line_control(robot_context_t *ctx, uint32_t dt_ms, bool freeze_integral) {
    const float dt_s = (float)dt_ms / 1000.0f;
    const float correction = pid_update(&g_pid, &ctx->config, (float)ctx->sensors.error, dt_s, freeze_integral);
    motor_set_targets_from_pid(&g_motor, &ctx->config, correction);
    motor_apply(&g_motor, &ctx->config);
}

static void safe_outputs(void) {
    motor_stop_brake(&g_motor);
    fan_off(&g_fan);
    pid_reset(&g_pid);
}

void robot_control_tick(robot_context_t *ctx, uint32_t now_ms) {
    uint32_t dt_ms = now_ms - ctx->last_control_ms;
    if (dt_ms == 0 || dt_ms > 100) dt_ms = 1000u / PEGASUS_CONTROL_HZ;
    ctx->last_control_ms = now_ms;

    update_sensors(ctx, dt_ms);
    line_detector_update(&g_line_detector, &ctx->sensors, &ctx->config, now_ms, dt_ms);

    if (ctx->stop_requested || safety_has_critical_fault(ctx, now_ms)) {
        robot_set_state(ctx, ST_SAFE_STOP, now_ms);
        ctx->stop_requested = false;
    }

    switch (ctx->state) {
    case ST_BOOT:
        safe_outputs();
        robot_set_state(ctx, ST_SELF_TEST, now_ms);
        break;
    case ST_SELF_TEST:
        if (!robot_config_valid(&ctx->config)) {
            ctx->error_flags |= ERR_CONFIG_INVALID;
            robot_set_state(ctx, ST_ERROR, now_ms);
        } else {
            robot_set_state(ctx, ST_IDLE, now_ms);
        }
        break;
    case ST_IDLE:
        safe_outputs();
        if (ctx->arm_requested) {
            ctx->arm_requested = false;
            robot_set_state(ctx, ST_ARMED, now_ms);
        }
        break;
    case ST_CALIBRATION:
        calibration_sample(&g_calibration, &ctx->sensors);
        motor_stop_coast(&g_motor);
        fan_off(&g_fan);
        break;
    case ST_READY:
        motor_stop_coast(&g_motor);
        fan_off(&g_fan);
        if (ctx->arm_requested) {
            ctx->arm_requested = false;
            robot_set_state(ctx, ST_ARMED, now_ms);
        }
        break;
    case ST_ARMED:
        motor_stop_coast(&g_motor);
        if (ctx->disarm_requested) {
            ctx->disarm_requested = false;
            robot_set_state(ctx, ST_READY, now_ms);
        } else if (ctx->start_requested || ctx->sensors.right_marker_valid) {
            ctx->start_requested = false;
            robot_set_state(ctx, ST_FAN_SPINUP, now_ms);
        }
        break;
    case ST_FAN_SPINUP:
        fan_spinup(&g_fan, &ctx->config);
        motor_stop_coast(&g_motor);
        if (state_age(ctx, now_ms) >= ctx->config.fan_start_delay_ms) {
            ctx->run_start_ms = now_ms;
            robot_set_state(ctx, ST_RUNNING, now_ms);
        }
        break;
    case ST_RUNNING:
        ctx->run_time_ms = now_ms - ctx->run_start_ms;
        fan_run(&g_fan, &ctx->config);
        if (ctx->sensors.crossing_detected) {
            robot_set_state(ctx, ST_CROSSING, now_ms);
        } else if (ctx->sensors.line_lost_confirmed) {
            robot_set_state(ctx, ST_LINE_LOST, now_ms);
        } else if (ctx->sensors.right_marker_valid && ctx->run_time_ms > ctx->config.finish_min_run_time_ms) {
            robot_set_state(ctx, ST_FINISH_DETECTED, now_ms);
        } else if (ctx->sensors.left_curve_valid) {
            robot_set_state(ctx, ST_CURVE_LEFT, now_ms);
        } else {
            run_line_control(ctx, dt_ms, false);
        }
        break;
    case ST_CROSSING:
        fan_run(&g_fan, &ctx->config);
        run_line_control(ctx, dt_ms, true);
        if (!ctx->sensors.crossing_detected) {
            robot_set_state(ctx, ST_RUNNING, now_ms);
        }
        break;
    case ST_CURVE_LEFT:
        fan_run(&g_fan, &ctx->config);
        run_line_control(ctx, dt_ms, false);
        if (ctx->sensors.crossing_detected) {
            robot_set_state(ctx, ST_CROSSING, now_ms);
        } else if (ctx->sensors.line_lost_confirmed) {
            robot_set_state(ctx, ST_LINE_LOST, now_ms);
        } else if (state_age(ctx, now_ms) > 250u) {
            robot_set_state(ctx, ST_RUNNING, now_ms);
        }
        break;
    case ST_LINE_LOST:
        fan_run(&g_fan, &ctx->config);
        g_motor.left_target = ctx->sensors.last_valid_error >= 0 ? ctx->config.base_speed / 2 : -ctx->config.base_speed / 2;
        g_motor.right_target = ctx->sensors.last_valid_error >= 0 ? -ctx->config.base_speed / 2 : ctx->config.base_speed / 2;
        motor_apply(&g_motor, &ctx->config);
        if (ctx->sensors.line_detected && !ctx->sensors.line_lost) {
            robot_set_state(ctx, ST_RUNNING, now_ms);
        } else if (state_age(ctx, now_ms) > 600u) {
            robot_set_state(ctx, ST_SAFE_STOP, now_ms);
        }
        break;
    case ST_FINISH_DETECTED:
        safe_outputs();
        robot_set_state(ctx, ST_SAFE_STOP, now_ms);
        break;
    case ST_SAFE_STOP:
        safe_outputs();
        if (ctx->disarm_requested) {
            ctx->disarm_requested = false;
            ctx->error_flags = 0;
            robot_set_state(ctx, ST_IDLE, now_ms);
        }
        break;
    case ST_HARDWARE_TEST:
        if (state_age(ctx, now_ms) > 3000u) {
            ctx->error_flags |= ERR_HARDWARE_TEST_TO;
            robot_set_state(ctx, ST_SAFE_STOP, now_ms);
        }
        break;
    case ST_ERROR:
    default:
        safe_outputs();
        break;
    }
}
