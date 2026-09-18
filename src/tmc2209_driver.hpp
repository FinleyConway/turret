#pragma once

#include <cstdint>
#include <cassert>

#include "gpio.hpp"

void sleep(uint32_t us) {}

// https://circuitdigest.com/microcontroller-projects/interfacing-TMC2209-stepper-motor-driver-with-arduino-uno-basic-direction-and-stepping-control

struct tmc2209_driver_spec {
    uint32_t steps_per_rev = 0;
    uint8_t pulse_width_us = 0;

    gpio_pin enable_pin = gpio_pin::unconfigured;
    gpio_pin step_pin = gpio_pin::unconfigured;
    gpio_pin direction_pin = gpio_pin::unconfigured;
};

class tmc2209_driver {
public:
    enum class direction {
        clockwise,
        anti_clockwise
    };

public:
    explicit tmc2209_driver(const tmc2209_driver_spec& spec) 
        : c_spec(spec) 
    {
        // prevent invalid pinouts
        assert(spec.enable_pin    != gpio_pin::unconfigured && "Enable pin is not configured");
        assert(spec.step_pin      != gpio_pin::unconfigured && "Step pin is not configured");
        assert(spec.direction_pin != gpio_pin::unconfigured && "Direction pin is not configured");

        // prevent 0 step motors
        assert(spec.steps_per_rev > 0 && "Steps per revolution must be greater than 0");

        // set up driver pins
        gpio::set_mode(spec.enable_pin, gpio_mode::output);
        gpio::set_mode(spec.step_pin, gpio_mode::output);
        gpio::set_mode(spec.direction_pin, gpio_mode::output);
        
        // enable driver by default
        gpio::write(spec.enable_pin, false);
    }

    bool step(uint16_t angle) {
        // dont step as there is no provided speed
        if (m_step_rpm == 0) return false;
        if (!m_enabled) return false;

        const uint64_t steps = angle_to_step(angle);

        // set the direction 
        gpio::write(c_spec.direction_pin, m_direction == direction::clockwise); // may need to flip when testing

        // send duration pulses for the amount of steps needed to perform the given angle
        for (uint64_t i = 0; i < steps; i++) {
            gpio::write(c_spec.step_pin, true);
            sleep(c_spec.pulse_width_us);

            gpio::write(c_spec.step_pin, false);
            sleep(rpm_to_step_period_us(m_step_rpm) - c_spec.pulse_width_us);
        }

        return true;
    }

    void set_direction(direction direction) {
        m_direction = direction;
    }

    bool set_speed(uint64_t rpm) {
        // early exit if wanting to stop motor
        if (rpm == 0) {
            m_step_rpm = 0;
            
            return true;
        }

        // check if the speed is slower then the pulse width to prevent overflow
        // during step delay
        if (rpm_to_step_period_us(rpm) <= c_spec.pulse_width_us) {
            return false;
        }

        m_step_rpm = rpm;

        return true;
    }

    void enable(bool enable) {
        m_enabled = enable;

        gpio::write(c_spec.enable_pin, !enable);
    }

private:
    constexpr uint16_t angle_to_step(uint16_t angle) const {
        return (angle * c_spec.steps_per_rev) / 360u;
    }

    constexpr uint64_t rpm_to_step_period_us(uint64_t rpm) const {
        return 60'000'000ull / (rpm * c_spec.steps_per_rev);
    }

private:
    const tmc2209_driver_spec c_spec;
    uint32_t m_step_rpm = 0;
    direction m_direction = direction::clockwise;
    bool m_enabled = true;
};