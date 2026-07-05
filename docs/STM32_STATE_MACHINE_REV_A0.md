# PEGASUS Rev A0 - Maquina de Estados STM32

Documento: HFS-STM32-SM-001  
Revisao: A0.1  
Responsavel: Wesley Mateus Basso / Hander  
Escopo: firmware STM32F411 Black Pill, piloto em tempo real do robo PEGASUS.

## 1. Objetivo

Este documento especifica a maquina de estados principal do STM32 no PEGASUS Rev A0.

O STM32 e o controlador de tempo real. Ele deve:

- ler sensores;
- detectar linha, cruzamento e marcadores laterais;
- executar PD/PID;
- controlar TB6612;
- controlar succao;
- aplicar seguranca;
- enviar telemetria ao ESP32;
- aceitar comandos do ESP32.

O ESP32 nao executa PID, nao gera PWM real e nao decide seguranca critica.

## 2. Visao Geral

```text
BOOT
  |
  v
SELF_TEST
  |
  v
IDLE
  |\
  | \-- CALIBRATION --> READY
  |                    |
  |                    v
  +-----------------> ARMED
                         |
                         v
                    FAN_SPINUP
                         |
                         v
                      RUNNING
       +-----------------+-----------------+----------------+
       |                 |                 |                |
       v                 v                 v                v
   CROSSING         CURVE_LEFT        LINE_LOST       FINISH_DETECTED
       |                 |                 |                |
       v                 v                 v                v
    RUNNING          RUNNING       RUNNING/SAFE_STOP    SAFE_STOP

Qualquer estado -- STOP/falha recuperavel --> SAFE_STOP
Qualquer estado -- falha critica -----------> ERROR
SAFE_STOP -- DISARM/CLEAR/RESET ------------> IDLE
```

## 3. Prioridade de Decisao Durante Corrida

Dentro de `RUNNING`, a prioridade deve ser:

```text
1. STOP manual ou emergencia
2. falha critica
3. cruzamento frontal
4. linha perdida
5. LAT_R valido = fim/parada
6. LAT_L valido = curva esquerda
7. seguimento normal por PD/PID
```

Essa ordem evita que um cruzamento largo seja confundido com marcador lateral.

## 4. Regras Fundamentais

### 4.1 Cruzamento tem prioridade sobre laterais

Se os sensores frontais detectarem cruzamento:

```c
left_curve_valid = false;
right_marker_valid = false;
```

Logo:

```c
right_marker_valid = lat_r_active && !crossing_detected;
left_curve_valid   = lat_l_active && !crossing_detected;
```

### 4.2 STOP tem prioridade absoluta

`STOP_RUN`, botao fisico futuro, watchdog interno ou falha critica devem cortar a corrida.

```text
qualquer estado -> SAFE_STOP ou ERROR
```

### 4.3 O robo nao depende do ESP32 para correr

Depois de armado e iniciado, o STM32 deve conseguir manter a corrida usando a configuracao ja recebida.

Em modo bancada, pode existir:

```c
stop_on_esp_timeout = true;
```

Em modo competicao, recomendacao inicial:

```c
stop_on_esp_timeout = false;
```

## 5. Estados

## 5.1 BOOT

### Funcao

Ligar o sistema em condicao segura.

### O que faz

- inicializa clock;
- inicializa GPIO;
- inicializa ADC;
- inicializa DMA;
- inicializa timers;
- inicializa PWM;
- inicializa UART;
- inicializa watchdog;
- carrega configuracao;
- coloca motores em zero;
- coloca TB6612 em standby;
- desliga succao;
- zera variaveis de controle.

### Saidas

```text
BOOT -> SELF_TEST
```

Condicao:

```text
inicializacao basica terminou
```

### Falhas

```text
BOOT -> ERROR
```

Condicoes possiveis:

- clock falhou;
- ADC nao inicializou;
- timer PWM nao inicializou;
- configuracao corrompida sem fallback.

## 5.2 SELF_TEST

### Funcao

Conferir se o robo pode operar.

### O que faz

