#pragma once

#include <WebServer.h>
#include "../telemetry/telemetry_model.h"
#include "../config/config_manager.h"
#include "../telemetry/logger.h"

void register_api_routes(WebServer &server, pegasus_telemetry_t &telemetry, pegasus_config_t &config, run_summary_t &summary);
