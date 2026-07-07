# F0 Bench Runbook - PEGASUS Rev A0

Atualizado: 2026-07-06.

Objetivo: validar comunicacao, dashboard e comandos sem motores, sem succao e
sem LiPo ligada ao sistema de potencia.

## Hardware

- Black Pill STM32F411CEU6.
- ESP32-S3-Zero Waveshare para Fase 0.
- Fonte USB ou fonte de bancada limitada.
- GND comum entre STM32 e ESP32.
- Sem TB6612, sem motores, sem fan/ESC.

## Ligacao UART

| Sinal | STM32 Black Pill | ESP32-S3-Zero |
|---|---|---|
| STM32_TX | PA9 | GPIO44 / RX |
| STM32_RX | PA10 | GPIO43 / TX |
| GND | GND | GND |
| Alimentacao | USB/3V3 conforme bancada | USB |

Observacao: se possivel, usar resistores serie de 47R nas linhas UART.

## Build ESP32

```powershell
cd firmware/esp32
pio run -e esp32-s3-zero
```

Upload:

```powershell
pio run -e esp32-s3-zero -t upload
```

Dashboard:

- Wi-Fi AP: `PEGASUS_SETUP`
- Senha: `pegasus123`
- URL: `http://192.168.4.1`

## Build STM32

```powershell
cd firmware/stm32
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Saidas:

- `build/firmware.elf`
- `build/firmware.bin`

## Teste Sem STM32 Real

Usar emulador com adaptador USB-serial conectado ao ESP32:

```powershell
python tools/pegasus_fake_stm32.py COM5 --baud 115200
```

Trocar `COM5` pela porta correta.

## Sequencia De Aceite F0

1. Ligar ESP32 por USB.
2. Conectar ao AP `PEGASUS_SETUP`.
3. Abrir dashboard.
4. Confirmar que STOP aparece no topo.
5. Conectar STM32 ou fake STM32.
6. Pressionar `PING`.
7. Confirmar contador `PONG` subindo no dashboard.
8. Pressionar `ARM`; esperar `ACK`.
9. Pressionar `START` apenas quando estado permitir.
10. Pressionar `STOP`; confirmar estado seguro e PWM zero.
11. Rodar por 10 minutos com telemetria fake.

## Criterios De GO

- Sem reset do ESP32.
- Sem travamento do dashboard.
- `PING/PONG` responde.
- `ACK/NACK` aparece nos contadores.
- Comando invalido gera `NACK`.
- STOP continua disponivel e prioritario.
- Nenhum atuador real conectado.

## Criterios De NO-GO

- STM32 offline intermitente.
- Erro de CRC recorrente.
- Dashboard congela.
- STOP demora ou nao retorna.
- START aceito fora de estado valido.
- Qualquer aquecimento, queda de tensao ou reset inesperado.

## Evidencia A Registrar

Criar uma entrada em `REVIEW_NOTES.md` com:

- Data.
- Hardware usado.
- Firmware ESP32 commit/hash.
- Firmware STM32 commit/hash.
- Tempo de teste.
- Contagem de ACK/NACK/PONG.
- Resultado final: GO ou NO-GO.

