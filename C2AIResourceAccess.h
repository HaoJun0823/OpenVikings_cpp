#pragma once
#include <cstdint>

namespace NC2Logic
{
    void AIResourceManager_Init(void* manager, std::uint32_t player, const void* villages);
    void AIResourceManager_UpdateResourcePointArray(void* manager, std::uint32_t player, const void* villages);
    bool AIResourceManager_l_CheckOldResourceArea(void* manager, std::int32_t index);
    void AIResourceManager_l_UpdateResourcePoint(void* manager, std::uint32_t player, std::int32_t index, const void* villages);
    void* AIResourceSearchTool_Construct(void* search, const void* villages, const void* entries, std::int32_t index);
    bool AIResourceSearchTool_IsCandidate(const void* search, const void* point);
    void AIResourceSearchTool_CheckAndSetData(void* search, const void* point);
    bool AIResourceManager_GetResourcePosition(const void* manager, std::uint32_t good, void* point);
    void AIResourceManager_UpdateGoodResource(void* manager, std::uint32_t good, const void* invalidAreas, const void* villages);
    std::uint8_t AIResourceManager_WasResourceDataUpdated(const void* manager, std::uint32_t good);
    std::uint8_t AIResourceManager_IsResourceOnMap(const void* manager, std::uint32_t good);
    void AIResourceManager_ClearResourceUpdateFlags(void* manager);
    bool AIResourceManager_IsExportingHouseType(const void* manager, std::uint32_t house, std::uint32_t exportedHouse);
    bool AIInvalidAreaManager_AddArea(void* manager, const void* area);
    bool AIInvalidAreaManager_IsInsideAnyArea(const void* manager, const void* point);
    std::int32_t AIVillagesData_GetVillageId(const void* villages, const void* point);
    void* AIVillagesData_GetVillageDataById(void* villages, std::int32_t id);
    const void* AIVillagesData_GetConstVillageDataById(const void* villages, std::int32_t id);
}
