#include "C2MissionExecution.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2GameControlNetwork.h"
#include "C2GameMapAccess.h"
#include "C2HumanIterator.h"
#include "C2VehicleIterator.h"
#include "C2VehicleAccess.h"
#include "C2MapGeometry.h"
#include "C2TributeAccess.h"
#include "C2MissionStartObjects.h"
#include "C2LogicObjectArrays.h"
#include "C2CampaignStaticDataManager.h"
#include "C2MultiPlayerGoalAccess.h"
#include "C2SavegameIO.h"
#include "C2StringAccess.h"
#include "C2UserMessageAccess.h"
#include "DSManager.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        void* GetMission(void* manager, std::uint32_t index) { return static_cast<std::byte*>(manager) + 0x1C + index * 0xDD8u; }
        bool GoalsMatch(std::int32_t mode, std::int32_t count, std::int32_t fulfilled)
        {
            switch (mode)
            {
            case 0: return fulfilled == count;
            case 1: return fulfilled >= 1;
            case 2: return fulfilled >= (count >> 1) + count % 2;
            case 3: return fulfilled == 0;
            default: return false;
            }
        }
    }

    // Original function: FUN_004230ee; only tick callbacks at multiples of 36 evaluate goals.
    bool __cdecl MissionManager_l_Mission_Callback(TCallbackTypes type, std::uint32_t /*userData*/, std::uint32_t tick, std::uint32_t /*param2*/, std::uint32_t /*param3*/)
    {
        if (type == 1 && tick % 36u == 0) return MissionManager_CheckAllMissionGoals(gMissionManager);
        return true;
    }

    // Original function: FUN_0041e2ae; parent restoration takes precedence over submap start.
    bool MissionManager_CheckAllMissionGoals(void* manager)
    {
        Write<std::uint8_t>(manager, 8, 0);
        Write<std::uint8_t>(manager, 6, 0);
        for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0x18); ++index)
        {
            void* mission = GetMission(manager, static_cast<std::uint32_t>(index));
            if (Read<std::uint8_t>(mission, 0x26) == 0) continue;
            MissionManager_l_CheckMission(manager, mission, true);
            if (Read<std::uint8_t>(manager, 6) != 0) break;
        }
        if (Read<std::uint8_t>(manager, 0x14) != 0)
        {
            SubMap_BackToParent();
            return false;
        }
        if (Read<std::uint8_t>(manager, 8) != 0)
        {
            const auto campaignId = Read<std::uint32_t>(manager, 0x0C), mapId = Read<std::uint32_t>(manager, 0x10), pathfinderLimitOverride = Read<std::uint32_t>(gGameControl, 0x5D200);
            SubMap_StartSubMap(campaignId, mapId, pathfinderLimitOverride);
            // The old mission manager can have been freed by either transition.
            return false;
        }
        return true;
    }

    // Original function: FUN_0041e31b; timed goals are handled before the general predicate.
    bool MissionManager_l_CheckMission(void* manager, void* mission, bool executeResults)
    {
        std::int32_t fulfilled = 0;
        for (std::int32_t index = 0; index < Read<std::int32_t>(mission, 0x38); ++index)
        {
            auto* goal = reinterpret_cast<SMissionGoal*>(static_cast<std::byte*>(mission) + 0x3C + static_cast<std::uint32_t>(index) * 0x28u);
            bool success;
            if (goal->type == 28 || goal->type == 45)
            {
                auto seconds = goal->parameters[0];
                if (goal->type == 45)
                {
                    if (goal->parameters[1] == 0)
                    {
                        const auto half = static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(seconds) / 2);
                        goal->parameters[1] = Game_Random_GetNumber() % half + half;
                    }
                    seconds = goal->parameters[1];
                }
                success = Read<std::uint32_t>(mission, 0x28) + seconds * 12u <= gGameTime.tick;
                if (success && goal->type == 45) goal->parameters[1] = 0;
            }
            else success = MissionManager_l_CheckGoal(manager, *goal);
            Write<std::uint8_t>(goal, 0x24, success);
            if (success) ++fulfilled;
        }
        const bool success = GoalsMatch(Read<std::int32_t>(mission, 0x34), Read<std::int32_t>(mission, 0x38), fulfilled);
        if (success && executeResults)
        {
            MissionData_SetMissionActiveFlag(mission, false);
            for (std::int32_t index = 0; index < Read<std::int32_t>(mission, 0x4EC); ++index)
            {
                const auto* result = reinterpret_cast<const SResult*>(static_cast<const std::byte*>(mission) + 0x4F0 + static_cast<std::uint32_t>(index) * 0x4Cu);
                MissionManager_l_ExecuteResult(manager, *result);
            }
        }
        Write<std::uint8_t>(mission, 0x25, success);
        return success;
    }

    // Partial original function: FUN_0041e45e; remaining object/area/combat predicates are tracked.
    bool MissionManager_l_CheckGoal(void* manager, const SMissionGoal& goal)
    {
        const auto* p = goal.parameters;
        switch (goal.type)
        {
        case 0: return true;
        case 11:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const void* point = Human_GetMapPositionRef(human);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]))) <= p[3]) return true;
            }
            return false;
        case 12:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                const void* point = Vehicle_GetMapPositionRef(vehicle);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]))) <= p[3]) return true;
            }
            return false;
        case 18: return MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]) == -1;
        case 19: return MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]) == -1;
        case 20: return MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]) == -1;
        case 21: return NXBasics::Runtime_TimeSigned32(p[0]) < 20 && Read<std::uint8_t>(gGameControl, p[0] * 0x4A80u + 8u) != 0;
        case 22:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            CHumanIterator humans;
            if (humans.GetFirstHumanId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[3])) != -1) return true;
            CVehicleIterator vehicles;
            return vehicles.GetFirstVehicleId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[3])) != -1;
        }
        case 24: return GameControl_Diplomacy_GetState(gGameControl, NXBasics::Runtime_TimeSigned32(goal.parameters[0]), NXBasics::Runtime_TimeSigned32(goal.parameters[1])) == NXBasics::Runtime_TimeSigned32(goal.parameters[2]);
        case 29: return MissionManager_l_CheckMission(manager, GetMission(manager, goal.parameters[0]), false);
        case 30:
        {
            const auto* tribute = static_cast<const STributeData*>(gTributeManager) + p[0];
            return tribute->allocated != 0 && tribute->paid != 0;
        }
        case 31:
        {
            CHumanIterator iterator;
            std::int32_t count = 0;
            for (auto id = iterator.GetFirstHumanId(goal.parameters[0]); id != -1; id = iterator.GetNextHumanId())
            {
                ++count;
                if (NXBasics::Runtime_TimeSigned32(goal.parameters[1]) <= count) return true;
            }
            return false;
        }
        case 46: return Read<std::uint8_t>(GetMission(manager, goal.parameters[0]), 0x26) != 0;
        case 50:
        {
            const void* mission = GetMission(manager, goal.parameters[0]);
            const auto count = Read<std::int32_t>(mission, 0x38);
            std::int32_t fulfilled = 0;
            for (std::int32_t index = 0; index < count; ++index)
                if (Read<std::uint8_t>(mission, 0x60u + static_cast<std::uint32_t>(index) * 0x28u) != 0) ++fulfilled;
            return GoalsMatch(Read<std::int32_t>(mission, 0x34), count, fulfilled);
        }
        default: return false;
        }
    }

    // Partial original function: FUN_0041f9f7; remaining script actions are tracked separately.
    void MissionManager_l_ExecuteResult(void* manager, const SResult& result)
    {
        const auto* p = result.parameters;
        switch (result.type)
        {
        case 9:
            if (p[1] != 0) Write(manager, 0, p[0]);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1B, 1, p[0], 0, 0);
            Write<std::uint8_t>(manager, 6, 1);
            if (NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\gui\\Briefing_Popup.wav", 100, 0xFFFFFFFFu, 1, false, false, 100);
            break;
        case 10: MissionData_SetMissionActiveFlag(GetMission(manager, p[0]), true); break;
        case 11: MissionData_SetMissionActiveFlag(GetMission(manager, p[0]), false); break;
        case 12:
            NC2LogicExternals::External_Misc_SendToUserStringMessage(StringAccess_GetMiscStringPtr(10), false);
            Write<std::uint8_t>(manager, 4, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, p[0], 6, 0);
            if (gGameStaticVars.mapType == 4) MultiPlayerGoalManager_ExternalTrigger_WonByMissionManager(gMultiPlayerGoalManager, p[0]);
            break;
        case 13:
            NC2LogicExternals::External_Misc_SendToUserStringMessage(StringAccess_GetMiscStringPtr(11), false);
            Write<std::uint8_t>(manager, 5, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, p[0], 7, 0);
            if (gGameStaticVars.mapType == 4) MultiPlayerGoalManager_ExternalTrigger_LostByMissionManager(gMultiPlayerGoalManager, p[0]);
            break;
        case 14: gCampaignStaticDataManager->CampaignSubMission_DoAllow(p[0], p[1], true); break;
        case 16:
        {
            // SMapMigPoint::SetPosition(..., false) truncates both coordinates to WORDs.
            const std::uint32_t point = static_cast<std::uint16_t>(p[1]) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(p[2])) << 16);
            if (Read<std::uint16_t>(&point, 0) != 0 && Read<std::uint16_t>(&point, 2) != 0 && p[3] != 0) GameMap_Exploration_DoExplore(p[0], &point, p[3]);
            else GameMap_Exploration_DoExploreAll(p[0]);
            break;
        }
        case 17: CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2F, 1, 0, 0, 0); break;
        case 18: AIManager_EntryPoint_Condition_ExternalActivate_SetFlag(gAIManager, p[0], p[1], p[2] != 0); break;
        case 19: GameControl_Diplomacy_SetState(gGameControl, nullptr, p[0], p[1], p[2]); break;
        case 20: Write<std::uint8_t>(GetMission(manager, p[0]), 0x24, p[1] != 0); break;
        case 27:
            TributeManager_CreateTribute(gTributeManager, p[1], p[2], p[0]);
            (static_cast<STributeData*>(gTributeManager) + p[0])->descriptionStringId = p[3];
            break;
        case 28: TributeManager_AddTribute(gTributeManager, p[0], p[1], p[2]); break;
        case 32: Write(manager, 0x0C, p[0]); Write(manager, 0x10, p[1]); Write<std::uint8_t>(manager, 8, 1); break;
        case 33: Write<std::uint8_t>(manager, 0x14, 1); break;
        case 36:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto flags = Read<std::uint8_t>(human, 0xB84);
                Write<std::uint8_t>(human, 0xB84, p[1] != 0 ? flags | 0x80u : flags & 0x7Fu);
            }
            break;
        default: break;
        }
    }

    // Original function: FUN_0042a5a0.
    bool AIManager_EntryPoint_Condition_ExternalActivate_SetFlag(void* manager, std::uint32_t player, std::uint32_t index, bool active)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_EntryPoint_Condition_ExternalActivate_SetFlag(handler, index, active);
    }

    // Original function: FUN_004593c6; only ExternalActivate condition records accept a flag.
    bool AISinglePlayerHandler_EntryPoint_Condition_ExternalActivate_SetFlag(void* handler, std::uint32_t index, bool active)
    {
        if (index >= 100 || Read<std::uint32_t>(handler, index * 0x48u + 0x18u) != 11) return false;
        Write<std::uint8_t>(handler, index * 0x48u + 0x24u, active);
        return true;
    }
}
