#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "C2MissionExecution.h"
#include "C2ScriptObjectAccess.h"
#include "C2WallRoadConstruction.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2EngineLifetimeAccess.h"
#include "C2HumanUserControl.h"
#include "C2MapMoveableAccess.h"
#include "C2HumanChest.h"
#include "C2AnimalAccess.h"
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
#include "C2MapResourceTools.h"
#include "C2TributeAccess.h"
#include "C2MissionStartObjects.h"
#include "C2MissionOnscreenInfoAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2CampaignStaticDataManager.h"
#include "C2MultiPlayerGoalAccess.h"
#include "C2SavegameIO.h"
#include "C2StringAccess.h"
#include "C2UserMessageAccess.h"
#include "DSManager.h"
#include "MasterControlProgram.h"
#include "XCrtFileAccess.h"
#include "InGameGuiManager.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_00422fdb; the original index is unchecked.
    std::uint32_t SResult::GetParameter(std::uint32_t index) const { return Read<std::uint32_t>(this, 4u + index * 4u); }

    namespace
    {
        void* GetMission(void* manager, std::uint32_t index) { return static_cast<std::byte*>(manager) + 0x1C + index * 0xDD8u; }
        bool GoalsMatch(std::int32_t mode, std::int32_t count, std::int32_t fulfilled)
        {
            switch (mode)
            {
            case SwitchIds::MissionGoalMatchMode::All: return fulfilled == count;
            case SwitchIds::MissionGoalMatchMode::Any: return fulfilled >= 1;
            case SwitchIds::MissionGoalMatchMode::MajorityAtLeastHalf: return fulfilled >= (count >> 1) + count % 2;
            case SwitchIds::MissionGoalMatchMode::None: return fulfilled == 0;
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
            const auto campaignId = Read<std::uint32_t>(manager, 0x0C), mapId = Read<std::uint32_t>(manager, 0x10), pathfinderLimitOverride = Read<std::uint32_t>(gGameControl, NC2Layout::GameControlOffset::DifficultyLevel);
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

    // Original function: FUN_0041e45e; all 63 native goal IDs retain their distinct branches.
    bool MissionManager_l_CheckGoal(void* manager, const SMissionGoal& goal)
    {
        const auto* p = goal.parameters;
        switch (goal.type)
        {
        case SwitchIds::MissionGoal::True: return true;
        case SwitchIds::MissionGoal::BuildHouses:
        {
            CHouseIterator iterator;
            std::uint32_t count = 0;
            bool fulfilled = false;
            for (auto id = iterator.GetFirstHouseId(p[0]); id != -1; id = iterator.GetNextHouseId())
            {
                void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (Read<std::uint32_t>(house, NC2Layout::HouseOffset::LogicType) != p[1] || Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) != 3) continue;
                ++count;
                Write(house, 0x7CC, p[3]);
                if (NXBasics::Runtime_TimeSigned32(count) >= NXBasics::Runtime_TimeSigned32(p[2])) fulfilled = true;
            }
            return fulfilled;
        }
        case SwitchIds::MissionGoal::GoodsInHouses:
        {
            std::uint32_t count = 0;
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                count += House_Stock_GetAmountOfGoods(house, p[1], false);
                if (NXBasics::Runtime_TimeSigned32(count) >= NXBasics::Runtime_TimeSigned32(p[2])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::FindHouses:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[1]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[1], id))
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                const void* micro = GameMap_GetConstMapMicroPointPtr(House_GetMapPositionRef(house));
                if (p[0] >= 16 || (Read<std::uint16_t>(micro, 8) & (1u << p[0])) != 0) return true;
            }
            return false;
        case SwitchIds::MissionGoal::FindPosByHumans:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const void* point = Human_GetMapPositionRef(human);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]))) <= p[3]) return true;
            }
            return false;
        case SwitchIds::MissionGoal::FindPosByVehicles:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                const void* point = Vehicle_GetMapPositionRef(vehicle);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]))) <= p[3]) return true;
            }
            return false;
        case SwitchIds::MissionGoal::FindHousesByHumans:
            for (auto humanId = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); humanId != -1; humanId = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], humanId))
            {
                const void* point = Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * CHumanArray::ElementSize);
                for (auto houseId = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[1]); houseId != -1; houseId = MissionCheck_GetNextHouseIdWithMissionCheckId(p[1], houseId))
                {
                    const void* target = House_GetMapPositionRef(CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseId) * CHouseArray::ElementSize);
                    if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(target, 0), Read<std::int16_t>(target, 2))) <= p[2]) return true;
                }
            }
            return false;
        case SwitchIds::MissionGoal::FindHousesByVehicles:
            for (auto vehicleId = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); vehicleId != -1; vehicleId = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], vehicleId))
            {
                const void* point = Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(vehicleId) * CVehicleArray::ElementSize);
                for (auto houseId = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[1]); houseId != -1; houseId = MissionCheck_GetNextHouseIdWithMissionCheckId(p[1], houseId))
                {
                    const void* target = House_GetMapPositionRef(CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseId) * CHouseArray::ElementSize);
                    if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(target, 0), Read<std::int16_t>(target, 2))) <= p[2]) return true;
                }
            }
            return false;
        case SwitchIds::MissionGoal::HumansDied: return MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]) == -1;
        case SwitchIds::MissionGoal::VehiclesDied: return MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]) == -1;
        case SwitchIds::MissionGoal::HousesDied: return MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]) == -1;
        case SwitchIds::MissionGoal::PlayerDied: return NXBasics::Runtime_TimeSigned32(p[0]) < 20 && Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + 8u) != 0;
        case SwitchIds::MissionGoal::FindPosByPlayersMapMoveable:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            CHumanIterator humans;
            if (humans.GetFirstHumanId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[3])) != -1) return true;
            CVehicleIterator vehicles;
            return vehicles.GetFirstVehicleId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[3])) != -1;
        }
        case SwitchIds::MissionGoal::BuildHouseOnContinent:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
            CHouseIterator iterator;
            for (auto id = iterator.GetFirstHouseId(p[0]); id != -1; id = iterator.GetNextHouseId())
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) == 3 && Read<std::uint32_t>(house, NC2Layout::HouseOffset::LogicType) == p[3] && Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(House_GetMapPositionRef(house)), 6) == continent) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::DiplomacyState: return GameControl_Diplomacy_GetState(gGameControl, NXBasics::Runtime_TimeSigned32(goal.parameters[0]), NXBasics::Runtime_TimeSigned32(goal.parameters[1])) == NXBasics::Runtime_TimeSigned32(goal.parameters[2]);
        case SwitchIds::MissionGoal::HumansWithHome:
        {
            CHumanIterator iterator;
            std::uint32_t count = 0;
            for (auto id = iterator.GetFirstHumanId(p[0]); id != -1; id = iterator.GetNextHumanId())
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (!Human_IsAttachedToHomeHouse(human) || !Human_IsAgeAdult(human)) continue;
                const void* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, NC2Layout::HumanOffset::HomeHouseId) * CHouseArray::ElementSize;
                if (Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) == 3 && NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[1])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::CheckMission: return MissionManager_l_CheckMission(manager, GetMission(manager, goal.parameters[0]), false);
        case SwitchIds::MissionGoal::PayTribute:
        {
            const auto* tribute = static_cast<const STributeData*>(gTributeManager) + p[0];
            return tribute->allocated != 0 && tribute->paid != 0;
        }
        case SwitchIds::MissionGoal::Population:
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
        case SwitchIds::MissionGoal::JobEnabled: return Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 56u + p[2] + 0x948u) != 0;
        case SwitchIds::MissionGoal::GoodProduceable: return Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 66u + p[2] + 0x1D4Cu) != 0;
        case SwitchIds::MissionGoal::HumanAttachedToWorkHouse:
        {
            CHumanIterator iterator;
            std::uint32_t count = 0;
            for (auto id = iterator.GetFirstHumanId(p[0]); id != -1; id = iterator.GetNextHumanId())
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == p[1] && Human_IsAttachedToWorkHouse(human) && NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[2])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::IfMissionIsActive: return Read<std::uint8_t>(GetMission(manager, goal.parameters[0]), 0x26) != 0;
        case SwitchIds::MissionGoal::IsMissionDone:
        {
            const void* mission = GetMission(manager, goal.parameters[0]);
            const auto count = Read<std::int32_t>(mission, 0x38);
            std::int32_t fulfilled = 0;
            for (std::int32_t index = 0; index < count; ++index)
                if (Read<std::uint8_t>(mission, 0x60u + static_cast<std::uint32_t>(index) * 0x28u) != 0) ++fulfilled;
            return GoalsMatch(Read<std::int32_t>(mission, 0x34), count, fulfilled);
        }
        case SwitchIds::MissionGoal::NumberOfHousesInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            return Tool_CountHousesOnMap(p[0], p[1], point, p[5]) >= p[2];
        }
        case SwitchIds::MissionGoal::NumberOfGoodsInHousesInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            return Tool_CountGoodsInHouses(p[0], p[1], point, p[5]) >= p[2];
        }
        case SwitchIds::MissionGoal::NumberOfGoodsInVehiclesInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[4]), static_cast<std::int16_t>(p[5]), false);
            return Tool_CountGoodsInVehicles(p[0], p[1], p[2], point, p[6]) >= p[3];
        }

        case SwitchIds::MissionGoal::BuildVehicles:
        {
            CVehicleIterator iterator;
            std::uint32_t count = 0;
            bool fulfilled = false;
            for (auto id = iterator.GetFirstVehicleId(p[0]); id != -1; id = iterator.GetNextVehicleId())
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType) != p[1]) continue;
                Write(vehicle, 0x51C, p[3]);
                if (NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[2])) fulfilled = true;
            }
            return fulfilled;
        }
        case SwitchIds::MissionGoal::BuildHumans:
        {
            CHumanIterator iterator;
            std::uint32_t count = 0;
            bool fulfilled = false;
            for (auto id = iterator.GetFirstHumanId(p[0]); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) != p[1]) continue;
                if (p[3] != 12345) Write(human, 0xB80, p[3]);
                if (NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[2])) fulfilled = true;
            }
            return fulfilled;
        }
        case SwitchIds::MissionGoal::GoodsInVehicles:
        {
            std::uint32_t count = 0;
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                count += Vehicle_Stock_GetAmountOfGoods(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, p[1]);
                if (NXBasics::Runtime_TimeSigned32(count) >= NXBasics::Runtime_TimeSigned32(p[2])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::GoodsGlobal:
        {
            CHouseIterator iterator;
            std::uint32_t count = 0;
            for (auto id = iterator.GetFirstHouseId(p[0]); id != -1; id = iterator.GetNextHouseId())
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (!House_Stock_CanStoreGoodType(house, p[1], true)) continue;
                count += House_Stock_GetAmountOfGoods(house, p[1], true);
                if (NXBasics::Runtime_TimeSigned32(count) >= NXBasics::Runtime_TimeSigned32(p[2])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::FindPos:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            return p[0] >= 16 || (Read<std::uint16_t>(GameMap_GetConstMapMicroPointPtr(&point), 8) & (1u << p[0])) != 0;
        }
        case SwitchIds::MissionGoal::FindHumans:
        case SwitchIds::MissionGoal::FindVehicles:
        case SwitchIds::MissionGoal::FindAnimals:
        {
            const auto first = goal.type == 8 ? MissionCheck_GetFirstHumanIdWithMissionCheckId : goal.type == 9 ? MissionCheck_GetFirstVehicleIdWithMissionCheckId : MissionCheck_GetFirstAnimalIdWithMissionCheckId;
            const auto next = goal.type == 8 ? MissionCheck_GetNextHumanIdWithMissionCheckId : goal.type == 9 ? MissionCheck_GetNextVehicleIdWithMissionCheckId : MissionCheck_GetNextAnimalIdWithMissionCheckId;
            for (auto id = first(p[1]); id != -1; id = next(p[1], id))
            {
                const void* point = goal.type == 8 ? Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize) : goal.type == 9 ? Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize) : Animal_GetMapPositionRef(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize);
                const auto mask = Read<std::uint16_t>(GameMap_GetConstMapMicroPointPtr(point), 8);
                if (p[0] >= 16 || (mask & (1u << p[0])) != 0) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::FindHumansByHumans:
        case SwitchIds::MissionGoal::FindHumansByVehicles:
        case SwitchIds::MissionGoal::FindVehiclesByVehicles:
        {
            const auto firstOuter = goal.type == 13 ? MissionCheck_GetFirstHumanIdWithMissionCheckId : MissionCheck_GetFirstVehicleIdWithMissionCheckId;
            const auto nextOuter = goal.type == 13 ? MissionCheck_GetNextHumanIdWithMissionCheckId : MissionCheck_GetNextVehicleIdWithMissionCheckId;
            const auto firstInner = goal.type == 15 ? MissionCheck_GetFirstVehicleIdWithMissionCheckId : MissionCheck_GetFirstHumanIdWithMissionCheckId;
            const auto nextInner = goal.type == 15 ? MissionCheck_GetNextVehicleIdWithMissionCheckId : MissionCheck_GetNextHumanIdWithMissionCheckId;
            for (auto outer = firstOuter(p[0]); outer != -1; outer = nextOuter(p[0], outer))
            {
                const void* a = goal.type == 13 ? Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(outer) * CHumanArray::ElementSize) : Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(outer) * CVehicleArray::ElementSize);
                for (auto inner = firstInner(p[1]); inner != -1; inner = nextInner(p[1], inner))
                {
                    const void* b = goal.type == 15 ? Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(inner) * CVehicleArray::ElementSize) : Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(inner) * CHumanArray::ElementSize);
                    if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(a, 0), Read<std::int16_t>(a, 2), Read<std::int16_t>(b, 0), Read<std::int16_t>(b, 2))) <= p[2]) return true;
                }
            }
            return false;
        }
        case SwitchIds::MissionGoal::NumberOfSoldiers:
        {
            CHumanIterator iterator;
            std::uint32_t count = 0;
            for (auto id = iterator.GetFirstHumanId(p[0]); id != -1; id = iterator.GetNextHumanId())
                if (Human_IsJobSoldier(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize) && NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[1])) return true;
            return false;
        }
        case SwitchIds::MissionGoal::DetectGuide:
        {
            CGuideIterator iterator;
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            return iterator.GetFirstGuideId(p[0], &point, p[3]) != -1;
        }
        // Timed goals are evaluated by l_CheckMission, not by this native dispatcher.
        case SwitchIds::MissionGoal::TimeGone:
        case SwitchIds::MissionGoal::RandomTimeGone: return false;
        case SwitchIds::MissionGoal::PlayerSeen: return Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] + 0x45F8u) != 0;
        case SwitchIds::MissionGoal::IsHumanInVehicle:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint8_t>(human, 0xB51) == 0 || Read<std::uint32_t>(CVehicleArray::mStaticVars + Read<std::uint32_t>(human, NC2Layout::HumanOffset::AttachedVehicleId) * CVehicleArray::ElementSize, 0x51C) != p[1]) return false;
            }
            return true;
        case SwitchIds::MissionGoal::FindHumansByPlayersMM:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                const void* point = Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize);
                CHumanIterator humans;
                if (humans.GetFirstHumanId(p[1], point, NXBasics::Runtime_TimeSigned32(p[2])) != -1) return true;
                CVehicleIterator vehicles;
                if (vehicles.GetFirstVehicleId(p[1], point, NXBasics::Runtime_TimeSigned32(p[2])) != -1) return true;
            }
            return false;
        case SwitchIds::MissionGoal::SoldiersDied: return Read<std::uint32_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + 0x4A1Cu) >= p[1];
        case SwitchIds::MissionGoal::AnimalsDied: return MissionCheck_GetNumberOfAnimalsWithMissionCheckId(p[0]) == 0;
        case SwitchIds::MissionGoal::PlayerAttackedByPlayer: return Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] + 0x4944u) != 0;
        case SwitchIds::MissionGoal::NumberOfHumansDied: return Read<std::uint32_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + 0x4A18u) >= p[1];
        case SwitchIds::MissionGoal::NumberOfGoodsTraded: return NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 4u + 0x4A30u)) >= NXBasics::Runtime_TimeSigned32(p[2]);
        case SwitchIds::MissionGoal::CheckNumberOfWildAnimals:
        case SwitchIds::MissionGoal::NumberOfAnimals:
        {
            CAnimalIterator iterator;
            std::uint32_t count = 0;
            const auto type = goal.type == 44 ? p[0] : p[1], threshold = goal.type == 44 ? p[1] : p[2];
            for (auto id = iterator.GetFirstAnimalId(goal.type == 44 ? 20 : p[0]); id != -1; id = iterator.GetNextAnimalId())
                if (Read<std::uint32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, 0x14) == type && NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(threshold)) return true;
            return false;
        }
        case SwitchIds::MissionGoal::NumberOfHumansKilled: return Read<std::uint32_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + 0x4618u) + Read<std::uint32_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + 0x4614u) >= p[1];
        case SwitchIds::MissionGoal::HumanIsOnContinent:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            if (!MapMigPoint_IsInsideMap(&point)) return false;
            const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
                if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize)), 6) == continent) return true;
            return false;
        }
        case SwitchIds::MissionGoal::NumberOfSoldiersNearPos:
        case SwitchIds::MissionGoal::NumberOfCivilainsNearPos:
        {
            std::uint32_t point, count = 0;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            CHumanIterator humans;
            for (auto id = humans.GetFirstSectorHumanId(p[0], point, NXBasics::Runtime_TimeSigned32(p[3]), goal.type == 51); id != -1; id = humans.GetNextSectorHumanId())
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (goal.type == 52 || (Human_IsJobSoldier(human) && !Human_IsJobHero(human))) ++count;
            }
            CVehicleIterator vehicles;
            for (auto id = vehicles.GetFirstVehicleId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[3])); id != -1; id = vehicles.GetNextVehicleId())
            {
                const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                auto qualifies = [&](std::uint32_t humanId) { const void* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize; return Human_IsJobSoldier(human) == (goal.type == 51) && !Human_IsJobHero(human); };
                if (Read<std::uint8_t>(vehicle, 0x2EC) != 0 && qualifies(static_cast<std::uint32_t>(Vehicle_Passengers_GetAttachedCommanderId(vehicle)))) ++count;
                if (Vehicle_Passengers_GetNumberOfAttachedHumans(vehicle, true) != 0)
                    for (std::uint32_t slot = 0; slot < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots); ++slot)
                    {
                        const void* reference = static_cast<const std::byte*>(vehicle) + 0x1F0u + slot * 12u;
                        if (HumanReference_IsAllocated(reference) && qualifies(Read<std::uint32_t>(reference, 0))) ++count;
                    }
            }
            return NXBasics::Runtime_TimeSigned32(count) >= NXBasics::Runtime_TimeSigned32(p[4]);
        }
        case SwitchIds::MissionGoal::ChestNearPos:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            auto isChest = [&]() { if (!MapMigPoint_IsInsideMap(&point)) return false; const void* micro = GameMap_GetConstMapMicroPointPtr(&point); return Read<std::uint8_t>(micro, 0) == 1 && (Read<std::uint8_t>(micro, 2) == 0x55 || Read<std::uint8_t>(micro, 2) == 0x56); };
            if (isChest()) return true;
            for (std::int32_t radius = 1; radius <= NXBasics::Runtime_TimeSigned32(p[2]); ++radius)
            {
                MapMigPoint_MoveCursorUnwrapped(point, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::int32_t step = 0; step < radius; ++step) { if (isChest()) return true; MapMigPoint_MoveCursorUnwrapped(point, direction); }
            }
            return false;
        }
        case SwitchIds::MissionGoal::NumberOfGoodsInArea:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            const auto houseCount = Tool_CountGoodsInHouses(p[0], p[1], point, p[5]);
            const auto count = houseCount + Tool_CountGoodsOnMap(p[1], point, p[5]);
            return count >= p[2];
        }
        case SwitchIds::MissionGoal::NumberOfVehiclesInArea:
        case SwitchIds::MissionGoal::NumberOfAnimalsInArea:
        {
            std::uint32_t point, count = 0;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            if (goal.type == 57)
            {
                CVehicleIterator iterator;
                for (auto id = iterator.GetFirstVehicleId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[5])); id != -1; id = iterator.GetNextVehicleId())
                    if (Read<std::uint32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, 0x1E8) == p[1]) ++count;
                return count >= p[2];
            }
            CAnimalIterator iterator;
            for (auto id = iterator.GetFirstAnimalId(p[0]); id != -1; id = iterator.GetNextAnimalId())
            {
                const void* animal = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize;
                if (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType) != p[1]) continue;
                const void* a = Animal_GetMapPositionRef(animal);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(a, 0), Read<std::int16_t>(a, 2), Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2))) <= p[5] && NXBasics::Runtime_TimeSigned32(++count) >= NXBasics::Runtime_TimeSigned32(p[2])) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::CheckHumanJob:
        {
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(); id != -1; id = iterator.GetNextHumanId())
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, 0xB80) == p[0] && Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == p[1]) return true;
            }
            return false;
        }
        case SwitchIds::MissionGoal::IsAnyLandscapeOnPoint:
        case SwitchIds::MissionGoal::IsLandscapePlayer10ConstructionSignOnPoint:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            if (!MapMigPoint_IsInsideMap(&point)) return false;
            const void* micro = GameMap_GetConstMapMicroPointPtr(&point);
            if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0) return false;
            if (goal.type == 61) return true;
            const auto engine = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(&point);
            if (engine == -1) return false;
            const auto target = NC2LogicExternals::External_Landscape_GetLandscapeIdByNamePtr("player10 construction sign");
            return target != -1 && engine == target;
        }

        default: return false;
        }
    }

    // Original function: FUN_0041f9f7; all 103 native result IDs, including the two no-op entries.
    void MissionManager_l_ExecuteResult(void* manager, const SResult& result)
    {
        const auto* p = result.parameters;

        auto redraw = [](std::uint32_t point, std::int32_t radius)
        {
            CallbackManager_GetExistingObjectPtr()->ExecuteCallback(0x25, true, 4, point, 0);
            for (std::int32_t ring = 1; ring <= radius; ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(point, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::int32_t step = 0; step < ring; ++step)
                    {
                        CallbackManager_GetExistingObjectPtr()->ExecuteCallback(0x25, true, 4, point, 0);
                        MapMigPoint_MoveCursorUnwrapped(point, direction);
                    }
            }
        };
        auto moveHuman = [&](std::int32_t id, std::uint32_t target)
        {
            void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            redraw(Read<std::uint32_t>(Human_GetMapPositionRef(human), 0), 1);
            MapMoveable_Attachment_AttachToMap(gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, NC2Layout::HumanOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable, &target);
            Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(Object_GetField10(human, nullptr)), ArrayObject_GetId(human, nullptr), Object_GetField08(human, nullptr), target, 0);
            GameMap_Exploration_DoExplore(Object_GetField10(human, nullptr), &target, Human_GetExplorationRange(human, nullptr));
            redraw(target, 1);
        };
        auto removeMarkers = [](std::uint32_t point, std::uint32_t type)
        {
            for (;;)
            {
                CMapMoveableIterator iterator;
                auto id = iterator.GetFirstMapMoveableId(&point);
                for (; id != -1; id = iterator.GetNextMapMoveableId())
                {
                    const void* moveable = GetMapMoveablePtr(id);
                    if (Object_GetField08(moveable, nullptr) == 4 && Object_GetField10(MapMoveable_GetMarkerPtr(moveable, nullptr), nullptr) == type) break;
                }
                if (id == -1) break;
                Logic_EntryPoint_FreeMapMoveable(id);
                // Deletion invalidates the cursor; Windows restarts at the first object on this point.
            }
        };
        switch (result.type)
        {
        case SwitchIds::MissionResult::SetHuman:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            Logic_EntryPoint_Add_Human(p[0], p[1], p[2], &point, p[5], p[6]);
            break;
        }
        case SwitchIds::MissionResult::SetVehicle:
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
        case SwitchIds::MissionResult::SetHouse:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[4]), static_cast<std::int16_t>(p[5]), false);
            const auto engineType = NC2LogicExternals::External_HouseType_GetEngineId(result.text);
            std::uint32_t search[4] = {p[0], static_cast<std::uint32_t>(engineType), p[2], 0};
            if (CMapPathfinderElement::SearchOverMap(&point, 12, false, Tool_HouseBuildPointSearchEvaluationFunction, search, false, -1)) Logic_EntryPoint_House_Set(p[0], static_cast<std::uint32_t>(engineType), p[2], search + 3, p[3] != 0, p[6]);
            else C2Debug_WarningBoxV("Can't set House near Position %d %d!!!", p[4], p[5]);
            break;
        }
        case SwitchIds::MissionResult::SetLandscape:
        {
            Logic_EntryPoint_Edit_Start();
            const auto engineId = NC2LogicExternals::External_Landscape_GetLandscapeIdByNamePtr(result.text);
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            NC2LogicExternals::External_Landscape_SetMapMicroPointLandscape(&point, engineId, p[3], p[4] != 0, -1);
            Logic_EntryPoint_Edit_Stop();
            break;
        }
        case SwitchIds::MissionResult::RemoveHumans:
            Write<std::uint8_t>(&gGameStaticVars, 0x1B0, 1);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, 0x0C));
            Write<std::uint8_t>(&gGameStaticVars, 0x1B0, 0);
            break;
        case SwitchIds::MissionResult::RemoveVehicles:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, 0x1DC));
            break;
        case SwitchIds::MissionResult::RemoveHouses:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
                Logic_EntryPoint_House_Remove(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize + 0x1C);
            break;
        case SwitchIds::MissionResult::RemoveLandscape:
        {
            Logic_EntryPoint_Edit_Start();
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 0, 0, -1, 0, -1);
            Logic_EntryPoint_Edit_Stop();
            break;
        }
        case SwitchIds::MissionResult::PlayCutscene:
            if (p[1] != 0) Write(manager, 0, p[0]);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1B, 1, p[0], 0, 0);
            Write<std::uint8_t>(manager, 6, 1);
            if (NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\gui\\Briefing_Popup.wav", 100, NXBasics::IntegerRange::UInt32Maximum, 1, false, false, 100);
            break;
        case SwitchIds::MissionResult::ActivateMission: MissionData_SetMissionActiveFlag(GetMission(manager, p[0]), true); break;
        case SwitchIds::MissionResult::DeactivateMission: MissionData_SetMissionActiveFlag(GetMission(manager, p[0]), false); break;
        case SwitchIds::MissionResult::MissionWon:
            NC2LogicExternals::External_Misc_SendToUserStringMessage(StringAccess_GetMiscStringPtr(10), false);
            Write<std::uint8_t>(manager, 4, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, p[0], 6, 0);
            if (gGameStaticVars.mapType == 4) MultiPlayerGoalManager_ExternalTrigger_WonByMissionManager(gMultiPlayerGoalManager, p[0]);
            break;
        case SwitchIds::MissionResult::MissionFailed:
            NC2LogicExternals::External_Misc_SendToUserStringMessage(StringAccess_GetMiscStringPtr(11), false);
            Write<std::uint8_t>(manager, 5, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, p[0], 7, 0);
            if (gGameStaticVars.mapType == 4) MultiPlayerGoalManager_ExternalTrigger_LostByMissionManager(gMultiPlayerGoalManager, p[0]);
            break;
        case SwitchIds::MissionResult::AllowMap: gCampaignStaticDataManager->CampaignSubMission_DoAllow(p[0], p[1], true); break;
        case SwitchIds::MissionResult::ExploreArea:
        {
            // SMapMigPoint::SetPosition(..., false) truncates both coordinates to WORDs.
            const std::uint32_t point = static_cast<std::uint16_t>(p[1]) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(p[2])) << 16);
            if (Read<std::uint16_t>(&point, 0) != 0 && Read<std::uint16_t>(&point, 2) != 0 && p[3] != 0) GameMap_Exploration_DoExplore(p[0], &point, p[3]);
            else GameMap_Exploration_DoExploreAll(p[0]);
            break;
        }
        case SwitchIds::MissionResult::Exit: CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2F, 1, 0, 0, 0); break;
        case SwitchIds::MissionResult::SetExternalFlag: AIManager_EntryPoint_Condition_ExternalActivate_SetFlag(gAIManager, p[0], p[1], p[2] != 0); break;
        case SwitchIds::MissionResult::SetDiplomacy: GameControl_Diplomacy_SetState(gGameControl, nullptr, p[0], p[1], p[2]); break;
        case SwitchIds::MissionResult::SetRandomChestOnRandomPos: Tool_MapChest_SetRandomChestOnRandomPosition(p[0]); break;
        case SwitchIds::MissionResult::SetRandomChestOnPosition:
        {
            const auto mask = p[0];
            const auto y = p[2];
            const auto x = p[1];
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), false);
            Tool_MapChest_SetRandomChestOnPosition(mask, &point);
            break;
        }
        case SwitchIds::MissionResult::SetVisible: Write<std::uint8_t>(GetMission(manager, p[0]), 0x24, p[1] != 0); break;
        case SwitchIds::MissionResult::ChangeHumanPlayerId:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
                Human_ChangePlayerId(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, NXBasics::Runtime_TimeSigned32(p[1]), true);
            break;
        case SwitchIds::MissionResult::ChangePlayerPlayerId:
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
            if (NXBasics::Runtime_TimeSigned32(p[1]) < 20 && Read<std::uint32_t>(gGameControl, p[1] * NC2Layout::RecordSize::GameControlPlayer + 4) == 2) AIManager_EntryPoint_RebuildHouseArray(gAIManager, NXBasics::Runtime_TimeSigned32(p[1]));
            break;
        }
        case SwitchIds::MissionResult::SendHuman:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            Tool_Map_GetNotBlockedPosition(&point, false);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto objectId = Read<std::uint32_t>(human, 4);
                const auto owner = Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId);
                Human_DoAddNewUserCommand(human, 4, owner, objectId, 0, MapMigPoint_GetLong(&point), 0);
            }
            break;
        }
        case SwitchIds::MissionResult::SendVehicle:
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
                Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::PlayerId));
                Write(command, 4, Read<std::uint32_t>(vehicle, 4));
                Vehicle_DoAddNewUserCommand(vehicle, command);
            }
            break;
        }
        case SwitchIds::MissionResult::DockVehicle:
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
                Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::PlayerId));
                Write(command, 4, Read<std::uint32_t>(vehicle, 4));
                Vehicle_DoAddNewUserCommand(vehicle, command);
            }
            break;
        }
        case SwitchIds::MissionResult::PlaySoundEffect:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, p[0], MapMigPoint_GetLong(&point), 0);
            break;
        }
        case SwitchIds::MissionResult::CreateTribute:
            TributeManager_CreateTribute(gTributeManager, p[1], p[2], p[0]);
            (static_cast<STributeData*>(gTributeManager) + p[0])->descriptionStringId = p[3];
            break;
        case SwitchIds::MissionResult::AddTributeGoods: TributeManager_AddTribute(gTributeManager, p[0], p[1], p[2]); break;
        case SwitchIds::MissionResult::AllowJob:
        {
            const auto player = p[0], tribe = p[1], job = p[2];
            const auto offset = player * NC2Layout::RecordSize::GameControlPlayer + tribe * 56u + job + 0x18u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case SwitchIds::MissionResult::AllowHouse:
        {
            const auto player = p[0], tribe = p[1], house = p[2];
            const auto offset = player * NC2Layout::RecordSize::GameControlPlayer + tribe * 55u + house + 0x2820u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case SwitchIds::MissionResult::AddGoodsToHouses:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
            {
                void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (House_Stock_CanStoreGoodType(house, p[1], false) || House_Stock_CanStoreGoodType(house, p[1], true)) House_Stock_ModifyAmount(house, nullptr, p[1], p[2], false);
            }
            break;
        case SwitchIds::MissionResult::StartSubMission: Write(manager, 0x0C, p[0]); Write(manager, 0x10, p[1]); Write<std::uint8_t>(manager, 8, 1); break;
        case SwitchIds::MissionResult::EndSubMission: Write<std::uint8_t>(manager, 0x14, 1); break;
        case SwitchIds::MissionResult::ChangeVehiclesPlayerId:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
                Vehicle_ChangePlayerId(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, p[1]);
            break;
        case SwitchIds::MissionResult::ChangeHousesPlayerId:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id))
                House_ChangePlayerId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, p[1]);
            break;
        case SwitchIds::MissionResult::SetImportHumanFlag:
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto flags = Read<std::uint8_t>(human, NC2Layout::HumanOffset::MiscFlags);
                Write<std::uint8_t>(human, NC2Layout::HumanOffset::MiscFlags, p[1] != 0 ? flags | NXBasics::BitMask::Bit7 : flags & NXBasics::IntegerRange::Int8Maximum);
            }
            break;
        case SwitchIds::MissionResult::EnableJob:
        {
            const auto offset = p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 56u + p[2] + 0x948u;
            if (Read<std::uint8_t>(gGameControl, offset) == 0) Write<std::uint8_t>(gGameControl, offset, 1);
            break;
        }
        case SwitchIds::MissionResult::EnableHouse:
            if (Read<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 55u + p[2] + 0x3126u) == 0) PlayerAllowedThings_MarkHouseAvailable(static_cast<std::byte*>(gGameControl) + p[0] * NC2Layout::RecordSize::GameControlPlayer + 0x18, p[1], p[2]);
            break;
        case SwitchIds::MissionResult::DisableAll:
            for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0x18); ++index) MissionData_SetMissionActiveFlag(GetMission(manager, static_cast<std::uint32_t>(index)), false);
            break;
        case SwitchIds::MissionResult::AllowGood:
        {
            const auto player = p[0], tribe = p[1], good = p[2];
            const auto offset = player * NC2Layout::RecordSize::GameControlPlayer + tribe * 66u + good + 0x1278u;
            if (Read<std::uint8_t>(gGameControl, offset) != 0) break;
            Write<std::uint8_t>(gGameControl, offset, 1);
            CHumanIterator iterator;
            for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType) == tribe) Human_UpdatePlayerSpecificStuffAndCaps(human);
            }
            break;
        }
        case SwitchIds::MissionResult::EnableGood:
        {
            const auto offset = p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] * 66u + p[2] + 0x1D4Cu;
            if (Read<std::uint8_t>(gGameControl, offset) == 0) Write<std::uint8_t>(gGameControl, offset, 1);
            break;
        }
        case SwitchIds::MissionResult::MissionQuitAndPlayVideo:
        {
            char fileName[100];
            NXBasics::Runtime_PrintString(fileName, "seq_%4.4d", NXBasics::Runtime_TimeSigned32(p[0]));
            NC2LogicExternals::External_Misc_PropertyString_AddString("start_fmv", fileName);
            break;
        }
        case SwitchIds::MissionResult::StartEarthquake:
            NC2LogicExternals::External_FX_Earthquake_Start(p[0] * 1000u);
            if (NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\misc\\earthquak.wav", 100, NXBasics::IntegerRange::UInt32Maximum, 1, false, false, 100);
            break;
        case SwitchIds::MissionResult::SelectHuman:
        {
            const auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]);
            // Windows 00420FA7 tests the mission-check ID, not the second script parameter.
            if (id != -1) NC2LogicExternals::External_Gui_SelectHuman(id, p[0] != 0);
            break;
        }
        case SwitchIds::MissionResult::AddGoodsToMapArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[2]), static_cast<std::int16_t>(p[3]), false);
            Tool_AddGoodsToMapArea(p[0], NXBasics::Runtime_TimeSigned32(p[1]), point, p[4], p[5] != 0, p[6]);
            break;
        }
        case SwitchIds::MissionResult::RemoveGoodsFromMapArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[2]), static_cast<std::int16_t>(p[3]), false);
            Tool_RemoveGoodsFromMapArea(p[0], NXBasics::Runtime_TimeSigned32(p[1]), point, p[4], p[5] != 0, p[6]);
            break;
        }
        case SwitchIds::MissionResult::SetHouseExtensionLevel:
        {
            std::int32_t houses[10], count = 0;
            CHouseIterator iterator;
            for (auto id = iterator.GetFirstHouseId(); id != -1; id = iterator.GetNextHouseId())
            {
                if (House_GetMissionCheckId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, nullptr) != p[0]) continue;
                houses[count++] = id;
                if (count == 10) break;
            }
            for (std::int32_t index = 0; index < count; ++index)
            {
                const void* house = GetHousePtr(houses[index]);
                const auto player = House_GetPlayerId(house), engineType = Read<std::uint32_t>(house, NC2Layout::HouseOffset::EngineType);
                const auto point = Read<std::uint32_t>(House_GetMapPositionRef(house), 0);
                Logic_EntryPoint_House_Remove(House_GetMapPositionRef(house));
                Logic_EntryPoint_House_Set(player, engineType, p[1], &point, true, p[0]);
            }
            break;
        }
        case SwitchIds::MissionResult::InfoCountHousesInArea:
            if (gMissionOnscreenInfoManager != nullptr) MissionOnscreenInfoManager_Info_Set(gMissionOnscreenInfoManager, p[0], p[1], 3, p[2], p[3], p[4], p[5], p[6], p[7], 0, 0, 0);
            break;
        case SwitchIds::MissionResult::RemoveHPsOfHousesInArea:
        case SwitchIds::MissionResult::RemoveHPsOfHousesInAreaX:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            std::int32_t houses[100], count = 0;
            CHouseIterator iterator;
            for (auto id = iterator.GetFirstHouseId(p[0], &point, p[3]); id != -1; id = iterator.GetNextHouseId())
            {
                if (result.type == 87 && House_GetMissionCheckId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, nullptr) == p[5]) continue;
                houses[count++] = id;
                if (count == 100) break;
            }
            const auto delta = NXBasics::Runtime_TimeSigned32(0u - p[4]);
            for (std::int32_t index = 0; index < count; ++index) House_HitPoints_ModifyValue(CHouseArray::mStaticVars + static_cast<std::uint32_t>(houses[index]) * CHouseArray::ElementSize, delta);
            break;
        }
        case SwitchIds::MissionResult::SetHouseOverlayState:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id)) House_LandscapeOverlay_SetActive(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, NXBasics::Runtime_TimeSigned32(p[1]), p[2] != 0);
            break;
        case SwitchIds::MissionResult::SetHouseBuildForbiddenArea:
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            Map_HouseBuildForbiddenFlagTool_SetArea(point, NXBasics::Runtime_TimeSigned32(p[2]), p[3] != 0);
            break;
        }
        case SwitchIds::MissionResult::SetHouseBehaviourFlag:
            for (auto id = MissionCheck_GetFirstHouseIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHouseIdWithMissionCheckId(p[0], id)) House_SetBehaviourFlag(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, p[1], p[2] != 0);
            break;

        case SwitchIds::MissionResult::None:
        case SwitchIds::MissionResult::CloseMap: break;
        case SwitchIds::MissionResult::AddGoodsToVehicle:
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(p[0], id))
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                if (!Vehicle_Stock_CanStoreGoodType(vehicle, p[1])) continue;
                Vehicle_Stock_ModifyFutureAmount(vehicle, p[1], NXBasics::Runtime_TimeSigned32(p[2]), NXBasics::IntegerRange::UInt32Maximum);
                Vehicle_Stock_ModifyAmount(vehicle, p[1], NXBasics::Runtime_TimeSigned32(p[2]), NXBasics::IntegerRange::UInt32Maximum);
                const auto wanted = Vehicle_Stock_GetAmountOfWantedGoods(vehicle, p[1]);
                Vehicle_Stock_SetWantedAmount(vehicle, p[1], wanted + p[2]);
            }
            break;
        case SwitchIds::MissionResult::AddGoodsToAnyStock:
        {
            std::uint32_t remaining = p[2];
            CHouseIterator houses;
            for (auto id = houses.GetFirstHouseId(p[0]); id != -1; id = houses.GetNextHouseId())
            {
                void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (House_GetHouseMainType(house) != 1 || !House_Stock_CanStoreGoodType(house, p[1], false)) continue;
                const auto maximum = House_Stock_GetMaximumAmount(house, p[1], false);
                const auto space = maximum - House_Stock_GetAmountOfGoods(house, p[1], false);
                if (NXBasics::Runtime_TimeSigned32(remaining) <= NXBasics::Runtime_TimeSigned32(space)) { House_Stock_ModifyAmount(house, p[1], NXBasics::Runtime_TimeSigned32(remaining), 0, false); return; }
                House_Stock_ModifyAmount(house, p[1], NXBasics::Runtime_TimeSigned32(space), 0, false);
                remaining -= space;
            }
            break;
        }
        case SwitchIds::MissionResult::ChangePlayerIdInArea:
        {
            CHumanIterator humans;
            CAnimalIterator animals;
            CVehicleIterator vehicles;
            CHouseIterator houses;
            CGuideIterator guides;
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[2]), static_cast<std::int16_t>(p[3]), false);
            for (auto id = vehicles.GetFirstVehicleId(p[0], &point, p[4]); id != -1; id = vehicles.GetNextVehicleId()) Vehicle_ChangePlayerId(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize, p[1]);
            for (auto id = houses.GetFirstHouseId(p[0], &point, p[4]); id != -1; id = houses.GetNextHouseId()) House_ChangePlayerId(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, p[1]);
            for (auto id = humans.GetFirstHumanId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[4])); id != -1; id = humans.GetNextHumanId()) Human_ChangePlayerId(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, NXBasics::Runtime_TimeSigned32(p[1]), true);
            for (auto id = animals.GetFirstAnimalId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[4])); id != -1; id = animals.GetNextAnimalId()) Animal_ChangePlayerId(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, p[1]);
            for (auto id = guides.GetFirstGuideId(p[0], &point, p[4]); id != -1; id = guides.GetNextGuideId())
            {
                Write(CGuideArray::mStaticVars + static_cast<std::uint32_t>(id) * CGuideArray::ElementSize, 4, p[1]);
                GuideManager_RecalculateGuideConnections(gGuideManager, id);
            }
            break;
        }
        case SwitchIds::MissionResult::SetDiplomacyNotChangeableFlag:
            Write<std::uint8_t>(gGameControl, p[0] * NC2Layout::RecordSize::GameControlPlayer + p[1] + 0x4958u, p[2] != 0);
            Write<std::uint8_t>(gGameControl, p[1] * NC2Layout::RecordSize::GameControlPlayer + p[0] + 0x4958u, p[2] != 0);
            break;
        case SwitchIds::MissionResult::RemoveFXWaveLandscapeInArea:
        case SwitchIds::MissionResult::RemoveFXSmokeLandscapeInArea:
        case SwitchIds::MissionResult::RemoveBlockerLandscapeInArea:
        case SwitchIds::MissionResult::RemoveFX1LandscapeInArea:
        case SwitchIds::MissionResult::RemoveFX2LandscapeInArea:
        {
            std::uint32_t point;
            Logic_EntryPoint_Edit_Start();
            // Entire native string region 00500D70..00500E4B. Duplicate fire-2 lookup in result 84 is intentional.
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            const char* waves[] = {"fx wave", "fx wave land"};
            const char* smoke[] = {"fx fire", "fx fire 2", "fx smoke", "fx fire small", "fx wave land", "fx fire incense", "fx fog", "fx fog waterfall", "fx fog waterfall00", "fx waterfall"};
            const char* blocker[] = {"block"};
            const char* fog[] = {"fx fog", "fx fog waterfall", "fx fog waterfall00", "fx waterfall"};
            const char* fire[] = {"fx fire", "fx fire 2", "fx fire house 0", "fx fire house 1", "fx fire 2", "fx fire small", "fx smoke", "fx fire house 2"};
            const char* const* names = waves;
            std::uint32_t count = 2;
            if (result.type == 79) { names = smoke; count = 10; }
            else if (result.type == 82) { names = blocker; count = 1; }
            else if (result.type == 83) { names = fog; count = 4; }
            else if (result.type == 84) { names = fire; count = 8; }
            std::int32_t engines[10];
            for (std::uint32_t index = 0; index < count; ++index)
            {
                engines[index] = NC2LogicExternals::External_Landscape_GetLandscapeIdByNamePtr(names[index]);
                // Windows exits without Edit_Stop when an engine definition is missing.
                if (engines[index] == -1) return;
            }
            auto clear = [&]()
            {
                for (std::uint32_t index = 0; index < count; ++index)
                    if (NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(&point) == engines[index]) { Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 0, 0, -1, 0, -1); break; }
            };
            clear();
            for (std::uint32_t ring = 1; ring <= p[2]; ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(point, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < ring; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(point, direction);
                        if (MapMigPoint_IsInsideMap(&point)) clear();
                    }
            }
            Logic_EntryPoint_Edit_Stop();
            break;
        }
        case SwitchIds::MissionResult::SetWallGateOpen:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            MapTool_WallGate_SetState(p[0], &point, p[3] != 0);
            break;
        }
        case SwitchIds::MissionResult::SetPlayerBehaviourFlag:
        case SwitchIds::MissionResult::SetHumanBehaviourFlag:
        {
            auto change = [&](std::int32_t id)
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto flags = Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags);
                Write(human, NC2Layout::HumanOffset::MiscFlags, p[2] != 0 ? flags | p[1] : flags & ~p[1]);
            };
            if (result.type == 49)
            {
                CHumanIterator humans;
                for (auto id = humans.GetFirstHumanId(p[0]); id != -1; id = humans.GetNextHumanId()) change(id);
            }
            else for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(p[0], id)) change(id);
            break;
        }
        case SwitchIds::MissionResult::RemoveHumansNearPos:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            auto mark = [&]()
            {
                CMapMoveableIterator moveables;
                for (auto id = moveables.GetFirstMapMoveableId(&point); id != -1; id = moveables.GetNextMapMoveableId())
                {
                    void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable;
                    if (Read<std::uint32_t>(moveable, 8) == 1) Write<std::uint8_t>(moveable, 0x0C, 1);
                }
            };
            mark();
            for (std::int32_t ring = 1; ring <= NXBasics::Runtime_TimeSigned32(p[2]); ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(point, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::int32_t step = 0; step < ring; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(point, direction);
                        if (MapMigPoint_IsInsideMap(&point)) mark();
                    }
            }
            break;
        }
        case SwitchIds::MissionResult::StopHumanByPlayerId:
        {
            alignas(4) std::byte walk[20]{}, work[20]{};
            Write<std::uint8_t>(walk, 0, 4); Write<std::uint8_t>(work, 0, 0x11);
            Write<std::uint8_t>(walk, 1, static_cast<std::uint8_t>(p[0])); Write<std::uint8_t>(work, 1, static_cast<std::uint8_t>(p[0]));
            CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(p[0]); id != -1; id = humans.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                std::uint32_t point = 0;
                MapMoveable_Attachment_GetNextMapPosition(gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, NC2Layout::HumanOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable, &point);
                if (Human_IsWalking(human))
                {
                    Write(walk, 4, Read<std::uint32_t>(human, 4)); Write(walk, 8, Read<std::uint32_t>(human, 8)); Write(walk, 12, point);
                    Human_DoAddNewUserCommand(human, walk);
                }
                if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::WorkCenter) != point)
                {
                    Write(work, 4, Read<std::uint32_t>(human, 4)); Write(work, 8, Read<std::uint32_t>(human, 8)); Write(work, 12, point);
                    Human_DoAddNewUserCommand(human, work);
                }
            }
            break;
        }
        case SwitchIds::MissionResult::SetAnimal:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            Logic_EntryPoint_Add_Animal(p[0], p[1], p[2], &point, p[5], p[6]);
            break;
        }
        case SwitchIds::MissionResult::RemoveAnimals:
            for (auto id = MissionCheck_GetFirstAnimalIdWithMissionCheckId(p[0]); id != -1; id = MissionCheck_GetNextAnimalIdWithMissionCheckId(p[0], id)) Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize, 0x0C));
            break;
        case SwitchIds::MissionResult::ChangeHumanObjectIdInArea:
        case SwitchIds::MissionResult::ChangeMissionIdOfHumanInRange:
        {
            const auto first = result.type == 56 ? 1u : 2u;
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[first]), static_cast<std::int16_t>(p[first + 1]), false);
            CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(p[0], &point, NXBasics::Runtime_TimeSigned32(p[first + 2])); id != -1; id = humans.GetNextHumanId()) Write(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, 0xB80, p[result.type == 56 ? 4 : 1]);
            break;
        }
        case SwitchIds::MissionResult::HealHumansInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(&point, NXBasics::Runtime_TimeSigned32(p[2])); id != -1; id = humans.GetNextHumanId())
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                Write(human, NC2Layout::HumanOffset::Hitpoints, Human_GetHumanAttributeMaximum_Hitpoints(human));
            }
            break;
        }
        case SwitchIds::MissionResult::ClearTribute: Write<std::uint8_t>(gTributeManager, p[0] * 0x3Cu, 0); break;
        case SwitchIds::MissionResult::SetGuiMarker:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            NC2LogicExternals::External_Gui_SetMarker(p[0], &point);
            break;
        }
        case SwitchIds::MissionResult::SetHumanName:
        {
            const auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(p[0]);
            if (id != -1) NC2LogicExternals::External_Human_SetName(id, p[1]);
            break;
        }
        case SwitchIds::MissionResult::SetWeather:
        {
            std::uint32_t first, second;
            MapMigPoint_Construct(&first, static_cast<std::int16_t>(p[0] - p[2]), static_cast<std::int16_t>(p[1] - p[2]), false);
            MapMigPoint_ValidatePosition(&first);
            MapMigPoint_Construct(&second, static_cast<std::int16_t>(p[0] + p[2]), static_cast<std::int16_t>(p[1] + p[2]), false);
            MapMigPoint_ValidatePosition(&second);
            NC2LogicExternals::External_FX_Weather_SetRectangle(first, second, p[3] != 0, NXBasics::Runtime_TimeSigned32(p[4]));
            break;
        }
        case SwitchIds::MissionResult::ChangeMissionIdOfPlayer:
        {
            CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(p[0]); id != -1; id = humans.GetNextHumanId()) Write(CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize, 0xB80, p[1]);
            break;
        }
        case SwitchIds::MissionResult::SetVertexColor:
        case SwitchIds::MissionResult::SetVertexColorOnLand:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            NC2LogicExternals::External_VertexColor_SetArea(&point, p[2], NXBasics::Runtime_TimeSigned32(p[3]), result.type == 97);
            break;
        }
        case SwitchIds::MissionResult::RemoveLandscapesInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            Logic_EntryPoint_Edit_Start();
            Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 0, 0, -1, 0, -1);
            for (std::int32_t ring = 1; ring < NXBasics::Runtime_TimeSigned32(p[2]); ++ring)
            {
                MapMigPoint_MoveCursorUnwrapped(point, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::int32_t step = 0; step < ring; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(point, direction);
                        if (MapMigPoint_IsInsideMap(&point)) Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 0, 0, -1, 0, -1);
                    }
            }
            Logic_EntryPoint_Edit_Stop();
            break;
        }
        case SwitchIds::MissionResult::MoveUnitsInArea:
        {
            std::uint32_t origin, target;
            MapMigPoint_Construct(&origin, static_cast<std::int16_t>(p[1]), static_cast<std::int16_t>(p[2]), false);
            MapMigPoint_Construct(&target, static_cast<std::int16_t>(p[4]), static_cast<std::int16_t>(p[5]), false);
            if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(static_cast<std::int16_t>(origin), static_cast<std::int16_t>(origin >> 16), static_cast<std::int16_t>(target), static_cast<std::int16_t>(target >> 16))) <= p[3]) break;
            bool moved = false;
            std::int32_t ids[20], count = 0;
            CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(p[0], &origin, NXBasics::Runtime_TimeSigned32(p[3])); id != -1; id = humans.GetNextHumanId())
            {
                const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, NC2Layout::HumanOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable;
                if (Read<std::uint8_t>(moveable, 0x2C) != 0 || Read<std::uint8_t>(human, 0xB51) != 0) continue;
                ids[count++] = id; moved = true;
                if (count == 20) break;
            }
            for (std::int32_t index = 0; index < count; ++index) moveHuman(ids[index], target);
            count = 0;
            CVehicleIterator vehicles;
            for (auto id = vehicles.GetFirstVehicleId(p[0], &origin, p[3]); id != -1; id = vehicles.GetNextVehicleId())
            {
                const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                if (Read<std::uint8_t>(gMapMoveableArrayStorage.entries + Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable, 0x2C) != 0) continue;
                ids[count++] = id; moved = true;
                if (count == 20) break;
            }
            for (std::int32_t index = 0; index < count; ++index)
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(ids[index]) * CVehicleArray::ElementSize;
                redraw(Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0), 4);
                MapMoveable_Attachment_AttachToMap(gMapMoveableArrayStorage.entries + Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable, &target);
                alignas(4) std::byte command[20]{};
                Write<std::uint8_t>(command, 0, 0x65); Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::PlayerId));
                Write(command, 4, Read<std::uint32_t>(vehicle, 4)); Write(command, 8, Read<std::uint32_t>(vehicle, 8)); Write(command, 12, target);
                Vehicle_DoAddNewUserCommand(vehicle, command);
                GameMap_Exploration_DoExplore(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), &target, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::ExplorationRadius));
                redraw(target, 4);
            }
            if (moved && NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_LoadAndPlay("data\\engine2d\\bin\\sounds\\misc\\teleporter.wav", 100, NXBasics::IntegerRange::UInt32Maximum, 1, false, false, 100);
            break;
        }
        case SwitchIds::MissionResult::InfoClear:
        case SwitchIds::MissionResult::InfoShowString:
        case SwitchIds::MissionResult::InfoCountGoodsInArea:
        case SwitchIds::MissionResult::InfoCountHumenInArea:
        case SwitchIds::MissionResult::InfoCountSoldiersInArea:
        case SwitchIds::MissionResult::InfoCountAnimalsInArea:
            if (gMissionOnscreenInfoManager != nullptr)
            {
                const auto type = result.type - 72;
                MissionOnscreenInfoManager_Info_Set(gMissionOnscreenInfoManager, p[0], p[1], type, type == 0 ? NXBasics::IntegerRange::UInt32Maximum : p[2], type >= 2 ? p[3] : 0, type >= 2 ? p[4] : 0, type >= 2 ? p[5] : 0, type >= 2 ? p[6] : 0, type == 2 || type == 6 ? p[7] : 0, 0, 0, 0);
            }
            break;
        case SwitchIds::MissionResult::ChangeAnimalPlayerIdInArea:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[2]), static_cast<std::int16_t>(p[3]), false);
            std::int32_t ids[50], count = 0;
            CAnimalIterator animals;
            for (auto id = animals.GetFirstAnimalId(p[0]); id != -1; id = animals.GetNextAnimalId())
            {
                const void* animal = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize;
                if (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType) != p[1]) continue;
                const auto position = Read<std::uint32_t>(Animal_GetMapPositionRef(animal), 0);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(static_cast<std::int16_t>(point), static_cast<std::int16_t>(point >> 16), static_cast<std::int16_t>(position), static_cast<std::int16_t>(position >> 16))) > p[4]) continue;
                ids[count++] = id;
                if (count == 50 || (p[5] != 0 && count >= NXBasics::Runtime_TimeSigned32(p[5]))) break;
            }
            for (std::int32_t index = 0; index < count; ++index) Animal_ChangePlayerId(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(ids[index]) * CAnimalArray::ElementSize, p[6]);
            break;
        }
        case SwitchIds::MissionResult::SetCameraPosition:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(result.GetParameter(0)), static_cast<std::int16_t>(result.GetParameter(1)), false);
            NC2LogicExternals::External_Camera_SetToPosition(&point);
            break;
        }
        case SwitchIds::MissionResult::SetHumanX:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[3]), static_cast<std::int16_t>(p[4]), false);
            for (std::int32_t index = 0; index < NXBasics::Runtime_TimeSigned32(p[7]); ++index) Logic_EntryPoint_Add_Human(p[0], p[1], p[2], &point, p[5], p[6]);
            break;
        }
        case SwitchIds::MissionResult::AttachHumanToVehicle:
        {
            const auto mission = result.GetParameter(0);
            const auto vehicleId = MissionCheck_GetFirstVehicleIdWithMissionCheckId(result.GetParameter(1));
            if (vehicleId == -1) break;
            const void* vehicle = GetVehiclePtr(vehicleId);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(mission); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(mission, id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                SUserCommandsForHumanHandler commands;
                Human_UserControl_GetCommands(human, &commands, false, false);
                if (commands.IsCommandAllowed(0x0C) && Vehicle_Passengers_CanHumanBeAttached(vehicle, id)) Human_DoAddNewUserCommand(human, SwitchIds::UserCommand::AttachVehicle, static_cast<std::uint8_t>(Object_GetField10(human, nullptr)), ArrayObject_GetId(human, nullptr), Object_GetField08(human, nullptr), ArrayObject_GetId(vehicle, nullptr), Object_GetField08(vehicle, nullptr));
            }
            break;
        }
        case SwitchIds::MissionResult::DetachHumanFromVehicle:
        {
            const auto mission = result.GetParameter(0);
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(mission); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(mission, id))
            {
                void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                SUserCommandsForHumanHandler commands;
                Human_UserControl_GetCommands(human, &commands, false, false);
                if (commands.IsCommandAllowed(0x0D)) Human_DoAddNewUserCommand(human, SwitchIds::UserCommand::DetachVehicle, static_cast<std::uint8_t>(Object_GetField10(human, nullptr)), ArrayObject_GetId(human, nullptr), Object_GetField08(human, nullptr), 0, 0);
            }
            break;
        }
        case SwitchIds::MissionResult::MoveHuman:
        {
            const auto mission = result.GetParameter(0);
            std::uint32_t target;
            MapMigPoint_Construct(&target, static_cast<std::int16_t>(result.GetParameter(1)), static_cast<std::int16_t>(result.GetParameter(2)), false);
            std::int32_t ids[20], count = 0;
            for (auto id = MissionCheck_GetFirstHumanIdWithMissionCheckId(mission); id != -1; id = MissionCheck_GetNextHumanIdWithMissionCheckId(mission, id)) { ids[count++] = id; if (count == 20) break; }
            for (std::int32_t index = 0; index < count; ++index) moveHuman(ids[index], target);
            break;
        }
        case SwitchIds::MissionResult::ChangeMissionIdOfVehiclesInRange:
        {
            std::uint32_t point;
            const auto player = result.GetParameter(0), mission = result.GetParameter(1);
            const auto y = result.GetParameter(3), x = result.GetParameter(2);
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), false);
            const auto radius = result.GetParameter(4);
            CVehicleIterator vehicles;
            for (auto id = vehicles.GetFirstVehicleId(player, &point, NXBasics::Runtime_TimeSigned32(radius)); id != -1; id = vehicles.GetNextVehicleId()) Vehicle_SetMissionCheckId(GetVehiclePtr(id), nullptr, mission);
            break;
        }
        case SwitchIds::MissionResult::RemoveVehiclesWithMissionId:
        {
            const auto mission = result.GetParameter(0);
            const bool passengers = result.GetParameter(1) != 0;
            std::int32_t ids[50], count = 0;
            for (auto id = MissionCheck_GetFirstVehicleIdWithMissionCheckId(mission); id != -1; id = MissionCheck_GetNextVehicleIdWithMissionCheckId(mission, id)) { ids[count++] = id; if (count == 50) break; }
            const bool deathStatistics = Game_GetSuppressDeathStatisticsFlag(), wreck = Game_GetSuppressVehicleWreckFlag();
            Game_SetSuppressDeathStatisticsFlag(true); Game_SetSuppressVehicleWreckFlag(true);
            for (std::int32_t index = 0; index < count; ++index)
            {
                void* vehicle = GetVehiclePtr(ids[index]);
                if (passengers)
                {
                    for (std::uint32_t slot = 0; slot < Vehicle_Passengers_GetNumberOfSlots(vehicle, nullptr); ++slot)
                    {
                        const void* reference = Vehicle_Passengers_GetAttachedHumanSlotPtr(vehicle, nullptr, slot);
                        if (!HumanReference_IsAllocated(reference)) continue;
                        const void* human = GetHumanPtr(HumanReference_GetHumanId(reference, nullptr));
                        Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(human, NC2Layout::HumanOffset::MapMoveableId));
                    }
                    if (Vehicle_Passengers_IsCommanderAttached(vehicle, nullptr)) Logic_EntryPoint_FreeMapMoveable(Read<std::uint32_t>(GetHumanPtr(Vehicle_Passengers_GetAttachedCommanderId(vehicle)), 0x0C));
                }
                redraw(Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0), 2);
                Logic_EntryPoint_FreeMapMoveable(Vehicle_GetMapMoveableId(vehicle, nullptr));
            }
            Game_SetSuppressDeathStatisticsFlag(deathStatistics); Game_SetSuppressVehicleWreckFlag(wreck);
            break;
        }
        case SwitchIds::MissionResult::SetImportLandscapeMarker:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(p[0]), static_cast<std::int16_t>(p[1]), false);
            if (!MapMigPoint_IsInsideMap(&point)) break;
            if (p[2] != 0) MarkerArray_AllocateMarker(MarkerArray_GetExistingObjectPtr(), nullptr, 2, &point, NXBasics::IntegerRange::UInt32Maximum);
            else removeMarkers(point, 2);
            break;
        }
        case SwitchIds::MissionResult::ChangeMissionIdOfPlayersVehiclesOnContinent:
        {
            std::uint32_t point;
            const auto player = result.GetParameter(0), mission = result.GetParameter(3);
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(result.GetParameter(1)), static_cast<std::int16_t>(result.GetParameter(2)), false);
            if (!MapMigPoint_IsInsideMap(&point)) break;
            const auto continent = MapMicroPoint_GetContinent(GameMap_GetConstMapMicroPointPtr(&point), nullptr);
            if (continent == NXBasics::IntegerRange::UInt8MaximumSigned) break;
            CVehicleIterator vehicles;
            for (auto id = vehicles.GetFirstVehicleId(player); id != -1; id = vehicles.GetNextVehicleId())
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                if (Vehicle_Attachment_IsInsideVehicle(vehicle)) continue;
                auto position = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
                if (Vehicle_IsDocked(vehicle, nullptr)) position = Read<std::uint32_t>(Vehicle_GetConstMapMicroPointPtr(vehicle), 0);
                if (MapMigPoint_IsInsideMap(&position) && MapMicroPoint_GetContinent(GameMap_GetConstMapMicroPointPtr(&position), nullptr) == continent) Vehicle_SetMissionCheckId(vehicle, nullptr, mission);
            }
            break;
        }
        case SwitchIds::MissionResult::ChangeMissionIdOfVehicles:
        {
            const auto oldMission = result.GetParameter(0), newMission = result.GetParameter(1);
            CVehicleIterator vehicles;
            for (auto id = vehicles.GetFirstVehicleId(); id != -1; id = vehicles.GetNextVehicleId())
            {
                void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                if (Vehicle_GetMissionCheckId(vehicle, nullptr) == oldMission) Vehicle_SetMissionCheckId(vehicle, nullptr, newMission);
            }
            break;
        }
        case SwitchIds::MissionResult::SetMapAreaMarker:
        case SwitchIds::MissionResult::SetMapAreaMarkerMagic:
        {
            std::uint32_t point;
            MapMigPoint_Construct(&point, static_cast<std::int16_t>(result.GetParameter(0)), static_cast<std::int16_t>(result.GetParameter(1)), false);
            auto radius = result.GetParameter(2), spacing = result.GetParameter(4);
            const bool enabled = result.GetParameter(3) != 0;
            if (radius == 0) radius = 1;
            if (spacing == 0) spacing = 1;
            if (!MapMigPoint_IsInsideMap(&point)) break;
            MapMigPoint_MoveCursorUnwrapped(&point, 4, radius);
            const auto type = result.type == 101 ? 3u : 4u;
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < radius; ++step)
                {
                    if (step % spacing == 0)
                    {
                        if (enabled) MarkerArray_AllocateMarker(MarkerArray_GetExistingObjectPtr(), nullptr, type, &point, NXBasics::IntegerRange::UInt32Maximum);
                        else removeMarkers(point, type);
                    }
                    MapMigPoint_MoveCursorUnwrapped(point, direction);
                }
            break;
        }
        default: break;
        }
    }

}
