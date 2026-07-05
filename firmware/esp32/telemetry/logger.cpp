#include "logger.h"

static int16_t abs16(int16_t value) {
    return value < 0 ? static_cast<int16_t>(-value) : value;
}

void logger_reset(run_summary_t &summary) {
    summary = run_summary_t{};
}

void logger_add_sample(run_summary_t &summary, const pegasus_telemetry_t &telemetry) {
    summary.samples++;
    const int16_t abs_error = abs16(telemetry.pid_error);
    summary.error_sum_abs += abs_error;
    if (abs_error > summary.error_max_abs) {
        summary.error_max_abs = abs_error;
    }
    if (telemetry.line_lost) {
        summary.line_lost_count++;
    }
    if (abs16(telemetry.pwm_left) > 720 || abs16(telemetry.pwm_right) > 720) {
        summary.pwm_saturation_count++;
    }
}

String logger_summary_json(const run_summary_t &summary) {
    const int32_t avg = summary.samples ? summary.error_sum_abs / static_cast<int32_t>(summary.samples) : 0;
    String s = "{";
    s += "\"samples\":" + String(summary.samples) + ",";
    s += "\"line_lost_count\":" + String(summary.line_lost_count) + ",";
    s += "\"error_avg_abs\":" + String(avg) + ",";
    s += "\"error_max_abs\":" + String(summary.error_max_abs) + ",";
    s += "\"pwm_saturation_count\":" + String(summary.pwm_saturation_count);
    s += "}";
    return s;
}
