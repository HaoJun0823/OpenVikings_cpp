#pragma once

#include <cstdint>
#include <cstring>

namespace NXBasics
{
	static_assert(sizeof(void*) == 4, "These routines require an x86 build");

	// Original function: FUN_00410900
	inline void __cdecl XB_Tool_Byte_FillBlock(std::uint8_t* pixels, std::uint32_t width, std::int32_t height, std::uint32_t rowPixelCount, std::uint8_t color)
	{
		if (height <= 0)
		{
			return;
		}

		std::uintptr_t rowAddress = reinterpret_cast<std::uintptr_t>(pixels);

		for (std::int32_t rowsRemaining = height; rowsRemaining > 0; --rowsRemaining)
		{
			std::memset(reinterpret_cast<void*>(rowAddress), color, width);
			rowAddress += rowPixelCount;
		}
	}

	// Original function: FUN_0041095a
	inline void __cdecl XB_Tool_Word_FillBlock(std::uint16_t* pixels, std::int32_t width, std::int32_t height, std::uint32_t rowPixelCount, std::uint16_t color)
	{
		if (height <= 0)
		{
			return;
		}

		const std::uint32_t rowByteCount = rowPixelCount * 2u;
		std::uintptr_t rowAddress = reinterpret_cast<std::uintptr_t>(pixels);

		if (color == 0)
		{
			const std::uint32_t fillByteCount = static_cast<std::uint32_t>(width) * 2u;

			for (std::int32_t rowsRemaining = height; rowsRemaining > 0; --rowsRemaining)
			{
				std::memset(reinterpret_cast<void*>(rowAddress), 0, fillByteCount);
				rowAddress += rowByteCount;
			}

			return;
		}

		const std::uint32_t pairedColor = (static_cast<std::uint32_t>(color) << 16) | color;

		for (std::int32_t rowsRemaining = height; rowsRemaining > 0; --rowsRemaining)
		{
			if (width > 0)
			{
				std::uintptr_t writeAddress = rowAddress;
				const std::uint32_t pairCount = static_cast<std::uint32_t>(width) >> 1;

				for (std::uint32_t pair = 0; pair < pairCount; ++pair)
				{
					std::memcpy(reinterpret_cast<void*>(writeAddress), &pairedColor, sizeof(pairedColor));
					writeAddress += 4u;
				}

				if ((static_cast<std::uint32_t>(width) & 1u) != 0)
				{
					std::memcpy(reinterpret_cast<void*>(writeAddress), &color, sizeof(color));
				}
			}

			rowAddress += rowByteCount;
		}
	}

	// Original function: FUN_00410a0a
	inline void __cdecl XB_Tool_Long_FillBlock(std::uint32_t* pixels, std::int32_t width, std::int32_t height, std::uint32_t rowPixelCount, std::uint32_t color)
	{
		if (height <= 0)
		{
			return;
		}

		const std::uint32_t rowByteCount = rowPixelCount * 4u;
		std::uintptr_t rowAddress = reinterpret_cast<std::uintptr_t>(pixels);

		if (color == 0)
		{
			const std::uint32_t fillByteCount = static_cast<std::uint32_t>(width) * 4u;

			for (std::int32_t rowsRemaining = height; rowsRemaining > 0; --rowsRemaining)
			{
				std::memset(reinterpret_cast<void*>(rowAddress), 0, fillByteCount);
				rowAddress += rowByteCount;
			}

			return;
		}

		for (std::int32_t rowsRemaining = height; rowsRemaining > 0; --rowsRemaining)
		{
			if (width > 0)
			{
				std::uintptr_t writeAddress = rowAddress;

				for (std::int32_t column = 0; column < width; ++column)
				{
					std::memcpy(reinterpret_cast<void*>(writeAddress), &color, sizeof(color));
					writeAddress += 4u;
				}
			}

			rowAddress += rowByteCount;
		}
	}
}