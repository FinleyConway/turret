#pragma once

#include <chrono>
#include <cstdint>

#include <nlohmann/json.hpp>

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

    inline void from_json(const nlohmann::json& j, stepper_axis_config& s) {
        j.at("direction_pin").get_to(s.direction);
        j.at("step_pin").get_to(s.step);
        j.at("enable_pin").get_to(s.enable);
    }

    inline void from_json(const nlohmann::json& j, stepper_config& s) {
        j.at("steps_per_rev").get_to(s.steps_per_rev);

        uint64_t pulse_width_us;

        j.at("pulse_width_us").get_to(pulse_width_us);
        s.pulse_width = std::chrono::microseconds{pulse_width_us};
    }
}