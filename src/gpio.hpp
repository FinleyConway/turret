#pragma once

#include <cassert>

#if HAS_WIRINGPI
#include <wiringPi.h>
#endif

// https://github.com/WiringPi/WiringPi/blob/master/documentation/english/functions.md

class gpio {
public:
    // BCM GPIO numbering.
    // https://pinout.xyz/pinout/gpio
    enum class pin : int {
        unconfigured = -1,

        pin_0 = 0,
        pin_1,
        pin_2,
        pin_3,
        pin_4,
        pin_5,
        pin_6,
        pin_7,
        pin_8,
        pin_9,
        pin_10,
        pin_11,
        pin_12,
        pin_13,
        pin_14,
        pin_15,
        pin_16,
        pin_17,
        pin_18,
        pin_19,
        pin_20,
        pin_21,
        pin_22,
        pin_23,
        pin_24,
        pin_25,
        pin_26,
        pin_27,
    };

    enum class mode {
        input,
        output
    };

public:
    static void setup() {
#if HAS_WIRINGPI
        assert(wiringPiSetupPinType(WPI_PIN_BCM) == 0);
#endif

        s_setup = true;
    }

    static void set_mode(pin pin, mode mode) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != pin::unconfigured && "Setting a unconfigured pin");

#if HAS_WIRINGPI
        pinMode(
            static_cast<int>(pin),
            mode == gpio_mode::output ? OUTPUT : INPUT
        );
#endif
    }

    static void write(pin pin, bool level) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != pin::unconfigured && "Writing a unconfigured pin");

#if HAS_WIRINGPI
        digitalWrite(
            static_cast<int>(pin),
            level ? HIGH : LOW
        );
#endif
    }

    static bool read(pin pin) {
        assert(s_setup && "gpio setup was not called");
        assert(pin != pin::unconfigured && "Reading a unconfigured pin");

#if HAS_WIRINGPI
        return digitalRead(static_cast<int>(pin)) == HIGH;
#else
        return false;
#endif
    }

private:
    static inline bool s_setup = false;
};