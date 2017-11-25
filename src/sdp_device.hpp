#pragma once

#include <mutex>
//
#include <v16/architecture.hpp>

class SdpDevice
{
	public:
	virtual ~SdpDevice() = default;

	Word &operator[](Word value);

	Word &sdp(Word value);
	protected:
	virtual Word &handleSdp(Word value) = 0;
	private:
	std::mutex mutex;
};
