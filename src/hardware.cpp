#include "hardware.hpp"

void Hardware::receive(Word word)
{
	(void) word;
}

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
