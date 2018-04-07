#pragma once

#include <string>
#include <cstdint>
//
#include <architecture.hpp>

struct Mode
{
    enum Type : unsigned
    {
        M = 00,
        R = 01,
        A = 02,
        B = 03,
        P = 04,
        N = 05,
        C = 06,
    };

    enum Dir : unsigned
    {
        D = 0b0,
        I = 0b1,
    };

    Type type : 3;
    Dir  dir  : 1;

    explicit Mode(uint8_t value);
    Mode(Dir dir, Type type);

    unsigned getCycleCost() const;
    bool takesWord() const;
};

struct Opcode
{
    enum Type : unsigned
    {
        NOP = 0x00,
        JMP = 0x01,
        TRM = 0x02,
        CLL = 0x03,
        RET = 0x04,

        MOV = 0x10,
        CPY = 0x11,
        SWP = 0x12,

        IEQ = 0x20,
        INQ = 0x21,
        IGT = 0x22,
        ILT = 0x23,
        IGQ = 0x24,
        ILQ = 0x25,

        NEG = 0x30,
        OR  = 0x31,
        AND = 0x32,
        XOR = 0x33,
        RSH = 0x34,
        LSH = 0x35,
        SPB = 0x36,

        ADD = 0x40,
        SUB = 0x41,
        MUL = 0x42,
        DIV = 0x43,
        MOD = 0x44,

        AIB = 0x50,
        AOB = 0x51,
        AIW = 0x52,
        AOW = 0x53,
    };

    Type type : 8;

    explicit Opcode(uint8_t value);
    Opcode(Type type);

    unsigned getCycleCost() const;
    unsigned getModeSize() const;
};

struct Instruction
{
    Opcode opcode;
    Mode   a;
    Mode   b;
    Word   x;
    Word   y;

    explicit Instruction(uint16_t value);
    Instruction(Opcode opcode, Mode a, Mode b, Word x, Word y);

    unsigned getCycleCost() const;
    unsigned getWordSize() const;
};
