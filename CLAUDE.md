# CLAUDE.md - Operacao Claude Code no PEGASUS Rev A0

Este arquivo e um resumo operacional para Claude Code. O prompt completo fica em
`prompts/CLAUDE_PEGASUS_Instructions.md`.

## Area Principal

Claude Code e dono de firmware e dashboard:

- `firmware/stm32/**`
- `firmware/esp32/**`
- `firmware/communication/**`
- `docs/esp32-control-dashboard.html`
- `tests/**`
- `tools/**`

## Contratos Compartilhados

Antes de mudar qualquer comportamento que afete hardware ou arquitetura,
registrar em `REVIEW_NOTES.md` ou `DECISIONS_PENDING.md`.

Contratos sensiveis:

- `docs/HPS-001_Pinout_Specification.md`
- `firmware/communication/PEGASUS_UART_PROTOCOL.md`
- `docs/STM32_STATE_MACHINE_REV_A0.md`
- `docs/HDR-001_Engineering_Decision_Record.md`

## Proximos Alvos De Firmware

1. Implementar gerenciador de comando ESP32 com ACK/NACK, timeout, retry e
   ultimo erro visivel no dashboard.
2. Dar prioridade absoluta ao STOP no ESP32, sem depender da fila comum.
3. Trocar validacao STM32 de `len >= esperado` para tamanho exato quando o
   pacote exigir tamanho fixo.
4. Clampar faixas de PID, motor e fan antes de aplicar configuracao.
5. Definir payload seguro de `HARDWARE_TEST` antes de acionar saidas reais.
6. Adicionar mutex ou snapshot atomico para telemetry/config no ESP32.

