#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define PEGASUS_UART_SOF 0xA5u
#define PEGASUS_UART_MAX_PAYLOAD 64u

typedef enum {
    MSG_TELEMETRY_BASIC = 0x01,
    MSG_SENSOR_VALUES = 0x02,
    MSG_SET_PID = 0x03,
    MSG_SET_MOTOR_CONFIG = 0x04,
    MSG_SET_FAN_CONFIG = 0x05,
    MSG_CALIBRATION_START = 0x06,
    MSG_CALIBRATION_STOP = 0x07,
    MSG_START_RUN = 0x08,
    MSG_STOP_RUN = 0x09,
    MSG_ARM = 0x0A,
    MSG_DISARM = 0x0B,
    MSG_HARDWARE_TEST = 0x0C,
    MSG_ERROR_STATUS = 0x0D,
    MSG_ACK = 0x0E,
    MSG_NACK = 0x0F,
    MSG_PING = 0x10,
    MSG_PONG = 0x11
} pegasus_msg_type_t;

typedef struct {
    uint8_t type;
    uint8_t len;
    uint8_t payload[PEGASUS_UART_MAX_PAYLOAD];
} pegasus_packet_t;

typedef struct {
    uint8_t state;
    uint8_t index;
    pegasus_packet_t packet;
} pegasus_parser_t;

uint8_t pegasus_crc8(const uint8_t *data, size_t len);
bool pegasus_encode_packet(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out, size_t out_capacity, size_t *out_len);
void pegasus_parser_init(pegasus_parser_t *parser);
bool pegasus_parser_push(pegasus_parser_t *parser, uint8_t byte, pegasus_packet_t *out);
