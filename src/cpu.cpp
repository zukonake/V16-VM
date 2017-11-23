#include <fstream>
#include <iostream>
//
#include "cpu.hpp"

CPU::CPU(MemoryInterface &memory) :
	M(&memory),
	PC(0x0000),
	F(0x0),
	SP(0x00)
{
	for(Word i = 0x00; i < 0x100; i++)
	{
		S[i] = 0x0000;
	}
	for(Word i = 0x0; i < 0x10; i++)
	{
		R[i] = 0x0000;
	}
	Word EEPROM[0x100];
	std::ifstream file("eeprom.bin", std::ios::in | std::ios::binary);
	if(file.is_open())
	{
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char *>(EEPROM), 0x200);
		file.close();
	}
	else
	{
		//TODO except
	}
	for(Word i = 0x0; i < 0x10; i++)
	{
		(*M)[i] = EEPROM[i];
	}
}

void CPU::start()
{
	running = true;
	loop();
}

void CPU::connectHardware(Byte channel, HardwareInterface *hardware)
{
	HW[channel] = hardware;
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
	Byte I = (*M)[PC] >> 8;
	Nibble A = ((*M)[PC] & 0x00F0) >> 4;
	Nibble B = ((*M)[PC] & 0x000F);
	Word &X = fetchValue(A, PC + 1);
	Word &Y = fetchValue(B, PC + 2);
	std::cout << std::hex
		<<  "PC: " << PC << "\n"
		<< "\tI: " << static_cast<unsigned>(I) << "\n"
		<< "\tA: " << static_cast<unsigned>(A) << "\n"
		<< "\tB: " << static_cast<unsigned>(B) << "\n"
		<< "\tX: " << X << "\n"
		<< "\tY: " << Y << "\n\n";
	if(I >= 0x20 && I < 0x30 && (F & static_cast<Nibble>(Flags::C)) != 0)
	{
		conditionalTrigger();
	}
	else
	{
		if((F & static_cast<Nibble>(Flags::C)) != 0)
		{
			F &= 0b1101; //remove C flag
		}
		switch(I)
		{
			case NOP: break;
			case JMP: PC = X; break;
			case CLL:
				S[++SP] = PC;
				PC = X;
				break;
			case RET:
				PC = S[SP--];
				break;
			case PNC: F |= static_cast<Nibble>(Flags::P); break;
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
			case IEQ: if(X == Y) conditionalTrigger(); break;
			case INQ: if(X != Y) conditionalTrigger(); break;
			case IGT: if(X >  Y) conditionalTrigger(); break;
			case ILT: if(X <  Y) conditionalTrigger(); break;
			case IGQ: if(X >= Y) conditionalTrigger(); break;
			case ILQ: if(X <= Y) conditionalTrigger(); break;
			//
			case NOT: X = !X; break;
			case OR : Y |= X; break;
			case AND: Y &= X; break;
			case XOR: Y ^= X; break;
			case RSF: Y >>= X; break;
			case LSF: Y <<= X; break;
			//
			case ADD: Y += X; break; //TODO check overflow
			case SUB: Y -= X; break; //TODO check overflow
			case MUL: Y *= X; break; //TODO check overflow
			case DIV: Y /= X; break; //TODO check overflow
			default: break; //TODO except
		}
	}
	if(I != JMP)
	{
		++PC;
	}
}

void CPU::conditionalTrigger()
{
	++PC;
	F |= static_cast<Nibble>(Flags::C);
}

Word &CPU::fetchValue(Nibble mode, Word address)
{
	bool indirect = (mode & 0b1000) >> 3;
	mode = (mode & 0b0111);
	if(indirect && mode > 04)
	{
		//TODO except?
	}
	Word *output;
	Word value = (*M)[address];
	switch(static_cast<PortMode>(mode))
	{
		case PortMode::M: output = &value; break;
		case PortMode::R: output = &R[value & 0x00FF]; break;
		case PortMode::H: output = &(*HW)[value & 0xFF00][value & 0x00FF]; break;
		case PortMode::P: output = &PC; break;
		case PortMode::T: output = &S[SP]; break;
		case PortMode::S: output = reinterpret_cast<Word *>(&SP); break;
		case PortMode::F: output = reinterpret_cast<Word *>(&F); break;
		default: break; //TODO except?
	}
	if(indirect) return (*M)[*output];
	else return *output;
}

