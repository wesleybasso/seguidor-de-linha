# HFS-STM32-001 — PEGASUS STM32F411 Control Firmware Specification

Projeto: **PEGASUS Line Follower Robot**  
Placa principal: **HDB-001 Brain Board**  
Placa inferior: **HDB-100 Chassis / Power Board**  
Placa de sensores: **HDB-101 Sensor Board**  
MCU de controle: **STM32F411CEU6 / Black Pill F411**  
Documento: **HFS-STM32-001**  
Revisão: **A0.1**  
Responsável: **Wesley Mateus Basso / Hander**

---

## 1. Função do STM32 no PEGASUS

O STM32F411 é o **piloto real do robô**.

Ele é responsável por tudo que precisa de tempo real, previsibilidade e segurança.

```text
STM32F411 = piloto em tempo real
ESP32-S3  = interface, telemetria, logs, configuração e análise
```

O STM32 deve continuar funcionando mesmo se o ESP32 travar, reiniciar ou perder Wi-Fi.

---

## 2. Observação sobre cores do STM32F411

O STM32F411 **não é dual-core**.  
Ele possui um único núcleo:

```text
ARM Cortex-M4F single-core com FPU
```

Portanto, para “usar o máximo” do STM32, o firmware deve explorar:

- timers de hardware;
- PWM por timer;
- ADC em scan mode;
- DMA para leitura dos sensores;
- interrupção periódica de controle;
- UART com interrupção ou DMA;
- watchdog;
- FPU para cálculo PID;
- código enxuto e determinístico.

O paralelismo vem dos **periféricos de hardware**, não de múltiplos cores.

---

## 3. Responsabilidades do STM32

O STM32 deve controlar:

- sensores frontais analógicos;
- sensores laterais analógicos;
- calibração dos sensores;
- normalização dos sensores;
- detecção de linha;
- detecção de cruzamento;
- detecção do sensor lateral esquerdo de curva;
- detecção do sensor lateral direito de início/parada;
- máquina de estados principal;
- controle PID/PD;
- PWM dos motores via TB6612FNG;
- PWM da sucção;
- leitura de bateria;
- segurança crítica;
- comunicação UART com ESP32;
- envio de telemetria;
- resposta a comandos;
- testes de hardware.

---

## 4. Sensores

### 4.1 Sensores frontais

O robô usa 8 sensores frontais analógicos:

```text
S0 S1 S2 S3 S4 S5 S6 S7
```

Funções:

- seguir a linha;
- calcular posição da linha;
- calcular erro do PID;
- detectar cruzamento;
- detectar linha perdida.

### 4.2 Sensor lateral esquerdo

```text
LAT_L
```

Função:

- detectar marca lateral de curva;
- acionar modo especial de curva esquerda.

### 4.3 Sensor lateral direito

```text
LAT_R
```

Função:

- detectar início;
- detectar parada/fim.

Regra especial:

```text
LAT_R só conta como início/parada se os sensores frontais NÃO indicarem cruzamento.
```

Se `LAT_R` estiver ativo, mas os sensores frontais também indicarem padrão largo/equivalente, o evento deve ser tratado como:

```text
CROSSING
```

e não como início/fim.

---

## 5. Fluxo de leitura dos sensores

A leitura deve seguir este pipeline:

```text
ADC cru
 ↓
filtro
 ↓
normalização pela calibração
 ↓
extração de características
 ↓
eventos lógicos
 ↓
máquina de estados
 ↓
PID / motores
```

---

## 6. Leitura ADC

### 6.1 Canais ADC mínimos

```text
S0 a S7   = 8 canais analógicos frontais
LAT_L     = 1 canal analógico lateral esquerdo
LAT_R     = 1 canal analógico lateral direito
VBAT_ADC  = opcional, se houver canal disponível
```

Total mínimo:

```text
10 canais analógicos
```

Total com bateria:

```text
11 canais analógicos
```

### 6.2 Estratégia recomendada

Usar:

```text
ADC1 em scan mode
DMA circular
buffer de amostras
```

O controle não deve ficar bloqueado esperando ADC.

