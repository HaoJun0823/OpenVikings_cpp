#pragma once
#include <cstdint>
#include "C2MapPathfinderSearch.h"
namespace NC2Logic
{
    std::int32_t AIHighLevelHandler_Tool_GetUnusedHuman(const void* handler, const void* point);
    void AIVillageManager_Init(void* village, std::uint32_t type, std::int32_t good, std::uint32_t id, std::uint32_t player, const void* point, std::uint32_t radius);
    void AIVillageManager_DoOneLogicTick(void* village, std::uint32_t tick);
    void AIVillageManager_CheckAndSetGoodData(void* village, const void* resources);
    bool AIVillageManager_Good_GetWorkCenterPoint(const void* village, std::uint32_t good, void* point);
    std::int32_t AIVillageManager_Good_GetWorkHouseId(void* village, std::uint32_t good);
    std::int32_t AIVillageManager_Good_AskForNewHouse(void* village, std::uint32_t good, const void* resources);
    std::int32_t AIVillageManager_Good_GetWorkHumanId(const void* village, const void* point);
    void AIHouseManager_Init(void* manager);
    void AIHouseManager_ClearAllNoHouseCanBeBuildFlags(void* manager);
    bool AIHouseManager_Military_IsBarrackBuild(const void* manager);
    void AIVillagesData_Init(void* villages, std::uint32_t player, const void* point);
    void AIVillagesData_DoOneLogicTick(void* villages, const void* resources, void* goods, std::uint32_t tick);
    void AIVillagesData_l_Village_Add(void* villages, std::uint32_t type, std::int32_t good, std::uint32_t player, const void* point, std::uint32_t radius);
    void AIVillagesData_l_Village_Clear(void* villages, std::int32_t id);
    std::int32_t AIVillagesData_House_BuildHouse(void* villages, std::uint32_t house, bool disableOnFailure, MapSearchCallback callback, std::uint32_t userData);
    bool AIVillagesData_Military_IsBarrackBuild(const void* villages);
    std::int32_t AIVillagesData_Military_GetBarrackHouseId(const void* villages);
    bool AIVillagesData_l_GetVillageFreePoint(const void* villages, void* point, std::uint32_t radius);
    std::int32_t AIVillagesData_Villages_CreateNew(void* villages, std::uint32_t type, std::uint32_t good, void* resources, void* invalidAreas);
}
