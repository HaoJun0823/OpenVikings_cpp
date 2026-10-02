#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"

namespace NC2Logic
{
    // Original command processing remains a separate reconstruction target.
    extern void __fastcall FUN_00410f00(void*, void* unusedEdx, const void*, bool, bool);

    // Original function: FUN_0040a4fd; Windows network-ID command adapter.
    // The Mac Player_LeavesGame name does not identify this Windows-only adapter.
    void GameControl_QueueNetworkPlayerDeparture(void* gameControl, std::uint32_t networkPlayerId, bool originalFlag)
    {
        (void)originalFlag; // The original stack argument is never read.
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            const auto id = NC2Lifetime::Read<std::uint32_t>(gameControl, 0x14u + player * 0x4A80u);
            if (id == 0 || id != networkPlayerId) continue;
            std::byte command[20]{};
            NC2Lifetime::Write<std::uint32_t>(command, 8, 0);
            NC2Lifetime::Write<std::uint32_t>(command, 12, 1);
            command[0] = std::byte{0x88};
            NC2Lifetime::Write(command, 4, player);
            FUN_00410f00(gUserNetworkCommandManager, nullptr, command, true, false);
            return;
        }
    }
}
