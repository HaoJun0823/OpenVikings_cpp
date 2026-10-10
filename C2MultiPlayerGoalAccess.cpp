#include "C2MultiPlayerGoalAccess.h"
#include "C2LifetimeAccess.h"

namespace NC2Logic
{
    // Original function: FUN_00419304; the player index is unchecked in Windows.
    void MultiPlayerGoalManager_ExternalTrigger_WonByMissionManager(void* manager, std::uint32_t player)
    {
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* goal = static_cast<std::byte*>(manager) + 0x1A0 + index * 0x48u;
            if (NC2Lifetime::Read<std::uint32_t>(goal, 0) == 4) NC2Lifetime::Write<std::uint8_t>(goal, 4u + player, 1);
        }
    }

    // Original function: FUN_00419324; losing conditions are distinct original records.
    void MultiPlayerGoalManager_ExternalTrigger_LostByMissionManager(void* manager, std::uint32_t player)
    {
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* goal = static_cast<std::byte*>(manager) + 0x1A0 + index * 0x48u;
            if (NC2Lifetime::Read<std::uint32_t>(goal, 0) == 5) NC2Lifetime::Write<std::uint8_t>(goal, 4u + player, 1);
        }
    }
}
