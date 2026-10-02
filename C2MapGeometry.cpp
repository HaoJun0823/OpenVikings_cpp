#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2LifetimeGlobals.h"

#include <cstdint>
#include <cstddef>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_00423761.
    bool MapMagPoint_IsInsideMap(const void* packedMagPoint)
    {
        std::int16_t point[2];
        std::memcpy(point, packedMagPoint, sizeof(point));
        return point[1] >= 0 && point[1] < gMapPointDimensions.macroHeight && point[0] >= 0 && point[0] < gMapPointDimensions.macroWidth;
    }
    // Original function: FUN_0042b33b.
    bool MapMagPoint_IsBorderPoint(const void* packedMagPoint)
    {
        std::int16_t point[2];
        std::memcpy(point, packedMagPoint, sizeof(point));
        return point[0] < 2 || point[0] >= static_cast<std::int32_t>(gMapPointDimensions.macroWidth) - 2 || point[1] < 2 || point[1] >= static_cast<std::int32_t>(gMapPointDimensions.macroHeight) - 2;
    }
    // Original function: FUN_0042ad4e.
    bool MapMigPoint_IsBorderPoint(const void* packedMigPoint)
    {
        std::int16_t point[2], macro[2];
        std::memcpy(point, packedMigPoint, sizeof(point));
        VE_MapCoords_MicroGridMapToMacroGridMapPoint(point[0], point[1], &macro[0], &macro[1]);
        return MapMagPoint_IsBorderPoint(macro);
    }
    static std::int16_t WrapCoordinate(std::int16_t coordinate, std::int16_t dimension)
    {
        if (coordinate >= dimension) return static_cast<std::int16_t>(coordinate % dimension);
        if (coordinate < 0)
        {
            const auto result = static_cast<std::int16_t>(dimension - (-static_cast<std::int32_t>(coordinate) % dimension));
            return result == dimension ? 0 : result;
        }
        return coordinate;
    }

    // Original function: FUN_0042a5f1.
    void VE_MapCoords_MicroGridMapToMacroGridMapPoint(std::int16_t x, std::int16_t y, std::int16_t* macroX, std::int16_t* macroY)
    {
        const auto row = static_cast<std::uint16_t>(y) & 3u;
        *macroX = static_cast<std::int16_t>((static_cast<std::int32_t>(x) + (row == 1 ? 1 : row == 2 ? -1 : 0)) >> 1);
        *macroY = static_cast<std::int16_t>(y >> 1);
    }
    // Original function: FUN_0042ac91; SMapMigPoint::ValidatePosition wraps coordinates.
    void MapMigPoint_WrapPosition(void* packedMigPoint)
    {
        std::int16_t point[2];
        std::memcpy(point, packedMigPoint, sizeof(point));
        point[0] = WrapCoordinate(point[0], gMapPointDimensions.microWidth);
        point[1] = WrapCoordinate(point[1], gMapPointDimensions.microHeight);
        std::memcpy(packedMigPoint, point, sizeof(point));
    }
    // Original function: FUN_0042b1ea; SMapMagPoint::ValidatePosition.
    void MapMagPoint_ValidatePosition(void* packedMagPoint)
    {
        std::int16_t point[2];
        std::memcpy(point, packedMagPoint, sizeof(point));
        point[0] = WrapCoordinate(point[0], gMapPointDimensions.macroWidth);
        point[1] = WrapCoordinate(point[1], gMapPointDimensions.macroHeight);
        std::memcpy(packedMagPoint, point, sizeof(point));
    }
    // Original function: FUN_0042af98.
    void* MapMagPoint_ConstructFromMig(void* packedMagPoint, const void* packedMigPoint, bool validate)
    {
        std::int16_t point[2];
        std::memcpy(point, packedMigPoint, sizeof(point));
        VE_MapCoords_MicroGridMapToMacroGridMapPoint(point[0], point[1], static_cast<std::int16_t*>(packedMagPoint), reinterpret_cast<std::int16_t*>(static_cast<std::byte*>(packedMagPoint) + 2));
        if (validate) MapMagPoint_ValidatePosition(packedMagPoint);
        return packedMagPoint;
    }
    // Original function: FUN_0042a9e7.
    void* MapMigPoint_ConstructOffset(void* packedMigPoint, const void* source, std::int16_t xOffset, std::int16_t yOffset, bool validate)
    {
        std::uint16_t point[2];
        std::memcpy(point, source, sizeof(point));
        point[0] = static_cast<std::uint16_t>(point[0] + static_cast<std::uint16_t>(xOffset));
        if ((static_cast<std::uint16_t>(yOffset) & 1u) != 0 && (point[1] & 1u) != 0) ++point[0];
        point[1] = static_cast<std::uint16_t>(point[1] + static_cast<std::uint16_t>(yOffset));
        std::memcpy(packedMigPoint, point, sizeof(point));
        if (validate) MapMigPoint_WrapPosition(packedMigPoint);
        return packedMigPoint;
    }
    // Original function: FUN_0042adec; the argument is a packed DWORD, not a pointer.
    void MapMigPoint_AssignLong(void* packedMigPoint, std::uint32_t value)
    {
        const std::uint16_t x = static_cast<std::uint16_t>(value);
        const std::uint16_t y = static_cast<std::uint16_t>(value >> 16);
        std::memcpy(packedMigPoint, &x, sizeof(x));
        std::memcpy(static_cast<std::byte*>(packedMigPoint) + 2, &y, sizeof(y));
    }

    // Original function: FUN_0042a83e; complete eight-entry table at 004f3a18.
    std::uint32_t VE_HexagonDirection_OppositeDirection(std::uint32_t direction)
    {
        static constexpr std::uint32_t oppositeDirections[8] = { 3, 4, 5, 0, 1, 2, 7, 6 };
        return oppositeDirections[direction];
    }

    // Original function: FUN_0042adff; sign-extended X participates in the OR.
    std::uint32_t MapMigPoint_GetLong(const void* packedMigPoint)
    {
        std::int16_t coordinates[2];
        std::memcpy(coordinates, packedMigPoint, sizeof(coordinates));
        return static_cast<std::uint32_t>(coordinates[0]) | (static_cast<std::uint32_t>(coordinates[1]) << 16);
    }

    // Original function: FUN_0042abdd; distinct jump tables for odd and even rows.
    void MapMigPoint_MoveCursorUnwrapped(std::uint32_t& packedMigPoint, std::uint32_t direction)
    {
        if (direction > 7u)
            return;

        const bool oddRow = (packedMigPoint & 0x10000u) != 0;
        static constexpr std::int32_t oddDeltaX[8] = { 1, 1, 0, -1, 0, 1, 0, 0 };
        static constexpr std::int32_t evenDeltaX[8] = { 1, 0, -1, -1, -1, 0, 0, 0 };
        static constexpr std::int32_t deltaY[8] = { 0, 1, 1, 0, -1, -1, -2, 2 };
        const std::uint16_t x = static_cast<std::uint16_t>(packedMigPoint + static_cast<std::uint32_t>(oddRow ? oddDeltaX[direction] : evenDeltaX[direction]));
        const std::uint16_t y = static_cast<std::uint16_t>((packedMigPoint >> 16) + static_cast<std::uint32_t>(deltaY[direction]));
        packedMigPoint = static_cast<std::uint32_t>(x) | (static_cast<std::uint32_t>(y) << 16);
    }

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
        const std::int32_t mapWidth = GameMap_GetWidth();

        if (right > mapWidth)
            storeShort(rectangle[2], mapWidth - rectangle[0]);

        const std::int32_t bottom = static_cast<std::int32_t>(rectangle[1]) + rectangle[3];
        const std::int32_t mapHeight = GameMap_GetHeight();

        if (bottom > mapHeight)
            storeShort(rectangle[3], mapHeight - rectangle[1]);
    }

    // Original function: FUN_0040b580
    bool MapMigPoint_IsInsideMap(const void* packedMigPoint)
    {
        std::int16_t coordinates[2];
        std::memcpy(coordinates, packedMigPoint, sizeof(coordinates));

        if (coordinates[1] < 0 || coordinates[1] >= GameMap_GetHeight())
            return false;

        if (coordinates[0] < 0 || coordinates[0] >= GameMap_GetWidth())
            return false;

        return true;
    }

    // Original function: FUN_0042ad18
    void MapMigPoint_ValidatePosition(void* packedMigPoint)
    {
        std::int16_t coordinates[2];
        std::memcpy(coordinates, packedMigPoint, sizeof(coordinates));

        const auto storeLowWord = [] (std::int16_t& destination, std::int32_t value)
        {
            const std::uint16_t bits = static_cast<std::uint16_t>(value);
            std::memcpy(&destination, &bits, sizeof(destination));
        };

        if (coordinates[0] < 0)
            coordinates[0] = 0;

        const std::int16_t mapWidth = GameMap_GetWidth();

        if (coordinates[0] >= mapWidth)
            storeLowWord(coordinates[0], static_cast<std::int32_t>(mapWidth) - 1);

        if (coordinates[1] < 0)
            coordinates[1] = 0;

        const std::int16_t mapHeight = GameMap_GetHeight();

        if (coordinates[1] >= mapHeight)
            storeLowWord(coordinates[1], static_cast<std::int32_t>(mapHeight) - 1);

        std::memcpy(packedMigPoint, coordinates, sizeof(coordinates));
    }
}
