#pragma once

#include <httplib.h>

#include "event_queue.hpp"
#include "task.hpp"
#include "gpio.hpp"

class application {
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
};