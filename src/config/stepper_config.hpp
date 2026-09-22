#pragma once

#include <chrono>
#include <cstdint>

#include "core/gpio.hpp"

namespace turret {
    struct stepper_axis_config {
        gpio::pin direction = gpio::pin::unconfigured;
        gpio::pin step = gpio::pin::unconfigured;
        gpio::pin enable = gpio::pin::unconfigured;
    };

    struct stepper_config {
        uint32_t steps_per_rev = 0;
        std::chrono::microseconds pulse_width;
    };
}