#include "pegasus_hal.h"

#if defined(__GNUC__)
#define WEAK __attribute__((weak))
#else
#define WEAK
#endif

/*
 * Rev A0 weak hardware adapter.
 * Replace these functions with STM32Cube/HAL implementations after CubeMX
 * freezes ADC, timer, PWM, GPIO and UART instances.
 */

WEAK void pegasus_hal_init(void) {}

WEAK uint32_t pegasus_hal_millis(void) {
    static uint32_t fake_ms = 0;
    fake_ms += 2;
    return fake_ms;
}

WEAK bool pegasus_hal_control_tick_due(void) {
    return true;
}

WEAK bool pegasus_hal_uart_read_byte(uint8_t *byte) {
    (void)byte;
    return false;
}

WEAK void pegasus_hal_uart_write(const uint8_t *data, uint16_t len) {
    (void)data;
    (void)len;
}

WEAK void pegasus_hal_get_adc_snapshot(uint16_t *values, uint8_t count) {
    for (uint8_t i = 0; i < count; ++i) {
        values[i] = 0;
    }
}

WEAK void pegasus_hal_motor_left(int16_t pwm) {
    (void)pwm;
}

WEAK void pegasus_hal_motor_right(int16_t pwm) {
    (void)pwm;
}

WEAK void pegasus_hal_motor_brake(void) {}
WEAK void pegasus_hal_motor_coast(void) {}

WEAK void pegasus_hal_tb6612_stby(bool enable) {
    (void)enable;
}

WEAK void pegasus_hal_fan_pwm(uint16_t pwm) {
    (void)pwm;
}

WEAK void pegasus_hal_watchdog_refresh(void) {}
