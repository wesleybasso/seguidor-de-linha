# HFS-ESP32-001 — PEGASUS ESP32-S3 Configuration Specification

Projeto: **PEGASUS Line Follower Robot**  
Placa oficial futura: **HDB-001 Brain Board**  
Módulo oficial futuro: **Seeed XIAO ESP32-S3 / XIAO ESP32-S3 Plus**  
Módulo de bancada inicial: **ESP32-S3 Zero**  
Documento: **HFS-ESP32-001**  
Revisão: **A0.1 — atualizada com etapa de protoboard**  
Responsável: **Wesley Mateus Basso / Hander**

---

## 1. Objetivo deste documento

Este documento define a arquitetura do firmware do ESP32-S3 no projeto PEGASUS.

A nova decisão da Rev A0.1 é:

> Antes de usar o Seeed XIAO ESP32-S3 na placa oficial, será usado um **ESP32-S3 Zero** em protoboard para validar comunicação, dashboard, protocolo UART e integração com STM32 Black Pill.

Isso reduz risco de queimar o módulo Seeed XIAO e permite testar a lógica do sistema com baixo custo.

---

## 2. Papel do ESP32-S3 no PEGASUS

O ESP32-S3 atua como:

```text
copiloto
central de telemetria
interface web
configurador de parâmetros
logger
engenheiro de pista
```

Ele **não controla o robô em tempo real**.

```text
STM32F411 = piloto em tempo real
ESP32-S3  = interface, telemetria, logs, configuração e análise
```

---

## 3. Divisão entre STM32 e ESP32

### 3.1 STM32F411 — controle crítico

O STM32 é responsável por tudo que precisa ser determinístico:

- leitura ADC dos sensores analógicos;
- normalização dos sensores;
- detecção de linha;
- detecção de cruzamento;
- detecção dos sensores laterais;
- máquina de estados principal;
- PID;
- PWM real dos motores;
- PWM real da sucção;
- segurança crítica;
- watchdog;
- parada de emergência.

### 3.2 ESP32-S3 — interface e inteligência auxiliar

O ESP32 é responsável por:

- criar interface web local;
- receber telemetria do STM32 via UART;
- enviar comandos ao STM32;
- iniciar/parar calibração;
- exibir sensores frontais e laterais;
- alterar parâmetros PID;
- alterar PWM base, máximo e mínimo dos motores;
- alterar PWM da sucção;
- iniciar/parar o robô;
- salvar perfis de configuração;
- registrar logs de testes/corridas;
- analisar comportamento do robô;
- sugerir ajustes de PID, sensores, velocidade e sucção;
- gerenciar Wi-Fi/BLE;
- futuramente permitir OTA.

### 3.3 Regra fundamental

```text
ESP32 nunca deve gerar diretamente o PWM dos motores.
ESP32 nunca deve rodar o PID principal.
ESP32 nunca deve decidir sozinho segurança crítica.
```

O ESP32 pode sugerir, configurar e enviar comandos.  
O STM32 executa o controle real.

---

## 4. Estratégia de hardware por fases

### 4.1 Fase 0 — bancada/protoboard

Objetivo: validar firmware e comunicação antes da placa.

Usar:

- STM32 Black Pill F411;
- ESP32-S3 Zero;
- protoboard/jumpers;
- alimentação por USB separada inicialmente;
- GND comum obrigatório.

Arquitetura:

```text
PC / celular
    ↓ Wi-Fi
ESP32-S3 Zero
    ↓ UART
STM32 Black Pill
    ↓
telemetria fake primeiro
sensores reais depois
motores depois
```

Ligação mínima:

```text
STM32 PA9  TX  →  RX do ESP32-S3 Zero
STM32 PA10 RX  ←  TX do ESP32-S3 Zero
GND             ↔  GND
```

Recomendação inicial:

```text
ESP32-S3 Zero alimentado pelo próprio USB.
Black Pill alimentada pelo próprio USB/ST-Link.
GND comum entre os dois.
```

Não alimentar um módulo pelo outro no primeiro teste.

---

### 4.2 Fase 1 — carrinho de teste

