#include "XBobRendering.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCPU.h"
#include "XCrtFileAccess.h"
#include "XDiagnostics.h"
#include "XPaletteAccess.h"
#include "XPixelFormatConverter.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mmintrin.h>

namespace NXBasics
{
	static std::byte* ShadowAddress(const void* base, std::uint32_t offset)
	{
		const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base)) + offset;
		return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(address));
	}

	template<typename T>
	static T ShadowRead(const void* base, std::uint32_t offset = 0)
	{
		T value;
		std::memcpy(&value, ShadowAddress(base, offset), sizeof(value));
		return value;
	}

	static std::byte* ShadowPointer(const void* base, std::uint32_t offset)
	{
		return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(ShadowRead<std::uint32_t>(base, offset)));
	}

	template<typename T, typename Transform>
	static void PrintShadowRuns(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, bool advanceClippedRuns, Transform transform)
	{
		const std::uint32_t index = bobId - ShadowRead<std::uint32_t>(bobManager, 4);

		if (index >= ShadowRead<std::uint32_t>(bobManager, 8))
			return;

		const std::byte* entry = ShadowAddress(ShadowPointer(bobManager, 0x24), index * 24u);

		if (ShadowRead<std::uint32_t>(entry) != 2u)
			return;

		SRectangle placed(0, 0, 0, 0);
		std::memcpy(&placed, entry + 4, sizeof(placed));

		placed.x = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
		placed.y = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

		SRectangle bounds(0, 0, 0, 0);
		std::memcpy(&bounds, reinterpret_cast<const std::byte*>(bitmap) + 8, sizeof(bounds));

		if (!placed.IsTouching(bounds))
			return;

		SRectangle clipped = placed;
		clipped.CutInside(bounds);

		std::byte* rows = ShadowAddress(ShadowPointer(bobManager, 0x2C), ShadowRead<std::uint32_t>(entry, 0x14) * 4u);

		// The original skips rows only for a negative placed Y coordinate.
		if (placed.y < 0)
			rows = ShadowAddress(rows, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

		const std::int32_t placedRight = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
		const bool clipRight = placedRight >= Runtime_TimeSigned32(ShadowRead<std::uint32_t>(bitmap, 0x10));

		if constexpr (sizeof(T) == 2)
		{
			// This path checks only the low WORD of the bitmap depth.
			if (ShadowRead<std::uint16_t>(bitmap, 4) != 1u || g16BitPixelConverter == nullptr)
				return;
		}
		else
		{
			if (ShadowRead<std::uint32_t>(bitmap, 4) != 2u)
				return;
		}

		if (clipped.height <= 0)
			return;

		const std::int32_t clippedRight = Runtime_TimeSigned32(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);
		std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);
		std::uint32_t rowCounter = 0;

		do
		{
			const std::uint32_t descriptor = ShadowRead<std::uint32_t>(rows);
			rows = ShadowAddress(rows, 4);

			if (descriptor != 0xFFFFFFFFu)
			{
				std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 22);
				std::byte* source = ShadowAddress(ShadowPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

				const std::uint32_t rowIndex = ShadowRead<std::uint32_t>(bitmap, 0x30) * destinationY;
				constexpr std::uint32_t pixelBytes = static_cast<std::uint32_t>(sizeof(T));
				constexpr std::uint32_t pixelPointerOffset = sizeof(T) == 2 ? 0x28u : 0x2Cu;

				std::byte* destinationRow = ShadowAddress(ShadowPointer(bitmap, pixelPointerOffset), rowIndex * pixelBytes);
				bool unchecked = !clipRight && Runtime_TimeSigned32(currentX) >= 0;

				for (;;)
				{
					const std::uint8_t token = ShadowRead<std::uint8_t>(source);
					source = ShadowAddress(source, 1);

					if (token == 0)
						break;

					const std::uint32_t count = token & 0x7Fu;

					if ((token & 0x80u) != 0)
					{
						currentX += count;
					}
					else if (unchecked || advanceClippedRuns)
					{
						for (std::uint32_t pixel = 0; pixel < count; ++pixel)
						{
							const std::int32_t signedX = Runtime_TimeSigned32(currentX);

							// The original clips against zero, not clipped.x.
							if (unchecked || (signedX >= 0 && signedX <= clippedRight))
							{
								if (advanceClippedRuns)
								{
									std::byte* destination = ShadowAddress(destinationRow, currentX * pixelBytes);
									const T value = transform(ShadowRead<T>(destination));
									std::memcpy(destination, &value, sizeof(value));
								}
							}

							++currentX;
						}
					}

					// Once entered, the unchecked path stays active for this row.
					if (!unchecked && !clipRight && Runtime_TimeSigned32(currentX) >= 0)
						unchecked = true;
				}
			}

			++destinationY;
			++rowCounter;

			// Preserve the original WORD truncation in the loop comparison.
		} while (static_cast<std::int32_t>(static_cast<std::uint16_t>(rowCounter)) < clipped.height);
	}

	// Original function: FUN_00439bc2
	std::uint32_t BobManager_GetBobType(const void* bobManager, std::uint32_t bobId)
	{
		const std::uint32_t index = bobId - ShadowRead<std::uint32_t>(bobManager, 4);

		if (index >= ShadowRead<std::uint32_t>(bobManager, 8))
			return 0;

		const std::byte* entry = ShadowAddress(ShadowPointer(bobManager, 0x24), index * 24u);

		if (ShadowRead<std::uint32_t>(entry) == 0)
			return 0;

		return ShadowRead<std::uint32_t>(entry);
	}

    // Original function: FUN_00439ab3; byte and alpha-word run streams share hit
    // intervals, but skip one and two payload bytes per opaque pixel respectively.
    bool BobManager_IsBobHit(const void* bobManager, std::uint32_t bobId, const std::int32_t* point)
    {
        const auto index = bobId - ShadowRead<std::uint32_t>(bobManager, 4);
        if (index >= ShadowRead<std::uint32_t>(bobManager, 8)) return false;
        const auto* entry = ShadowAddress(ShadowPointer(bobManager, 0x24), index * 0x18u);
        const auto left = ShadowRead<std::int32_t>(entry, 4), top = ShadowRead<std::int32_t>(entry, 8);
        const auto right = Runtime_TimeSigned32(ShadowRead<std::uint32_t>(entry, 4) + ShadowRead<std::uint32_t>(entry, 0xC) - 1u);
        const auto bottom = Runtime_TimeSigned32(ShadowRead<std::uint32_t>(entry, 8) + ShadowRead<std::uint32_t>(entry, 0x10) - 1u);
        if (point[0] < left || point[0] > right || point[1] < top || point[1] > bottom) return false;
        const auto row = ShadowRead<std::uint32_t>(entry, 0x14) - static_cast<std::uint32_t>(top) + static_cast<std::uint32_t>(point[1]);
        const auto descriptor = ShadowRead<std::uint32_t>(ShadowPointer(bobManager, 0x2C), row * 4u);
        if (descriptor == 0xFFFFFFFFu) return false;
        const auto x = Runtime_TimeSigned32(static_cast<std::uint32_t>(point[0]) - static_cast<std::uint32_t>(left));
        std::uint32_t runX = descriptor >> 22;
        if (x < static_cast<std::int32_t>(runX)) return false;
        const auto type = ShadowRead<std::uint32_t>(entry);
        if (type != 1 && type != 4) return false;
        const auto* cursor = reinterpret_cast<const std::uint8_t*>(ShadowPointer(bobManager, 0x28)) + (descriptor & 0x3FFFFFu);
        for (;;)
        {
            const auto control = *cursor++;
            if (control == 0) return false;
            const auto length = static_cast<std::uint32_t>(control & 0x7Fu);
            if ((control & 0x80u) == 0)
            {
                if (x >= Runtime_TimeSigned32(runX) && x < Runtime_TimeSigned32(runX + length)) return true;
                cursor += length * (type == 1 ? 1u : 2u);
            }
            runX += length;
        }
    }

	// Original function: FUN_00463aee
	void BobManager_PrintBob(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette)
	{
		static_assert(sizeof(void*) == 4);

		const auto readWord = [] (const void* address) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, address, sizeof(value));
			return value;
		};

		const auto offsetAddress = [] (const void* address, std::uint32_t offset) -> const std::byte*
		{
			const std::uintptr_t result = reinterpret_cast<std::uintptr_t>(address) + offset;
			return reinterpret_cast<const std::byte*>(result);
		};

		const auto readPointer = [&] (const void* object, std::uint32_t offset) -> const std::byte*
		{
			const std::byte* value;
			std::memcpy(&value, offsetAddress(object, offset), sizeof(value));
			return value;
		};

		const auto signedValue = [] (std::uint32_t value) -> std::int32_t
		{
			return Runtime_TimeSigned32(value);
		};

		const std::uint32_t index = bobId - readWord(offsetAddress(bobManager, 4));

		if (index >= readWord(offsetAddress(bobManager, 8)))
			return;

		const std::byte* entry = offsetAddress(readPointer(bobManager, 0x24), index * 24u);

		if (readWord(entry) == 0)
			return;

		SRectangle placed(0, 0, 0, 0);
		std::memcpy(&placed, entry + 4, sizeof(placed));

		placed.x = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
		placed.y = signedValue(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

		SRectangle bounds(0, 0, 0, 0);
		std::memcpy(&bounds, offsetAddress(bitmap, 8), sizeof(bounds));

		if (!placed.IsTouching(bounds))
			return;

		SRectangle clipped = placed;
		clipped.CutInside(bounds);

		const bool wasClipped = clipped.x != placed.x || clipped.y != placed.y || clipped.width != placed.width || clipped.height != placed.height;
		const std::byte* rowTable = offsetAddress(readPointer(bobManager, 0x2C), readWord(entry + 0x14) * 4u);

		if (placed.y < 0)
			rowTable = offsetAddress(rowTable, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

		const std::int32_t placedRight = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
		const bool clipRight = placedRight >= signedValue(readWord(offsetAddress(bitmap, 0x10)));

		if (readWord(entry) != 1)
			return;

		if (palette == nullptr)
			return;

		const std::uint32_t depth = readWord(offsetAddress(bitmap, 4));
		const std::uint16_t* highColorTable = nullptr;
		const std::uint32_t* trueColorTable = nullptr;
		std::uint32_t pixelBytes;
		std::uint32_t pixelPointerOffset;

		if (depth == 0)
		{
			pixelBytes = 1;
			pixelPointerOffset = 0x24;
		}
		else if (depth == 1)
		{
			highColorTable = Palette_GetHighColorTablePtr(palette);

			if (highColorTable == nullptr)
				return;

			pixelBytes = 2;
			pixelPointerOffset = 0x28;
		}
		else if (depth == 2)
		{
			trueColorTable = Palette_GetTrueColorTablePtr(palette);

			if (trueColorTable == nullptr)
				return;

			pixelBytes = 4;
			pixelPointerOffset = 0x2C;
		}
		else
		{
			return;
		}

		const auto writePixel = [&] (const std::byte* address, std::uint8_t colourIndex)
		{
			if (depth == 0)
			{
				*const_cast<std::byte*>(address) = static_cast<std::byte>(colourIndex);
			}
			else if (depth == 1)
			{
				const std::uint16_t pixel = highColorTable[colourIndex];
				std::memcpy(const_cast<std::byte*>(address), &pixel, sizeof(pixel));
			}
			else
			{
				const std::uint32_t pixel = trueColorTable[colourIndex];
				std::memcpy(const_cast<std::byte*>(address), &pixel, sizeof(pixel));
			}
		};

		const std::int32_t clippedRight = signedValue(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);
		std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);

		for (std::int32_t row = 0; row < clipped.height; ++row, ++destinationY)
		{
			const std::uint32_t descriptor = readWord(rowTable);
			rowTable = offsetAddress(rowTable, 4);

			if (descriptor == 0xFFFFFFFFu)
				continue;

			// The disassembly's shift count 16 is hexadecimal (22 bits).
			std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 22);
			const std::byte* input = offsetAddress(readPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

			bool fast = depth == 1 && !wasClipped;
			bool fastPrepared = false;
			const std::byte* output = nullptr;

			for (;;)
			{
				if (!fast && signedValue(currentX) >= 0 && !clipRight)
					fast = true;

				if (fast && !fastPrepared)
				{
					const std::uint32_t pixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY + currentX;
					output = offsetAddress(readPointer(bitmap, pixelPointerOffset), pixelIndex * pixelBytes);
					fastPrepared = true;
				}

				const std::uint8_t token = static_cast<std::uint8_t>(*input);
				input = offsetAddress(input, 1);

				if (token == 0)
					break;

				const std::uint32_t count = token & 0x7Fu;

				if ((token & 0x80u) != 0)
				{
					if (fast)
						output = offsetAddress(output, count * pixelBytes);
					else
						currentX += count;

					continue;
				}

				if (fast)
				{
					for (std::uint32_t pixel = 0; pixel < count; ++pixel)
					{
						const std::uint8_t colourIndex = static_cast<std::uint8_t>(*input);

						writePixel(output, colourIndex);

						input = offsetAddress(input, 1);
						output = offsetAddress(output, pixelBytes);
					}
				}
				else
				{
					const std::uint32_t rowPixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY;
					const std::byte* rowPixels = offsetAddress(readPointer(bitmap, pixelPointerOffset), rowPixelIndex * pixelBytes);
					const std::byte* pixelAddress = offsetAddress(rowPixels, currentX * pixelBytes);

					for (std::uint32_t pixel = 0; pixel < count; ++pixel)
					{
						if (signedValue(currentX) >= 0 && signedValue(currentX) <= clippedRight)
						{
							const std::uint8_t colourIndex = static_cast<std::uint8_t>(*input);
							writePixel(pixelAddress, colourIndex);
						}

						++currentX;
						input = offsetAddress(input, 1);
						pixelAddress = offsetAddress(pixelAddress, pixelBytes);
					}
				}
			}
		}
	}

	// Original function: FUN_00464314
	void BobManager_PrintBob_DoubleByteBobNormal(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette)
	{
		const std::uint32_t index = bobId - ShadowRead<std::uint32_t>(bobManager, 4);

		if (index >= ShadowRead<std::uint32_t>(bobManager, 8))
			return;

		const std::byte* entry = ShadowAddress(ShadowPointer(bobManager, 0x24), index * 24u);

		if (ShadowRead<std::uint32_t>(entry) == 0)
			return;

		SRectangle placed(0, 0, 0, 0);
		std::memcpy(&placed, entry + 4, sizeof(placed));

		placed.x = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
		placed.y = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

		SRectangle bounds(0, 0, 0, 0);
		std::memcpy(&bounds, reinterpret_cast<const std::byte*>(bitmap) + 8, sizeof(bounds));

		if (!placed.IsTouching(bounds))
			return;

		SRectangle clipped = placed;
		clipped.CutInside(bounds);

		const bool wasClipped = clipped.x != placed.x || clipped.y != placed.y || clipped.width != placed.width || clipped.height != placed.height;

		std::byte* rows = ShadowAddress(ShadowPointer(bobManager, 0x2C), ShadowRead<std::uint32_t>(entry, 0x14) * 4u);

		// Preserve the original row-table adjustment.
		if (placed.y < 0)
			rows = ShadowAddress(rows, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

		const std::int32_t placedRight = Runtime_TimeSigned32(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
		const bool clipRight = placedRight >= Runtime_TimeSigned32(ShadowRead<std::uint32_t>(bitmap, 0x10));

		if (ShadowRead<std::uint32_t>(entry) != 4u)
			return;

		const std::uint32_t depth = ShadowRead<std::uint32_t>(bitmap, 4);
		const void* colorTable;
		std::uint32_t pixelBytes;
		std::uint32_t pixelPointerOffset;

		if (depth == 1u)
		{
			colorTable = Palette_GetHighColorTablePtr(palette);
			pixelBytes = 2;
			pixelPointerOffset = 0x28;
		}
		else if (depth == 2u)
		{
			colorTable = Palette_GetTrueColorTablePtr(palette);
			pixelBytes = 4;
			pixelPointerOffset = 0x2C;
		}
		else
		{
			return;
		}

		if (colorTable == nullptr)
			return;

		const std::int32_t clippedRight = Runtime_TimeSigned32(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);
		std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);

		for (std::int32_t row = 0; row < clipped.height; ++row, ++destinationY)
		{
			const std::uint32_t descriptor = ShadowRead<std::uint32_t>(rows);
			rows = ShadowAddress(rows, 4);

			if (descriptor == 0xFFFFFFFFu)
				continue;

			std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 22);
			std::byte* source = ShadowAddress(ShadowPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

			const std::uint32_t rowIndex = ShadowRead<std::uint32_t>(bitmap, 0x30) * destinationY;
			std::byte* destinationRow = ShadowAddress(ShadowPointer(bitmap, pixelPointerOffset), rowIndex * pixelBytes);

			// Only the 16-bit path has a separate wholly unclipped branch.
			bool unchecked = (depth == 1u && !wasClipped) || (!clipRight && Runtime_TimeSigned32(currentX) >= 0);

			for (;;)
			{
				const std::uint8_t token = ShadowRead<std::uint8_t>(source);
				source = ShadowAddress(source, 1);

				if (token == 0)
					break;

				const std::uint32_t count = token & 0x7Fu;

				if ((token & 0x80u) != 0)
				{
					currentX += count;
				}
				else
				{
					for (std::uint32_t pixel = 0; pixel < count; ++pixel)
					{
						const std::int32_t signedX = Runtime_TimeSigned32(currentX);

						// The original lower clipping bound is zero.
						if (unchecked || (signedX >= 0 && signedX <= clippedRight))
						{
							const std::uint8_t colorIndex = ShadowRead<std::uint8_t>(source);
							std::byte* destination = ShadowAddress(destinationRow, currentX * pixelBytes);

							if (depth == 1u)
							{
								const std::uint16_t color = ShadowRead<std::uint16_t>(colorTable, static_cast<std::uint32_t>(colorIndex) * 2u);
								std::memcpy(destination, &color, sizeof(color));
							}
							else
							{
								const std::uint32_t color = ShadowRead<std::uint32_t>(colorTable, static_cast<std::uint32_t>(colorIndex) * 4u);
								std::memcpy(destination, &color, sizeof(color));
							}
						}

						// Normal drawing ignores the second byte of each pair.
						source = ShadowAddress(source, 2);
						++currentX;
					}
				}

				if (!unchecked && !clipRight && Runtime_TimeSigned32(currentX) >= 0)
					unchecked = true;
			}
		}
	}

	// Original function: FUN_0049b3ee
	void BobManager_PrintBob_Shadow(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		std::uint32_t depth;
		std::memcpy(&depth, reinterpret_cast<const std::byte*>(bitmap) + 4, sizeof(depth));

		switch (depth)
		{
			case 0:
				ReportWarning("NXBasics::CBobManager::PrintBob_Shadow\n\nUnsupported bitmap depth!");
				break;

			case 1:
				BobManager_PrintBob_Shadow16(bobManager, bobId, bitmap, x, y, 0);
				break;

			case 2:
				BobManager_PrintBob_Shadow32(bobManager, bobId, bitmap, x, y, 0);
				break;
		}
	}

	// Original function: FUN_0049b437
	void BobManager_PrintBob_Shadow16(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, std::uint32_t mode)
	{
		const auto transform = [mode] (std::uint16_t pixel) -> std::uint16_t
		{
			switch (mode)
			{
				case 0:
					{
						const std::byte* table = ShadowPointer(g16BitPixelConverter, 8);
						return ShadowRead<std::uint16_t>(table, static_cast<std::uint32_t>(pixel) * 2u);
					}

				case 1:
					return g16BitPixelConverter->ApplyShadowMode1(pixel);

				case 2:
					return g16BitPixelConverter->ApplyShadowMode2(pixel);

				case 3:
					return g16BitPixelConverter->ApplyShadowMode3(pixel);
			}

			return pixel;
		};

		PrintShadowRuns<std::uint16_t>(bobManager, bobId, bitmap, x, y, mode <= 3u, transform);
	}

	// Original function: FUN_0049b930
	void BobManager_PrintBob_Shadow32(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, std::uint32_t /*mode*/)
	{
		const auto transform = [] (std::uint32_t pixel) -> std::uint32_t
		{
			// Preserve the original masks and blue-channel adjustment.
			const std::uint32_t upper = ((pixel >> 15) & 0x1FFFEu) / 3u;
			const std::uint32_t blue = ((pixel & 0xFFu) * 2u) / 3u;
			const std::uint32_t green = ((pixel >> 7) & 0x1FEu) / 3u;

			return (((green | (upper << 8)) << 8) | (blue + ((blue + upper) >> 4)));
		};

		PrintShadowRuns<std::uint32_t>(bobManager, bobId, bitmap, x, y, true, transform);
	}

	// Original function: FUN_0046534f
	void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette)
	{
		static_assert(sizeof(void*) == 4);

		const auto readWord = [] (const void* address) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, address, sizeof(value));
			return value;
		};

		const auto readHalfWord = [] (const void* address) -> std::uint16_t
		{
			std::uint16_t value;
			std::memcpy(&value, address, sizeof(value));
			return value;
		};

		const auto offsetAddress = [] (const void* address, std::uint32_t offset) -> const std::byte*
		{
			const std::uintptr_t result = reinterpret_cast<std::uintptr_t>(address) + offset;
			return reinterpret_cast<const std::byte*>(result);
		};

		const auto readPointer = [&] (const void* object, std::uint32_t offset) -> const std::byte*
		{
			const std::byte* value;
			std::memcpy(&value, offsetAddress(object, offset), sizeof(value));
			return value;
		};

		const auto signedValue = [] (std::uint32_t value) -> std::int32_t
		{
			return Runtime_TimeSigned32(value);
		};

		const std::uint32_t index = bobId - readWord(offsetAddress(bobManager, 4));

		if (index >= readWord(offsetAddress(bobManager, 8)))
			return;

		const std::byte* entry = offsetAddress(readPointer(bobManager, 0x24), index * 24u);

		if (readWord(entry) == 0)
			return;

		SRectangle placed(0, 0, 0, 0);
		std::memcpy(&placed, entry + 4, sizeof(placed));

		placed.x = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
		placed.y = signedValue(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

		SRectangle bounds(0, 0, 0, 0);
		std::memcpy(&bounds, offsetAddress(bitmap, 8), sizeof(bounds));

		if (!placed.IsTouching(bounds))
			return;

		SRectangle clipped = placed;
		clipped.CutInside(bounds);

		const bool wasClipped = clipped.x != placed.x || clipped.y != placed.y || clipped.width != placed.width || clipped.height != placed.height;
		const std::byte* rowTable = offsetAddress(readPointer(bobManager, 0x2C), readWord(entry + 0x14) * 4u);

		if (placed.y < 0)
			rowTable = offsetAddress(rowTable, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

		const std::int32_t placedRight = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
		const bool clipRight = placedRight >= signedValue(readWord(offsetAddress(bitmap, 0x10)));

		if (readWord(entry) != 4)
			return;

		const std::uint32_t depth = readWord(offsetAddress(bitmap, 4));

		if (depth == 0)
			return;

		if (depth != 1 && depth != 2)
		{
			_mm_empty();
			return;
		}

		const std::uint16_t* highColorTable = nullptr;
		const std::uint32_t* trueColorTable = nullptr;
		std::uint32_t greenMask = 0;
		bool useMmx;

		if (depth == 2)
		{
			trueColorTable = Palette_GetTrueColorTablePtr(palette);

			if (trueColorTable == nullptr)
				return;

			useMmx = CPU_HasMMX();
		}
		else
		{
			highColorTable = Palette_GetHighColorTablePtr(palette);

			if (highColorTable == nullptr)
				return;

			const auto* converterBytes = reinterpret_cast<const std::byte*>(g16BitPixelConverter);
			greenMask = converterBytes[0x3A] != std::byte{ 0 } ? 0x07E0u : 0x03E0u;

			useMmx = CPU_HasMMX();

			if (useMmx)
			{
				// The original requests this table but does not consume the result.
				Palette_GetTrueColorTablePtr(palette);
			}
		}

		const auto blend = [&] (std::uint32_t destination, std::uint32_t source, std::uint32_t alpha, bool fast) -> std::uint32_t
		{
			const std::uint32_t inverseAlpha = 256u - alpha;

			if (depth == 2)
			{
				const std::uint32_t channelCount = useMmx && fast ? 4u : 3u;
				std::uint32_t result = 0;

				for (std::uint32_t channel = 0; channel < channelCount; ++channel)
				{
					const std::uint32_t shift = channel * 8u;
					const std::uint32_t destinationChannel = (destination >> shift) & 0xFFu;
					const std::uint32_t sourceChannel = (source >> shift) & 0xFFu;
					const std::uint32_t blendedChannel = (destinationChannel * inverseAlpha + sourceChannel * alpha) >> 8;

					result |= blendedChannel << shift;
				}

				return result;
			}

			if (useMmx && fast)
			{
				const std::uint32_t otherMask = (~greenMask) & 0xFFFFu;
				const std::uint32_t destinationOther = destination & otherMask;
				const std::uint32_t sourceOther = source & otherMask;

				const std::uint32_t lowByte = ((destinationOther & 0xFFu) * inverseAlpha + (sourceOther & 0xFFu) * alpha) >> 8;
				const std::uint32_t highByte = ((destinationOther >> 8) * inverseAlpha + (sourceOther >> 8) * alpha) >> 8;
				const std::uint32_t other = (lowByte | (highByte << 8)) & otherMask;

				const std::uint32_t destinationGreen = (destination & greenMask) >> 4;
				const std::uint32_t sourceGreen = (source & greenMask) >> 4;
				const std::uint32_t green = ((destinationGreen * inverseAlpha + sourceGreen * alpha) >> 4) & greenMask;

				return other + green;
			}

			// Preserve the scalar RGB555 red-mask defect.
			const std::uint32_t red = (((destination & 0xF800u) * inverseAlpha + (source & 0xF800u) * alpha) >> 8) & 0xF800u;
			const std::uint32_t green = (((destination & greenMask) * inverseAlpha + (source & greenMask) * alpha) >> 8) & greenMask;
			const std::uint32_t blue = ((destination & 0x1Fu) * inverseAlpha + (source & 0x1Fu) * alpha) >> 8;

			return red + green + blue;
		};

		const auto drawPixel = [&] (const std::byte* output, const std::byte* input, bool fast)
		{
			const std::uint8_t colourIndex = static_cast<std::uint8_t>(input[0]);
			const std::uint32_t alpha = static_cast<std::uint8_t>(input[1]);

			if (depth == 2)
			{
				const std::uint32_t destination = readWord(output);
				const std::uint32_t source = trueColorTable[colourIndex];
				const std::uint32_t result = blend(destination, source, alpha, fast);

				std::memcpy(const_cast<std::byte*>(output), &result, sizeof(result));
			}
			else
			{
				const std::uint16_t destination = readHalfWord(output);
				const std::uint16_t source = highColorTable[colourIndex];
				const std::uint16_t result = static_cast<std::uint16_t>(blend(destination, source, alpha, fast));

				std::memcpy(const_cast<std::byte*>(output), &result, sizeof(result));
			}
		};

		const std::uint32_t pixelBytes = depth == 2 ? 4u : 2u;
		const std::uint32_t pixelPointerOffset = depth == 2 ? 0x2Cu : 0x28u;
		const std::int32_t clippedRight = signedValue(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);
		std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);

		for (std::int32_t row = 0; row < clipped.height; ++row, ++destinationY)
		{
			const std::uint32_t descriptor = readWord(rowTable);
			rowTable = offsetAddress(rowTable, 4);

			if (descriptor == 0xFFFFFFFFu)
				continue;

			std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 16);
			const std::byte* input = offsetAddress(readPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

			bool fast = depth == 1 && !wasClipped;
			bool fastPrepared = false;
			const std::byte* output = nullptr;

			for (;;)
			{
				if (!fast && signedValue(currentX) >= 0 && !clipRight)
					fast = true;

				if (fast && !fastPrepared)
				{
					const std::uint32_t pixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY + currentX;
					output = offsetAddress(readPointer(bitmap, pixelPointerOffset), pixelIndex * pixelBytes);
					fastPrepared = true;
				}

				const std::uint8_t token = static_cast<std::uint8_t>(*input);
				input = offsetAddress(input, 1);

				if (token == 0)
					break;

				std::uint32_t count = token & 0x7Fu;

				if ((token & 0x80u) != 0)
				{
					if (fast)
						output = offsetAddress(output, count * pixelBytes);
					else
						currentX += count;

					continue;
				}

				if (fast)
				{
					if (depth == 1 && useMmx)
					{
						// Preserve the paired destination read and write of the MMX loop.
						while (count >= 2)
						{
							const std::uint32_t destinationPair = readWord(output);

							const std::uint8_t firstIndex = static_cast<std::uint8_t>(input[0]);
							const std::uint16_t firstSource = highColorTable[firstIndex];
							const std::uint32_t firstAlpha = static_cast<std::uint8_t>(input[1]);

							const std::uint8_t secondIndex = static_cast<std::uint8_t>(input[2]);
							const std::uint16_t secondSource = highColorTable[secondIndex];
							const std::uint32_t secondAlpha = static_cast<std::uint8_t>(input[3]);

							const std::uint32_t firstResult = blend(destinationPair & 0xFFFFu, firstSource, firstAlpha, true);
							const std::uint32_t secondResult = blend(destinationPair >> 16, secondSource, secondAlpha, true);
							const std::uint32_t resultPair = firstResult | (secondResult << 16);

							std::memcpy(const_cast<std::byte*>(output), &resultPair, sizeof(resultPair));

							input = offsetAddress(input, 4);
							output = offsetAddress(output, 4);
							count -= 2;
						}
					}

					while (count != 0)
					{
						drawPixel(output, input, true);

						input = offsetAddress(input, 2);
						output = offsetAddress(output, pixelBytes);
						--count;
					}
				}
				else
				{
					const std::uint32_t rowPixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY;
					const std::byte* rowPixels = offsetAddress(readPointer(bitmap, pixelPointerOffset), rowPixelIndex * pixelBytes);
					const std::byte* pixelAddress = offsetAddress(rowPixels, currentX * pixelBytes);

					while (count != 0)
					{
						if (signedValue(currentX) >= 0 && signedValue(currentX) <= clippedRight)
							drawPixel(pixelAddress, input, false);

						++currentX;
						input = offsetAddress(input, 2);
						pixelAddress = offsetAddress(pixelAddress, pixelBytes);
						--count;
					}
				}
			}
		}

		_mm_empty();
	}

	// Original function: FUN_004660c6
	void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour)
	{
		static_assert(sizeof(void*) == 4);

		const auto readWord = [] (const void* address) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, address, sizeof(value));
			return value;
		};

		const auto offsetAddress = [] (const void* address, std::uint32_t offset) -> const std::byte*
		{
			const std::uintptr_t result = reinterpret_cast<std::uintptr_t>(address) + offset;
			return reinterpret_cast<const std::byte*>(result);
		};

		const auto readPointer = [&] (const void* object, std::uint32_t offset) -> const std::byte*
		{
			const std::byte* value;
			std::memcpy(&value, offsetAddress(object, offset), sizeof(value));
			return value;
		};

		const auto signedValue = [] (std::uint32_t value) -> std::int32_t
		{
			return Runtime_TimeSigned32(value);
		};

		const auto blendChannel = [] (std::uint32_t destination, std::uint32_t source, std::uint32_t alpha) -> std::uint32_t
		{
			return (destination * (256u - alpha) + source * alpha) >> 8;
		};

		const std::uint32_t index = bobId - readWord(offsetAddress(bobManager, 4));

		if (index >= readWord(offsetAddress(bobManager, 8)))
			return;

		const std::byte* entry = offsetAddress(readPointer(bobManager, 0x24), index * 24u);

		if (readWord(entry) == 0)
			return;

		SRectangle placed(0, 0, 0, 0);
		std::memcpy(&placed, entry + 4, sizeof(placed));

		placed.x = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
		placed.y = signedValue(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

		SRectangle bounds(0, 0, 0, 0);
		std::memcpy(&bounds, offsetAddress(bitmap, 8), sizeof(bounds));

		if (!placed.IsTouching(bounds))
			return;

		SRectangle clipped = placed;
		clipped.CutInside(bounds);

		const bool wasClipped = clipped.x != placed.x || clipped.y != placed.y || clipped.width != placed.width || clipped.height != placed.height;

		const std::byte* rowTable = offsetAddress(readPointer(bobManager, 0x2C), readWord(entry + 0x14) * 4u);

		if (placed.y < 0)
			rowTable = offsetAddress(rowTable, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

		const std::int32_t placedRight = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
		const bool clipRight = placedRight >= signedValue(readWord(offsetAddress(bitmap, 0x10)));

		if (readWord(entry) != 1)
			return;

		const std::uint32_t depth = readWord(offsetAddress(bitmap, 4));

		if (depth == 0)
			return;

		if (depth != 1 && depth != 2)
		{
			_mm_empty();
			return;
		}

		std::uint32_t sourceColour = 0;
		std::uint32_t fastSourceColour = 0;
		std::uint32_t greenMask = 0;
		bool useMmx;

		if (depth == 2)
		{
			sourceColour = static_cast<std::uint32_t>(colour->blue);
			sourceColour |= static_cast<std::uint32_t>(colour->green) << 8;
			sourceColour |= static_cast<std::uint32_t>(colour->red) << 16;

			useMmx = CPU_HasMMX();
		}
		else
		{
			const std::byte* converterBytes = reinterpret_cast<const std::byte*>(g16BitPixelConverter);
			greenMask = converterBytes[0x3A] != std::byte{ 0 } ? 0x07E0u : 0x03E0u;
			sourceColour = g16BitPixelConverter->PackColor(*colour);

			useMmx = CPU_HasMMX();

			if (useMmx)
				fastSourceColour = g16BitPixelConverter->PackColor(*colour);
		}

		const auto blendPixel = [&] (const std::byte* address, std::uint32_t alpha, bool fast)
		{
			if (depth == 2)
			{
				const std::uint32_t destination = readWord(address);
				const bool mmxPixel = useMmx && fast;
				const std::uint32_t source = mmxPixel ? fastSourceColour : sourceColour;
				const std::uint32_t channelCount = mmxPixel ? 4u : 3u;
				std::uint32_t result = 0;

				for (std::uint32_t channel = 0; channel < channelCount; ++channel)
				{
					const std::uint32_t shift = channel * 8u;
					const std::uint32_t destinationChannel = (destination >> shift) & 0xFFu;
					const std::uint32_t sourceChannel = (source >> shift) & 0xFFu;

					result |= blendChannel(destinationChannel, sourceChannel, alpha) << shift;
				}

				std::memcpy(const_cast<std::byte*>(address), &result, sizeof(result));
			}
			else
			{
				std::uint16_t destinationWord;
				std::memcpy(&destinationWord, address, sizeof(destinationWord));

				const std::uint32_t destination = destinationWord;
				const std::uint32_t inverseAlpha = 256u - alpha;
				std::uint32_t result;

				if (useMmx && fast)
				{
					// Reproduce the independent MMX byte lanes without changing rounding.
					const std::uint32_t otherMask = (~greenMask) & 0xFFFFu;
					const std::uint32_t destinationOther = destination & otherMask;
					const std::uint32_t sourceOther = fastSourceColour & otherMask;

					const std::uint32_t lowByte = blendChannel(destinationOther & 0xFFu, sourceOther & 0xFFu, alpha);
					const std::uint32_t highByte = blendChannel(destinationOther >> 8, sourceOther >> 8, alpha);
					const std::uint32_t other = (lowByte | (highByte << 8)) & otherMask;

					const std::uint32_t destinationGreen = (destination & greenMask) >> 4;
					const std::uint32_t sourceGreen = (fastSourceColour & greenMask) >> 4;
					const std::uint32_t green = ((destinationGreen * inverseAlpha + sourceGreen * alpha) >> 4) & greenMask;

					result = other + green;
				}
				else
				{
					// The scalar path uses 0xF800 even with the RGB555 green mask.
					const std::uint32_t red = (((destination & 0xF800u) * inverseAlpha + (sourceColour & 0xF800u) * alpha) >> 8) & 0xF800u;
					const std::uint32_t green = (((destination & greenMask) * inverseAlpha + (sourceColour & greenMask) * alpha) >> 8) & greenMask;
					const std::uint32_t blue = ((destination & 0x1Fu) * inverseAlpha + (sourceColour & 0x1Fu) * alpha) >> 8;

					result = red + green + blue;
				}

				const std::uint16_t resultWord = static_cast<std::uint16_t>(result);
				std::memcpy(const_cast<std::byte*>(address), &resultWord, sizeof(resultWord));
			}
		};

		std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);
		const std::uint32_t pixelBytes = depth == 2 ? 4u : 2u;
		const std::uint32_t pixelPointerOffset = depth == 2 ? 0x2Cu : 0x28u;
		const std::int32_t clippedRight = signedValue(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);

		for (std::int32_t row = 0; row < clipped.height; ++row, ++destinationY)
		{
			const std::uint32_t descriptor = readWord(rowTable);
			rowTable = offsetAddress(rowTable, 4);

			if (descriptor == 0xFFFFFFFFu)
				continue;

			std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 16);
			const std::byte* input = offsetAddress(readPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

			bool fast = depth == 1 && !wasClipped;
			bool fastPrepared = false;
			const std::byte* output = nullptr;

			for (;;)
			{
				if (!fast && signedValue(currentX) >= 0 && !clipRight)
					fast = true;

				if (fast && !fastPrepared)
				{
					const std::uint32_t pixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY + currentX;
					output = offsetAddress(readPointer(bitmap, pixelPointerOffset), pixelIndex * pixelBytes);

					if (depth == 2 && useMmx)
					{
						fastSourceColour = readWord(colour);
					}

					fastPrepared = true;
				}

				const std::uint8_t token = static_cast<std::uint8_t>(*input);
				input = offsetAddress(input, 1);

				if (token == 0)
					break;

				const std::uint32_t count = token & 0x7Fu;

				if ((token & 0x80u) != 0)
				{
					if (fast)
						output = offsetAddress(output, count * pixelBytes);
					else
						currentX += count;

					continue;
				}

				if (fast)
				{
					for (std::uint32_t pixel = 0; pixel < count; ++pixel)
					{
						const std::uint32_t alpha = static_cast<std::uint8_t>(*input);
						input = offsetAddress(input, 1);

						blendPixel(output, alpha, true);
						output = offsetAddress(output, pixelBytes);
					}
				}
				else
				{
					const std::uint32_t rowPixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY;
					const std::byte* rowPixels = offsetAddress(readPointer(bitmap, pixelPointerOffset), rowPixelIndex * pixelBytes);
					const std::byte* pixelAddress = offsetAddress(rowPixels, currentX * pixelBytes);

					for (std::uint32_t pixel = 0; pixel < count; ++pixel)
					{
						if (signedValue(currentX) >= 0 && signedValue(currentX) <= clippedRight)
						{
							const std::uint32_t alpha = static_cast<std::uint8_t>(*input);
							blendPixel(pixelAddress, alpha, false);
						}

						++currentX;
						input = offsetAddress(input, 1);
						pixelAddress = offsetAddress(pixelAddress, pixelBytes);
					}
				}
			}
		}

		_mm_empty();
	}
}
