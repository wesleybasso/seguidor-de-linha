# Offline KiCad Sprint - Ate 2026-07-13

Contexto: sem recursos fisicos disponiveis ate segunda-feira, 2026-07-13.
Trabalho permitido neste periodo: ideias, arquitetura, KiCad, revisao de docs e
preparacao de bancada. Trabalho bloqueado: medicao real, teste eletrico,
validacao mecanica 1:1 e qualquer Gerber final.

## Objetivo

Transformar a Rev A0 de maquete contratual em uma base de PCB mais discutivel,
sem fingir que ja existe placa fabricavel.

## Foco Principal

1. HDB-100: organizar arquitetura de potencia.
2. HDB-001: preparar pinagem e mecanica para o ESP32 definitivo.
3. HDB-101: manter adaptador simples para o sensor vermelho existente.
4. Documentos: manter HPS, HDR, TEST_PLAN e DECISIONS_PENDING sincronizados.

## O Que Pode Ser Feito Sem Hardware

- Posicionar placeholders mecanicos com nomes reais de funcao.
- Separar regioes: sensores analogicos, logica, motores, buck, LiPo e succao.
- Revisar se J1/J2 batem com `docs/HPS-001_Pinout_Specification.md`.
- Criar net names e comentarios para reduzir erro de revisao.
- Criar checklist de DRC/ERC e fabricacao, ainda sem gerar Gerber.
- Criar lista de medidas para fazer assim que as pecas chegarem.
- Revisar o visual do robo para manter formato de competicao: frente estreita,
  longarinas e corpo traseiro largo.

## O Que Fica Bloqueado Ate Chegar Hardware

- Confirmar dimensoes reais de Black Pill, ESP32, TB6612, Mini360, bateria,
  motores, rodas, sensor vermelho, fan/ESC, chave e conectores.
- Confirmar altura dos modulos e risco de colisao entre bateria, capacitor,
  conectores e Brain Board.
- Validar orientacao de pin 1 dos modulos.
- Validar corrente de motor e succao.
- Validar ruido dos sensores com buck/motor ligados.
- Rodar revisao impressa 1:1.

## HDB-100 - Estado Apos Este Sprint

Novos placeholders no PCB:

| Ref | Funcao | Net inicial |
|---|---|---|
| JBAT | entrada LiPo 2S / XT30 placeholder | `VBAT_RAW`, `GND` |
| SW_PWR | chave geral placeholder | `VBAT_RAW` para `VMOTOR` |
| CBULK1 | capacitor bulk 470-1000 uF placeholder | `VMOTOR`, `GND` |
| J_ML | motor esquerdo | `AOUT1`, `AOUT2` |
| J_MR | motor direito | `BOUT1`, `BOUT2` |
| J_FAN | fan/ESC/succao placeholder | `VMOTOR`, `GND`, `FAN_PWM`, `5V_IN` |

Observacao: `Q_FAN` ainda nao foi implementado como circuito real. A placa esta
marcada com `Q_FAN OR ESC DRIVER TBD` para manter a decisao visivel.

## Revisao Visual Esperada

- A frente deve continuar estreita para sensor.
- As longarinas devem levar sinais de sensor para o corpo principal.
- A traseira deve concentrar LiPo, motores, buck, TB6612, chave e succao.
- A Brain Board deve empilhar acima da HDB-100 com espacadores.
- Conectores de bateria e chave devem ficar acessiveis sem desmontar o robo.

## Lista Para Segunda-feira, 2026-07-13

- Medir todos os modulos com paquimetro.
- Fotografar frente e verso dos modulos.
- Confirmar pitch e diametro de pino dos conectores.
- Confirmar altura maxima da bateria e do capacitor bulk.
- Confirmar se fan/ESC usa 2, 3 ou 4 fios.
- Confirmar orientacao real do XT30 no chassi.
- Imprimir outline 1:1 e colocar componentes por cima.

## Regra De Ouro

Enquanto houver placeholder nao medido, Gerber final continua bloqueado.
