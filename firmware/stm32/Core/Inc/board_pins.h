#pragma once

/*
 * PEGASUS Rev A0 logical pin map from docs/HPS-001.
 * Configure the real STM32Cube GPIO/ADC/TIM instances to match these signals.
 *
 * Sensors:
 *   A0..A7  -> PA0..PA7
 *   LAT_L   -> PB0
 *   LAT_R   -> PB1
 *
 * TB6612:
 *   PWMA -> PB8
 *   PWMB -> PB9
 *   AIN1 -> PB10
 *   AIN2 -> PB11
 *   BIN1 -> PB12
 *   BIN2 -> PB13
 *   STBY -> PB14
 *
 * Fan:
 *   FAN_PWM -> PA8
 *
 * UART to ESP32:
 *   STM32_TX -> PA9
 *   STM32_RX -> PA10
 */

#define PEGASUS_SENSOR_FRONT_COUNT 8u
#define PEGASUS_SENSOR_TOTAL_COUNT 10u
#define PEGASUS_PWM_MAX 1000
#define PEGASUS_CONTROL_HZ 500u
#define PEGASUS_TELEMETRY_HZ 25u
