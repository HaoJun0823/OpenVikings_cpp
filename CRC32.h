#pragma once

#include <cstdint>

namespace NCRC32
{
	// Original function: FUN_004f29c1
	void UpdateByte(std::uint32_t* checksum, std::uint8_t value);
}