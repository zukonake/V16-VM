#include <cstdlib>
#include <thread>
//
#include <curses.h>
//
#include "cpu.hpp"
#include <hw/vrwm.hpp>

CPU::CPU(Word romSize) :
    PC(0x0000),
    SP(0x00),
    C(0x0),
    cycle(1),
    clockFrequency(100.0),
    romSize(romSize),
    jumped(false)
{
    clock.setCycle(utility::Clock::Duration(1.0 / clockFrequency));
    for(Word i = 0x0; i < S_COUNT;  i++) S[i] = 0x0000;
    for(Word i = 0x0; i < R_COUNT;  i++) R[i] = 0x0000;
    for(Word i = 0x0; i < HW_COUNT; i++) connectHardware(i, dummyDevice);
    for(Word i = 0x0; i < R_COUNT;  i++) debugChangedR[i] = 0x0000;
}

void CPU::debugScreen()
{
    init_pair(1, COLOR_BLACK,  COLOR_GREEN);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    Word pos = 0;
    bool run = true;
    Word const LINES = 31;
    while(run)
    {
        clear();
        printw("F: %f\n", clockFrequency);
        printw("SP: %02x\n", SP);
        printw("R: ");
        for(Word it = 0; it < R_COUNT; ++it)
        {
            if(debugChangedR[it])
            {
                attron(COLOR_PAIR(2));
                debugChangedR[it] = false;
            }
            printw("%04x", R[it]);
            attroff(COLOR_PAIR(2));
            printw(" ");
        }
        printw("\n");
        printw("\n");
        for(Word it = 0; it < LINES; ++it)
        {
            Word current = static_cast<Word>(pos + it);
            if(PC == current) attron(COLOR_PAIR(1));
            printw("%04x: %04x\n", current,
                    reinterpret_cast<Vrwm *>(HW[MEM_CHANNEL])->debugRead(current));
            attroff(COLOR_PAIR(1));
        }
        refresh();
        switch(getch())
        {
            case 's': ++pos; break;
            case 'w': --pos; break;
            case 'q': run = false; running = false; break;
            case 'a':
                clockFrequency = std::max(1.0, clockFrequency - (1.0 + clockFrequency * 0.2));
                clock.setCycle(utility::Clock::Duration(1.0 / clockFrequency));
                break;
            case 'd':
                clockFrequency = std::max(1.0, clockFrequency + (1.0 + clockFrequency * 0.2));
                clock.setCycle(utility::Clock::Duration(1.0 / clockFrequency));
                break;
            default:         break;
        }
    }
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
    std::thread debugThread;
    debugThread = std::thread([&] {debugScreen();});
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
    debugThread.join();
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

    auto rA = [&]() { return readMode(instr.a, instr.x); };
    auto rB = [&]() { return readMode(instr.b, instr.y); };
    auto wA = [&](Word val) { writeMode(instr.a, instr.x, val); };
    auto wB = [&](Word val) { writeMode(instr.b, instr.y, val); };

    Word temp;
    switch(instr.opcode.type)
    {
        case Opcode::NOP:                                                     break;
        case Opcode::JUMP: PC = rA(); jumped = true;                          break;
        case Opcode::TERM: running = false;                                   break;
        case Opcode::CALL: S[SP++] = PC; PC = rA(); jumped = true;            break;
        case Opcode::RET:  PC = S[--SP];                                      break;
        case Opcode::PUSH: S[SP++] = rA();                                    break;
        case Opcode::POP:  wA(S[--SP]);                                       break;

        case Opcode::MOVE: wB(rA()); wA(std::rand() % 0x10000);               break;
        case Opcode::COPY: wB(rA());                                          break;
        case Opcode::SWAP: temp = rA(); wA(rB()); wB(temp);                   break;

        case Opcode::IFEQ: if(rA() == rB()) skipInstruction();                break;
        case Opcode::IFNQ: if(rA() != rB()) skipInstruction();                break;
        case Opcode::IFGT: if(rA() >  rB()) skipInstruction();                break;
        case Opcode::IFLT: if(rA() <  rB()) skipInstruction();                break;
        case Opcode::IFGQ: if(rA() >= rB()) skipInstruction();                break;
        case Opcode::IFLQ: if(rA() <= rB()) skipInstruction();                break;

        case Opcode::NEG:  wB(~rA());                                         break;
        case Opcode::OR:   wB(rB() | rA());                                   break;
        case Opcode::AND:  wB(rB() & rA());                                   break;
        case Opcode::XOR:  wB(rB() ^ rA());                                   break;
        case Opcode::RSHF: wB(rB() >> rA());                                  break;
        case Opcode::LSHF: wB(rB() << rA());                                  break;
        case Opcode::SWPB: wA(((rA() & 0xFF00) >> 8) | ((rA() & 0xFF) << 8)); break;

        case Opcode::ADD:  C = rA() > 0x10000 - rB(); wB(rB() + rA());        break;
        case Opcode::SUB:  C = rA() > rB(); wB(rB() - rA());                  break;
        case Opcode::MUL:  C = rA() > 0x10000 / rB(); wB(rB() * rA());        break;
        case Opcode::DIV:  wB(rB() / rA());                                   break;
        case Opcode::MOD:  wB(rB() % rA());                                   break;

        case Opcode::ADBI: HW[rB()]->adpI8(rA());                             break;
        case Opcode::ADBO: wB(HW[rA()]->adpO8());                             break;
        case Opcode::ADWI: HW[rB()]->adpI16(rA());                            break;
        case Opcode::ADWO: wB(HW[rA()]->adpO16());                            break;
        default: throw std::runtime_error("illegal instruction");
    }
}

