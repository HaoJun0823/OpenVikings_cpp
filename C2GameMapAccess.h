#pragma once

#include <cstdint>

namespace NC2Logic
{
    // Original storage: signed word at DAT_00511570.
    std::int16_t GameMap_GetWidth();

    // Original storage: signed word at DAT_00511570 + 0x02.
    std::int16_t GameMap_GetHeight();

    // Extracted from FUN_0042329f: releases the three map buffers only.
    void GameMap_ReleaseMapBuffers();

    // Original function: FUN_004234c4
    void GameMap_InitAndCreateEmptyMap(std::int16_t macroWidth, std::int16_t macroHeight);

    // Original function: FUN_0042378b
    const void* GameMap_GetConstMapMacroPointPtr(std::int16_t x, std::int16_t y);

    // Original function: FUN_004237d2
    const void* GameMap_GetConstMapMicroPointPtr(const void* packedMigPoint);
    void* GameMap_GetMapMicroPointPtr(const void* packedMigPoint);
    std::int16_t GameMap_CalculationMap_GetValue(const void* packedMigPoint);
    void GameMap_CalculationMap_SetValue(const void* packedMigPoint, std::int16_t value);
    void GameMap_CalculationMap_Clear(const void* packedMigPoint, std::int32_t radius);
    std::uint32_t GameMap_Continent_GetContinentTyp(std::uint32_t continentId);

    // Original function: FUN_00423fb0
    std::uint8_t GameMap_GetHeightAtMapMigPoint(const void* packedMigPoint);
}
