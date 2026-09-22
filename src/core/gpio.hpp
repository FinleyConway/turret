#pragma once

// https://github.com/WiringPi/WiringPi/blob/master/documentation/english/functions.md

namespace turret {
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
        static void setup();

        static void set_mode(pin pin, mode mode);

        static void write(pin pin, bool level);

        static bool read(pin pin);

        static pin to_pin(int value);

    private:
        static inline bool s_setup = false;
    };
}