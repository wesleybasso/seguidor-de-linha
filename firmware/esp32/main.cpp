#include <Arduino.h>
#include <WiFi.h>
#include "app_config.h"
#include "board_pins.h"
#include "communication/packet_parser.h"
#include "communication/uart_protocol.h"
#include "config/config_manager.h"
#include "safety/stm32_watchdog.h"
#include "telemetry/logger.h"
#include "telemetry/telemetry_model.h"
#include "web/web_server.h"

QueueHandle_t telemetry_queue = nullptr;
QueueHandle_t command_queue = nullptr;
QueueHandle_t event_queue = nullptr;
QueueHandle_t log_queue = nullptr;

static HardwareSerial Stm32Serial(STM32_UART_NUM);
static pegasus_telemetry_t telemetry;
static pegasus_config_t config;
static run_summary_t run_summary;
static bool uart_enabled = false;

static uint8_t command_type(pegasus_command_kind_t kind) {
    using namespace pegasus;
    switch (kind) {
    case CMD_SET_PID: return MSG_SET_PID;
    case CMD_SET_MOTOR_CONFIG: return MSG_SET_MOTOR_CONFIG;
    case CMD_SET_FAN_CONFIG: return MSG_SET_FAN_CONFIG;
    case CMD_CALIBRATION_START: return MSG_CALIBRATION_START;
    case CMD_CALIBRATION_STOP: return MSG_CALIBRATION_STOP;
    case CMD_ARM: return MSG_ARM;
    case CMD_DISARM: return MSG_DISARM;
    case CMD_START_RUN: return MSG_START_RUN;
    case CMD_STOP_RUN: return MSG_STOP_RUN;
    case CMD_HARDWARE_TEST: return MSG_HARDWARE_TEST;
    case CMD_PING: return MSG_PING;
    }
    return MSG_NACK;
}

static void uart_rx_task(void *) {
    pegasus::PacketParser parser;
    pegasus::Packet packet;
    for (;;) {
        if (!uart_enabled) {
            vTaskDelay(pdMS_TO_TICKS(100));
            continue;
        }
        while (Stm32Serial.available() > 0) {
            if (parser.push(static_cast<uint8_t>(Stm32Serial.read()), packet)) {
                telemetry_apply_packet(packet.type, packet.payload, packet.len, telemetry);
                stm32_watchdog.mark_packet(millis());
                xQueueSend(telemetry_queue, &telemetry, 0);
                if (packet.type == pegasus::MSG_TELEMETRY_BASIC) {
                    logger_add_sample(run_summary, telemetry);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(2));
    }
}

static void uart_tx_task(void *) {
    pegasus_command_t cmd{};
    for (;;) {
        if (xQueueReceive(command_queue, &cmd, portMAX_DELAY) == pdTRUE) {
            if (uart_enabled) {
                pegasus::write_packet(Stm32Serial, command_type(cmd.kind), cmd.payload, cmd.len);
            }
        }
    }
}

static void stm32_watchdog_task(void *) {
    for (;;) {
        stm32_watchdog.apply(telemetry, millis());
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

static void wifi_manager_task(void *) {
    WiFi.mode(WIFI_AP);
    WiFi.softAP(PEGASUS_AP_SSID, PEGASUS_AP_PASS);
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void web_server_task(void *) {
    web_server_begin(telemetry, config, run_summary);
    for (;;) {
        web_server_loop();
        vTaskDelay(pdMS_TO_TICKS(5));
    }
}

static void logger_task(void *) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

static void analyzer_task(void *) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

static void profile_manager_task(void *) {
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
}

void setup() {
    Serial.begin(115200);
    delay(200);
    Serial.printf("PEGASUS ESP32 Rev A0 on %s\n", BOARD_NAME);

    telemetry_queue = xQueueCreate(PEGASUS_TELEMETRY_QUEUE_LEN, sizeof(pegasus_telemetry_t));
    command_queue = xQueueCreate(PEGASUS_COMMAND_QUEUE_LEN, sizeof(pegasus_command_t));
    event_queue = xQueueCreate(PEGASUS_EVENT_QUEUE_LEN, sizeof(uint32_t));
    log_queue = xQueueCreate(PEGASUS_LOG_QUEUE_LEN, sizeof(pegasus_telemetry_t));

    load_config(config);
    logger_reset(run_summary);

#if STM32_UART_TX_PIN >= 0 && STM32_UART_RX_PIN >= 0
    Stm32Serial.begin(PEGASUS_UART_BAUD, SERIAL_8N1, STM32_UART_RX_PIN, STM32_UART_TX_PIN);
    uart_enabled = true;
#else
    Serial.println("STM32 UART pins are TODO; running dashboard in no-UART mode.");
    uart_enabled = false;
#endif

    xTaskCreatePinnedToCore(uart_rx_task, "uart_rx", 4096, nullptr, 4, nullptr, 0);
    xTaskCreatePinnedToCore(uart_tx_task, "uart_tx", 4096, nullptr, 4, nullptr, 0);
    xTaskCreatePinnedToCore(stm32_watchdog_task, "stm32_wd", 2048, nullptr, 2, nullptr, 0);
    xTaskCreatePinnedToCore(wifi_manager_task, "wifi", 3072, nullptr, 2, nullptr, 0);

    xTaskCreatePinnedToCore(web_server_task, "web", 8192, nullptr, 2, nullptr, 1);
    xTaskCreatePinnedToCore(logger_task, "logger", 3072, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(analyzer_task, "analyzer", 3072, nullptr, 1, nullptr, 1);
    xTaskCreatePinnedToCore(profile_manager_task, "profiles", 3072, nullptr, 1, nullptr, 1);
}

void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
