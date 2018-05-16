#include <fstream>
//
#include <curses.h>
//
#include <int.hpp>
#include <hw/vrwm.hpp>
#include <hw/rom.hpp>
#include <hw/bmd.hpp>
#include <cpu.hpp>

int main(int argc, char *argv[])
{
    initscr();
    timeout(100);
    start_color();

    if(argc < 2)
    {
        printf("usage: %s ROM_PATH\n", argv[0]);
        endwin();
        return 1;
    }
    std::string romPath = argv[1];
    std::ifstream file(romPath, std::ios::in | std::ios::binary);
    if(!file.good())
    {
        throw std::runtime_error("couldn't open ROM file");
    }
    file.seekg(0, file.end);
    u16 romSize = (file.tellg() / 2);
    file.seekg(0, file.beg);
    u8  *bytes = new uint8_t[romSize * 2];
    u16 *words = new uint16_t[romSize];
    file.read(reinterpret_cast<char *>(bytes), romSize * 2);
    for(unsigned i = 0; i < romSize; i++)
    {
        words[i] = bytes[(i * 2)] | static_cast<u16>(bytes[(i * 2) + 1] << 8);
    }
    file.close();

    Vrwm memory(0x10000);
    Rom rom(words, romSize);
    //Bmd display(4 * 0x10, 0x10, 16);

    delete[] bytes;
    delete[] words;

    Cpu cpu;
    cpu.connect_hw(0x00, memory);
    //cpu.connectHardware(0x01, display);
    cpu.connect_hw(0xFF, rom);
    cpu.start();
    cpu.await();

    endwin();
    return 0;
}
