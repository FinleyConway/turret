#include <iostream>

void set_pin(int, bool) {}

void sleep(uint32_t us) {}

struct stepper_motor_spec {
    uint8_t step_pin = 0;
    uint8_t direction_pin = 0;
    uint32_t steps_per_rev = 0;
    uint8_t pulse_width_us = 0; /* check data sheet for required pwm for step*/
};

class stepper_motor {
public:
    enum class direction {
        none,
        clockwise,
        anti_clockwise
    };

public:
    constexpr explicit stepper_motor(const stepper_motor_spec& spec) 
        : c_spec(spec) {}

    void step(uint16_t angle) {
        const uint32_t steps = angle_to_step(angle);
        const uint32_t step_period_us = rpm_to_step_period_us(m_step_rpm);

        if (c_spec.pulse_width_us >= step_period_us) {
            return;
        }

        set_pin(c_spec.direction_pin, get_direction_level());

        for (uint32_t i = 0; i < steps; i++) {
            set_pin(c_spec.step_pin, true);
            sleep(c_spec.pulse_width_us); // see driver spec

            set_pin(c_spec.step_pin, false);
            sleep(step_period_us - c_spec.pulse_width_us);
        }
    }

    void set_direction(direction direction) {
        m_direction = direction;
    }

    bool set_speed(uint32_t rpm) { 
        if (rpm == 0) {
            m_step_rpm = rpm;
            
            return true;
        }

        if (c_spec.pulse_width_us >= rpm_to_step_period_us(rpm)) {
            return false;
        }

        m_step_rpm = rpm;

        return true;
    }

private:
    constexpr uint32_t angle_to_step(uint16_t angle) const {
        return (angle * c_spec.steps_per_rev) / 360u;
    }

    constexpr bool get_direction_level() const {
        return m_direction == direction::clockwise;
    }

    constexpr uint64_t rpm_to_step_period_us(uint32_t rpm) const {
        return 60'000'000ull / (static_cast<uint64_t>(rpm) * c_spec.steps_per_rev);
    }

private:
    const stepper_motor_spec c_spec;
    direction m_direction = direction::none;
    uint32_t m_step_rpm = 0;
};

int main() {
}