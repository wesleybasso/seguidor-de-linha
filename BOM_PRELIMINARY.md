# BOM Preliminar — PEGASUS Rev A0

## HDB-001

| Item | Qtd | Observação |
|------|----:|------------|
| Black Pill STM32F411 | 1 | já possui |
| XIAO ESP32-S3/Plus | 1 | confirmar modelo |
| Mini360 opcional | 1 | DNP se 5V vier do chassi |
| Soquetes 2.54mm | vários | Black Pill/XIAO |
| J1 2x6 board-to-board | 1 | sensores |
| J2 2x6 board-to-board | 1 | comandos |
| SWD 5 pinos | 1 | debug |
| UART 4 pinos | 1 | debug |
| LEDs | 4–5 | status |
| Resistores 1k | 4–5 | LEDs |
| Resistores 47R | 2 | UART |
| Capacitores 100nF | 4–8 | desacoplamento |
| Test points | 8–12 | debug |

## HDB-100

| Item | Qtd | Observação |
|------|----:|------------|
| XT30 | 1 | bateria |
| Chave | 1 | confirmar corrente |
| TB6612FNG módulo | 1 | plugável |
| Mini360/buck | 1 | 5V |
| Capacitor 470–1000uF | 1–3 | VBAT |
| Capacitores 100nF | 6–12 | filtragem |
| Conector motor L | 1 | 2 pinos |
| Conector motor R | 1 | 2 pinos |
| Conector fan/ESC | 1 | 2/3 pinos |
| MOSFET fan | 1 | opcional |
| Diodo flyback | 1 | motor DC |
| J1/J2 board-to-board | 2 | brain |
| Conector sensor | 1 | HDB-101 |
| Test points | 8–12 | debug |

Notas HDB-100:

- `JBAT`, `J_ML` e `J_MR` usam footprint placeholder
  `PowerConnector_1x02_3p5`; substituir pelo conector real antes de Gerber.
- `SW_PWR` usa `SlideSwitch_SPST_8x4`; confirmar corrente e fixacao mecanica.
- `CBULK1` usa `BulkCap_Radial_D10_P5`; confirmar diametro, altura e colisao
  com bateria/Brain Board.
- `J_FAN` esta como header 1x04: `VMOTOR`, `GND`, `FAN_PWM`, `5V_IN`; ajustar
  quando fan/ESC real for escolhido.

## HDB-101

| Item | Qtd | Observação |
|------|----:|------------|
| Sensor vermelho 8 canais | 1 | Rev A0 |
| Sensor lateral | 2 | se usados |
| Conector | 1 | flat/JST/header |
