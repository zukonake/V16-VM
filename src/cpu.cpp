#include <cstdlib>
#include <thread>
//
#include <curses.h>
//
#include "cpu.hpp"
#include <hw/vrwm.hpp>
#include <hw/rom.hpp>

#define VX_INLINE(op, i, ax, b) \
    (exec_instr(((op & 0b11111) << 11) | \
                   ((i  & 0b1    ) << 10) | \
                   ((ax & 0b11111) <<  5) | \
                    (b  & 0b11111), ax))

Cpu::Cpu() :
    state(NOT_STARTED),
    clock_freq(256.0),
    sp(0x0000),
    cycle(0),
    skip(0)
{
    clock.set_cycle(utility::Clock::Duration(1.0 / clock_freq));
    for(u16 i = 0x0; i < REG_COUNT; i++) r[i] = 0x0000;
    for(u16 i = 0x0; i < REG_COUNT; i++) reg_tracking[i] = 0x0000;
    pc = ROM_START;
    fl = 0x0000;
}

void Cpu::start()
{
    switch(state)
    {
        case RUNNING: throw std::runtime_error("cpu is already running");
        case NOT_STARTED: copy_rom(); state = RUNNING; break;
        case STOPPED:                 state = RUNNING; break;
        default: throw std::runtime_error("illegal cpu state");
    }
    execution_thread = std::thread([&] {run();});
}

void Cpu::stop()
{
    switch(state)
    {
        case NOT_STARTED:
        case STOPPED: throw std::runtime_error("cpu is already stopped");
        case RUNNING: state = STOPPED; break;
        default: throw std::runtime_error("illegal cpu state");
    }
    execution_thread = std::thread([&] {run();});
}

void Cpu::await()
{
    switch(state)
    {
        case NOT_STARTED:
        case STOPPED: throw std::runtime_error("cpu is not running");
        case RUNNING: execution_thread.join(); break;
        default: throw std::runtime_error("illegal cpu state");
    }
}

void Cpu::connect_hw(u16 channel, Hw &hw)
{
    hw_conns[channel].connect(hw);
    if(channel == VRWM_CHANNEL) debug_vrwm = dynamic_cast<Vrwm *>(&hw);
}

void Cpu::disconnect_hw(u16 channel)
{
    hw_conns[channel].disconnect();
}

void Cpu::run()
{
    std::thread debug_thread;
    debug_thread = std::thread([&] {debug_screen();});
    while(state == RUNNING)
    {
        u16 new_pc = pc;
        u16 instr  = vrwm_read(pc);
        u16 x      = 0x0000;
        ++new_pc;
        if((instr & 0b0000010000000000) >> 10)
        {
            if(skip == 0) x = vrwm_read(pc + 1);
            ++new_pc;
        }
        if(skip == 0) exec_instr(instr, x);
        else --skip;
        pc = new_pc;
    }
    debug_thread.join();
}

void Cpu::copy_rom()
{
    VX_INLINE(MOV , 1, ROM_START  , A0);

    VX_INLINE(MOV , 1, Rom::INFO  , A1);
    VX_INLINE(HWI , 1, ROM_CHANNEL, A1);
    VX_INLINE(HWO , 1, ROM_CHANNEL, A1);
    VX_INLINE(ADD , 1, ROM_START  , A1);

    VX_INLINE(MOV , 1, Rom::BLIT  , A2);
    VX_INLINE(HWI , 1, ROM_CHANNEL, A2);
    while(r[A0] < r[A1])
    {
        clear();
        printw("COPYING ROM: 0x%02x/0x%02x\n", r[A0], r[A1] - 1);
        refresh();
        VX_INLINE(HWO , 1, ROM_CHANNEL, A3);
        VX_INLINE(SAVE, 0, A3         , A0);
        VX_INLINE(ADD , 1, 1          , A0);
    }
}

void Cpu::exec_instr(u16 instr, u16 x)
{
    u16 op = (instr & 0b1111100000000000) >> 11;
    u16 i  = (instr & 0b0000010000000000) >> 10;
    u16 a  = (instr & 0b0000001111100000) >>  5;
    u16 b  =  instr & 0b0000000000011111;
    u16 ax;
    if(i) ax = x;
    else  ax = r[a];

    while(cycle > 0)
    {
        clock.start();
        clock.stop();
        clock.synchronize();
        --cycle;
    }
    cycle = get_instr_cost(instr);

    #define  OP(op, x) case  op: { x; } break
    #define SKIP_NEXT() (skip += 1 + i)

    switch(op)
    {
        case 000: switch(b)
        {
            OP(NOP ,);
            OP(TERM, state = STOPPED);
            OP(JMP , pc  = ax; skip = 0;);
            OP(JMPF, pc += ax; skip = 0;);
            OP(JMPB, pc -= ax; skip = 0;);
            OP(CALL, vrwm_write(sp++, pc); pc = ax);
            OP(RET , pc = vrwm_read(--sp));
            OP(PUSH, vrwm_write(sp++, ax););
            OP(POP , r[a] = vrwm_read(--sp));
            default: throw std::runtime_error("illegal cpu instruction " + std::to_string(op) +
                                              " " + std::to_string(b));
        }; break;
        OP(MOV,  r[b] = ax);
        OP(LOAD, r[b] = vrwm_read(ax));
        OP(SAVE, vrwm_write(r[b], ax));
        OP(SWAP, u16 temp = r[a];
                 r[a] = r[b];
                 r[b] = temp);

        OP(IFEQ, if(ax == r[b]) SKIP_NEXT());
        OP(IFNQ, if(ax != r[b]) SKIP_NEXT());
        OP(IFGT, if(ax >  r[b]) SKIP_NEXT());
        OP(IFLT, if(ax <  r[b]) SKIP_NEXT());
        OP(IFGQ, if(ax >= r[b]) SKIP_NEXT());
        OP(IFLQ, if(ax <= r[b]) SKIP_NEXT());

        OP(NEG , r[b]   = ~ax);
        OP(OR  , r[b]  |=  ax);
        OP(AND , r[b]  &=  ax);
        OP(XOR , r[b]  ^=  ax);
        OP(RSHF, r[b] >>=  ax);
        OP(LSHF, r[b] <<=  ax);
        OP(SWPB, r[b]   = ((ax & 0xFF00) >> 8) |
                          ((ax & 0x00FF) << 8));

        OP(ADD , fl    = ax > 0x10000 - r[b];
                 r[b] += ax);
        OP(SUB , fl    = ax > r[b];
                 r[b] -= ax);
        OP(MUL , r[b] *= ax);
        OP(DIV , r[b] /= ax);
        OP(MOD , r[b] %= ax);

        OP(HWI , hw_conns[ax].write(r[b]));
        OP(HWO , r[b] = hw_conns[ax].read());

        default: throw std::runtime_error("illegal cpu instruction " + std::to_string(op));
    }
}

