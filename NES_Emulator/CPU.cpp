#include "CPU.h"
#include "Bus.h"

CPU::CPU(Bus* bus): bus_(bus)
{
}

CPU::~CPU()
{
}

/* Registers START */

uint8_t CPU::GetFlag(FLAGS flag)
{
	// Using logical AND operation to get specified bit of status register.
	return (S & flag) > 0 ? 1 : 0;
}

void CPU::SetFlag(FLAGS flag, bool value)
{
	value
		? (S |= flag)	// Using logical AND to set bit.
		: (S &= ~flag);	// Using logical AND with flag reverse to unset bit.
}

/* Registers END */

/* Bus START */

uint8_t CPU::ReadBus(uint16_t address)
{
	return bus_->Read(address);
}

void CPU::WriteBus(uint16_t address, uint8_t data)
{
	bus_->Write(address, data);
}

/* Bus END */