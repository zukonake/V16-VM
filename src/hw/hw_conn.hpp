#pragma once

#include <int.hpp>

class Hw;

class HwConn
{
    public:
    HwConn();

    void write(u16 val);
    u16  read();

    void    connect(Hw &new_hw);
    void disconnect();

    Hw &get();
    private:
    Hw *hw;
};
