#include <stdexcept>
#include <string>
//
#include "vrwm.hpp"

Vrwm::Vrwm(std::size_t size)
{
    memory.reserve(size);
    for(std::size_t i = 0; i < size; ++i)
    {
        memory.push_back(0x0000);
    }
}

u16 Vrwm::debug_read(u16 addr)
{
    return memory.at(addr);
}

void Vrwm::single_iteration()
{
    Command command = static_cast<Command>(receive());
    switch(command)
    {
        case INFO:
        {
            send(memory.size());
        }; break;

        case READ:
        {
            u16 addr = receive();
            send(memory.at(addr));
        }; break;

        case WRITE:
        {
            u16 addr = receive();
            u16 val  = receive();
            memory.at(addr) = val;
        }; break;

        default: throw std::runtime_error("illegal vrwm command " + std::to_string(command));
    }
}

