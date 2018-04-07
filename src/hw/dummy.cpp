#include "dummy.hpp"

void DummyDevice::handleAdpI8(Byte value)
{
    byte_out = value;
}

void DummyDevice::handleAdpI16(Word value)
{
    word_out = value;
}
