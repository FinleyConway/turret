#pragma once

#if HAS_WIRINGPI
#include <wiringPi.h>
#endif

enum class gpio_pin {
    unconfigured = 0,
    motor_enable = 17, // test values
    motor_step = 18,
    motor_direction = 27,
};

enum class gpio_mode {
    input,
    output
};

class gpio {
public:
    static void setup() {
#if HAS_WIRINGPI
        wiringPiSetupGpio();
#endif
    }

    static void set_mode(gpio_pin pin, gpio_mode mode) {
#if HAS_WIRINGPI
        pinMode(
            static_cast<int>(pin),
            mode == gpio_mode::output ? OUTPUT : INPUT
        );
#endif
    }

    static void write(gpio_pin pin, bool level) {
#if HAS_WIRINGPI
        digitalWrite(
            static_cast<int>(pin),
            level ? HIGH : LOW
        );
#endif
    }

    static bool read(gpio_pin pin) {
#if HAS_WIRINGPI
        return digitalRead(static_cast<int>(pin)) == HIGH;
#else
        return false;
#endif
    }
};