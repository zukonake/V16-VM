#include <hw.hpp>
//
#include "hw_conn.hpp"

HwConn::HwConn() :
    hw(nullptr)
{

}

void HwConn::write(u16 val)
{
    if(hw != nullptr) hw->write(val);
}

u16 HwConn::read()
{
    if(hw != nullptr) return hw->read();
    else              return 0x0000;
}

void HwConn::connect(Hw &new_hw)
{
    if(hw != nullptr) throw std::runtime_error("hw connection is already connected");
    else              hw = &new_hw;
}

void HwConn::disconnect()
{
    if(hw == nullptr) throw std::runtime_error("hw connection is already disconnected");
    else              hw = nullptr;
}

Hw &HwConn::get()
{
    if(hw == nullptr) throw std::runtime_error("cannot return disconnected hw");
    else              return *hw;
}
