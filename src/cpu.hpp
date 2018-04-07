#pragma once

#include <ratio>
//
#include <architecture.hpp>
#include <instruction.hpp>
#include <hw/adp_device.hpp>
#include <hw/dummy.hpp>
#include <clock.hpp>

#define INSTR(o, ad, ak, x, bd, bk, y) {{Opcode::o}, {Mode::ad, Mode::ak}, {Mode::bd, Mode::bk}, x, y}
#define INLINE(o, ad, ak, x, bd, bk, y) execute(INSTR(o, ad, ak, x, bd, bk, y))

class CPU
{
    public:
    typedef AdpDevice Hardware;

    CPU(Word romSize, bool log = true);

    void start();

    void connectHardware(Word channel, Hardware &hardware);

    void disconnectHardware(Word channel);
    private:
    Word static const ROM_CHANNEL = 0xFF;
    Word static const MEM_CHANNEL = 0x00;
    Word static const HW_COUNT    = 0x100;
    Word static const ROM_SIZE    = 0x100;
    Word static const S_COUNT     = 0x100;
    Word static const R_COUNT     = 0x10;

    Word static const ITEREG = 0x0;
    Word static const ARHREG = 0x1;
    Word static const OPEREG = 0xD;
    Word static const SP1REG = 0xE;
    Word static const SP2REG = 0xF;

    void skipInstruction();
    void loop();
    void execute(Instruction instr);
    Word readMode (Mode mode, Word address);
    void writeMode(Mode mode, Word address, Word value);

    Instruction fetch();

    void copyRom();
    Word memRead(Word address);
    void memWrite(Word address, Word value);

    bool running;

    utility::Clock<float, std::ratio<1, 10>> clock;

    Hardware *HW[HW_COUNT];
    DummyDevice dummyDevice;

    Word R[R_COUNT];
    Word S[S_COUNT];
    Word PC;
    Byte SP;
    Bit C;

    unsigned cycle;
    bool log;
    Word romSize;
    bool jumped;
};
