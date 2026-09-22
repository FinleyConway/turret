#pragma once

#include <nlohmann/json.hpp>

#include "core/gpio.hpp"

namespace turret {
    inline void from_json(const nlohmann::json& j, gpio::pin& p) {
        p = gpio::to_pin(j.get<int>());
    }
}