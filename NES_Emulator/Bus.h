#pragma once

#include <cstdint>
#include <array>

/*
* General BUS class.
*/
class Bus
{
public:
	Bus();
	~Bus();

private: //Devices
	std::array<uint8_t, 64 * 1024> ram_; // Temporary RAM emulation

private: // Methods
	void ClearRAM();

public: // Methods
	void Write(uint16_t address, uint8_t data);
	uint8_t Read(uint16_t address);
};

