# DECISIONS.md - Decisoes Fechadas PEGASUS Rev A0

Atualizado: 2026-07-06.

## DEC-001 - Arquitetura Em Tres Placas

PEGASUS Rev A0 usa tres placas:

- HDB-001 Brain Board: Black Pill STM32F411 e ESP32 plugavel.
- HDB-100 Chassis / Power Board: LiPo 2S, buck, TB6612, motores e succao.
- HDB-101 Sensor Board: adaptador frontal para sensor vermelho de 8 canais.

Motivo: separar logica, potencia e sensor para facilitar teste, manutencao e
iteracao barata.

## DEC-002 - Fase 0 Em Protoboard

A Fase 0 fica congelada em Black Pill STM32F411 + Waveshare ESP32-S3-Zero.
O XIAO ESP32-S3 permanece como alvo de placa oficial, mas ainda depende de
confirmacao de variante e pinagem.

## DEC-003 - STM32 E Dono Do Tempo Real

O STM32 executa leitura de sensores, maquina de estados, controle PID, motores,
succao e safety critico. O ESP32 e painel, configuracao, telemetria, perfis e
analise pos-corrida.

## DEC-004 - UART Como Contrato Entre Cerebros

ESP32 e STM32 se comunicam por UART 115200 8N1 com pacotes HCP-001, CRC8,
`ACK/NACK`, `PING/PONG` e telemetria estruturada.

## DEC-005 - STOP Tem Prioridade Absoluta

`STOP_RUN` deve estar sempre disponivel no dashboard e deve cortar atuadores no
STM32 mesmo se outros fluxos estiverem ativos.

## DEC-006 - KiCad Atual E Maquete Contratual

Os projetos KiCad atuais sao base mecanica e contratual. Eles ainda nao sao
PCB fabricavel porque usam placeholders, nao possuem esquematico eletrico real,
trilhas, vias ou zonas.

## DEC-007 - Sem Gerber Final Antes De Revisao Humana

Gerbers finais so podem existir apos footprints reais, ERC/DRC, conferencia
HPS contra KiCad, revisao mecanica 1:1 e revisao humana explicita.