Objetivo: validar comunicação + sensores + motores.

Usar:

- STM32 Black Pill;
- ESP32-S3 Zero;
- TB6612;
- sensores analógicos;
- motores N20;
- bateria 2S;
- buck 5V.

O ESP32-S3 Zero continua sendo usado para proteger o XIAO.

---

### 4.3 Fase 2 — Brain Board oficial

Objetivo: migrar o mesmo firmware para o Seeed XIAO ESP32-S3.

Usar:

- HDB-001 Brain Board;
- Seeed XIAO ESP32-S3 ou XIAO ESP32-S3 Plus;
- HDB-100 Chassis Board;
- HDB-101 Sensor Board.

O firmware deve mudar apenas o perfil de pinos.

---

## 5. Suporte a múltiplas placas ESP32

O firmware deve suportar pelo menos dois alvos:

```text
BOARD_ESP32_S3_ZERO
BOARD_XIAO_ESP32_S3
```

Criar arquivo:

```text
firmware/esp32/board_pins.h
```

Exemplo:

```cpp
#pragma once

// Selecione apenas uma placa por vez.
// #define BOARD_ESP32_S3_ZERO
// #define BOARD_XIAO_ESP32_S3

#if defined(BOARD_ESP32_S3_ZERO)

    #define BOARD_NAME "ESP32-S3 Zero Dev Prototype"

    // ATENÇÃO:
    // Preencher estes pinos conforme o modelo exato do ESP32-S3 Zero usado.
    // Não assumir pinagem sem conferir serigrafia/datasheet.
    #define STM32_UART_NUM      1
    #define STM32_UART_TX_PIN   -1  // TODO: definir
    #define STM32_UART_RX_PIN   -1  // TODO: definir

    #define STATUS_LED_PIN      -1  // TODO: definir se existir LED onboard
    #define BOOT_BUTTON_PIN     -1  // opcional

#elif defined(BOARD_XIAO_ESP32_S3)

    #define BOARD_NAME "Seeed XIAO ESP32-S3 Brain Board"

    // ATENÇÃO:
    // Preencher conforme o modelo exato: XIAO ESP32-S3 ou XIAO ESP32-S3 Plus.
    #define STM32_UART_NUM      1
    #define STM32_UART_TX_PIN   -1  // TODO: definir após pinout da HDB-001
    #define STM32_UART_RX_PIN   -1  // TODO: definir após pinout da HDB-001

    #define STATUS_LED_PIN      -1
    #define BOOT_BUTTON_PIN     -1

#else
    #error "Selecione BOARD_ESP32_S3_ZERO ou BOARD_XIAO_ESP32_S3"
#endif
```

Regra para o Codex:

> Não fixar pinos reais sem confirmação do modelo físico.  
> Deixar `TODO` quando a pinagem depender do módulo exato.

---

## 6. Uso dos dois cores do ESP32-S3

Usar FreeRTOS com tarefas fixadas em cores.

### Core 0 — Comunicação e infraestrutura

Responsável por:

- UART com STM32;
- parser de pacotes;
- envio de comandos;
- recepção de telemetria;
- watchdog de comunicação;
- gerenciamento Wi-Fi básico;
- OTA futura.

### Core 1 — Interface, logs e inteligência

Responsável por:

- web dashboard;
- API HTTP/WebSocket;
- logs;
- perfis;
- NVS/Flash;
- análise da corrida;
- sugestões automáticas;
- diagnóstico.

---

## 7. Tarefas FreeRTOS

### 7.1 Core 0

#### `uart_rx_task`

```text
Core: 0
Prioridade: Alta
Função: receber telemetria do STM32 sem perder pacotes.
```

Responsabilidades:

- ler bytes da UART;
- montar frames;
- validar CRC;
- atualizar estrutura de telemetria;
- enviar dados para `telemetry_queue`.

#### `uart_tx_task`

```text
Core: 0
Prioridade: Alta
Função: enviar comandos ao STM32.
```

Responsabilidades:

- receber comandos da `command_queue`;
- empacotar frames;
- calcular CRC;
- enviar para STM32;
- aguardar ACK/NACK quando necessário.