### 6.3 Frequências sugeridas

```text
ADC scan: 2 kHz a 10 kHz agregado
Controle: 500 Hz a 1 kHz
Telemetria: 20 Hz a 50 Hz
PWM motores: 20 kHz a 25 kHz
PWM fan DC: 20 kHz a 25 kHz
PWM ESC/servo: 50 Hz, se usar ESC
```

---

## 7. Calibração dos sensores

Durante a calibração, o STM32 deve registrar para cada sensor:

```c
sensor_min[i]
sensor_max[i]
sensor_noise[i]
```

Depois calcular:

```c
sensor_range[i] = sensor_max[i] - sensor_min[i];
```

Um sensor com range baixo deve ser marcado como suspeito.

```c
if (sensor_range[i] < SENSOR_MIN_RANGE) {
    sensor_fault[i] = true;
}
```

---

## 8. Normalização

Após calibração, cada sensor deve ser normalizado para uma escala comum:

```text
0    = sem linha
1000 = linha forte
```

Exemplo:

```c
norm[i] = (raw[i] - min[i]) * 1000 / (max[i] - min[i]);
```

Com saturação:

```c
if (norm[i] < 0) norm[i] = 0;
if (norm[i] > 1000) norm[i] = 1000;
```

Se a linha for branca em fundo preto, aplicar inversão:

```c
norm[i] = 1000 - norm[i];
```

---

## 9. Filtro dos sensores

Usar filtro IIR leve:

```c
filtered[i] = (filtered[i] * 7 + raw[i] * 3) / 10;
```

Filtro muito forte deixa o robô lento.  
Filtro muito fraco deixa o robô nervoso.

Parâmetro configurável pelo ESP32:

```text
sensor_filter_alpha
```

---

## 10. Cálculo da posição da linha

Usar média ponderada dos sensores frontais.

Pesos sugeridos:

```text
S0    S1    S2    S3    S4    S5    S6    S7
-3500 -2500 -1500 -500  +500 +1500 +2500 +3500
```

Fórmula:

```c
position = sum(norm[i] * weight[i]) / sum(norm[i]);
```

Erro:

```c
error = target_position - position;
```

Normalmente:

```c
target_position = 0;
```

Interpretação:

```text
error < 0  = linha para um lado
error > 0  = linha para o outro lado
```

A direção exata deve ser conferida no teste dos motores.

---

## 11. Extração de características dos sensores frontais

A cada ciclo de controle, calcular:

```c
front_sum
active_count
strong_count
first_active
last_active
span
position
error
```

### 11.1 Sensor ativo

```c
active[i] = norm[i] > sensor_threshold[i];
```

### 11.2 Sensor fortemente ativo

```c
strong_active[i] = norm[i] > sensor_strong_threshold[i];
```

### 11.3 Soma frontal

```c
front_sum = norm[0] + norm[1] + ... + norm[7];
```

### 11.4 Largura do padrão

```c
span = last_active - first_active + 1;
```

---

## 12. Detecção de cruzamento

Cruzamento deve ser detectado pelos sensores frontais, não pelos laterais.

Critérios recomendados:

```c
crossing_detected =
    strong_count >= crossing_min_count &&
    span >= crossing_min_span &&
    front_sum >= crossing_sum_threshold;
```

Valores iniciais sugeridos:

```text
crossing_min_count = 5
crossing_min_span = 5
crossing_sum_threshold = 3500
```

Esses valores devem ser ajustáveis pelo ESP32.

### 12.1 Debounce de cruzamento

O cruzamento só deve ser confirmado após persistir por alguns milissegundos.

```text
crossing_debounce_ms = 20 a 50 ms
```

### 12.2 Regra de prioridade

Se cruzamento for detectado:

```text
ignorar LAT_L
ignorar LAT_R
```

---

## 13. Detecção dos sensores laterais

### 13.1 Lateral esquerdo

```c
lat_l_active = lat_l_norm > lat_l_threshold;
```

Evento válido:

```c
left_curve_valid = lat_l_active && !crossing_detected;
```

### 13.2 Lateral direito

