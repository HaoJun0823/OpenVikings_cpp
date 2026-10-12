#pragma once
#include <cstdint>

namespace NC2E2
{
    using MacroMapPositionCallback = bool (__cdecl*)(const void* point, std::uint32_t triangle);
    void* MacroMap_Construct(void* map, const char* filename);
    void MacroMap_Destruct(void* map);
    void MacroMap_l_FreeAllData(void* map);
    bool MacroMap_LoadFromIniFile(void* map, const char* filename);
    void MacroMap_CreateFromGameMap(void* map, const char* filename, bool includeCreatures, std::int16_t minX, std::int16_t minY, std::int16_t maxX, std::int16_t maxY, bool mirrorX, bool mirrorY);
    void MapIO_NewMap(void* map, bool enableTime, std::int16_t width, std::int16_t height, bool initializeAllPlayers);
    void MapIO_RebuildCurrentMapViaMacroMap(std::int16_t width, std::int16_t height);
    void EngineMap_PostLoad_RebuildIfRequired();
    void MacroMap_SetPatternToGameMap(void* map, const void* root, MacroMapPositionCallback callback);
    void MacroMap_SetTransitionsToGameMap(void* map, const void* root, MacroMapPositionCallback callback);
    void MacroMap_SetLandscapesToGameMap(void* map, const void* root, MacroMapPositionCallback callback, bool preserveWalkingBlock);
    void MacroMap_SetHeightsToGameMap(void* map, const void* root, std::int32_t mode, MacroMapPositionCallback callback);
    void MacroMap_SetVertexColorsToGameMap(void* map, const void* root, MacroMapPositionCallback callback);
    void MacroMap_SetRoadsToGameMap(void* map, const void* root, MacroMapPositionCallback callback);
    void MacroMap_SetHumansToGameMap(void* map, const void* root);
    void MacroMap_SetAnimalsToGameMap(void* map, const void* root);
    void MacroMap_SetHousesToGameMap(void* map, const void* root);
    void MacroMap_SetFishSwarmsToGameMap(void* map, const void* root);
    std::int32_t Engine2d_EntryPoint_Edit_SetHouse(std::uint32_t player, std::int32_t engineType, std::uint32_t stage, const void* point, bool completed);
    void* __fastcall MacroMapManager_Construct(void* manager, void* unusedEdx);
    void MacroMapManager_RescanMacroMaps(void* manager);
    void MacroMapManager_l_FreeAllMacroMaps(void* manager);
}
