#pragma once

#include <Arduino.h>
#include "../telemetry/telemetry_model.h"

class Stm32Watchdog {
public:
    void mark_packet(uint32_t now_ms);
    bool online(uint32_t now_ms) const;
    void apply(pegasus_telemetry_t &telemetry, uint32_t now_ms) const;

private:
    uint32_t last_packet_ms_ = 0;
};

extern Stm32Watchdog stm32_watchdog;
