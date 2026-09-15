#pragma once

#include <stop_token>
#include <concepts>
#include <utility>
#include <thread>

template<typename TWorker>
concept Worker = requires(TWorker worker, std::stop_token token) {
    { worker.run(token) }  -> std::same_as<void>;
};

template<Worker T>
class task {
public:
    template<typename... Args>
    explicit task(Args&&... args)
        : m_worker([args = std::forward_as_tuple(std::forward<Args>(args)...)](std::stop_token token) {
            // very messy template wise but it allows me to pass constructor args syntax cleanly
            std::apply([&](auto&&... args) {
                T worker(std::forward<decltype(args)>(args)...);
                    worker.run(token);
                },
                args
            );
        }) 
    {
    }

    ~task() = default;
    task(const task&) = delete;
    task& operator=(const task&) = delete;

public:
    void stop() {
        m_worker.request_stop();
    }

private:
    std::jthread m_worker;
};