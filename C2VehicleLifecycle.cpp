#include "C2VehicleLifecycle.h"
#include "C2VehicleAccess.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanWalking.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2MapPathfinderSearch.h"
#include "C2MissionStartObjects.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2StaticTribeTypeData.h"
#include "C2StaticVehicleTypeData.h"
#include "XCrtFileAccess.h"
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0043b317; the micro cursor advances cumulatively around each ring.
    bool Tool_IsAnyNotMovingVehicleInsidePoint(const void* point)
    {
        using namespace NC2Lifetime;
        if (Tool_IsPointHitByNotMovingVehicle(point, point)) return true;
        auto cursor = Read<std::uint32_t>(point, 0);
        for (std::uint32_t radius = 1; radius < 4; ++radius)
        {
            MapMigPoint_MoveCursorUnwrapped(cursor, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < radius; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                    if (MapMigPoint_IsInsideMap(&cursor) && Tool_IsPointHitByNotMovingVehicle(point, &cursor)) return true;
                }
        }
        return false;
    }

    // Original function: FUN_0043b39a; stationary vehicle footprints use their logic size.
    bool Tool_IsPointHitByNotMovingVehicle(const void* point, const void* searchPoint)
    {
        using namespace NC2Lifetime;
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(searchPoint); id != -1; id = iterator.GetNextMapMoveableId())
        {
            const void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            if (Read<std::uint32_t>(moveable, 8) != 3) continue;
            const void* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CVehicleArray::ElementSize;
            if (Read<std::uint32_t>(vehicle, 0x4FC) == 1) continue;
            const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(moveable, 0x12), Read<std::int16_t>(moveable, 0x14)));
            if (gStaticVehicleTypeStorage.records[Read<std::uint32_t>(vehicle, 0x1E8)].logicSize >= distance) return true;
        }
        return false;
    }
    using namespace NC2Lifetime;
    namespace
    {
        void* Vehicle(std::int32_t id) { return CVehicleArray::mStaticVars + static_cast<std::intptr_t>(id) * CVehicleArray::ElementSize; }
        void* Moveable(const void* vehicle) { return gMapMoveableArrayStorage.entries + static_cast<std::intptr_t>(Read<std::int32_t>(vehicle, 0x1DC)) * 0x58; }
        const void* Slot(const void* vehicle, std::uint32_t offset, std::uint32_t index) { return static_cast<const std::byte*>(vehicle) + offset + index * 12u; }
        std::uint32_t Distance(const void* first, const void* second) { return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(first, 0), Read<std::int16_t>(first, 2), Read<std::int16_t>(second, 0), Read<std::int16_t>(second, 2))); }

        // Original constructor: FUN_0043b0d9. The callback retains the initial output point.
        struct SVehiclePointSearch
        {
            const SStaticVehicleTypeData* type;
            const void* source;
            void* output;
            std::uint32_t initialPoint;
            std::uint32_t bestDistance;
            std::uint32_t minimumDistance;
            std::uint32_t sourceDistance;
            SVehiclePointSearch(const SStaticVehicleTypeData* data, const void* from, void* to) : type(data), source(from), output(to), initialPoint(Read<std::uint32_t>(to, 0)), bestDistance(Distance(from, to)), minimumDistance(0) {}
        };
        static_assert(sizeof(SVehiclePointSearch) == 0x1C);

        // Original function: FUN_0043b12b. False means continue searching, including on success.
        bool __cdecl slTool_CheckForVehiclePoint(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            if (callback->distance == 0) return false;
            auto& data = *static_cast<SVehiclePointSearch*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint16_t>(micro, 10) != 0xFFFFu || (Read<std::uint8_t>(micro, 12) & 7u) < static_cast<std::uint32_t>(data.type->logicSize)) return false;
            const auto distance = Distance(point, &data.initialPoint);
            const auto fromSource = Distance(data.source, point);
            if (fromSource < data.minimumDistance || distance > data.bestDistance) return false;
            if (data.sourceDistance == 0) data.sourceDistance = fromSource;
            else if (data.sourceDistance < fromSource) return false;
            Write(data.output, 0, Read<std::uint32_t>(point, 0));
            data.bestDistance = distance;
            return false;
        }

        // Original function: FUN_004627b1. Ships additionally require land on the outer ring.
        bool __cdecl slTool_CheckForVehiclePointNearLand(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            if (callback->distance == 0) return false;
            auto& data = *static_cast<SVehiclePointSearch*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint16_t>(micro, 10) != 0xFFFFu || (Read<std::uint8_t>(micro, 12) & 7u) < static_cast<std::uint32_t>(data.type->logicSize)) return false;
            const auto distance = Distance(point, &data.initialPoint);
            const auto fromSource = Distance(data.source, point);
            if (fromSource < data.minimumDistance || distance > data.bestDistance) return false;
            auto cursor = Read<std::uint32_t>(point, 0);
            const auto radius = static_cast<std::uint32_t>(data.type->logicSize) + 1u;
            MapMigPoint_MoveCursorUnwrapped(&cursor, 4, radius);
            bool land = false;
            for (std::uint32_t direction = 0; direction < 6 && !land; ++direction)
                for (std::uint32_t step = 0; step < radius; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                    if (MapMigPoint_IsInsideMap(&cursor) && GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&cursor), 6)) == 1) { land = true; break; }
                }
            if (!land) return false;
            if (data.sourceDistance == 0) data.sourceDistance = fromSource;
            else if (data.sourceDistance < fromSource) return false;
            Write(data.output, 0, Read<std::uint32_t>(point, 0));
            data.bestDistance = distance;
            return false;
        }

        struct SVehicleFreePositionSearch { void* vehicle; void* output; };
        // Original function: FUN_0044178a.
        bool __cdecl slVehicle_SearchVehicleFreePos(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            const auto& data = *static_cast<SVehicleFreePositionSearch*>(callback->userData);
            if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 12) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(data.vehicle)->logicSize) || Vehicle_GoAway_HitByVehicle(data.vehicle, point)) return false;
            Write(data.output, 0, Read<std::uint32_t>(point, 0));
            return true;
        }
    }

    // Original function: FUN_0040adb7.
    std::int32_t Logic_EntryPoint_Add_Vehicle(std::uint32_t player, std::uint32_t tribe, std::uint32_t type, const void* point, std::uint32_t missionCheck) { return CVehicleArray::sTheObjectPtr->AllocateVehicle(player, tribe, type, point, missionCheck); }

    // Original function: FUN_0040b87a; Windows reserves the slot before validating the point.
    std::int32_t CVehicleArray::AllocateVehicle(std::uint32_t player, std::uint32_t tribe, std::uint32_t type, const void* point, std::uint32_t missionCheck)
    {
        const auto id = Element_Allocate();
        if (id == -1 || !MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point)) return -1;
        if (!Vehicle_Init(Vehicle(id), id, player, tribe, type, point, missionCheck)) { FreeVehicle(id); return -1; }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(8, true, id, 1, 0);
        return id;
    }

    // Original function: FUN_0040b8f5.
    void CVehicleArray::FreeVehicle(std::int32_t id)
    {
        auto* vehicle = Vehicle(id);
        if (Read<std::uint8_t>(vehicle, 0) == 0 || !Element_Free(id, true)) return;
        if (Read<std::int32_t>(vehicle, 0x1DC) != -1) CCallbackManager::sTheObjectPtr->ExecuteCallback(8, false, id, 0, 0);
        Vehicle_Exit(vehicle);
    }

    // Original function: FUN_00436b39.
    bool Vehicle_Init(void* vehicle, std::int32_t id, std::uint32_t player, std::uint32_t tribe, std::uint32_t type, const void* point, std::uint32_t missionCheck)
    {
        if (Read<std::uint8_t>(vehicle, 0) != 0) return false;
        std::memset(vehicle, 0, CVehicleArray::ElementSize);
        Write<std::uint8_t>(vehicle, 0, 1);
        Write(vehicle, 4, id);
        Write(vehicle, 8, Game_UniqueId_Generate());
        Write(vehicle, 0x1E0, player);
        Write(vehicle, 0x1E4, tribe);
        Write(vehicle, 0x1E8, type);
        Write(vehicle, 0x1EC, type >= 1 && type <= 6 ? type + 49u : 0u);
        const auto moveableId = MapMoveableArray_AllocateMapMoveable(gMapMoveableArrayStorage.manager, 3, id);
        Write(vehicle, 0x1DC, moveableId);
        if (moveableId == -1) return false;
        auto* moveable = Moveable(vehicle);
        MapMoveable_Attachment_AttachToMap(moveable, point);
        auto* path = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        Write<std::uint32_t>(moveable, 0x44, 8);
        Write<std::uint32_t>(moveable, 0x48, 1250);
        Write<std::uint32_t>(moveable, 0x4C, 2);
        Write(path, 0x10, Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize);
        Write<std::uint8_t>(path, 0x16, 1);
        Write<std::uint8_t>(moveable, 0x50, 1);
        Write(path, 0x2C, player);
        Write<std::int32_t>(path, 0x24, -1);
        Write<std::int32_t>(path, 0x1C, -1);
        Write<std::uint32_t>(vehicle, 12, type == 3 || type == 4 ? 5000u : type == 5 ? 3000u : 1000u);
        Vehicle_ResetUserCommandArray(vehicle);
        Vehicle_l_Passengers_ResetContents(vehicle);
        Write<std::uint32_t>(vehicle, 0x518, type == 4 ? 25u : type == 3 || type == 5 ? 20u : 15u);
        Write<std::int32_t>(vehicle, 0x2F4, -1);
        Write(vehicle, 0x51C, missionCheck);
        if (type == 3 || type == 4)
        {
            const auto continent = Vehicle_l_GetLandContinentIdFromBorder(vehicle, Read<std::uint32_t>(point, 0));
            if (continent == -1) Write<std::uint32_t>(vehicle, 0x1CC, 0xFFFFFFFFu);
            else { Write<std::uint8_t>(vehicle, 0x1D0, 1); Vehicle_l_GetDoorPoint(vehicle, static_cast<std::uint32_t>(continent), Read<std::uint32_t>(point, 0)); }
        }
        Write(vehicle, 0x1D6, Read<std::uint32_t>(point, 0));
        Write<std::uint32_t>(vehicle, 0x53C, 3);
        GameMap_Exploration_DoExplore(player, point, Read<std::uint32_t>(vehicle, 0x518));
        Write<std::int32_t>(vehicle, 0x540, -1);
        Vehicle_l_StartAtomicIdle(vehicle);
        return true;
    }

    // Original function: FUN_00436d90; Windows passes the packed point by value.
    void Vehicle_l_GetDoorPoint(void* vehicle, std::uint32_t continent, std::uint32_t point)
    {
        const auto distance = Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerVectorY;
        MapMigPoint_MoveCursorUnwrapped(&point, 4, static_cast<std::uint32_t>(distance));
        for (std::uint32_t direction = 0; direction < 6; ++direction)
            for (std::int32_t step = 0; step < distance; ++step)
            {
                MapMigPoint_MoveCursorUnwrapped(point, direction);
                if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6) == continent) { Write(vehicle, 0x1CC, point); return; }
            }
    }

    // Original function: FUN_00436e0b.
    std::int32_t Vehicle_l_GetLandContinentIdFromBorder(const void* vehicle, std::uint32_t point)
    {
        const auto distance = Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerVectorY;
        MapMigPoint_MoveCursorUnwrapped(&point, 4, static_cast<std::uint32_t>(distance));
        for (std::uint32_t direction = 0; direction < 6; ++direction)
            for (std::int32_t step = 0; step < distance; ++step)
            {
                MapMigPoint_MoveCursorUnwrapped(point, direction);
                const auto* micro = GameMap_GetConstMapMicroPointPtr(&point);
                const auto continent = Read<std::uint8_t>(micro, 6);
                if (GameMap_Continent_GetContinentTyp(continent) == 1 && (Read<std::uint8_t>(micro, 12) & 0x80u) == 0) return continent;
            }
        return -1;
    }

    // Original function: FUN_00436e8d; landscape editing remains an external boundary.
    void Vehicle_Exit(void* vehicle)
    {
        if (Read<std::int32_t>(vehicle, 0x1DC) != -1 && Vehicle_Door_IsEntryPointOnLand(vehicle, 0)) Vehicle_Passengers_MoveAllOut(vehicle, true);
        auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x1F0, index);
            if (HumanReference_IsAllocated(entry)) CHumanArray::sTheObjectPtr->FreeHuman(Read<std::int32_t>(entry, 0));
        }
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0) CHumanArray::sTheObjectPtr->FreeHuman(Vehicle_Passengers_GetAttachedCommanderId(vehicle));
        count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x2FC, index);
            if (VehicleReference_IsAllocated(entry)) CVehicleArray::sTheObjectPtr->FreeVehicle(Read<std::int32_t>(entry, 0));
        }
        if (Read<std::uint8_t>(vehicle, 0) == 0) return;
        if (Read<std::uint8_t>(Moveable(vehicle), 0x10) != 0)
        {
            Logic_EntryPoint_Edit_Start();
            const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
            if (Read<std::uint8_t>(&gGameStaticVars, 0x1B1) == 0 && type != 3 && type != 4)
            {
                auto point = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
                const auto wreck = [&](bool checkInside)
                {
                    const auto chance = Game_Random_GetNumber() % 100u;
                    if (chance > 50u || (checkInside && !MapMigPoint_IsInsideMap(&point))) return;
                    const auto engine = NC2LogicExternals::External_Misc_GetEngineDepotLandscapeId(Read<std::uint32_t>(vehicle, 0x1E4), 46);
                    if (engine != -1) Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(&point, 1, 1, engine, 0, -1);
                };
                wreck(false);
                for (std::uint32_t ring = 1; ring <= static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize); ++ring)
                {
                    MapMigPoint_MoveCursorUnwrapped(point, 4);
                    for (std::uint32_t direction = 0; direction < 6; ++direction)
                        for (std::uint32_t step = 0; step < ring; ++step) { MapMigPoint_MoveCursorUnwrapped(point, direction); wreck(true); }
                }
            }
            if (Read<std::uint8_t>(&gGameStaticVars, 0x1B0) == 0 && type != 3 && type != 4)
                for (std::uint32_t good = 0; good < 66; ++good)
                {
                    if (!Vehicle_Stock_CanStoreGoodType(vehicle, good)) continue;
                    const auto amount = Vehicle_Stock_GetAmountOfGoods(vehicle, good);
                    if (amount != 0) Map_GoodTool_PutOnMap(good, amount, Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0), 10, false);
                }
            Logic_EntryPoint_Edit_Stop();
        }
        const auto moveableId = Read<std::int32_t>(vehicle, 0x1DC);
        if (moveableId != -1) MapMoveableArray_FreeMapMoveable(gMapMoveableArrayStorage.manager, moveableId);
        Write<std::uint8_t>(vehicle, 0, 0);
    }

    // Original function: FUN_0043fdaa.
    void Vehicle_ResetUserCommandArray(void* vehicle) { for (std::uint32_t index = 0; index < 22; ++index) Write<std::uint8_t>(vehicle, 0x10u + index * 20u, 0); }

    // Original function: FUN_004413a4.
    void Vehicle_SetTargetWorkPosition(void* vehicle, const void* point) { Write(vehicle, 0x1D2, Read<std::uint32_t>(point, 0)); }

    // Original function: FUN_004413f9; send nearby humans away from the entire footprint.
    void Vehicle_l_SendAwayRadial(void* vehicle)
    {
        if (Vehicle_Attachment_IsInsideVehicle(vehicle)) return;
        auto point = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
        Vehicle_l_SendAllAway(vehicle, &point);
        for (std::uint32_t ring = 1; ring <= static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize); ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < ring; ++step) { MapMigPoint_MoveCursorUnwrapped(point, direction); Vehicle_l_SendAllAway(vehicle, &point); }
        }
    }

    // Original function: FUN_00441484; opcode 5 retains the human's player byte.
    void Vehicle_l_SendAllAway(void* vehicle, const void* point)
    {
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
        {
            if (id == Read<std::int32_t>(vehicle, 0x1DC)) continue;
            const auto* moveable = gMapMoveableArrayStorage.entries + id * 0x58;
            if (Read<std::uint32_t>(moveable, 8) != 1) continue;
            auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CHumanArray::ElementSize;
            if (Human_IsWalking(human) || Human_IsProducingVehicle(human) || Human_IsAttachedToVehicle(human) || Read<std::uint32_t>(human, 0xB48) == Read<std::uint32_t>(vehicle, 4)) continue;
            std::byte command[20]{};
            Write<std::uint8_t>(command, 0, 5);
            Write(command, 1, Read<std::uint8_t>(human, 0x10));
            Write(command, 12, MapMigPoint_GetLong(Vehicle_GetMapPositionRef(vehicle)));
            Write(command, 16, Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize);
            Human_DoAddNewUserCommand(human, command);
        }
    }

    // Original function: FUN_00441618.
    bool Vehicle_GoAway_HitByVehicle(const void* vehicle, const void* point)
    {
        if (Vehicle_l_GoAway_HitByVehicle(vehicle, point, point)) return true;
        auto cursor = Read<std::uint32_t>(point, 0);
        for (std::uint32_t ring = 1; ring < 10; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(cursor, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < ring; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                    if (MapMigPoint_IsInsideMap(&cursor) && Vehicle_l_GoAway_HitByVehicle(vehicle, point, &cursor)) return true;
                }
        }
        return false;
    }

    // Original function: FUN_004416a0; moving vehicles do not occupy an idle footprint.
    bool Vehicle_l_GoAway_HitByVehicle(const void* vehicle, const void* point, const void* searchPoint)
    {
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(searchPoint); id != -1; id = iterator.GetNextMapMoveableId())
        {
            if (id == Read<std::int32_t>(vehicle, 0x1DC)) continue;
            const auto* moveable = gMapMoveableArrayStorage.entries + id * 0x58;
            if (Read<std::uint32_t>(moveable, 8) != 3) continue;
            const auto* other = Vehicle(Read<std::int32_t>(moveable, 0x54));
            if (Read<std::uint32_t>(other, 0x4FC) == 1) continue;
            const auto distance = Distance(static_cast<const std::byte*>(moveable) + 0x12, point);
            if (static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(other)->logicSize) + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize) >= distance) return true;
        }
        return false;
    }

    // Original function: FUN_00441755.
    bool Vehicle_l_GoAway_GetVehicleFreePos(void* vehicle, void* point)
    {
        SVehicleFreePositionSearch data{vehicle, point};
        return CMapPathfinderElement::SearchOverMap(Vehicle_GetMapPositionRef(vehicle), 10, true, slVehicle_SearchVehicleFreePos, &data, true, Read<std::int32_t>(vehicle, 0x1E0));
    }

    // Original function: FUN_004417fc.
    bool VehicleReference_IsAllocated(const void* reference)
    {
        const auto id = Read<std::int32_t>(reference, 0);
        if (id == -1) return false;
        const auto* vehicle = Vehicle(id);
        return Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(vehicle, 8) == Read<std::uint32_t>(reference, 4);
    }

    // Original function: FUN_0044c202.
    void Vehicle_l_Passengers_ResetContents(void* vehicle)
    {
        for (std::uint32_t index = 0; index < 21; ++index) { Write<std::int32_t>(vehicle, 0x1F0u + index * 12u, -1); Write<std::uint8_t>(vehicle, 0x1F8u + index * 12u, 0); }
        Write<std::uint8_t>(vehicle, 0x2EC, 0);
        for (std::uint32_t index = 0; index < 5; ++index) { Write<std::int32_t>(vehicle, 0x2FCu + index * 12u, -1); Write<std::uint8_t>(vehicle, 0x304u + index * 12u, 0); }
    }

    // Original function: FUN_0044c2dd.
    void Vehicle_Passengers_MoveAllOut(void* vehicle, bool includeCommander)
    {
        auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x1F0, index);
            if (HumanReference_IsAllocated(entry) && Vehicle_Passengers_MoveOut_h(vehicle, Read<std::uint32_t>(entry, 0))) Vehicle_Passengers_DetachHuman(vehicle, Read<std::uint32_t>(entry, 0));
        }
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0 && includeCommander)
        {
            const auto id = static_cast<std::uint32_t>(Vehicle_Passengers_GetAttachedCommanderId(vehicle));
            if (Vehicle_Passengers_MoveOut_h(vehicle, id)) Vehicle_Passengers_DetachHuman(vehicle, id);
        }
        count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x2FC, index);
            if (VehicleReference_IsAllocated(entry) && Vehicle_Passengers_MoveOut_v(vehicle, Read<std::uint32_t>(entry, 0))) Vehicle_Passengers_DetachVehicle(vehicle, Read<std::uint32_t>(entry, 0));
        }
    }

    // Original function: FUN_0044c393.
    bool Vehicle_Passengers_IsCommanderInVehicle(const void* vehicle) { return Read<std::uint8_t>(vehicle, 0x2EC) != 0 && Read<std::uint8_t>(Slot(vehicle, 0x1F0, static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots)), 8) != 0; }
    // Original function: FUN_0044c3bc.
    bool Vehicle_Passengers_NeedsDraggingAnimal(const void* vehicle) { return Vehicle_GetVehicleTypeDataPtr(vehicle)->draggingAnimalTribe != 0 && Read<std::uint8_t>(vehicle, 0x2F0) == 0; }

    // Original function: FUN_0044c6c4; the inclusive bound also checks the commander slot.
    bool Vehicle_Passengers_AllPassengersInside(const void* vehicle)
    {
        const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
        if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) return true;
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index <= count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x1F0, index);
            if (HumanReference_IsAllocated(entry) && Read<std::uint8_t>(entry, 8) == 0) return false;
        }
        return Vehicle_Passengers_AllVehiclesInside(vehicle);
    }

    // Original function: FUN_0044ca58; Game.exe encodes OR with a sign-extended imm8 FF.
    bool Vehicle_Passengers_DetachVehicle(void* vehicle, std::uint32_t id)
    {
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto offset = 0x2FCu + index * 12u;
            if (Read<std::uint32_t>(vehicle, offset) != id) continue;
            Write<std::int32_t>(vehicle, offset, -1);
            Write<std::uint8_t>(vehicle, offset + 8, 0);
            Vehicle_Attachment_DetachFromVehicle(Vehicle(static_cast<std::int32_t>(id)));
            return true;
        }
        return false;
    }

    // Original function: FUN_0044cb80; the zero-space fallback door result is ignored.
    bool Vehicle_Passengers_MoveOut_v(void* vehicle, std::uint32_t id)
    {
        if (Read<std::uint32_t>(vehicle, 0x4FC) == 1) return false;
        const auto slot = Vehicle_Passengers_GetAttachedVehicleSlot(vehicle, id);
        if (slot == -1) return false;
        const auto offset = 0x304u + static_cast<std::uint32_t>(slot) * 12u;
        if (Read<std::uint8_t>(vehicle, offset) == 0) return true;
        auto* passenger = Vehicle(static_cast<std::int32_t>(id));
        std::uint32_t point = 0;
        if (!Vehicle_Door_GetEntryPoint(vehicle, &point, static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(passenger)->logicSize))) Vehicle_Door_GetEntryPoint(vehicle, &point, 0);
        Vehicle_Attachment_LeaveVehicle(passenger, &point);
        Write<std::uint8_t>(vehicle, offset, 0);
        return true;
    }

    // Original function: FUN_0044cc83.
    bool Vehicle_Passengers_AllVehiclesInside(const void* vehicle)
    {
        const auto count = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto* entry = Slot(vehicle, 0x2FC, index);
            if (VehicleReference_IsAllocated(entry) && Read<std::uint8_t>(entry, 8) == 0) return false;
        }
        return true;
    }

    // Original function: FUN_0044da95; clear the carrier only when its revision matches.
    void Vehicle_Attachment_DetachFromVehicle(void* vehicle)
    {
        auto* carrier = Vehicle(Read<std::int32_t>(vehicle, 0x540));
        if (Read<std::uint32_t>(carrier, 8) != Read<std::uint32_t>(vehicle, 0x544)) return;
        Vehicle_Passengers_DetachVehicle(carrier, Read<std::uint32_t>(vehicle, 4));
        Write<std::int32_t>(vehicle, 0x540, -1);
    }

    // Original function: FUN_0044db85; start idle before reattaching, as in Windows.
    void Vehicle_Attachment_LeaveVehicle(void* vehicle, const void* point)
    {
        Vehicle_l_StartAtomicIdle(vehicle);
        MapMoveable_Attachment_AttachToMap(Moveable(vehicle), point);
        Write<std::uint8_t>(vehicle, 0x548, 0);
        Vehicle_SetTargetWorkPosition(vehicle, point);
    }

    // Original function: FUN_004623b3.
    bool Vehicle_l_StartAtomicAnimation(void* vehicle, std::uint32_t action, std::int32_t direction)
    {
        if (action == 1 || !Vehicle_l_IsAtomicInterruptable(vehicle)) return false;
        auto job = Read<std::uint32_t>(vehicle, 0x1EC);
        std::uint32_t animation;
        for (;;)
        {
            const auto* data = static_cast<const std::byte*>(gStaticJobTypeData) + job * 0x88u;
            if (Read<std::uint8_t>(data, 0x24u + action) == 0) return false;
            animation = gStaticTribeTypeData[Read<std::uint32_t>(vehicle, 0x1E4)].atomicIds[job][action];
            if (animation != 0xFFFFFFFFu) break;
            job = Read<std::uint32_t>(data, 0x20);
            if (job == 0) return false;
        }
        Write<std::uint32_t>(vehicle, 0x508, 0);
        Write(vehicle, 0x50C, Read<std::uint32_t>(vehicle, 0x50C) + 1u);
        Write(vehicle, 0x504, animation);
        Write(vehicle, 0x4FC, action);
        Write<std::uint8_t>(vehicle, 0x500, 1);
        const auto preferred = gStaticAtomicAnimationData[animation].startDirection;
        if (preferred != -1) MapMoveable_Attachment_StartTurn(Moveable(vehicle), preferred);
        else if (direction != -1) MapMoveable_Attachment_StartTurn(Moveable(vehicle), direction);
        return true;
    }

    // Original function: FUN_00462483; Game.exe's 004F3B24 radius is 60.
    bool Vehicle_l_StartAtomicWalk(void* vehicle, const void* point, bool force)
    {
        if (Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0) == Read<std::uint32_t>(point, 0)) { Vehicle_SetTargetWorkPosition(vehicle, point); return true; }
        if (!Vehicle_l_IsAtomicInterruptable(vehicle)) return false;
        if (!force)
        {
            if (!Vehicle_Passengers_AllPassengersInside(vehicle)) { Write<std::uint32_t>(vehicle, 0x1C8, 3); return false; }
            if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) { Write<std::uint32_t>(vehicle, 0x1C8, 4); return false; }
            if (!Vehicle_Passengers_IsCommanderInVehicle(vehicle) && Read<std::uint32_t>(vehicle, 0x1C8) != 1) return false;
        }
        Vehicle_SetTargetWorkPosition(vehicle, point);
        if (!MapMoveable_Pathfinder_Start(Moveable(vehicle), point, 60, false, true, false)) return false;
        Write<std::uint8_t>(vehicle, 0x500, 0);
        Write<std::uint8_t>(vehicle, 0x1D0, 0);
        Write(vehicle, 0x1D6, Read<std::uint32_t>(point, 0));
        Write<std::uint32_t>(vehicle, 0x4FC, 1);
        return true;
    }

    // Original function: FUN_004625e7.
    bool Vehicle_l_IsAtomicInterruptable(const void* vehicle)
    {
        if (Read<std::uint32_t>(vehicle, 0x4FC) == 1) return Read<std::uint8_t>(Moveable(vehicle), 0x2C) == 0;
        return Read<std::uint8_t>(vehicle, 0x500) == 0 || gStaticAtomicAnimationData[Read<std::uint32_t>(vehicle, 0x504)].interruptable != 0;
    }

    // Original function: FUN_0046263c; failed relocation still returns immediately.
    void Vehicle_l_StartAtomicIdle(void* vehicle)
    {
        if (!Vehicle_Attachment_IsInsideVehicle(vehicle))
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle));
            if ((Read<std::uint8_t>(micro, 12) & 0x80u) != 0 || (Read<std::uint8_t>(micro, 12) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize))
            {
                auto point = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
                std::uint32_t output = 0;
                const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
                if ((type == 3 || type == 4) && !Vehicle_Passengers_IsCommanderInVehicle(vehicle))
                {
                    SVehiclePointSearch data(Vehicle_GetVehicleTypeDataPtr(vehicle), &point, &output);
                    data.minimumDistance = 0;
                    data.sourceDistance = 0;
                    CMapPathfinderElement::SearchOverMap(&point, 20, true, slTool_CheckForVehiclePointNearLand, &data, true, -1);
                    if (output != data.initialPoint) { Vehicle_l_StartAtomicWalk(vehicle, &output, true); return; }
                }
                else if (Tool_Map_GetNotBlockedPosition(&point, true) && Tool_Vehicle_GetNextMovePointNearPosition(type, &point, &output, 20, 0)) { Vehicle_l_StartAtomicWalk(vehicle, &output, true); return; }
            }
            if (Read<std::uint8_t>(vehicle, 0x2EC) != 0 && Vehicle_GoAway_HitByVehicle(vehicle, Vehicle_GetMapPositionRef(vehicle)))
            {
                std::uint32_t output = 0;
                if (Vehicle_l_GoAway_GetVehicleFreePos(vehicle, &output)) { Vehicle_l_StartAtomicWalk(vehicle, &output, false); return; }
            }
        }
        if (Read<std::uint32_t>(vehicle, 0x4FC) != 2) { Vehicle_l_SendAwayRadial(vehicle); Vehicle_l_StartAtomicAnimation(vehicle, 2, -1); }
    }

    // Original function: FUN_0043b066.
    bool Tool_Vehicle_GetNextMovePointNearPosition(std::uint32_t type, const void* source, void* point, std::uint32_t radius, std::uint32_t minimumDistance)
    {
        SVehiclePointSearch data(gStaticVehicleTypeStorage.records + type, source, point);
        if (minimumDistance > data.bestDistance) data.bestDistance = 0xFFFFFFFFu;
        data.minimumDistance = minimumDistance;
        data.sourceDistance = 0;
        CMapPathfinderElement::SearchOverMap(source, radius, true, slTool_CheckForVehiclePoint, &data, true, -1);
        return Read<std::uint32_t>(point, 0) != data.initialPoint;
    }
}
