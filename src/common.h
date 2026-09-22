#pragma once

#include <string_view>
#include <functional>
#include <variant>
#include <syncstream>
#include <mutex>
#include <condition_variable>

using callable = std::function<void()>;

struct StackOverflow
{
    std::string_view message = "StackOverflow";
};

using Error = std::variant<StackOverflow>;

struct ThreadSignal
{
    std::mutex mutex;
    std::condition_variable cv;
    bool has_work = false;
};