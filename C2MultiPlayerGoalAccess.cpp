#include "SwitchIdentifiers.h"
#include "C2MultiPlayerGoalAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2ChecksumTools.h"
#include "C2PlayerStatisticAccess.h"
#include "C2UserNetworkCommandAccess.h"
#include "XBTokenList.h"
#include "XIniFileReader.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace
{
    // Complete Windows token region 004F3820..004F3847.
    const NXBasics::SToken gMultiPlayerGoalTokens[] = { { "goalwonwhengoods", 0x6777676F }, { "goalwonwheninhabitants", 0x6777696E }, { "goalwonbymission", 0x6777776D }, { "goallostbymission", 0x67776C6D }, { nullptr, 0 } };
    const NXBasics::SToken* gMultiPlayerGoalTokenList = nullptr; // Original 005114F8.
    std::uint8_t gMultiPlayerGoalTokenInitFlags = 0; // Original 005114FC.
    static_assert(sizeof(gMultiPlayerGoalTokens) == 0x28);
    // Original function: FUN_00419303; the original exit callback is a bare RET.
    int __cdecl DestroyMultiPlayerGoalTokenList() { return 0; }
}

namespace NC2Logic
{
    // Original function: FUN_004190FD.
    std::uint32_t MultiPlayerGoalManager_Checksum_Calculate(const void* manager) { return Tool_Checksum_Calculate(manager, 0x740); }

    // Original function: FUN_0041910B; existing goal payloads are not cleared here.
    void __fastcall MultiPlayerGoalManager_IO_LoadGoals(void* manager, void* /*unusedEdx*/, NXBasics::CIniFileReader* reader)
    {
        using namespace NC2Lifetime;
        Write<std::uint8_t>(manager, 0, gGameStaticVars.mapType == 4);
        if (Read<std::uint8_t>(manager, 0) == 0) return;
        Write<std::uint32_t>(manager, 0x19C, 1);
        Write<std::uint32_t>(manager, 0x1A0, 1);
        if (!reader->SearchGroup("misc_multiplayer_goals")) return;
        while (reader->GetNextCommandLineInGroup() != nullptr)
        {
            if ((gMultiPlayerGoalTokenInitFlags & 1u) == 0)
            {
                gMultiPlayerGoalTokenInitFlags |= 1u;
                gMultiPlayerGoalTokenList = gMultiPlayerGoalTokens;
                NXBasics::Runtime_AtExit(DestroyMultiPlayerGoalTokenList);
            }
            NXBasics::CTokenList tokens(gMultiPlayerGoalTokenList);
            const auto command = tokens.FindToken(reader->GetCurrentName());
            if (command != 0x6777676F && command != 0x6777696E && command != 0x6777776D && command != 0x67776C6D) continue;
            const auto count = Read<std::uint32_t>(manager, 0x19C);
            if (count == 20) continue;
            auto* goal = static_cast<std::byte*>(manager) + 0x1A0 + count * 0x48u;
            if (command == 0x6777676F)
            {
                Write<std::uint32_t>(goal, 0, 2);
                while (reader->GetTypeOfNextParameter() == 3 && Read<std::uint32_t>(goal, 4) != 8)
                {
                    const auto good = reader->GetInt();
                    const auto amount = reader->GetInt();
                    const auto pair = Read<std::uint32_t>(goal, 4);
                    Write(goal, 8 + pair * 8u, good);
                    Write(goal, 0x0C + pair * 8u, amount);
                    Write(goal, 4, pair + 1u);
                }
            }
            else if (command == 0x6777696E)
            {
                const auto amount = reader->GetInt();
                const auto mode = reader->GetInt();
                Write<std::uint32_t>(goal, 0, 3);
                Write(goal, 4, amount);
                Write<std::uint8_t>(goal, 8, mode != 0);
            }
            else Write<std::uint32_t>(goal, 0, command == 0x6777776D ? 4 : 5);
            Write(manager, 0x19C, count + 1u);
        }
    }

    // Original function: FUN_0041950B; no player bounds check exists in Windows.
    bool MultiPlayerGoalManager_l_IsWonOrLost(const void* manager, std::uint32_t player)
    {
        return NC2Lifetime::Read<std::uint8_t>(manager, 4 + player * 20u) != 0 || NC2Lifetime::Read<std::uint8_t>(manager, 0x0C + player * 20u) != 0;
    }

