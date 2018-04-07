#include <cstdlib>
#include <iostream>
//
#include "cpu.hpp"
#include <hw/vrwm.hpp>

CPU::CPU(Word romSize, bool log) :
    PC(0x0000),
    SP(0x00),
    C(0x0),
    cycle(1),
    log(log),
    romSize(romSize),
    jumped(false)
{
    for(Word i = 0x0; i < S_COUNT;  i++) S[i] = 0x0000;
    for(Word i = 0x0; i < R_COUNT;  i++) R[i] = 0x0000;
    for(Word i = 0x0; i < HW_COUNT; i++) connectHardware(i, dummyDevice);
}

void CPU::start()
{
    copyRom();
    running = true;
    loop();
}

void CPU::connectHardware(Word channel, Hardware &hardware)
{
    HW[channel] = &hardware;
}

void CPU::disconnectHardware(Word channel)
{
    HW[channel] = &dummyDevice;
}

void CPU::loop()
{
    while(running)
    {
        execute(fetch());
        if(jumped)
        {
            jumped = false;
        }
        else
        {
            skipInstruction();
        }
    }
}

void CPU::execute(Instruction instr)
{
    while(cycle >= 1)
    {
        clock.start();
        clock.stop();
        clock.synchronize();
        cycle--;
    }
    cycle = instr.getCycleCost();
    std::cout << std::hex
              << " OP: " << instr.opcode.type
              << " A: " << instr.a.type
              << " B: " << instr.b.type
              << " X: " << instr.x
              << " Y: " << instr.y << std::endl;

    auto rA = [&]() { return readMode(instr.a, instr.x); };
    auto rB = [&]() { return readMode(instr.b, instr.y); };
    auto wA = [&](Word val) { writeMode(instr.a, instr.x, val); };
    auto wB = [&](Word val) { writeMode(instr.b, instr.y, val); };

    Word temp;
    switch(instr.opcode.type)
    {
        case Opcode::NOP:                                                    break;
        case Opcode::JMP: PC = rA(); jumped = true;                          break;
        case Opcode::TRM: running = false;                                   break;
        case Opcode::CLL: S[++SP] = PC; PC = rA();                           break;
        case Opcode::RET: PC = S[SP--];                                      break;

        case Opcode::MOV: wB(rA()); wA(std::rand() % 0x10000);               break;
        case Opcode::CPY: wB(rA());                                          break;
        case Opcode::SWP: temp = rA(); wA(rB()); wB(temp);                   break;

        case Opcode::IEQ: if(rA() == rB()) skipInstruction();                break;
        case Opcode::INQ: if(rA() != rB()) skipInstruction();                break;
        case Opcode::IGT: if(rA() >  rB()) skipInstruction();                break;
        case Opcode::ILT: if(rA() <  rB()) skipInstruction();                break;
        case Opcode::IGQ: if(rA() >= rB()) skipInstruction();                break;
        case Opcode::ILQ: if(rA() <= rB()) skipInstruction();                break;

        case Opcode::NEG: wB(~rA());                                         break;
        case Opcode::OR:  wB(rB() | rA());                                   break;
        case Opcode::AND: wB(rB() & rA());                                   break;
        case Opcode::XOR: wB(rB() ^ rA());                                   break;
        case Opcode::RSH: wB(rB() >> rA());                                  break;
        case Opcode::LSH: wB(rB() << rA());                                  break;
        case Opcode::SPB: wA(((rA() & 0xFF00) >> 8) | ((rA() & 0xFF) << 8)); break;

        case Opcode::ADD: C = rA() > 0x10000 - rB(); wB(rB() + rA());        break;
        case Opcode::SUB: C = rA() > rB(); wB(rB() - rA());                  break;
        case Opcode::MUL: C = rA() > 0x10000 / rB(); wB(rB() * rA());        break;
        case Opcode::DIV: wB(rB() / rA());                                   break;
        case Opcode::MOD: wB(rB() % rA());                                   break;

        case Opcode::AIB: HW[rB()]->adpI8(rA());                             break;
        case Opcode::AOB: wB(HW[rA()]->adpO8());                             break;
        case Opcode::AIW: HW[rB()]->adpI16(rA());                            break;
        case Opcode::AOW: wB(HW[rA()]->adpO16());                            break;
        default: throw std::runtime_error("illegal instruction");
    }
}

Word CPU::readMode(Mode mode, Word address)
{
    Word result;
    switch(mode.type)
    {
        case Mode::M: result = address;      break;
        case Mode::R: result = R[address];   break;
        case Mode::A: result = R[address]++; break;
        case Mode::B: result = ++R[address]; break;
        case Mode::P: result = PC + address; break;
        case Mode::N: result = PC - address; break;
        case Mode::C: result = C;            break;
    }
    if(mode.dir == Mode::I) result = memRead(address);
    return result;
}

void CPU::writeMode(Mode mode, Word address, Word value)
{
    if(mode.dir == Mode::I)
    {
        address   = readMode({Mode::D, mode.type}, address);
        mode.type = Mode::M;
    }
    switch(mode.type)
    {
        case Mode::M: memWrite(address, value); break;
        case Mode::R: R[address] =   value; break;
        case Mode::A: R[address] = ++value; break;
        case Mode::B: R[address] =   value; break;
        case Mode::P: break;
        case Mode::N: break;
        case Mode::C: break;
    }
}

void CPU::skipInstruction()
{
    PC += fetch().getWordSize();
}

Instruction CPU::fetch()
{
    Word tempPC = PC;
    Word word = memRead(tempPC++);
    Opcode opcode((word & 0xFF00) >> 8);
    Mode        a((word & 0x00F0) >> 4);
    Mode        b (word & 0x000F);

    Word x = 0x0000;
    Word y = 0x0000;

    if(a.takesWord() && opcode.getModeSize() >= 1)
    {
        x = memRead(tempPC++);
    }
    if(b.takesWord() && opcode.getModeSize() >= 2)
    {
        y = memRead(tempPC++);
    }
    return Instruction(opcode, a, b, x, y);
}

void CPU::copyRom()
{
    INLINE(CPY, D, M, 0x0000, D, R, SP2REG);
    while(R[SP2REG] < romSize)
    {
        std::cout << "COPYING ROM " << R[SP2REG] << "/" << romSize - 1 << std::endl;
        INLINE(AIW, D, R, SP2REG     , D, M, ROM_CHANNEL);
        INLINE(AOW, D, M, ROM_CHANNEL, I, A, SP2REG     );
    }
}

Word CPU::memRead(Word address)
{
    INLINE(AIW, D, M, VrwmCommand::READ, D, M, MEM_CHANNEL);
    INLINE(AIW, D, M, address          , D, M, MEM_CHANNEL);
    INLINE(AOW, D, M, MEM_CHANNEL      , D, R, SP1REG     );
    return R[SP1REG];
}

void CPU::memWrite(Word address, Word value)
{
    INLINE(AIW, D, M, VrwmCommand::WRITE, D, M, MEM_CHANNEL);
    INLINE(AIW, D, M, address           , D, M, MEM_CHANNEL);
    INLINE(AIW, D, M, value             , D, M, MEM_CHANNEL);
}
