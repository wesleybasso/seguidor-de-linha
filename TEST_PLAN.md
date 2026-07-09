# TEST_PLAN.md - Plano De Testes PEGASUS Rev A0

Atualizado: 2026-07-08.

## Principios

- Enquanto nao houver hardware fisico, testar apenas consistencia KiCad/docs.
- Primeiro testar sem motores e sem succao.
- Depois testar potencia com fonte limitada.
- Depois motores com rodas suspensas.
- Depois succao isolada.
- So depois colocar no chao, em velocidade baixa.
- `STOP_RUN` precisa funcionar em todas as fases com atuadores.

## F-KiCad - Sem Hardware Fisico

Objetivo: preparar a Rev A0 para revisao humana sem gerar Gerber.

Testes:

- Conferir balanceamento de parenteses dos arquivos `.kicad_pcb` e
  `.kicad_mod`.
- Conferir se todo placeholder novo esta listado em BOM e pendencias.
- Conferir se `VBAT_RAW` e `VMOTOR` nao foram misturados sem chave.
- Conferir visualmente se bateria, chave, XT30, capacitor e conectores estao
  em regioes plausiveis.
- Confirmar que documentos continuam dizendo que Gerber final esta bloqueado.

Sucesso:

- KiCad abre os projetos.
- Nomes de nets e referencias sao legiveis.
- Toda decisao sem medida real aparece em `DECISIONS_PENDING.md`.

## F0 - Protoboard Sem Motores

Objetivo: validar ESP32 <-> STM32.

Testes:

- ESP32 AP `PEGASUS_SETUP` abre dashboard em `192.168.4.1`.
- UART 115200 8N1 entre ESP32-S3-Zero GPIO43/44 e STM32 PA10/PA9 com GND comum.
- `PING/PONG` por 1000 ciclos.
- CRC invalido gera rejeicao controlada.
- Comandos validos geram `ACK`; comandos invalidos geram `NACK`.
- Telemetria fake aparece no dashboard por 10 minutos sem travar.

Sucesso:

- Sem reset espontaneo.
- Sem perda de link recorrente.
- Ultimo ACK/NACK e idade da telemetria visiveis.

## F1 - Sensores

Objetivo: validar sensores frontais e laterais.

Testes:

- Ler raw de A0-A7 e laterais.
- Calibrar fundo e linha.
- Verificar normalizacao 0-1000.
- Validar linha perdida, cruzamento e lateral direita/esquerda.
- Medir ruido com motores e buck desligados, depois ligados.

Sucesso:

- Cada canal muda claramente com contraste.
- Thresholds sao reproduziveis.
- Ruido nao confunde linha com cruzamento.

## F2 - Potencia Sem Carga

Objetivo: validar alimentacao antes de motores.

Testes:

- Usar fonte de bancada limitada no lugar da LiPo.
- Conferir 5V, 3V3, GND comum e queda de tensao.
- Medir PWM/STBY do TB6612 e FAN_PWM no osciloscopio.
- Confirmar TB6612 em standby no boot.

Sucesso:

- Tensoes dentro da margem.
- Sem aquecimento anormal de buck.
- Nenhum atuador liga sozinho.

## F3 - Motores Suspensos

Objetivo: validar TB6612 e motores N20 sem tocar o chao.

Testes:

- Rodas no ar.
- PWM baixo.
- Sentido esquerda/direita.
- Brake/coast.
- STOP imediato.
- Corrente de stall por teste curto e controlado.

Sucesso:

- Sentido correto.
- STOP corta imediatamente.
- Corrente e temperatura dentro da margem.

## F4 - Succao Isolada

Objetivo: validar fan/EDF/ESC sem tracao.

Testes:

- Sem motores de tracao.
- Corrente medida.
- Spin-up controlado.
- FAN_PWM validado.
- Protecao mecanica do rotor/duto.

Sucesso:

- Sem brownout.
- Sem vibracao excessiva.
- Corrente dentro da capacidade do sistema.

## F5 - Integracao Suspensa

Objetivo: sensores + motores + succao com rodas no ar.

Testes:

- `ARM -> FAN_SPINUP -> RUNNING -> SAFE_STOP`.
- Telemetria em tempo real.
- STOP em todos os estados ativos.
- Logs coerentes.

Sucesso:

- Sem reset.
- Sem estado incoerente.
- Atuadores obedecem safety.

## F6 - Primeira Pista Lenta

Objetivo: seguir linha de forma controlada.

Testes:

- Perfil SAFE.
- Base speed baixo.
- Succao desligada ou baixa.
- Linha reta e curva simples.

Sucesso:

- Segue linha.
- Para seguro ao perder linha.
- Nao satura motor de forma permanente.

## F7 - Pre-Competicao

Objetivo: repetibilidade.

Testes:

- 5 a 10 voltas.
- Bateria cheia e meia carga.
- Temperatura de TB6612, buck, motores, bateria e succao.
- Revisao mecanica apos cada run.

Sucesso:

- Comportamento repetivel.
- Logs completos.
- Sem aquecimento perigoso.