- verifica configuracao;
- verifica ADC;
- verifica timers;
- verifica UART;
- verifica watchdog;
- verifica se sensores tem leitura plausivel;
- verifica bateria, se `VBAT_ADC` existir;
- verifica se ha calibracao salva.

### Saidas

```text
SELF_TEST -> IDLE
```

Condicao:

```text
sistema basico OK
```

```text
SELF_TEST -> ERROR
```

Condicoes:

- configuracao invalida;
- ADC falhando;
- sensor obrigatorio morto;
- falha de hardware critica;
- bateria critica.

## 5.3 IDLE

### Funcao

Estado parado, seguro e aguardando comando.

### O que faz

- motores desligados;
- TB6612 desabilitado;
- succao desligada;
- PID resetado;
- aceita comandos do ESP32;
- permite iniciar calibracao;
- permite teste de hardware;
- envia telemetria basica.

### Saidas

```text
IDLE -> CALIBRATION
```

Condicao:

```text
ESP32 envia CALIBRATION_START
```

```text
IDLE -> READY
```

Condicao:

```text
calibracao valida ja existe
```

```text
IDLE -> HARDWARE_TEST
```

Condicao:

```text
ESP32 envia HARDWARE_TEST
```

```text
IDLE -> ERROR
```

Condicao:

```text
falha critica detectada
```

## 5.4 CALIBRATION

### Funcao

Medir fundo e linha para calibrar os sensores.

### O que faz

- coleta `min` e `max` dos sensores `A0..A7`;
- coleta `min` e `max` de `LAT_L`;
- coleta `min` e `max` de `LAT_R`;
- calcula range de cada sensor;
- detecta sensor com contraste baixo;
- calcula thresholds iniciais;
- mantem motores desligados;
- mantem succao desligada;
- envia progresso ao ESP32.

### Regra de qualidade

```c
range = sensor_max - sensor_min;
sensor_fault = range < SENSOR_MIN_RANGE;
```

### Saidas

```text
CALIBRATION -> READY
```

Condicoes:

- ESP32 envia `CALIBRATION_STOP`;
- ranges sao validos;
- sensores principais passaram.

```text
CALIBRATION -> IDLE
```

Condicao:

```text
calibracao cancelada
```

```text
CALIBRATION -> ERROR
```

Condicoes:

- sensor critico com range muito baixo;
- ADC falhou;
- timeout de calibracao.

## 5.5 READY

### Funcao

Robo calibrado, mas ainda nao armado.

### O que faz

- motores desligados;
- succao desligada;
- aceita ajuste de PID;
- aceita ajuste de PWM dos motores;
- aceita ajuste de succao;
- aceita nova calibracao;
- aceita comando `ARM`.

### Saidas

```text
READY -> ARMED
```

Condicao:

```text
ESP32 envia ARM
```

```text
READY -> CALIBRATION
```

Condicao:

```text
ESP32 envia CALIBRATION_START
```

```text
READY -> IDLE
```

Condicoes:

- reset;
- cancelamento;
- clear manual.

```text
READY -> ERROR
```

Condicao:

```text
falha critica
```

## 5.6 ARMED

### Funcao

Robo pronto para largar, mas ainda parado.

### O que faz

- motores continuam em zero;
- TB6612 pode ficar em standby;
- succao pode ficar desligada;
- monitora comando de start;
- monitora `LAT_R` como marcador de largada, se configurado.

### Saidas

```text
ARMED -> FAN_SPINUP
```

Condicao 1:

```text
ESP32 envia START_RUN
```

Condicao 2:

```c
lat_r_active && !crossing_detected
```

```text
ARMED -> READY
```

Condicao:

```text
ESP32 envia DISARM
```

```text
ARMED -> SAFE_STOP
```

Condicoes:

- `STOP_RUN`;
- emergencia;
- falha recuperavel.

```text
ARMED -> ERROR
```

Condicao:

```text
falha critica
```

## 5.7 FAN_SPINUP

### Funcao

Subir succao antes de acionar motores.

### O que faz

