# DECISIONS_PENDING.md - Pendencias E Travamentos PEGASUS Rev A0

Atualizado: 2026-07-06.

## P0 - Travamentos Antes De PCB Fabricavel

- Escolher ESP32 definitivo para HDB-001: Seeed XIAO ESP32-S3, XIAO ESP32-S3
  Plus ou Waveshare ESP32-S3-Zero.
- Medir com paquimetro: Black Pill, ESP32 escolhido, TB6612, Mini360, sensor
  vermelho, bateria, motores, rodas, succao/duto e conectores.
- Substituir footprints placeholder por footprints reais com pin 1, silkscreen,
  courtyard e altura.
- Criar esquematicos eletricos reais; hoje ha labels globais, mas nao circuito
  completo.
- Validar pinout real do modulo TB6612 comprado.
- Definir conectores reais de motor, fan/ESC, sensor, XT30, chave e
  board-to-board.
- Definir estrategia de GND: potencia, analogico e ponto comum.

## P0 - Travamentos Antes De Bancada Com Atuadores

- Implementar transacao real de comando no ESP32: comando pendente, ACK/NACK,
  timeout, retry, RTT e ultimo erro.
- Confirmar em hardware que STOP fora da fila comum corta PWM/STBY/fan no tempo
  maximo definido.
- Integrar HAL real minima no STM32 para UART, tick e watchdog.
- Definir payload de `HARDWARE_TEST`: alvo, PWM, duracao, direcao e timeout.
- Resolver divergencia entre requisito de loop/sensores `>= 1 kHz` e
  `PEGASUS_CONTROL_HZ 500`.

## P1 - Decisoes De Sistema

- Definir medicao de bateria: divisor no STM32, divisor no ESP32, ADC externo
  ou remover `battery_mv` da promessa Rev A0.
- Definir se sensores laterais `LAT_L/LAT_R` serao analogicos ou digitais.
- Definir politica quando ESP32 some: STM32 deve parar sempre ou apenas em
  certos estados?
- Definir semantica de `ERROR_STATUS` no protocolo.
- Definir resultado esperado de calibracao na telemetria.

## P2 - Dashboard

- Manter mockup HTML como visual source e gerar versao embarcada enxuta em
  `web_ui.h`.
- Criar modo Bring-up com checklist guiado.
- Mostrar idade da telemetria, ultimo pacote, stale/timeout, ACK/NACK e RTT.
- Habilitar/desabilitar botoes conforme estado STM32.
- Expor `/api/summary` e `/api/analyze` na UI.
