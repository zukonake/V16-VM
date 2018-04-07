#include "dummy.hpp"

void DummyDevice::handleAdpI8(Byte value)
{
    output = value;
}

void DummyDevice::handleAdpI16(Word value)
{
    output = value;
}
