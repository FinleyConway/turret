#pragma once

#include <chrono>
#include <cstdint>

#include "core/gpio.hpp"

// https://circuitdigest.com/microcontroller-projects/interfacing-TMC2209-stepper-motor-driver-with-arduino-uno-basic-direction-and-stepping-control

namespace turret {
    struct tmc2209_driver_spec {
        const uint8_t microsteps = 8; // will want to adjust this via uart too, keep const for now
        uint32_t steps_per_rev = 0;
        std::chrono::microseconds pulse_width;

        gpio::pin step_pin      = gpio::pin::unconfigured;
        gpio::pin enable_pin    = gpio::pin::unconfigured;
        gpio::pin direction_pin = gpio::pin::unconfigured;
    };

    class tmc2209_driver {
    public:
        enum class direction {
            clockwise,
            anti_clockwise
        };

    public:
        explicit tmc2209_driver(const tmc2209_driver_spec& spec);

        bool rotate(uint16_t angle);

        void set_direction(direction direction);

        bool set_speed(uint64_t rpm);

        void enable(bool enable);

    private:
        constexpr uint64_t angle_to_step(uint16_t angle) const;

        constexpr std::chrono::microseconds rpm_to_step_period(uint64_t rpm) const;

    private:
        const tmc2209_driver_spec c_spec;
        uint32_t m_step_rpm = 0;
        direction m_direction = direction::clockwise;
        bool m_enabled = true;
    };
}