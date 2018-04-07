#include "adp_device.hpp"

void AdpDevice::adpI8(Byte value)
{
    handleAdpI8(value);
}

void AdpDevice::adpI16(Word value)
{
    handleAdpI16(value);
}

Byte AdpDevice::adpO8()
{
    return byte_out;
}

Word AdpDevice::adpO16()
{
    return word_out;
}
