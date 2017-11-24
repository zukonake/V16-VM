#pragma once

#include <architecture.hpp>

class HardwareInterface
{
	public:
	virtual ~HardwareInterface() = default;

	virtual Word &operator[](Byte address) = 0;
	virtual Word const &operator[](Byte address) const = 0;

	virtual void receive(Word word) = 0;
};

class DummyHardware : public HardwareInterface
{
	public:
	virtual Word &operator[](Byte address) override;
	virtual Word const &operator[](Byte address) const override;

	virtual void receive(Word word) override;
	private:
	Word mutable dummy;
};