    // Original function: FUN_00419344; all twenty goal slots are scanned.
    void MultiPlayerGoalManager_l_Goals_Check(void* manager)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            if (MultiPlayerGoalManager_l_IsWonOrLost(manager, player)) continue;
            if (Read<std::uint8_t>(gGameControl, player * 0x4A80u) == 0 || MultiPlayerGoalManager_l_IsWonOrLost(manager, player)) continue;
            for (std::uint32_t index = 0; index < 20; ++index)
            {
                const auto* goal = static_cast<const std::byte*>(manager) + 0x1A0 + index * 0x48u;
                bool won = false;
                bool lost = false;
                switch (Read<std::uint32_t>(goal, 0))
                {
                case SwitchIds::MultiplayerGoal::Survive: lost = Read<std::uint8_t>(gGameControl, player * 0x4A80u + 8) != 0; break;
                case SwitchIds::MultiplayerGoal::CollectGoods:
                {
                    std::byte goods[0x88]{};
                    Tool_PlayerStatistics_CountGoods(player, goods);
                    std::int32_t pair = 0;
                    for (; pair < Read<std::int32_t>(goal, 4); ++pair)
                    {
                        const auto good = Read<std::uint32_t>(goal, 8 + static_cast<std::uint32_t>(pair) * 8u);
                        if (Read<std::uint16_t>(goods, 4 + good * 2u) < Read<std::int32_t>(goal, 0x0C + static_cast<std::uint32_t>(pair) * 8u)) break;
                    }
                    won = pair == Read<std::int32_t>(goal, 4);
                    break;
                }
                case SwitchIds::MultiplayerGoal::Population:
                {
                    std::uint16_t counts[10]{};
                    Tool_PlayerStatistics_CountHumans(player, counts);
                    won = static_cast<std::uint32_t>(counts[Read<std::uint8_t>(goal, 8) != 0 ? 9 : 0]) >= Read<std::uint32_t>(goal, 4);
                    break;
                }
                case SwitchIds::MultiplayerGoal::PlayersWin: won = Read<std::uint8_t>(goal, 4 + player) != 0; break;
                case SwitchIds::MultiplayerGoal::PlayersLose: lost = Read<std::uint8_t>(goal, 4 + player) != 0; break;
                }
                if (won)
                {
                    Write<std::uint8_t>(manager, 4 + player * 20u, 1);
                    const auto order = Read<std::uint32_t>(manager, 0x194);
                    Write(manager, 8 + player * 20u, order);
                    Write(manager, 0x194, order + 1u);
                    CCallbackManager::sTheObjectPtr->ExecuteCallback(0x30, true, player, 1, 0);
                    break;
                }
                if (lost)
                {
                    Write<std::uint8_t>(manager, 0x0C + player * 20u, 1);
                    const auto order = Read<std::uint32_t>(manager, 0x198);
                    Write(manager, 0x10 + player * 20u, order);
                    Write(manager, 0x198, order + 1u);
                    CCallbackManager::sTheObjectPtr->ExecuteCallback(0x30, true, player, 0, 1);
                    Write<std::uint8_t>(manager, 0x14 + player * 20u, 1);
                    break;
                }
            }
        }
    }

    // Original function: FUN_0041952C; clear pending flags before checking activity.
    void MultiPlayerGoalManager_l_KickFlags_Execute(void* manager)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            if (Read<std::uint8_t>(manager, 0x14 + player * 20u) == 0) continue;
            Write<std::uint8_t>(manager, 0x14 + player * 20u, 0);
            if (Read<std::uint8_t>(gGameControl, player * 0x4A80u) == 0) continue;
            std::byte command[20]{};
            Write<std::uint32_t>(command, 0, 0x89);
            Write(command, 4, player);
            Write<std::uint32_t>(command, 0x0C, 1);
            UserNetworkCommandManager_AddCommand(gUserNetworkCommandManager, nullptr, command, true, false);
        }
    }

    // Original function: FUN_004195A4; queued departures precede this pass's goals.
    bool __cdecl MultiPlayerGoalManager_l_LogicCallback(unsigned int type, unsigned int /*userData*/, unsigned int tick, unsigned int /*param2*/, unsigned int /*param3*/)
    {
        if (type == 1 && tick % 120u == 0)
        {
            MultiPlayerGoalManager_l_KickFlags_Execute(gMultiPlayerGoalManager);
            MultiPlayerGoalManager_l_Goals_Check(gMultiPlayerGoalManager);
        }
        return true;
    }

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
