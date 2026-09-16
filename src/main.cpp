#include <iostream>

#include "task.hpp"
#include "event_queue.hpp"

struct stepper_axis {
    uint16_t tilt_degree = 0;
    uint16_t pan_degree = 0;
};

struct motor_control {
    enum class type {
        manual,
        automatic
    };

    type command;
    stepper_axis axis;
};

class motor_task {
public:
    explicit motor_task(event_queue<motor_control>& motor_control) : m_motor_control(motor_control) {
    }

public:
    void run(std::stop_token token) {
        motor_control motor_control;

        while (m_motor_control.receive(motor_control, token)) {
            stepper_axis axis = motor_control.axis;

            std::cout << "Turning: [x: " << axis.tilt_degree << ", y: " << axis.pan_degree << "]\n";
        }

        std::cout << "task ended!\n";
    }

private:
    event_queue<motor_control>& m_motor_control;
};

int main() {
    event_queue<motor_control> motor;
    task<motor_task> motor_task(motor);

    motor.send(motor_control { 
        .command = motor_control::type::manual,
        .axis = {
            .tilt_degree = 60,
            .pan_degree = 360
        }
    });

    std::this_thread::sleep_for(std::chrono::seconds(1));
}