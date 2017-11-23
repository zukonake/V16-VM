#include <memory.hpp>
#include <cpu.hpp>

int main()
{
	Memory<0x10000> memory;
	CPU cpu(memory);
	cpu.start();
	while(true){}
	return 0;
}
