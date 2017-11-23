#pragma once

#include <architecture.hpp>

class HardwareInterface
{
	public:
	virtual ~HardwareInterface() = default;

	virtual Word &operator[](Byte address) = 0;
	virtual Word const &operator[](Byte address) const = 0;
};

template<uint32_t size>
class Hardware : public HardwareInterface
{
	public:
	virtual Word &operator[](Byte address) override;
	virtual Word const &operator[](Byte address) const override;
	protected:
	Word M[size];
};

template<uint32_t size>
Word &Hardware<size>::operator[](Byte address)
{
	return M[address];
}

template<uint32_t size>
Word const &Hardware<size>::operator[](Byte address) const
{
	return M[address];
}

