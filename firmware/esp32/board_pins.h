#pragma once

// Select only one board at build time.
// PlatformIO environments define BOARD_ESP32_S3_ZERO or BOARD_XIAO_ESP32_S3.

#if defined(BOARD_ESP32_S3_ZERO)

#define BOARD_NAME "ESP32-S3 Zero Dev Prototype"
#define STM32_UART_NUM 1

// Waveshare ESP32-S3-Zero: board silkscreen TX = GPIO43, RX = GPIO44.
// Wire ESP32 TX/GPIO43 -> STM32 PA10/RX and ESP32 RX/GPIO44 <- STM32 PA9/TX.
#define STM32_UART_TX_PIN 43
#define STM32_UART_RX_PIN 44

// Onboard WS2812 RGB LED is connected to GPIO21. It is not used by Rev A0 yet.
#define STATUS_LED_PIN 21
#define BOOT_BUTTON_PIN 0

#elif defined(BOARD_XIAO_ESP32_S3)

#define BOARD_NAME "Seeed XIAO ESP32-S3 Brain Board"
#define STM32_UART_NUM 1

// TODO(Wesley): confirm if the module is XIAO ESP32-S3 or XIAO ESP32-S3 Plus.
// Do not assume real pins before HDB-001 pinout review.
#define STM32_UART_TX_PIN -1
#define STM32_UART_RX_PIN -1

#define STATUS_LED_PIN -1
#define BOOT_BUTTON_PIN -1

#else
#error "Select BOARD_ESP32_S3_ZERO or BOARD_XIAO_ESP32_S3"
#endif
