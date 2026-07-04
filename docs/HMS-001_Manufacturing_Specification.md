# HMS-001 — Manufacturing Specification

Projeto: PEGASUS  
Revisão: A0

## 1. PCB

- FR4.
- 1.6 mm.
- 2 camadas.
- Cobre 1 oz.
- Silkscreen branco.
- Máscara preta se custo permitir.

## 2. Regras de potência

- Trilhas de bateria/motor: mínimo 2.0 mm; preferir polígonos.
- Capacitor bulk próximo da entrada XT30.
- Capacitor próximo do TB6612.
- Buck perto da entrada, mas longe dos sinais analógicos.
- Corrente alta não deve passar pela Brain Board.

## 3. Regras de sinal

- Sinais A0–A7 longe de motor, buck e FAN.
- UART curta e com GND próximo.
- Test points:
  - VBAT
  - 5V
  - 3V3
  - GND
  - PWMA
  - PWMB
  - STBY
  - FAN_PWM
  - UART TX/RX

## 4. Mecânica

- Confirmar todos os footprints com paquímetro.
- Confirmar altura entre placas.
- Confirmar acesso ao USB-C do XIAO.
- Confirmar posição da bateria.
- Confirmar furo da ventosa.

## 5. Checklist antes de fabricar

- [ ] ERC sem erro crítico.
- [ ] DRC sem erro crítico.
- [ ] Gerbers conferidos.
- [ ] Drill conferido.
- [ ] Silkscreen conferido.
- [ ] Pin 1 de todos conectores identificado.
- [ ] Polaridade XT30 e motores conferida.
