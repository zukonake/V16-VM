#pragma once

#include <architecture.hpp>

class Hardware
{
	public:
	virtual ~Hardware() = default;

	virtual Word &operator[](Byte address) = 0;
	virtual Word const &operator[](Byte address) const = 0;

	virtual void receive(Word word);
};

class DummyHardware : public Hardware
{
	public:
	virtual Word &operator[](Byte address) override;
	virtual Word const &operator[](Byte address) const override;
	private:
	Word mutable dummy;
}; //TODO is dummy saving data?

class TestHardware : public DummyHardware
{
	public:
	virtual void receive(Word word) override;
};
