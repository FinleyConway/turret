#pragma once

#include <thread>
#include <atomic>
#include <utility>

template<typename Fn> class task;

class task_context {
public:
    bool is_cancelled() const {
        return m_cancelled.load();
    }

private:
    template<typename>
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

    ~task() = default;
    task(const task&) = delete;
    task& operator=(const task&) = delete;

    void stop() {
        m_worker.request_stop();
    }

private:
    task() = default;

private:
    std::jthread m_worker;
    task_context m_context;
};