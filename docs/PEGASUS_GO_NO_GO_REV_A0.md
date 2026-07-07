# PEGASUS Go/No-Go - Rev A0

Atualizado: 2026-07-06.

Este documento define o significado de "pronto" por gate. O projeto nao deve
ser tratado como pronto para pista ou competicao enquanto os gates fisicos nao
forem vencidos com evidencia.

## Status Atual

- F0 comunicacao: GO parcial em software, NO-GO ate teste real 1000 ciclos.
- F1 sensores reais: NO-GO.
- F2 potencia sem carga: NO-GO.
- F3 motores suspensos: NO-GO.
- F4 succao isolada: NO-GO critico.
- F5 integracao suspensa: NO-GO.
- F6 primeira pista lenta: NO-GO.
- F7 pre-competicao: NO-GO.

## Gate F0 - Comunicacao Sem Motores

Para virar GO:

- `PING/PONG` real por 1000 ciclos.
- CRC invalido rejeitado.
- `ACK/NACK` em comandos validos/invalidos.
- Dashboard aberto por 10 minutos sem travar.
- STOP enfileirado com prioridade e visivel no painel.

## Gate F1 - Sensores

Para virar GO:

- Raw e normalizado 0-1000 medidos em A0-A7.
- LAT_L/LAT_R definidos como analogico ou digital.
- Ruido medido com buck, motor e fan em cenarios separados.
- Calibracao repetivel em piso real.

## Gate F2 - Potencia Sem Carga

Para virar GO:

- Fonte limitada antes de LiPo.
- 5V, 3V3 e VMOTOR medidos.
- TB6612 em standby no boot.
- PWMA/PWMB/STBY/FAN_PWM conferidos no osciloscopio.
- Nenhum atuador liga sozinho.

## Gate F3 - Motores Suspensos

Para virar GO:

- HAL STM32 real para PWM/GPIO.
- Rodas suspensas.
- STOP corta PWM/STBY/brake dentro do tempo medido.
- Sentido L/R confirmado.
- Corrente de stall medida por teste curto e controlado.
- Temperatura de TB6612 e motores registrada.

## Gate F4 - Succao Isolada

Para virar GO:

- Motor/EDF/ESC escolhido.
- Corrente e brownout medidos.
- Rotor/duto protegidos mecanicamente.
- Vibracao e temperatura registradas.
- STOP corta succao.

## Gate F5 - Integracao Suspensa

Para virar GO:

- Sequencia `ARM -> FAN_SPINUP -> RUNNING -> SAFE_STOP` com rodas no ar.
- STOP provado em todos os estados ativos.
- Logs de telemetria coerentes.
- Sem reset por ruido ou queda de tensao.

## Gate F6 - Primeira Pista Lenta

Para virar GO:

- Perfil SAFE.
- Base speed baixo.
- Succao desligada ou baixa.
- Parada segura ao perder linha.
- Revisao termica e mecanica apos cada run.

## Gate F7 - Pre-Competicao

Para virar GO:

- 5 a 10 voltas repetiveis.
- Teste com bateria cheia e meia carga.
- Logs completos.
- Temperaturas dentro da margem.
- Fixacoes mecanicas revisadas.

## Bloqueios P0

- STOP prioritario ponta a ponta ainda precisa evidencia em hardware.
- HAL STM32 real ainda nao substituiu todos os simbolos weak.
- `HARDWARE_TEST` ainda precisa payload seguro antes de acionar atuadores.
- LiPo 2S cheia exige limite formal para motores 6 V.
- `battery_mv` ainda nao e medicao real.
- TB6612 real precisa pinout, corrente e temperatura.
- Succao e o maior risco aberto.
- Frequencia `>= 1 kHz` versus `PEGASUS_CONTROL_HZ 500` precisa decisao.
- Modo competicao sem ESP32 precisa politica fechada.
- PCB ainda depende de footprints reais, ERC/DRC e revisao humana.

