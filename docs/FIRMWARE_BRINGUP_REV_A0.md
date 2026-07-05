# PEGASUS Firmware Bring-up Rev A0.1

## Objetivo

Validar primeiro em bancada:

```text
ESP32-S3 Zero <-> UART <-> STM32 Black Pill
```

Depois migrar o mesmo firmware ESP32 para o XIAO ESP32-S3 na HDB-001.

## Ligacao UART Inicial

STM32 Black Pill:

- `PA9` = `STM32_TX`
- `PA10` = `STM32_RX`
- `GND` comum obrigatorio

ESP32-S3 Zero:

- usar o pad marcado `RX` / `GPIO44` para receber `STM32_TX / PA9`
- usar o pad marcado `TX` / `GPIO43` para transmitir para `STM32_RX / PA10`

Regra:

```text
STM32 PA9 TX  -> ESP32 RX / GPIO44
STM32 PA10 RX <- ESP32 TX / GPIO43
GND           <-> GND
```

## Pinagem ESP32-S3 Zero

A documentacao da Waveshare informa que os pads serigrafados `TX` e `RX` sao:

```cpp
#define STM32_UART_TX_PIN 43
#define STM32_UART_RX_PIN 44
```

O XIAO ESP32-S3 continua pendente porque ainda falta confirmar a variante exata.

## Sequencia De Testes

1. Build ESP32 para `esp32-s3-zero`.
2. Gravar ESP32 ainda sem UART, abrir AP `PEGASUS_SETUP`.
3. Confirmar dashboard em `192.168.4.1`.
4. Testar `PING/PONG`.
5. Testar telemetria fake do STM32.
6. Testar sensores reais.
7. Testar motores com rodas suspensas.
8. Testar succao isolada medindo corrente.

## Referencias

- Waveshare ESP32-S3-Zero wiki: `TX = GPIO43`, `RX = GPIO44`, WS2812 em `GPIO21`, BOOT em `GPIO0`.
- DeepBlueEmbedded STM32F411 Black Pill: confirma Black Pill STM32F411CEU6, 20x2 headers, ADC, timers e USARTs; pinout por diagrama WeAct.

## Comandos Uteis

Build ESP32:

```powershell
cd firmware\esp32
pio run
```

Checar sintaxe STM32:

```powershell
arm-none-eabi-gcc -std=c11 -Wall -Wextra -I firmware\stm32\Core\Inc -fsyntax-only firmware\stm32\Core\Src\*.c
```
