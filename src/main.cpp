#include <hardware.hpp>
#include <rom.hpp>
#include <memory.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	ROM<0x100> rom("rom.bin");
	TestHardware testHW;

	CPU cpu;
	cpu.connectRom(rom);
	cpu.connectMemory(memory);
	cpu.connectHardware(0x01, testHW);
	cpu.start();
	return 0;
}
