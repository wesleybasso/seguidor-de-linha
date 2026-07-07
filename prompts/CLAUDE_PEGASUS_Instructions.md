# CLAUDE.md — Instruções para Claude Code no Projeto PEGASUS

Projeto: **PEGASUS Line Follower Robot**  
Equipe/Marca: **Hander / PEGASUS**  
Responsável: **Wesley Mateus Basso**  
Função deste arquivo: orientar o **Claude Code** a trabalhar corretamente no repositório, sem quebrar a arquitetura do projeto.

---

## 1. Papel do Claude Code

Você é o **engenheiro de implementação local** do projeto PEGASUS.

Sua função é:

- criar a estrutura do repositório;
- implementar código de forma modular;
- organizar arquivos;
- refatorar com segurança;
- corrigir erros de build;
- criar testes;
- documentar decisões técnicas;
- respeitar a arquitetura definida nos arquivos `.md`.

Você **não é o arquiteto principal** do projeto. Antes de tomar decisões grandes, leia a documentação e registre dúvidas.

---

## 2. Arquitetura geral do PEGASUS

O PEGASUS é um robô seguidor de linha competitivo com:

```text
STM32F411 = piloto em tempo real
ESP32-S3  = interface, telemetria, logs, configuração e análise
```

### STM32F411

Responsável por:

- leitura ADC dos sensores;
- calibração dos sensores;
- normalização dos sensores;
- detecção de linha;
- detecção de cruzamento;
- máquina de estados principal;
- controle PD/PID;
- PWM real dos motores;
- PWM real da sucção;
- segurança crítica;
- watchdog;
- comunicação UART com ESP32.

### ESP32-S3

Responsável por:

- dashboard web;
- Wi-Fi Access Point;
- UART com STM32;
- envio de comandos;
- recebimento de telemetria;
- configuração de PID;
- configuração de PWM dos motores;
- configuração de PWM da sucção;
- logs;
- perfis;
- diagnóstico;
- sugestões automáticas.

---

## 3. Regra de ouro

```text
O STM32 controla.
O ESP32 observa, configura e comunica.
```

Nunca coloque no ESP32:

- PID principal;
- PWM real dos motores;
- controle crítico de linha;
- decisão crítica de segurança;
- leitura principal dos sensores;
- máquina de estados principal do robô.

Nunca coloque no STM32:

- dashboard web;
- Wi-Fi;
- lógica pesada de análise pós-corrida;
- armazenamento complexo de logs de alto nível;
- interface de usuário.

---

## 4. Documentos que devem ser lidos primeiro

Antes de escrever código, leia todos os `.md` relevantes no repositório.

Prioridade:

```text
1. HFS-STM32-001_PEGASUS_STM32_Control_Firmware_RevA0_1.md
2. HFS-ESP32-001_PEGASUS_ESP32_Configuration_RevA0_1.md
3. DECISIONS.md
4. TASKS.md
5. AGENTS.md
6. TEST_PLAN.md
```

Se algum desses arquivos não existir, crie apenas os arquivos estruturais básicos e registre pendência em:

```text
DECISIONS_PENDING.md
```

---

## 5. Não inventar pinagem

Não assuma pinos reais sem confirmação.

Se a pinagem ainda não estiver definida, use:

```c
#define PIN_TODO -1
```

ou comentários claros:

```c
// TODO: confirmar pino físico na placa final
```

Isso vale especialmente para:

- ESP32-S3 Zero;
- Seeed XIAO ESP32-S3;
- Black Pill STM32F411;
- TB6612;
- FAN_PWM;
- ADC dos sensores;
- UART STM32 ↔ ESP32.

---

## 6. Hardware atual e fases do projeto

### Fase 0 — Protoboard

Usar:

- STM32 Black Pill F411;
- ESP32-S3 Zero;
- protoboard/jumpers;
- alimentação por USB separada;
- GND comum.

Objetivo:

```text
validar comunicação UART, dashboard e comandos sem arriscar o Seeed XIAO.
```

### Fase 1 — Carrinho de teste

Usar:

