#pragma once

#include <cstddef>
#include <string>
//
#include <v16/architecture.hpp>
#include <hardware.hpp>

class HostConsole : public Hardware
{
	public:
	static std::size_t const BUFFER_SIZE = 0x80;
	enum Command
	{
		NOP = 0x00,
		WRT = 0x01,
		CLR = 0x02,
	};

	protected:
	virtual Word &handleSdp(Word value) override;
	virtual void handleAdpI(Word value) override;

	Word buffer[BUFFER_SIZE];
};

