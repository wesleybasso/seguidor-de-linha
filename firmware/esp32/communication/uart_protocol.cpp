#include "uart_protocol.h"

namespace pegasus {

uint8_t crc8(const uint8_t *data, size_t len) {
    uint8_t crc = 0;
    for (size_t i = 0; i < len; ++i) {
        crc ^= data[i];
        for (uint8_t bit = 0; bit < 8; ++bit) {
            crc = (crc & 0x80) ? static_cast<uint8_t>((crc << 1) ^ 0x07) : static_cast<uint8_t>(crc << 1);
        }
    }
    return crc;
}

bool encode_packet(uint8_t type, const uint8_t *payload, uint8_t len, uint8_t *out, size_t out_capacity, size_t *out_len) {
    if (len > kMaxPayloadLen || out_capacity < static_cast<size_t>(len) + 4) {
        return false;
    }

    out[0] = kSof;
    out[1] = type;
    out[2] = len;
    for (uint8_t i = 0; i < len; ++i) {
        out[3 + i] = payload[i];
    }
    out[3 + len] = crc8(&out[1], static_cast<size_t>(len) + 2);
    *out_len = static_cast<size_t>(len) + 4;
    return true;
}

bool write_packet(Stream &stream, uint8_t type, const uint8_t *payload, uint8_t len) {
    uint8_t frame[kMaxPayloadLen + 4];
    size_t frame_len = 0;
    if (!encode_packet(type, payload, len, frame, sizeof(frame), &frame_len)) {
        return false;
    }
    return stream.write(frame, frame_len) == frame_len;
}

}  // namespace pegasus
