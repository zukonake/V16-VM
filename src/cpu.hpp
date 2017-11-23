#pragma once

#include <string>
#include <ratio>
//
#include <architecture.hpp>
#include <hardware.hpp>
#include <memory.hpp>
#include <clock.hpp>

class CPU
{
	public:
	CPU(MemoryInterface &memory, std::string const &eepromPath);

	void start();

	void connectHardware(Byte channel, HardwareInterface &hardware);
	private:
	void loop();
	void execute();

	static unsigned getModeSize(Nibble mode);
	static unsigned getInstructionSize(Word instruction, Nibble A, Nibble B);
	Word &fetchValue(Nibble mode, Word address);

	bool running;

	utility::Clock<float, std::ratio<1, 10>> clock;

	MemoryInterface *M;
	HardwareInterface *HW[0x100];

	Word R[0xF];
	Word PC;
	Nibble F;

	Word S[0x100];
	Byte SP;
};

enum class PortMode
{
	M = 00,
	R = 01,
	H = 02,
	P = 03,
	T = 04,
	S = 05,
	F = 06,
};

enum Instruction
{
	// SIMPLE
	NOP = 0x00,
	JMP = 0x01,
	CLL = 0x02,
	RET = 0x03,
	// ..
	PNC = 0x0F,
	// IO
	MOV = 0x10,
	CPY = 0x11,
	SWP = 0x12,
	// CONDITIONAL
	IEQ = 0x20,
	INQ = 0x21,
	IGT = 0x22,
	ILT = 0x23,
	IGQ = 0x24,
	ILQ = 0x25,
	// BINARY
	NOT = 0x30,
	OR  = 0x31,
	AND = 0x32,
	XOR = 0x33,
	RSF = 0x34,
	LSF = 0x35,
	// ARITHMETIC
	ADD = 0x40,
	SUB = 0x41,
	MUL = 0x42,
	DIV = 0x43,
};

