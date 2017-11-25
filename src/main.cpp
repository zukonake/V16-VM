#include <rom.hpp>
#include <memory.hpp>
#include <host_console.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	ROM<0x100> rom("test.bin");
	HostConsole console;

	CPU cpu;
	cpu.connectRom(rom);
	cpu.connectMemory(memory);
	cpu.connectHardware(0x01, console);
	cpu.start();
	return 0;
}
