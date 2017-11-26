#include <iostream>
//
#include <rom.hpp>
#include <memory.hpp>
#include <host_console.hpp>
#include <cpu.hpp>

int main(int argc, char *argv[])
{
	if(argc < 2)
	{
		std::cout << "usage: " << argv[0] << " ROM_PATH\n";
		return 1;
	}
	std::string romPath = argv[1];

	Memory<0x10000> memory;
	ROM<0x100> rom(romPath);
	HostConsole console;

	CPU cpu(false);
	cpu.connectRom(rom);
	cpu.connectMemory(memory);
	cpu.connectHardware(0x01, console);
	cpu.start();
	return 0;
}