Word CPU::readMode(Mode mode, Word address)
{
    Word result;
    switch(mode.type)
    {
        case Mode::L: result = address;      break;
        case Mode::R: result = R[address];   break;
        case Mode::A: result = R[address]++; debugChangedR[address] = true; break;
        case Mode::B: result = ++R[address]; debugChangedR[address] = true; break;
        case Mode::P: result = PC + address; break;
        case Mode::N: result = PC - address; break;
        case Mode::C: result = C;            break;
    }
    if(mode.dir == Mode::I) result = memRead(result);
    return result;
}

void CPU::writeMode(Mode mode, Word address, Word value)
{
    if(mode.dir == Mode::I)
    {
        address   = readMode({Mode::D, mode.type}, address);
        mode.type = Mode::L;
    }
    switch(mode.type)
    {
        case Mode::L: memWrite(address, value); break;
        case Mode::R: R[address] =   value; debugChangedR[address] = true; break;
        case Mode::A: R[address] = ++value; debugChangedR[address] = true; break;
        case Mode::B: R[address] =   value; debugChangedR[address] = true; break;
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
    INLINE(COPY, D, L, 0x0000, D, R, SBR);
    while(R[SBR] < romSize)
    {
        clear();
        printw("COPYING ROM: 0x%02x/0x%02x\n", R[SBR], romSize - 1);
        refresh();
        INLINE(ADWI, D, R, SBR        , D, L, ROM_CHANNEL);
        INLINE(ADWO, D, L, ROM_CHANNEL, I, A, SBR        );
    }
}

Word CPU::memRead(Word address)
{
    INLINE(ADWI, D, L, Vrwm::READ  , D, L, MEM_CHANNEL);
    INLINE(ADWI, D, L, address     , D, L, MEM_CHANNEL);
    INLINE(ADWO, D, L, MEM_CHANNEL , D, R, SAR        );
    return R[SAR];
}

void CPU::memWrite(Word address, Word value)
{
    INLINE(ADWI, D, L, Vrwm::WRITE, D, L, MEM_CHANNEL);
    INLINE(ADWI, D, L, address    , D, L, MEM_CHANNEL);
    INLINE(ADWI, D, L, value      , D, L, MEM_CHANNEL);
}
