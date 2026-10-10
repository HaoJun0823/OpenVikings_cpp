#include "C2HumanInitialization.h"
#include "C2DefaultInteraction.h"
#include "C2HouseAccess.h"
#include "C2HumanJobAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanTaskAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2GameEngineStartup.h"
#include <initializer_list>
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2MissionStartObjects.h"
#include "C2StaticTribeTypeData.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2VehicleAccess.h"
#include "C2CallbackManager.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineLandscapeAccess.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_00438df2.
    std::int32_t Marker_PlayerId_Get(const void* marker)
    {
        const auto humanId = NC2Lifetime::Read<std::int32_t>(marker, 0x14);
        return humanId == -1 ? -1 : NC2Lifetime::Read<std::int32_t>(GetHumanPtr(humanId), 0x10);
    }

    using namespace NC2Lifetime;

    // Original function: FUN_00438c79; the original constructor name remains unresolved.
    static void FUN_00438C79(void* data)
    {
        std::memset(static_cast<std::byte*>(data) + 1, 0, 66);
        Write<std::uint8_t>(data, 1, 1);
        Write<std::uint8_t>(data, 0, 0);
    }

    // Original function: FUN_0044b5e1; the original constructor name remains unresolved.
    static void FUN_0044B5E1(void* data)
    {
        std::memset(static_cast<std::byte*>(data) + 1, 0, 7);
        Write<std::uint8_t>(data, 1, 1);
        Write<std::uint8_t>(data, 0, 0);
    }

    // Original function: FUN_0044b590; native name unresolved, reset ranges confirmed from Windows.
    static void FUN_0044B590(void* human, std::int32_t player)
    {
        Write(human, 0x10, player);
        std::memset(static_cast<std::byte*>(human) + 0x15C, 0, 0x38);
        FUN_00438C79(static_cast<std::byte*>(human) + 0x194);
        FUN_0044B5E1(static_cast<std::byte*>(human) + 0x1D7);
        std::memset(static_cast<std::byte*>(human) + 0x1E0, 0, 0x38);
        Human_SetupPlayerSpecificStuff(human);
    }

    // Original function: FUN_0044b45d; the native player upper bound is signed without a lower guard.
    void Human_ChangePlayerId(void* human, std::int32_t player, bool detach)
    {
        if (player >= 20 || Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u) == 0) return;
        if (Human_IsJobSoldier(human) || Human_IsJobHero(human))
        {
            Write<std::uint32_t>(human, 0xB64, 0);
            Write<std::uint32_t>(human, 0xB70, 0);
            Write<std::uint8_t>(human, 0xB60, 1);
            Write<std::uint32_t>(human, 0xB5C, 2);
        }
        std::uint32_t point = 0;
        bool wasAttached = false;
        void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            point = Read<std::uint32_t>(moveable, 0x12);
            MapMoveable_Attachment_DetachFromMap(moveable);
            wasAttached = true;
        }
        FUN_0044B590(human, player);
        void* pathfinder = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        Write<std::int32_t>(pathfinder, 0x24, -1);
        Write<std::int32_t>(pathfinder, 0x1C, -1);
        Write(pathfinder, 0x2C, player);
        if (wasAttached)
        {
            MapMoveable_Attachment_AttachToMap(moveable, &point);
            const auto radius = Read<std::uint32_t>(human, 0xB7C);
            const auto owner = Read<std::uint32_t>(human, 0x10);
            GameMap_Exploration_DoExplore(owner, Human_GetMapPositionRef(human), radius);
        }
        if (detach) Human_Detach(human, false);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 2, static_cast<std::uint32_t>(player));
        Human_SetupCurrentJobType(human);
        Human_UpdateCurrentJobType(human);
        if (player == 0 && Read<std::uint32_t>(gGameControl, 0x5D200) == 2) Write(human, 0xB84, Read<std::uint32_t>(human, 0xB84) | 1u);
    }

    // Original function: FUN_00436aad; Windows outlined work-marker eligibility predicate.
    bool Human_WorkCenterMarker_IsAllowed(const void* human)
    {
        if (!Human_IsSexMale(human) || !Human_IsAgeAdult(human)) return false;
        const auto job = Read<std::uint32_t>(human, 0x18);
        return (Human_IsJobCarrier(human) && Human_IsAttachedToWorkHouse(human)) || job == 8 || Human_IsJobHunter(human) || job == 18 || Human_IsJobFisher(human) || job == 28 || Human_IsJobBuilder(human);
    }

    namespace
    {
        std::byte* Human(std::uint32_t id) { return CHumanArray::mStaticVars + id * CHumanArray::ElementSize; }
        std::byte* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        std::byte* Moveable(std::uint32_t id) { return gMapMoveableArrayStorage.entries + id * 0x58u; }

        // Original function: FUN_00438d39.
        bool Marker_Init(void* marker, std::int32_t id, std::uint32_t type, const void* point, std::uint32_t human)
        {
            if (Read<std::uint8_t>(marker, 0) != 0) return false;
            std::memset(marker, 0, 0x18);
            Write<std::uint8_t>(marker, 0, 1);
            Write(marker, 4, id);
            Write(marker, 8, Game_UniqueId_Generate());
            Write(marker, 0x10, type);
            Write(marker, 0x14, human);
            const auto moveable = MapMoveableArray_AllocateMapMoveable(gMapMoveableArrayStorage.manager, 4, id);
            Write(marker, 0x0C, moveable);
            if (moveable == -1) return false;
            MapMoveable_Attachment_AttachToMap(Moveable(moveable), point);
            return true;
        }
        // Original function: FUN_00438da4.
        void Marker_Exit(void* marker)
        {
            if (Read<std::uint8_t>(marker, 0) == 0) return;
            const auto id = Read<std::int32_t>(marker, 0x0C);
            if (id != -1) MapMoveableArray_FreeMapMoveable(gMapMoveableArrayStorage.manager, id);
            Write<std::uint8_t>(marker, 0, 0);
        }
        // Original function: FUN_0040badc.
        void MarkerArray_FreeMarker(std::int32_t id)
        {
            void* marker = static_cast<std::byte*>(gMarkerEntries) + id * 0x18;
            if (Read<std::uint8_t>(marker, 0) != 0 && static_cast<CArrayHandler*>(gMarkerArray)->Element_Free(id, true)) Marker_Exit(marker);
        }
        // Original function: FUN_0040ba7d; invalid points retain the original slot reservation.
        std::int32_t MarkerArray_AllocateMarker(std::uint32_t type, const void* point, std::uint32_t human)
        {
            const auto id = static_cast<CArrayHandler*>(gMarkerArray)->Element_Allocate();
            if (id == -1 || !MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point)) return -1;
            if (!Marker_Init(static_cast<std::byte*>(gMarkerEntries) + id * 0x18, id, type, point, human)) { MarkerArray_FreeMarker(id); return -1; }
            return id;
        }
        // Original function: FUN_00438dc5.
        void Marker_MapPosition_Set(void* marker, const void* point)
        {
            void* moveable = Moveable(Read<std::uint32_t>(marker, 0x0C));
            MapMoveable_Attachment_DetachFromMap(moveable);
            MapMoveable_Attachment_AttachToMap(moveable, point);
        }
    }

    // Original function: FUN_0040b09b.
    void Logic_EntryPoint_Edit_Start() { GameMapModifyManager_StartGameMapModify(); }
    // Original function: FUN_0040b0a0.
    void Logic_EntryPoint_Edit_Stop() { GameMapModifyManager_StopGameMapModify(); }

    // Original function: FUN_0040ad73.
    std::int32_t Logic_EntryPoint_Add_Human(std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags)
    {
        return CHumanArray::sTheObjectPtr->AllocateHuman(player, tribe, job, point, missionCheck, flags);
    }
    // Original function: FUN_0040b502.
    std::int32_t CHumanArray::AllocateHuman(std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags)
    {
        const auto id = Element_Allocate();
        if (id == -1 || !MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point)) return -1;
        if (!Human_Init(Human(id), id, player, tribe, job, point, missionCheck, flags)) { FreeHuman(id); return -1; }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(4, true, id, 1, 0);
        return id;
    }
    // Original function: FUN_0040b5aa.
    void CHumanArray::FreeHuman(std::int32_t id)
    {
        void* human = Human(id);
        if (Read<std::uint8_t>(human, 0) == 0 || !Element_Free(id, true)) return;
        if (Read<std::int32_t>(human, 0x0C) != -1) CCallbackManager::sTheObjectPtr->ExecuteCallback(4, false, id, 0, 0);
        Human_Exit(human);
    }
    // Original function: FUN_0041c5f1.
    static void MerchantData_Init(void* merchant)
    {
        Write<std::uint8_t>(merchant, 0, 1);
        Write<std::uint32_t>(merchant, 0x14, 0);
        Write<std::int32_t>(merchant, 0x18, -1);
        Write<std::int32_t>(merchant, 0x1C, -1);
        Write<std::int32_t>(merchant, 0x70, -1);
        Write<std::int32_t>(merchant, 4, -1);
        Write<std::uint32_t>(merchant, 8, 0);
        Write<std::uint32_t>(merchant, 0x0C, 0);
        Write<std::uint32_t>(merchant, 0x10, 0);
    }
    // Original function: FUN_0041c586; the human argument is unused by Windows.
    std::int32_t HumanMerchantArray_AllocateMerchant(void* manager, std::uint32_t /*human*/)
    {
        const auto id = static_cast<CArrayHandler*>(manager)->Element_Allocate();
        if (id != -1) MerchantData_Init(Read<std::byte*>(manager, 0x30) + id * 0xC4);
        return id;
    }
    // Original function: FUN_0041c5af.
    void HumanMerchantArray_FreeMerchant(void* manager, std::int32_t id)
    {
        void* merchant = Read<std::byte*>(manager, 0x30) + id * 0xC4;
        if (Read<std::uint8_t>(merchant, 0) != 0 && static_cast<CArrayHandler*>(manager)->Element_Free(id, true)) Write<std::uint8_t>(merchant, 0, 0);
    }
    // Original function: FUN_0042c817.
    bool Human_IsAttachedToLearnHouse(const void* human)
    {
        const auto id = Read<std::int32_t>(human, 0xB34);
        if (id == -1) return false;
        const void* house = House(id);
        if (Read<std::uint32_t>(gStaticHouseTypeData, Read<std::uint32_t>(house, 0x14) * 0x624u) != 4) return false;
        return House_Inhabitants_IsHumanAttachedAsStudent(house, Read<std::uint32_t>(human, 4));
    }
    // Original function: FUN_0042c6aa.
    bool Human_IsAttachedToRoadPoint(const void* human)
    {
        if (!Human_IsJobBuilder(human)) return false;
        if (Read<std::uint32_t>(human, 0xAB8) == 41) return true;
        const void* point = static_cast<const std::byte*>(human) + 0xB30;
        return !Human_IsAttachedToWorkHouse(human) && MapMigPoint_IsInsideMap(point) && RoadBuildManager_IsRoadBuildFlagSetForPlayer(gRoadBuildManager, Read<std::int32_t>(human, 0x10), point);
    }
    // Original function: FUN_0042c6f2.
    bool Human_IsAttachedToWallPoint(const void* human)
    {
        if (!Human_IsJobBuilder(human)) return false;
        if (Read<std::uint32_t>(human, 0xAB8) == 42) return true;
        const void* point = static_cast<const std::byte*>(human) + 0xB30;
        if (Human_IsAttachedToWorkHouse(human) || !MapMigPoint_IsInsideMap(point)) return false;
        if (RoadBuildManager_IsRoadBuildFlagSetForPlayer(gWallBuildManager, Read<std::int32_t>(human, 0x10), point)) return true;
        const void* micro = GameMap_GetConstMapMicroPointPtr(point);
        if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0 || Read<std::int8_t>(micro, 4) != Read<std::int32_t>(human, 0x10)) return false;
        std::int32_t engineId;
        switch (Read<std::uint8_t>(micro, 2))
        {
        case 82: engineId = NC2LogicExternals::External_Misc_GetEngineWallElementLandscapeId(); break;
        case 83: engineId = NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(0, true); break;
        case 84: engineId = NC2LogicExternals::External_Misc_GetEngineWallGateLandscapeId(0, false); break;
        default: return false;
        }
        return Read<std::uint8_t>(micro, 3) < Read<std::uint32_t>(NC2LogicExternals::External_Landscape_GetLogicDataPtr(engineId), 4);
    }
    // Original function: FUN_0042c7b3; active build animations keep their attachment.
    bool Human_IsAttachedToBuildSite(const void* human)
    {
        const auto animation = Read<std::uint32_t>(human, 0xAB8);
        if (animation == 41 || animation == 42 || Read<std::int32_t>(human, 0xB34) != -1) return true;
        const void* point = static_cast<const std::byte*>(human) + 0xB30;
        if (!MapMigPoint_IsInsideMap(point)) return false;
        const auto player = Read<std::int32_t>(human, 0x10);
        return RoadBuildManager_IsRoadBuildFlagSetForPlayer(gRoadBuildManager, player, point) || RoadBuildManager_IsRoadBuildFlagSetForPlayer(gWallBuildManager, player, point);
    }
    // Original function: FUN_00427a6d; shared CRoad/CWallBuildManager::DetachBuilder body.
    void BuildManager_DetachBuilder(void* manager, std::uint32_t player, const void* point)
    {
        void* data = Read<void**>(manager, 4)[player];
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0); ++i)
        {
            if (Read<std::uint32_t>(data, 8u + i * 6u) != Read<std::uint32_t>(point, 0)) continue;
            if (Read<std::uint8_t>(data, 12u + i * 6u) != 0)
            {
                Write<std::uint8_t>(data, 12u + i * 6u, 0);
                Write<std::uint32_t>(data, 4, Read<std::uint32_t>(data, 4) - 1u);
            }
            break;
        }
    }

    // Original function: FUN_00453493.
    bool Human_WorkCenterMarker_IsVisible(const void* human)
    {
        if (!Human_WorkCenterMarker_IsAllowed(human)) return false;
        if (Human_IsAttachedToWorkHouse(human))
        {
            if (Read<std::uint32_t>(House(Read<std::uint32_t>(human, 0xB34)), 0x24) == Read<std::uint32_t>(human, 0xB30)) return false;
        }
        else
        {
            if (Read<std::uint8_t>(gStaticJobTypeData, Read<std::uint32_t>(human, 0x18) * 0x88u + 0x83u) != 0) return false;
            if (Human_IsJobBuilder(human) && !Human_IsAttachedToRoadPoint(human) && !Human_IsAttachedToWallPoint(human)) return false;
        }
        return Read<std::uint8_t>(human, 0xB51) == 0;
    }
    // Original function: FUN_00453518.
    std::int32_t Human_WorkCenterMarker_GetId(const void* human)
    {
        const auto* array = static_cast<const CArrayHandler*>(gMarkerArray);
        for (auto id = array->mFirstScanIndex; id <= array->mLastScanIndex; ++id)
        {
            const void* marker = static_cast<const std::byte*>(gMarkerEntries) + id * 0x18;
            if (Read<std::uint8_t>(marker, 0) != 0 && Read<std::uint32_t>(marker, 0x10) == 1 && Read<std::uint32_t>(marker, 0x14) == Read<std::uint32_t>(human, 4)) return id;
        }
        return -1;
    }
    // Original function: FUN_00453558.
    void Human_WorkCenterMarker_Create(void* human) { MarkerArray_AllocateMarker(1, static_cast<std::byte*>(human) + 0xB30, Read<std::uint32_t>(human, 4)); }
    // Original function: FUN_00453570.
    void Human_WorkCenterMarker_Remove(void* human) { const auto id = Human_WorkCenterMarker_GetId(human); if (id != -1) MarkerArray_FreeMarker(id); }
    // Original function: FUN_00453587.
    void Human_WorkCenterMarker_Update(void* human)
    {
        const auto id = Human_WorkCenterMarker_GetId(human);
        if (id != -1)
        {
            void* marker = static_cast<std::byte*>(gMarkerEntries) + id * 0x18;
            if (Human_WorkCenterMarker_IsVisible(human)) Marker_MapPosition_Set(marker, static_cast<std::byte*>(human) + 0xB30);
            else MarkerArray_FreeMarker(Read<std::int32_t>(marker, 4));
        }
        else if (Human_WorkCenterMarker_IsVisible(human)) Human_WorkCenterMarker_Create(human);
    }
    // Original function: FUN_0043472e; Windows outlined work-house assignment.
    void Human_l_AssignWorkHouse(void* human, std::uint32_t house)
    {
        Write(human, 0xB34, house);
        Write<std::uint32_t>(human, 0xB38, Read<std::uint32_t>(House(house), 8));
        Human_SetupCurrentJobType(human);
        Human_Production_Update(human);
    }
    // Original function: FUN_004419c8.
    bool Human_HouseMisc_Enter(void* human)
    {
        if (Read<std::uint8_t>(human, 0xB50) != 0) return false;
        std::uint32_t point = 0;
        if (!Human_GetMapPosition(human, &point)) return false;
        const void* micro = GameMap_GetConstMapMicroPointPtr(&point);
        if (Read<std::uint8_t>(micro, 0) != 2 || (Read<std::uint8_t>(micro, 2) & 2u) == 0) return false;
        const auto type = Read<std::int32_t>(House(Read<std::uint16_t>(micro, 4)), 0x14);
        if (type >= 10 && type <= 11) return false;
        Write<std::uint8_t>(human, 0xB50, 1);
        return true;
    }
    // Original function: FUN_0045c762.
    bool Human_l_StartAtomicAnimation(void* human, std::uint32_t action, std::int32_t direction)
    {
        if (action == 1 || !Human_l_IsAtomicInterruptable(human)) return false;
        auto job = Read<std::uint32_t>(human, 0x18);
        std::uint32_t animation;
        for (;;)
        {
            const auto* descriptor = static_cast<const std::byte*>(gStaticJobTypeData) + job * 0x88u;
            if (Read<std::uint8_t>(descriptor, 0x24u + action) == 0) return false;
            animation = gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].atomicIds[job][action];
            if (animation != 0xFFFFFFFFu) break;
            job = Read<std::uint32_t>(descriptor, 0x20);
            if (job == 0) return false;
        }
        Write<std::uint32_t>(human, 0xAC4, 0);
        Write<std::uint32_t>(human, 0xAC8, Read<std::uint32_t>(human, 0xAC8) + 1u);
        Write(human, 0xAC0, animation);
        Write(human, 0xAB8, action);
        Write<std::uint8_t>(human, 0xABC, 1);
        void* moveable = Moveable(Read<std::uint32_t>(human, 0x0C));
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            const auto preferred = gStaticAtomicAnimationData[animation].startDirection;
            if (preferred != -1) MapMoveable_Attachment_StartTurn(moveable, preferred);
            else if (direction != -1) MapMoveable_Attachment_StartTurn(moveable, direction);
        }
        return true;
    }
    // Original function: FUN_0043819b.
    bool Human_Init(void* human, std::int32_t id, std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags)
    {
        if (Read<std::uint8_t>(human, 0) != 0 || GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6)) == 0) return false;
        std::memset(human, 0, 0xC58);
        Write<std::uint8_t>(human, 0, 1);
        Write(human, 4, id);
        Write(human, 8, Game_UniqueId_Generate());
        Write(human, 0x10, player);
        for (const auto offset : {0xB28u, 0xB34u, 0xB3Cu, 0xB48u, 0xB44u, 0xB14u, 0xB18u, 0xB20u, 0xB1Cu, 0xB24u}) Write<std::int32_t>(human, offset, -1);
        const auto moveableId = MapMoveableArray_AllocateMapMoveable(gMapMoveableArrayStorage.manager, 1, id);
        Write(human, 0x0C, moveableId);
        if (moveableId == -1) return false;
        void* moveable = Moveable(moveableId);
        MapMoveable_Attachment_AttachToMap(moveable, point);
        const void* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
        if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0 && Read<std::uint32_t>(House(Read<std::uint16_t>(micro, 4)), 0x2C) == 3) Human_HouseMisc_Enter(human);
        Write(human, 0x14, tribe);
        // Original outlined operation 00438CEC: clear cargo before notifying its change.
        Write<std::uint32_t>(human, 0xAD4, 0);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
        Write<std::uint32_t>(human, 0xB30, Read<std::uint32_t>(Human_GetMapPositionRef(human), 0));
        Write<std::uint32_t>(human, 0x38C, 0);
        Human_WorkCenterMarker_Update(human);
        Write<std::uint32_t>(human, 0xADC, 0);
        Write<std::uint32_t>(human, 0xAE0, 0);
        FUN_00438C79(static_cast<std::byte*>(human) + 0x194);
        std::memset(static_cast<std::byte*>(human) + 0x24, 0, 0x138);
        std::memset(static_cast<std::byte*>(human) + 0x15C, 0, 0x38);
        Write<std::uint8_t>(human, 0x15Cu + job, 1);
        std::memset(static_cast<std::byte*>(human) + 0x1E0, 0, 0x38);
        Write<std::uint8_t>(human, 0x1DF, 0);
        Write(human, 0xB80, missionCheck);
        Write(human, 0xB84, flags);
        if (tribe == 5 || tribe == 6)
        {
            Write<std::uint32_t>(human, 0xB5C, (flags & 0x10000u) != 0 ? 2u : 1u);
            Write<std::uint8_t>(human, 0xB60, 1);
            Write<std::uint32_t>(human, 0xAE8, 2);
            Write<std::uint32_t>(human, 0xAE4, 1);
        }
        else if (static_cast<std::int32_t>(job) >= 31 && static_cast<std::int32_t>(job) <= 41)
        {
            Write<std::uint32_t>(human, 0xB5C, 2);
            Write<std::uint8_t>(human, 0xB60, 1);
            switch (job)
            {
            case 31: Write<std::uint32_t>(human, 0xAE4, 1); break;
            case 32: Write<std::uint32_t>(human, 0xAE4, 4); break;
            case 33: Write<std::uint32_t>(human, 0xAE4, 5); Write<std::uint8_t>(human, 0x17D, 1); break;
            case 34: Write<std::uint32_t>(human, 0xAE4, 7); break;
            case 35: Write<std::uint32_t>(human, 0xAE4, 8); Write<std::uint8_t>(human, 0x17F, 1); break;
            case 40: Write<std::uint32_t>(human, 0xAE4, 16); break;
            case 41: Write<std::uint32_t>(human, 0xAE4, 17); Write<std::uint8_t>(human, 0x185, 1); break;
            default: job = 31; Write<std::uint32_t>(human, 0xAE4, 1); break;
            }
        }
        else if (static_cast<std::int32_t>(job) >= 42 && static_cast<std::int32_t>(job) <= 47)
        {
            Write<std::uint32_t>(human, 0xB5C, 2);
            Write<std::uint8_t>(human, 0xB60, 1);
            switch (job)
            {
            case 43: Write<std::uint32_t>(human, 0xAE4, 6); Write<std::uint32_t>(human, 0xAE8, 4); break;
            case 44: Write<std::uint32_t>(human, 0xAE4, 9); Write<std::uint32_t>(human, 0xAE8, 3); break;
            case 45: Write<std::uint32_t>(human, 0xAE4, 12); Write<std::uint32_t>(human, 0xAE8, 3); break;
            case 46: Write<std::uint32_t>(human, 0xAE4, 15); Write<std::uint32_t>(human, 0xAE8, 3); break;
            case 47: Write<std::uint32_t>(human, 0xAE4, 18); Write<std::uint32_t>(human, 0xAE8, 2); break;
            default: job = 42; Write<std::uint32_t>(human, 0xAE4, 3); Write<std::uint32_t>(human, 0xAE8, 1); break;
            }
            const auto experience = Read<std::uint32_t>(human, 0x13C);
            Write<std::uint32_t>(human, 0x13C, experience > 9995u ? 10000u : experience + 5u);
            Write<std::uint32_t>(human, 0xB84, Read<std::uint32_t>(human, 0xB84) | 0x41u);
            if (tribe != 2 || job != 47) Write<std::uint32_t>(human, 0xB84, Read<std::uint32_t>(human, 0xB84) | 0x80u);
            Write<std::uint8_t>(human, 0xB85, Read<std::uint8_t>(human, 0xB85) | 0x18u);
            Human_Equipment_SetShoeType(human, 30);
            Write<std::uint32_t>(human, 0xADC, 10000);
        }
        else
        {
            Write<std::uint32_t>(human, 0xB5C, 2);
            Write<std::uint8_t>(human, 0xB60, 1);
            Write<std::uint32_t>(human, 0xAE4, job == 5 ? 2u : 1u);
        }
        Human_SetJobType(human, job);
        Human_SetupPlayerSpecificStuff(human);
        Human_Production_Setup(human);
        Write(human, 0xAA0, Human_GetHumanAttributeMaximum_Hitpoints(human));
        Write<std::uint32_t>(human, 0xAA8, Game_Random_GetNumber() % 5000u + 5000u);
        Write<std::uint32_t>(human, 0xAB4, Game_Random_GetNumber() % 5000u + 5000u);
        Write<std::uint32_t>(human, 0xAB0, Game_Random_GetNumber() % 5000u + 5000u);
        const auto stamina = Game_Random_GetNumber() % 5000u + 5000u;
        Write<std::uint32_t>(human, 0x370, 0);
        Write<std::uint32_t>(human, 0x368, 1);
        Write<std::uint32_t>(human, 0x36C, 1);
        Write(human, 0xAAC, stamina);
        Human_ResetUserCommandArray(human);
        moveable = Moveable(Read<std::uint32_t>(human, 0x0C));
        void* pathfinder = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        Write<std::uint32_t>(moveable, 0x44, 8);
        Write<std::uint32_t>(moveable, 0x48, 1250);
        Write<std::uint32_t>(moveable, 0x4C, 1);
        Write<std::uint8_t>(pathfinder, 0x16, 1);
        Write<std::uint8_t>(moveable, 0x50, 1);
        Write<std::int32_t>(pathfinder, 0x24, -1);
        Write<std::int32_t>(pathfinder, 0x1C, -1);
        Write<std::uint32_t>(pathfinder, 0x2C, Read<std::uint32_t>(human, 0x10));
        if (Human_WorkCenterMarker_IsVisible(human)) Human_WorkCenterMarker_Create(human);
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(human, 0x10), point, Read<std::uint32_t>(human, 0xB7C));
        Human_l_StartAtomicAnimation(human, 2, -1);
        if (Human_IsAgeBaby(human))
        {
            Write<std::uint32_t>(human, 0x37C, 10); // Original constant 004F3C34.
            if (gGameTime.tick > 1u)
            {
                const auto offset = Read<std::uint32_t>(human, 0x10) * 0x4A80u + 0x4A14u;
                Write<std::uint32_t>(gGameControl, offset, Read<std::uint32_t>(gGameControl, offset) + 1u);
            }
        }
        if (Read<std::uint32_t>(gGameControl, 0x5D200) == 2)
        {
            if (gGameStaticVars.DAT_005109c4 != 0)
            {
                if (static_cast<std::int32_t>(player) < 20 && Read<std::uint32_t>(gGameControl, player * 0x4A80u + 4u) == 1) Write<std::uint32_t>(human, 0xB84, Read<std::uint32_t>(human, 0xB84) | 1u);
            }
            else if (player == 0) Write<std::uint32_t>(human, 0xB84, Read<std::uint32_t>(human, 0xB84) | 1u);
        }
        // Original SGameAssistantSingleHumanData::System_Init, 0044057D.
        std::memset(static_cast<std::byte*>(human) + 0xB88, 0, 0xD0);
        Write<std::uint8_t>(human, 0xB88, 1);
        return true;
    }

    // Original function: FUN_004418ec.
    void Human_DetachFromHomeHouse(void* human)
    {
        const auto circulation = Read<std::uint32_t>(human, 0xACC);
        if (Read<std::uint32_t>(human, 0x18) == 5 && circulation != 0 && circulation != 1)
        {
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
        }
        Write<std::uint32_t>(human, 0xB28, 0xFFFFFFFFu);
        Write<std::uint32_t>(human, 0xB2C, 0);
    }

    // Original function: FUN_00441925.
    void Human_DetachFromWorkHouse(void* human)
    {
        const auto produced = Read<std::uint32_t>(human, 0x1C);
        if (Read<std::uint32_t>(human, 0xACC) != 0 && (Read<std::uint32_t>(human, 0xAD0) == produced || produced == 65 || produced == 58 || produced == 57))
        {
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
        }
        Human_DetachFromWorkHouse2(human);
    }

    // Original function: FUN_00442028; setup precedes production update.
    void Human_DetachFromWorkHouse2(void* human)
    {
        Write<std::uint32_t>(human, 0xB34, 0xFFFFFFFFu);
        Write<std::uint32_t>(human, 0xB38, 0);
        Human_SetupCurrentJobType(human);
        Human_Production_Update(human);
    }

    // Original function: FUN_00441962.
    void Human_DetachFromVehicle(void* human)
    {
        const auto circulation = Read<std::uint32_t>(human, 0xACC);
        if (circulation != 0 && circulation != 1) { Human_StopCurrentGoodCirculationMode(human); Write<std::uint32_t>(human, 0xAD0, 0); }
        void* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
        if (Read<std::uint32_t>(vehicle, 8) == Read<std::uint32_t>(human, 0xB4C) && Vehicle_Passengers_GetAttachedHumanSlot(vehicle, Read<std::uint32_t>(human, 4)) != -1) Vehicle_Passengers_DetachHuman(vehicle, Read<std::uint32_t>(human, 4));
        Write<std::int32_t>(human, 0xB48, -1);
        Write<std::uint32_t>(human, 0xB4C, 0);
    }
    // Original function: FUN_00441f8c; passenger state changes before detaching from the map.
    std::uint32_t Human_VehicleMisc_Enter(void* human)
    {
        auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
        if (!Vehicle_Passengers_MoveIn_h(vehicle, Read<std::uint32_t>(human, 4))) return 0;
        MapMoveable_Attachment_DetachFromMap(Moveable(Read<std::uint32_t>(human, 0x0C)));
        Human_Target_Reset(human);
        Human_Target_ResetSecondaryTarget(human);
        Write<std::uint8_t>(human, 0xB51, 1);
        Human_WorkCenterMarker_Update(human);
        return 1;
    }

    // Original function: FUN_00441fe2.
    void Human_VehicleMisc_Leave(void* human, const void* point)
    {
        Write<std::uint32_t>(human, 0x36C, 2);
        MapMoveable_Attachment_AttachToMap(Moveable(Read<std::uint32_t>(human, 0x0C)), point);
        Write<std::uint8_t>(human, 0xB51, 0);
        Write<std::uint32_t>(human, 0x38C, 0);
        Write<std::uint32_t>(human, 0xB30, Read<std::uint32_t>(point, 0));
        Human_WorkCenterMarker_Update(human);
    }
    // Original function: FUN_00438a26.
    void Human_Detach(void* human, bool forceVehicleDetach)
    {
        if (Human_IsAttachedToHomeHouse(human)) House_Inhabitants_DetachHuman(House(Read<std::uint32_t>(human, 0xB28)), Read<std::uint32_t>(human, 4), 1);
        if (Human_IsAttachedToLearnHouse(human)) House_Inhabitants_DetachHuman(House(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 4), 8);
        if (Human_IsAttachedToWorkHouse(human)) House_Inhabitants_DetachHuman(House(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 4), Human_IsJobBuilder(human) ? 4u : 2u);
        if (Human_IsAttachedToRoadPoint(human)) BuildManager_DetachBuilder(gRoadBuildManager, Read<std::uint32_t>(human, 0x10), static_cast<std::byte*>(human) + 0xB30);
        if (Human_IsAttachedToWallPoint(human)) BuildManager_DetachBuilder(gWallBuildManager, Read<std::uint32_t>(human, 0x10), static_cast<std::byte*>(human) + 0xB30);
        const auto merchant = Read<std::int32_t>(human, 0xB44);
        if (merchant != -1) HumanMerchantArray_FreeMerchant(gHumanMerchantArray, merchant);
        const auto vehicleId = Read<std::uint32_t>(human, 0xB48);
        if (vehicleId != 0xFFFFFFFFu)
        {
            void* vehicle = CVehicleArray::mStaticVars + vehicleId * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(vehicle, 8) == Read<std::uint32_t>(human, 0xB4C))
            {
                if (forceVehicleDetach)
                {
                    if (Read<std::uint8_t>(human, 0xB51) != 0 && Vehicle_Door_IsEntryPointOnLand(vehicle, 0)) Human_VehicleMisc_Leave(human, Human_GetMapPositionRef(human));
                    Write<std::uint8_t>(human, 0xB51, 0);
                    Human_DetachFromVehicle(human);
                }
                else if (Read<std::uint32_t>(vehicle, 0x1E0) != Read<std::uint32_t>(human, 0x10)) Human_DoAddNewUserCommand(human, 39, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            }
        }
        for (const auto parentOffset : {0xB18u, 0xB14u})
        {
            const auto parent = Read<std::int32_t>(human, parentOffset);
            if (parent != -1) { Write<std::int32_t>(Human(parent), 0xB24, -1); Write<std::int32_t>(human, parentOffset, -1); }
        }
        const auto child = Read<std::int32_t>(human, 0xB24);
        if (child != -1) { Write<std::int32_t>(Human(child), Human_IsSexFemale(human) ? 0xB18u : 0xB14u, -1); Write<std::int32_t>(human, 0xB24, -1); }
        const auto spouse = Read<std::int32_t>(human, 0xB20);
        if (spouse != -1)
        {
            Write<std::int32_t>(Human(spouse), 0xB20, -1);
            Write<std::int32_t>(Human(spouse), 0xB1C, -1);
            Write<std::int32_t>(human, 0xB20, -1);
            Write<std::int32_t>(human, 0xB1C, -1);
        }
        const auto fiance = Read<std::int32_t>(human, 0xB1C);
        if (fiance != -1)
        {
            if (Read<std::uint8_t>(Human(fiance), 0) != 0) Human_StopWaitingForOtherHuman(Human(fiance));
            Human_StopWaitingForOtherHuman(human);
        }
    }
    // Original function: FUN_0045d0a8.
    static bool Human_l_Item_IsItemUnused(const void* human, std::int32_t slot)
    {
        return slot != -1 && Read<std::uint32_t>(human, 0xAF4u + static_cast<std::uint32_t>(slot) * 8u) != 0 && Read<std::uint32_t>(human, 0xAF8u + static_cast<std::uint32_t>(slot) * 8u) == 100;
    }
    // Original function: FUN_004386d9.
    void Human_Exit(void* human)
    {
        // Shared System_Exit body at 00430CA4 clears the assistant allocation flag.
        Write<std::uint8_t>(human, 0xB88, 0);
        if (Read<std::uint8_t>(&gGameStaticVars, 0x1B0) == 0)
        {
            const auto playerOffset = Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            Write<std::uint32_t>(gGameControl, playerOffset + 0x4A18u, Read<std::uint32_t>(gGameControl, playerOffset + 0x4A18u) + 1u);
            if (Human_IsJobSoldier(human)) Write<std::uint32_t>(gGameControl, playerOffset + 0x4A1Cu, Read<std::uint32_t>(gGameControl, playerOffset + 0x4A1Cu) + 1u);
            else HappinessManager_Messages_CivilistDied(gHappinessManager, human);
        }
        Human_StopCurrentGoodCirculationMode(human);
        Human_Detach(human, true);
        const auto moveableId = Read<std::int32_t>(human, 0x0C);
        if (Read<std::uint8_t>(&gGameStaticVars, 0x1B0) == 0 && moveableId != -1 && Read<std::uint8_t>(Moveable(moveableId), 0x10) != 0)
        {
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x25, true, 4, MapMigPoint_GetLong(Human_GetMapPositionRef(human)), 0);
            if ((Read<std::uint32_t>(human, 0xB84) & 0x4000u) == 0 && !(Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32))
            {
                const auto cadaver = NC2LogicExternals::External_Misc_GetEngineCadaverLandscapeId(Read<std::uint32_t>(human, 0x14), 87);
                if (cadaver != -1)
                {
                    const void* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
                    bool blocked = false;
                    if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0)
                    {
                        const auto engineId = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(Human_GetMapPositionRef(human));
                        const auto* area0 = NC2LogicExternals::External_Landscape_GetAreaPtr(engineId, Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0, 0);
                        const auto* area1 = NC2LogicExternals::External_Landscape_GetAreaPtr(engineId, Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0, 1);
                        const auto* area3 = NC2LogicExternals::External_Landscape_GetAreaPtr(engineId, Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0, 3);
                        blocked = area0 != nullptr || area1 != nullptr || area3 != nullptr;
                    }
                    if (!blocked)
                    {
                        Logic_EntryPoint_Edit_Start();
                        Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(Human_GetMapPositionRef(human), 87, 1, cadaver, 0, -1);
                        Logic_EntryPoint_Edit_Stop();
                    }
                }
                const auto drop = [&](std::uint32_t good) { Map_GoodTool_PutOnMap(good, 1, Read<std::uint32_t>(Human_GetMapPositionRef(human), 0), 10, true); };
                if (Read<std::uint32_t>(human, 0xAD4) != 0) drop(Read<std::uint32_t>(human, 0xAD4));
                // Windows multiplies as a DWORD before signed division; preserve overflow bits.
                const auto percentage = [&](std::uint32_t offset) { return NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(human, offset) * 100u) / 10000; };
                if (Read<std::uint32_t>(human, 0xAEC) != 0 && Read<std::uint32_t>(human, 0xADC) != 0 && percentage(0xADC) >= 90) drop(Read<std::uint32_t>(human, 0xAEC));
                const auto tool = Read<std::uint32_t>(human, 0xAF0);
                if (tool != 0 && Read<std::uint32_t>(human, 0xAE0) != 0 && (tool == 31 || tool == 32) && percentage(0xAE0) >= 90) drop(tool);
                const auto armor = Read<std::uint32_t>(human, 0xAE8);
                if (armor != 0) drop(Read<std::uint32_t>(gStaticArmorTypeData, armor * 0x34u + 0x24u));
                const auto weaponGood = Read<std::uint32_t>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)], Read<std::uint32_t>(human, 0xAE4) * 0xC4u + 0x24u);
                if (weaponGood != 0 && Human_IsJobSoldier(human)) drop(weaponGood);
                for (std::uint32_t slot = 0; slot < 4; ++slot) if (Read<std::uint32_t>(human, 0xAF4u + slot * 8u) != 0 && Read<std::int32_t>(human, 0xAF8u + slot * 8u) > 0 && Human_l_Item_IsItemUnused(human, slot)) drop(Read<std::uint32_t>(human, 0xAF4u + slot * 8u));
            }
        }
        Human_WorkCenterMarker_Remove(human);
        if (Read<std::uint8_t>(human, 0) != 0)
        {
            const auto id = Read<std::int32_t>(human, 0x0C);
            if (id != -1) MapMoveableArray_FreeMapMoveable(gMapMoveableArrayStorage.manager, id);
            Write<std::uint8_t>(human, 0, 0);
        }
    }
}
