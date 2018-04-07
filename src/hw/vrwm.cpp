#include "vrwm.hpp"

Vrwm::Vrwm(std::size_t size) :
    state(AWAITING_COMMAND),
    address(0x0000)
{
    memory.reserve(size);
    for(std::size_t i = 0; i < size; ++i)
    {
        memory.push_back(0x0000);
    }
}

Word Vrwm::debugRead(Word address)
{
    return memory.at(address);
}

void Vrwm::handleAdpI8(Byte value)
{
    handleAdp(value, true);
}

void Vrwm::handleAdpI16(Word value)
{
    handleAdp(value, false);
}

void Vrwm::handleAdp(Word value, bool byte)
{
    switch(state)
    {
        case AWAITING_COMMAND:
            switch(static_cast<Command>(value))
            {
                case READ:  state = AWAITING_READ_ADDRESS;  break;
                case WRITE: state = AWAITING_WRITE_ADDRESS; break;
            } break;

        case AWAITING_READ_ADDRESS:
            state = AWAITING_COMMAND;
            address = value;
            if(byte) byte_out = memory.at(address);
            else     word_out = memory.at(address);
            state = AWAITING_COMMAND;
            break;

        case AWAITING_WRITE_ADDRESS:
            state = AWAITING_VALUE;
            address = value;
            break;

        case AWAITING_VALUE:
            memory.at(address) = value;
            state = AWAITING_COMMAND;
            break;
    }
}

