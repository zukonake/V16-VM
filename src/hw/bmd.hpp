#pragma once

#include <cstddef>
//
#include <SFML/Graphics.hpp>
//
#include <hw/adp_device.hpp>

class Bmd : public AdpDevice
{
    public:
    enum Command
    {
        CLEAR     = 0x00,
        BLIT      = 0x01,
        PART_BLIT = 0x02,
        OR        = 0x03,
        SET       = 0x04,
        DISPLAY   = 0x05,
        TEXT      = 0x06,
        TEXT_BLIT = 0x07,
        INFO      = 0x08
    };

    enum State
    {
        COMMAND,
        BLIT_0,
        PART_BLIT_0,
        PART_BLIT_1,
        PART_BLIT_2,
        OR_0,
        OR_1,
        SET_0,
        SET_1,
        TEXT_0,
        TEXT_1,
        TEXT_BLIT_0,
        TEXT_BLIT_1,
        TEXT_BLIT_2,
        INFO_0,
        INFO_1,
    };

    Bmd(std::size_t vPixelWidth, std::size_t vPixelHeight, float pixelSize);

    protected:
    virtual void handleAdpI8(Byte value) override;
    virtual void handleAdpI16(Word value) override;
    private:
    std::size_t getWordSize() const;
    void clear();
    void display();

    unsigned inAddr;
    unsigned inSize;
    uint8_t  inCode;
    std::vector<Word> buffer;
    float pixelSize;
    std::size_t wordWidth;
    std::size_t wordHeight;
    State state;
    sf::RenderWindow window;

    Word const font[2 * 26] =
    {
        0x5200, 0x0557, //A 0x00
        0x5300, 0x0353, //B 0x01
        0x5700, 0x0751, //C 0x02
        0x5300, 0x0355, //D 0x03
        0x1700, 0x0717, //E 0x04
        0x1700, 0x0117, //F 0x05
        0x1600, 0x0655, //G 0x06
        0x5500, 0x0557, //H 0x07
        0x0200, 0x0222, //I 0x08
        0x4700, 0x0754, //J 0x09
        0x5500, 0x0553, //K 0x0A
        0x1100, 0x0711, //L 0x0B
        0xFF00, 0x0DDD, //M 0x0C
        0x5300, 0x0555, //N 0x0D
        0x5700, 0x0755, //O 0x0E
        0x5700, 0x0117, //P 0x0F
        0x5700, 0x4755, //Q 0x10
        0x5700, 0x0537, //R 0x11
        0x1700, 0x0747, //S 0x12
        0x2700, 0x0222, //T 0x13
        0x5500, 0x0755, //U 0x14
        0x5500, 0x0225, //V 0x15
        0xDD00, 0x06FD, //W 0x16
        0x5500, 0x0552, //X 0x17
        0x5500, 0x0113, //Y 0x18
        0x4700, 0x0712  //Z 0x19
    };
};