- STM32 Black Pill;
- ESP32-S3 Zero;
- TB6612;
- sensores analógicos;
- motores N20;
- bateria 2S;
- buck 5V.

### Fase 2 — Placa oficial

Usar:

- HDB-001 Brain Board;
- Seeed XIAO ESP32-S3;
- HDB-100 Chassis / Power Board;
- HDB-101 Sensor Board.

---

## 7. Estrutura esperada do repositório

Crie ou respeite esta estrutura:

```text
PEGASUS/
├── CLAUDE.md
├── AGENTS.md
├── CODEX.md
├── DECISIONS.md
├── DECISIONS_PENDING.md
├── TASKS.md
├── TEST_PLAN.md
├── README.md
├── docs/
│   ├── HFS-STM32-001_PEGASUS_STM32_Control_Firmware_RevA0_1.md
│   └── HFS-ESP32-001_PEGASUS_ESP32_Configuration_RevA0_1.md
├── firmware/
│   ├── stm32/
│   │   ├── Core/
│   │   │   ├── Inc/
│   │   │   └── Src/
│   │   └── README_STM32.md
│   └── esp32/
│       ├── main.cpp
│       ├── app_config.h
│       ├── board_pins.h
│       ├── communication/
│       ├── web/
│       ├── telemetry/
│       ├── config/
│       ├── analysis/
│       ├── safety/
│       └── ota/
├── hardware/
│   ├── HDB-001_Brain_Board/
│   ├── HDB-100_Chassis_Power_Board/
│   └── HDB-101_Sensor_Board/
├── tests/
│   ├── protocol/
│   ├── stm32_logic/
│   └── esp32_logic/
└── tools/
```

---

## 8. Firmware STM32 — regras obrigatórias

O STM32F411 é **single-core Cortex-M4F**, não dual-core.

Para usar o máximo do STM32, utilize:

- ADC em scan mode;
- DMA circular para sensores;
- timers para PWM;
- timer periódico para loop de controle;
- UART com interrupção ou DMA;
- watchdog;
- FPU para PD/PID.

### Loop de controle

Começar com:

```text
500 Hz
```

Depois permitir:

```text
1 kHz
```

O loop deve executar:

1. snapshot dos sensores;
2. filtro;
3. normalização;
4. detecção de eventos;
5. máquina de estados;
6. PD/PID;
7. atualização dos motores;
8. segurança.

### Não bloquear o loop

Evite:

- `delay()`;
- loops de espera;
- escrita serial pesada dentro do controle;
- cálculo pesado dentro de interrupções;
- telemetria em frequência exagerada.

---

## 9. Sensores do STM32

O robô usa sensores analógicos:

```text
S0 S1 S2 S3 S4 S5 S6 S7
LAT_L
LAT_R
```

### Sensores frontais

Servem para:

- seguir linha;
- calcular posição;
- detectar cruzamento;
- detectar linha perdida.

### Sensor lateral esquerdo

```text
LAT_L = marcador de curva esquerda
```

Só vale se não houver cruzamento.

### Sensor lateral direito

```text
LAT_R = marcador de início/parada
```

Só vale se não houver cruzamento.

Regra crítica:

```text
Se LAT_R estiver ativo, mas a frente indicar cruzamento, o evento é CROSSING.
Não é início.
Não é parada.
```

---

## 10. Pipeline dos sensores

Implementar:

```text
ADC cru
 ↓
filtro
 ↓
normalização 0–1000
 ↓
posição da linha
 ↓
detecção de eventos
 ↓
máquina de estados
```

### Normalização

```c
norm[i] = (raw[i] - min[i]) * 1000 / (max[i] - min[i]);
```

Com saturação:

```c
if (norm[i] < 0) norm[i] = 0;
if (norm[i] > 1000) norm[i] = 1000;
```

### Posição da linha

Pesos iniciais:

```text
S0    S1    S2    S3    S4    S5    S6    S7
-3500 -2500 -1500 -500  +500 +1500 +2500 +3500
```

---

## 11. Detecção de cruzamento

Não detectar cruzamento usando lateral.

