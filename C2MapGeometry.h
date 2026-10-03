#pragma once

#include <cstdint>

namespace NC2Logic
{
    std::uint32_t VE_HexagonDirection_OppositeDirection(std::uint32_t direction);
    std::int32_t VE_HexagonDirection_GetDistance(std::int16_t sourceX, std::int16_t sourceY, std::int16_t targetX, std::int16_t targetY);
    void MapMigRectangle_CutInsideMap(std::int16_t* rectangle);

    // Original function: FUN_0040b580
    bool MapMigPoint_IsInsideMap(const void* packedMigPoint);
    bool MapMagPoint_IsInsideMap(const void* packedMagPoint);
    bool MapMagPoint_IsBorderPoint(const void* packedMagPoint);
    bool MapMigPoint_IsBorderPoint(const void* packedMigPoint);

    // Original function: FUN_0042ad18
    void MapMigPoint_ValidatePosition(void* packedMigPoint);
    void MapMigPoint_WrapPosition(void* packedMigPoint);
    void MapMagPoint_ValidatePosition(void* packedMagPoint);
    void VE_MapCoords_MicroGridMapToMacroGridMapPoint(std::int16_t x, std::int16_t y, std::int16_t* macroX, std::int16_t* macroY);
    void* MapMagPoint_ConstructFromMig(void* packedMagPoint, const void* packedMigPoint, bool validate);
    void* MapMigPoint_ConstructOffset(void* packedMigPoint, const void* source, std::int16_t xOffset, std::int16_t yOffset, bool validate);
    std::uint32_t MapMigPoint_GetLong(const void* packedMigPoint);
    void MapMigPoint_AssignLong(void* packedMigPoint, std::uint32_t value);
    void MapMigPoint_MoveCursorUnwrapped(std::uint32_t& packedMigPoint, std::uint32_t direction);
    void MapMigPoint_MoveCursorUnwrapped(void* packedMigPoint, std::uint32_t direction, std::uint32_t distance);
}
