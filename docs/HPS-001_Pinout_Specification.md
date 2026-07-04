# HPS-001 — Pinout Specification

Projeto: PEGASUS  
Revisão: A0

## 1. STM32F411 — Black Pill

### Sensores centrais

| Sinal | Pino STM32 | Função |
|------|------------|--------|
| A0 | PA0 | Sensor central 0 |
| A1 | PA1 | Sensor central 1 |
| A2 | PA2 | Sensor central 2 |
| A3 | PA3 | Sensor central 3 |
| A4 | PA4 | Sensor central 4 |
| A5 | PA5 | Sensor central 5 |
| A6 | PA6 | Sensor central 6 |
| A7 | PA7 | Sensor central 7 |

### Sensores laterais

| Sinal | Pino STM32 | Função |
|------|------------|--------|
| LAT_L | PB0 | Sensor lateral esquerdo |
| LAT_R | PB1 | Sensor lateral direito |

Observação:
Com A0–A7 + LAT_L + LAT_R, os canais ADC práticos ficam ocupados.  
Medição de bateria deve ser opcional por divisor no ESP32, ADC externo ou multiplexação futura.

### TB6612

| Sinal | Pino STM32 | Função |
|------|------------|--------|
| PWMA | PB8 | PWM motor esquerdo |
| AIN1 | PB10 | Direção motor esquerdo 1 |
| AIN2 | PB11 | Direção motor esquerdo 2 |
| PWMB | PB9 | PWM motor direito |
| BIN1 | PB12 | Direção motor direito 1 |
| BIN2 | PB13 | Direção motor direito 2 |
| STBY | PB14 | Standby TB6612 |

### Sucção

| Sinal | Pino STM32 | Função |
|------|------------|--------|
| FAN_PWM | PA8 | PWM sucção / ESC |

### UART STM32 ↔ ESP32

| Sinal | Pino STM32 |
|------|------------|
| STM32_TX | PA9 |
| STM32_RX | PA10 |

Adicionar resistores série de 47Ω, se possível.

### SWD

| Função | Pino |
|--------|------|
| SWDIO | PA13 |
| SWCLK | PA14 |
| NRST | NRST |
| 3V3 | 3V3 |
| GND | GND |

## 2. J1 — Brain ↔ Chassis — Sensores/Lógica

Tipo sugerido: 2x6 2.54 mm board-to-board.

| Pino | Sinal |
|-----:|-------|
| 1 | 3V3 |
| 2 | GND |
| 3 | A0 |
| 4 | A1 |
| 5 | A2 |
| 6 | A3 |
| 7 | A4 |
| 8 | A5 |
| 9 | A6 |
| 10 | A7 |
| 11 | LAT_L |
| 12 | LAT_R |

## 3. J2 — Brain ↔ Chassis — Comandos/Potência

Tipo sugerido: 2x6 2.54 mm board-to-board.

| Pino | Sinal |
|-----:|-------|
| 1 | VBAT_ADC opcional |
| 2 | GND |
| 3 | 5V_IN |
| 4 | GND |
| 5 | PWMA |
| 6 | AIN1 |
| 7 | AIN2 |
| 8 | PWMB |
| 9 | BIN1 |
| 10 | BIN2 |
| 11 | STBY |
| 12 | FAN_PWM |

## 4. Pendências

- Confirmar modelo exato do XIAO.
- Confirmar pinagem real do módulo TB6612.
- Definir medição de bateria.
- Definir se laterais serão analógicos ou digitais.
