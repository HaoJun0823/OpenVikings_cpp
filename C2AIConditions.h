#pragma once
#include <cstdint>

namespace NC2Logic
{
    bool AIConditionToConditionCheck_Tool_CheckNumberOfConditions(const void* check);
    void AICondition_Init(void* condition, std::uint32_t type, std::uint8_t flag);
    bool AICondition_SetCondition(void* condition, std::uint8_t active);
    std::uint8_t AISinglePlayerHandler_Condition_IsActive(const void* handler, std::uint32_t index);
    bool AISinglePlayerHandler_Condition_RecheckAll(void* handler, std::uint32_t tick);
    bool AISinglePlayerHandler_l_Condition_CheckForConditionChange(void* handler, void* condition);
    bool AISinglePlayerHandler_Condition_Set(void* handler, std::uint32_t index, const void* condition);
    bool AIManager_EntryPoint_Condition_Set(void* manager, std::uint32_t player, std::uint32_t index, const void* condition);
    bool AIManager_EntryPoint_SetUnitLimit(void* manager, std::uint32_t player, std::uint32_t limit);
    bool AIManager_EntryPoint_SetMaxUnitLimit(void* manager, std::uint32_t player, std::uint32_t limit);
    bool AIManager_EntryPoint_Condition_ExternalActivate_SetFlag(void* manager, std::uint32_t player, std::uint32_t index, bool active);
    bool AISinglePlayerHandler_EntryPoint_Condition_ExternalActivate_SetFlag(void* handler, std::uint32_t index, bool active);
}
