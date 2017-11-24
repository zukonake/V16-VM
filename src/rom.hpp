#pragma once

#include <stdexcept>
#include <fstream>
//
#include <architecture.hpp>
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
	if(file.is_open())
	{
		file.seekg(0, std::ios::beg);
		file.read(reinterpret_cast<char *>(M), 0x200);
		file.close();
	}
	else throw std::runtime_error("couldn't open rom file");
}

template<uint32_t size>
Word &ROM<size>::operator[](Word address)
{
	Hardware::dummy = M[address];
	return Hardware::dummy;
}

template<uint32_t size>
Word const &ROM<size>::operator[](Word address) const
{
	return M[address];
}

