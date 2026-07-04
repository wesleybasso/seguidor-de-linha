# HRS-001 — Hardware Requirements Specification

Projeto: PEGASUS  
Revisão: A0

## 1. Visão geral

O hardware é dividido em três placas:

- HDB-001 Brain Board.
- HDB-100 Chassis / Power Board.
- HDB-101 Sensor Board.

## 2. HDB-001 — Brain Board

### Função

Placa superior de lógica, controle, comunicação e debug.

### Componentes

- Black Pill STM32F411CEU6 plugável.
- XIAO ESP32-S3 ou XIAO ESP32-S3 Plus plugável.
- LEDs de status.
- Botões de boot/reset.
- Header SWD.
- Header UART debug.
- Conectores J1/J2 para chassi.
- Buck local opcional apenas para uso standalone.

### Não incluir

- TB6612.
- Motores.
- XT30.
- MOSFET de alta corrente.
- Capacitores grandes de potência.

## 3. HDB-100 — Chassis / Power Board

### Função

Placa inferior estrutural e de potência.

### Componentes

- XT30 para LiPo 2S.
- Chave geral ou chave de lógica.
- Capacitores bulk.
- Buck 5V.
- TB6612FNG.
- Conectores dos motores.
- Conector/área para sucção central.
- Furo/duto de sucção.
- Conectores J1/J2 para Brain Board.
- Conexão para Sensor Board.

## 4. HDB-101 — Sensor Board

### Função

Placa frontal de sensores.

### Rev A0

Pode ser interface para sensor comercial existente.

### Futuro

Sensor Board própria com sensores IR, resistores, filtros e conectores.

## 5. Alimentação

- Entrada: LiPo 2S.
- VBAT/VMOTOR: motores e sucção.
- 5V: lógica e Brain Board.
- 3V3: sensores e sinais lógicos.

## 6. Requisitos de segurança

- Todos os GNDs em comum.
- Corrente alta não deve passar pela Brain Board.
- Trilhas de potência largas.
- Capacitores próximos do TB6612, buck e entrada.
- Sinais analógicos afastados de motor/buck.
- Usar test points para sinais críticos.

## 7. Fabricação

- FR4.
- 1.6 mm.
- 2 camadas na Rev A0.
- Cobre 1 oz.
- Montagem manual possível.
- Silkscreen claro.
