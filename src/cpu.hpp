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

    CPU(Word romSize);

    void start();

    void connectHardware(Word channel, Hardware &hardware);

    void disconnectHardware(Word channel);
    private:
    Word static const ROM_CHANNEL = 0xFF;
    Word static const MEM_CHANNEL = 0x00;
    Word static const HW_COUNT    = 0x100;
    Word static const R_COUNT     = 0x10;

    // Memory Map
    Word static const STACK_START = 0x0000;
    Word static const ROM_START   = 0x0100;
    Word static const FREE_START  = 0x0200;

    Word static const ITR = 0x0;
    Word static const ARR = 0x1;
    Word static const SPR = 0xC;
    Word static const OPR = 0xD;
    Word static const SAR = 0xE;
    Word static const SBR = 0xF;

    void debugScreen();

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

    utility::Clock clock;

    Hardware *HW[HW_COUNT];
    DummyDevice dummyDevice;

    Word R[R_COUNT];
    Word PC;
    Byte SP;
    Bit C;

    unsigned cycle;
    double clockFrequency;
    Word romSize;
    bool jumped;
    bool debugChangedR[R_COUNT];
};
