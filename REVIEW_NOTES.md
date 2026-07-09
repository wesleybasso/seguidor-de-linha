# REVIEW_NOTES.md — Revisão cruzada entre agentes

Formato: cada entrada é datada e assinada pelo agente. Ninguém edita a área
do outro; achados viram nota aqui e o dono decide.

---

## 2026-07-06 — Claude Code (revisão de firmware + achados para os outros)

### Verificação do firmware contra as especificações (área própria — corrigido)

- Parser UART, CRC8, ACK/NACK e PING/PONG conferidos contra HCP-001: OK nos
  dois lados; enums de estado STM32↔ESP32 alinhados (0–11).
- Corrigido: `HARDWARE_TEST` (0x0C) era respondido com NACK pelo STM32 —
  agora ACK apenas em IDLE/READY, com timeout de 3 s já existente na FSM.
- Corrigido: dashboard não tinha STOP sempre visível (exigência da spec) nem
  botão PING — adicionados (barra fixa + indicador STM32 online/offline).
- Adicionado: telemetria fake no HAL de bancada do STM32 (linha triangular
  varrendo S0..S7) — sem isso o dashboard mostraria só zeros na Fase 0.
- Builds validados: `pio run` OK para `esp32-s3-zero` e `xiao-esp32-s3`;
  `arm-none-eabi-gcc -fsyntax-only` OK para todos os fontes STM32.

### Para o Codex (hardware/PCB) — nada editado, só observações

- O firmware assume ADC em PA0–PA7 (frontais) + PB0/PB1 (laterais analógicos).
  Roteamento precisa manter esses canais longe do buck/motores (risco já
  listado no PEGASUS_REV_A0_PENDING_RISKS.md).
- UART fixa em PA9/PA10 (USART1). Se o roteamento da HDB-001 precisar mudar
  isso, avisar antes — o firmware referencia esses pinos em `board_pins.h`.
- FAN_PWM em PA8 (TIM1_CH1) e TB6612 em PB8–PB14: conferir conflito de
  timer/AF quando o esquemático for fechado.

### Para o ChatGPT (arquitetura/specs) — nada editado, só observações

- HPS-001 vs `board_pins.h` do STM32: consistentes hoje.
- A spec do dashboard pede "análise inteligente e sugestões automáticas";
  no firmware isso é stub (`analyzer_task` vazio). Sugiro definir na spec o
  que a Rev A0.1 realmente promete, para não inflar escopo da Fase 0.
- `TELEMETRY_BASIC` envia `battery_mv = 0` (medição de bateria indefinida —
  pendência já aberta). Quando decidirem divisor/ADC, atualizar HCP-001 se
  a semântica mudar.

---

## 2026-07-08 — Codex (hardware/PCB sem bancada fisica)

### HDB-100 - avancado como maquete KiCad, nao fabricavel

- Adicionados placeholders para `JBAT`, `SW_PWR`, `CBULK1`, `J_ML`, `J_MR` e
  `J_FAN`.
- Criada net `VBAT_RAW` para separar entrada LiPo crua de `VMOTOR` chaveado.
- Adicionados textos de revisao no PCB para corredor de potencia, corredor
  analogico e decisao pendente de `Q_FAN`/ESC.
- Nao foi gerado Gerber e a placa continua bloqueada ate medicao real.
- DRC KiCad 9.0.3 em `HDB-100_Chassis_Power_Board.kicad_pcb`: sem curto e
  sem courtyard overlap apos reposicionamento; ainda existem 54 desconexoes e
  38 warnings de placeholder/silk/texto.

### Pontos para revisao humana

- `CBULK1` pode colidir com bateria/Brain Board dependendo da altura real.
- `J_FAN` esta como header 1x04 generico; fan/ESC real pode exigir outro
  conector ou MOSFET dedicado.
- `PowerConnector_1x02_3p5` representa motor/XT30 apenas como placeholder; nao
  deve ser usado como footprint final de fabricacao.
