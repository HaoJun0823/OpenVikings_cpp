#pragma once

#include <cstdint>

namespace NC2Logic
{
    std::int32_t VE_HexagonDirection_GetDistance(std::int16_t sourceX, std::int16_t sourceY, std::int16_t targetX, std::int16_t targetY);
    void MapMigRectangle_CutInsideMap(std::int16_t* rectangle);

    // Original function: FUN_0040b580
    bool MapMigPoint_IsInsideMap(const void* packedMigPoint);

    // Original function: FUN_0042ad18
    void MapMigPoint_ValidatePosition(void* packedMigPoint);
    void MapMigPoint_MoveCursorUnwrapped(std::uint32_t& packedMigPoint, std::uint32_t direction);
}
