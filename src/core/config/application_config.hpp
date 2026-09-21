#pragma once

#include <fstream>
#include <filesystem>

#include <nlohmann/json.hpp>

#include "stepper_config.hpp"

struct application_config;

void from_json(const nlohmann::json& json, application_config& config);

struct application_config {
    stepper_config stepper_motor;

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

void from_json(const nlohmann::json& json, application_config& a) {
    a.stepper_motor = json.at("stepper_motor").get<stepper_config>();
}