#pragma once

#include <stop_token>

#include "config/stepper_config.hpp"
#include "drivers/tmc2209_driver.hpp"
#include "task/events/stepper_control.hpp"

namespace turret {
    class stepper_task {
    public:
        stepper_task(event_queue<stepper_control>& stepper_control, const stepper_config& config) : 
            m_config(config),
            m_stepper_control(stepper_control)
        {
        }
    
    public:
        void run(std::stop_token token) {
            // probably need a task for each stepper motor??
            tmc2209_driver pan_stepper(tmc2209_driver_spec {
                .steps_per_rev = m_config.steps_per_rev,
                .pulse_width = m_config.pulse_width,
                .step_pin = m_config.pan.step,
                .enable_pin = m_config.pan.enable,
                .direction_pin = m_config.pan.direction,
            });
            pan_stepper.set_speed(100);

            stepper_control control;

            while (m_stepper_control.receive(control, token)) {
                std::cout << "Rotating pan stepper!\n";

                pan_stepper.rotate(control.angle);
            }
        }

    private:
        const stepper_config& m_config;
        event_queue<stepper_control>& m_stepper_control;
    };
}