```c
lat_r_active = lat_r_norm > lat_r_threshold;
```

Evento válido:

```c
right_marker_valid = lat_r_active && !crossing_detected;
```

Regra principal:

```text
Se houver detecção equivalente na frente, é cruzamento.
O sensor lateral direito não conta como início ou fim.
```

---

## 14. Detecção de linha perdida

Linha perdida:

```c
line_lost = front_sum < lost_line_sum_threshold;
```

Não parar instantaneamente. Usar temporizador:

```c
if (line_lost) {
    line_lost_timer += dt;
} else {
    line_lost_timer = 0;
}
```

Confirmar perda:

```c
line_lost_confirmed = line_lost_timer > line_lost_timeout_ms;
```

Valor inicial sugerido:

```text
line_lost_timeout_ms = 80 ms
```

---

## 15. Máquina de estados principal

Estados:

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

---

## 16. Diagrama geral

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

---

## 17. Descrição dos estados

### 17.1 ST_BOOT

Ações:

- inicializar clock;
- inicializar GPIO;
- inicializar ADC + DMA;
- inicializar timers;
- inicializar PWM;
- inicializar UART;
- inicializar watchdog;
- carregar configuração;
- motores desligados;
- sucção desligada.

Transição:

```text
ST_BOOT → ST_SELF_TEST
```

---

### 17.2 ST_SELF_TEST

Ações:

- verificar configuração válida;
- verificar ADC ativo;
- verificar comunicação básica;
- verificar calibração salva;
- verificar bateria se VBAT_ADC existir.

Transições:

```text
ST_SELF_TEST → ST_IDLE
ST_SELF_TEST → ST_ERROR se falha crítica
```

---

### 17.3 ST_IDLE

Robô parado.

Ações:

- motores em 0;
- STBY do TB6612 desligado;
- sucção desligada;
- aguardando comando.

Transições:

```text
ST_IDLE → ST_CALIBRATION se comando CALIBRATE
ST_IDLE → ST_READY se calibração válida
ST_IDLE → ST_HARDWARE_TEST se comando de teste
```

---

### 17.4 ST_CALIBRATION

Ações:

- coletar min/max dos sensores;
- calcular range;
- calcular thresholds;
- enviar progresso ao ESP32;
- motores desligados, exceto se houver modo manual específico.

Transições:

```text
ST_CALIBRATION → ST_READY se calibração concluída
ST_CALIBRATION → ST_IDLE se cancelada
ST_CALIBRATION → ST_ERROR se sensor crítico falhar
```

---

### 17.5 ST_READY

Robô calibrado, mas não armado.

Ações:

- motores desligados;
- sucção desligada;
- aceitar ajuste de parâmetros;
- aceitar ARM.

Transições:

```text
ST_READY → ST_ARMED se comando ARM
ST_READY → ST_CALIBRATION se recalibrar
ST_READY → ST_IDLE se reset/cancelar
```

---

### 17.6 ST_ARMED

Robô pronto para largar.

Ações:

- motores ainda em 0;
- TB6612 pode ficar em standby desligado;
- sucção pode ficar desligada ou em pré-armamento;
- aguardar START pelo ESP32 ou marcador lateral direito.

Transições:

```text
ST_ARMED → ST_FAN_SPINUP se START pelo ESP32
ST_ARMED → ST_FAN_SPINUP se LAT_R ativo && !crossing_detected
ST_ARMED → ST_READY se DISARM
ST_ARMED → ST_SAFE_STOP se STOP
```

---

### 17.7 ST_FAN_SPINUP

Estado opcional para subir a sucção antes da largada.

Ações:

- ligar sucção em `fan_pwm_start`;
- aguardar `fan_start_delay_ms`;
- manter motores em 0.

Transições:

```text
ST_FAN_SPINUP → ST_RUNNING após delay
ST_FAN_SPINUP → ST_SAFE_STOP se STOP
```

---

### 17.8 ST_RUNNING

Estado principal da corrida.

Ações:

