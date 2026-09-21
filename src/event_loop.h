#pragma once

#include <memory>
#include <expected>

#include "tasks.h"
#include "executor.h"
#include "future_tasks.h"

class EventLoop
{
private:
    std::shared_ptr<Tasks> tasks;
    std::shared_ptr<FutureTasks> ft;
    Executor e;
    std::jthread executor_thread;

public:
    EventLoop(size_t stack_size) : tasks(std::make_shared<Tasks>(stack_size / 2)), ft(std::make_shared<FutureTasks>(stack_size / 2)), e(tasks, ft, std::chrono::microseconds(200)) {};
    void Run()
    {
        executor_thread = std::jthread([this](std::stop_token t)
                                       { e.Init(t); });
    }
    ~EventLoop()
    {
        Stop();
    }

    void Stop()
    {
        if (executor_thread.joinable())
        {
            executor_thread.request_stop(); // Signals the while loop to terminate
            executor_thread.join();         // Safely cleans up the background thread handles
        }
    }
    std::expected<void, Error> Post(callable &&func)
    {
        return tasks->enqueue(std::move(func));
    }

    std::expected<void, Error> Set_timeout(callable &&func, std::chrono::milliseconds delay)
    {
        auto f = FutureStruct{};
        f.func = func;
        f.in = std::chrono::steady_clock::now() + delay;
        ft->enqueue(std::move(f));
        return {};
    }
};
