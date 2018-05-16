#include <mutex>
//
#include "hw.hpp"

Hw::Hw() :
    running(true),
    input_ready(true),
    output_ready(true),
    thread([&]() { run(); })
{

}

Hw::~Hw()
{
    running = false;
    thread.join();
}

void Hw::write(u16 value)
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    input_condition.wait(lock, [&]{return input_ready.load();});
    input = value;
    input_ready = false;
    input_condition.notify_all();
}

u16 Hw::read()
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    output_condition.wait(lock, [&]{return !output_ready.load();});
    u16 val = output;
    output_ready = true;
    output_condition.notify_all();
    return val;
}

u16 Hw::receive()
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    input_condition.wait(lock, [&]{return !input_ready.load();});
    u16 val = input;
    input_ready = true;
    input_condition.notify_all();
    return val;
}

void Hw::send(u16 value)
{
    std::mutex mutex;
    std::unique_lock<std::mutex> lock(mutex);
    output_condition.wait(lock, [&]{return output_ready.load();});
    output = value;
    output_ready = false;
    output_condition.notify_all();
}

void Hw::single_iteration()
{
    /* This function must be implemented, because otherwise it will be called
     * from Hw::run(), before the derived is constructed, since Hw::run() is
     * called from Hw constructor, thus causing a pure virtual function call.
     *
     * After the derived object is constructed, Hw::run() will call the
     * derived version instead.
     *
     * This solution might not be the most optimal, but I've found it to be
     * the most trivial and convinient to implement.
     */
}

void Hw::run()
{
    while(running)
    {
        single_iteration();
    }
}
