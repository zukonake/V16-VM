#pragma once

#include <stdexcept>
#include <fstream>
#include <string>
//
#include <v16/architecture.hpp>
#include <hardware.hpp>

template<uint32_t size>
class ROM : public Hardware
{
	public:
	ROM(std::string const &path);

	virtual Word &operator[](Word address) override;
	virtual Word const &operator[](Word address) const override;
	private:
	Word M[size];
};

template<uint32_t size>
ROM<size>::ROM(std::string const &path)
{
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if(!file.good())
	{
		throw std::runtime_error("couldn't open rom file");
	}
	uint8_t *bytes = new uint8_t[size * 2];
	file.read(reinterpret_cast<char *>(bytes), 0x200);
	for(unsigned i = 0; i < size; i++)
	{
		M[i] = (static_cast<Word>(bytes[(i * 2)] << 8)) | bytes[(i * 2) + 1];
	}
	file.close();
}

template<uint32_t size>
Word &ROM<size>::operator[](Word address)
{
	if(address >= size)
	{
		throw std::out_of_range("rom address: " + std::to_string(address) + " is out of range");
	}
	Hardware::dummy = M[address];
	return Hardware::dummy;
}

template<uint32_t size>
Word const &ROM<size>::operator[](Word address) const
{
	if(address >= size)
	{
		throw std::out_of_range("rom address: " + std::to_string(address) + " is out of range");
	}
	return M[address];
}