- ler sensores normalizados;
- calcular posição da linha;
- detectar eventos;
- executar PID/PD;
- atualizar motores;
- manter sucção em `fan_pwm_run`;
- enviar telemetria.

Prioridade de eventos:

```text
1. STOP manual ou emergência
2. Falha crítica
3. Cruzamento frontal
4. Linha perdida
5. Sensor direito de fim/parada
6. Sensor esquerdo de curva
7. Seguimento normal por PID
```

Transições:

```text
ST_RUNNING → ST_SAFE_STOP se STOP/emergência
ST_RUNNING → ST_CROSSING se crossing_detected
ST_RUNNING → ST_LINE_LOST se line_lost_confirmed
ST_RUNNING → ST_FINISH_DETECTED se right_marker_valid && run_time > finish_min_run_time_ms
ST_RUNNING → ST_CURVE_LEFT se left_curve_valid
ST_RUNNING → ST_RUNNING caso normal
```

---

### 17.9 ST_CROSSING

Ações:

- ignorar sensores laterais;
- manter trajetória;
- usar último erro válido ou erro centralizado;
- evitar contar início/fim;
- evitar contar curva lateral.

Estratégias possíveis:

```text
se todos sensores muito ativos: usar erro = 0
se padrão ainda tem tendência lateral: usar posição ponderada
se padrão saturado: usar last_valid_error com decaimento
```

Transições:

```text
ST_CROSSING → ST_RUNNING quando crossing_detected desaparecer
ST_CROSSING → ST_SAFE_STOP se STOP
```

Após sair de cruzamento, aplicar lockout dos laterais:

```text
lateral_lockout_after_crossing_ms = 50 a 150 ms
```

---

### 17.10 ST_CURVE_LEFT

Ativado pelo sensor lateral esquerdo quando não há cruzamento.

Ações:

- reduzir velocidade base temporariamente;
- opcionalmente aumentar ganho proporcional;
- opcionalmente aumentar sucção;
- manter por tempo curto.

Parâmetros:

```text
curve_mode_duration_ms
curve_speed_factor
curve_kp_factor
curve_fan_boost
```

Transições:

```text
ST_CURVE_LEFT → ST_RUNNING após timer ou estabilização
ST_CURVE_LEFT → ST_CROSSING se cruzamento aparecer
ST_CURVE_LEFT → ST_LINE_LOST se linha perdida
ST_CURVE_LEFT → ST_SAFE_STOP se STOP
```

---

### 17.11 ST_LINE_LOST

Ações:

- reduzir velocidade;
- congelar integral;
- buscar linha usando último erro conhecido;
- virar para o lado onde a linha foi vista por último.

Exemplo:

```text
se last_valid_error > 0: procurar para um lado
se last_valid_error < 0: procurar para o outro lado
```

Transições:

```text
ST_LINE_LOST → ST_RUNNING se linha reencontrada
ST_LINE_LOST → ST_SAFE_STOP se timeout excedido
```

Parâmetros:

```text
line_lost_timeout_ms
line_search_timeout_ms
line_search_pwm
```

---

### 17.12 ST_FINISH_DETECTED

Ativado pelo sensor lateral direito válido durante RUNNING.

Condições:

```c
right_marker_valid &&
run_time_ms > finish_min_run_time_ms
```

Ações:

- registrar tempo final;
- reduzir motores;
- desligar sucção com atraso opcional;
- enviar evento ao ESP32.

Transição:

```text
ST_FINISH_DETECTED → ST_SAFE_STOP
```

---

### 17.13 ST_SAFE_STOP

Estado seguro.

Ações:

- PWM motores = 0;
- TB6612 em brake ou coast conforme configuração;
- sucção desligada;
- travar comandos de corrida até reset/clear;
- enviar erro/evento ao ESP32.

Transições:

```text
ST_SAFE_STOP → ST_IDLE se comando RESET/CLEAR
```

---

### 17.14 ST_HARDWARE_TEST

Estado para teste de bancada.

Ações possíveis:

- testar motor esquerdo;
- testar motor direito;
- testar ambos;
- testar sucção;
- testar sensores;
- testar LEDs.

Regras:

