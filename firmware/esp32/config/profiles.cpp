#include "profiles.h"

const char *profile_name(profile_id_t id) {
    switch (id) {
    case PROFILE_SAFE: return "SAFE";
    case PROFILE_FAST: return "FAST";
    case PROFILE_CURVES: return "CURVES";
    case PROFILE_TEST: return "TEST";
    case PROFILE_CUSTOM_1: return "CUSTOM_1";
    case PROFILE_CUSTOM_2: return "CUSTOM_2";
    }
    return "UNKNOWN";
}

pegasus_config_t profile_defaults(profile_id_t id) {
    pegasus_config_t c = default_config();
    if (id == PROFILE_FAST) {
        c.base_speed = 500;
        c.kp = 0.65f;
        c.kd = 0.14f;
    } else if (id == PROFILE_CURVES) {
        c.base_speed = 320;
        c.kp = 0.75f;
        c.kd = 0.18f;
        c.fan_pwm_run = 600;
    } else if (id == PROFILE_TEST) {
        c.base_speed = 200;
        c.fan_mode = FAN_OFF;
    }
    c.checksum = config_checksum(c);
    return c;
}