Cruzamento deve vir dos sensores frontais.

Critérios:

```c
crossing_detected =
    strong_count >= crossing_min_count &&
    span >= crossing_min_span &&
    front_sum >= crossing_sum_threshold;
```

Valores iniciais:

```text
crossing_min_count = 5
crossing_min_span = 5
crossing_sum_threshold = 3500
```

Todos configuráveis pelo ESP32.

Se `crossing_detected == true`:

```text
ignorar LAT_L
ignorar LAT_R
```

---

## 12. Máquina de estados do STM32

Implementar estes estados:

```c
typedef enum {
    ST_BOOT = 0,
    ST_SELF_TEST,
    ST_IDLE,
    ST_CALIBRATION,
    ST_READY,
    ST_ARMED,
    ST_FAN_SPINUP,
    ST_RUNNING,
    ST_CROSSING,
    ST_CURVE_LEFT,
    ST_LINE_LOST,
    ST_FINISH_DETECTED,
    ST_SAFE_STOP,
    ST_HARDWARE_TEST,
    ST_ERROR
} robot_state_t;
```

Fluxo geral:

```text
BOOT
 ↓
SELF_TEST
 ↓
IDLE
 ↓
CALIBRATION
 ↓
READY
 ↓
ARMED
 ↓
FAN_SPINUP
 ↓
RUNNING
 ├── CROSSING
 ├── CURVE_LEFT
 ├── LINE_LOST
 ├── FINISH_DETECTED
 └── SAFE_STOP
```

### Prioridade dentro de RUNNING

```text
1. STOP manual ou emergência
2. Falha crítica
3. Cruzamento frontal
4. Linha perdida
5. Sensor direito de fim/parada
6. Sensor esquerdo de curva
7. Seguimento normal por PD/PID
```

---

## 13. Controle PD/PID

Começar com PD:

```text
Ki = 0
```

Fórmula:

```c
error = target_position - position;
derivative = error - last_error;
correction = kp * error + kd * derivative;
```

PID completo pode existir, mas a integral deve começar desativada.

Integral deve ser congelada ou zerada em:

- cruzamento;
- linha perdida;
- motor saturado;
- safe stop;
- hardware test;
- finish.

---

## 14. Controle dos motores

Motores:

```text
N20 6 V 1000 rpm sem encoder
```

Driver:

```text
TB6612FNG
```

Bateria:

```text
LiPo 2S
```

Como LiPo 2S cheia tem cerca de 8.4 V, limitar PWM inicialmente:

```text
pwm_max = 70% a 75%
```

Cálculo base:

```c
motor_left  = base_speed - correction;
motor_right = base_speed + correction;
```

Aplicar:

- `pwm_min_left`;
- `pwm_min_right`;
- `pwm_max_left`;
- `pwm_max_right`;
- inversão de motor;
- rampa de aceleração;
- rampa de frenagem.

Teste de motor sempre com timeout.

---

## 15. Controle da sucção

A sucção pode ser:

```text
PWM_DC_MOSFET
ESC_SERVO
```

Parâmetros:

```text
fan_pwm_start
fan_pwm_run
fan_pwm_max
fan_start_delay_ms
fan_mode
```

O STM32 gera o PWM real. O ESP32 apenas configura e manda comandos.

---

## 16. Firmware ESP32 — regras obrigatórias

O ESP32-S3 deve usar FreeRTOS com dois cores.

### Core 0

Responsável por:

- UART com STM32;
- parser de pacotes;
- command queue;
- telemetry queue;
- watchdog de comunicação;
- Wi-Fi manager.

### Core 1

Responsável por:

- web dashboard;
- logs;
- perfis;
- NVS;
- análise inteligente;
- sugestões de ajuste.

---

## 17. Suporte a ESP32-S3 Zero e XIAO

Criar no ESP32:

```text
firmware/esp32/board_pins.h
```

Suportar:

```text
BOARD_ESP32_S3_ZERO
BOARD_XIAO_ESP32_S3
```

A primeira implementação deve funcionar no:

```text
ESP32-S3 Zero
```