```text
todo teste deve ter timeout
todo teste deve aceitar STOP imediato
rodas devem estar suspensas nos primeiros testes
```

Transições:

```text
ST_HARDWARE_TEST → ST_IDLE ao finalizar
ST_HARDWARE_TEST → ST_SAFE_STOP se STOP
```

---

### 17.15 ST_ERROR

Falha crítica.

Ações:

- motores desligados;
- sucção desligada;
- enviar erro ao ESP32;
- aguardar reset.

---

## 18. Controle PID/PD

O STM32 deve implementar PID, mas na prática começar como PD:

```text
Ki = 0 inicialmente
```

Isso reduz risco de integral acumulada atrapalhar curvas rápidas.

### 18.1 Fórmula

```c
error = target_position - position;

derivative = error - last_error;

integral += error * dt;
integral = clamp(integral, -integral_limit, +integral_limit);

correction = kp * error + ki * integral + kd * derivative;
```

### 18.2 Filtro derivativo

```c
derivative_filtered =
    derivative_filtered * derivative_filter +
    derivative * (1.0f - derivative_filter);
```

### 18.3 Anti-windup

A integral deve ser congelada ou zerada em:

- cruzamento;
- linha perdida;
- motor saturado;
- safe stop;
- hardware test;
- finish.

Recomendação inicial:

```text
Ki = 0
```

Ativar integral somente depois do robô seguir linha de forma estável.

---

## 19. Cálculo dos motores

Base:

```c
motor_left  = base_speed - correction;
motor_right = base_speed + correction;
```

A depender da montagem, talvez precise inverter o sinal da correção.

Aplicar:

- limite mínimo;
- limite máximo;
- compensação esquerda/direita;
- inversão de motor;
- rampa de aceleração;
- rampa de frenagem.

### 19.1 Limite para LiPo 2S com motores 6 V

Com LiPo 2S:

```text
tensão cheia = 8.4 V
```

Para motor 6 V, usar limite inicial:

```text
pwm_max = 70% a 75%
```

Porque:

```text
8.4 V × 0.75 ≈ 6.3 V
```

### 19.2 PWM mínimo útil

Motores N20 podem não girar abaixo de certo PWM.

Parâmetros:

```text
pwm_min_left
pwm_min_right
```

Se comando for diferente de zero e menor que o mínimo:

```c
if (pwm > 0 && pwm < pwm_min) pwm = pwm_min;
```

### 19.3 Rampa

Evita tranco e derrapagem.

```c
pwm_output = slew_limit(pwm_target, pwm_previous, accel_limit, brake_limit);
```

---

## 20. Controle do TB6612FNG

Sinais:

```text
PWMA
AIN1
AIN2
PWMB
BIN1
BIN2
STBY
```

Estados:

### Frente

```text
IN1 = 1
IN2 = 0
PWM = duty
```

### Ré

```text
IN1 = 0
IN2 = 1
PWM = duty
```

### Coast

```text
IN1 = 0
IN2 = 0
PWM = 0
```

### Brake

```text
IN1 = 1
IN2 = 1
PWM = 0 ou duty conforme estratégia
```

Recomendação:

```text
finish normal: desaceleração controlada
emergência: brake ou PWM 0 com STBY baixo
```

---

## 21. Controle da sucção

A sucção pode ter dois modos:

```text
PWM_DC_MOSFET
ESC_SERVO
```

### 21.1 Motor DC com MOSFET

Usar PWM alto:

```text
20 kHz a 25 kHz
```

Parâmetros:

```text
fan_pwm_start
fan_pwm_run
fan_pwm_max
fan_start_delay_ms
```

### 21.2 ESC/brushless

Usar PWM tipo servo:

```text
50 Hz
1000 us a 2000 us
```

O modo deve ser configurável.

---

## 22. Comunicação UART com ESP32

O STM32 deve aceitar comandos e enviar telemetria.

### 22.1 Configuração

```text
Baudrate inicial: 115200
Formato: 8N1
```

Futuro:

```text
921600
```

### 22.2 Frame

```text
SOF | TYPE | LEN | PAYLOAD | CRC8
```

