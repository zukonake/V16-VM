#include <cassert>
#include <stdexcept>
#include <string>
//
#include "bmd.hpp"

Bmd::Bmd(std::size_t v_pixel_width, std::size_t v_pixel_height, float pixel_size) :
    pixel_size(pixel_size),
    word_width(v_pixel_width * 0x10),
    word_height(v_pixel_height * 0x10),
    window(sf::VideoMode(v_pixel_width * pixel_size, v_pixel_height * pixel_size, 2),
           "Basic Monochromatic Display",
           sf::Style::Close | sf::Style::Titlebar)
{
    assert(pixel_size > 0.0);
    assert(!(v_pixel_width % 0x10));
    assert(!(v_pixel_height % 0x10));
    window.clear(sf::Color::Black);
    window.display();
    buffer.reserve(word_size());
    for(std::size_t i = 0; i < word_size(); ++i) buffer.push_back(0x0000);
}

void Bmd::single_iteration()
{
    Command command = static_cast<Command>(receive());
    switch(command)
    {
        case INFO:
        {
            send(word_width);
            send(word_height);
        }
        case CLEAR: clear(); break;
        case BLIT:
        {
            for(u16 i = 0; i < word_size(); ++i) buffer.at(i) = receive();
        }; break;

        case PART_BLIT:
        {
            u16 addr = receive();
            u16 size = receive();
            for(u16 i = addr; i < addr + size; ++i) buffer.at(i) = receive();
        }; break;

        case OR:
        {
            u16 addr = receive();
            buffer.at(addr) |= receive();
        }; break;

        case SET:
        {
            u16 addr = receive();
            buffer.at(addr) = receive();
        }; break;

        case DISPLAY: display(); break;

        case TEXT:
        {
            u16 addr = receive();
            u16 code = receive();
            buffer.at(addr)              = font[code * 2];
            buffer.at(addr + word_width) = font[code * 2 + 1];
        }; break;

        case TEXT_BLIT:
        {
            u16 addr = receive();
            u16 size = receive();
            for(u16 i = addr; i < addr + size; ++i)
            {
                u16 code = receive();
                buffer.at(i)              = font[code * 2];
                buffer.at(i + word_width) = font[code * 2 + 1];
            }
        }; break;

        default: throw std::runtime_error("illegal bmd command: " + std::to_string(command));
    }
}

std::size_t Bmd::word_size() const
{
    return word_width * word_height;
}

void Bmd::clear()
{
    for(std::size_t i = 0; i < word_size(); ++i) buffer[i] = 0x0000;
}

void Bmd::display()
{
    window.clear(sf::Color::Black);
    sf::VertexArray quad(sf::Quads, 4);
    for(std::size_t w_y = 0; w_y < word_height; ++w_y)
    {
        for(std::size_t w_x = 0; w_x < word_width; ++w_x)
        {
            for(std::size_t p_y = 0; p_y < 4; ++p_y)
            {
                for(std::size_t p_x = 0; p_x < 4; ++p_x)
                {
                    quad[0].position = sf::Vector2f(w_x * 4 + p_x    , w_y * 4 + p_y    ) * pixel_size;
                    quad[1].position = sf::Vector2f(w_x * 4 + p_x + 1, w_y * 4 + p_y    ) * pixel_size;
                    quad[2].position = sf::Vector2f(w_x * 4 + p_x + 1, w_y * 4 + p_y + 1) * pixel_size;
                    quad[3].position = sf::Vector2f(w_x * 4 + p_x    , w_y * 4 + p_y + 1) * pixel_size;
                    sf::Color color = sf::Color::Black;
                    if((buffer[(w_y * word_width) + w_x] >> ((p_y * 4) + p_x)) & 1)
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