u16 Cpu::vrwm_read(u16 address)
{
    VX_INLINE(MOV , 1, Vrwm::READ  , S0);
    VX_INLINE(HWI , 1, VRWM_CHANNEL, S0);
    VX_INLINE(MOV , 1, address     , S0);
    VX_INLINE(HWI , 1, VRWM_CHANNEL, S0);
    VX_INLINE(HWO , 1, VRWM_CHANNEL, S0);
    return r[S0];
}

void Cpu::vrwm_write(u16 address, u16 value)
{
    VX_INLINE(MOV , 1, Vrwm::WRITE , S0);
    VX_INLINE(HWI , 1, VRWM_CHANNEL, S0);
    VX_INLINE(MOV , 1, address     , S0);
    VX_INLINE(HWI , 1, VRWM_CHANNEL, S0);
    VX_INLINE(MOV , 1, value       , S0);
    VX_INLINE(HWI , 1, VRWM_CHANNEL, S0);
}

unsigned Cpu::get_instr_cost(u16 instr)
{
    u16 op = (instr & 0b1111100000000000) >> 11;
    u16 b  =  instr & 0b0000000000011111;
    switch(op)
    {
        case 000: switch(b)
        {
            case NOP:  return 1;
            case TERM: return 1;
            case JMP:  return 1;
            case JMPF: return 2;
            case JMPB: return 2;
            case CALL: return 2;
            case RET:  return 2;
            case PUSH: return 2;
            case POP:  return 2;
            default: throw std::runtime_error("illegal cpu instruction " + std::to_string(op) +
                                             " " + std::to_string(b));
        }; break;
        case MOV:  return 1;
        case LOAD: return 2;
        case SAVE: return 2;
        case SWAP: return 1;
        case IFEQ: return 1;
        case IFNQ: return 1;
        case IFGT: return 2;
        case IFLT: return 2;
        case IFGQ: return 2;
        case IFLQ: return 2;
        case NEG : return 1;
        case OR  : return 1;
        case AND : return 1;
        case XOR : return 1;
        case RSHF: return 1;
        case LSHF: return 1;
        case SWPB: return 1;
        case ADD : return 2;
        case SUB : return 2;
        case MUL : return 3;
        case DIV : return 3;
        case MOD : return 3;
        case HWI : return 2;
        case HWO : return 2;
        default: throw std::runtime_error("illegal cpu instruction " + std::to_string(op));
    }
}

void Cpu::debug_screen()
{
    init_pair(1, COLOR_BLACK,  COLOR_GREEN);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    u16 pos = 0;
    u16 const DISPLAY_LINES = 31;
    while(state == RUNNING)
    {
        clear();
        printw("\n");
        printw("F: %f\n", clock_freq);
        printw("S(%02x): ", sp);
        for(u16 i = 0; i < sp; ++i)
        {
            printw("%04x ", debug_vrwm->debug_read(STACK_START + i));
        }
        printw("\n");
        printw("R: ");
        for(u16 i = 0; i < REG_COUNT; ++i)
        {
            if(reg_tracking[i])
            {
                attron(COLOR_PAIR(2));
                reg_tracking[i] = false;
            }
            printw("%x: %04x", i, r[i]);
            attroff(COLOR_PAIR(2));
            printw(" ");
        }
        printw("\n");
        printw("\n");
        for(u16 i = 0; i < DISPLAY_LINES; ++i)
        {
            u16 current = static_cast<u16>(pos + i);
            if(pc == current) attron(COLOR_PAIR(1));
            printw("%04x: %04x\n", current, debug_vrwm->debug_read(current));
            attroff(COLOR_PAIR(1));
        }
        refresh();
        switch(getch())
        {
            case 's': ++pos;           break;
            case 'w': --pos;           break;
            case 'S': pos += 0x10;     break;
            case 'W': pos -= 0x10;     break;
            case 'q': state = STOPPED; break;
            case 'a':
                clock_freq = std::max(1.0, clock_freq - (1.0 + clock_freq * 0.2));
                clock.set_cycle(utility::Clock::Duration(1.0 / clock_freq));
                break;
            case 'd':
                clock_freq = std::max(1.0, clock_freq + (1.0 + clock_freq * 0.2));
                clock.set_cycle(utility::Clock::Duration(1.0 / clock_freq));
                break;
            default: break;
        }
    }
}

