#pragma once

#include <httplib.h>

#include "config/application_config.hpp"

#include "task/events/stepper_control.hpp"
#include "task/event_queue.hpp"

#include "task/tasks/stepper_task.hpp"
#include "task/task.hpp"

namespace turret {
    class application {
    public:
        explicit application(const application_config& config);

    public:
        void start();

    private:
        void create_endpoints(httplib::Server& server);

    private:
        application_config m_config;

        task<stepper_task> m_stepper_pan_task;
        event_queue<stepper_control> m_stepper_event;
    };
}