#### `stm32_watchdog_task`

```text
Core: 0
Prioridade: Média
Período: 100 ms
```

Responsabilidades:

- verificar tempo desde último pacote recebido;
- marcar STM32 como online/offline;
- notificar dashboard;
- bloquear comandos perigosos se STM32 estiver offline.

#### `wifi_manager_task`

```text
Core: 0
Prioridade: Média
```

Responsabilidades:

- criar Access Point;
- opcionalmente conectar a rede externa;
- manter status de rede;
- reiniciar Wi-Fi em caso de falha.

---

### 7.2 Core 1

#### `web_server_task`

```text
Core: 1
Prioridade: Média
```

Responsabilidades:

- servir página HTML;
- receber comandos do usuário;
- exibir telemetria;
- atualizar gráficos;
- expor API REST/WebSocket.

#### `logger_task`

```text
Core: 1
Prioridade: Baixa/Média
```

Responsabilidades:

- salvar telemetria em buffer;
- exportar CSV;
- registrar eventos;
- evitar escrita em Flash em frequência excessiva.

#### `analyzer_task`

```text
Core: 1
Prioridade: Baixa
```

Responsabilidades:

- calcular erro médio;
- calcular erro máximo;
- contar perdas de linha;
- contar saturação dos motores;
- detectar oscilação;
- detectar velocidade excessiva;
- sugerir ajustes.

#### `profile_manager_task`

```text
Core: 1
Prioridade: Baixa
```

Responsabilidades:

- salvar PID;
- salvar thresholds;
- salvar limites de PWM;
- salvar perfis de pista;
- carregar perfil ativo;
- resetar perfil padrão.

---

## 8. Comunicação interna no ESP32

Usar filas FreeRTOS.

Telemetria:

```text
STM32 UART
   ↓
uart_rx_task
   ↓
telemetry_queue
   ↓
web_server_task
logger_task
analyzer_task
```

Comandos:

```text
Web Dashboard
   ↓
command_queue
   ↓
uart_tx_task
   ↓
STM32
```

Filas recomendadas:

```cpp
QueueHandle_t telemetry_queue;
QueueHandle_t command_queue;
QueueHandle_t event_queue;
QueueHandle_t log_queue;
```

---

## 9. UART ESP32 ↔ STM32

Configuração inicial:

```text
Baudrate: 115200
Formato: 8N1
Flow control: nenhum
```

Configuração futura:

```text
Baudrate: 921600
```

Só subir baudrate depois da comunicação estar estável.

---

## 10. Protocolo UART

Frame binário:

```text
SOF | TYPE | LEN | PAYLOAD | CRC8
```

| Campo | Tamanho | Descrição |
|------|---------|-----------|
| SOF | 1 byte | Sempre `0xA5` |
| TYPE | 1 byte | Tipo da mensagem |
| LEN | 1 byte | Tamanho do payload |
| PAYLOAD | N bytes | Dados |
| CRC8 | 1 byte | Validação |

### Tipos de mensagem

| Código | Nome | Direção | Função |
|------:|------|---------|--------|
| `0x01` | TELEMETRY_BASIC | STM32 → ESP32 | Estado geral |
| `0x02` | SENSOR_VALUES | STM32 → ESP32 | Sensores analógicos |
| `0x03` | SET_PID | ESP32 → STM32 | Alterar PID |
| `0x04` | SET_MOTOR_CONFIG | ESP32 → STM32 | PWM motores |
| `0x05` | SET_FAN_CONFIG | ESP32 → STM32 | PWM sucção |
| `0x06` | CALIBRATION_START | ESP32 → STM32 | Iniciar calibração |
| `0x07` | CALIBRATION_STOP | ESP32 → STM32 | Finalizar calibração |
| `0x08` | START_RUN | ESP32 → STM32 | Iniciar corrida |
| `0x09` | STOP_RUN | ESP32 → STM32 | Parar robô |
| `0x0A` | ARM | ESP32 → STM32 | Armar robô |
| `0x0B` | DISARM | ESP32 → STM32 | Desarmar robô |
| `0x0C` | HARDWARE_TEST | ESP32 → STM32 | Teste de hardware |
| `0x0D` | ERROR_STATUS | STM32 → ESP32 | Erros |
| `0x0E` | ACK | Bidirecional | Confirmação |
| `0x0F` | NACK | Bidirecional | Falha |

