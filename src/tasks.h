#pragma once

#include <functional>
#include <variant>
#include <queue>
#include <string_view>
#include <optional>
#include <expected>
#include <memory>
#include <iostream>
#include <print>

#include "common.h"

class Tasks
{
private:
    std::queue<callable> buffer;
    size_t count;
    size_t max_count;
    std::shared_ptr<ThreadSignal> signal;
    std::mutex tasks_mutex;

public:
    Tasks(size_t stack_size, std::shared_ptr<ThreadSignal>) : count(0), max_count(stack_size), signal(signal) {};
    std::expected<void, Error> enqueue(callable &&func)
    {
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            if (count >= max_count)
            {
                return std::unexpected(StackOverflow{});
            }
            buffer.push(std::move(func));
            --count;
        }

        {
            std::lock_guard<std::mutex> lock(signal->mutex);
            signal->has_work = true;
        }
        signal->cv.notify_one();
        return {};
    };
    std::optional<callable> pop()
    {
        callable next;
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            if (buffer.empty())
            {
                return std::nullopt;
            }

            next = buffer.front();
            buffer.pop();
            --count;
        }
        return next;
    }
};
