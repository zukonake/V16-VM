#include <cassert>
#include <stdexcept>
#include <string>
//
#include "bmd.hpp"

Bmd::Bmd(std::size_t vPixelWidth, std::size_t vPixelHeight, float pixelSize) :
    pixelSize(pixelSize),
    wordWidth(vPixelWidth * 0x10),
    wordHeight(vPixelHeight * 0x10),
    state(COMMAND),
    window(sf::VideoMode(vPixelWidth * pixelSize, vPixelHeight * pixelSize, 2),
           "Basic Monochromatic Display",
           sf::Style::Close | sf::Style::Titlebar)
{
    assert(pixelSize > 0.0);
    assert(!(vPixelWidth % 0x10));
    assert(!(vPixelHeight % 0x10));
    window.clear(sf::Color::Black);
    window.display();
    buffer.reserve(getWordSize());
    for(std::size_t i = 0; i < getWordSize(); ++i) buffer.push_back(0x0000);
}

void Bmd::handleAdpI8(Byte value)
{
    switch(state)
    {
        case COMMAND:
        switch(static_cast<Command>(value))
        {
            case CLEAR:     clear();                    break;
            case BLIT:      state = BLIT_0; inAddr = 0; break;
            case PART_BLIT: state = PART_BLIT_0;        break;
            case OR:        state = OR_0;               break;
            case SET:       state = SET_0;              break;
            case DISPLAY:   display();                  break;
            case TEXT:      state = TEXT_0;             break;
            case TEXT_BLIT: state = TEXT_BLIT_0;        break;
            case INFO:      state = INFO_0;             break;
            default: throw std::runtime_error("illegal BMD command: " + std::to_string(value));
        } break;

        case TEXT_0: inCode = value; state = TEXT_1; break;

        case TEXT_BLIT_2:
        buffer[inAddr]             = font[value * 2];
        buffer[inAddr + wordWidth] = font[value * 2 + 1];
        if(++inAddr >= inSize) state = COMMAND;
        break;

        case INFO_0: word_out = wordWidth;  state = INFO_1;  break;
        case INFO_1: word_out = wordHeight; state = COMMAND; break;

        default: throw std::runtime_error("illegal BMD state: " + std::to_string(state));
    }
}

void Bmd::handleAdpI16(Word value)
{
    switch(state)
    {
        case BLIT_0:
        buffer[inAddr++] = value;
        if(inAddr >= getWordSize()) state = COMMAND;
        break;

        case PART_BLIT_0: inAddr = value; state = PART_BLIT_1; break;
        case PART_BLIT_1: inSize = value + inAddr; state = PART_BLIT_2; break;
        case PART_BLIT_2:
        buffer[inAddr++] = value;
        if(inAddr >= inSize) state = COMMAND;
        break;

        case SET_0: inAddr = value;         state = OR_1;    break;
        case SET_1: buffer[inAddr] = value; state = COMMAND; break;

        case OR_0: inAddr = value;          state = OR_1;    break;
        case OR_1: buffer[inAddr] |= value; state = COMMAND; break;

        case TEXT_1:
            inAddr = value;
            buffer[inAddr]             = font[inCode * 2];
            buffer[inAddr + wordWidth] = font[inCode * 2 + 1];
            state = COMMAND; break;

        case TEXT_BLIT_0: inAddr = value; state = TEXT_BLIT_1; break;
        case TEXT_BLIT_1: inSize = value + inAddr; state = TEXT_BLIT_2; break;

        default: throw std::runtime_error("illegal BMD state: " + std::to_string(state));
    }
}

std::size_t Bmd::getWordSize() const
{
    return wordWidth * wordHeight;
}

void Bmd::clear()
{
    for(std::size_t i = 0; i < getWordSize(); ++i) buffer[i] = 0x0000;
}

void Bmd::display()
{
    window.clear(sf::Color::Black);
    sf::VertexArray quad(sf::Quads, 4);
    for(std::size_t wY = 0; wY < wordHeight; ++wY)
    {
        for(std::size_t wX = 0; wX < wordWidth; ++wX)
        {
            for(std::size_t pY = 0; pY < 4; ++pY)
            {
                for(std::size_t pX = 0; pX < 4; ++pX)
                {
                    quad[0].position = sf::Vector2f(wX * 4 + pX    , wY * 4 + pY    ) * pixelSize;
                    quad[1].position = sf::Vector2f(wX * 4 + pX + 1, wY * 4 + pY    ) * pixelSize;
                    quad[2].position = sf::Vector2f(wX * 4 + pX + 1, wY * 4 + pY + 1) * pixelSize;
                    quad[3].position = sf::Vector2f(wX * 4 + pX    , wY * 4 + pY + 1) * pixelSize;
                    sf::Color color = sf::Color::Black;
                    if((buffer[(wY * wordWidth) + wX] >> ((pY * 4) + pX)) & 1)
                    {
                        color = sf::Color::White;
                        quad[0].color = color;
                        quad[1].color = color;
                        quad[2].color = color;
                        quad[3].color = color;
                        window.draw(quad);
                    }
                }
            }
        }
    }
    window.display();
}
