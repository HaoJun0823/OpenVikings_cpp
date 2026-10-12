#pragma once
#include <cstdint>
namespace NC2Logic
{
    void AIGoodManager_Init(void* manager, std::uint32_t tribe);
    void AIGoodManager_DoOneLogicGameTick(void* manager, void* village, const void* resources);
    std::int32_t AIGoodManager_l_House_SearchHouse(void* manager, std::uint32_t good, std::uint32_t producer, void* village);
    bool AIGoodManager_l_GetWorkerForHouse(void* manager, std::uint32_t good, std::uint32_t producer, const void* village);
    bool AIGoodManager_l_House_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t producer, const void* resources);
    bool AIGoodManager_l_Collect_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t producer, const void* village, const void* resources);
}
