#include <memory.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	CPU cpu(memory, "rom.bin");
	cpu.start();
	return 0;
}
