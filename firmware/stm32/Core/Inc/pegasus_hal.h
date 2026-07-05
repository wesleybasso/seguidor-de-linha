#pragma once

#include <stdbool.h>
#include <stdint.h>

void pegasus_hal_init(void);
uint32_t pegasus_hal_millis(void);
bool pegasus_hal_control_tick_due(void);
bool pegasus_hal_uart_read_byte(uint8_t *byte);
void pegasus_hal_uart_write(const uint8_t *data, uint16_t len);
void pegasus_hal_get_adc_snapshot(uint16_t *values, uint8_t count);
void pegasus_hal_motor_left(int16_t pwm);
void pegasus_hal_motor_right(int16_t pwm);
void pegasus_hal_motor_brake(void);
void pegasus_hal_motor_coast(void);
void pegasus_hal_tb6612_stby(bool enable);
void pegasus_hal_fan_pwm(uint16_t pwm);
void pegasus_hal_watchdog_refresh(void);
