# RRS-001 — Robot Requirements Specification

Projeto: PEGASUS  
Revisão: A0  
Autor: Wesley Mateus Basso / Hander

## 1. Objetivo

Definir os requisitos do robô antes do desenvolvimento detalhado das PCBs.

## 2. Arquitetura física

O robô usa dois andares:

- **Andar inferior:** HDB-100 Chassis / Power Board.
- **Andar superior:** HDB-001 Brain Board.
- **Frente:** HDB-101 Sensor Board.

A PCB inferior deve funcionar como parte estrutural do chassi.

## 3. Formato

O robô deve ser alongado, inspirado em seguidores de linha competitivos.

Características:

- frente estreita com régua de sensores;
- corpo central/traseiro largo;
- motores nas laterais;
- bateria baixa e centralizada;
- sucção central;
- Brain Board empilhada acima da região de controle.

## 4. Componentes mecânicos/eletrônicos principais

### Motores

- Tipo: N20.
- Rotação: 1000 rpm.
- Tensão nominal: 6V.
- Encoder: não, na Rev A0.

### Bateria

- Tipo: LiPo 2S.
- Tensão nominal: 7.4V.
- Tensão cheia: 8.4V.
- Capacidade sugerida: 450 a 850 mAh.
- Capacidade final: pendente.

### Sucção

- Um motor de sucção central.
- Tipo ainda indefinido.
- A placa deve suportar:
  - motor DC via MOSFET; ou
  - EDF/brushless via ESC.

### Sensores

- Sensor frontal vermelho tipo QTR/HY-S301/8 canais.
- Entradas A0–A7.
- Sensores laterais unitários já comprados.

## 5. Objetivos de firmware

- Loop PID: >= 1 kHz.
- Leitura de sensores: >= 1 kHz.
- PWM motores: 20 a 50 kHz se possível.
- Comunicação STM32 ↔ ESP32 por UART.
- ESP32 não deve ser necessário para o robô competir.

## 6. Prioridades

1. Funcionar.
2. Ser barato.
3. Ser fácil de consertar.
4. Validar firmware.
5. Validar mecânica.
6. Depois integrar e miniaturizar.
