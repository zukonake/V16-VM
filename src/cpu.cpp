#include <iostream>
//
#include "cpu.hpp"

CPU::CPU() :
	PC(0x0000),
	F(0x0),
	SP(0x00),
	cycle(1)
{
	for(unsigned i = 0x00; i < 0x100; i++) S[i] = 0x0000;
	for(unsigned i = 0x0; i < 0x10; i++) R[i] = 0x0000;
	for(Word i = 0x00; i < 0x100; i++) connectHardware(static_cast<Byte>(i), dummyHardware);
}

void CPU::start()
{
	copyRom();
	running = true;
	loop();
}

void CPU::connectMemory(MemoryInterface &memory)
{
	M = &memory; //TODO M as reference to HW[0x00], HW takes Words as addresses
	disconnectHardware(0x00);
	connectHardware(0x00, dynamic_cast<Hardware &>(memory));
}

void CPU::connectHardware(Byte channel, Hardware &hardware)
{
	HW[channel] = &hardware;
}

void CPU::disconnectHardware(Byte channel)
{
	HW[channel] = &dummyHardware;
}

void CPU::copyRom()
{
	for(Word i = 0x00; i < 0x100; i++)
	{
		Word const &word = (*HW[0xFF])[static_cast<Byte>(i)];
		(*M)[i] = word;
	}
}

void CPU::loop()
{
	while(running)
	{
		clock.start();
		if(--cycle == 0)
		{
			execute();
		}
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
	cycle = getInstructionCycle(I, A, B);
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
		case NOT: Y = !X; break;
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
		case SND: HW[static_cast<Byte>(Y)]->receive(X); break;
		default: throw std::runtime_error("illegal instruction");
	}
	if(I != JMP) PC += instructionSize;
}

unsigned CPU::getModeCycle(Nibble mode)
{
	unsigned cycle = 0;
	if(mode & 0b1000)
	{
		cycle += 2;
	}
	switch(static_cast<PortMode>(mode & 0b0111))
	{
		case PortMode::P:
		case PortMode::S:
		case PortMode::M: cycle += 1; break;
		//
		case PortMode::T:
		case PortMode::H: cycle += 2; break;
		//
		case PortMode::F:
		case PortMode::R:
		default: break;
	}
	return cycle;
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

unsigned CPU::getInstructionCycle(Word instruction, Nibble A, Nibble B)
{
	unsigned size = getInstructionSize(instruction, A, B);
	unsigned cycle = 0;
	if(size >= 1)
	{
		switch(instruction)
		{
			case NOP:
			case JMP:
			case RET:
			case PNC:
			case MOV:
			case NOT:
			case OR :
			case AND:
			case XOR:
			case RSF:
			case LSF: cycle += 1; break;
			//
			case CLL:
			case CPY:
			case SWP:
			case IEQ:
			case INQ:
			case IGT:
			case ILT:
			case IGQ:
			case ILQ:
			case SND: cycle += 2; break;
			//
			case ADD:
			case SUB: cycle += 3; break;
			//
			case MUL:
			case DIV: cycle += 8; break;
		}
	}
	if(size >= 2)
	{
		cycle += getModeCycle(A);
	}
	if(size >= 3)
	{
		cycle += getModeCycle(B);
	}
	return cycle;
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
		case CLL: useA = true; useB = false; break;
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

