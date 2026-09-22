#pragma once

#include <queue>
#include <chrono>
#include <expected>
#include <vector>
#include <iostream>
#include <print>
#include <optional>

#include "common.h"

struct FutureStruct
{
    unsigned int id;
    callable func;
    std::chrono::_V2::steady_clock::time_point in;
};

struct FutureStructCompare
{
    bool operator()(const FutureStruct &f1, const FutureStruct &f2) const
    {
        return f1.in > f2.in;
    }
};

class FutureTasks
{
private:
    std::priority_queue<FutureStruct, std::vector<FutureStruct>, FutureStructCompare> ft;
    size_t max;
    size_t count;
    size_t id = 0;
    std::shared_ptr<ThreadSignal> signal;
    std::mutex ft_mutex;

public:
    FutureTasks(size_t max, std::shared_ptr<ThreadSignal> signal) : max(max), count(0), signal(signal) {};
    std::expected<void, Error> enqueue(FutureStruct &&f)
    {
        {
            std::lock_guard<std::mutex> lock(ft_mutex);

            if (count >= max)
                return std::unexpected(StackOverflow{});

            f.id = id;

            ft.push(std::move(f));
            ++count;
            ++id;
        }

        {
            std::lock_guard<std::mutex> lock(signal->mutex);
            signal->has_work = true;
        }
        signal->cv.notify_one();

        return {};
    }

    std::optional<FutureStruct> pop()
    {
        FutureStruct top;
        {
            std::lock_guard<std::mutex> lock(ft_mutex);
            if (ft.empty())
                return std::nullopt;

            top = ft.top();
            ft.pop();
            --count;
        }
        return top;
    }

    std::optional<std::chrono::_V2::steady_clock::time_point> peek_next_timeout()
    {
        if (ft.empty())
            return std::nullopt;
        return ft.top().in;
    }
};