#include <iostream>

#include "task.hpp"
#include "event_queue.hpp"

int main() {
    event_queue<int> q; 
    task task([&](std::stop_token token) {
        int i = 0;

        while (q.receive(i, token)) {
            std::cout << "Received number: " << i << std::endl;
        }

        std::cout << "task ended!\n";
    });

    q.send(10);
}