#include "C2UserNetworkCommandAccess.h"
#include "C2PlayerStatisticAccess.h"
#include "C2GameTime.h"
#include "C2CallbackManager.h"
#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"

#include <cstring>
#include <Windows.h>
#include <mmsystem.h>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original subsystem updates/command processing are separate unresolved bodies.
    extern std::uint32_t Game_Checksum_Calculate();

    extern void __fastcall MapMoveableArray_DoOneGameTick(void*, void* unusedEdx);
    extern void __fastcall HouseArray_DoOneGameTick(void*, void* unusedEdx);
    extern void __fastcall HouseArray_DoOneGameSecond(void*, void* unusedEdx);
    extern void __fastcall RoadBuildManager_DoOneGameSecond(void*, void* unusedEdx);
    extern void __fastcall WallBuildManager_DoOneGameSecond(void*, void* unusedEdx);
    extern void Map_BioSystem_DoOneGameTick();
    extern void __fastcall MapLandscapeAutoTransitionManager_System_DoOneGameTick(void*, void* unusedEdx);
    extern void __fastcall MapBioRegrowManager_System_DoOneGameTick(void*, void* unusedEdx);
    extern void __fastcall DelayedActionManager_System_DoOneGameTick(void*, void* unusedEdx);
    extern void __fastcall GameControl_System_DoOneGameTick(void*, void* unusedEdx, std::uint32_t);
    extern void Game_l_Autosave_Update();
    extern std::uint32_t GameTime_Network_GetHighestPlayerLatency();

    static bool GameTime_IsATickToGo();

    // Original function: FUN_0040b1eb; maximum of the complete ten-sample region.
    static std::uint32_t GameTime_Network_GetHighestStoredLatency()
    {
        std::uint32_t maximum = 0;
        for (std::uint32_t sample : gGameTime.latencySamples) if (sample > maximum) maximum = sample;
        return maximum;
    }

    // Original function: FUN_0040b206; unsigned addition and threshold comparisons.
    static std::uint32_t GameTime_Network_LatencyToFrequency(std::uint32_t latency)
    {
        if (latency == 0) return 12;
        const std::uint32_t adjusted = latency + 150u;
        if (adjusted < 166u) return 6;
        if (adjusted < 250u) return 4;
        if (adjusted < 333u) return 3;
        return adjusted < 500u ? 2u : 1u;
    }

    // Original function: FUN_004077b8.
    void GameTime_Update()
    {
        if (gGameTime.maximizeSpeed != 0)
        {
            const std::uint32_t endTime = timeGetTime() + 500u;
            while (timeGetTime() < endTime) GameTime_DoOneGameTick();
            gGameTime.lastRealTime = static_cast<double>(timeGetTime());
            return;
        }
        for (std::int32_t count = 0; count < 50; ++count)
        {
            if (!GameTime_IsATickToGo()) break;
            GameTime_DoOneGameTick();
        }
    }

    // Original function: FUN_00407812.
    void GameTime_DoOneGameTick()
    {
        if (gGameInitialized == 0) return;
        if (gGameStaticVars.DAT_005109c4 != 0)
        {
            const std::uint32_t speed = static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(gGameTime.speed));
            if (gGameTime.tick % (speed * 30u) == 0)
            {
                std::byte command[20]{};
                command[0] = std::byte{0x8A};
                Write<std::uint32_t>(command, 4, Game_Checksum_Calculate());
                Write<std::uint32_t>(command, 8, gGameTime.tick);
                Write<std::uint32_t>(command, 12, NNetwork::GetDynamicNetworkLocalPlayerId(&gDynamicNetworkData));
                UserNetworkCommandManager_AddCommand(gUserNetworkCommandManager, nullptr, command, true, false);
            }
        }
        ++gGameTime.tick;
        gGameTime.lastRealTime += gGameTime.tickDuration;
        if (gGameTime.tick % 720u == 0) PlayerStatisticManager_UpdateStatistics(gPlayerStatisticManager, nullptr, gGameTime.tick / 12u);
        MapMoveableArray_DoOneGameTick(gMapMoveableArrayStorage.manager, nullptr);
        HouseArray_DoOneGameTick(CHouseArray::sTheObjectPtr, nullptr);
        if (gGameTime.tick % 12u == 0)
        {
            HouseArray_DoOneGameSecond(CHouseArray::sTheObjectPtr, nullptr);
            RoadBuildManager_DoOneGameSecond(gRoadBuildManager, nullptr);
            WallBuildManager_DoOneGameSecond(gWallBuildManager, nullptr);
        }
        Map_BioSystem_DoOneGameTick();
        MapLandscapeAutoTransitionManager_System_DoOneGameTick(gMapLandscapeAutoTransitionManager, nullptr);
        MapBioRegrowManager_System_DoOneGameTick(gMapBioRegrowManager, nullptr);
        DelayedActionManager_System_DoOneGameTick(gDelayedActionManager, nullptr);
        GameControl_System_DoOneGameTick(gGameControl, nullptr, gGameTime.tick);
        Game_l_Autosave_Update();
        CCallbackManager::sTheObjectPtr->ExecuteCallback(1, true, gGameTime.tick, 0, 0);
    }

    // Original function: FUN_004079cf; separate initialization entry.
    void GameTime_ResetNetworkSynchronization()
    {
        if (gGameStaticVars.DAT_005109c4 != 0)
        {
            NNetwork::AdvanceDynamicNetworkTick(&gDynamicNetworkData);
            UserNetworkCommandManager_SendCommands(gUserNetworkCommandManager, nullptr);
            GameTime_SetNetworkWaitFlag(false);
        }
        gGameTime.networkBaseTick = 0;
        gGameTime.networkBaseRealTime = 0;
        gGameTime.networkFrequency = 3;
        gGameTime.latencySampleTime = timeGetTime();
        gGameTime.autoAdjustNetworkFrequency = 1;
        for (auto& sample : gGameTime.latencySamples) sample = 400;
        gGameTime.latencySampleIndex = 0;
    }

    // Original function: FUN_00407a36; Mac l_GameTime_IsATickToGo.
    static bool GameTime_IsATickToGo()
    {
        const std::uint32_t now = timeGetTime();
        if (gGameStaticVars.DAT_005109c4 != 0)
        {
            if (NNetwork::Network_IsHost())
            {
                if (now > gGameTime.latencySampleTime + 3000u)
                {
                    const std::uint32_t latency = GameTime_Network_GetHighestPlayerLatency();
                    if (latency == 0)
                    {
                        std::memset(gGameTime.latencySamples, 0, sizeof(gGameTime.latencySamples));
                        gGameTime.latencySampleIndex = 0;
                    }
                    else
                    {
                        gGameTime.latencySamples[gGameTime.latencySampleIndex] = latency;
                        if (++gGameTime.latencySampleIndex == 10) gGameTime.latencySampleIndex = 0;
                    }
                    gGameTime.latencySampleTime = now;
                }
                if (gGameTime.autoAdjustNetworkFrequency != 0)
                {
                    const std::uint32_t frequency = GameTime_Network_LatencyToFrequency(GameTime_Network_GetHighestStoredLatency());
                    if (frequency != gGameTime.networkFrequency)
                    {
                        std::byte command[20]{};
                        command[0] = std::byte{0x8B};
                        Write<std::uint32_t>(command, 4, frequency);
                        UserNetworkCommandManager_AddCommand(gUserNetworkCommandManager, nullptr, command, true, false);
                    }
                }
            }
            // Preserve unordered comparisons and unchecked Windows divisors.
            const double speed = gGameTime.speed;
            if (!(speed >= 0.01) && now - gGameTime.networkBaseRealTime < 1000u)
            {
                gGameTime.lastRealTime = static_cast<double>(now);
                return false;
            }
            const std::uint32_t integerSpeed = static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(speed));
            const std::uint32_t targetTick = gGameTime.networkBaseTick + integerSpeed / gGameTime.networkFrequency;
            if (!(gGameTime.tick < targetTick && speed >= 0.01))
            {
                if (!UserNetworkCommandManager_ReceiveCommands(gUserNetworkCommandManager, nullptr))
                {
                    gGameTime.lastRealTime = static_cast<double>(now);
                    return false;
                }
                gGameTime.networkBaseTick = gGameTime.tick;
                gGameTime.networkBaseRealTime = now;
                NNetwork::AdvanceDynamicNetworkTick(&gDynamicNetworkData);
                UserNetworkCommandManager_SendCommands(gUserNetworkCommandManager, nullptr);
                GameTime_SetNetworkWaitFlag(false);
            }
            else
                return static_cast<double>(now) - gGameTime.lastRealTime >= gGameTime.tickDuration;
        }
        if (!(gGameTime.speed >= 0.01))
        {
            gGameTime.lastRealTime = static_cast<double>(now);
            return false;
        }
        return static_cast<double>(now) - gGameTime.lastRealTime >= gGameTime.tickDuration;
    }

    // Original function: FUN_0040aeb2; distinct tail-forwarding entry.
    void Logic_EntryPoint_GameTime_Update() { GameTime_Update(); }

    // Original function: FUN_0040b246.
    void GameTime_SetNetworkWaitFlag(bool waiting) { gGameStaticVars.networkWait = static_cast<std::uint8_t>(waiting); }
}
