#pragma once

#include <cstdint>
#include <string>
#include <vector>

#define LOGMODE

#ifdef LOGMODE
#include <stdio.h>
#endif

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

	void Clock();
	void Reset();
	void IRQ();		// Interrupt request signal
	void NMI();		// Non-maskable interrupt request signal

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
	Bus* bus_ = nullptr;

public:
	uint8_t ReadBus(uint16_t address);
	void WriteBus(uint16_t address, uint8_t data);

/* Bus END */

/* Opcodes START */

private:
	// Addressing Modes =============================================
	// The 6502 has a variety of addressing modes to access data in 
	// memory, some of which are direct and some are indirect (like
	// pointers in C++). Each opcode contains information about which
	// addressing mode should be employed to facilitate the 
	// instruction, in regards to where it reads/writes the data it
	// uses. The address mode changes the number of bytes that
	// makes up the full instruction, so we implement addressing
	// before executing the instruction, to make sure the program
	// counter is at the correct location, the instruction is
	// primed with the addresses it needs, and the number of clock
	// cycles the instruction requires is calculated. These functions
	// may adjust the number of cycles required depending upon where
	// and how the memory is accessed, so they return the required
	// adjustment.

	uint8_t IMP();	uint8_t IMM();
	uint8_t ZP0();	uint8_t ZPX();
	uint8_t ZPY();	uint8_t REL();
	uint8_t ABS();	uint8_t ABX();
	uint8_t ABY();	uint8_t IND();
	uint8_t IZX();	uint8_t IZY();

	uint8_t  opcode_		= 0x00;	// Is the instruction byte
	uint8_t  cycles_		= 0x00;	// Counts how many cycles the instruction has remaining
	uint32_t clock_count_	= 0x00;	// A global accumulation of the number of clocks

	// Opcodes ======================================================
	// There are 56 "legitimate" opcodes provided by the 6502 CPU. I
	// have not modelled "unofficial" opcodes. As each opcode is 
	// defined by 1 byte, there are potentially 256 possible codes.
	// Codes are not used in a "switch case" style on a processor,
	// instead they are repsonisble for switching individual parts of
	// CPU circuits on and off. The opcodes listed here are official, 
	// meaning that the functionality of the chip when provided with
	// these codes is as the developers intended it to be. Unofficial
	// codes will of course also influence the CPU circuitry in 
	// interesting ways, and can be exploited to gain additional
	// functionality!
	//
	// These functions return 0 normally, but some are capable of
	// requiring more clock cycles when executed under certain
	// conditions combined with certain addressing modes. If that is 
	// the case, they return 1.
	//
	// I have included detailed explanations of each function in 
	// the class implementation file. Note they are listed in
	// alphabetical order here for ease of finding.

	uint8_t ADC();	uint8_t AND();	uint8_t ASL();	uint8_t BCC();
	uint8_t BCS();	uint8_t BEQ();	uint8_t BIT();	uint8_t BMI();
	uint8_t BNE();	uint8_t BPL();	uint8_t BRK();	uint8_t BVC();
	uint8_t BVS();	uint8_t CLC();	uint8_t CLD();	uint8_t CLI();
	uint8_t CLV();	uint8_t CMP();	uint8_t CPX();	uint8_t CPY();
	uint8_t DEC();	uint8_t DEX();	uint8_t DEY();	uint8_t EOR();
	uint8_t INC();	uint8_t INX();	uint8_t INY();	uint8_t JMP();
	uint8_t JSR();	uint8_t LDA();	uint8_t LDX();	uint8_t LDY();
	uint8_t LSR();	uint8_t NOP();	uint8_t ORA();	uint8_t PHA();
	uint8_t PHP();	uint8_t PLA();	uint8_t PLP();	uint8_t ROL();
	uint8_t ROR();	uint8_t RTI();	uint8_t RTS();	uint8_t SBC();
	uint8_t SEC();	uint8_t SED();	uint8_t SEI();	uint8_t STA();
	uint8_t STX();	uint8_t STY();	uint8_t TAX();	uint8_t TAY();
	uint8_t TSX();	uint8_t TXA();	uint8_t TXS();	uint8_t TYA();

	// I capture all "unofficial" opcodes with this function. It is
	// functionally identical to a NOP
	uint8_t XXX();

	uint8_t  fetched_		= 0x00;   // Represents the working input value to the ALU
	uint16_t address_abs_	= 0x0000; // All used memory addresses end up in here
	uint16_t address_rel_	= 0x00;   // Represents absolute address following a branch

	// The read location of data can come from two sources, a memory address, or
	// its immediately available as part of the instruction. This function decides
	// depending on address mode of instruction byte
	uint8_t Fetch();

	struct INSTRUCTION
	{
		std::string name;
		uint8_t(CPU::* operate)(void)		= nullptr;
		uint8_t(CPU::* address_mode)(void)	= nullptr;
		uint8_t     cycles = 0;
	};

	std::vector<INSTRUCTION> instructions_;

/* Opcodes END */

#ifdef LOGMODE
private:
	FILE* logfile_ = nullptr;
#endif
};

