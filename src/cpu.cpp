#include <iostream>
//
#include "cpu.hpp"

CPU::CPU(bool log) :
	PC(0x0000),
	F(0x0),
	SP(0x00),
	cycle(1),
	log(log)
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

void CPU::connectMemory(Hardware &memory)
{
	disconnectHardware(0x00);
	connectHardware(0x00, memory);
	M = HW[0x00];
}

void CPU::connectRom(Hardware &rom)
{
	disconnectHardware(0xFF);
	connectHardware(0xFF, rom);
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
	Instruction instr((*M)[PC]);
	cycle = instr.getCycleCost();
	unsigned instructionSize = instr.getSize();
	Word &X = fetchValue(instr.A, PC + 1);
	Word &Y = fetchValue(instr.B, PC + 2);
	if(log)
	{
		std::cout << std::hex << "PC: " << PC << "\n";
		if(instructionSize >= 1) std::cout << "\tI: " << (*M)[PC] << "\n";
		if(instructionSize >= 2) std::cout << "\tX: " << X << "\n";
		if(instructionSize >= 3) std::cout << "\tY: " << Y << "\n";
		std::cout << "\n";
	}
	switch(instr.opcode)
	{
		case NOP: break;
		case JMP: PC = X; return;
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
		case IEQ: if(X == Y) skipInstruction(); break;
		case INQ: if(X != Y) skipInstruction(); break;
		case IGT: if(X >  Y) skipInstruction(); break;
		case ILT: if(X <  Y) skipInstruction(); break;
		case IGQ: if(X >= Y) skipInstruction(); break;
		case ILQ: if(X <= Y) skipInstruction(); break;
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
		//
		case API: HW[static_cast<Byte>(Y)]->adpI(X); break;
		case APO: Y = HW[static_cast<Byte>(X)]->adpO(); break;
		case SDP: (*HW[R[0xF]])[X] = Y; break;
		default: throw std::runtime_error("illegal instruction");
	}
	skipInstruction();
}

void CPU::skipInstruction()
{
	Instruction instr((*M)[PC]);
	unsigned instructionSize = instr.getSize();
	PC += instructionSize;
}

Word &CPU::fetchValue(Mode mode, Word address)
{
	Word *output;
	Word &value = (*M)[address];
	switch(mode.type)
	{
		case Mode::Type::M: output = &value; break;
		case Mode::Type::R: output = &R[value & 0x000F]; break;
		case Mode::Type::H: output = &(*HW[(value & 0xFF00) >> 8])[value & 0x00FF]; break;
		case Mode::Type::P: output = &dummy; dummy = PC; break;
		case Mode::Type::S: output = &dummy; dummy = PC - value; break;
		case Mode::Type::A: output = &dummy; dummy = PC + value; break;
		case Mode::Type::F: output = reinterpret_cast<Word *>(&F); break;
		case Mode::Type::I: output = &dummy; dummy = R[value & 0x000F]; R[value & 0x000F]++; break;
		default: throw std::runtime_error("illegal mode");
	}
	if(mode.indirect) return (*M)[*output];
	else return *output;
}

