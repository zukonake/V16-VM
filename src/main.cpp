#include <memory.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	Hardware<0x100> hardware;
	CPU cpu(memory, "eeprom.bin");
	cpu.connectHardware(0x00, hardware);
	cpu.start();
	return 0;
}
