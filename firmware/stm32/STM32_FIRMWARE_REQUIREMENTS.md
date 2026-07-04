# STM32 Firmware Requirements

## Mínimo

- Piscar LED.
- Ler A0–A7.
- Ler LAT_L/LAT_R.
- Controlar TB6612.
- Controlar FAN_PWM.
- Comunicar via UART.
- SAFE_STOP.

## Arquivos sugeridos

- board_pins.h
- adc_sensors.c
- motor_tb6612.c
- fan_control.c
- pid_controller.c
- line_position.c
- uart_protocol.c
- robot_state_machine.c