```text
SOF = 0xA5
```

### 22.3 Comandos recebidos

O STM32 deve aceitar:

- SET_PID;
- SET_MOTOR_CONFIG;
- SET_FAN_CONFIG;
- CALIBRATION_START;
- CALIBRATION_STOP;
- ARM;
- DISARM;
- START_RUN;
- STOP_RUN;
- HARDWARE_TEST;
- RESET_ERROR;
- SAVE_CONFIG opcional;
- LOAD_PROFILE opcional.

### 22.4 Telemetria enviada

Enviar periodicamente:

```text
robot_state
battery_voltage
front_raw[8]
front_norm[8]
lat_l_raw
lat_l_norm
lat_r_raw
lat_r_norm
line_position
pid_error
pid_correction
pwm_left
pwm_right
fan_pwm
crossing_detected
left_curve_valid
right_marker_valid
line_lost
run_time_ms
error_flags
```

---

## 23. Frequência de execução

### 23.1 Loop de controle

```text
500 Hz inicialmente
1 kHz quando estável
```

Executa:

- snapshot dos sensores;
- normalização;
- detecção de eventos;
- máquina de estados;
- PID;
- atualização PWM.

### 23.2 Telemetria

```text
20 Hz a 50 Hz
```

Não enviar sensores em alta frequência demais no começo.

### 23.3 Watchdog

```text
IWDG ativo
```

O STM32 deve resetar se travar.

---

## 24. Estrutura de arquivos esperada

```text
firmware/stm32/
├── Core/
│   ├── Inc/
│   │   ├── main.h
│   │   ├── board_pins.h
│   │   ├── robot_state.h
│   │   ├── sensor_processing.h
│   │   ├── calibration.h
│   │   ├── line_detector.h
│   │   ├── pid_controller.h
│   │   ├── motor_control.h
│   │   ├── fan_control.h
│   │   ├── uart_protocol.h
│   │   ├── telemetry.h
│   │   ├── safety.h
│   │   └── config.h
│   └── Src/
│       ├── main.c
│       ├── robot_state.c
│       ├── sensor_processing.c
│       ├── calibration.c
│       ├── line_detector.c
│       ├── pid_controller.c
│       ├── motor_control.c
│       ├── fan_control.c
│       ├── uart_protocol.c
│       ├── telemetry.c
│       ├── safety.c
│       └── config.c
└── README_STM32.md
```

---

## 25. Estruturas principais

### 25.1 Estado dos sensores

```c
typedef struct {
    uint16_t raw[8];
    uint16_t filtered[8];
    uint16_t min[8];
    uint16_t max[8];
    uint16_t norm[8];

    uint16_t lat_l_raw;
    uint16_t lat_l_filtered;
    uint16_t lat_l_min;
    uint16_t lat_l_max;
    uint16_t lat_l_norm;

    uint16_t lat_r_raw;
    uint16_t lat_r_filtered;
    uint16_t lat_r_min;
    uint16_t lat_r_max;
    uint16_t lat_r_norm;

    uint8_t active_count;
    uint8_t strong_count;
    uint8_t first_active;
    uint8_t last_active;
    uint8_t span;

    uint32_t front_sum;
    int16_t position;
    int16_t error;

    bool line_detected;
    bool line_lost;
    bool crossing_detected;
    bool lat_l_active;
    bool lat_r_active;
    bool left_curve_valid;
    bool right_marker_valid;
} sensor_state_t;
```

### 25.2 Configuração do robô

```c
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
    uint16_t lateral_lockout_after_crossing_ms;

    uint32_t config_version;
    uint32_t checksum;
} robot_config_t;
```

---

## 26. Prioridade de segurança

O STM32 deve sempre priorizar:

```text
1. STOP físico, se existir
2. STOP recebido do ESP32
3. watchdog interno
4. falha crítica de sensor/ADC
5. bateria crítica
6. perda de linha prolongada
7. comandos normais
```

---

## 27. Comportamento se ESP32 falhar

O STM32 não deve depender do ESP32 para continuar o loop de controle.

