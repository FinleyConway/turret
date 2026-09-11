#pragma once

#include <cstdint>
#include <cassert>

void set_pin(int, bool) {}

void sleep(uint32_t us) {}

// https://circuitdigest.com/microcontroller-projects/interfacing-TMC2209-stepper-motor-driver-with-arduino-uno-basic-direction-and-stepping-control

struct stepper_driver_spec {
    uint32_t steps_per_rev = 0;
    uint8_t pulse_width_us = 0;

    uint8_t enable_pin = 0;
    uint8_t direction_pin = 0;
    uint8_t step_pin = 0;
};

class stepper_driver {
public:
    enum class direction {
        none,
        clockwise,
        anti_clockwise
    };

public:
    constexpr explicit stepper_driver(const stepper_driver_spec& spec) 
        : c_spec(spec) 
    {
        // prevent invalid pinouts
        assert(spec.enable_pin > 0 && "Enable pin is invalid");
        assert(spec.step_pin > 0 && "Step pin is invalid");
        assert(spec.direction_pin > 0 && "Direction pin is invalid");

        // prevent 0 step motors
        assert(spec.steps_per_rev > 0 && "Steps per revolution must be greater than 0");

        // enable driver by default
        set_pin(c_spec.enable_pin, false);
    }

    bool step(uint16_t angle) {
        // dont step as there is no provided speed
        if (m_step_rpm == 0) return false;

        const uint64_t steps = angle_to_step(angle);

        set_pin(c_spec.direction_pin, get_direction_level());

        for (uint64_t i = 0; i < steps; i++) {
            set_pin(c_spec.step_pin, true);
            sleep(c_spec.pulse_width_us);

            set_pin(c_spec.step_pin, false);
            sleep(rpm_to_step_period_us(m_step_rpm) - c_spec.pulse_width_us);
        }

        return true;
    }

    void set_direction(direction direction) {
        m_direction = direction;
    }

    bool set_speed(uint64_t rpm) {
        if (rpm == 0) {
            m_step_rpm = 0;
            return true;
        }

        if (rpm_to_step_period_us(rpm) <= c_spec.pulse_width_us) {
            return false;
        }

        m_step_rpm = rpm;
        return true;
    }

    void enable(bool enable) {
        set_pin(c_spec.enable_pin, enable);
    }

private:
    constexpr uint16_t angle_to_step(uint16_t angle) const {
        return (angle * c_spec.steps_per_rev) / 360u;
    }

    constexpr bool get_direction_level() const {
        return m_direction == direction::clockwise;
    }

    constexpr uint64_t rpm_to_step_period_us(uint64_t rpm) const {
        return 60'000'000ull / (rpm * c_spec.steps_per_rev);
    }

private:
    const stepper_driver_spec c_spec;
    direction m_direction = direction::none;
    uint32_t m_step_rpm = 0;
};