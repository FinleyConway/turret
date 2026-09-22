#pragma once

#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "config/stepper_config.hpp"

namespace turret {
    struct application_config;

    void from_json(const nlohmann::json& json, application_config& config);

    struct application_config {
        stepper_config stepper;
        stepper_axis_config pan;
        stepper_axis_config tilt;

        static application_config read(const std::filesystem::path& path) {
            std::ifstream file(path);
            
            if (!file) {
                throw std::runtime_error(
                    "Failed to open config: " + path.string()
                );
            }

            auto json = nlohmann::json::parse(file);

            return json.get<application_config>();
        }
    };

    inline void from_json(const nlohmann::json& json, application_config& a) {
        const auto& stepper = json.at("stepper_motor");

        a.stepper = stepper.get<stepper_config>();
        a.pan = stepper.at("pan_axis").get<stepper_axis_config>();
        a.tilt = stepper.at("tilt_axis").get<stepper_axis_config>();
    }
}