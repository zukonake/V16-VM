#include <rom.hpp>
#include <memory.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	ROM<0x100> rom("rom.bin");

	CPU cpu;
	cpu.connectHardware(0xFF, rom);
	cpu.connectMemory(memory);
	cpu.start();
	return 0;
}
