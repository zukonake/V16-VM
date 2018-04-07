#pragma once

#include <hw/adp_device.hpp>

class Rom : public AdpDevice
{
    public:
    Word static const MAX_SIZE = 0x100;

    Rom(Word *data, Word size);

    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;
    private:
    static void checkAddress(Word address);

    Word memory[MAX_SIZE];
};
