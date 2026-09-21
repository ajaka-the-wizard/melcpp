#pragma once

#include <chrono>
#include <thread>
#include <print>

#include "tasks.h"
#include "future_tasks.h"

class Executor
{
private:
    std::shared_ptr<Tasks> buf;
    std::shared_ptr<FutureTasks> ft;
    std::chrono::microseconds sleep_duration;

public:
    Executor(std::shared_ptr<Tasks> buf, std::shared_ptr<FutureTasks> ft, std::chrono::microseconds sleep_duration) : buf(buf), ft(ft), sleep_duration(sleep_duration) {};
    void Init(std::stop_token t)
    {
        std::println("Executor is Running");
        while (!t.stop_requested())
        {
            auto now = std::chrono::steady_clock::now();
            while (auto next = buf->pop())
            {
                next.value()();
            }
            if (auto next_deadline = ft->peek_next_timeout())
            {
                if (now >= next_deadline)
                {
                    auto v = ft->pop().value();
                    v.func();
                    continue;
                }
                else
                {
                    auto next_sleep_duration = next_deadline.value() - now;
                    std::this_thread::sleep_for(next_sleep_duration);
                    continue;
                }
            }
            std::println("All queues drained, Executor sleeping for standard interval {}", sleep_duration);
            std::this_thread::sleep_for(sleep_duration);
        }
    }
};