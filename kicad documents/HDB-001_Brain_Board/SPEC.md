# HDB-001 — Brain Board

## Função

Placa superior do PEGASUS. Responsável por lógica, controle, comunicação e debug.

## Dimensões

- Alvo: 75 x 40 mm.
- 2 camadas.
- 4 furos M2/M2.5.

## Componentes

| Ref | Componente | Função |
|-----|------------|--------|
| U1 | Black Pill STM32F411 | Controle em tempo real |
| U2 | XIAO ESP32-S3 | Telemetria/comunicação |
| U3 | Mini360 opcional | 5V standalone |
| J1 | 2x6 board-to-board | sensores/lógica |
| J2 | 2x6 board-to-board | comandos/potência |
| J3 | SWD | debug STM32 |
| J4 | UART debug | debug serial |
| D1–D5 | LEDs | status |
| SW1/SW2 | botões | reset/boot |

## Observação

Se o 5V vier do HDB-100, U3 pode ser DNP.
