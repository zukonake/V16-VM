#pragma once

#include <v16/architecture.hpp>

class Hardware
{
	public:
	virtual ~Hardware() = default;

	virtual Word &operator[](Word address);
	virtual Word const &operator[](Word address) const;

	virtual void receive(Word word);
	protected:
	Word mutable dummy;
};

class DummyHardware : public Hardware {};

class TestHardware : public DummyHardware
{
	public:
	virtual void receive(Word word) override;
};
