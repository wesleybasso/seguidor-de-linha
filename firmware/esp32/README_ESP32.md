# PEGASUS ESP32-S3 Rev A0.1

Firmware do copiloto/telemetria do PEGASUS.

## Alvos

- `BOARD_ESP32_S3_ZERO`: fase de protoboard.
- `BOARD_XIAO_ESP32_S3`: HDB-001 futura.

Para o ESP32-S3 Zero, a UART de bancada usa os pads serigrafados `TX/RX`:

- `TX` = `GPIO43`
- `RX` = `GPIO44`

No XIAO ESP32-S3, os pinos continuam como `-1` ate confirmar se o modulo sera XIAO ESP32-S3 ou XIAO ESP32-S3 Plus.

## O Que Implementa

- Access Point `PEGASUS_SETUP` / `pegasus123`.
- Dashboard simples em `192.168.4.1`.
- Filas FreeRTOS para comandos e telemetria.
- Core 0: UART, parser, TX, watchdog STM32 e Wi-Fi.
- Core 1: web server, logger, analise e perfis.
- Protocolo `SOF/TYPE/LEN/PAYLOAD/CRC8`.
- NVS para configuracao.

## Proximo Passo De Bancada

1. Ligar ESP32 `GPIO43/TX` ao STM32 `PA10/RX`.
2. Ligar ESP32 `GPIO44/RX` ao STM32 `PA9/TX`.
3. Ligar GND comum.
4. Rodar `PING/PONG` com o STM32.
5. Exibir telemetria fake no dashboard.
