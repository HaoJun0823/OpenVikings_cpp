#pragma once
#include <cstdint>

namespace NC2Logic
{
    bool AIManager_EntryPoint_MainTask_Add(void* manager, std::uint32_t player, const void* task);
    bool AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(void* manager, std::uint32_t player, const void* point, std::uint32_t radius);
    std::uint32_t AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(void* handler);
    std::uint32_t AISinglePlayerHandler_MainTask_Add(void* handler, const void* task);
    bool AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(void* handler, const void* point, std::uint32_t radius);
    bool AITaskSimpleDescription_IsEqualTo(const void* description, const void* other);
    bool AITaskSimpleDescription_Tool_GetMapPosition(const void* description, void* point);
    void AIMainTask_Init(void* task, std::uint32_t type, std::int32_t priority, std::uint32_t condition);
    void AIActiveCombinedMainTasks_Init(void* task, std::uint32_t mainTask, std::int32_t priority, const void* description);
    bool AIActiveCombinedMainTasks_AddMainTask(void* task, std::uint32_t mainTask, std::int32_t priority, const void* description);
    void AIConditionToConditionCheck_Tool_Init(void* check, std::uint32_t operation);
    void* AIActiveCombinedMainTasks_CopyConstruct(void* task, const void* other);
    bool AISinglePlayerHandler_l_BuildHouses_AddBuildHouse(void* handler, std::uint32_t type, const void* point, std::uint32_t stage, std::uint32_t requested);
    void AISinglePlayerHandler_l_CombinedActiveMainTask_UpdateList(void* handler);
    void AISinglePlayerHandler_MainTask_RecheckAll(void* handler);
    void AISinglePlayerHandler_l_BuildHouses_SaveAllHousesIntoArray(void* handler);
}