- liga succao em `fan_pwm_start`;
- espera `fan_start_delay_ms`;
- mantem motores em zero;
- mantem TB6612 sem tracionar.

### Saidas

```text
FAN_SPINUP -> RUNNING
```

Condicao:

```text
tempo fan_start_delay_ms terminou
```

```text
FAN_SPINUP -> SAFE_STOP
```

Condicoes:

- `STOP_RUN`;
- watchdog;
- falha;
- emergencia.

## 5.8 RUNNING

### Funcao

Estado principal de corrida.

### O que faz a cada ciclo de controle

- captura snapshot dos ADCs;
- filtra sensores;
- normaliza sensores para `0..1000`;
- calcula `front_sum`;
- calcula sensores ativos;
- calcula sensores fortemente ativos;
- calcula `first_active`;
- calcula `last_active`;
- calcula `span`;
- calcula posicao da linha;
- calcula erro;
- detecta cruzamento;
- detecta `LAT_L`;
- detecta `LAT_R`;
- detecta linha perdida;
- executa PD/PID;
- atualiza PWM dos motores;
- mantem succao em `fan_pwm_run`;
- envia telemetria.

### Saidas

```text
RUNNING -> CROSSING
```

Condicao:

```c
crossing_detected == true
```

```text
RUNNING -> LINE_LOST
```

Condicao:

```c
line_lost_confirmed == true
```

```text
RUNNING -> FINISH_DETECTED
```

Condicao:

```c
right_marker_valid && run_time_ms > finish_min_run_time_ms
```

Onde:

```c
right_marker_valid = lat_r_active && !crossing_detected;
```

```text
RUNNING -> CURVE_LEFT
```

Condicao:

```c
left_curve_valid == true
```

Onde:

```c
left_curve_valid = lat_l_active && !crossing_detected;
```

```text
RUNNING -> SAFE_STOP
```

Condicoes:

- `STOP_RUN`;
- emergencia;
- bateria critica;
- timeout de busca;
- falha recuperavel.

```text
RUNNING -> ERROR
```

Condicao:

```text
falha critica nao recuperavel
```

## 5.9 CROSSING

### Funcao

Atravessar cruzamento sem confundir marcadores laterais.

### O que faz

- ignora `LAT_L`;
- ignora `LAT_R`;
- evita contar largada/fim;
- evita contar curva;
- mantem trajetoria;
- usa erro ponderado se ainda houver informacao;
- se os sensores saturarem, pode usar erro zero ou ultimo erro valido;
- congela integral do PID.

### Regra principal

```c
if (crossing_detected) {
    left_curve_valid = false;
    right_marker_valid = false;
}
```

### Saidas

```text
CROSSING -> RUNNING
```

Condicao:

```c
crossing_detected == false
```

Depois de sair:

```text
ignorar laterais por lateral_lockout_after_crossing_ms
```

Valor inicial sugerido:

```text
50 ms a 150 ms
```

```text
CROSSING -> SAFE_STOP
```

Condicoes:

- `STOP_RUN`;
- emergencia;
- falha.

## 5.10 CURVE_LEFT

### Funcao

Tratar marcador lateral esquerdo de curva.

### Entrada valida

```c
lat_l_active && !crossing_detected
```

### O que faz

- reduz velocidade temporariamente;
- pode aumentar ganho proporcional;
- pode aumentar succao;
- continua seguindo linha;
- dura um tempo curto;
- nao pode ser ativado se houver cruzamento frontal.

### Saidas

```text
CURVE_LEFT -> RUNNING
```

Condicoes:

- tempo de curva acabou;
- linha estabilizou;
- marcador lateral saiu.

```text
CURVE_LEFT -> CROSSING
```

Condicao:

```c
crossing_detected == true
```

```text
CURVE_LEFT -> LINE_LOST
```

Condicao:

```c
line_lost_confirmed == true
```

```text
CURVE_LEFT -> SAFE_STOP
```

Condicoes:

- `STOP_RUN`;
- emergencia;
- falha.

## 5.11 LINE_LOST

### Funcao

