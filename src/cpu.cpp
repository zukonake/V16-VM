#include <stdexcept>
#include <fstream>
#include <iostream>
//
#include "cpu.hpp"

CPU::CPU(MemoryInterface &memory, std::string const &romPath) :
	M(&memory),
	PC(0x0000),
	F(0x0),
	SP(0x00)
{
	for(Word i = 0x00; i < 0x100; i++) S[i] = 0x0000;
	for(Word i = 0x0; i < 0x10; i++) R[i] = 0x0000;
	Word ROM[0x100];
	std::ifstream file(romPath, std::ios::in | std::ios::binary);
	if(file.is_open())
	{
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char *>(ROM), 0x200);
		file.close();
	}
	else throw std::runtime_error("couldn't open rom file");
	for(Word i = 0x0; i < 0x10; i++) (*M)[i] = ROM[i];
}

void CPU::start()
{
	running = true;
	loop();
}

void CPU::connectHardware(Byte channel, HardwareInterface &hardware)
{
	HW[channel] = &hardware;
}

void CPU::loop()
{
	while(running)
	{
		clock.start();
		execute();
		clock.stop();
		clock.synchronize();
	}
}

void CPU::execute()
{
	Word base = (*M)[PC];
	Byte I = base >> 8;
	Nibble A = (base & 0x00F0) >> 4;
	Nibble B = (base & 0x000F);
	unsigned instructionSize = getInstructionSize(I, A, B);
	Word &X = fetchValue(A, PC + 1);
	Word &Y = fetchValue(B, PC + 2);
	std::cout << std::hex << "PC: " << PC << "\n";
	if(instructionSize >= 1) std::cout << "\tW0: " << (*M)[PC] << "\n";
	if(instructionSize >= 2) std::cout << "\tW1: " << (*M)[PC + 1] << "\n";
	if(instructionSize >= 3) std::cout << "\tW2: " << (*M)[PC + 2] << "\n";
	std::cout << "\n";
	switch(I)
	{
		case NOP: break;
		case JMP: PC = X; break;
		case CLL:
			S[++SP] = PC;
			PC = X;
			break;
		case RET: PC = S[SP--]; break;
		case PNC: F |= 0b0100; break;
		//
		case MOV: Y = X; break;
		case CPY: Y = X; break;
		case SWP:
			{
				Word buffer = Y;
				Y = X;
				X = buffer;
			}
			break;
		//
		case IEQ: if(X == Y) ++PC; break;
		case INQ: if(X != Y) ++PC; break;
		case IGT: if(X >  Y) ++PC; break;
		case ILT: if(X <  Y) ++PC; break;
		case IGQ: if(X >= Y) ++PC; break;
		case ILQ: if(X <= Y) ++PC; break;
		//
		case NOT: X = !X; break;
		case OR : Y |= X; break;
		case AND: Y &= X; break;
		case XOR: Y ^= X; break;
		case RSF: Y >>= X; break;
		case LSF: Y <<= X; break;
		//
		case ADD: Y += X; break;
			if(X > 0x10000 - Y) F |= 0b1000;
			else F &= 0b0111;
			Y += X;
			break;
		case SUB:
			if(X > Y) F |= 0b1000;
			else F &= 0b0111;
			Y -= X;
			break;
		case MUL: Y *= X; break;
			if(X > 0x10000 / Y) F |= 0b1000;
			else F &= 0b0111;
			Y *= X;
			break;
		case DIV:
			if((Y % X) != 0) F |= 0b1000;
			else F &= 0b0111;
			Y /= X;
			break;
		case MOD: Y %= X; break;
		default: throw std::runtime_error("illegal instruction");
	}
	if(I != JMP) PC += instructionSize;
}

unsigned CPU::getModeSize(Nibble mode)
{
	unsigned size = 0;
	switch(static_cast<PortMode>(mode & 0b0111))
	{
		case PortMode::M:
		case PortMode::R:
		case PortMode::H: size += 1; break;
		//
		default: break;
	}
	return size;
}

unsigned CPU::getInstructionSize(Word instruction, Nibble A, Nibble B)
{
	unsigned size = 1;
	bool useA;
	bool useB;
	switch(instruction)
	{
		case NOP:
		case RET:
		case PNC: useA = false; useB = false; break;
		//
		case JMP:
		case CLL:
		case NOT: useA = true; useB = false; break;
		//
		default: useA = true; useB = true; break;
	}
	if(useA) size += getModeSize(A);
	if(useB)
	{
		unsigned Bsize = getModeSize(B);
		if(size == 1 && Bsize == 1) size += 2;
		else size += Bsize;
	}
	return size;
}

Word &CPU::fetchValue(Nibble mode, Word address)
{
	bool indirect = (mode & 0b1000) >> 3;
	mode = (mode & 0b0111);
	if(indirect && mode > 04) throw std::runtime_error("illegal mode");
	Word *output;
	Word &value = (*M)[address];
	switch(static_cast<PortMode>(mode))
	{
		case PortMode::M: output = &value; break;
		case PortMode::R: output = &R[value & 0x00FF]; break;
		case PortMode::H: output = &(*HW)[value & 0xFF00][value & 0x00FF]; break;
		case PortMode::P: output = &PC; break;
		case PortMode::T: output = &S[SP]; break;
		case PortMode::S: output = reinterpret_cast<Word *>(&SP); break;
		case PortMode::F: output = reinterpret_cast<Word *>(&F); break;
		default: throw std::runtime_error("illegal mode");
	}
	if(indirect) return (*M)[*output];
	else return *output;
}

