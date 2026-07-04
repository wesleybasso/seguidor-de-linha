# HFS-001 — Firmware Specification

Projeto: PEGASUS  
Revisão: A0

## 1. STM32F411

Responsável por:

- leitura A0–A7;
- leitura laterais;
- cálculo da posição da linha;
- PID;
- controle TB6612;
- controle FAN_PWM;
- comunicação UART;
- estado de segurança.

Estados:

- BOOT
- IDLE
- CALIBRATION
- READY
- RUN
- ERROR
- SAFE_STOP

## 2. ESP32-S3

Responsável por:

- telemetria;
- interface web futura;
- configuração de PID;
- log;
- comunicação USB/Wi-Fi/BLE.

Não deve controlar motores em tempo real.

## 3. Frequências sugeridas

- ADC: >= 1 kHz.
- PID: >= 1 kHz.
- Telemetria: 20–100 Hz.
- PWM motor: 20–50 kHz.
- PWM sucção:
  - MOSFET: 10–25 kHz.
  - ESC: sinal compatível com servo/ESC.
