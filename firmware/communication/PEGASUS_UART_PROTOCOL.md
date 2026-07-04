# PEGASUS UART Protocol

## Configuração inicial

- Baudrate: 115200.
- Futuro: 921600 se necessário.
- Formato: 8N1.

## Frame

| Campo | Bytes | Descrição |
|-------|------:|-----------|
| SOF | 1 | 0xA5 |
| TYPE | 1 | tipo |
| LEN | 1 | tamanho |
| PAYLOAD | N | dados |
| CRC8 | 1 | verificação |

## Tipos

| Tipo | Direção | Função |
|------|---------|--------|
| 0x01 | STM → ESP | telemetria |
| 0x02 | ESP → STM | set PID |
| 0x03 | ESP → STM | start/stop |
| 0x04 | STM → ESP | sensores |
| 0x05 | STM → ESP | erro/estado |
| 0x06 | ESP → STM | calibração |
| 0x07 | STM → ESP | ACK |
