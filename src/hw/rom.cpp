#include <stdexcept>
#include <string>
//
#include "rom.hpp"

Rom::Rom(Word *data, std::size_t size)
{
    if(size > MAX_SIZE)
    {
        throw std::runtime_error("rom size too high: " + std::to_string(size));
    }
    for(Word i = 0; i < size; ++i) memory[i] = data[i];
}

void Rom::handleAdpI8(Byte value)
{
    byte_out = memory.at(value);
}

void Rom::handleAdpI16(Word value)
{
    word_out = memory.at(value);
}
