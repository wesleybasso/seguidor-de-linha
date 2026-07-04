# KiCad Implementation Plan — PEGASUS Rev A0

## Projetos

Criar:

- HDB-001_Brain_Board
- HDB-100_Chassis_Power_Board
- HDB-101_Sensor_Board

## Footprints mecânicos placeholder

| Módulo | Dimensão aproximada |
|--------|---------------------|
| Black Pill | 53 x 21 mm |
| XIAO ESP32-S3 | 21 x 18 mm |
| TB6612 módulo | 20 x 20 mm |
| Mini360 | 17 x 11 mm |
| Bateria LiPo | 70 x 38 x 18 mm |
| Ventosa | furo Ø22 mm |

## HDB-001

Outline:
- 75 x 40 mm.
- J1/J2 na borda inferior.
- XIAO à esquerda.
- Black Pill à direita/centro.
- LEDs no topo.
- SWD e UART acessíveis.
- 4 furos.

## HDB-100

Outline:
- Até 182 x 78 mm.
- Furo de sucção central Ø22 mm.
- Bateria baixa e central.
- Motores nas laterais.
- TB6612 perto dos motores.
- XT30 acessível.
- Buck 5V.
- J1/J2 alinhados com HDB-001.

## HDB-101

Outline:
- ~90 x 16–20 mm.
- 8 sensores.
- Conexão para A0–A7 + alimentação + GND.

## Ordem de implementação

1. Criar símbolos/footprints placeholder.
2. Criar esquemático HDB-001.
3. Criar esquemático HDB-100.
4. Criar esquemático HDB-101.
5. Criar outlines mecânicos.
6. Posicionar componentes.
7. Revisar com Wesley.
8. Só depois rotear.
