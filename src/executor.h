#pragma once

#include <chrono>
#include <thread>
#include <print>
#include <condition_variable>

#include "tasks.h"
#include "future_tasks.h"

class Executor
{
private:
    std::shared_ptr<Tasks> buf;
    std::shared_ptr<FutureTasks> ft;
    std::chrono::microseconds sleep_duration;
    std::condition_variable cv;

public:
    Executor(std::shared_ptr<Tasks> buf, std::shared_ptr<FutureTasks> ft, std::chrono::microseconds sleep_duration) : buf(buf), ft(ft), sleep_duration(sleep_duration) {};
    void Init(std::stop_token t, std::shared_ptr<ThreadSignal> signal)
    {
        std::println("Executor is Running");
        while (!t.stop_requested())
        {
            while (auto next = buf->pop())
            {
                next.value()();
            }
            auto now = std::chrono::steady_clock::now();
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
                    sleep_duration = std::chrono::duration_cast<std::chrono::microseconds>(next_deadline.value() - now);
                    continue;
                }
            };

            std::unique_lock<std::mutex> lock(signal->mutex);
            signal->cv.wait_for(lock, sleep_duration, [&t, signal, this]()
                                { return signal->has_work || t.stop_requested(); });
            signal->has_work = false;
            std::this_thread::sleep_for(sleep_duration);
        }
    }
};