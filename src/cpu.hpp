#pragma once

#include <string>
#include <ratio>
//
#include <v16/architecture.hpp>
#include <v16/instruction.hpp>
#include <hardware.hpp>
#include <memory.hpp>
#include <clock.hpp>

class CPU
{
	public:
	CPU();

	void start();

	void connectMemory(Hardware &memory);
	void connectRom(Hardware &rom);
	void connectHardware(Byte channel, Hardware &hardware);

	void disconnectHardware(Byte channel);
	private:
	void copyRom();
	void loop();
	void execute();

	Word &fetchValue(Mode mode, Word address);

	bool running;

	utility::Clock<float, std::ratio<1, 20>> clock;

	Hardware *M;
	Hardware *HW[0x100];
	DummyHardware dummyHardware;

	Word R[0x10];
	Word PC;
	Nibble F;

	Word S[0x100];
	Byte SP;

	Word mutable dummy;
	unsigned cycle;
};
