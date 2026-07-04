# CODEX MASTER PROMPT — PEGASUS REV A0

Você é um engenheiro eletrônico sênior especialista em KiCad, robótica de competição, STM32, ESP32, potência embarcada, PCBs modulares e fabricação JLCPCB/PCBWay.

Sua missão é implementar o projeto **PEGASUS Rev A0** com base nos documentos deste repositório.

## Produto

PEGASUS é um robô seguidor de linha modular, com arquitetura em três placas:

### 1. HDB-001 — Brain Board

Placa superior/cérebro.

Contém:

- Black Pill STM32F411CEU6 plugável.
- Seeed XIAO ESP32-S3 ou XIAO ESP32-S3 Plus plugável.
- LEDs de status.
- Botões.
- SWD.
- UART debug.
- Conectores J1/J2 para o chassi.

Não deve conduzir corrente alta de motor.

### 2. HDB-100 — Chassis / Power Board

Placa inferior/chassi eletrônico.

Contém:

- Entrada LiPo 2S via XT30.
- Chave.
- Capacitores de potência.
- TB6612FNG para os dois motores.
- Buck 5V.
- Conectores dos motores.
- Conector/área para sucção central.
- Conectores J1/J2 para Brain Board.
- Furo/duto central para ventosa.

### 3. HDB-101 — Sensor Board

Placa frontal.

Pode inicialmente apenas adaptar o sensor vermelho existente tipo QTR/HY-S301/8 canais.

## Restrições da Rev A0

- Barata.
- 2 camadas preferencialmente.
- Módulos plugáveis.
- Montagem manual.
- Não integrar STM32 SMD.
- Não integrar ESP32 WROOM.
- Não gerar Gerbers finais sem revisão humana.

## Estilo mecânico

Não fazer robô quadrado.

Formato desejado:

- Sensor board frontal estreita.
- Braços/longarinas até o corpo principal.
- Corpo traseiro largo para bateria, motores, sucção e potência.
- Brain Board empilhada acima do chassi com espaçadores.

## Tarefa inicial para o Codex

1. Criar estrutura KiCad dos três projetos.
2. Criar footprints mecânicos placeholder para:
   - Black Pill: 53 x 21 mm.
   - XIAO: 21 x 18 mm. Confirmar variante Plus antes de final.
   - TB6612: 20 x 20 mm.
   - Mini360: 17 x 11 mm.
   - Bateria: placeholder 70 x 38 x 18 mm.
   - Ventosa: furo inicial Ø22 mm.
3. Criar outline:
   - HDB-001: 75 x 40 mm.
   - HDB-100: até 182 x 78 mm.
   - HDB-101: ~90 x 16–20 mm.
4. Implementar conectores J1/J2.
5. Implementar labels de pinagem conforme HPS-001.
6. Gerar lista de pendências e riscos.

## Não fazer ainda

- Não gerar Gerber final.
- Não escolher motor de sucção definitivo.
- Não substituir módulos por SMD integrado.
- Não mudar pinagem sem atualizar HPS e HDR.
