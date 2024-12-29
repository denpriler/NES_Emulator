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
	enum REGS
	{
		A	= 0,		// Accumulator Register
		X	= 1,		// X Register
		Y	= 2,		// Y Register
		SP	= 3,		// Stack Pointer (points to location on bus)
		S	= 4,		// Status Register
		PC,				// Program Counter
	};

public:
	// Status register flags.
	enum FLAGS
	{
		C = (1 << 0),	// Carry Bit
		Z = (1 << 1),	// Zero
		I = (1 << 2),	// Disable Interrupts
		D = (1 << 3),	// Decimal Mode (unused in this implementation)
		B = (1 << 4),	// Break
		U = (1 << 5),	// Unused
		V = (1 << 6),	// Overflow
		N = (1 << 7),	// Negative
	};

private:
	void ClearRegisters();

public:
	uint8_t ReadReg(REGS reg);
	void WriteReg(REGS reg, uint8_t data);
	// Status flags register methods
	uint8_t GetFlag(FLAGS flag);
	void SetFlag(FLAGS flag, bool value);

/* Registers END */
};

