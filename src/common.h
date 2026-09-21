#pragma once

#include <string_view>
#include <functional>
#include <variant>
#include <syncstream>

using callable = std::function<void()>;

struct StackOverflow
{
    std::string_view message = "StackOverflow";
};

using Error = std::variant<StackOverflow>;