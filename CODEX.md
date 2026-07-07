# CODEX.md - Operacao Codex no PEGASUS Rev A0

Este arquivo define como o Codex deve trabalhar neste repositorio.

## Missao

Transformar o PEGASUS Rev A0 em um projeto fabricavel e testavel, sem pular
etapas de seguranca. O foco do Codex e hardware, KiCad, fabricacao, validacao
cruzada e integracao com firmware.

## Areas De Dono

Codex pode editar diretamente:

- `kicad documents/**`
- `manufacturing/**`
- `BOM_PRELIMINARY.md`
- `KICAD_IMPLEMENTATION_PLAN.md`
- documentos de processo: `AGENTS.md`, `TASKS.md`, `TEST_PLAN.md`,
  `DECISIONS.md`, `DECISIONS_PENDING.md`, `REVIEW_NOTES.md`

Codex deve propor em `REVIEW_NOTES.md`, antes de editar diretamente:

- `firmware/stm32/**`
- `firmware/esp32/**`
- `firmware/communication/**`
- especificacoes oficiais em `docs/HPS-*`, `docs/HRS-*`, `docs/HFS-*`

## Regras Inviolaveis

- Nao gerar Gerbers finais sem revisao humana.
- Nao trocar pinagem sem atualizar HPS, firmware e HDR.
- Nao substituir Black Pill ou ESP32 plugavel por MCU SMD integrado nesta Rev A0.
- Nao tratar placeholder mecanico como footprint fabricavel.
- Nao acionar motores ou succao sem plano de teste com STOP validado.

## Prioridade Atual

Fase 0: protoboard com Black Pill STM32F411 + Waveshare ESP32-S3-Zero.

Ordem de execucao:

1. Validar UART `PING/PONG` e `ACK/NACK`.
2. Tornar comandos ESP32 transacionais, com timeout e erro visivel.
3. Integrar HAL real minima no STM32: USART1 PA9/PA10, SysTick e ring buffer.
4. Validar telemetria fake e dashboard.
5. So depois entrar em ADC real, motores suspensos e succao isolada.

## Comandos Uteis

```powershell
python -m unittest discover tests/protocol
pio run -d firmware/esp32
arm-none-eabi-gcc -std=c11 -Wall -Wextra -DSTM32F411xE -I firmware/stm32/Core/Inc -I firmware/stm32/Drivers/CMSIS/Include -I firmware/stm32/Drivers/CMSIS/Device/ST/STM32F4xx/Include -fsyntax-only firmware/stm32/Core/Src/*.c
git status --short --branch
```
