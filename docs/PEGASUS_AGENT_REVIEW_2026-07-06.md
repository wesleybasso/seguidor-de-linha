# PEGASUS Agent Review - 2026-07-06

Relatorio consolidado dos quatro agentes criados para revisar o projeto em
paralelo. Nenhum agente alterou arquivos; todos trabalharam em modo de analise.

## Firmware Core

Validacao executada:

- `python -m unittest discover tests/protocol` passou com 12 testes.

Achados criticos:

- O ESP32 ainda nao trata comando como transacao real. `/api/command` responde
  `queued`, mas nao confirma ACK/NACK, timeout, retry ou erro final.
- `ACK/NACK/PONG` chegam ao ESP32 como pacotes comuns; falta correlacionar com
  o comando enviado.
- HAL STM32 ainda e mock/fraca para UART, ADC, PWM, GPIO e watchdog.
- `HARDWARE_TEST` so entra em estado e espera timeout; ainda nao testa motor,
  fan, sensor ou LED.
- Dados compartilhados no ESP32 precisam de mutex ou snapshot atomico.
- Validacao STM32 deve preferir tamanho exato de payload e clamp de faixa.
- `battery_mv` e `pid_correction` ainda podem parecer reais no dashboard, mas
  sao incompletos.

Proximas tarefas sugeridas:

1. Criar command manager ESP32 com ACK/NACK, timeout, retry e ultimo erro.
2. Tornar STOP prioritario fora da fila comum.
3. Criar HAL minima STM32Cube com USART1 PA9/PA10.
4. Definir payload seguro de `HARDWARE_TEST`.
5. Adicionar testes dos parsers C/C++ reais, nao apenas da referencia Python.

## Hardware / KiCad

Achados criticos:

- KiCad atual nao e PCB fabricavel: nao ha trilhas, vias ou zonas.
- Esquematicos ainda nao sao eletricos completos.
- Footprints sao placeholders e nao provam numeracao fisica.
- HDB-001 ainda nao liga UART STM32 PA9/PA10 ao ESP32 escolhido.
- HDB-100 ainda falta XT30 real, chave, capacitores, conectores de motor,
  fan/ESC, protecao e poligonos de potencia.
- TB6612 segue com pinout pendente.
- A escolha XIAO ESP32-S3/Plus versus ESP32-S3-Zero precisa ser fechada.

Proximas tarefas sugeridas:

1. Fechar ESP32 definitivo.
2. Medir todos os modulos com paquimetro.
3. Criar footprints reais antes de roteamento.
4. Fazer HDB-001 eletrica real primeiro.
5. Fazer revisao mecanica impressa 1:1 antes de fabricar.

## Dashboard / UX

Pontos fortes:

- Mockup separado tem boa hierarquia operacional.
- STOP e status ficam em destaque.
- Visual escuro, cinza, laranja e branco combina com bancada/corrida.

Riscos:

- Mockup ainda usa dados fake e animacao local.
- UI embarcada ainda mostra JSON cru demais.
- Falta fluxo guiado de bancada.
- Falta feedback confiavel de comando: ACK/NACK, timeout, RTT, ultimo pacote.
- Controles perigosos precisam ser habilitados conforme estado STM32.

Proximas tarefas sugeridas:

1. Usar o mockup como fonte de design, nao copiar 31 KB direto para `web_ui.h`.
2. Criar modo Bring-up.
3. Mostrar idade da telemetria e link stale/offline.
4. Expor `/api/summary` e `/api/analyze` na UI.
5. Minificar e, se necessario, servir gzip em `PROGMEM`.

## Validacao / Safety

Achados criticos:

- Antes de atuadores, validar PING/PONG, CRC e dashboard por tempo continuo.
- Antes de motores, usar fonte limitada.
- Antes de pista, testar motores suspensos.
- Succao/EDF/ESC e o maior risco mecanico e eletrico.
- Resolver divergencia entre requisito `>= 1 kHz` e `PEGASUS_CONTROL_HZ 500`.

Plano sugerido:

- F0: protoboard sem motores.
- F1: sensores.
- F2: potencia sem carga.
- F3: motores suspensos.
- F4: succao isolada.
- F5: integracao suspensa.
- F6: primeira pista lenta.
- F7: pre-competicao.

## Conclusao

O projeto ja tem espinha dorsal: specs, firmware base, protocolo, dashboard
mockup, KiCad mecanico e testes iniciais. O salto para "ouro" nao e adicionar
mais coisa solta; e fechar contratos observaveis: UART confiavel, STOP
prioritario, footprints reais, testes por fase e decisoes P0 resolvidas.

