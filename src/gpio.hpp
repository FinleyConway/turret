#pragma once

#include <cassert>

#if HAS_WIRINGPI
#include <wiringPi.h>
#endif

// https://github.com/WiringPi/WiringPi/blob/master/documentation/english/functions.md

enum class gpio_pin {
    unconfigured = 0,
};

enum class gpio_mode {
    input,
    output
};

class gpio {
public:
    static void setup() {
#if HAS_WIRINGPI
        assert(wiringPiSetupPinType(WPI_PIN_PHYS) == 0);
#endif

        s_setup = true;
    }

    static void set_mode(gpio_pin pin, gpio_mode mode) {
        assert(s_setup && "gpio set was not called");
        assert(pin != gpio_pin::unconfigured && "Setting a unconfigured pin");

#if HAS_WIRINGPI
        pinMode(
            static_cast<int>(pin),
            mode == gpio_mode::output ? OUTPUT : INPUT
        );
#endif
    }

    static void write(gpio_pin pin, bool level) {
        assert(s_setup && "gpio set was not called");
        assert(pin != gpio_pin::unconfigured && "Writing a unconfigured pin");

#if HAS_WIRINGPI
        digitalWrite(
            static_cast<int>(pin),
            level ? HIGH : LOW
        );
#endif
    }

    static bool read(gpio_pin pin) {
        assert(s_setup && "gpio set was not called");
        assert(pin != gpio_pin::unconfigured && "Reading a unconfigured pin");

#if HAS_WIRINGPI
        return digitalRead(static_cast<int>(pin)) == HIGH;
#else
        return false;
#endif
    }

private:
    static inline bool s_setup = false;
};