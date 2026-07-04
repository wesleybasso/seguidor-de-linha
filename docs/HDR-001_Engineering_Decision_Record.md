# HDR-001 — Engineering Decision Record

## HDR-001 — Arquitetura em três placas

Status: aprovado.

Decisão:
- HDB-001 Brain Board.
- HDB-100 Chassis / Power Board.
- HDB-101 Sensor Board.

Motivo:
- Separar lógica, potência e sensores.
- Facilitar manutenção.
- Permitir evolução modular.

## HDR-002 — Black Pill na Rev A0

Status: aprovado.

Decisão:
Usar Black Pill STM32F411CEU6 plugável.

Motivo:
- O usuário já possui.
- Baixo custo.
- Fácil troca.
- Validação rápida.

## HDR-003 — XIAO ESP32-S3 na Rev A0

Status: aprovado.

Decisão:
Usar XIAO ESP32-S3 ou XIAO ESP32-S3 Plus plugável.

Motivo:
- USB-C, antena, regulador e boot já resolvidos.
- Evita risco RF/USB na Rev A0.

Risco:
- Pinagem e dimensões podem mudar entre XIAO e XIAO Plus.

## HDR-004 — UART STM32 ↔ ESP32

Status: aprovado.

Decisão:
Comunicação principal via UART.

Motivo:
- Simples.
- Robusta.
- Fácil de debugar.
- Suficiente para telemetria e configuração.

## HDR-005 — Um TB6612 para dois motores

Status: aprovado.

Decisão:
Usar apenas um TB6612.

Motivo:
- O TB6612 já possui dois canais.
- Canal A = motor esquerdo.
- Canal B = motor direito.

## HDR-006 — Sucção central

Status: aprovado.

Decisão:
Um único sistema de sucção no centro.

Motivo:
- Simetria.
- Maior aderência.
- Melhor controle em curva.

Pendência:
- Definir se será motor DC, EDF ou brushless com ESC.

## HDR-007 — LiPo 2S

Status: aprovado.

Decisão:
Usar LiPo 2S.

Motivo:
- Boa potência.
- Compatível com motores 6V se houver limite de PWM.
- Compatível com TB6612 em tensão.

Risco:
- 8.4V cheia é maior que o nominal dos motores 6V.
- Firmware deve limitar PWM.
