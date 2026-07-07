# AGENTS.md - Equipes PEGASUS Rev A0

Projeto: PEGASUS Line Follower (Hander)
Responsavel humano: Wesley Mateus Basso

Regra central: cada agente tem uma area de dono. Mudancas fora da area devem
vir como nota em `REVIEW_NOTES.md` ou pendencia em `DECISIONS_PENDING.md`,
salvo pedido explicito do humano.

## Agentes Principais

### Claude Code - Firmware E Dashboard

Area de dono:

- `firmware/stm32/**`
- `firmware/esp32/**`
- `firmware/communication/**`
- `docs/esp32-control-dashboard.html`
- `tests/**`
- `tools/**`

Responsabilidades:

- Maquina de estados STM32.
- Sensores, PID, motores, succao e safety.
- UART HCP-001.
- Dashboard embarcado e mockup HTML.
- Testes de protocolo, unidade e bancada.

### Codex - Hardware, KiCad, Fabricacao E Integracao

Area de dono:

- `kicad documents/**`
- `manufacturing/**`
- `BOM_PRELIMINARY.md`
- `KICAD_IMPLEMENTATION_PLAN.md`
- `TEST_PLAN.md`
- `DECISIONS.md`
- `DECISIONS_PENDING.md`
- `AGENTS.md`
- `REVIEW_NOTES.md`

Responsabilidades:

- HDB-001, HDB-100 e HDB-101.
- Footprints reais e revisao mecanica.
- ERC/DRC, checklist JLCPCB/PCBWay e BOM.
- Revisao cruzada entre firmware, pinagem e PCB.

### ChatGPT / Arquiteto De Sistema - Specs E Produto

Area de dono:

- `docs/RRS-*`
- `docs/HRS-*`
- `docs/HPS-*`
- `docs/HDS-*`
- `docs/HMS-*`
- `docs/HFS-*`
- `docs/HDR-*`
- `prompts/**`

Responsabilidades:

- Requisitos oficiais.
- Pinagem oficial.
- Decisoes de engenharia.
- Estrategia de corrida e escopo Rev A0/A0.1.

## Subequipes Operacionais

### Squad Firmware Tempo Real

Objetivo: tornar o STM32 confiavel em bancada e depois em pista.

Primeiros alvos:

- HAL real minima.
- STOP prioritario.
- Validacao de payload e limites.
- `HARDWARE_TEST` seguro.
- Testes unitarios de controle.

### Squad ESP32 Operador

Objetivo: transformar o ESP32 em painel de bancada e corrida.

Primeiros alvos:

- ACK/NACK transacional.
- RTT, timeout e ultimo erro.
- Dashboard com gating por estado.
- Perfis e resumo pos-corrida.

### Squad PCB Fabricavel

Objetivo: sair de maquete contratual para placa Rev A0 fabricavel.

Primeiros alvos:

- Escolher ESP32 definitivo.
- Criar esquematico real da HDB-001.
- Substituir placeholders por footprints reais.
- Conferir HPS contra KiCad.

### Squad Validacao E Safety

Objetivo: impedir salto perigoso de protoboard para motores/succao.

Primeiros alvos:

- Rodar matriz `TEST_PLAN.md`.
- Definir go/no-go por fase.
- Bloquear Gerbers prematuros.
- Registrar falhas de bancada.

### Squad Arquitetura E Documentacao

Objetivo: manter firmware, hardware e produto falando a mesma lingua.

Primeiros alvos:

- Atualizar decisoes.
- Fechar pendencias P0.
- Manter HPS/HDR sincronizados.
- Reduzir escopo inflado da Rev A0 quando necessario.

## Interfaces Compartilhadas

Mudanca exige registro e revisao cruzada:

- `firmware/communication/PEGASUS_UART_PROTOCOL.md`
- `docs/HPS-001_Pinout_Specification.md`
- `docs/STM32_STATE_MACHINE_REV_A0.md`
- `docs/HDR-001_Engineering_Decision_Record.md`

## Estado Atual

Atualizado: 2026-07-06.

- Firmware base existe e tem testes Python de protocolo.
- Dashboard mockup separado existe em `docs/esp32-control-dashboard.html`.
- KiCad existe como outline/maquete, ainda nao como PCB roteavel.
- Fase atual: Fase 0, protoboard sem motores.
- Gerbers finais seguem bloqueados.