Tentar recuperar a linha antes de parar.

### O que faz

- reduz velocidade;
- congela integral;
- usa `last_valid_error`;
- procura para o lado onde a linha foi vista;
- mantem succao conforme configuracao;
- aplica timeout de busca.

### Logica sugerida

```c
if (last_valid_error > 0) {
    procurar_para_um_lado();
} else {
    procurar_para_o_outro_lado();
}
```

### Saidas

```text
LINE_LOST -> RUNNING
```

Condicao:

```c
line_detected && !line_lost
```

```text
LINE_LOST -> SAFE_STOP
```

Condicoes:

- timeout de busca excedido;
- `STOP_RUN`;
- emergencia;
- falha.

## 5.12 FINISH_DETECTED

### Funcao

Finalizar corrida.

### Entrada valida

```c
right_marker_valid && run_time_ms > finish_min_run_time_ms
```

### O que faz

- registra tempo final;
- reduz motores;
- envia evento ao ESP32;
- desliga succao;
- prepara parada segura.

### Saida

```text
FINISH_DETECTED -> SAFE_STOP
```

Condicao:

```text
fim registrado
```

## 5.13 SAFE_STOP

### Funcao

Parar tudo em seguranca.

### O que faz

- PWM motores = `0`;
- TB6612 em brake ou coast;
- succao desligada;
- integral do PID zerada;
- derivativo resetado;
- corrida bloqueada ate clear/reset;
- envia estado ao ESP32.

### Saidas

```text
SAFE_STOP -> IDLE
```

Condicoes:

- ESP32 envia `DISARM`;
- clear/reset seguro;
- falha recuperavel foi limpa.

Opcao futura:

```text
SAFE_STOP -> READY
```

Condicao:

```text
manter calibracao valida depois da parada
```

Para Rev A0, recomendacao:

```text
SAFE_STOP -> IDLE
```

## 5.14 HARDWARE_TEST

### Funcao

Testar hardware isolado em bancada.

### O que faz

- testa motor esquerdo;
- testa motor direito;
- testa ambos os motores;
- testa succao;
- testa sensores;
- testa LEDs futuros;
- sempre com timeout;
- sempre aceita STOP imediato.

### Regras

```text
rodas suspensas nos primeiros testes de motor
succao testada isolada medindo corrente
STOP sempre visivel no dashboard
```

### Saidas

```text
HARDWARE_TEST -> IDLE
```

Condicao:

```text
teste terminou normalmente
```

```text
HARDWARE_TEST -> SAFE_STOP
```

Condicoes:

- timeout;
- `STOP_RUN`;
- falha.

## 5.15 ERROR

### Funcao

Estado de falha critica.

### O que faz

- motores desligados;
- succao desligada;
- TB6612 desabilitado;
- envia erro ao ESP32;
- bloqueia `START_RUN`;
- aguarda reset ou clear controlado.

### Entradas

Qualquer estado pode ir para `ERROR`.

Condicoes:

- configuracao invalida sem fallback;
- ADC falhando;
- sensor critico invalido;
- watchdog interno;
- bateria critica;
- falha de hardware nao recuperavel.

### Saida

```text
ERROR -> IDLE
```

Condicao:

```text
reset ou clear, apenas se a falha desapareceu
```

## 6. Resumo Das Setas