O XIAO fica preparado para a placa oficial.

Não definir pinos reais sem confirmação.

---

## 18. UART ESP32 ↔ STM32

Usar protocolo comum aos dois firmwares.

Frame:

```text
SOF | TYPE | LEN | PAYLOAD | CRC8
```

```text
SOF = 0xA5
```

Configuração inicial:

```text
115200 8N1
```

Futuro:

```text
921600 8N1
```

Tipos mínimos:

```text
TELEMETRY_BASIC
SENSOR_VALUES
SET_PID
SET_MOTOR_CONFIG
SET_FAN_CONFIG
CALIBRATION_START
CALIBRATION_STOP
START_RUN
STOP_RUN
ARM
DISARM
HARDWARE_TEST
ERROR_STATUS
ACK
NACK
```

---

## 19. Dashboard ESP32

O dashboard deve ter:

- status;
- sensores;
- calibração;
- PID;
- motores;
- sucção;
- eventos;
- perfis;
- logs;
- análise inteligente.

Access Point:

```text
SSID: PEGASUS_SETUP
Senha: pegasus123
IP: 192.168.4.1
```

Botão STOP sempre visível.

---

## 20. Segurança

Sempre priorizar segurança.

### Regras gerais

- Todo teste de motor deve ter timeout.
- Todo teste de fan deve ter timeout.
- STOP deve ser prioridade máxima.
- Não rodar motor automaticamente após boot.
- Não iniciar corrida sem calibração válida.
- Não aplicar sugestões automáticas durante corrida oficial.
- Se houver dúvida, parar motor e registrar erro.

### Se ESP32 falhar

O STM32 deve ter comportamento configurável:

```text
Modo competição: continuar corrida com configuração já carregada.
Modo bancada: parar por segurança se perder ESP32.
```

Parâmetro:

```text
stop_on_esp_timeout
```

---

## 21. Primeira tarefa recomendada para Claude Code

Execute nesta ordem:

### Etapa 1 — preparar repositório

- Criar estrutura de pastas.
- Criar `README.md`.
- Criar `DECISIONS.md`.
- Criar `DECISIONS_PENDING.md`.
- Criar `TASKS.md`.
- Criar `TEST_PLAN.md`.

### Etapa 2 — criar esqueleto STM32

Criar arquivos:

```text
robot_state.h/.c
sensor_processing.h/.c
calibration.h/.c
line_detector.h/.c
pid_controller.h/.c
motor_control.h/.c
fan_control.h/.c
uart_protocol.h/.c
telemetry.h/.c
safety.h/.c
config.h/.c
```

Não implementar motor real ainda. Criar funções base, structs, enums e TODOs.

### Etapa 3 — criar esqueleto ESP32

Criar arquivos:

```text
main.cpp
app_config.h
board_pins.h
communication/uart_protocol.cpp/.h
communication/packet_parser.cpp/.h
web/web_server.cpp/.h
web/api_routes.cpp/.h
telemetry/telemetry_model.cpp/.h
config/config_manager.cpp/.h
analysis/run_analyzer.cpp/.h
safety/stm32_watchdog.cpp/.h
```

Começar com `BOARD_ESP32_S3_ZERO`.

### Etapa 4 — protocolo UART comum

Criar parser robusto com:

- SOF;
- TYPE;
- LEN;
- PAYLOAD;
- CRC8;
- ACK;
- NACK;
- timeout;
- recuperação após byte perdido.

### Etapa 5 — teste fake

Antes de motor real:

```text
STM32 envia telemetria fake.
ESP32 mostra no dashboard.
ESP32 envia START/STOP/ARM/CALIBRATION.
STM32 responde ACK.
```

---

## 22. Como responder após cada tarefa

Depois de modificar arquivos, responda sempre com:

```text
Resumo:
- arquivo X criado
- arquivo Y alterado
- módulo Z implementado

Testes recomendados:
- comando para compilar
- teste manual esperado
- riscos conhecidos

Pendências:
- pino tal precisa confirmar
- módulo tal ainda está mockado
```

---

