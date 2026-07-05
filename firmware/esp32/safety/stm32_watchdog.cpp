#include "stm32_watchdog.h"
#include "../app_config.h"

Stm32Watchdog stm32_watchdog;

void Stm32Watchdog::mark_packet(uint32_t now_ms) {
    last_packet_ms_ = now_ms;
}

bool Stm32Watchdog::online(uint32_t now_ms) const {
    return last_packet_ms_ != 0 && (now_ms - last_packet_ms_) <= PEGASUS_STM32_TIMEOUT_MS;
}

void Stm32Watchdog::apply(pegasus_telemetry_t &telemetry, uint32_t now_ms) const {
    telemetry.stm32_online = online(now_ms);
}
