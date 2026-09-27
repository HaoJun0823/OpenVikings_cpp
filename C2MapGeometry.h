#pragma once

#include <cstdint>

namespace NC2Logic
{
    std::int32_t VE_HexagonDirection_GetDistance(std::int16_t sourceX, std::int16_t sourceY, std::int16_t targetX, std::int16_t targetY);
    void MapMigRectangle_CutInsideMap(std::int16_t* rectangle);
}