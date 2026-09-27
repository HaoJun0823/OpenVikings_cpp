#include "C2MapGeometry.h"

#include <cstring>

extern std::int16_t DAT_005125f8;
extern std::int16_t DAT_005125fa;

namespace NC2Logic
{
    // Original function: FUN_0042a935
    std::int32_t VE_HexagonDirection_GetDistance(std::int16_t sourceX, std::int16_t sourceY, std::int16_t targetX, std::int16_t targetY)
    {
        const auto signedLowWord = [] (std::int32_t value) -> std::int32_t
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(value);
            std::int16_t result;
            std::memcpy(&result, &bits, sizeof(result));
            return result;
        };

        const std::int32_t deltaX = static_cast<std::int32_t>(targetX) - sourceX;
        const std::int32_t deltaY = static_cast<std::int32_t>(targetY) - sourceY;
        const std::int32_t signedDeltaX = signedLowWord(deltaX);

        std::int32_t horizontal = signedDeltaX < 0 ? -deltaX : deltaX;
        std::int32_t vertical = signedLowWord(deltaY) < 0 ? -deltaY : deltaY;

        if ((static_cast<std::uint32_t>(vertical) & 1u) != 0)
        {
            const bool targetRowIsOdd = (static_cast<std::uint16_t>(targetY) & 1u) != 0;

            if ((targetRowIsOdd && signedDeltaX < 0) || (!targetRowIsOdd && signedDeltaX > 0))
                --horizontal;
        }

        vertical = signedLowWord(vertical);
        horizontal = signedLowWord(horizontal);

        const std::int32_t halfVertical = vertical / 2;

        if (horizontal > halfVertical)
            return vertical + horizontal - halfVertical;

        return vertical;
    }

    // Original function: FUN_0042ae99
    void MapMigRectangle_CutInsideMap(std::int16_t* rectangle)
    {
        const auto storeShort = [] (std::int16_t& destination, std::int32_t value)
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(value);
            std::memcpy(&destination, &bits, sizeof(destination));
        };

        if (rectangle[0] < 0)
        {
            storeShort(rectangle[2], static_cast<std::int32_t>(rectangle[2]) + rectangle[0]);
            rectangle[0] = 0;
        }

        if (rectangle[1] < 0)
        {
            storeShort(rectangle[3], static_cast<std::int32_t>(rectangle[3]) + rectangle[1]);
            rectangle[1] = 0;
        }

        const std::int32_t right = static_cast<std::int32_t>(rectangle[0]) + rectangle[2];
        const std::int32_t mapWidth = DAT_005125f8;

        if (right > mapWidth)
            storeShort(rectangle[2], mapWidth - rectangle[0]);

        const std::int32_t bottom = static_cast<std::int32_t>(rectangle[1]) + rectangle[3];
        const std::int32_t mapHeight = DAT_005125fa;

        if (bottom > mapHeight)
            storeShort(rectangle[3], mapHeight - rectangle[1]);
    }
}