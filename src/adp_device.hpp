#pragma once

#include <cstddef>
#include <thread>
#include <future>
//
#include <v16/architecture.hpp>

class AdpDevice
{
	public:
	virtual ~AdpDevice();

	void adpI(Word value);
	std::future<Word> adpO();
	protected:
	virtual void handleAdpI(Word value) = 0;

	std::promise<Word> output;
	private:
	std::thread thread;
};
