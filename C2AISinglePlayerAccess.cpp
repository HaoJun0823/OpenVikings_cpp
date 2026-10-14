#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2AISinglePlayerAccess.h"
#include "C2AITaskAccess.h"
#include "C2AISoldierAccess.h"
#include "C2AIMovementMessages.h"
#include "C2AIConditions.h"
#include "C2AIHumanRequests.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanMessageAccess.h"
#include "C2HumanFamily.h"
#include "C2HumanIterator.h"
#include "C2MissionStartObjects.h"
#include "C2OverviewObjectIterators.h"
#include "C2PlayerCenterPosition.h"
#include "C2PlayerStatisticAccess.h"
#include "C2StaticHouseTypeData.h"
#include "C2HouseBuildPosition.h"
#include "C2EngineDisplayAccess.h"
#include <initializer_list>
#include "C2MilitaryTargetSearch.h"
#include "C2MapGeometry.h"
#include "C2MapWalkSectorManager.h"
#include "C2MapPathfinderSearch.h"
#include "C2WallRoadConstruction.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        bool HasHumanNearby(std::uint32_t player, const void* point)
        {
            CHumanIterator iterator;
            return iterator.GetFirstSectorHumanId(player, Read<std::uint32_t>(point, 0), 40, false) != -1;
        }
        bool IsDamagedOwnedLandscape(const void* micro, std::uint32_t player)
        {
            return Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0 && Read<std::int8_t>(micro, 4) == static_cast<std::int32_t>(player) && Read<std::uint8_t>(micro, 3) < 100;
        }
    }
    // Original function: FUN_0045b612.
    void AISinglePlayerHandler_l_Misc_CheckAndAddDefaultTasks(void* handler)
    {
        std::uint32_t center = 0;
        Tool_Misc_GetPlayerCenterPosition(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), &center);
        if (Read<std::uint8_t>(handler, NC2Layout::AISinglePlayerOffset::DefaultSoldierPositionEnabled) == 0) AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(handler, &center, 15);
        for (std::uint32_t i = 0; i < 50; ++i) if (Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::FirstMainTaskDescription + i * 68) != 0) return;
        std::byte task[68]{};
        AIMainTask_Init(task, 1, 10, 100000);
        Write(task, 12, center); Write<std::uint32_t>(task, 16, 40);
        AISinglePlayerHandler_MainTask_Add(handler, task);
    }
    // Original function: FUN_0045b6ab.
    void AISinglePlayerHandler_l_Women_WorkOn(void* handler)
    {
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), false); id != -1; id = iterator.GetNextSectorHumanId())
        {
            auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (!Human_IsJobWoman(human) || Human_IsAboutToMarry(human)) continue;
            if (!Human_IsMarried(human)) { Human_DoAddNewUserCommand(human, 14, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId), 0, 0, 0, 0); continue; }
            if (!Human_IsAttachedToHomeHouse(human)) continue;
            auto* home = CHouseArray::mStaticVars + Read<std::uint32_t>(human, NC2Layout::HumanOffset::HomeHouseId) * CHouseArray::ElementSize;
            const auto* stats = PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId));
            if (stats == nullptr) continue;
            const auto workers = static_cast<std::uint32_t>(Read<std::uint16_t>(stats, 2)) - Read<std::uint16_t>(stats, 18);
            if (workers >= Read<std::uint32_t>(handler, 0xAD38)) continue;
            const auto maximum = Read<std::uint32_t>(handler, 0xAD3C);
            if (maximum != 0 && Read<std::uint16_t>(stats, 0) >= maximum) continue;
            if (!Human_IsAbleToGetPregnant(human) || House_Stock_RequestAmountOfGoods(home, 16, false) < 3) continue;
            if (static_cast<std::int32_t>(workers) / 3 > Read<std::uint16_t>(stats, 8))
            {
                Human_DoAddNewUserCommand(human, 16, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId), 0, 0, 0, 0);
                return;
            }
            Human_DoAddNewUserCommand(human, 15, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId), 0, 0, 0, 0);
        }
    }
    // Original function: FUN_0045b7ea.
    void AISinglePlayerHandler_l_House_FillStocks(void* handler)
    {
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId)); id != -1; id = iterator.GetNextHouseId())
        {
            auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, NC2Layout::HouseOffset::DemolitionFlag) != 0 || Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) != 3 || House_GetHouseMainType(house) != 1) continue;
            const auto* definition = House_GetHouseTypeDataPtr(house);
            for (std::uint32_t i = 0; i < static_cast<std::uint32_t>(definition->stockCount); ++i)
            {
                const auto good = Read<std::uint32_t>(definition, 0x64 + i * 12);
                const auto delta = 5u - House_Stock_RequestAmountOfGoods(house, good, false);
                House_Stock_ModifyFutureAmount(house, good, static_cast<std::int32_t>(delta), true);
            }
        }
    }
    // Original function: FUN_0045b88c.
    void AISinglePlayerHandler_l_BuildWallGates_SaveAllIntoArray(void* handler)
    {
        std::uint32_t point = 0;
        Tool_Misc_GetPlayerCenterPosition(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), &point);
        const auto maximum = GameMap_GetWidth() > GameMap_GetHeight() ? GameMap_GetWidth() : GameMap_GetHeight();
        const auto save = [&](bool checkCapacity)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(&point);
            if (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) != 1) return;
            const auto landscape = Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData);
            if (landscape == 82 && Read<std::int8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapePlayerId) == static_cast<std::int32_t>(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId)))
            {
                const auto count = Read<std::uint32_t>(handler, 0x43A8);
                if (checkCapacity && count == 1000) return;
                Write(handler, 0x43AE + count * 6, point);
                if (checkCapacity) Write<std::uint8_t>(handler, 0x43AC + count * 6, 0);
                Write(handler, 0x43A8, count + 1);
            }
            else if (landscape == 83 || landscape == 84)
            {
                const auto count = Read<std::uint32_t>(handler, 0x5B20);
                if (checkCapacity && count == 50) return;
                Write(handler, 0x5B26 + count * 8, point);
                Write<std::uint8_t>(handler, 0x5B2A + count * 8, landscape == 84);
                Write(handler, 0x5B20, count + 1);
            }
        };
        save(false);
        for (std::uint32_t radius = 1; radius < static_cast<std::uint32_t>(maximum); ++radius)
        {
            MapMigPoint_MoveCursorUnwrapped(&point, 4, 1);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t i = 0; i < radius; ++i)
                {
                    MapMigPoint_MoveCursorUnwrapped(&point, direction, 1);
                    if (MapMigPoint_IsInsideMap(&point)) save(true);
                }
        }
    }
    // Original function: FUN_0045ba65.
    void AISinglePlayerHandler_l_BuildWall_CheckForBuildOrRepairWall(void* handler)
    {
        Write<std::uint8_t>(handler, 0x43A4, 0);
        if (Read<std::uint32_t>(handler, 0x43A8) == 0) return;
        const auto player = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId);
        const auto* stats = PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, player);
        if (stats == nullptr || Read<std::uint16_t>(stats, 0) == 0) return;
        std::uint32_t count = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x43A8); ++i)
        {
            auto* point = static_cast<std::byte*>(handler) + 0x43AE + i * 6;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const bool build = (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::WalkingFlags) & NXBasics::BitMask::Bit7Signed) == 0;
            if (!build && !IsDamagedOwnedLandscape(micro, player)) continue;
            if (AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(handler, point) || !HasHumanNearby(player, point)) continue;
            Write<std::uint8_t>(handler, 0x43A4, 1);
            if (build) WallBuildManager_SetWallBuildFlag(gWallBuildManager, nullptr, player, point);
            Write<std::uint8_t>(handler, 0x43AC + i * 6, 1);
            if (++count == 10) return;
        }
    }
    // Original function: FUN_0045bb8c.
    void AISinglePlayerHandler_l_BuildGate_CheckForDamagedGate(void* handler)
    {
        Write<std::uint8_t>(handler, 0x5B1C, 0);
        if (Read<std::uint32_t>(handler, 0x5B20) == 0) return;
        const auto player = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId);
        const auto* stats = PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, player);
        if (stats == nullptr || Read<std::uint16_t>(stats, 0) == 0) return;
        std::uint32_t count = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x5B20); ++i)
        {
            auto* point = static_cast<std::byte*>(handler) + 0x5B26 + i * 8;
            if (!IsDamagedOwnedLandscape(GameMap_GetConstMapMicroPointPtr(point), player)) continue;
            if (AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(handler, point) || !HasHumanNearby(player, point)) continue;
            Write<std::uint8_t>(handler, 0x5B1C, 1); Write<std::uint8_t>(handler, 0x5B24 + i * 8, 1);
            if (++count == 10) return;
        }
    }
    // Original function: FUN_0045bc60.
    void AISinglePlayerHandler_l_CheckDefenceMode(void* handler)
    {
        const auto time = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::LastAttackedTick);
        if (time == 0 || time + 360 >= gGameTime.tick) return;
        std::uint32_t origin = 0;
        CHouseIterator iterator;
        for (const bool secondList : {true, false})
            for (auto id = iterator.GetFirstSectorHouseId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), &origin, 10000, secondList); id != -1; id = iterator.GetNextSectorHouseId()) House_SetDefenceModeFlag(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, false);
        Write<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::LastAttackedTick, 0);
    }
    // Original function: FUN_0045bd18.
    bool AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(void* handler, const void* point)
    {
        if (Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), point, 0, 40, true, 1, 0, true, false) != -1) return true;
        return Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId), point, 0, 40, true, 1, 0, false, false) != -1;
    }
    // Original function: FUN_0045bd63.
    void AISinglePlayerHandler_l_OpenCloseGates(void* handler)
    {
        const auto player = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x5B20); ++i)
        {
            auto* point = static_cast<std::byte*>(handler) + 0x5B26 + i * 8;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectType) != 1 || Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ObjectData) == 0 || Read<std::int8_t>(micro,NC2Layout::MapMicroPointOffset::LandscapePlayerId) != static_cast<std::int32_t>(player)) continue;
            const bool enemy = AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(handler, point);
            const auto flag = Read<std::uint8_t>(handler, 0x5B2A + i * 8);
            if ((enemy && flag == 1) || (!enemy && flag == 0))
            {
                MapTool_WallGate_SetState(player, point, !enemy);
                Write<std::uint8_t>(handler, 0x5B2A + i * 8, !enemy);
            }
        }
    }
    // Original function: FUN_00473070.
    void AISinglePlayerHandler_l_Messages_TryToRepairAllMainTasks(void* handler)
    {
        for (std::uint32_t i = 0; i <= Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::LastMainTaskIndex); ++i)
        {
            auto* task = static_cast<std::byte*>(handler) + NC2Layout::AISinglePlayerOffset::MainTasks + i * 68;
            if (Read<std::uint32_t>(task, 8) == 0 || Read<std::uint8_t>(task, 52) == 0) continue;
            std::uint32_t temporary = 0;
            if (!CMapWalkSectorManager::GetNextSectorBasePoint(task + 60, task + 64, 0, true, &temporary, &temporary, 40, 20)) continue;
            Write<std::uint8_t>(task, 52, 0);
            const auto type = Read<std::uint32_t>(task, 56);
            if (type == 1 || type == 3) Write(task, 12, Read<std::uint32_t>(task, 64));
            else if (type == 2) Write(task, 28, Read<std::uint32_t>(task, 64));
        }
    }
    // Original function: FUN_0044e49a.
    void AISinglePlayerHandler_l_BuildHouses_CheckForBuildHouse(void* handler)
    {
        if (Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::BuildHouseCount) == 0) return;
        const auto player = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId);
        const auto* stats = PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, player);
        if (stats == nullptr || Read<std::uint16_t>(stats, 0) == 0) return;
        CHouseIterator iterator;
        std::uint32_t building = 0;
        for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId()) if (Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, NC2Layout::HouseOffset::ConstructionState) != 3) ++building;
        if (building >= 3) return;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::BuildHouseCount); ++i)
        {
            const auto* entry = static_cast<const std::byte*>(handler) + NC2Layout::AISinglePlayerOffset::BuildHouseTypes + i * 16;
            const auto engine = Read<std::uint32_t>(entry, 0), stage = Read<std::uint32_t>(entry, 8);
            const auto* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engine);
            if (Read<std::uint32_t>(definition, 0x48 + stage * 64) == 0) continue;
            if (iterator.GetFirstHouseId(player, entry + 4, 12, engine) != -1) continue;
            std::uint32_t data[4] = {player, engine, stage, 0};
            if (!CMapPathfinderElement::SearchOverMap(entry + 4, 12, false, Tool_HouseBuildPointSearchEvaluationFunction, data, false, player)) continue;
            if (AISinglePlayerHandler_l_IsEnemySodlierOrVehicleNearPosition(handler, data + 3) || !HasHumanNearby(player, data + 3)) continue;
            Logic_EntryPoint_House_Set(player, engine, stage, data + 3, false, Read<std::uint32_t>(entry, 12));
            if (++building >= 3) return;
        }
    }
    // Original function: FUN_0045b4f4; the native cadence and call order are preserved.
    void AISinglePlayerHandler_WorkOnAI(void* handler, std::uint32_t tick)
    {
        if (Read<std::uint8_t>(handler, 4) == 0) return;
        const auto initialize = Read<std::uint8_t>(handler, 5);
        if (initialize != 0 || Read<std::uint8_t>(handler, NC2Layout::AISinglePlayerOffset::MainTasksChanged) != 0)
        {
            if (initialize != 0)
            {
                AISinglePlayerHandler_l_Misc_CheckAndAddDefaultTasks(handler);
                AISinglePlayerHandler_l_BuildHouses_SaveAllHousesIntoArray(handler);
                AISinglePlayerHandler_l_BuildWallGates_SaveAllIntoArray(handler);
            }
            AISinglePlayerHandler_l_SingleSoldiers_RecreateListCompletly(handler);
            AISinglePlayerHandler_l_Vehicles_RecreateListCompletly(handler);
            AISinglePlayerHandler_MainTask_RecheckAll(handler);
            Write<std::uint8_t>(handler, 5, 0); Write<std::uint8_t>(handler, NC2Layout::AISinglePlayerOffset::MainTasksChanged, 0);
        }
        AISinglePlayerHandler_l_Soldiers_AttachToTowers(handler);
        AISinglePlayerHandler_l_SingleSoldiers_UpdateList(handler);
        AISinglePlayerHandler_l_Vehicles_UpdateList(handler);
        AISinglePlayerHandler_l_Vehicles_CheckForCommander(handler);
        if (AISinglePlayerHandler_Condition_RecheckAll(handler, tick)) AISinglePlayerHandler_MainTask_RecheckAll(handler);
        AISinglePlayerHandler_l_SingleSoldiers_ClearInvalidTask(handler);
        if (tick % 2 == 0) { AISinglePlayerHandler_l_CheckDefenceMode(handler); AISinglePlayerHandler_l_SingleSoldiers_UpdateAttachedTasks(handler); }
        AISinglePlayerHandler_l_SingleSoldiers_WorkOnAttachedTasks(handler);
        AISinglePlayerHandler_l_Vehicles_WorkOnAttachedTasks(handler);
        if (tick % 12 == 0) { AISinglePlayerHandler_l_SingleSoldiers_RefreshAllEnergyAndCondition(handler); AISinglePlayerHandler_l_Messages_TryToRepairAllMainTasks(handler); }
        if (tick % 3 == 0) AISinglePlayerHandler_l_BuildHouses_CheckForBuildHouse(handler);
        if (tick % 6 == 0)
        {
            AISinglePlayerHandler_l_BuildWall_CheckForBuildOrRepairWall(handler);
            AISinglePlayerHandler_l_BuildGate_CheckForDamagedGate(handler);
            AISinglePlayerHandler_l_House_FillStocks(handler);
            AISinglePlayerHandler_l_Misc_Human_GetInhabitants(handler);
        }
        if (tick % 12 == 0) AISinglePlayerHandler_l_Women_WorkOn(handler);
        AISinglePlayerHandler_l_OpenCloseGates(handler);
    }
}
