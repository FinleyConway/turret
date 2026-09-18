#pragma once

#include <chrono>
#include <thread>
#include <cstdint>
#include <cassert>

#include "gpio.hpp"

// https://circuitdigest.com/microcontroller-projects/interfacing-TMC2209-stepper-motor-driver-with-arduino-uno-basic-direction-and-stepping-control

struct tmc2209_driver_spec {
    uint32_t steps_per_rev = 0;
    std::chrono::microseconds pulse_width;

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

        const auto steps = angle_to_step(angle);
        const auto step_period = rpm_to_step_period(m_step_rpm);
        auto next_step = std::chrono::steady_clock::now();

        // set the direction 
        gpio::write(c_spec.direction_pin, m_direction == direction::clockwise); // may need to flip when testing

        // send duration pulses for the amount of steps needed to perform the given angle
        for (uint64_t i = 0; i < steps; i++) {
            next_step += step_period;

            // move to the next step
            gpio::write(c_spec.step_pin, true);
            std::this_thread::sleep_for(c_spec.pulse_width);
            gpio::write(c_spec.step_pin, false);

            std::this_thread::sleep_until(next_step);
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
        if (rpm_to_step_period(rpm) <= c_spec.pulse_width) {
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
    constexpr uint64_t angle_to_step(uint16_t angle) const {
        return (static_cast<uint64_t>(angle) * c_spec.steps_per_rev) / 360u;
    }

    constexpr std::chrono::microseconds rpm_to_step_period(uint64_t rpm) const {
        return std::chrono::microseconds{
            60'000'000ull / (rpm * c_spec.steps_per_rev)
        };
    }

private:
    const tmc2209_driver_spec c_spec;
    uint32_t m_step_rpm = 0;
    direction m_direction = direction::clockwise;
    bool m_enabled = true;
};