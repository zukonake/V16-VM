#include <stdexcept>
#include <string>
//
#include "rom.hpp"

Rom::Rom(u16 *data, std::size_t size) :
    memory(data, data + size)
{

}

void Rom::single_iteration()
{
    Command command = static_cast<Command>(receive());
    switch(command)
    {
        case INFO:
        {
            send(memory.size());
        }; break;

        case AT:
        {
            u16 addr = receive();
            send(memory.at(addr));
        }; break;

        case BLIT:
        {
            for(u16 i = 0; i < memory.size(); ++i) send(memory.at(i));
        }; break;

        default: throw std::runtime_error("illegal rom command " + std::to_string(command));
    }
}
