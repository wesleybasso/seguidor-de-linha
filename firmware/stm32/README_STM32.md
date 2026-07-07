# PEGASUS STM32F411 Rev A0.1

Firmware do piloto em tempo real do PEGASUS.

## O Que Implementa

- Leitura logica de sensores A0-A7, LAT_L e LAT_R.
- Calibracao min/max.
- Normalizacao 0-1000.
- Extracao de linha, cruzamento, laterais e linha perdida.
- Regra: `LAT_R` so conta quando nao houver cruzamento frontal.
- Maquina de estados Rev A0.
- PD/PID com `Ki = 0` no default.
- Controle de motores TB6612 via camada `pegasus_hal`.
- Controle de succao via camada `pegasus_hal`.
- UART com ESP32 usando `SOF/TYPE/LEN/PAYLOAD/CRC8`.
- Telemetria basica e sensores.

## Build Black Pill Real

No Windows:

```powershell
cd firmware/stm32
powershell -ExecutionPolicy Bypass -File .\build.ps1
```

Saidas geradas em `firmware/stm32/build/`:

- `firmware.elf`
- `firmware.bin`
- `firmware.map`

O build usa `PEGASUS_TARGET_F411`, CMSIS, startup `startup_stm32f411xe.s`,
linker `STM32F411CEUX_FLASH.ld` e a HAL register-level
`Core/Src/pegasus_hal_f411.c`.

## Integracao Com STM32Cube

Os arquivos em `Core/Src` e `Core/Inc` foram escritos para serem levados para um projeto STM32CubeIDE.

Substitua as funcoes fracas em `Core/Src/pegasus_hal.c` por implementacoes HAL reais:

- ADC1 scan + DMA circular para sensores.
- TIM PWM para `PWMA`, `PWMB` e `FAN_PWM`.
- GPIO para `AIN1`, `AIN2`, `BIN1`, `BIN2`, `STBY`.
- USART para PA9/PA10.
- IWDG para watchdog interno.

Nao teste motores no chao antes dos testes de hardware com rodas suspensas.
