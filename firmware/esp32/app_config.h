#pragma once

#include <Arduino.h>

#define PEGASUS_UART_BAUD 115200
#define PEGASUS_AP_SSID "PEGASUS_SETUP"
#define PEGASUS_AP_PASS "pegasus123"
#define PEGASUS_STM32_TIMEOUT_MS 500

#define PEGASUS_TELEMETRY_QUEUE_LEN 8
#define PEGASUS_COMMAND_QUEUE_LEN 12
#define PEGASUS_EVENT_QUEUE_LEN 12
#define PEGASUS_LOG_QUEUE_LEN 16

enum robot_state_t : uint8_t {
    ROBOT_BOOT = 0,
    ROBOT_IDLE,
    ROBOT_CALIBRATION,
    ROBOT_READY,
    ROBOT_ARMED,
    ROBOT_RUNNING,
    ROBOT_CROSSING,
    ROBOT_CURVE_LEFT,
    ROBOT_FINISH_DETECTED,
    ROBOT_LINE_LOST,
    ROBOT_SAFE_STOP,
    ROBOT_ERROR
};

enum pegasus_command_kind_t : uint8_t {
    CMD_SET_PID = 0,
    CMD_SET_MOTOR_CONFIG,
    CMD_SET_FAN_CONFIG,
    CMD_CALIBRATION_START,
    CMD_CALIBRATION_STOP,
    CMD_ARM,
    CMD_DISARM,
    CMD_START_RUN,
    CMD_STOP_RUN,
    CMD_HARDWARE_TEST,
    CMD_PING
};

struct pegasus_command_t {
    pegasus_command_kind_t kind;
    uint8_t payload[48];
    uint8_t len;
};

extern QueueHandle_t telemetry_queue;
extern QueueHandle_t command_queue;
extern QueueHandle_t event_queue;
extern QueueHandle_t log_queue;
