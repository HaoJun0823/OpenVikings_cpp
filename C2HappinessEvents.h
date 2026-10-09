#pragma once

#include <cstdint>
#include "C2CallbackManager.h"

namespace NC2Logic
{
    void Happiness_AddEventType(void* happiness, std::uint32_t type);
    std::int32_t Happiness_GetEventValue(const void* happiness);
    void Happiness_UpdateEvents(void* happiness);
    std::int32_t Happiness_Events_GetValue(const void* event);
    void HappinessManager_RecalculateAllPlayer(void* manager);
    void HappinessManager_l_RecalculatePlayer(void* manager, std::uint32_t player);
    std::int32_t HappinessManager_l_GetGoodHappinessValue(const void* manager, std::uint32_t player);
    std::int32_t HappinessManager_l_GetBuildingHappinessValue(const void* manager, std::uint32_t player);
    std::int32_t HappinessManager_l_GetHumanHappinessValue(const void* manager, std::uint32_t player);
    std::uint32_t HappinessManager_l_GetMilitaryHappinessValue(const void* manager, std::uint32_t player);
    bool __cdecl HappinessManager_l_Happiness_Callback(TCallbackTypes type, std::uint32_t userData, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    void HappinessManager_Messages_PlayerAttackedByPlayer(void* manager, std::uint32_t player, std::uint32_t attacker);
    void HappinessManager_Messages_CivilistDied(void* manager, const void* human);
    void HappinessManager_Messages_HouseAttacked(void* manager, const void* house);
    void HappinessManager_Messages_HouseDestroyedTroughAttack(void* manager, const void* house);
}
