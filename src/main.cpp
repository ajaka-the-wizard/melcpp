#include <iostream>

#include "event_loop.h"

int main()
{

    auto loop = std::make_unique<EventLoop>(200);
    loop->Run();

    while (true)
    {

        loop->Set_timeout([]
                          { std::println("func One timeout"); }, std::chrono::milliseconds(300));

        loop->Set_timeout([]
                          { std::println("func Two timeout"); }, std::chrono::milliseconds(400));

        loop->Set_timeout([]
                          { std::println("func Three timeout"); }, std::chrono::milliseconds(100));

        loop->Post([]
                   { std::println("func One"); });

        loop->Post([]
                   { std::println("func Two"); });

        loop->Post([]
                   { std::println("func Three"); });

        loop->Stop();
        break;
    }
    return 0;
}