Modos possíveis:

### Modo competição

```text
Se ESP32 perder comunicação durante RUNNING:
continuar corrida usando configuração já recebida.
```

### Modo bancada

```text
Se ESP32 perder comunicação:
parar por segurança.
```

Parâmetro:

```text
stop_on_esp_timeout
```

---

## 28. Pinagem lógica da HDB-001

### 28.1 Conector frontal JF

Recebe sinais dos sensores via HDB-100/HDB-101:

```text
3V3
GND
A0
A1
A2
A3
A4
A5
A6
A7
LAT_L
LAT_R
```

### 28.2 Conector traseiro JR

Envia comandos para potência/motores na HDB-100:

```text
5V
GND
PWMA
AIN1
AIN2
PWMB
BIN1
BIN2
STBY
FAN_PWM
VBAT_ADC
GPIO_RESERVA
```

---

## 29. Testes obrigatórios

### Teste 1 — sensores fake

- STM32 gera valores simulados.
- ESP32 mostra no dashboard.
- Máquina de estados não aciona motores.

### Teste 2 — UART

- ESP32 envia PING.
- STM32 responde ACK/PONG.

### Teste 3 — ADC real

- ler sensores;
- exibir raw/norm no ESP32;
- calibrar min/max.

### Teste 4 — TB6612 com rodas suspensas

- motor esquerdo frente/ré;
- motor direito frente/ré;
- STOP imediato.

### Teste 5 — PID sem robô no chão

- simular posição de linha;
- verificar PWM calculado.

### Teste 6 — linha real em baixa velocidade

- base_speed baixo;
- fan desligado;
- Ki = 0.

### Teste 7 — sucção

- testar fan isolado;
- medir corrente;
- verificar queda de tensão.

---

## 30. Requisitos mínimos da Rev A0

Implementar primeiro:

- ADC + DMA dos sensores;
- calibração min/max;
- normalização 0–1000;
- detecção de cruzamento;
- regra LAT_R inválido quando cruzamento frontal existir;
- máquina de estados;
- PD inicial;
- PWM motores via TB6612;
- fan PWM simples;
- UART com ESP32;
- telemetria básica;
- comando START/STOP/ARM/CALIBRATION;
- hardware test com timeout;
- safe stop.

---

## 31. Prompt curto para Codex

```text
Implemente o firmware STM32F411 do projeto PEGASUS conforme o documento HFS-STM32-001 Rev A0.1.

O STM32F411 é single-core Cortex-M4F; use o máximo dos periféricos:
- ADC em scan mode com DMA circular para sensores analógicos;
- timers para PWM dos motores TB6612 e fan;
- timer periódico de controle em 500 Hz inicialmente, futuro 1 kHz;
- UART com ESP32 usando protocolo SOF/TYPE/LEN/PAYLOAD/CRC8;
- watchdog interno.

O STM32 deve ser o piloto em tempo real:
- ler sensores frontais S0-S7 e laterais LAT_L/LAT_R;
- calibrar min/max;
- normalizar 0-1000;
- calcular posição da linha por média ponderada;
- detectar cruzamento por strong_count, span e front_sum;
- aplicar a regra: LAT_R só conta como início/parada se NÃO houver cruzamento frontal;
- usar LAT_L como marcador de curva quando NÃO houver cruzamento;
- executar a máquina de estados:
  BOOT, SELF_TEST, IDLE, CALIBRATION, READY, ARMED, FAN_SPINUP, RUNNING, CROSSING, CURVE_LEFT, LINE_LOST, FINISH_DETECTED, SAFE_STOP, HARDWARE_TEST, ERROR;
- executar controle PD/PID;
- gerar PWM real dos motores;
- gerar PWM da sucção;
- enviar telemetria ao ESP32;
- aceitar comandos do ESP32.

Não colocar lógica crítica no ESP32. O ESP32 apenas configura, monitora e envia comandos.
Comece pela Rev A0 funcional, com Ki=0 inicialmente, motores limitados para LiPo 2S com N20 6V, hardware test com timeout e STOP sempre prioritário.
```