---

## 11. Estados do robô exibidos pelo ESP32

O ESP32 deve refletir a máquina de estados do STM32.

```cpp
typedef enum {
    ROBOT_BOOT = 0,
    ROBOT_IDLE,
    ROBOT_CALIBRATION,
    ROBOT_READY,
    ROBOT_ARMED,
    ROBOT_RUNNING,
    ROBOT_CROSSING,
    ROBOT_CURVE_LEFT,
    ROBOT_FINISH_DETECTED,
    ROBOT_LINE_LOST,
    ROBOT_SAFE_STOP,
    ROBOT_ERROR
} robot_state_t;
```

Estados perigosos que devem aparecer em destaque:

- `ROBOT_LINE_LOST`;
- `ROBOT_SAFE_STOP`;
- `ROBOT_ERROR`;
- STM32 offline;
- bateria baixa.

---

## 12. Dashboard Web

O ESP32 deve criar um Access Point Wi-Fi.

```text
SSID padrão: PEGASUS_SETUP
Senha padrão: pegasus123
Endereço local: 192.168.4.1
```

---

## 13. Seções do Dashboard

### 13.1 Status

Mostrar:

- estado do robô;
- STM32 online/offline;
- bateria;
- linha detectada;
- cruzamento;
- sensor lateral esquerdo;
- sensor lateral direito;
- PWM motor esquerdo;
- PWM motor direito;
- PWM sucção;
- tempo de corrida.

### 13.2 Sensores

Mostrar:

```text
A0 A1 A2 A3 A4 A5 A6 A7
LAT_L
LAT_R
```

Para cada sensor mostrar:

- valor cru;
- valor normalizado;
- threshold;
- estado ativo/inativo.

Exibir gráfico em barras.

### 13.3 Calibração

Botões:

- iniciar calibração;
- finalizar calibração;
- cancelar calibração;
- ver qualidade da calibração;
- salvar calibração;
- resetar calibração.

Mostrar:

- mínimo de cada sensor;
- máximo de cada sensor;
- range;
- threshold calculado;
- ruído estimado.

### 13.4 PID

Parâmetros editáveis:

```text
Kp
Ki
Kd
integral_limit
derivative_filter
```

### 13.5 Motores

Parâmetros editáveis:

```text
base_speed
pwm_min_left
pwm_min_right
pwm_max_left
pwm_max_right
accel_limit
brake_limit
left_motor_inverted
right_motor_inverted
```

Teste manual:

```text
Motor L PWM: -100% a +100%
Motor R PWM: -100% a +100%
Tempo máximo de teste: 3 segundos
Botão STOP sempre visível
```

### 13.6 Sucção

Parâmetros editáveis:

```text
fan_pwm_start
fan_pwm_run
fan_pwm_max
fan_start_delay_ms
fan_mode
```

Modos:

```text
OFF
PWM_DC_MOSFET
ESC_SERVO
```

### 13.7 Eventos e Máquina de Estados

Mostrar eventos detectados:

- CROSSING;
- CURVE_LEFT;
- RIGHT_MARKER;
- FINISH;
- LINE_LOST.

Regra especial:

```text
LAT_R só conta como início/parada se os sensores frontais NÃO indicarem cruzamento.
```

### 13.8 Perfis

Perfis sugeridos:

```text
SAFE
FAST
CURVES
TEST
CUSTOM_1
CUSTOM_2
```

Cada perfil salva:

- PID;
- thresholds;
- PWM motores;
- PWM sucção;
- configurações de cruzamento;
- configurações de linha perdida.

### 13.9 Logs

Mostrar:

- última corrida;
- duração;
- erro médio;
- erro máximo;
- número de cruzamentos;
- número de perdas de linha;
- PWM médio;
- bateria inicial/final.

