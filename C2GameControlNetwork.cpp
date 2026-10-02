#include "C2UserNetworkCommandAccess.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"

namespace NC2Logic
{
    // Original function: FUN_0040a0d7; matching entries are consumed immediately.
    bool GameControl_SpecialItem_Use(void* gameControl, std::int32_t player, std::uint32_t type, std::uint32_t item)
    {
        using namespace NC2Lifetime;
        if (player >= 20) return false;
        const auto base = static_cast<std::uint32_t>(player) * 0x4A80u;
        if (Read<std::uint8_t>(gameControl, base) == 0) return false;
        for (std::uint32_t index = 0; index < 100; ++index)
        {
            const auto offset = base + 0x4624u + index * 8u;
            if (Read<std::uint32_t>(gameControl, offset) != type || Read<std::uint32_t>(gameControl, offset + 4) != item) continue;
            Write<std::uint32_t>(gameControl, offset, 0);
            Write<std::uint32_t>(gameControl, offset + 4, 0);
            return true;
        }
        return false;
    }
    // Original function: FUN_0040a4da; zero network IDs never match.
    std::int32_t GameControl_GetPlayerByNetworkId(const void* gameControl, std::uint32_t networkId)
    {
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            const auto id = NC2Lifetime::Read<std::uint32_t>(gameControl, player * 0x4A80u + 0x14u);
            if (id != 0 && id == networkId) return static_cast<std::int32_t>(player);
        }
        return -1;
    }

    // Original function: FUN_0040b250.
    std::int32_t GameTime_Network_GetPlayerByNetworkId(std::uint32_t networkId)
    {
        return gGameStaticVars.DAT_005109c4 != 0 ? GameControl_GetPlayerByNetworkId(gGameControl, networkId) : -1;
    }
    // Original function: FUN_0040a55f; the signed upper bound has no lower-bound check.
    bool GameControl_Player_IsNetworkPlayer(const void* gameControl, std::int32_t player)
    {
        if (gGameStaticVars.DAT_005109c4 == 0 || player >= 20) return false;
        const auto offset = static_cast<std::uint32_t>(player) * 0x4A80u;
        return NC2Lifetime::Read<std::uint8_t>(gameControl, offset) != 0 && NC2Lifetime::Read<std::uint32_t>(gameControl, offset + 4) == 1;
    }
    // Original function: FUN_0040a12b; CGameControl::Diplomacy_GetState.
    std::int32_t GameControl_Diplomacy_GetState(const void* gameControl, std::int32_t player, std::int32_t otherPlayer)
    {
        if (player >= 20 || NC2Lifetime::Read<std::uint8_t>(gameControl, static_cast<std::uint32_t>(player) * 0x4A80u) == 0) return 0;
        if (otherPlayer >= 20 || NC2Lifetime::Read<std::uint8_t>(gameControl, static_cast<std::uint32_t>(otherPlayer) * 0x4A80u) == 0) return 0;
        return NC2Lifetime::Read<std::int32_t>(gameControl, static_cast<std::uint32_t>(player) * 0x4A80u + static_cast<std::uint32_t>(otherPlayer) * 4u + 0x45A8u);
    }
    // Original command processing remains a separate reconstruction target.


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
            UserNetworkCommandManager_AddCommand(gUserNetworkCommandManager, nullptr, command, true, false);
            return;
        }
    }
}
