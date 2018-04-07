#include "clock.hpp"

namespace utility
{

Clock::Clock() :
    currentCycle(0),
    cycle(0),
    mBefore(mClock.now())
{

}

void Clock::reset()
{
    currentCycle = Duration(0);
}

void Clock::start()
{
    mBefore = mClock.now();
}

void Clock::stop()
{
    currentCycle += Duration(mClock.now() - mBefore);
}

void Clock::setCycle(Clock::Duration value)
{
    cycle = value;
}

typename Clock::Duration Clock::synchronize()
{
    Duration delta = cycle - currentCycle;
    if(currentCycle < cycle)
    {
        std::this_thread::sleep_for( delta );
        reset();
    }
    currentCycle = Duration::zero();
    return delta;
}

}
