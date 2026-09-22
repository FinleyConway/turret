#include "core/gpio.hpp"

#include <cassert>
#include <iostream>

#if HAS_WIRINGPI
#include <wiringPi.h>
#endif

namespace turret {
    void gpio::setup() {
#if HAS_WIRINGPI
        assert(wiringPiSetupPinType(WPI_PIN_BCM) == 0);
#endif

        s_setup = true;
    }

    void gpio::set_mode(gpio::pin pin, gpio::mode mode) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != gpio::pin::unconfigured && "Setting a unconfigured pin");

#if HAS_WIRINGPI
        pinMode(
            static_cast<int>(pin),
            mode == gpio::mode::output ? OUTPUT : INPUT
        );
#endif
    }

    void gpio::write(gpio::pin pin, bool level) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != gpio::pin::unconfigured && "Writing a unconfigured pin");

#if HAS_WIRINGPI
        digitalWrite(
            static_cast<int>(pin),
            level ? HIGH : LOW
        );
#endif
    }

    bool gpio::read(gpio::pin pin) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != gpio::pin::unconfigured && "Reading a unconfigured pin");

#if HAS_WIRINGPI
        return digitalRead(static_cast<int>(pin)) == HIGH;
#else
        return false;
#endif
    }

    gpio::pin gpio::to_pin(int value) {
        if (value == -1) {
            return gpio::pin::unconfigured;
        }

        if (value < 0 || value > 27) {
            std::cerr << "Converting a pin value to an unconfigured pin! Pin: " << value << std::endl;

            return gpio::pin::unconfigured;
        }

        return static_cast<gpio::pin>(value);
    }
}