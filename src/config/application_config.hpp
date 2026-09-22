#pragma once

#include <filesystem>

#include "config/stepper_config.hpp"

namespace turret {
    struct application_config {
        stepper_config stepper;
        stepper_axis_config pan;
        stepper_axis_config tilt;

        static application_config read(const std::filesystem::path& path);
    };
}