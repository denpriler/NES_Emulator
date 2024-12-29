#include "Bus.h"

#define RAM_ADDR_START 0x0000
#define RAM_ADDR_END 0xFFFF

Bus::Bus()
{
	ClearRAM();
}

Bus::~Bus()
{
}

void Bus::ClearRAM()
{
	for (uint8_t &i : ram_) {
		ram_[i] = 0x000;
	}
}

void Bus::Write(uint16_t address, uint8_t data)
{
	if (address >= RAM_ADDR_START && address <= RAM_ADDR_END) {
		ram_[address] = data;
	}
}

uint8_t Bus::Read(uint16_t address)
{
	if (address >= RAM_ADDR_START && address <= RAM_ADDR_END) {
		return ram_[address];
	}

	return 0x000;
}
