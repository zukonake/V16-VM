#include <iostream>
#include "hardware.hpp"

void Hardware::receive(Word word)
{
	(void) word;
}

Word &Hardware::operator[](Word address)
{
	(void) address;
	dummy = 0x0000;
	return dummy;
}

Word const &Hardware::operator[](Word address) const
{
	(void) address;
	dummy = 0x0000;
	return dummy;
}

void TestHardware::receive(Word word)
{
	std::cout << "TestHardware: received word: " << std::hex << word << "\n";
}
