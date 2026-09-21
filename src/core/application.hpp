#pragma once

#include <httplib.h>

#include "config/application_config.hpp"

#include "task/events/stepper_control.hpp"
#include "task/event_queue.hpp"

#include "task/tasks/stepper_task.hpp"
#include "task/task.hpp"

#include "core/gpio.hpp"

namespace turret {
    class application {
    public:
        explicit application(const application_config& config) : 
            m_config(config),
            m_stepper_task(m_stepper_event, m_config.stepper_motor)
        {
        }

    public:
        void start() {
            // init raspberry gpio 
            gpio::setup();
            
            // setup http server for call and response
            httplib::Server server;

            create_endpoints(server);

            server.listen("0.0.0.0", 8080);
        }

    private:
        void create_endpoints(httplib::Server& server) {
            server.Post("/api/stepper_motor/pan/:angle", [&](const httplib::Request& req, httplib::Response& res) {
                try {
                    // TODO: need to saftly convert to u16 and keep it within angle range
                    const auto angle = std::stoul(req.path_params.at("angle"));

                    m_stepper_event.send(stepper_control {
                        .angle = angle
                    });

                    res.status = 200;
                } 
                catch (const std::exception&) {
                    res.status = 400;
                    res.set_content("Invalid angle parameter", "text/plain");
                }
            });
        }

    private:
        application_config m_config;

        task<stepper_task> m_stepper_task;
        event_queue<stepper_control> m_stepper_event;
    };
}