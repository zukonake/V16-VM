#pragma once

#include <cstddef>
#include <array>
//
#include <hw/adp_device.hpp>

class Rom : public AdpDevice
{
    public:
    Word static const MAX_SIZE = 0x100;

    Rom(Word *data, std::size_t size);

    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;
    private:
    std::array<Word, MAX_SIZE> memory;
};
