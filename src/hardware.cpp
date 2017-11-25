#include <iostream>
#include "hardware.hpp"

Word &Hardware::handleSdp(Word value)
{
	(void) value;
	dummy = 0x0000;
	return dummy;
}

void Hardware::handleAdpI(Word value)
{
	(void) value;
	AdpDevice::output.set_value(0x0000);
}

