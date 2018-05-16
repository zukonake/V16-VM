#pragma once

#include <thread>
//
#include <int.hpp>
#include <clock.hpp>
#include <hw/hw_conn.hpp>

class Vrwm;

class Cpu
{
    public:
    Cpu();

    void start();
    void stop();
    void await();

    void connect_hw   (u16 channel, Hw &hw);
    void disconnect_hw(u16 channel);
    private:
    enum AReg //register aliases
    {
        A0 = 0x00,
        A1 = 0x01,
        A2 = 0x02,
        A3 = 0x03,
        A4 = 0x04,
        A5 = 0x05,
        A6 = 0x06,
        A7 = 0x07,

        B0 = 0x08,
        B1 = 0x09,
        B2 = 0x0A,
        B3 = 0x0B,
        B4 = 0x0C,
        B5 = 0x0D,
        B6 = 0x0E,
        B7 = 0x0F,

        C0 = 0x10,
        C1 = 0x11,
        C2 = 0x12,
        C3 = 0x13,
        C4 = 0x14,
        C5 = 0x15,
        C6 = 0x16,
        C7 = 0x17,

        D0 = 0x18,
        D1 = 0x19,
        D2 = 0x1A,
        D3 = 0x1B,

        S0 = 0x1C,
        PC = 0x1D,
        FL = 0x1E,
        CP = 0x1F
    };

    enum SpecialOpcode
    {
        NOP  = 000,
        TERM = 001,
        JMP  = 002,
        JMPF = 003,
        JMPB = 004,
        CALL = 005,
        RET  = 006,
        PUSH = 007,
        POP  = 010
    };

    enum Opcode
    {
        MOV  = 001,
        LOAD = 002,
        SAVE = 003,
        SWAP = 004,
        IFEQ = 005,
        IFNQ = 006,
        IFGT = 007,
        IFLT = 010,
        IFGQ = 011,
        IFLQ = 012,
        NEG  = 013,
        OR   = 014,
        AND  = 015,
        XOR  = 016,
        RSHF = 017,
        LSHF = 020,
        SWPB = 021,
        ADD  = 022,
        SUB  = 023,
        MUL  = 024,
        DIV  = 025,
        MOD  = 026,
        HWI  = 027,
        HWO  = 030
    };

    enum State
    {
        NOT_STARTED,
        RUNNING,
        STOPPED
    };

    u16 static const ROM_CHANNEL  = 0xFF;
    u16 static const VRWM_CHANNEL = 0x00;
    u16 static const HW_COUNT     = 0x100;
    u16 static const REG_COUNT    = 0x20;

    // Memory Map
    u16 static const STACK_START = 0x0000;
    u16 static const ROM_START   = 0x0100;
    u16 static const FREE_START  = 0x0200;

    void run();
    void copy_rom();
    void exec_instr(u16 instr, u16 x);

    u16  vrwm_read (u16 address);
    void vrwm_write(u16 address, u16 value);

    static unsigned get_instr_cost(u16 instr);
    void debug_screen();

    State          state;
    std::thread    execution_thread;
    utility::Clock clock;
    double   clock_freq;

    u16 r[REG_COUNT]; //accessible registers
    u16 &fl = r[FL];
    u16 &pc = r[PC];
    u16 sp;
    HwConn hw_conns[HW_COUNT];

    unsigned cycle;
    unsigned skip;  //how many instructions to skip

    Vrwm *debug_vrwm;
    bool reg_tracking[REG_COUNT]; //used for debug
};
