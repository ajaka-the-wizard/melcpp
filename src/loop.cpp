#include <functional>
#include <variant>
#include <queue>
#include <string_view>
#include <optional>
#include <expected>
#include <memory>
#include <iostream>

void print(std::string_view msg)
{
    std::cout << msg << "\n";
}

using callable = std::function<void()>;

struct StackOverflow
{
    std::string_view message = "StackOverflow";
};

using Error = std::variant<StackOverflow>;

class Queue
{
private:
    std::queue<callable> buffer;
    size_t count;
    size_t max_count;

public:
    Queue(size_t stack_size) : count(0), max_count(stack_size) {};
    std::expected<void, Error> enqueue(callable &&func)
    {
        if (count >= max_count)
        {
            return std::unexpected(StackOverflow{});
        }
        buffer.push(std::move(func));
        count++;
        return {};
    }
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

class Executor
{
private:
    std::shared_ptr<Queue> buf;

public:
    Executor(std::shared_ptr<Queue> buf) : buf(buf) {};
    void Init()
    {
        print("Executor is Running");
        while (1)
        {
            auto next = buf->pop();
            if (!next.has_value())
            {
                print("Callback drained, Executor exiting");
                break;
            }
            next.value()();
        }
    }
};

void ExampleUsage()
{
    auto buf = std::make_shared<Queue>(5);

    buf->enqueue([]{ print("func One"); });

    buf->enqueue([]{ print("func Two"); });

    buf->enqueue([]{ print("func Three"); });
    Executor(buf).Init();
}
