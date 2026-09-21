#pragma once

#include <condition_variable>
#include <stop_token>
#include <utility>
#include <mutex>
#include <deque>

// https://www.freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/09-xQueueReceive

namespace turret {
    template<typename T>
    class event_queue {
    public:
        event_queue() = default;

        ~event_queue() {
            {
                std::lock_guard lock(m_mutex);

                m_shutdown = true;
            }

            m_condition_var.notify_all();
        }

    public:
        bool receive(T& data, std::stop_token token) {
            std::unique_lock lock(m_mutex);

            // block thread when the queue is empty or if queue is destoryed
            if (!m_condition_var.wait(lock, token, [this] {
                return !m_queue.empty() || m_shutdown;
            })) {
                return false;
            }

            // leave function as the queue is destroyed 
            // prevents dangling task
            if (m_shutdown) return false;

            // move, pop and return the receieved value
            // making sure the queue no longer owns the data anymore
            data = std::move(m_queue.front());
            m_queue.pop_front();

            return true;
        }

        void send(const T& data) {
            {   
                std::lock_guard lock(m_mutex);

                // prevent any more requests as queue is being destroyed
                if (m_shutdown) return;

                m_queue.emplace_back(data);
            }

            // notify the waiting blocked thread
            m_condition_var.notify_one();
        }

        size_t size() const {
            std::lock_guard lock(m_mutex);

            return m_queue.size();
        }

    private:
        bool m_shutdown = false;
        std::deque<T> m_queue;
        std::mutex m_mutex;
        std::condition_variable_any m_condition_var;
    };
}