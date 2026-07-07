# PEGASUS Gold Roadmap - Rev A0

Atualizado: 2026-07-06.

Objetivo: organizar o projeto como uma equipe de engenharia real, com times,
contratos, gates e entregas verificaveis.

## Equipes

### Time 1 - Firmware Tempo Real

Dono: Claude Code.

Escopo:

- STM32 state machine.
- Sensores, PID, motores, succao e safety.
- HAL real STM32Cube.
- Testes unitarios STM32.

Entregas proximas:

- HAL minima com USART1 PA9/PA10 e tick real.
- Validacao de payload com tamanho exato e clamp de faixa.
- `HARDWARE_TEST` seguro e parametrizado.

### Time 2 - ESP32 Dashboard E Telemetria

Dono: Claude Code, com revisao UX.

Escopo:

- Web server ESP32.
- Dashboard.
- Gerenciador de comandos.
- Perfis, NVS e analise pos-corrida.

Entregas proximas:

- ACK/NACK transacional.
- STOP prioritario.
- UI com estado do link, RTT, ultimo erro e gating por estado STM32.

### Time 3 - Hardware E KiCad

Dono: Codex.

Escopo:

- HDB-001, HDB-100, HDB-101 no KiCad.
- Footprints reais.
- DRC/ERC.
- BOM e fabricacao.

Entregas proximas:

- Escolher ESP32 definitivo da placa oficial.
- Criar HDB-001 eletrica real antes de roteamento.
- Substituir placeholders por footprints conferidos.

### Time 4 - Validacao E Safety

Dono: Codex + revisao humana.

Escopo:

- Plano de testes.
- Riscos eletricos/mecanicos.
- Checklist de bancada.
- Checklist pre-JLCPCB/PCBWay.

Entregas proximas:

- Executar F0 sem motores.
- Travar Gerbers enquanto placeholders existirem.
- Definir criterios de go/no-go por fase.

### Time 5 - Arquitetura E Produto

Dono: ChatGPT / humano.

Escopo:

- Requisitos oficiais.
- Pinagem HPS.
- Decisoes HDR.
- Estrategia de corrida.

Entregas proximas:

- Resolver decisoes pendentes P0.
- Manter docs sincronizados com firmware e KiCad.
- Definir escopo realista da Rev A0.1.

## Plano De Ataque Das Proximas 5 Horas

1. Congelar Fase 0: ESP32-S3-Zero + Black Pill + UART + GND comum.
2. Rodar testes de protocolo Python e builds ESP32/STM32.
3. Registrar riscos P0 em `DECISIONS_PENDING.md`.
4. Transformar matriz de validacao em `TEST_PLAN.md`.
5. Revisar dashboard mockup e manter separado do firmware embarcado.
6. Criar gate oficial: sem Gerber final antes de footprint real + ERC/DRC.
7. Commitar e enviar o pacote de coordenacao para GitHub.

## Gates

### Gate A - Bancada Sem Atuadores

Pode passar quando:

- `PING/PONG` passa por 1000 ciclos.
- Dashboard mostra STM32 online/offline corretamente.
- Telemetria fake roda por 10 minutos.
- STOP envia comando prioritario.

### Gate B - Atuadores Suspensos

Pode passar quando:

- Fonte limitada em uso.
- Motores no ar.
- STOP testado antes de START.
- Corrente e temperatura medidas.
- `HARDWARE_TEST` tem payload e timeout seguros.

### Gate C - PCB Fabricavel

Pode passar quando:

- ESP32 definitivo escolhido.
- Footprints reais validados com paquimetro.
- Esquematico eletrico completo.
- ERC/DRC limpos ou justificados.
- Revisao mecanica 1:1 feita.
- Gerbers abertos em viewer externo.

## Estado Honesto Hoje

O projeto esta acima de uma ideia: ja tem arquitetura, specs, firmware base,
dashboard mockup, protocolo, testes Python e KiCad mecanico.

Ainda nao esta pronto para fabricar nem para atuar motores reais sem bancada
controlada. O caminho para ficar forte e fechar os contratos de UART, safety,
footprints e testes antes de acelerar.

