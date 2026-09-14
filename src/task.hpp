#pragma once

#include <thread>
#include <atomic>

class task_context {
public:
    bool is_cancelled() const {
        return m_cancelled.load();
    }

private:
    friend class task;

private:
    std::atomic_bool m_cancelled = false;
};

template<typename Fn>
class task {
public:
    explicit task(Fn&& fn)
        : m_worker([this, fn = std::forward<Fn>(fn)] {
            fn(m_context);
        })
    {
    }

    ~task() {
        stop();

        if (m_worker.joinable()) {
            m_worker.join();
        }
    }

    void stop() {
        m_context.m_cancelled = true;
    }

private:
    task() = default;

private:
    std::thread m_worker;
    task_context m_context;
};