#pragma once

#include "config_manager.h"

enum profile_id_t : uint8_t {
    PROFILE_SAFE = 0,
    PROFILE_FAST,
    PROFILE_CURVES,
    PROFILE_TEST,
    PROFILE_CUSTOM_1,
    PROFILE_CUSTOM_2
};

const char *profile_name(profile_id_t id);
pegasus_config_t profile_defaults(profile_id_t id);
