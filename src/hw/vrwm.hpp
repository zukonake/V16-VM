#pragma once

#include <cstddef>
#include <string>
#include <vector>
//
#include <int.hpp>
#include <hw.hpp>


class Vrwm : public Hw
{
    public:
    enum Command
    {
        INFO  = 0x0000,
        READ  = 0x0001,
        WRITE = 0x0002
    };

    Vrwm(std::size_t size);

    u16 debug_read(u16 address);

    private:
    virtual void single_iteration() override;

    std::vector<u16> memory;
};
