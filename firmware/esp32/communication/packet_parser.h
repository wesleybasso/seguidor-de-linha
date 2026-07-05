#pragma once

#include "uart_protocol.h"

namespace pegasus {

class PacketParser {
public:
    bool push(uint8_t byte, Packet &packet);
    void reset();

private:
    enum State : uint8_t {
        WAIT_SOF,
        READ_TYPE,
        READ_LEN,
        READ_PAYLOAD,
        READ_CRC
    };

    State state_ = WAIT_SOF;
    Packet packet_;
    uint8_t index_ = 0;
};

}  // namespace pegasus
