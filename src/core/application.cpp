#include "core/application.hpp"

#include "core/gpio.hpp"

namespace turret {
    application::application(const application_config& config) : 
        m_config(config),
        m_stepper_pan_task(m_stepper_event, m_config.stepper, m_config.pan)
    {
    }

    void application::start() {
        // init raspberry gpio 
        gpio::setup();
        
        // setup http server for call and response
        httplib::Server server;

        create_endpoints(server);

        server.listen("0.0.0.0", 8080);
    }

    void application::create_endpoints(httplib::Server& server) {
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
}