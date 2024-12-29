#pragma once

#include <cstdint>
#include <array>

// Forward declaration of generic communications bus class to prevent circular inclusions.
class Bus;

/*
* CPU olc6102 NES processor class.
*/
class CPU
{
private:
	Bus* bus_;

public:
	CPU(Bus* bus);
	~CPU();

/* Registers START */

private:
	std::array<uint8_t, 5> registers_;
	uint16_t PC_ = 0x0000;	// Program Counter

public:
	// CPU Core registers, exposed as public here for ease of access from external examinors.
	// This is all the 6502 has.
	enum REGS6502
	{
		A	= 0,		// Accumulator Register
		X	= 1,		// X Register
		Y	= 2,		// Y Register
		SP	= 3,		// Stack Pointer (points to location on bus)
		S	= 4,		// Status Register
		// PC,				// Program Counter
	};

private:
	void ClearRegisters();
	uint8_t ReadReg(REGS6502 reg);
	void WriteReg(REGS6502 reg, uint8_t data);
	uint16_t ReadPC();
	void WritePC(uint16_t data);

/* Registers END */
};

