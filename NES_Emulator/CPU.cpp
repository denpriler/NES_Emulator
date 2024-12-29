#include "CPU.h"

CPU::CPU(Bus* bus): bus_(bus)
{
	ClearRegisters();
}

CPU::~CPU()
{
}

uint8_t CPU::ReadReg(REGS6502 reg)
{
	return registers_.at(reg);
}

void CPU::WriteReg(REGS6502 reg, uint8_t data)
{
	registers_[reg] = data;
}

uint16_t CPU::ReadPC()
{
	return PC_;
}

void CPU::WritePC(uint16_t data)
{
	PC_ = data;
}

void CPU::ClearRegisters()
{
	for (auto& i : registers_) {
		registers_[i] = 0x00;
	}
	PC_ = 0x0000;
}
