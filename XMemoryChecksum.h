#pragma once

#include <cstdint>

namespace NXBasics
{
	// Original function: FUN_00410b85
	inline std::uint32_t XB_GetMemoryChecksum(const void* data, std::uint32_t byteCount)
	{
		const auto* bytes = static_cast<const std::uint8_t*>(data);

		std::uint32_t word = 0;
		std::uint32_t bytesInGroup = 0;
		std::uint32_t checksum = 0x6515D4F1u;
		std::uint32_t state = 0x6FD318ABu;

		for (std::uint32_t index = 0; index < byteCount; ++index)
		{
			word = (word << 8) | bytes[index];
			++bytesInGroup;

			if (bytesInGroup == 4)
			{
				state ^= word;
				bytesInGroup = 0;
				checksum ^= state;
				state += 0x63FE53BAu;
			}
		}

		return word ^ checksum;
	}
}