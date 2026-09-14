#pragma once

#include <mutex>
#include <deque>
#include <condition_variable>

#include "task.hpp"

// https://www.freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/09-xQueueReceive

template<typename T>
class event_queue {
public:
    bool receive(T& data, const task_context& ctx) {
        std::unique_lock lock(m_mutex);

        // block thread when the queue is empty or if task is done
        // context makes sure that the task isnt dangling when it wants to end
        m_condition_var.wait(lock, [&] {
            return !m_queue.empty() || ctx.is_cancelled();
        });

        // return with no value if task is done
        if (ctx.is_cancelled()) return false;

        // move, pop and return the receieved value
        // making sure the queue no longer owns the data anymore
        data = std::move(m_queue.front());
        m_queue.pop_front();

        return true;
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