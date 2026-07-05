#include "packet_parser.h"

namespace pegasus {

void PacketParser::reset() {
    state_ = WAIT_SOF;
    packet_ = Packet{};
    index_ = 0;
}

bool PacketParser::push(uint8_t byte, Packet &packet) {
    switch (state_) {
    case WAIT_SOF:
        if (byte == kSof) {
            packet_ = Packet{};
            index_ = 0;
            state_ = READ_TYPE;
        }
        break;
    case READ_TYPE:
        packet_.type = byte;
        state_ = READ_LEN;
        break;
    case READ_LEN:
        if (byte > kMaxPayloadLen) {
            reset();
            break;
        }
        packet_.len = byte;
        state_ = packet_.len == 0 ? READ_CRC : READ_PAYLOAD;
        break;
    case READ_PAYLOAD:
        packet_.payload[index_++] = byte;
        if (index_ >= packet_.len) {
            state_ = READ_CRC;
        }
        break;
    case READ_CRC: {
        uint8_t check[kMaxPayloadLen + 2];
        check[0] = packet_.type;
        check[1] = packet_.len;
        for (uint8_t i = 0; i < packet_.len; ++i) {
            check[2 + i] = packet_.payload[i];
        }
        const bool ok = crc8(check, static_cast<size_t>(packet_.len) + 2) == byte;
        if (ok) {
            packet = packet_;
        }
        reset();
        return ok;
    }
    }
    return false;
}

}  // namespace pegasus
