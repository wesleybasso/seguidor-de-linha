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

/*
 * Bench build without real ADC: generate a fake line sweeping across the
 * 8 front sensors so the dashboard shows live data (Fase 0, telemetria fake).
 * The real DMA/ADC implementation overrides this weak symbol.
 */
WEAK void pegasus_hal_get_adc_snapshot(uint16_t *values, uint8_t count) {
    static uint32_t phase = 0;
    phase += 4;

    /* Triangle wave 0..7000 -> line center sweeping S0..S7 and back. */
    uint32_t t = phase % 14000u;
    uint32_t center_milli = t < 7000u ? t : 14000u - t;

    for (uint8_t i = 0; i < count; ++i) {
        values[i] = 200; /* dark floor with small offset */
    }
    for (uint8_t i = 0; i < 8 && i < count; ++i) {
        int32_t sensor_pos = (int32_t)i * 1000;
        int32_t dist = sensor_pos - (int32_t)center_milli;
        if (dist < 0) dist = -dist;
        if (dist < 1500) {
            values[i] = (uint16_t)(200 + (3600 * (1500 - dist)) / 1500);
        }
    }
    /* Lateral sensors stay inactive in fake mode. */
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
