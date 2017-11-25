#include "adp_device.hpp"

AdpDevice::~AdpDevice()
{
	if(thread.joinable()) thread.join();
}

void AdpDevice::adpI(Word value)
{
	if(thread.joinable()) thread.join();
	thread = std::thread(&AdpDevice::handleAdpI, this, value);
}

std::future<Word> AdpDevice::adpO()
{
	if(thread.joinable()) thread.join();
	return output.get_future();
}
