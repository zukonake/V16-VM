#pragma once

#include <string>
#include <v16/architecture.hpp>
//
#include <hardware.hpp>

class HostConsole : public Hardware
{
	public:
	enum class Command
	{
		NOP = 0x00,
		APD = 0x01,
		WRT = 0x02,
		CLR = 0x03,
	};

	protected:
	virtual void handleAdpI(Word value) override;

	std::string buffer;
};

