#include "clock.hpp"

namespace utility
{

Clock::Clock() :
    current_cycle(0),
    cycle(0),
    before(clock.now())
{

}

void Clock::reset()
{
    current_cycle = Duration(0);
}

void Clock::start()
{
    before = clock.now();
}

void Clock::stop()
{
    current_cycle += Duration(clock.now() - before);
}

void Clock::set_cycle(Clock::Duration value)
{
    cycle = value;
}

typename Clock::Duration Clock::synchronize()
{
    Duration delta = cycle - current_cycle;
    if(current_cycle < cycle)
    {
        std::this_thread::sleep_for(delta);
        reset();
    }
    current_cycle = Duration::zero();
    return delta;
}

}
