#pragma once

#include "telemetry_model.h"

struct run_summary_t {
    uint32_t samples = 0;
    uint32_t line_lost_count = 0;
    int32_t error_sum_abs = 0;
    int16_t error_max_abs = 0;
    uint32_t pwm_saturation_count = 0;
};

void logger_reset(run_summary_t &summary);
void logger_add_sample(run_summary_t &summary, const pegasus_telemetry_t &telemetry);
String logger_summary_json(const run_summary_t &summary);
