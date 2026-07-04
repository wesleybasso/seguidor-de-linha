# PEGASUS Rev A0 - Pendencias e Riscos KiCad

Status: base KiCad mecanica criada. Nao gerar Gerbers finais sem revisao humana.

## Implementado

- Tres projetos KiCad separados:
  - HDB-001_Brain_Board
  - HDB-100_Chassis_Power_Board
  - HDB-101_Sensor_Board
- Biblioteca local `hardware/lib/PEGASUS_Placeholders.pretty`.
- Footprints placeholder:
  - Black Pill 53 x 21 mm.
  - XIAO ESP32-S3 21 x 18 mm.
  - TB6612 module 20 x 20 mm.
  - Mini360 17 x 11 mm.
  - Bateria LiPo 70 x 38 x 18 mm.
  - Ventosa/duto Ø22 mm.
  - J1/J2 2x6 2.54 mm.
- Outlines iniciais:
  - HDB-001: 75 x 40 mm.
  - HDB-100: 182 x 78 mm maximo, corpo traseiro largo e frente estreita.
  - HDB-101: 90 x 18 mm.
- Net labels J1/J2 conforme HPS-001.

## Pendencias antes de roteamento

- Confirmar pinagem fisica real da Black Pill STM32F411CEU6 usada.
- Confirmar se o modulo XIAO sera ESP32-S3 ou ESP32-S3 Plus; dimensoes/pinos podem mudar.
- Confirmar pinagem real do modulo TB6612FNG comprado.
- Definir medicao de bateria: divisor no ESP32, ADC externo ou revisao futura.
- Definir conectores reais de motor, fan/ESC, sensor e XT30.
- Definir se sensores laterais LAT_L/LAT_R serao analogicos ou digitais.
- Revisar acesso mecanico a USB-C, botoes boot/reset e SWD.
- Validar altura de bateria, spacers e folga entre HDB-001 e HDB-100.
- Definir motor/EDF/ESC de succao e corrente esperada.

## Riscos Rev A0

- TB6612 pode nao suportar corrente de stall dos motores escolhidos.
- LiPo 2S cheia tem 8.4 V; firmware deve limitar PWM para motores 6 V.
- Buck Mini360 precisa margem termica e corrente suficiente para logica/sensores.
- Ruido de motor/buck/fan pode contaminar A0-A7 se o roteamento misturar analogico e potencia.
- Duto Ø22 mm pode conflitar com bateria, fan, fixacao ou centro de massa.
- Board-to-board 2x6 precisa altura e alinhamento mecanico validados com spacers reais.
- HDB-100 ainda esta mecanica/posicional, nao e layout eletrico roteado.
