#pragma once

#include <architecture.hpp>
#include <hardware.hpp>

class MemoryInterface
{
	public:
	virtual ~MemoryInterface() = default;

	virtual Word &operator[](Word address) = 0;
	virtual Word const &operator[](Word address) const = 0;
};

template<uint32_t size>
class Memory : public MemoryInterface, public Hardware
{
	public:
	virtual Word &operator[](Word address) override;
	virtual Word const &operator[](Word address) const override;

	virtual Word &operator[](Byte address) override;
	virtual Word const &operator[](Byte address) const override;
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

template<uint32_t size>
Word &Memory<size>::operator[](Byte address)
{
	return M[address];
}

template<uint32_t size>
Word const &Memory<size>::operator[](Byte address) const
{
	return M[address];
}
