#include "C2MapSectorGridCursor.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_004584c7
    SSectorGridCursor::SSectorGridCursor(std::uint32_t packedMigPoint)
    {
        const std::uint16_t xBits = static_cast<std::uint16_t>(packedMigPoint);
        const std::uint16_t yBits = static_cast<std::uint16_t>(packedMigPoint >> 16);

        std::int16_t pointX;
        std::int16_t pointY;
        std::memcpy(&pointX, &xBits, sizeof(pointX));
        std::memcpy(&pointY, &yBits, sizeof(pointY));

        x = static_cast<std::int32_t>(pointX) / 20;
        y = static_cast<std::int32_t>(pointY) / 20;
    }

    // Original function: FUN_00458572
    void SSectorGridCursor::CutInsideMap()
    {
        const std::int32_t width = sGridWidth;

        if (x >= width)
        {
            const std::uint32_t bits = static_cast<std::uint32_t>(width) - 1u;
            std::memcpy(&x, &bits, sizeof(x));
        }
        else if (x < 0)
        {
            x = 0;
        }

        const std::int32_t height = sGridHeight;

        if (y >= height)
        {
            const std::uint32_t bits = static_cast<std::uint32_t>(height) - 1u;
            std::memcpy(&y, &bits, sizeof(y));
        }
        else if (y < 0)
        {
            y = 0;
        }
    }
}