#include "CPU.h"
#include "Bus.h"

#define SP_BASE 0x0100

CPU::CPU(Bus* bus): bus_(bus)
{
	// Assembles the translation table. It's big, it's ugly, but it yields a convenient way
	// to emulate the 6502. I'm certain there are some "code-golf" strategies to reduce this
	// but I've deliberately kept it verbose for study and alteration

	// It is 16x16 entries. This gives 256 instructions. It is arranged to that the bottom
	// 4 bits of the instruction choose the column, and the top 4 bits choose the row.

	// For convenience to get function pointers to members of this class, I'm using this
	// or else it will be much much larger :D

	// The table is one big initialiser list of initialiser lists...
	using a = CPU;
	instructions_ =
	{
		{ "BRK", &a::BRK, &a::IMM, 7 },{ "ORA", &a::ORA, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::ZP0, 3 },{ "ASL", &a::ASL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHP", &a::PHP, &a::IMP, 3 },{ "ORA", &a::ORA, &a::IMM, 2 },{ "ASL", &a::ASL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABS, 4 },{ "ASL", &a::ASL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BPL", &a::BPL, &a::REL, 2 },{ "ORA", &a::ORA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ZPX, 4 },{ "ASL", &a::ASL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLC", &a::CLC, &a::IMP, 2 },{ "ORA", &a::ORA, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ORA", &a::ORA, &a::ABX, 4 },{ "ASL", &a::ASL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "JSR", &a::JSR, &a::ABS, 6 },{ "AND", &a::AND, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "BIT", &a::BIT, &a::ZP0, 3 },{ "AND", &a::AND, &a::ZP0, 3 },{ "ROL", &a::ROL, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLP", &a::PLP, &a::IMP, 4 },{ "AND", &a::AND, &a::IMM, 2 },{ "ROL", &a::ROL, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "BIT", &a::BIT, &a::ABS, 4 },{ "AND", &a::AND, &a::ABS, 4 },{ "ROL", &a::ROL, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BMI", &a::BMI, &a::REL, 2 },{ "AND", &a::AND, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ZPX, 4 },{ "ROL", &a::ROL, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEC", &a::SEC, &a::IMP, 2 },{ "AND", &a::AND, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "AND", &a::AND, &a::ABX, 4 },{ "ROL", &a::ROL, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTI", &a::RTI, &a::IMP, 6 },{ "EOR", &a::EOR, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "EOR", &a::EOR, &a::ZP0, 3 },{ "LSR", &a::LSR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PHA", &a::PHA, &a::IMP, 3 },{ "EOR", &a::EOR, &a::IMM, 2 },{ "LSR", &a::LSR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::ABS, 3 },{ "EOR", &a::EOR, &a::ABS, 4 },{ "LSR", &a::LSR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVC", &a::BVC, &a::REL, 2 },{ "EOR", &a::EOR, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ZPX, 4 },{ "LSR", &a::LSR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLI", &a::CLI, &a::IMP, 2 },{ "EOR", &a::EOR, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "EOR", &a::EOR, &a::ABX, 4 },{ "LSR", &a::LSR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "RTS", &a::RTS, &a::IMP, 6 },{ "ADC", &a::ADC, &a::IZX, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 3 },{ "ADC", &a::ADC, &a::ZP0, 3 },{ "ROR", &a::ROR, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "PLA", &a::PLA, &a::IMP, 4 },{ "ADC", &a::ADC, &a::IMM, 2 },{ "ROR", &a::ROR, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "JMP", &a::JMP, &a::IND, 5 },{ "ADC", &a::ADC, &a::ABS, 4 },{ "ROR", &a::ROR, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BVS", &a::BVS, &a::REL, 2 },{ "ADC", &a::ADC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ZPX, 4 },{ "ROR", &a::ROR, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SEI", &a::SEI, &a::IMP, 2 },{ "ADC", &a::ADC, &a::ABY, 4 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "ADC", &a::ADC, &a::ABX, 4 },{ "ROR", &a::ROR, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "???", &a::NOP, &a::IMP, 2 },{ "STA", &a::STA, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZP0, 3 },{ "STA", &a::STA, &a::ZP0, 3 },{ "STX", &a::STX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "DEY", &a::DEY, &a::IMP, 2 },{ "???", &a::NOP, &a::IMP, 2 },{ "TXA", &a::TXA, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "STY", &a::STY, &a::ABS, 4 },{ "STA", &a::STA, &a::ABS, 4 },{ "STX", &a::STX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCC", &a::BCC, &a::REL, 2 },{ "STA", &a::STA, &a::IZY, 6 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "STY", &a::STY, &a::ZPX, 4 },{ "STA", &a::STA, &a::ZPX, 4 },{ "STX", &a::STX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "TYA", &a::TYA, &a::IMP, 2 },{ "STA", &a::STA, &a::ABY, 5 },{ "TXS", &a::TXS, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::NOP, &a::IMP, 5 },{ "STA", &a::STA, &a::ABX, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "???", &a::XXX, &a::IMP, 5 },
		{ "LDY", &a::LDY, &a::IMM, 2 },{ "LDA", &a::LDA, &a::IZX, 6 },{ "LDX", &a::LDX, &a::IMM, 2 },{ "???", &a::XXX, &a::IMP, 6 },{ "LDY", &a::LDY, &a::ZP0, 3 },{ "LDA", &a::LDA, &a::ZP0, 3 },{ "LDX", &a::LDX, &a::ZP0, 3 },{ "???", &a::XXX, &a::IMP, 3 },{ "TAY", &a::TAY, &a::IMP, 2 },{ "LDA", &a::LDA, &a::IMM, 2 },{ "TAX", &a::TAX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "LDY", &a::LDY, &a::ABS, 4 },{ "LDA", &a::LDA, &a::ABS, 4 },{ "LDX", &a::LDX, &a::ABS, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "BCS", &a::BCS, &a::REL, 2 },{ "LDA", &a::LDA, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 5 },{ "LDY", &a::LDY, &a::ZPX, 4 },{ "LDA", &a::LDA, &a::ZPX, 4 },{ "LDX", &a::LDX, &a::ZPY, 4 },{ "???", &a::XXX, &a::IMP, 4 },{ "CLV", &a::CLV, &a::IMP, 2 },{ "LDA", &a::LDA, &a::ABY, 4 },{ "TSX", &a::TSX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 4 },{ "LDY", &a::LDY, &a::ABX, 4 },{ "LDA", &a::LDA, &a::ABX, 4 },{ "LDX", &a::LDX, &a::ABY, 4 },{ "???", &a::XXX, &a::IMP, 4 },
		{ "CPY", &a::CPY, &a::IMM, 2 },{ "CMP", &a::CMP, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPY", &a::CPY, &a::ZP0, 3 },{ "CMP", &a::CMP, &a::ZP0, 3 },{ "DEC", &a::DEC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INY", &a::INY, &a::IMP, 2 },{ "CMP", &a::CMP, &a::IMM, 2 },{ "DEX", &a::DEX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 2 },{ "CPY", &a::CPY, &a::ABS, 4 },{ "CMP", &a::CMP, &a::ABS, 4 },{ "DEC", &a::DEC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BNE", &a::BNE, &a::REL, 2 },{ "CMP", &a::CMP, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ZPX, 4 },{ "DEC", &a::DEC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "CLD", &a::CLD, &a::IMP, 2 },{ "CMP", &a::CMP, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "CMP", &a::CMP, &a::ABX, 4 },{ "DEC", &a::DEC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
		{ "CPX", &a::CPX, &a::IMM, 2 },{ "SBC", &a::SBC, &a::IZX, 6 },{ "???", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "CPX", &a::CPX, &a::ZP0, 3 },{ "SBC", &a::SBC, &a::ZP0, 3 },{ "INC", &a::INC, &a::ZP0, 5 },{ "???", &a::XXX, &a::IMP, 5 },{ "INX", &a::INX, &a::IMP, 2 },{ "SBC", &a::SBC, &a::IMM, 2 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::SBC, &a::IMP, 2 },{ "CPX", &a::CPX, &a::ABS, 4 },{ "SBC", &a::SBC, &a::ABS, 4 },{ "INC", &a::INC, &a::ABS, 6 },{ "???", &a::XXX, &a::IMP, 6 },
		{ "BEQ", &a::BEQ, &a::REL, 2 },{ "SBC", &a::SBC, &a::IZY, 5 },{ "???", &a::XXX, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 8 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ZPX, 4 },{ "INC", &a::INC, &a::ZPX, 6 },{ "???", &a::XXX, &a::IMP, 6 },{ "SED", &a::SED, &a::IMP, 2 },{ "SBC", &a::SBC, &a::ABY, 4 },{ "NOP", &a::NOP, &a::IMP, 2 },{ "???", &a::XXX, &a::IMP, 7 },{ "???", &a::NOP, &a::IMP, 4 },{ "SBC", &a::SBC, &a::ABX, 4 },{ "INC", &a::INC, &a::ABX, 7 },{ "???", &a::XXX, &a::IMP, 7 },
	};
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

/* Addressing mode START */

// Address Mode: Implied
// There is no additional data required for this instruction. The instruction
// does something very simple like like sets a status bit. However, we will
// target the accumulator, for instructions like PHA
uint8_t CPU::IMP()
{
	fetched_ = A;
	return 0;
}

// Address Mode: Immediate
// The instruction expects the next byte to be used as a value, so we'll prep
// the read address to point to the next byte
uint8_t CPU::IMM()
{
	address_abs_ = PC++;
	return 0;
}

// Address Mode: Zero Page
// To save program bytes, zero page addressing allows you to absolutely address
// a location in first 0xFF bytes of address range. Clearly this only requires
// one byte instead of the usual two.
uint8_t CPU::ZP0()
{
	address_abs_ = ReadBus(PC);
	PC++;
	address_abs_ &= 0x00FF;
	return 0;
}

// Address Mode: Zero Page with X Offset
// Fundamentally the same as Zero Page addressing, but the contents of the X Register
// is added to the supplied single byte address. This is useful for iterating through
// ranges within the first page.
uint8_t CPU::ZPX()
{
	address_abs_ = ReadBus(PC) + X;
	PC++;
	address_abs_ &= 0x00FF;
	return 0;
}

// Address Mode: Zero Page with Y Offset
// Same as above but uses Y Register for offset
uint8_t CPU::ZPY()
{
	address_abs_ = ReadBus(PC) + Y;
	PC++;
	address_abs_ &= 0x00FF;
	return 0;
}

// Address Mode: Relative
// This address mode is exclusive to branch instructions. The address
// must reside within -128 to +127 of the branch instruction, i.e.
// you cant directly branch to any address in the addressable range.
uint8_t CPU::REL()
{
	address_rel_ = ReadBus(PC);
	PC++;
	if (address_rel_ & 0x80)
		address_rel_ |= 0xFF00;
	return 0;
}

// Address Mode: Absolute 
// A full 16-bit address is loaded and used
uint8_t CPU::ABS()
{
	uint16_t lo = ReadBus(PC);
	PC++;
	uint16_t hi = ReadBus(PC);
	PC++;

	address_abs_ = (hi << 8) | lo;

	return 0;
}

// Address Mode: Absolute with X Offset
// Fundamentally the same as absolute addressing, but the contents of the X Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t CPU::ABX()
{
	uint16_t lo = ReadBus(PC);
	PC++;
	uint16_t hi = ReadBus(PC);
	PC++;

	address_abs_ = (hi << 8) | lo;
	address_abs_ += X;

	if ((address_abs_ & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

// Address Mode: Absolute with Y Offset
// Fundamentally the same as absolute addressing, but the contents of the Y Register
// is added to the supplied two byte address. If the resulting address changes
// the page, an additional clock cycle is required
uint8_t CPU::ABY()
{
	uint16_t lo = ReadBus(PC);
	PC++;
	uint16_t hi = ReadBus(PC);
	PC++;

	address_abs_ = (hi << 8) | lo;
	address_abs_ += Y;

	if ((address_abs_ & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

// Note: The next 3 address modes use indirection (aka Pointers!)

// Address Mode: Indirect
// The supplied 16-bit address is read to get the actual 16-bit address. This is
// instruction is unusual in that it has a bug in the hardware! To emulate its
// function accurately, we also need to emulate this bug. If the low byte of the
// supplied address is 0xFF, then to read the high byte of the actual address
// we need to cross a page boundary. This doesnt actually work on the chip as 
// designed, instead it wraps back around in the same page, yielding an 
// invalid actual address
uint8_t CPU::IND()
{
	uint16_t ptr_lo = ReadBus(PC);
	PC++;
	uint16_t ptr_hi = ReadBus(PC);
	PC++;

	uint16_t ptr = (ptr_hi << 8) | ptr_lo;

	if (ptr_lo == 0x00FF) // Simulate page boundary hardware bug
	{
		address_abs_ = (ReadBus(ptr & 0xFF00) << 8) | ReadBus(ptr + 0);
	}
	else // Behave normally
	{
		address_abs_ = (ReadBus(ptr + 1) << 8) | ReadBus(ptr + 0);
	}

	return 0;
}


// Address Mode: Indirect X
// The supplied 8-bit address is offset by X Register to index
// a location in page 0x00. The actual 16-bit address is read 
// from this location
uint8_t CPU::IZX()
{
	uint16_t t = ReadBus(PC);
	PC++;

	uint16_t lo = ReadBus((uint16_t)(t + (uint16_t)X) & 0x00FF);
	uint16_t hi = ReadBus((uint16_t)(t + (uint16_t)X + 1) & 0x00FF);

	address_abs_ = (hi << 8) | lo;

	return 0;
}


// Address Mode: Indirect Y
// The supplied 8-bit address indexes a location in page 0x00. From 
// here the actual 16-bit address is read, and the contents of
// Y Register is added to it to offset it. If the offset causes a
// change in page then an additional clock cycle is required.
uint8_t CPU::IZY()
{
	uint16_t t = ReadBus(PC);
	PC++;

	uint16_t lo = ReadBus(t & 0x00FF);
	uint16_t hi = ReadBus((t + 1) & 0x00FF);

	address_abs_ = (hi << 8) | lo;
	address_abs_ += Y;

	if ((address_abs_ & 0xFF00) != (hi << 8))
		return 1;
	else
		return 0;
}

/* Addressing mode END */

/* Instructions START */

// Instruction: Add with Carry In
// Function:    A = A + M + C
// Flags Out:   C, V, N, Z
//
// Explanation:
// The purpose of this function is to add a value to the accumulator and a carry bit. If
// the result is > 255 there is an overflow setting the carry bit. Ths allows you to
// chain together ADC instructions to add numbers larger than 8-bits. This in itself is
// simple, however the 6502 supports the concepts of Negativity/Positivity and Signed Overflow.
//
// 10000100 = 128 + 4 = 132 in normal circumstances, we know this as unsigned and it allows
// us to represent numbers between 0 and 255 (given 8 bits). The 6502 can also interpret 
// this word as something else if we assume those 8 bits represent the range -128 to +127,
// i.e. it has become signed.
//
// Since 132 > 127, it effectively wraps around, through -128, to -124. This wraparound is
// called overflow, and this is a useful to know as it indicates that the calculation has
// gone outside the permissable range, and therefore no longer makes numeric sense.
//
// Note the implementation of ADD is the same in binary, this is just about how the numbers
// are represented, so the word 10000100 can be both -124 and 132 depending upon the 
// context the programming is using it in. We can prove this!
//
//  10000100 =  132  or  -124
// +00010001 = + 17      + 17
//  ========    ===       ===     See, both are valid additions, but our interpretation of
//  10010101 =  149  or  -107     the context changes the value, not the hardware!
//
// In principle under the -128 to 127 range:
// 10000000 = -128, 11111111 = -1, 00000000 = 0, 00000000 = +1, 01111111 = +127
// therefore negative numbers have the most significant set, positive numbers do not
//
// To assist us, the 6502 can set the overflow flag, if the result of the addition has
// wrapped around. V <- ~(A^M) & A^(A+M+C) :D lol, let's work out why!
//
// Let's suppose we have A = 30, M = 10 and C = 0
//          A = 30 = 00011110
//          M = 10 = 00001010+
//     RESULT = 40 = 00101000
//
// Here we have not gone out of range. The resulting significant bit has not changed.
// So let's make a truth table to understand when overflow has occurred. Here I take
// the MSB of each component, where R is RESULT.
//
// A  M  R | V | A^R | A^M |~(A^M) | 
// 0  0  0 | 0 |  0  |  0  |   1   |
// 0  0  1 | 1 |  1  |  0  |   1   |
// 0  1  0 | 0 |  0  |  1  |   0   |
// 0  1  1 | 0 |  1  |  1  |   0   |  so V = ~(A^M) & (A^R)
// 1  0  0 | 0 |  1  |  1  |   0   |
// 1  0  1 | 0 |  0  |  1  |   0   |
// 1  1  0 | 1 |  1  |  0  |   1   |
// 1  1  1 | 0 |  0  |  0  |   1   |
//
// We can see how the above equation calculates V, based on A, M and R. V was chosen
// based on the following hypothesis:
//       Positive Number + Positive Number = Negative Result -> Overflow
//       Negative Number + Negative Number = Positive Result -> Overflow
//       Positive Number + Negative Number = Either Result -> Cannot Overflow
//       Positive Number + Positive Number = Positive Result -> OK! No Overflow
//       Negative Number + Negative Number = Negative Result -> OK! NO Overflow
uint8_t CPU::ADC()
{
	// Grab the data that we are adding to the accumulator
	Fetch();

	// Add is performed in 16-bit domain for emulation to capture any
	// carry bit, which will exist in bit 8 of the 16-bit word
	uint16_t temp = (uint16_t)A + (uint16_t)fetched_ + (uint16_t)GetFlag(C);

	// The carry flag out exists in the high byte bit 0
	SetFlag(C, temp > 255);

	// The Zero flag is set if the result is 0
	SetFlag(Z, (temp & 0x00FF) == 0);

	// The signed Overflow flag is set based on all that up there! :D
	SetFlag(V, (~((uint16_t)A ^ (uint16_t)fetched_) & ((uint16_t)A ^ (uint16_t)temp)) & 0x0080);

	// The negative flag is set to the most significant bit of the result
	SetFlag(N, temp & 0x80);

	// Load the result into the accumulator (it's 8-bit dont forget!)
	A = temp & 0x00FF;

	// This instruction has the potential to require an additional clock cycle
	return 1;
}

// Instruction: Subtraction with Borrow In
// Function:    A = A - M - (1 - C)
// Flags Out:   C, V, N, Z
//
// Explanation:
// Given the explanation for ADC above, we can reorganise our data
// to use the same computation for addition, for subtraction by multiplying
// the data by -1, i.e. make it negative
//
// A = A - M - (1 - C)  ->  A = A + -1 * (M - (1 - C))  ->  A = A + (-M + 1 + C)
//
// To make a signed positive number negative, we can invert the bits and add 1
// (OK, I lied, a little bit of 1 and 2s complement :P)
//
//  5 = 00000101
// -5 = 11111010 + 00000001 = 11111011 (or 251 in our 0 to 255 range)
//
// The range is actually unimportant, because if I take the value 15, and add 251
// to it, given we wrap around at 256, the result is 10, so it has effectively 
// subtracted 5, which was the original intention. (15 + 251) % 256 = 10
//
// Note that the equation above used (1-C), but this got converted to + 1 + C.
// This means we already have the +1, so all we need to do is invert the bits
// of M, the data(!) therfore we can simply add, exactly the same way we did 
// before.

uint8_t CPU::SBC()
{
	Fetch();

	// Operating in 16-bit domain to capture carry out

	// We can invert the bottom 8 bits with bitwise xor
	uint16_t value = ((uint16_t)fetched_) ^ 0x00FF;

	// Notice this is exactly the same as addition from here!
	uint16_t temp = (uint16_t)A + value + (uint16_t)GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, ((temp & 0x00FF) == 0));
	SetFlag(V, (temp ^ (uint16_t)A) & (temp ^ value) & 0x0080);
	SetFlag(N, temp & 0x0080);
	A = temp & 0x00FF;
	return 1;
}

// OK! Complicated operations are done! the following are much simpler
// and conventional. The typical order of events is:
// 1) Fetch the data you are working with
// 2) Perform calculation
// 3) Store the result in desired place
// 4) Set Flags of the status register
// 5) Return if instruction has potential to require additional 
//    clock cycle


// Instruction: Bitwise Logic AND
// Function:    A = A & M
// Flags Out:   N, Z
uint8_t CPU::AND()
{
	Fetch();
	A = A & fetched_;
	SetFlag(Z, A == 0x00);	// Update zero result flag
	SetFlag(N, A & 0x80);	// Update negative result flag if 7th bit is equal to 1
	return 1;
}

// Instruction: Arithmetic Shift Left
// Function:    A = C <- (A << 1) <- 0
// Flags Out:   N, Z, C
uint8_t CPU::ASL()
{
	Fetch();
	uint16_t temp = (uint16_t)fetched_ << 1;
	SetFlag(C, (temp & 0xFF00) > 0);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x80);
	if (instructions_[opcode_].address_mode== &CPU::IMP) {
		A = temp & 0x00FF;
	}
	else {
		WriteBus(address_abs_, temp & 0x00FF);
	}
	return 0;
}

// Instruction: Branch if Carry Clear
// Function:    if(C == 0) pc = address 
uint8_t CPU::BCC()
{
	if (GetFlag(C) == 0)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Branch if Carry Set
// Function:    if(C == 1) pc = address
uint8_t CPU::BCS()
{
	if (GetFlag(C) == 1)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00)) {
			cycles_++;
		}

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Branch if Equal
// Function:    if(Z == 1) pc = address
uint8_t CPU::BEQ()
{
	if (GetFlag(Z) == 1)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

uint8_t CPU::BIT()
{
	Fetch();
	uint16_t temp = A & fetched_;
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, fetched_ & (1 << 7));
	SetFlag(V, fetched_ & (1 << 6));
	return 0;
}

// Instruction: Branch if Negative
// Function:    if(N == 1) pc = address
uint8_t CPU::BMI()
{
	if (GetFlag(N) == 1)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Branch if Not Equal
// Function:    if(Z == 0) pc = address
uint8_t CPU::BNE()
{
	if (GetFlag(Z) == 0)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Branch if Positive
// Function:    if(N == 0) pc = address
uint8_t CPU::BPL()
{
	if (GetFlag(N) == 0)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Break
// Function:    Program Sourced Interrupt
uint8_t CPU::BRK()
{
	PC++;

	SetFlag(I, 1);
	WriteBus(SP_BASE + SP, (PC >> 8) & 0x00FF);
	SP--;
	WriteBus(SP_BASE + SP, PC & 0x00FF);
	SP--;

	SetFlag(B, 1);
	WriteBus(SP_BASE + SP, S);
	SP--;
	SetFlag(B, 0);

	PC = (uint16_t)ReadBus(0xFFFE) | ((uint16_t)ReadBus(0xFFFF) << 8);
	return 0;
}

// Instruction: Branch if Overflow Clear
// Function:    if(V == 0) pc = address
uint8_t CPU::BVC()
{
	if (GetFlag(V) == 0)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Branch if Overflow Set
// Function:    if(V == 1) pc = address
uint8_t CPU::BVS()
{
	if (GetFlag(V) == 1)
	{
		cycles_++;
		address_abs_ = PC + address_rel_;

		// Check if branch requires to cross a page boundary -> extra cycle
		if ((address_abs_ & 0xFF00) != (PC & 0xFF00))
			cycles_++;

		PC = address_abs_;
	}
	return 0;
}

// Instruction: Clear Carry Flag
// Function:    C = 0
uint8_t CPU::CLC()
{
	SetFlag(C, false);
	return 0;
}

// Instruction: Clear Decimal Flag
// Function:    D = 0
uint8_t CPU::CLD()
{
	SetFlag(D, false);
	return 0;
}

// Instruction: Disable Interrupts / Clear Interrupt Flag
// Function:    I = 0
uint8_t CPU::CLI()
{
	SetFlag(I, false);
	return 0;
}


// Instruction: Clear Overflow Flag
// Function:    V = 0
uint8_t CPU::CLV()
{
	SetFlag(V, false);
	return 0;
}

// Instruction: Compare Accumulator
// Function:    C <- A >= M      Z <- (A - M) == 0
// Flags Out:   N, C, Z
uint8_t CPU::CMP()
{
	Fetch();
	uint16_t temp = (uint16_t)A - (uint16_t)fetched_;
	SetFlag(C, A >= fetched_);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 1;
}

// Instruction: Compare X Register
// Function:    C <- X >= M      Z <- (X - M) == 0
// Flags Out:   N, C, Z
uint8_t CPU::CPX()
{
	Fetch();
	uint16_t temp = (uint16_t)X - (uint16_t)fetched_;
	SetFlag(C, X >= fetched_);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}

// Instruction: Compare Y Register
// Function:    C <- Y >= M      Z <- (Y - M) == 0
// Flags Out:   N, C, Z
uint8_t CPU::CPY()
{
	Fetch();
	uint16_t temp = (uint16_t)Y - (uint16_t)fetched_;
	SetFlag(C, Y >= fetched_);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}

// Instruction: Decrement Value at Memory Location
// Function:    M = M - 1
// Flags Out:   N, Z
uint8_t CPU::DEC()
{
	uint16_t temp = Fetch() - 1;
	WriteBus(address_abs_, temp & 0x00FF);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}

// Instruction: Decrement X Register
// Function:    X = X - 1
// Flags Out:   N, Z
uint8_t CPU::DEX()
{
	X--;
	SetFlag(Z, X == 0x00);
	SetFlag(N, X & 0x80);
	return 0;
}

// Instruction: Decrement Y Register
// Function:    Y = Y - 1
// Flags Out:   N, Z
uint8_t CPU::DEY()
{
	Y--;
	SetFlag(Z, Y == 0x00);
	SetFlag(N, Y & 0x80);
	return 0;
}

// Instruction: Bitwise Logic XOR
// Function:    A = A xor M
// Flags Out:   N, Z
uint8_t CPU::EOR()
{
	A = A ^ Fetch();
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 1;
}

// Instruction: Increment Value at Memory Location
// Function:    M = M + 1
// Flags Out:   N, Z
uint8_t CPU::INC()
{
	uint16_t temp = Fetch() + 1;
	WriteBus(address_abs_, temp & 0x00FF);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	return 0;
}

// Instruction: Increment X Register
// Function:    X = X + 1
// Flags Out:   N, Z
uint8_t CPU::INX()
{
	X++;
	SetFlag(Z, X == 0x00);
	SetFlag(N, X & 0x80);
	return 0;
}

// Instruction: Increment Y Register
// Function:    Y = Y + 1
// Flags Out:   N, Z
uint8_t CPU::INY()
{
	Y++;
	SetFlag(Z, Y == 0x00);
	SetFlag(N, Y & 0x80);
	return 0;
}

// Instruction: Jump To Location
// Function:    pc = address
uint8_t CPU::JMP()
{
	PC = address_abs_;
	return 0;
}

// Instruction: Jump To Sub-Routine
// Function:    Push current pc to stack, pc = address
uint8_t CPU::JSR()
{
	PC--;

	WriteBus(SP_BASE + SP, (PC >> 8) & 0x00FF);
	SP--;
	WriteBus(SP_BASE + SP, PC & 0x00FF);
	SP--;

	PC = address_abs_;
	return 0;
}

// Instruction: Load The Accumulator
// Function:    A = M
// Flags Out:   N, Z
uint8_t CPU::LDA()
{
	A = Fetch();
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 1;
}

// Instruction: Load The X Register
// Function:    X = M
// Flags Out:   N, Z
uint8_t CPU::LDX()
{
	X = Fetch();
	SetFlag(Z, X == 0x00);
	SetFlag(N, X & 0x80);
	return 1;
}

// Instruction: Load The Y Register
// Function:    Y = M
// Flags Out:   N, Z
uint8_t CPU::LDY()
{
	Y = Fetch();
	SetFlag(Z, Y == 0x00);
	SetFlag(N, Y & 0x80);
	return 1;
}

uint8_t CPU::LSR()
{
	Fetch();
	SetFlag(C, fetched_ & 0x0001);
	uint16_t temp = fetched_ >> 1;
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	if (instructions_[opcode_].address_mode == &CPU::IMP)
		A = temp & 0x00FF;
	else
		WriteBus(address_abs_, temp & 0x00FF);
	return 0;
}

uint8_t CPU::NOP()
{
	// Sadly not all NOPs are equal, Ive added a few here
	// based on https://wiki.nesdev.com/w/index.php/CPU_unofficial_opcodes
	// and will add more based on game compatibility, and ultimately
	// I'd like to cover all illegal opcodes too
	switch (opcode_) {
	case 0x1C:
	case 0x3C:
	case 0x5C:
	case 0x7C:
	case 0xDC:
	case 0xFC:
		return 1;
		break;
	}
	return 0;
}

// Instruction: Bitwise Logic OR
// Function:    A = A | M
// Flags Out:   N, Z
uint8_t CPU::ORA()
{
	Fetch();
	A = A | fetched_;
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 1;
}

// Instruction: Push Accumulator to Stack
// Function:    A -> stack
uint8_t CPU::PHA()
{
	WriteBus(SP_BASE + SP, A);
	SP--;
	return 0;
}

// Instruction: Push Status Register to Stack
// Function:    status -> stack
// Note:        Break flag is set to 1 before push
uint8_t CPU::PHP()
{
	WriteBus(SP_BASE + SP, S | B | U);
	SetFlag(B, 0);
	SetFlag(U, 0);
	SP--;
	return 0;
}

// Instruction: Pop Accumulator off Stack
// Function:    A <- stack
// Flags Out:   N, Z
uint8_t CPU::PLA()
{
	SP++;
	A = ReadBus(SP_BASE + SP);
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 0;
}

// Instruction: Pop Status Register off Stack
// Function:    Status <- stack
uint8_t CPU::PLP()
{
	SP++;
	S = ReadBus(SP_BASE + SP);
	SetFlag(U, 1);
	return 0;
}

uint8_t CPU::ROL()
{
	Fetch();
	uint16_t temp = (uint16_t)(fetched_ << 1) | GetFlag(C);
	SetFlag(C, temp & 0xFF00);
	SetFlag(Z, (temp & 0x00FF) == 0x0000);
	SetFlag(N, temp & 0x0080);
	if (instructions_[opcode_].address_mode== &CPU::IMP)
		A = temp & 0x00FF;
	else
		WriteBus(address_abs_, temp & 0x00FF);
	return 0;
}

uint8_t CPU::ROR()
{
	Fetch();
	uint16_t temp = (uint16_t)(GetFlag(C) << 7) | (fetched_ >> 1);
	SetFlag(C, fetched_ & 0x01);
	SetFlag(Z, (temp & 0x00FF) == 0x00);
	SetFlag(N, temp & 0x0080);
	if (instructions_[opcode_].address_mode == &CPU::IMP) {
		A = temp & 0x00FF;
	}
	else {
		WriteBus(address_abs_, temp & 0x00FF);
	}
	return 0;
}

uint8_t CPU::RTI()
{
	SP++;
	S = ReadBus(SP_BASE + SP);
	S &= ~B;
	S &= ~U;

	SP++;
	PC = (uint16_t)ReadBus(SP_BASE + SP);
	SP++;
	PC |= (uint16_t)ReadBus(SP_BASE + SP) << 8;
	return 0;
}

uint8_t CPU::RTS()
{
	SP++;
	PC = (uint16_t)ReadBus(SP_BASE + SP);
	SP++;
	PC |= (uint16_t)ReadBus(SP_BASE + SP) << 8;

	PC++;
	return 0;
}

// Instruction: Set Carry Flag
// Function:    C = 1
uint8_t CPU::SEC()
{
	SetFlag(C, true);
	return 0;
}


// Instruction: Set Decimal Flag
// Function:    D = 1
uint8_t CPU::SED()
{
	SetFlag(D, true);
	return 0;
}

// Instruction: Set Interrupt Flag / Enable Interrupts
// Function:    I = 1
uint8_t CPU::SEI()
{
	SetFlag(I, true);
	return 0;
}

// Instruction: Store Accumulator at Address
// Function:    M = A
uint8_t CPU::STA()
{
	WriteBus(address_abs_, A);
	return 0;
}

// Instruction: Store X Register at Address
// Function:    M = X
uint8_t CPU::STX()
{
	WriteBus(address_abs_, X);
	return 0;
}

// Instruction: Store Y Register at Address
// Function:    M = Y
uint8_t CPU::STY()
{
	WriteBus(address_abs_, Y);
	return 0;
}

// Instruction: Transfer Accumulator to X Register
// Function:    X = A
// Flags Out:   N, Z
uint8_t CPU::TAX()
{
	X = A;
	SetFlag(Z, X == 0x00);
	SetFlag(N, X & 0x80);
	return 0;
}

// Instruction: Transfer Accumulator to Y Register
// Function:    Y = A
// Flags Out:   N, Z
uint8_t CPU::TAY()
{
	Y = A;
	SetFlag(Z, Y == 0x00);
	SetFlag(N, Y & 0x80);
	return 0;
}

// Instruction: Transfer Stack Pointer to X Register
// Function:    X = stack pointer
// Flags Out:   N, Z
uint8_t CPU::TSX()
{
	X = SP;
	SetFlag(Z, X == 0x00);
	SetFlag(N, X & 0x80);
	return 0;
}

// Instruction: Transfer X Register to Accumulator
// Function:    A = X
// Flags Out:   N, Z
uint8_t CPU::TXA()
{
	A = X;
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 0;
}

// Instruction: Transfer X Register to Stack Pointer
// Function:    stack pointer = X
uint8_t CPU::TXS()
{
	SP = X;
	return 0;
}

// Instruction: Transfer Y Register to Accumulator
// Function:    A = Y
// Flags Out:   N, Z
uint8_t CPU::TYA()
{
	A = Y;
	SetFlag(Z, A == 0x00);
	SetFlag(N, A & 0x80);
	return 0;
}

// This function captures illegal opcodes
uint8_t CPU::XXX()
{
	return 0;
}

/* Instructions END */

void CPU::Clock()
{
	// Each instruction requires a variable number of clock cycles to execute.
	// In my emulation, I only care about the final result and so I perform
	// the entire computation in one hit. In hardware, each clock cycle would
	// perform "microcode" style transformations of the CPUs state.
	//
	// To remain compliant with connected devices, it's important that the 
	// emulation also takes "time" in order to execute instructions, so I
	// implement that delay by simply counting down the cycles required by 
	// the instruction. When it reaches 0, the instruction is complete, and
	// the next one is ready to be executed.
	if (cycles_ == 0) {
		// Read next instruction byte. This 8-bit value is used to index
		// the translation table to get the relevant information about
		// how to implement the instruction
		opcode_ = ReadBus(PC);

#ifdef LOGMODE
		uint16_t log_pc = PC;
#endif

		// Always set the unused status flag bit to 1
		SetFlag(U, true);

		// Increment program counter, we read the opcode byte
		PC++;

		// Get Starting number of cycles
		cycles_ = instructions_[opcode_].cycles;

		// Perform fetch of intermmediate data using the
		// required addressing mode
		uint8_t additional_cycle1 = (this->*instructions_[opcode_].address_mode)();

		// Perform operation
		uint8_t additional_cycle2 = (this->*instructions_[opcode_].operate)();

		// The addressmode and opcode may have altered the number
		// of cycles this instruction requires before its completed
		cycles_ += (additional_cycle1 & additional_cycle2);

		// Always set the unused status flag bit to 1
		SetFlag(U, true);

#ifdef LOGMODE
		// This logger dumps every cycle the entire processor state for analysis.
		// This can be used for debugging the emulation, but has little utility
		// during emulation. Its also very slow, so only use if you have to.
		if (logfile_ == nullptr)	logfile_ = fopen("CPU.txt", "wt");
		if (logfile_ != nullptr)
		{
			fprintf(logfile_, "%10d:%02d PC:%04X %s A:%02X X:%02X Y:%02X %s%s%s%s%s%s%s%s SP:%02X\n",
				clock_count_, 0, log_pc, "XXX", A, X, Y,
				GetFlag(N) ? "N" : ".", GetFlag(V) ? "V" : ".", GetFlag(U) ? "U" : ".",
				GetFlag(B) ? "B" : ".", GetFlag(D) ? "D" : ".", GetFlag(I) ? "I" : ".",
				GetFlag(Z) ? "Z" : ".", GetFlag(C) ? "C" : ".", SP);
		}
#endif
	}

	// Increment global clock count - This is actually unused unless logging is enabled
	// but I've kept it in because its a handy watch variable for debugging
	clock_count_++;

	// Decrement the number of cycles remaining for this instruction
	cycles_--;
}

void CPU::Reset()
{
	// Get address to set program counter to
	address_abs_ = 0xFFFC;
	uint16_t lo = ReadBus(address_abs_ + 0);
	uint16_t hi = ReadBus(address_abs_ + 1);

	// Set it
	PC = (hi << 8) | lo;

	// Reset internal registers
	A = X = Y = 0;
	SP = 0xFD;
	S = 0x00 | U;

	// Clear internal helper variables
	address_rel_ = address_abs_ = fetched_ = 0x00;

	// Reset takes time
	cycles_ = 8;
}

uint8_t CPU::Fetch()
{
	if (!(instructions_[opcode_].address_mode == &CPU::IMP)) {
		fetched_ = ReadBus(address_abs_);
	}

	return fetched_;
}

// Interrupt requests are a complex operation and only happen if the
// "disable interrupt" flag is 0. IRQs can happen at any time, but
// you dont want them to be destructive to the operation of the running 
// program. Therefore the current instruction is allowed to finish
// (which I facilitate by doing the whole thing when cycles == 0) and 
// then the current program counter is stored on the stack. Then the
// current status register is stored on the stack. When the routine
// that services the interrupt has finished, the status register
// and program counter can be restored to how they where before it 
// occurred. This is impemented by the "RTI" instruction. Once the IRQ
// has happened, in a similar way to a reset, a programmable address
// is read form hard coded location 0xFFFE, which is subsequently
// set to the program counter.
void CPU::IRQ()
{
	// If interrupts are allowed
	if (GetFlag(I) == 0)
	{
		// Push the program counter to the stack. It's 16-bits dont
		// forget so that takes two pushes
		WriteBus(SP_BASE + SP, (PC >> 8) & 0x00FF);
		SP--;
		WriteBus(SP_BASE + SP, PC & 0x00FF);
		SP--;

		// Then Push the status register to the stack
		SetFlag(B, 0);
		SetFlag(U, 1);
		SetFlag(I, 1);
		WriteBus(SP_BASE + SP, S);
		SP--;

		// Read new program counter location from fixed address
		address_abs_ = 0xFFFE;
		uint16_t lo = ReadBus(address_abs_ + 0);
		uint16_t hi = ReadBus(address_abs_ + 1);
		PC = (hi << 8) | lo;

		// IRQs take time
		cycles_ = 7;
	}
}

// A Non-Maskable Interrupt cannot be ignored. It behaves in exactly the
// same way as a regular IRQ, but reads the new program counter address
// form location 0xFFFA.
void CPU::NMI()
{
	WriteBus(SP_BASE + SP, (PC >> 8) & 0x00FF);
	SP--;
	WriteBus(SP_BASE + SP, PC & 0x00FF);
	SP--;

	SetFlag(B, 0);
	SetFlag(U, 1);
	SetFlag(I, 1);
	WriteBus(SP_BASE + SP, S);
	SP--;

	address_abs_= 0xFFFA;
	uint16_t lo = ReadBus(address_abs_ + 0);
	uint16_t hi = ReadBus(address_abs_ + 1);
	PC = (hi << 8) | lo;

	cycles_ = 8;
}


