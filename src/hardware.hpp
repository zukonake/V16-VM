#pragma once

#include <v16/architecture.hpp>
#include <sdp_device.hpp>
#include <adp_device.hpp>

class Hardware : public SdpDevice, public AdpDevice
{
	public:
	virtual ~Hardware() = default;

	protected:
	virtual Word &handleSdp(Word value) override;
	virtual void handleAdpI(Word value) override;

	private:
	Word mutable dummy;
};

class DummyHardware : public Hardware { };
