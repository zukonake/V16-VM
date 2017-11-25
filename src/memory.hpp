#pragma once

#include <v16/architecture.hpp>
#include <hardware.hpp>

template<uint32_t size>
class Memory : public Hardware
{
	protected:
	virtual Word &handleSdp(Word value) override;

	Word M[size];
};

template<uint32_t size>
Word &Memory<size>::handleSdp(Word value)
{
	if(value >= size)
	{
		throw std::out_of_range("memory address: " + std::to_string(value) + " is out of range");
	}
	return M[value];
}
