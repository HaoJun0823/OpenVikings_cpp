#include "C2MissionExecution.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2GameControlNetwork.h"
#include "C2GameMapAccess.h"
#include "C2HumanIterator.h"
#include "C2VehicleIterator.h"
#include "C2VehicleAccess.h"
#include "C2HumanWalking.h"
#include "C2HumanInitialization.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanJobAccess.h"
#include "C2GameControlStartup.h"
#include "C2HouseBuildPosition.h"
#include "C2MapPathfinderSearch.h"
#include "C2EngineLandscapeAccess.h"
#include "C2StaticVehicleTypeData.h"
#include "C2Debug.h"
#include "C2OverviewObjectIterators.h"
#include "C2GuideAccess.h"
#include "C2AISavegameAccess.h"
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
        case 38: return Read<std::uint8_t>(gGameControl, p[0] * 0x4A80u + p[1] * 56u + p[2] + 0x948u) != 0;
        case 39: return Read<std::uint8_t>(gGameControl, p[0] * 0x4A80u + p[1] * 66u + p[2] + 0x1D4Cu) != 0;
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
        case 1:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            Logic_EntryPoint_Add_Human(p[0], p[1], p[2], &point, p[5], p[6]);
            break;
        }
        case 2:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            const auto vehicleId = Logic_EntryPoint_Add_Vehicle(p[0], p[1], p[2], &point, p[5]);
            if (vehicleId == -1 || p[6] == 0) break;
            void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(vehicleId) * CVehicleArray::ElementSize;
            const auto* type = Vehicle_GetVehicleTypeDataPtr(vehicle);
            std::uint32_t entryPoint = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &entryPoint, 0);
            if (!MapMigPoint_IsInsideMap(&entryPoint)) break;
            const auto humanId = Logic_EntryPoint_Add_Human(p[0], p[1], static_cast<std::uint32_t>(type->commanderType), &entryPoint, p[5], 0);
            if (humanId == -1) break;
            void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * CHumanArray::ElementSize;
            // Windows tests nonzero, including the command's unchanged/failed result 1.
            if (Human_DoExecuteUserCommand_AttachVehicle(human, vehicleId, Read<std::uint32_t>(vehicle, 8)) != 0 && Human_VehicleMisc_Enter(human) != 0) break;
            CHumanArray::sTheObjectPtr->FreeHuman(humanId);
            break;
        }
        case 3:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[4]), static_cast<std::int16_t>(p[5]), false);
            const auto engineType = NC2LogicExternals::External_HouseType_GetEngineId(result.text);
            std::uint32_t search[4] = {p[0], static_cast<std::uint32_t>(engineType), p[2], 0};
            if (CMapPathfinderElement::SearchOverMap(&point, 12, false, Tool_HouseBuildPointSearchEvaluationFunction, search, false, -1)) Logic_EntryPoint_House_Set(p[0], static_cast<std::uint32_t>(engineType), p[2], search + 3, p[3] != 0, p[6]);
            else C2Debug_WarningBoxV("Can't set House near Position %d %d!!!", p[4], p[5]);
            break;
        }
        case 4:
        {
            Logic_EntryPoint_Edit_Start();
            const auto engineId = NC2LogicExternals::External_Landscape_GetLandscapeIdByNamePtr(result.text);
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            NC2LogicExternals::External_Landscape_SetMapMicroPointLandscape(&point, engineId, p[3], p[4] != 0, -1);
            Logic_EntryPoint_Edit_Stop();
            break;
        }
        case 5:
            Write<std::uint8_t>(&gGameStaticVars, 0x1B0, 1);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, 0x0C));
            Write<std::uint8_t>(&gGameStaticVars, 0x1B0, 0);
            break;
        case 6:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, 0x1DC));
            break;
        case 7:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_House_Remove(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize + 0x1C);
            break;
        case 8:
        {
            Logic_EntryPoint_Edit_Start();
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 0, 0, -1, 0, -1);
            Logic_EntryPoint_Edit_Stop();
            break;
        }
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
        case 21:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
                Human_ChangePlayerId(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, NXBasics::Runtime_TimeSigned32(p[1]), true);
            break;
        case 22:
        {
            CHumanIterator humans;
            CAnimalIterator animals;
            CVehicleIterator vehicles;
            CHouseIterator houses;
            CGuideIterator guides;
            for (auto id = vehicles.GetFirstVehicleId(p[0]); id != -1; id = vehicles.GetNextVehicleId())
                Vehicle_ChangePlayerId(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, p[1]);
            for (auto id = houses.GetFirstHouseId(p[0]); id != -1; id = houses.GetNextHouseId())
                House_ChangePlayerId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, p[1]);
            for (auto id = humans.GetFirstHumanId(p[0]); id != -1; id = humans.GetNextHumanId())
                Human_ChangePlayerId(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, NXBasics::Runtime_TimeSigned32(p[1]), false);
            for (auto id = animals.GetFirstAnimalId(p[0]); id != -1; id = animals.GetNextAnimalId())
                Animal_ChangePlayerId(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, p[1]);
            for (auto id = guides.GetFirstGuideId(p[0]); id != -1; id = guides.GetNextGuideId())
            {
                Write(CGuideArray::mStaticVars + static_cast<std::uint32_t>(id) * CGuideArray::ElementSize, 4, p[1]);
                GuideManager_RecalculateGuideConnections(gGuideManager, id);
            }
            if (NXBasics::Runtime_TimeSigned32(p[1]) < 20 && Read<std::uint32_t>(gGameControl, p[1] * 0x4A80u + 4) == 2) AIManager_EntryPoint_RebuildHouseArray(gAIManager, NXBasics::Runtime_TimeSigned32(p[1]));
            break;
        }
        case 23:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            Tool_Map_GetNotBlockedPosition(&point, false);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto objectId = Read<std::uint32_t>(human, 4);
                const auto owner = Read<std::uint8_t>(human, 0x10);
                Human_DoAddNewUserCommand(human, 4, owner, objectId, 0, MapMigPoint_GetLong(&point), 0);
            }
            break;
        }
        case 24:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            Tool_Map_GetNotBlockedPosition(&point, false);
            alignas(4) std::byte command[20]{};
            Write<std::uint8_t>(command, 0, 0x65);
            Write(command, 0x0C, MapMigPoint_GetLong(&point));
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, 0x1E0));
                Write(command, 4, Read<std::uint32_t>(vehicle, 4));
                Vehicle_DoAddNewUserCommand(vehicle, command);
            }
            break;
        }
        case 25:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            Tool_Map_GetNotBlockedPosition(&point, false);
            alignas(4) std::byte command[20]{};
            Write<std::uint8_t>(command, 0, 0x67);
            Write(command, 0x0C, MapMigPoint_GetLong(&point));
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, 0x1E0));
                Write(command, 4, Read<std::uint32_t>(vehicle, 4));
                Vehicle_DoAddNewUserCommand(vehicle, command);
            }
            break;
        }
        case 27:
            TributeManager_CreateTribute(gTributeManager, p[1], p[2], p[0]);
            (static_cast<STributeData*>(gTributeManager) + p[0])->descriptionStringId = p[3];
            break;
        case 28: TributeManager_AddTribute(gTributeManager, p[0], p[1], p[2]); break;
        case 29:
        {
            const auto player = p[0], tribe = p[1], job = p[2];
            const auto offset = player * 0x4A80u + tribe * 56u + job + 0x18u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, 0x14) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case 30:
        {
            const auto player = p[0], tribe = p[1], house = p[2];
            const auto offset = player * 0x4A80u + tribe * 55u + house + 0x2820u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, 0x14) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case 32: Write(manager, 0x0C, p[0]); Write(manager, 0x10, p[1]); Write<std::uint8_t>(manager, 8, 1); break;
        case 33: Write<std::uint8_t>(manager, 0x14, 1); break;
        case 34:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
                Vehicle_ChangePlayerId(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, p[1]);
            break;
        case 35:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
                House_ChangePlayerId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, p[1]);
            break;
        case 36:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto flags = Read<std::uint8_t>(human, 0xB84);
                Write<std::uint8_t>(human, 0xB84, p[1] != 0 ? flags | 0x80u : flags & 0x7Fu);
            }
            break;
        case 37:
        {
            const auto offset = p[0] * 0x4A80u + p[1] * 56u + p[2] + 0x948u;
            if (Read<std::uint8_t>(gGameControl, offset) == 0) Write<std::uint8_t>(gGameControl, offset, 1);
            break;
        }
        case 38:
            if (Read<std::uint8_t>(gGameControl, p[0] * 0x4A80u + p[1] * 55u + p[2] + 0x3126u) == 0) PlayerAllowedThings_MarkHouseAvailable(static_cast<std::byte*>(gGameControl) + p[0] * 0x4A80u + 0x18, p[1], p[2]);
            break;
        case 39:
            for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0x18); ++index) MissionData_SetMissionActiveFlag(GetMission(manager, static_cast<std::uint32_t>(index)), false);
            break;
        case 42:
        {
            const auto player = p[0], tribe = p[1], good = p[2];
            const auto offset = player * 0x4A80u + tribe * 66u + good + 0x1278u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, 0x14) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case 43:
        {
            const auto offset = p[0] * 0x4A80u + p[1] * 66u + p[2] + 0x1D4Cu;
            if (Read<std::uint8_t>(gGameControl, offset) == 0) Write<std::uint8_t>(gGameControl, offset, 1);
            break;
        }
        case 63:
        {
            const auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]);
            // Windows 00420FA7 tests the mission-check ID, not the second script parameter.
            if (id != -1) NC2LogicExternals::External_Gui_SelectHuman(id, p[0] != 0);
            break;
        }
        case 91:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            Map_HouseBuildForbiddenFlagTool_SetArea(point, NXBasics::Runtime_TimeSigned32(p[2]), p[3] != 0);
            break;
        }
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
