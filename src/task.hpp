#pragma once

#include <stop_token>
#include <utility>
#include <thread>

template<typename Fn>
class task {
public:
    explicit task(Fn&& fn) : m_worker([fn = std::forward<Fn>(fn)](std::stop_token token) {
        fn(token);
    }) {
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
};