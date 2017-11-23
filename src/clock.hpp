#pragma once

#include <thread>
#include <chrono>

namespace utility
{

template< typename unit, typename ratio >
class Clock
{
	public:
	typedef std::chrono::duration< unit, ratio > Duration;
	typedef std::chrono::time_point< std::chrono::steady_clock > TimePoint;

	Clock();

	void reset();
	void start();
	void stop();
	Duration get();
	Duration synchronize();
	private:
	std::chrono::steady_clock mClock;
	Duration mCycle;
	TimePoint mBefore;
};

template< typename unit, typename ratio >
Clock< unit, ratio >::Clock() :
	mCycle( 0 ),
	mBefore( mClock.now())
{

}

template< typename unit, typename ratio >
void Clock< unit, ratio >::reset()
{
	mCycle = Duration( 0 );
}

template< typename unit, typename ratio >
void Clock< unit, ratio >::start()
{
	mBefore = mClock.now();
}

template< typename unit, typename ratio >
void Clock< unit, ratio >::stop()
{
	mCycle += Duration( mClock.now() - mBefore );
}

template< typename unit, typename ratio >
typename Clock< unit, ratio >::Duration Clock< unit, ratio >::get()
{
	return mCycle;
}

template< typename unit, typename ratio >
typename Clock< unit, ratio >::Duration Clock< unit, ratio >::synchronize()
{
	Duration delta = Duration( 1 ) - mCycle;
	if( mCycle < Duration( 1 ))
	{
		std::this_thread::sleep_for( delta );
		reset();
	}
	mCycle = Duration::zero();
	return delta;
}

}
