#include "config_manager.h"
#include <Preferences.h>

static constexpr uint32_t kConfigVersion = 0xA0000001;
static constexpr char kNvsNamespace[] = "pegasus";
static constexpr char kConfigKey[] = "cfg";

pegasus_config_t default_config() {
    pegasus_config_t c{};
    c.kp = 0.55f;
    c.ki = 0.0f;
    c.kd = 0.10f;
    c.integral_limit = 500.0f;
    c.derivative_filter = 0.65f;
    for (uint8_t i = 0; i < 8; ++i) {
        c.sensor_threshold[i] = 350;
        c.sensor_strong_threshold[i] = 700;
    }
    c.lat_l_threshold = 500;
    c.lat_r_threshold = 500;
    c.crossing_sum_threshold = 3500;
    c.crossing_min_count = 5;
    c.crossing_min_span = 5;
    c.lost_line_sum_threshold = 250;
    c.base_speed = 350;
    c.pwm_min_left = 120;
    c.pwm_min_right = 120;
    c.pwm_max_left = 750;
    c.pwm_max_right = 750;
    c.accel_limit = 25;
    c.brake_limit = 60;
    c.left_motor_inverted = false;
    c.right_motor_inverted = false;
    c.fan_pwm_start = 300;
    c.fan_pwm_run = 500;
    c.fan_pwm_max = 800;
    c.fan_start_delay_ms = 400;
    c.fan_mode = FAN_OFF;
    c.finish_min_run_time_ms = 3000;
    c.line_lost_timeout_ms = 80;
    c.crossing_debounce_ms = 30;
    c.lateral_debounce_ms = 50;
    c.config_version = kConfigVersion;
    c.checksum = config_checksum(c);
    return c;
}

uint32_t config_checksum(const pegasus_config_t &config) {
    pegasus_config_t copy = config;
    copy.checksum = 0;
    const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&copy);
    uint32_t sum = 2166136261u;
    for (size_t i = 0; i < sizeof(copy); ++i) {
        sum ^= bytes[i];
        sum *= 16777619u;
    }
    return sum;
}

bool config_is_valid(const pegasus_config_t &config) {
    return config.config_version == kConfigVersion && config.checksum == config_checksum(config);
}

bool load_config(pegasus_config_t &config) {
    Preferences prefs;
    if (!prefs.begin(kNvsNamespace, true)) {
        config = default_config();
        return false;
    }
    const size_t read = prefs.getBytes(kConfigKey, &config, sizeof(config));
    prefs.end();
    if (read != sizeof(config) || !config_is_valid(config)) {
        config = default_config();
        return false;
    }
    return true;
}

bool save_config(pegasus_config_t &config) {
    config.config_version = kConfigVersion;
    config.checksum = config_checksum(config);
    Preferences prefs;
    if (!prefs.begin(kNvsNamespace, false)) {
        return false;
    }
    const size_t written = prefs.putBytes(kConfigKey, &config, sizeof(config));
    prefs.end();
    return written == sizeof(config);
}

String config_to_json(const pegasus_config_t &c) {
    String s = "{";
    s += "\"kp\":" + String(c.kp, 4) + ",";
    s += "\"ki\":" + String(c.ki, 4) + ",";
    s += "\"kd\":" + String(c.kd, 4) + ",";
    s += "\"base_speed\":" + String(c.base_speed) + ",";
    s += "\"pwm_max_left\":" + String(c.pwm_max_left) + ",";
    s += "\"pwm_max_right\":" + String(c.pwm_max_right) + ",";
    s += "\"fan_pwm_run\":" + String(c.fan_pwm_run) + ",";
    s += "\"fan_mode\":" + String(c.fan_mode);
    s += "}";
    return s;
}
