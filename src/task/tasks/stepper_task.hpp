#pragma once

#include <stop_token>

#include "config/stepper_config.hpp"
#include "drivers/tmc2209_driver.hpp"
#include "task/events/stepper_control.hpp"

namespace turret {
    class stepper_task {
    public:
        stepper_task(event_queue<stepper_control>& stepper_control, const stepper_config& stepper, const stepper_axis_config& axis) : 
            m_stepper_config(stepper),
            m_axis_config(axis),
            m_stepper_control(stepper_control)
        {
        }
    
    public:
        void run(std::stop_token token) {
            tmc2209_driver stepper = setup_driver();
            stepper_control control;

            while (m_stepper_control.receive(control, token)) {
                std::cout << "Rotating stepper!\n";
                
                // make add stop token inside this so i can stop the task all togethor when its stuck turning?
                stepper.rotate(control.angle);
            }
        }
    
    private:
        tmc2209_driver setup_driver() {
            tmc2209_driver stepper(tmc2209_driver_spec {
                .steps_per_rev = m_stepper_config.steps_per_rev,
                .pulse_width = m_stepper_config.pulse_width,
                .step_pin = m_axis_config.step,
                .enable_pin = m_axis_config.enable,
                .direction_pin = m_axis_config.direction,
            });

            stepper.set_speed(100);

            return stepper;
        }

    private:
        const stepper_config& m_stepper_config;
        const stepper_axis_config& m_axis_config;
        event_queue<stepper_control>& m_stepper_control;
    };
}