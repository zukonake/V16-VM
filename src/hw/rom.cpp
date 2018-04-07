#include <iostream>
#include <stdexcept>
#include <string>
//
#include "rom.hpp"

Rom::Rom(Word *data, Word size)
{
    if(size > MAX_SIZE)
    {
        throw std::runtime_error("rom size too high: " + std::to_string(size));
    }
    for(Word i = 0; i < size; ++i) memory[i] = data[i];
}

void Rom::handleAdpI8(Byte value)
{
    checkAddress(value);
    output = memory[value];
}

void Rom::handleAdpI16(Word value)
{
    checkAddress(value);
    output = memory[value];
}

void Rom::checkAddress(Word address)
{
    if(address >= MAX_SIZE)
    {
        throw std::out_of_range("rom address out of range: " + std::to_string(address));
    }
}

