#include "C2EngineDisplayAccess.h"
#include "C2MapGeometry.h"

#include <cstddef>
#include <cstring>

namespace NC2E2
{
    // Original function: FUN_004821c7
    void DisplayDescription_GetMapRectangle(const void* display, std::uint32_t pixelX, std::uint32_t pixelY, std::uint32_t pixelWidth, std::uint32_t pixelHeight, std::int16_t* rectangle)
    {
        const auto readWord = [] (const void* base, std::uint32_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(base) + offset, sizeof(value));
            return value;
        };

        const auto signedDivide = [] (std::uint32_t numerator, std::uint32_t divisor) -> std::int32_t
        {
            std::int32_t signedNumerator;
            std::int32_t signedDivisor;
            std::memcpy(&signedNumerator, &numerator, sizeof(signedNumerator));
            std::memcpy(&signedDivisor, &divisor, sizeof(signedDivisor));

            return signedNumerator / signedDivisor;
        };

        const auto storeShort = [] (std::int16_t* destination, std::int32_t value)
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(value);
            std::memcpy(destination, &bits, sizeof(bits));
        };

        const std::int32_t mapY = signedDivide(pixelY << 8, readWord(display, 0x0C));
        storeShort(rectangle + 1, mapY);

        const std::uint32_t rowOffset = (static_cast<std::uint32_t>(mapY) & 1u) * readWord(display, 0x2C);
        const std::uint32_t adjustedX = (pixelX + rowOffset) << 8;
        const std::int32_t mapX = signedDivide(adjustedX, readWord(display, 0x10));
        storeShort(rectangle, mapX);

        const std::int32_t mapWidth = signedDivide(pixelWidth, readWord(display, 0x24));
        storeShort(rectangle + 2, mapWidth);

        const std::int32_t mapHeight = signedDivide(pixelHeight, readWord(display, 0x20));
        storeShort(rectangle + 3, mapHeight);

        NC2Logic::MapMigRectangle_CutInsideMap(rectangle);
    }
}