#include "core/application.hpp"
#include "core/gpio.hpp"
#include "config/application_config.hpp"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: program config/settings.json\n";

        return -1;
    }

    // init raspberry gpio
    turret::gpio::setup();

    turret::application app(
        turret::application_config::read(argv[1])
    );

    app.start();
}
