#include "api_routes.h"
#include "../app_config.h"
#include "../analysis/run_analyzer.h"

static void queue_simple_command(pegasus_command_kind_t kind) {
    pegasus_command_t cmd{};
    cmd.kind = kind;
    xQueueSend(command_queue, &cmd, 0);
}

void register_api_routes(WebServer &server, pegasus_telemetry_t &telemetry, pegasus_config_t &config, run_summary_t &summary) {
    server.on("/api/status", HTTP_GET, [&]() {
        server.send(200, "application/json", telemetry_to_json(telemetry));
    });

    server.on("/api/config", HTTP_GET, [&]() {
        server.send(200, "application/json", config_to_json(config));
    });

    server.on("/api/save_config", HTTP_GET, [&]() {
        const bool ok = save_config(config);
        server.send(ok ? 200 : 500, "text/plain", ok ? "saved" : "save failed");
    });

    server.on("/api/summary", HTTP_GET, [&]() {
        server.send(200, "application/json", logger_summary_json(summary));
    });

    server.on("/api/analyze", HTTP_GET, [&]() {
        server.send(200, "text/plain", analyze_run(summary));
    });

    server.on("/api/command", HTTP_GET, [&]() {
        const String c = server.arg("cmd");
        if (c == "arm") queue_simple_command(CMD_ARM);
        else if (c == "disarm") queue_simple_command(CMD_DISARM);
        else if (c == "start") queue_simple_command(CMD_START_RUN);
        else if (c == "stop") queue_simple_command(CMD_STOP_RUN);
        else if (c == "cal_start") queue_simple_command(CMD_CALIBRATION_START);
        else if (c == "cal_stop") queue_simple_command(CMD_CALIBRATION_STOP);
        else {
            server.send(400, "text/plain", "unknown command");
            return;
        }
        server.send(200, "text/plain", "queued");
    });

    server.on("/api/set_pid", HTTP_GET, [&]() {
        if (server.hasArg("kp")) config.kp = server.arg("kp").toFloat();
        if (server.hasArg("ki")) config.ki = server.arg("ki").toFloat();
        if (server.hasArg("kd")) config.kd = server.arg("kd").toFloat();

        pegasus_command_t cmd{};
        cmd.kind = CMD_SET_PID;
        memcpy(&cmd.payload[0], &config.kp, sizeof(float));
        memcpy(&cmd.payload[4], &config.ki, sizeof(float));
        memcpy(&cmd.payload[8], &config.kd, sizeof(float));
        cmd.len = 12;
        xQueueSend(command_queue, &cmd, 0);
        server.send(200, "text/plain", "pid queued");
    });
}
