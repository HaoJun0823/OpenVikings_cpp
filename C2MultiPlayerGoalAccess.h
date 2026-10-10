#pragma once
#include <cstdint>

namespace NC2Logic
{
    void MultiPlayerGoalManager_ExternalTrigger_WonByMissionManager(void* manager, std::uint32_t player);
    void MultiPlayerGoalManager_ExternalTrigger_LostByMissionManager(void* manager, std::uint32_t player);
}