| Transicao | Condicao |
|---|---|
| `BOOT -> SELF_TEST` | Inicializacao terminou |
| `BOOT -> ERROR` | Falha critica na inicializacao |
| `SELF_TEST -> IDLE` | Sistema basico OK |
| `SELF_TEST -> ERROR` | Configuracao/hardware invalido |
| `IDLE -> CALIBRATION` | `CALIBRATION_START` |
| `IDLE -> READY` | Calibracao valida existente |
| `IDLE -> HARDWARE_TEST` | `HARDWARE_TEST` |
| `CALIBRATION -> READY` | Calibracao concluida e valida |
| `CALIBRATION -> IDLE` | Calibracao cancelada |
| `CALIBRATION -> ERROR` | Sensor/ADC falhou |
| `READY -> ARMED` | `ARM` |
| `READY -> CALIBRATION` | Recalibrar |
| `READY -> IDLE` | Reset/cancelamento |
| `ARMED -> FAN_SPINUP` | `START_RUN` ou `LAT_R && !crossing` |
| `ARMED -> READY` | `DISARM` |
| `ARMED -> SAFE_STOP` | Stop ou falha |
| `FAN_SPINUP -> RUNNING` | Delay da succao terminou |
| `FAN_SPINUP -> SAFE_STOP` | Stop/falha |
| `RUNNING -> CROSSING` | Cruzamento frontal |
| `CROSSING -> RUNNING` | Cruzamento acabou |
| `RUNNING -> CURVE_LEFT` | `LAT_L && !crossing` |
| `CURVE_LEFT -> RUNNING` | Tempo/evento de curva acabou |
| `CURVE_LEFT -> CROSSING` | Cruzamento apareceu |
| `RUNNING -> LINE_LOST` | Linha perdida confirmada |
| `LINE_LOST -> RUNNING` | Linha reencontrada |
| `LINE_LOST -> SAFE_STOP` | Timeout de busca |
| `RUNNING -> FINISH_DETECTED` | `LAT_R && !crossing && tempo minimo` |
| `FINISH_DETECTED -> SAFE_STOP` | Fim registrado |
| `SAFE_STOP -> IDLE` | Clear/reset/disarm |
| `qualquer estado -> SAFE_STOP` | Stop/emergencia/falha recuperavel |
| `qualquer estado -> ERROR` | Falha critica |

## 7. Variaveis Principais

### Sensores

```c
front_raw[8]
front_norm[8]
lat_l_raw
lat_l_norm
lat_r_raw
lat_r_norm
front_sum
active_count
strong_count
span
position
error
last_valid_error
```

### Eventos

```c
crossing_detected
line_lost
line_lost_confirmed
lat_l_active
lat_r_active
left_curve_valid
right_marker_valid
```

### Controle

```c
kp
ki
kd
integral_limit
derivative_filter
base_speed
pwm_min_left
pwm_min_right
pwm_max_left
pwm_max_right
fan_pwm_start
fan_pwm_run
fan_pwm_max
```

## 8. Pseudocodigo Do RUNNING

```c
if (stop_requested || emergency_fault) {
    state = ST_SAFE_STOP;
}
else if (critical_fault) {
    state = ST_ERROR;
}
else if (crossing_detected) {
    state = ST_CROSSING;
}
else if (line_lost_confirmed) {
    state = ST_LINE_LOST;
}
else if (right_marker_valid && run_time_ms > finish_min_run_time_ms) {
    state = ST_FINISH_DETECTED;
}
else if (left_curve_valid) {
    state = ST_CURVE_LEFT;
}
else {
    run_pid();
    update_motors();
}
```

## 9. Relacao Com ESP32

O ESP32 pode enviar:

- `SET_PID`;
- `SET_MOTOR_CONFIG`;
- `SET_FAN_CONFIG`;
- `CALIBRATION_START`;
- `CALIBRATION_STOP`;
- `ARM`;
- `DISARM`;
- `START_RUN`;
- `STOP_RUN`;
- `HARDWARE_TEST`;
- `PING`.

O STM32 responde:

- `ACK`;
- `NACK`;
- `PONG`;
- `TELEMETRY_BASIC`;
- `SENSOR_VALUES`;
- `ERROR_STATUS`.

Regra:

```text
ESP32 configura, monitora e registra.
STM32 decide e executa em tempo real.
```

## 10. Pendencias Rev A0

- Definir se `SAFE_STOP -> READY` sera permitido mantendo calibracao.
- Definir botao fisico de STOP, se existir.
- Definir timeout exato de `LINE_LOST`.
- Definir duracao e ganhos especiais de `CURVE_LEFT`.
- Definir se modo competicao ignora perda de ESP32 durante corrida.
- Validar sinais dos sensores reais na bancada.
- Validar sentido dos motores e sinal da correcao PID.
