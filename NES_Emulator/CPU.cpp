#include "CPU.h"
#include "Bus.h"

CPU::CPU(Bus* bus): bus_(bus)
{
	ClearRegisters();
}

CPU::~CPU()
{
}

/* Registers START */

uint8_t CPU::ReadReg(REGS reg)
{
	if (reg == PC) {
		return PC_;
	}

	return registers_[reg];
}

void CPU::WriteReg(REGS reg, uint8_t data)
{
	if (reg == PC) {
		PC_ = data;
		return;
	}

	registers_[reg] = data;
}

uint8_t CPU::GetFlag(FLAGS flag)
{
	// Using logical AND operation to get specified bit of status register.
	return (registers_[S] & flag) > 0 ? 1 : 0;
}

void CPU::SetFlag(FLAGS flag, bool value)
{
	value
		? (registers_[S] |= flag)	// Using logical AND to set bit.
		: (registers_[S] &= ~flag);	// Using logical AND with flag reverse to unset bit.
}

void CPU::ClearRegisters()
{
	for (auto& i : registers_) {
		registers_[i] = 0x00;
	}
	PC_ = 0x0000;
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