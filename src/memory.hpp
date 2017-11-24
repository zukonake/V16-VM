#pragma once

#include <architecture.hpp>
#include <hardware.hpp>

template<uint32_t size>
class Memory : public Hardware
{
	public:
	virtual Word &operator[](Word address) override;
	virtual Word const &operator[](Word address) const override;
	private:
	Word M[size];
};

template<uint32_t size>
Word &Memory<size>::operator[](Word address)
{
	return M[address];
}

template<uint32_t size>
Word const &Memory<size>::operator[](Word address) const
{
	return M[address];
}
