#pragma once

#include <iostream>
#include <cstddef>
#include <stdexcept>
#include <string>
//
#include <hw/adp_device.hpp>

enum VrwmCommand
{
    READ  = 0x0000,
    WRITE = 0x0001
};

template<std::size_t size>
class Vrwm : public AdpDevice
{
    enum State
    {
        AWAITING_COMMAND,
        AWAITING_READ_ADDRESS,
        AWAITING_WRITE_ADDRESS,
        AWAITING_VALUE
    };
    public:
    Vrwm();
    protected:
    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;

    private:
    void handleAdp(Word value);
    void checkAddress(Word value);

    State       state;
    Word        address;

    Word memory[size];
};

template<std::size_t size>
Vrwm<size>::Vrwm() :
    state(AWAITING_COMMAND),
    address(0x0000)
{
    for(std::size_t i = 0; i < size; ++i) memory[i] = 0x0000;
}

template<std::size_t size>
void Vrwm<size>::handleAdpI8(Byte value)
{
    handleAdp(static_cast<Byte>(value));
    //TODO output must be byte
}

template<std::size_t size>
void Vrwm<size>::handleAdpI16(Word value)
{
    handleAdp(value);
}

template<std::size_t size>
void Vrwm<size>::handleAdp(Word value)
{
    switch(state)
    {
        case AWAITING_COMMAND:
            switch(static_cast<VrwmCommand>(value))
            {
                case READ:  state = AWAITING_READ_ADDRESS;  break;
                case WRITE: state = AWAITING_WRITE_ADDRESS; break;
            } break;

        case AWAITING_READ_ADDRESS:
            checkAddress(value);
            state = AWAITING_COMMAND;
            address = value;
            output = memory[address];
            state = AWAITING_COMMAND;
            break;

        case AWAITING_WRITE_ADDRESS:
            checkAddress(value);
            state = AWAITING_VALUE;
            address = value;
            break;

        case AWAITING_VALUE:
            memory[address] = value;
            state = AWAITING_COMMAND;
            break;
    }
}

template<std::size_t size>
void Vrwm<size>::checkAddress(Word value)
{
    if(address >= size)
    {
        throw std::out_of_range("vrwm address out of range: " + std::to_string(address));
    }
}
