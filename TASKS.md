# TASKS.md - PEGASUS Rev A0

Atualizado: 2026-07-06.

## Fase 0 - Protoboard Sem Motores

- [x] Criar estrutura do repositorio.
- [x] Criar protocolo UART comum HCP-001.
- [x] Implementar CRC8 em STM32, ESP32 e referencia Python.
- [x] Implementar parser de pacotes em STM32 e ESP32.
- [x] Implementar telemetria fake no STM32 para bancada.
- [x] Implementar dashboard minimo embarcado no ESP32.
- [x] Criar dashboard mockup separado em `docs/esp32-control-dashboard.html`.
- [x] Criar emulador de STM32 em `tools/pegasus_fake_stm32.py`.
- [x] Criar testes Python de protocolo em `tests/protocol`.
- [x] Tratar `HARDWARE_TEST` no STM32 com ACK somente em IDLE/READY.
- [x] Fazer STOP entrar na frente da fila de comandos do ESP32.
- [x] Rejeitar payload extra em comandos simples STM32.
- [x] Proteger encoder UART contra payload nulo invalido.
- [x] Fazer ARM/START/DISARM/CAL responder ACK/NACK conforme estado valido.
- [x] Portar cockpit compacto para `firmware/esp32/web/web_ui.h`.
- [x] Rastrear no ESP32 ultimo ACK/NACK/PONG, comando pendente e RTT simples.
- [ ] Implementar command manager ESP32 com ACK/NACK, timeout, retry e RTT.
- [ ] Implementar retry/timeout ativo para comando pendente no ESP32.
- [x] Mostrar no dashboard: contadores ACK/NACK/PONG, RTT e idade da telemetria.
- [ ] Adicionar mutex ou snapshot atomico para telemetry/config/log no ESP32.
- [x] Trocar validacoes STM32 para tamanho exato quando aplicavel.
- [x] Clampar PID, motor, fan e `fan_mode` antes de aplicar configuracao.
- [ ] Definir payload seguro de `HARDWARE_TEST`.
- [ ] Criar HAL minima STM32Cube: USART1 PA9/PA10, SysTick, ring buffer.
- [ ] Testar `PING/PONG` por 1000 ciclos no hardware real.
- [ ] Testar START/STOP/ARM/CALIBRATION no hardware real.
- [ ] Exibir sensores fake no dashboard via hardware real por 10 minutos.

## Fase 1 - Sensores Reais

- [ ] Ler sensores analogicos A0-A7.
- [ ] Decidir e validar LAT_L/LAT_R analogico ou digital.
- [ ] Calibrar fundo e linha.
- [ ] Normalizar sensores 0-1000.
- [ ] Validar linha perdida.
- [ ] Validar cruzamento.
- [ ] Medir ruido com buck/motores desligados e ligados.
- [ ] Resolver divergencia `>= 1 kHz` versus `PEGASUS_CONTROL_HZ 500`.

## Fase 2 - Atuadores Em Bancada

- [ ] Usar fonte limitada antes da LiPo.
- [ ] Validar 5V, 3V3, VMOTOR e GND comum.
- [ ] Validar TB6612 em standby no boot.
- [ ] Testar motores com rodas suspensas e PWM baixo.
- [ ] Medir corrente de stall com teste curto e controlado.
- [ ] Validar STOP imediato durante motor suspenso.
- [ ] Testar succao/fan/ESC isolado, sem tracao.
- [ ] Medir corrente, vibracao e temperatura da succao.

## Fase 3 - PCB Rev A0 Fabricavel

- [ ] Escolher ESP32 definitivo: XIAO ESP32-S3, XIAO ESP32-S3 Plus ou ESP32-S3-Zero.
- [ ] Medir todos os modulos com paquimetro.
- [ ] Substituir footprints placeholder por footprints reais.
- [x] Adicionar header roteado JSENS na HDB-101 para adaptador Rev A0 em ordem HPS.
- [x] Marcar J3 da HDB-001 como header de bancada SWD/UART para ESP32-S3-Zero.
- [ ] Criar esquematico eletrico real da HDB-001.
- [ ] Ligar UART STM32 PA9/PA10 ao ESP32 definitivo.
- [ ] Definir conectores reais de J1/J2, motores, fan/ESC, sensor, XT30 e chave.
- [ ] Criar esquematico eletrico real da HDB-100.
- [ ] Conferir HPS-001 contra KiCad.
- [ ] Rodar ERC/DRC.
- [ ] Fazer revisao mecanica impressa 1:1.
- [ ] Abrir Gerbers/drill em viewer externo.
- [ ] Solicitar revisao humana antes de qualquer Gerber final.

## Fase 4 - Primeira Pista

- [ ] Perfil SAFE.
- [ ] Base speed baixo.
- [ ] Succao desligada ou baixa.
- [ ] Linha reta curta.
- [ ] Curva simples.
- [ ] Logs completos no dashboard.
- [ ] Revisao de temperatura e fixacao apos cada run.
