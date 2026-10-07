#pragma once

#include <cstdint>

namespace NC2Logic
{
    // Original storage: signed word at DAT_00511570.
    std::int16_t GameMap_GetWidth();

    // Original storage: signed word at DAT_00511570 + 0x02.
    std::int16_t GameMap_GetHeight();
    std::uint16_t GameMap_GetMacroWidth();
    std::uint16_t GameMap_GetMacroHeight();

    // Extracted from FUN_0042329f: releases the three map buffers only.
    void GameMap_ReleaseMapBuffers();

    // Original function: FUN_004234c4
    void GameMap_InitAndCreateEmptyMap(std::int16_t macroWidth, std::int16_t macroHeight);

    // Original function: FUN_0042378b
    const void* GameMap_GetConstMapMacroPointPtr(std::int16_t x, std::int16_t y);

    // Original function: FUN_004237d2
    const void* GameMap_GetConstMapMicroPointPtr(const void* packedMigPoint);
    void* GameMap_GetMapMicroPointPtr(const void* packedMigPoint);
    bool GameMap_Misc_GetLandscapeWorkPosition(const void* realPosition, void* workPosition);
    bool GameMap_Misc_CanBeUsedAsLandscapeWorkPosition(const void* point);
    std::int16_t GameMap_CalculationMap_GetValue(const void* packedMigPoint);
    void GameMap_CalculationMap_SetValue(const void* packedMigPoint, std::int16_t value);
    void GameMap_CalculationMap_Clear(const void* packedMigPoint, std::int32_t radius);
    std::uint32_t GameMap_Continent_GetContinentTyp(std::uint32_t continentId);

    // Original function: FUN_00423fb0
    std::uint8_t GameMap_GetHeightAtMapMigPoint(const void* packedMigPoint);
    const void* GameMap_GetTrianglePatternDescriptionA(const void* packedMagPoint);
    const void* GameMap_GetTrianglePatternDescriptionB(const void* packedMagPoint);
    bool GameMap_Misc_GetTrianglePatternDescriptions_AroundWay(const void* packedMigPoint, std::int32_t direction, const void** descriptions);
    bool GameMap_Misc_GetTrianglePatternDescriptions_AroundPoint(const void* packedMigPoint, const void** descriptions);
    bool GameMap_Exploration_HasBeenSeenOnce(std::uint32_t player, const void* packedMigPoint);
    bool GameMap_Exploration_DoExplore(std::uint32_t player, const void* packedMigPoint, std::uint32_t radius);
    bool GameMap_Exploration_DoExploreAll(std::uint32_t player);
    void GameMapModifyManager_GameMapModified(std::uint32_t type, const void* packedMigPoint, std::uint32_t parameter);
    void GameMapModifyManager_GameMapModifiedMacro(std::uint32_t type, const void* packedMagPoint, std::uint32_t parameter);
}
