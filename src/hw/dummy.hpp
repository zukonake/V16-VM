#pragma once

#include <architecture.hpp>
#include <hw/adp_device.hpp>

class DummyDevice : public AdpDevice
{
    protected:
    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;
};
