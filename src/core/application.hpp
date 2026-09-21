#pragma once

#include <httplib.h>

#include "config/application_config.hpp"
#include "task/event_queue.hpp"
#include "task/task.hpp"
#include "core/gpio.hpp"

class application {
public:
    explicit application(const application_config& config) {
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
    }

private:
    application_config m_config;
};