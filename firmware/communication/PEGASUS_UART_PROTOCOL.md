# PEGASUS UART Protocol

Documento: HCP-001  
Revisao: A0.1

## Configuracao Inicial

- Baudrate: `115200`
- Futuro: `921600` somente depois da comunicacao estar estavel
- Formato: `8N1`
- Flow control: nenhum

## Frame

```text
SOF | TYPE | LEN | PAYLOAD | CRC8
```

| Campo | Bytes | Descricao |
|---|---:|---|
| SOF | 1 | Sempre `0xA5` |
| TYPE | 1 | Tipo da mensagem |
| LEN | 1 | Tamanho do payload |
| PAYLOAD | N | Dados |
| CRC8 | 1 | CRC-8 sobre `TYPE`, `LEN` e `PAYLOAD`, polinomio `0x07`, init `0x00` |

## Tipos

| Codigo | Nome | Direcao | Funcao |
|---:|---|---|---|
| `0x01` | `TELEMETRY_BASIC` | STM32 -> ESP32 | Estado geral, PWM, flags e tempo |
| `0x02` | `SENSOR_VALUES` | STM32 -> ESP32 | Sensores raw/norm |
| `0x03` | `SET_PID` | ESP32 -> STM32 | Alterar Kp/Ki/Kd |
| `0x04` | `SET_MOTOR_CONFIG` | ESP32 -> STM32 | PWM base/min/max |
| `0x05` | `SET_FAN_CONFIG` | ESP32 -> STM32 | Configuracao da succao |
| `0x06` | `CALIBRATION_START` | ESP32 -> STM32 | Iniciar calibracao |
| `0x07` | `CALIBRATION_STOP` | ESP32 -> STM32 | Finalizar calibracao |
| `0x08` | `START_RUN` | ESP32 -> STM32 | Iniciar corrida |
| `0x09` | `STOP_RUN` | ESP32 -> STM32 | Parar robo |
| `0x0A` | `ARM` | ESP32 -> STM32 | Armar robo |
| `0x0B` | `DISARM` | ESP32 -> STM32 | Desarmar/limpar parada |
| `0x0C` | `HARDWARE_TEST` | ESP32 -> STM32 | Teste de bancada com timeout |
| `0x0D` | `ERROR_STATUS` | STM32 -> ESP32 | Erros |
| `0x0E` | `ACK` | Bidirecional | Confirmacao |
| `0x0F` | `NACK` | Bidirecional | Falha |
| `0x10` | `PING` | ESP32 -> STM32 | Teste de link |
| `0x11` | `PONG` | STM32 -> ESP32 | Resposta ao PING |

## Payloads Rev A0

### `TELEMETRY_BASIC` - 24 bytes

```text
u8  robot_state
u16 battery_mv
i16 line_position
i16 pid_error
i16 pid_correction
i16 pwm_left
i16 pwm_right
u16 fan_pwm
u8  flags
u32 run_time_ms
u32 error_flags
```

Flags:

- bit 0: linha detectada
- bit 1: cruzamento detectado
- bit 2: LAT_L ativo
- bit 3: LAT_R ativo
- bit 4: linha perdida confirmada

### `SENSOR_VALUES` - 40 bytes

```text
u16 front_raw[8]
u16 front_norm[8]
u16 lat_l_raw
u16 lat_l_norm
u16 lat_r_raw
u16 lat_r_norm
```

### `SET_PID` - 12 bytes

```text
float kp
float ki
float kd
```

### `SET_MOTOR_CONFIG` - 10 bytes

```text
i16 base_speed
i16 pwm_min_left
i16 pwm_min_right
i16 pwm_max_left
i16 pwm_max_right
```

### `SET_FAN_CONFIG` - 9 bytes

```text
u16 fan_pwm_start
u16 fan_pwm_run
u16 fan_pwm_max
u16 fan_start_delay_ms
u8  fan_mode
```

### `ACK`, `NACK` e `PONG` - 2 bytes

```text
u8 about_type
u8 reason_code
```

Uso:

- `ACK`: `about_type` e o comando aceito; `reason_code = 0`.
- `NACK`: `about_type` e o comando recusado; `reason_code = 0` na Rev A0,
  reservado para codigos futuros.
- `PONG`: `about_type = PING`; `reason_code = 0`.

Observacao: comandos simples (`ARM`, `DISARM`, `START_RUN`, `STOP_RUN`,
`CALIBRATION_START`, `CALIBRATION_STOP`, `HARDWARE_TEST`, `PING`) usam payload
vazio no sentido ESP32 -> STM32. Payload extra deve gerar `NACK`.

## Regras

- O ESP32 nunca gera PWM real de motores.
- O STM32 deve responder `ACK` ou `NACK` para comandos recebidos.
- `STOP_RUN` tem prioridade sobre qualquer outro comando.
- `PING/PONG` e telemetria fake devem ser testados antes de motor real.
