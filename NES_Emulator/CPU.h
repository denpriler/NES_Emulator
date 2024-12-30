#pragma once

#include <cstdint>

// Forward declaration of generic communications bus class to prevent circular inclusions.
class Bus;

/*
* CPU olc6102 NES processor class.
*/
class CPU
{
public:
	CPU(Bus* bus);
	~CPU();

/* Registers START */

public:
	// CPU Core registers, exposed as public here for ease of access from external examinors.
	// This is all the 6502 has.
	uint8_t A	= 0x00;		// Accumulator Register
	uint8_t X	= 0x00;		// X Register
	uint8_t Y	= 0x00;		// Y Register
	uint8_t SP	= 0x00;		// Stack Pointer (points to location on bus)
	uint8_t S	= 0x00;		// Status Register
	uint16_t PC = 0x00000;	// Program Counter

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

public:
	// Status flags register methods
	uint8_t GetFlag(FLAGS flag);
	void SetFlag(FLAGS flag, bool value);

/* Registers END */

/* Bus START */

private:
	Bus* bus_;

public:
	uint8_t ReadBus(uint16_t address);
	void WriteBus(uint16_t address, uint8_t data);

/* Bus END */
};

