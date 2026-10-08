#include "C2UserNetworkCommandAccess.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2UserMessageAccess.h"

#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0040b13a; same player checks and matrix access as 0040A12B.
    std::int32_t Logic_EntryPoint_GetDiplomacyState(std::int32_t player, std::int32_t otherPlayer) { return GameControl_Diplomacy_GetState(gGameControl, player, otherPlayer); }

    // Original function: FUN_0040a231. The signed upper bound has no lower check.
    const char* GameControl_Names_GetPlayerTribeNamePtr(const void* gameControl, std::int32_t player)
    {
        if (player >= 20) return nullptr;
        const auto offset = static_cast<std::uint32_t>(player) * 0x4A80u;
        if (NC2Lifetime::Read<std::uint8_t>(gameControl, offset) == 0) return nullptr;
        const auto* name = static_cast<const char*>(gameControl) + offset + 0x49ACu;
        return std::strlen(name) != 0 ? name : nullptr;
    }
    // Original function: FUN_0040a034; CGameControl::SpecialItem_Add.
    bool GameControl_SpecialItem_Add(void* gameControl, std::int32_t player, std::uint32_t type, std::uint32_t item, const void* position)
    {
        using namespace NC2Lifetime;
        if (player >= 20) return false;
        const std::uint32_t base = static_cast<std::uint32_t>(player) * 0x4A80u;
        if (Read<std::uint8_t>(gameControl, base) == 0) return false;
        for (std::uint32_t index = 0; index < 100; ++index)
        {
            const std::uint32_t offset = base + 0x4624u + index * 8u;
            if (Read<std::uint32_t>(gameControl, offset) != 0) continue;
            Write(gameControl, offset, type);
            Write(gameControl, offset + 4, item);
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 0x3F);
            Write(message, 8, player);
            Write(message, 0x2FC, type);
            Write(message, 0x300, item);
            if (position != nullptr) Write(message, 0x28, Read<std::uint32_t>(position, 0));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
            return true;
        }
        return false;
    }

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
    // Original function: FUN_0040a171; CGameControl::Diplomacy_SetState.
    bool __fastcall GameControl_Diplomacy_SetState(void* gameControl, void* /*unusedEdx*/, std::uint32_t player, std::uint32_t otherPlayer, std::uint32_t state)
    {
        using namespace NC2Lifetime;
        // Signed upper bounds deliberately retain the original missing lower checks.
        if (static_cast<std::int32_t>(player) >= 20 || Read<std::uint8_t>(gameControl, player * 0x4A80u) == 0) return false;
        if (static_cast<std::int32_t>(otherPlayer) >= 20 || Read<std::uint8_t>(gameControl, otherPlayer * 0x4A80u) == 0) return false;
        const std::uint32_t offset = player * 0x4A80u + otherPlayer * 4u + 0x45A8u;
        if (Read<std::uint32_t>(gameControl, offset) == state) return true;
        Write(gameControl, offset, state);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1A, true, player, otherPlayer, state);
        if (Read<std::uint8_t>(gameControl, player * 0x4A80u + otherPlayer + 0x496Cu) == 0)
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 0x3D);
            Write(message, 8, otherPlayer);
            Write(message, 0x0C, player);
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        return true;
    }


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
