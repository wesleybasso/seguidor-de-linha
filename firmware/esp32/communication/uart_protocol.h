#pragma once

#include <Arduino.h>

namespace pegasus {

constexpr uint8_t kSof = 0xA5;
constexpr size_t kMaxPayloadLen = 64;

enum MessageType : uint8_t {
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
};

struct Packet {
    uint8_t type = 0;
    uint8_t len = 0;
    uint8_t payload[kMaxPayloadLen] = {};
};

uint8_t crc8(const uint8_t *data, size_t len);
bool encode_packet(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out, size_t out_capacity, size_t *out_len);
bool write_packet(Stream &stream, uint8_t type, const uint8_t *payload, uint8_t len);

}  // namespace pegasus
