# PEGASUS Rev A0

PEGASUS e um robo seguidor de linha modular em tres placas.

- `HDB-001` Brain Board: Black Pill STM32F411, XIAO ESP32-S3/Plus, debug e conectores para o chassi.
- `HDB-100` Chassis / Power Board: LiPo 2S, chave, TB6612FNG, buck 5V, motores e succao central.
- `HDB-101` Sensor Board: placa frontal/adaptadora para sensor vermelho de 8 canais na Rev A0.

## Estrutura

- `kicad documents/`: projetos KiCad e biblioteca local de footprints placeholder.
- `docs/`: requisitos, pinagem e decisoes de engenharia.
- `firmware/`: requisitos iniciais de firmware STM32/ESP32 e protocolo UART.
- `manufacturing/`: checklist preliminar JLCPCB/PCBWay.
- `sketches/`: desenhos de arquitetura e mecanica.

## Estado Atual

Base mecanica KiCad Rev A0 criada. Ainda nao ha roteamento final, ERC/DRC final, nem Gerbers de fabricacao.

Nao enviar para fabricacao sem revisar:

- footprints reais com paquimetro;
- pinagem real da Black Pill, XIAO e modulo TB6612;
- corrente dos motores e succao;
- espacamento de bateria, motores, ventosa e empilhamento;
- DRC/ERC completo apos esquematicos e roteamento.
