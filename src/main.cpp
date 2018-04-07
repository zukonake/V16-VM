#include <fstream>
#include <iostream>
//
#include <hw/vrwm.hpp>
#include <hw/rom.hpp>
#include <cpu.hpp>

int main(int argc, char *argv[])
{
    if(argc < 2)
    {
        std::cout << "usage: " << argv[0] << " ROM_PATH\n";
        return 1;
    }
    std::string romPath = argv[1];
    std::ifstream file(romPath, std::ios::in | std::ios::binary);
    if(!file.good())
    {
        throw std::runtime_error("couldn't open ROM file");
    }
    file.seekg(0, file.end);
    Word romSize = file.tellg();
    file.seekg(0, file.beg);
    Byte *bytes = new uint8_t[romSize * 2];
    Word *words = new uint16_t[romSize];
    file.read(reinterpret_cast<char *>(bytes), romSize * 2);
    for(unsigned i = 0; i < romSize; i++)
    {
        words[i] = bytes[(i * 2)] | static_cast<Word>(bytes[(i * 2) + 1] << 8);
    }
    file.close();

    Vrwm<0x100> memory;
    Rom rom(words, romSize);

    delete[] bytes;
    delete[] words;

    CPU cpu(romSize, true);
    cpu.connectHardware(0x00, memory);
    cpu.connectHardware(0xFF, rom);
    cpu.start();
    return 0;
}
