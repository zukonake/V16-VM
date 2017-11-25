#include <iostream>
//
#include "host_console.hpp"

void HostConsole::handleAdpI(Word value)
{
	Command command = static_cast<Command>((value & 0xFF00) >> 8);
	switch(command)
	{
		case NOP: break;
		case APD: buffer += static_cast<char>(value & 0x00FF); break;
		case WRT: std::cout << buffer; break;
		case CLR: buffer = ""; break;
	}
}
