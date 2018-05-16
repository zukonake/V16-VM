#pragma once

#include <cstddef>
#include <vector>
//
#include <int.hpp>
#include <hw.hpp>

class Rom : public Hw
{
    public:
    enum Command
    {
        INFO = 0x0000,
        AT   = 0x0001,
        BLIT = 0x0002
    };

    Rom(u16 *data, std::size_t size);

    private:
    virtual void single_iteration() override;

    std::vector<u16> memory;
};
