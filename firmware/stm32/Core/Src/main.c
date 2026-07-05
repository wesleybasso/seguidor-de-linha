#include "main.h"
#include "config.h"
#include "pegasus_hal.h"
#include "robot_state.h"
#include "telemetry.h"
#include "uart_protocol.h"
#include <string.h>

static robot_context_t g_robot;
static pegasus_parser_t g_parser;

static void send_simple(uint8_t type, uint8_t about_type) {
    uint8_t payload[2] = {about_type, 0};
    uint8_t frame[PEGASUS_UART_MAX_PAYLOAD + 4u];
    size_t len = 0;
    if (pegasus_encode_packet(type, payload, sizeof(payload), frame, sizeof(frame), &len)) {
        pegasus_hal_uart_write(frame, (uint16_t)len);
    }
}

static float read_float_le(const uint8_t *p) {
    float v;
    memcpy(&v, p, sizeof(float));
    return v;
}

static int16_t read_i16_le(const uint8_t *p) {
    return (int16_t)((uint16_t)p[0] | ((uint16_t)p[1] << 8));
}

static void handle_packet(const pegasus_packet_t *packet) {
    g_robot.last_esp_packet_ms = pegasus_hal_millis();
    g_robot.esp_online = true;

    switch (packet->type) {
    case MSG_SET_PID:
        if (packet->len >= 12) {
            g_robot.config.kp = read_float_le(&packet->payload[0]);
            g_robot.config.ki = read_float_le(&packet->payload[4]);
            g_robot.config.kd = read_float_le(&packet->payload[8]);
            robot_config_finalize(&g_robot.config);
            send_simple(MSG_ACK, packet->type);
        } else {
            send_simple(MSG_NACK, packet->type);
        }
        break;
    case MSG_SET_MOTOR_CONFIG:
        if (packet->len >= 10) {
            g_robot.config.base_speed = read_i16_le(&packet->payload[0]);
            g_robot.config.pwm_min_left = read_i16_le(&packet->payload[2]);
            g_robot.config.pwm_min_right = read_i16_le(&packet->payload[4]);
            g_robot.config.pwm_max_left = read_i16_le(&packet->payload[6]);
            g_robot.config.pwm_max_right = read_i16_le(&packet->payload[8]);
            robot_config_finalize(&g_robot.config);
            send_simple(MSG_ACK, packet->type);
        } else {
            send_simple(MSG_NACK, packet->type);
        }
        break;
    case MSG_SET_FAN_CONFIG:
        if (packet->len >= 9) {
            g_robot.config.fan_pwm_start = (uint16_t)read_i16_le(&packet->payload[0]);
            g_robot.config.fan_pwm_run = (uint16_t)read_i16_le(&packet->payload[2]);
            g_robot.config.fan_pwm_max = (uint16_t)read_i16_le(&packet->payload[4]);
            g_robot.config.fan_start_delay_ms = (uint16_t)read_i16_le(&packet->payload[6]);
            g_robot.config.fan_mode = packet->payload[8];
            robot_config_finalize(&g_robot.config);
            send_simple(MSG_ACK, packet->type);
        } else {
            send_simple(MSG_NACK, packet->type);
        }
        break;
    case MSG_CALIBRATION_START:
        robot_begin_calibration(&g_robot, pegasus_hal_millis());
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_CALIBRATION_STOP:
        robot_finish_calibration(&g_robot, pegasus_hal_millis());
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_ARM:
        robot_request_arm(&g_robot);
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_DISARM:
        robot_request_disarm(&g_robot);
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_START_RUN:
        robot_request_start(&g_robot);
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_STOP_RUN:
        robot_request_stop(&g_robot);
        send_simple(MSG_ACK, packet->type);
        break;
    case MSG_PING:
        send_simple(MSG_PONG, packet->type);
        break;
    default:
        send_simple(MSG_NACK, packet->type);
        break;
    }
}

void pegasus_app_init(void) {
    pegasus_hal_init();
    robot_init(&g_robot);
    pegasus_parser_init(&g_parser);
}

void pegasus_app_loop(void) {
    uint8_t byte = 0;
    pegasus_packet_t packet;

    while (pegasus_hal_uart_read_byte(&byte)) {
        if (pegasus_parser_push(&g_parser, byte, &packet)) {
            handle_packet(&packet);
        }
    }

    if (pegasus_hal_control_tick_due()) {
        const uint32_t now = pegasus_hal_millis();
        robot_control_tick(&g_robot, now);
        telemetry_send_periodic(&g_robot, robot_motor_state(), robot_fan_state(), now);
        pegasus_hal_watchdog_refresh();
    }
}

int main(void) {
    pegasus_app_init();
    while (1) {
        pegasus_app_loop();
    }
}
