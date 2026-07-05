#pragma once

#include "../telemetry/telemetry_model.h"
#include "../config/config_manager.h"
#include "../telemetry/logger.h"

void web_server_begin(pegasus_telemetry_t &telemetry, pegasus_config_t &config, run_summary_t &summary);
void web_server_loop();