Exportar como CSV.

### 13.10 Análise inteligente

Botão:

```text
Analisar última corrida
```

Saída esperada:

```text
Diagnóstico:
- Robô oscilou em reta.
- PWM saturou em curvas.
- Linha foi perdida 2 vezes.
- Bateria caiu abaixo do ideal.

Sugestões:
- Reduzir Kp em 10%.
- Aumentar Kd em 15%.
- Reduzir base_speed em 5%.
- Aumentar fan_pwm_run em 10%.
```

As sugestões devem ser opcionais.

---

## 14. Configurações salvas em NVS

Criar estrutura de configuração:

```cpp
typedef struct {
    float kp;
    float ki;
    float kd;
    float integral_limit;
    float derivative_filter;

    uint16_t sensor_threshold[8];
    uint16_t sensor_strong_threshold[8];
    uint16_t lat_l_threshold;
    uint16_t lat_r_threshold;

    uint16_t crossing_sum_threshold;
    uint8_t crossing_min_count;
    uint8_t crossing_min_span;
    uint16_t lost_line_sum_threshold;

    int16_t base_speed;
    int16_t pwm_min_left;
    int16_t pwm_min_right;
    int16_t pwm_max_left;
    int16_t pwm_max_right;
    int16_t accel_limit;
    int16_t brake_limit;

    bool left_motor_inverted;
    bool right_motor_inverted;

    uint16_t fan_pwm_start;
    uint16_t fan_pwm_run;
    uint16_t fan_pwm_max;
    uint16_t fan_start_delay_ms;
    uint8_t fan_mode;

    uint16_t finish_min_run_time_ms;
    uint16_t line_lost_timeout_ms;
    uint16_t crossing_debounce_ms;
    uint16_t lateral_debounce_ms;

    uint32_t config_version;
    uint32_t checksum;
} pegasus_config_t;
```

---

## 15. AutoCalibração dos sensores

O ESP32 deve solicitar ao STM32:

```text
CALIBRATION_START
```

O STM32 mede:

- mínimo;
- máximo;
- ruído;
- range;
- estabilidade.

Depois envia resultados ao ESP32.

O ESP32 calcula ou recebe:

- threshold individual;
- strong threshold individual;
- thresholds laterais;
- qualidade da calibração.

Regra de qualidade:

```text
range = max - min
```

Se:

```text
range < SENSOR_MIN_RANGE
```

Mostrar:

```text
Sensor S3 com contraste baixo. Verifique altura, alinhamento ou iluminação.
```

---

## 16. Inteligência de ajuste

O ESP32 pode sugerir ajustes, mas não aplicar automaticamente durante corrida oficial.

### Modo treino

Pode sugerir e aplicar com confirmação.

### Modo corrida

Parâmetros congelados.

Diagnósticos simples:

- oscilação em reta → reduzir Kp ou aumentar Kd;
- robô atrasado nas curvas → aumentar Kp, reduzir base_speed ou aumentar fan;
- perda de linha → reduzir velocidade, melhorar calibração ou aumentar sucção;
- motor desequilibrado → ajustar pwm_max_left/right ou compensação.

---

## 17. Estrutura de pastas esperada

```text
firmware/esp32/
├── main.cpp
├── app_config.h
├── board_pins.h
├── communication/
│   ├── uart_protocol.cpp
│   ├── uart_protocol.h
│   ├── packet_parser.cpp
│   └── packet_parser.h
├── web/
│   ├── web_server.cpp
│   ├── web_server.h
│   ├── api_routes.cpp
│   ├── api_routes.h
│   └── web_ui.h
├── telemetry/
│   ├── telemetry_model.cpp
│   ├── telemetry_model.h
│   ├── logger.cpp
│   └── logger.h
├── config/
│   ├── config_manager.cpp
│   ├── config_manager.h
│   ├── profiles.cpp
│   └── profiles.h
├── analysis/
│   ├── run_analyzer.cpp
│   └── run_analyzer.h
├── safety/
│   ├── stm32_watchdog.cpp
│   └── stm32_watchdog.h
└── ota/
    ├── ota_update.cpp
    └── ota_update.h
```

