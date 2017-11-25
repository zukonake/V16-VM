#include <mutex>
//
#include "sdp_device.hpp"

Word &SdpDevice::operator[](Word value)
{
	return sdp(value);
}

Word &SdpDevice::sdp(Word value)
{
	std::lock_guard<std::mutex> lock(mutex);
	return handleSdp(value);
}

