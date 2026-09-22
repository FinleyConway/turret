#include "config/application_config.hpp"

#include <fstream>
#include <stdexcept>

#include <nlohmann/json.hpp>

#include "config/json/stepper_config_json.hpp"

namespace turret {
    application_config application_config::read(const std::filesystem::path& path) {
        std::ifstream file(path);
        
        if (!file) {
            throw std::runtime_error(
                "Failed to open config: " + path.string()
            );
        }

        auto json = nlohmann::json::parse(file);

        application_config config;

        const auto& stepper = json.at("stepper_motor");

        config.stepper = stepper.get<stepper_config>();
        config.pan = stepper.at("pan_axis").get<stepper_axis_config>();
        config.tilt = stepper.at("tilt_axis").get<stepper_axis_config>();

        return config;
    }
}