---

## 18. Requisitos mínimos da Rev A0

A primeira versão do firmware ESP32 deve implementar:

- suporte ao perfil `BOARD_ESP32_S3_ZERO`;
- suporte futuro ao perfil `BOARD_XIAO_ESP32_S3`;
- Wi-Fi Access Point;
- Web dashboard simples;
- UART com STM32;
- exibir estado do robô;
- exibir sensores;
- alterar PID;
- alterar PWM dos motores;
- alterar PWM da sucção;
- iniciar/parar calibração;
- armar;
- start;
- stop;
- salvar configuração em NVS.

---

## 19. Plano de testes na protoboard

### Teste 1 — UART básica

Objetivo:

```text
ESP32-S3 Zero envia PING.
STM32 responde PONG.
```

Critério de sucesso:

```text
ACK recebido por 1000 ciclos sem erro.
```

### Teste 2 — telemetria fake

Objetivo:

```text
STM32 envia sensores falsos A0-A7.
ESP32 mostra no dashboard.
```

### Teste 3 — comandos

Objetivo:

```text
Dashboard envia ARM, START, STOP e calibração.
STM32 responde ACK.
```

### Teste 4 — sensores reais

Objetivo:

```text
STM32 lê sensores analógicos reais.
ESP32 exibe gráfico de barras.
```

### Teste 5 — motores

Objetivo:

```text
ESP32 envia teste de motor.
STM32 aciona TB6612 com timeout.
```

Regra:

```text
Sempre testar com rodas suspensas primeiro.
```

### Teste 6 — sucção

Objetivo:

```text
ESP32 envia teste de fan/sucção.
STM32 aciona FAN_PWM com timeout.
```

---

## 20. Instruções para o Codex

Ao gerar código:

1. Não misturar controle crítico no ESP32.
2. Fixar tarefas UART no Core 0.
3. Fixar dashboard/análise no Core 1.
4. Usar filas FreeRTOS.
5. Criar código modular.
6. Criar structs claras.
7. Criar comentários explicando a separação STM32/ESP32.
8. Não aplicar sugestões automáticas durante corrida sem confirmação.
9. Priorizar Rev A0 funcional antes de recursos avançados.
10. Não depender de Wi-Fi para o robô correr.
11. Suportar primeiro `BOARD_ESP32_S3_ZERO`.
12. Deixar preparado para migrar para `BOARD_XIAO_ESP32_S3`.
13. Não inventar pinagem real sem confirmação.

---

## 21. Prompt curto para Codex

```text
Implemente o firmware ESP32-S3 do projeto PEGASUS conforme o documento HFS-ESP32-001 Rev A0.1.

A primeira fase usará ESP32-S3 Zero em protoboard para validar comunicação com STM32 Black Pill. Depois o firmware será migrado para Seeed XIAO ESP32-S3 na HDB-001 Brain Board.

Use FreeRTOS com duas divisões principais:
- Core 0: UART com STM32, parser de pacotes, command queue, watchdog e Wi-Fi manager.
- Core 1: web dashboard, logs, perfis, NVS e análise inteligente.

Não implemente PID nem controle direto de motores no ESP32. O ESP32 deve apenas enviar comandos e parâmetros para o STM32.

Crie a estrutura de pastas em firmware/esp32/ conforme especificado.

Implemente inicialmente:
- suporte ao perfil BOARD_ESP32_S3_ZERO;
- arquivo board_pins.h com TODOs claros para pinos;
- Access Point PEGASUS_SETUP;
- dashboard web simples;
- UART protocol frame SOF/TYPE/LEN/PAYLOAD/CRC8;
- exibição de sensores;
- alteração de PID;
- alteração de PWM dos motores;
- alteração de PWM da sucção;
- start/stop/calibração/arm;
- salvar configuração em NVS;
- watchdog de comunicação com STM32.

Deixe preparado para BOARD_XIAO_ESP32_S3, mas não assuma pinos reais sem confirmação.

Documente pendências para Rev A1 e não invente lógica crítica fora da especificação.
```