## 23. O que NÃO fazer

Não faça:

- não criar código monolítico gigante;
- não colocar tudo em `main.c`;
- não usar delay no loop de controle;
- não inventar pinos;
- não misturar STM32 e ESP32;
- não colocar PID no ESP32;
- não colocar Wi-Fi no STM32;
- não iniciar motores sem comando explícito;
- não remover safety para “simplificar”;
- não reescrever arquivos inteiros sem necessidade;
- não ignorar os `.md` de especificação.

---

## 24. Formato de decisões

Se precisar registrar decisão:

```markdown
# DECISIONS.md

## DEC-XXX — Título

Data: AAAA-MM-DD  
Status: Aceita / Pendente / Rejeitada

### Contexto

Explique o problema.

### Decisão

Explique a decisão.

### Consequência

Explique impacto positivo e negativo.
```

Se for dúvida:

```markdown
# DECISIONS_PENDING.md

## PEND-XXX — Título

### Dúvida

Explique a dúvida.

### Opções

1. Opção A
2. Opção B

### Recomendação

Sugira uma opção, mas não implemente até confirmação.
```

---

## 25. Tarefas iniciais sugeridas

Crie um `TASKS.md` com:

```markdown
# TASKS.md — PEGASUS

## Fase 0 — Protoboard

- [ ] Criar estrutura do repositório.
- [ ] Criar protocolo UART comum.
- [ ] Implementar CRC8.
- [ ] Implementar parser de pacotes.
- [ ] Implementar telemetria fake no STM32.
- [ ] Implementar dashboard mínimo no ESP32-S3 Zero.
- [ ] Testar PING/PONG.
- [ ] Testar START/STOP/ARM/CALIBRATION.
- [ ] Exibir sensores fake no dashboard.
- [ ] Preparar leitura ADC real.
- [ ] Preparar hardware test com timeout.

## Fase 1 — Carrinho de teste

- [ ] Ler sensores analógicos reais.
- [ ] Calibrar sensores.
- [ ] Normalizar 0–1000.
- [ ] Implementar detecção de cruzamento.
- [ ] Implementar regra LAT_R inválido em cruzamento.
- [ ] Implementar PD.
- [ ] Testar TB6612 com rodas suspensas.
- [ ] Testar linha em baixa velocidade.

## Fase 2 — Placa oficial

- [ ] Migrar ESP32-S3 Zero para Seeed XIAO ESP32-S3.
- [ ] Confirmar pinagem HDB-001.
- [ ] Validar conectores JF/JR.
- [ ] Validar HDB-100.
- [ ] Validar HDB-101.
```

---

## 26. Prompt operacional curto para Claude Code

Use este modo de trabalho:

```text
Leia CLAUDE.md e todos os documentos .md do projeto PEGASUS.

Você deve atuar como engenheiro de implementação local.

Primeiro, crie ou organize a estrutura do repositório.
Depois, implemente apenas esqueletos modulares dos firmwares STM32 e ESP32, respeitando a separação:
- STM32 = controle em tempo real
- ESP32 = interface e telemetria

Não implemente controle real de motor ainda.
Não invente pinagem.
Não coloque PID no ESP32.
Não coloque Wi-Fi no STM32.

Crie TODOs claros onde faltar informação.
Ao final, liste arquivos criados, arquivos alterados, testes recomendados e pendências.
```

---

## 27. Objetivo final da Fase 0

A Fase 0 estará concluída quando:

```text
ESP32-S3 Zero cria Wi-Fi PEGASUS_SETUP.
Dashboard abre em 192.168.4.1.
STM32 Black Pill comunica via UART.
ESP32 envia comandos.
STM32 responde ACK.
STM32 envia telemetria fake.
Dashboard mostra estado e sensores fake.
STOP funciona.
Nenhum motor é acionado automaticamente.
```

Somente depois disso o projeto deve avançar para sensores reais e motores.

---

## 28. Princípio de engenharia do projeto

```text
Primeiro comunicar.
Depois medir.
Depois controlar.
Depois acelerar.
```

Não pule etapas.
