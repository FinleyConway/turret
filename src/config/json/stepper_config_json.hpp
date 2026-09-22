#pragma once

#include <cstdint>

#include <nlohmann/json.hpp>

#include "config/stepper_config.hpp"
#include "config/json/gpio_json.hpp"

namespace turret {
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