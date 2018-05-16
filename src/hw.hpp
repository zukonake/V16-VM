#pragma once

#include <queue>
#include <atomic>
#include <future>
#include <thread>
#include <mutex>
//
#include <int.hpp>

class Hw
{
    public:
    Hw();
    virtual ~Hw();

    void write(u16 value);
    u16  read();
    protected:
    virtual void single_iteration();
    u16  receive();
    void send(u16);
    private:
    void run();

    u16 input;
    u16 output;

    std::atomic<bool> running;
    std::atomic<bool> input_ready;
    std::atomic<bool> output_ready;
    std::mutex        input_mutex;
    std::mutex        output_mutex;
    std::thread       thread;
    std::condition_variable input_condition;
    std::condition_variable output_condition;
};
