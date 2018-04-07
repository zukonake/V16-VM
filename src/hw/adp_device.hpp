#pragma once

#include <cstddef>
//
#include <architecture.hpp>

class AdpDevice
{
    public:
    virtual ~AdpDevice() = default;

    void adpI8(Byte value);
    void adpI16(Word value);
    Byte adpO8();
    Word adpO16();
    protected:
    virtual void handleAdpI8(Byte value) = 0;
    virtual void handleAdpI16(Word value) = 0;

    Word word_out;
    Word byte_out;
};
