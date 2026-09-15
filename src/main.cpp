#include <iostream>

#include "task.hpp"
#include "event_queue.hpp"

class test_task {
public:
    test_task(event_queue<int>& q) : m_test_q(q) {
    }

public:
    void run(std::stop_token token) {
        int i = 0;

        while (m_test_q.receive(i, token)) {
            std::cout << "Received number: " << i << std::endl;
        }

        std::cout << "task ended!\n";
    }

private:
    event_queue<int>& m_test_q;
};

int main() {
    event_queue<int> q; 
    task<test_task> t(q);

    q.send(10);
    q.send(9);
    q.send(8);
    q.send(7);
    q.send(6);
    q.send(5);

    std::this_thread::sleep_for(std::chrono::seconds(1));
}