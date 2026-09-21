#pragma once

#include <chrono>
#include <cstdint>

#include <nlohmann/json.hpp>

#include "core/gpio.hpp"

namespace turret {
    struct stepper_config {
        uint32_t steps_per_rev = 0;
        std::chrono::microseconds pulse_width;

        struct axis_config {
            gpio::pin direction;
            gpio::pin step;
            gpio::pin enable;
        } pan, tilt;
    };

    void from_json(const nlohmann::json& j, stepper_config& s) {
        j.at("steps_per_rev").get_to(s.steps_per_rev);

        uint64_t pulse_width_us;

        j.at("pulse_width_us").get_to(pulse_width_us);
        s.pulse_width = std::chrono::microseconds{pulse_width_us};


        const auto& pan = j.at("pan_axis");

        pan.at("direction_pin").get_to(s.pan.direction);
        pan.at("step_pin").get_to(s.pan.step);
        pan.at("enable_pin").get_to(s.pan.enable);

        const auto& tilt = j.at("tilt_axis");

        tilt.at("direction_pin").get_to(s.tilt.direction);
        tilt.at("step_pin").get_to(s.tilt.step);
        tilt.at("enable_pin").get_to(s.tilt.enable);
    }
}