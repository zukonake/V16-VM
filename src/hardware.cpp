#include "hardware.hpp"

Word &DummyHardware::operator[](Byte address)
{
	(void) address;
	dummy = 0x0000;
	return dummy;
}

Word const &DummyHardware::operator[](Byte address) const
{
	(void) address;
	dummy = 0x0000;
	return dummy;
}

