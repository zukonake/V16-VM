#pragma once

#include <thread>
#include <chrono>

namespace utility
{

using namespace std::chrono;

class Clock
{
    public:
    typedef duration<double> Duration;
    typedef time_point<steady_clock> TimePoint;

    Clock();

    void reset();
    void start();
    void stop();
    void set_cycle(Duration value);
    Duration get();
    Duration synchronize();
    private:
    steady_clock clock;
    Duration current_cycle;
    Duration cycle;
    TimePoint before;
};

}
