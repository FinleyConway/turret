#pragma once

#include <cstdint>

namespace turret {
    struct stepper_control {
        enum class type {
            manual,
            automatic
        };

        type command = type::manual;
        uint16_t angle = 0;
    };
}