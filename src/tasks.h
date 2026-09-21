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

public:
    Tasks(size_t stack_size) : count(0), max_count(stack_size) {};
    std::expected<void, Error> enqueue(callable &&func)
    {
        if (count >= max_count)
        {
            return std::unexpected(StackOverflow{});
        }
        buffer.push(std::move(func));
        count++;
        return {};
    };
    std::optional<callable> pop()
    {
        if (buffer.empty())
        {
            return std::nullopt;
        }
        auto next = buffer.front();
        buffer.pop();
        count--;
        return next;
    }
};
