#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>

// https://www.freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/09-xQueueReceive

template<typename T>
class event_queue {
public:
    T receive() {
        std::unique_lock lock(m_mutex);

        // block thread when the queue is empty
        m_condition_var.wait(lock, [&] {
            return !m_queue.empty();
        });

        // move, pop and return the receieved value
        // making sure the queue no longer owns the data anymore
        T data = std::move(m_queue.front());
        m_queue.pop_front();

        return data;
    }

    void send(const T& data) {
        {
            std::lock_guard lock(m_mutex);

            m_queue.emplace_back(std::move(data));
        }

        // notify the waiting blocked thread
        m_condition_var.notify_one();
    }

    size_t size() const {
        return m_queue.size();
    }

private:
    std::deque<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_condition_var;
};