#include <iostream>

#include <httplib.h>

int main() {
    httplib::Server server;

    server.listen("0.0.0.0", 8080);
}