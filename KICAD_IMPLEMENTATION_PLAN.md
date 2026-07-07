# KiCad Implementation Plan - PEGASUS Rev A0

Atualizado: 2026-07-06.

## Estado Atual Honesto

- HDB-001 existe como outline, posicionamento mecanico, J1/J2 conforme HPS,
  debug J3 e algumas pre-rotas de bancada.
- HDB-101 existe como adaptador Rev A0 com `JSENS` e `J1` em ordem HPS, com
  pre-rotas diretas entre os headers.
- HDB-100 existe como maquete mecanica/potencia com placeholders.
- Nenhuma placa esta liberada para Gerber final.

## Atualizacao 2026-07-06

- HDB-101 recebeu `JSENS` em ordem HPS e pre-rotas diretas para `J1`.
- HDB-001 recebeu labels de bancada para SWD/UART e pre-rotas curtas de J3
  para sinais de debug/USART do Black Pill.
- Foi criado footprint placeholder `PinHeader_1x04_2p54` para headers curtos
  de bancada.

## Prioridade Atual

1. Fechar ESP32 oficial da HDB-001: XIAO ESP32-S3/Plus ou Waveshare Zero.
2. Transformar HDB-001 de schematic placeholder em schematic eletrico real.
3. Validar pad a pad do Black Pill contra HPS.
4. Roteamento completo da HDB-001 somente apos pinagem real.
5. HDB-100 continua bloqueada por XT30, chave, TB6612 real, buck, conectores,
   capacitores, fan/ESC e estrategia de GND.

## Projetos

- `HDB-001_Brain_Board`
- `HDB-100_Chassis_Power_Board`
- `HDB-101_Sensor_Board`

## Footprints Mecanicos Placeholder

| Modulo | Dimensao aproximada | Status |
|---|---:|---|
| Black Pill | 53 x 21 mm | Placeholder, precisa validar pad a pad |
| XIAO ESP32-S3 | 21 x 18 mm | Placeholder, variante ainda pendente |
| TB6612 modulo | 20 x 20 mm | Placeholder, pinout real pendente |
| Mini360 | 17 x 11 mm | Placeholder |
| Bateria LiPo | 70 x 38 x 18 mm | Placeholder mecanico |
| Ventosa | furo D22 mm | Inicial |

## HDB-001 Brain Board

Outline:

- 75 x 40 mm.
- J1/J2 na borda inferior.
- ESP32 a esquerda.
- Black Pill a direita/centro.
- SWD/UART acessiveis.
- 4 furos.

Proximos passos:

- Criar esquematico real.
- Adicionar resistores serie UART 47R.
- Adicionar LEDs e resistores.
- Adicionar reset/boot conforme modulo escolhido.
- Adicionar test points.
- So rotear depois de validar pinagem Black Pill e ESP32.

## HDB-100 Chassis / Power Board

Outline:

- Ate 182 x 78 mm.
- Furo de succao central D22 mm.
- Bateria baixa e central/traseira.
- Motores nas laterais.
- TB6612 perto dos motores.
- XT30 acessivel.
- Buck 5V.
- J1/J2 alinhados com HDB-001.

Bloqueios:

- XT30 real.
- Chave real.
- Capacitores bulk.
- Conectores de motor.
- Fan/ESC real.
- Pinout TB6612 real.
- Estrategia de GND e potencia.

## HDB-101 Sensor Board

Outline:

- 90 x 18 mm.
- `JSENS` e `J1` seguem a mesma ordem HPS:
  `3V3, GND, A0-A7, LAT_L, LAT_R`.
- Pre-rotas diretas ja existem.

Proximos passos:

- Validar pinagem fisica do sensor vermelho.
- Adicionar slots/furos de ajuste de altura.
- Ajustar posicao do cabo para nao raspar na pista.

## Ordem De Implementacao

1. Validar modulo ESP32 oficial.
2. Validar Black Pill pad a pad.
3. Criar schematic real da HDB-001.
4. Roteamento HDB-001.
5. Validar sensor vermelho e finalizar HDB-101.
6. Medir componentes de potencia.
7. Criar schematic real da HDB-100.
8. Roteamento HDB-100 com net classes.
9. ERC/DRC.
10. Revisao humana.
11. Somente depois gerar Gerbers.
