#pragma once

#include <cstddef>
#include <string>
#include <vector>
//
#include <hw/adp_device.hpp>


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
    enum Command
    {
        READ  = 0x0000,
        WRITE = 0x0001
    };

    Vrwm(std::size_t size);

    Word debugRead(Word address);
    protected:
    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;
    private:
    void handleAdp(Word value, bool byte);

    State       state;
    Word        address;

    std::vector<Word> memory;
};
