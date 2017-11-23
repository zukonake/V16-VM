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
class Memory : public MemoryInterface, protected Hardware<size>
{
	public:
	virtual Word &operator[](Word address) override;
	virtual Word const &operator[](Word address) const override;
};

template<uint32_t size>
Word &Memory<size>::operator[](Word address)
{
	return Hardware<size>::M[address];
}

template<uint32_t size>
Word const &Memory<size>::operator[](Word address) const
{
	return Hardware<size>::M[address];
}
