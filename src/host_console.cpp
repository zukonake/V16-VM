#include <iostream>
//
#include "host_console.hpp"

HostConsole::HostConsole()
{
}

Word &HostConsole::handleSdp(Word value)
{
	if(value >= BUFFER_SIZE)
	{
		throw std::out_of_range("buffer address: " + std::to_string(value) + " is out of range");
	}
	return reinterpret_cast<Word &>(buffer[value]);
}

void HostConsole::handleAdpI(Word value)
{
	Command command = static_cast<Command>(value);
	switch(command)
	{
		case NOP: break;
		case WRT: std::cout << reinterpret_cast<char const *>(buffer); break;
		case CLR:
			for(unsigned i = 0; i < BUFFER_SIZE; i++) buffer[i] = 0x00;
			bufferIterator = 0;
			break;
	}
}
