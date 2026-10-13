#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "C2MapMoveableAccess.h"
#include "C2AnimalAccess.h"
#include "C2AnimalTasks.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanTaskAccess.h"
#include "C2HumanNeeds.h"
#include "C2HumanWalking.h"
#include "C2VehicleAccess.h"
#include "C2VehicleSimulation.h"
#include "C2MissionStartObjects.h"
#include "C2MapGeometry.h"
#include "C2MapPathfinderSearch.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapSectorAttachment.h"
#include "C2MapSectorGridCursor.h"

#include <cstddef>
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_0047dbcb; callers pass a CMapMoveable receiver.
    const void* MapMoveable_GetMapPositionRef(const void* moveable) { return static_cast<const std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition; }

    namespace
    {
        std::byte* GetSector(const SSectorGridCursor& sector)
        {
            auto* manager = CGameMapSectorAttachmentManager::sTheObjectPtr;
            const auto index = static_cast<std::uint32_t>(sector.y) * static_cast<std::uint32_t>(manager->GetGridWidth()) + static_cast<std::uint32_t>(sector.x);
            return manager->GetSectorData() + index * 0x540u;
        }
    }

    // Original function: FUN_0040bcbf.
    std::int32_t MapMoveableArray_AllocateMapMoveable(CArrayHandler* array, std::uint32_t type, std::int32_t objectId)
    {
        const auto id = array->Element_Allocate();
        if (id == -1) return -1;
        if (!MapMoveable_Init(gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable, id, type, objectId))
        {
            MapMoveableArray_FreeMapMoveable(array, id);
            return -1;
        }
        return id;
    }

    // Original function: FUN_0040bd00. Element_Free leaves the allocation flag intact.
    void MapMoveableArray_FreeMapMoveable(CArrayHandler* array, std::int32_t id)
    {
        void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable;
        if (Read<std::uint8_t>(moveable, 0) != 0 && array->Element_Free(id, true)) MapMoveable_Exit(moveable);
    }

    // Original function: FUN_0040BD33; deletion follows each individual tick.
    void __fastcall MapMoveableArray_DoOneGameTick(void* array, void* /*unusedEdx*/)
    {
        auto* handler = static_cast<CArrayHandler*>(array);
        for (auto id = handler->mFirstScanIndex; id <= handler->mLastScanIndex; ++id)
        {
            void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable;
            if (Read<std::uint8_t>(moveable, 0) == 0) continue;
            MapMoveable_DoOneGameTick(moveable);
            if (Read<std::uint8_t>(moveable, 0x0C) != 0) Logic_EntryPoint_FreeMapMoveable(static_cast<std::uint32_t>(id));
        }
    }

    // Original function: FUN_0043902B.
    void MapMoveable_DoOneGameTick(void* moveable)
    {
        const auto id = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId);
        switch (Read<std::uint32_t>(moveable, 8))
        {
        case SwitchIds::MapMoveableType::Human: MapMoveable_DoOneGameTick_Human(moveable, CHumanArray::mStaticVars + id * CHumanArray::ElementSize); return;
        case SwitchIds::MapMoveableType::Animal: MapMoveable_DoOneGameTick_Animal(moveable, CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize); return;
        case SwitchIds::MapMoveableType::Vehicle: MapMoveable_DoOneGameTick_Vehicle(moveable, CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize); return;
        }
    }

    // Original function: FUN_00439079; preserve its distinct native boundary.
    void MapMoveable_DoOneGameTick_Human(void* moveable, void* creature)
    {
        Human_AI_DoUpdateConditions(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            if (Read<std::uint8_t>(moveable, 0x39) != 0 && !MapMoveable_Attachment_PerformTurn(moveable)) return;
            const auto id = Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId);
            if (id != -1)
            {
                auto* pathfinder = GetUnsafePathfinderPtr(id);
                if (Read<std::uint8_t>(moveable, 0x2C) != 0)
                {
                    Write(moveable, 0x34, Read<std::uint32_t>(moveable, 0x34) + Read<std::uint32_t>(moveable, 0x48));
                    if (Read<std::uint8_t>(moveable, 0x38) == 0 && Read<std::uint32_t>(moveable, 0x34) >= 5000u)
                    {
                        Write<std::uint8_t>(moveable, 0x38, 1);
                        MapMoveable_l_Attachment_DetachFromGameMap(moveable);
                        auto point = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition);
                        MapMigPoint_MoveCursorUnwrapped(point, Read<std::uint32_t>(moveable, 0x30));
                        Write(moveable, NC2Layout::MapMoveableOffset::MapPosition, point);
                        MapMoveable_l_Attachment_AttachToGameMap(moveable);
                        const SSectorGridCursor sector(Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition));
                        const auto previous = Read<SSectorGridCursor>(moveable, 0x18);
                        if (sector.x != previous.x || sector.y != previous.y)
                        {
                            MapMoveable_Attachment_DetachFromSectorMap(moveable);
                            MapMoveable_Attachment_AttachToSectorMap(moveable);
                        }
                    }
                    if (Read<std::uint32_t>(moveable, 0x34) >= 10000u) Write<std::uint8_t>(moveable, 0x2C, 0);
                    return;
                }
                auto direction = MapPathfinderElement_GetNextDirection(pathfinder);
                if (direction == -4) return;
                if (direction >= 0)
                {
                    if (!Human_AI_DoNewMapPositionReached(creature, false))
                    {
                        MapMoveable_Pathfinder_Stop(moveable);
                        direction = -3;
                    }
                    if (direction != -3)
                    {
                        const auto turning = Read<std::uint32_t>(moveable, 0x4C);
                        const auto speed = Read<std::uint32_t>(moveable, 0x48);
                        Write<std::uint8_t>(moveable, 0x2C, 1);
                        Write(moveable, 0x30, direction);
                        Write(moveable, 0x34, speed);
                        Write<std::uint8_t>(moveable, 0x38, 0);
                        if (turning == 0) Write(moveable, 0x20, direction);
                        else if (direction != Read<std::int32_t>(moveable, 0x20))
                        {
                            Write<std::uint8_t>(moveable, 0x39, 1);
                            Write<std::uint32_t>(moveable, 0x3C, 0);
                            Write(moveable, 0x40, direction);
                        }
                    }
                }
                else if (direction == -2) Human_AI_DoNewMapPositionReached(creature, true);
                else if (direction == -1) Human_AI_DoPathfinderFailed(creature);
                else if (direction == -3) Human_AI_DoPathfinderStopped(creature);
            }
        }
        Human_AI_DoUpdateAI(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0) Tool_Map_CheckUserInformationAboutEnemies(Read<std::uint32_t>(creature, 0x10), static_cast<std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition);
        GameAssistantSingleHumanData_CommandHistory_Validate(static_cast<std::byte*>(creature) + 0xB88);
    }

    // Original function: FUN_004391EB; preserve its distinct native boundary.
    void MapMoveable_DoOneGameTick_Animal(void* moveable, void* creature)
    {
        Animal_DoUpdateConditions(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            if (Read<std::uint8_t>(moveable, 0x39) != 0 && !MapMoveable_Attachment_PerformTurn(moveable)) return;
            const auto id = Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId);
            if (id != -1)
            {
                auto* pathfinder = GetUnsafePathfinderPtr(id);
                if (Read<std::uint8_t>(moveable, 0x2C) != 0)
                {
                    Write(moveable, 0x34, Read<std::uint32_t>(moveable, 0x34) + Read<std::uint32_t>(moveable, 0x48));
                    if (Read<std::uint8_t>(moveable, 0x38) == 0 && Read<std::uint32_t>(moveable, 0x34) >= 5000u)
                    {
                        Write<std::uint8_t>(moveable, 0x38, 1);
                        MapMoveable_l_Attachment_DetachFromGameMap(moveable);
                        auto point = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition);
                        MapMigPoint_MoveCursorUnwrapped(point, Read<std::uint32_t>(moveable, 0x30));
                        Write(moveable, NC2Layout::MapMoveableOffset::MapPosition, point);
                        MapMoveable_l_Attachment_AttachToGameMap(moveable);
                        const SSectorGridCursor sector(Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition));
                        const auto previous = Read<SSectorGridCursor>(moveable, 0x18);
                        if (sector.x != previous.x || sector.y != previous.y)
                        {
                            MapMoveable_Attachment_DetachFromSectorMap(moveable);
                            MapMoveable_Attachment_AttachToSectorMap(moveable);
                        }
                    }
                    if (Read<std::uint32_t>(moveable, 0x34) >= 10000u) Write<std::uint8_t>(moveable, 0x2C, 0);
                    return;
                }
                auto direction = MapPathfinderElement_GetNextDirection(pathfinder);
                if (direction == -4) return;
                if (direction >= 0)
                {
                    if (!Animal_DoNewMapPositionReached(creature, false))
                    {
                        MapMoveable_Pathfinder_Stop(moveable);
                        direction = -3;
                    }
                    if (direction != -3)
                    {
                        const auto turning = Read<std::uint32_t>(moveable, 0x4C);
                        const auto speed = Read<std::uint32_t>(moveable, 0x48);
                        Write<std::uint8_t>(moveable, 0x2C, 1);
                        Write(moveable, 0x30, direction);
                        Write(moveable, 0x34, speed);
                        Write<std::uint8_t>(moveable, 0x38, 0);
                        if (turning == 0) Write(moveable, 0x20, direction);
                        else if (direction != Read<std::int32_t>(moveable, 0x20))
                        {
                            Write<std::uint8_t>(moveable, 0x39, 1);
                            Write<std::uint32_t>(moveable, 0x3C, 0);
                            Write(moveable, 0x40, direction);
                        }
                    }
                }
                else if (direction == -2) Animal_DoNewMapPositionReached(creature, true);
                else if (direction == -1) Animal_DoPathfinderFailed(creature);
                else if (direction == -3) Animal_DoPathfinderStopped(creature);
            }
        }
        Animal_DoUpdateAI(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0) Tool_Map_CheckUserInformationAboutEnemies(Read<std::uint32_t>(creature, 0x10), static_cast<std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition);
    }

    // Original function: FUN_00439352; preserve its distinct native boundary.
    void MapMoveable_DoOneGameTick_Vehicle(void* moveable, void* creature)
    {
        Vehicle_DoUpdateConditions(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            if (Read<std::uint8_t>(moveable, 0x39) != 0 && !MapMoveable_Attachment_PerformTurn(moveable)) return;
            const auto id = Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId);
            if (id != -1)
            {
                auto* pathfinder = GetUnsafePathfinderPtr(id);
                if (Read<std::uint8_t>(moveable, 0x2C) != 0)
                {
                    Write(moveable, 0x34, Read<std::uint32_t>(moveable, 0x34) + Read<std::uint32_t>(moveable, 0x48));
                    if (Read<std::uint8_t>(moveable, 0x38) == 0 && Read<std::uint32_t>(moveable, 0x34) >= 5000u)
                    {
                        Write<std::uint8_t>(moveable, 0x38, 1);
                        MapMoveable_l_Attachment_DetachFromGameMap(moveable);
                        auto point = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition);
                        MapMigPoint_MoveCursorUnwrapped(point, Read<std::uint32_t>(moveable, 0x30));
                        Write(moveable, NC2Layout::MapMoveableOffset::MapPosition, point);
                        MapMoveable_l_Attachment_AttachToGameMap(moveable);
                        const SSectorGridCursor sector(Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition));
                        const auto previous = Read<SSectorGridCursor>(moveable, 0x18);
                        if (sector.x != previous.x || sector.y != previous.y)
                        {
                            MapMoveable_Attachment_DetachFromSectorMap(moveable);
                            MapMoveable_Attachment_AttachToSectorMap(moveable);
                        }
                    }
                    if (Read<std::uint32_t>(moveable, 0x34) >= 10000u) Write<std::uint8_t>(moveable, 0x2C, 0);
                    return;
                }
                auto direction = MapPathfinderElement_GetNextDirection(pathfinder);
                if (direction == -4) return;
                if (direction >= 0)
                {
                    if (!Vehicle_DoNewMapPositionReached(creature, false))
                    {
                        MapMoveable_Pathfinder_Stop(moveable);
                        direction = -3;
                    }
                    if (direction != -3)
                    {
                        const auto turning = Read<std::uint32_t>(moveable, 0x4C);
                        const auto speed = Read<std::uint32_t>(moveable, 0x48);
                        Write<std::uint8_t>(moveable, 0x2C, 1);
                        Write(moveable, 0x30, direction);
                        Write(moveable, 0x34, speed);
                        Write<std::uint8_t>(moveable, 0x38, 0);
                        if (turning == 0) Write(moveable, 0x20, direction);
                        else if (direction != Read<std::int32_t>(moveable, 0x20))
                        {
                            Write<std::uint8_t>(moveable, 0x39, 1);
                            Write<std::uint32_t>(moveable, 0x3C, 0);
                            Write(moveable, 0x40, direction);
                        }
                    }
                }
                else if (direction == -2) Vehicle_DoNewMapPositionReached(creature, true);
                else if (direction == -1) Vehicle_DoPathfinderFailed(creature);
            }
        }
        Vehicle_DoUpdateAI(creature);
        if (Read<std::uint8_t>(moveable, 0x10) != 0) Tool_Map_CheckUserInformationAboutEnemies(Read<std::uint32_t>(creature, 0x1E0), static_cast<std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition);
    }

    // Original function: FUN_0041cef8.
    std::int32_t MapPathfinderManager_AllocatePathfinder(CArrayHandler* manager)
    {
        const auto id = manager->Element_Allocate();
        if (id != -1) MapPathfinderElement_Init(GetUnsafePathfinderPtr(id));
        return id;
    }

    // Original function: FUN_0041cf1e. Windows shares the one-byte exit body with FUN_00430ca4.
    void MapPathfinderManager_FreePathfinder(CArrayHandler* manager, std::int32_t id)
    {
        void* pathfinder = GetUnsafePathfinderPtr(id);
        if (Read<std::uint8_t>(pathfinder, 0) != 0 && manager->Element_Free(id, true)) Write<std::uint8_t>(pathfinder, 0, 0);
    }

    // Original function: FUN_00438e09.
    bool MapMoveable_Init(void* moveable, std::int32_t id, std::uint32_t type, std::int32_t objectId)
    {
        if (Read<std::uint8_t>(moveable, 0) != 0) return false;
        std::memset(moveable, 0, 88);
        Write<std::int32_t>(moveable, 0x24, -1);
        Write<std::int32_t>(moveable, 4, id);
        Write<std::uint8_t>(moveable, 0, 1);
        Write<std::uint32_t>(moveable, 8, type);
        Write<std::uint32_t>(moveable, 0x20, 2);
        const auto pathfinderId = MapPathfinderManager_AllocatePathfinder(static_cast<CArrayHandler*>(gMapPathfinderManager));
        Write<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId, pathfinderId);
        if (pathfinderId == -1)
        {
            MapMoveable_l_Misc_FreeAllAllocatedData(moveable);
            return false;
        }
        if (type >= 1u && type <= 4u) Write<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId, objectId);
        return true;
    }

    // Original function: FUN_00438e7d.
    void MapMoveable_Exit(void* moveable)
    {
        if (Read<std::uint8_t>(moveable, 0) == 0) return;
        if (Read<std::uint8_t>(moveable, 0x10) != 0) MapMoveable_Attachment_DetachFromMap(moveable);
        MapMoveable_l_Misc_FreeAllAllocatedData(moveable);
        Write<std::uint8_t>(moveable, 0, 0);
    }

    // Original function: FUN_00438e9c.
    void MapMoveable_l_Misc_FreeAllAllocatedData(void* moveable)
    {
        if (Read<std::uint8_t>(moveable, 0) == 0) return;
        const auto id = Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId);
        if (id == -1) return;
        MapPathfinderManager_FreePathfinder(static_cast<CArrayHandler*>(gMapPathfinderManager), id);
        Write<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId, -1);
    }

    // Original function: FUN_00438ebe. The source point is copied after detachment.
    void MapMoveable_Attachment_AttachToMap(void* moveable, const void* packedMigPoint)
    {
        if (Read<std::uint8_t>(moveable, 0x10) != 0) MapMoveable_Attachment_DetachFromMap(moveable);
        const auto point = Read<std::uint32_t>(packedMigPoint, 0);
        Write<std::int32_t>(moveable, 0x24, -1);
        Write<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition, point);
        Write<std::uint8_t>(moveable, 0x10, 1);
        MapMoveable_l_Attachment_AttachToGameMap(moveable);
        MapMoveable_Attachment_AttachToSectorMap(moveable);
    }

    // Original function: FUN_00438eef.
    void MapMoveable_Attachment_DetachFromMap(void* moveable)
    {
        if (Read<std::uint8_t>(moveable, 0x10) == 0) return;
        MapMoveable_l_Attachment_DetachFromGameMap(moveable);
        MapMoveable_Attachment_DetachFromSectorMap(moveable);
        Write<std::uint8_t>(moveable, 0x10, 0);
    }

    // Original function: FUN_00438F6A. An unrelocated walking object predicts its next point.
    void MapMoveable_Attachment_GetNextMapPosition(const void* moveable, void* point)
    {
        auto packed = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition);
        Write(point, 0, packed);
        if (Read<std::uint8_t>(moveable, 0x2C) != 0 && Read<std::uint8_t>(moveable, 0x38) == 0)
        {
            MapMigPoint_MoveCursorUnwrapped(packed, Read<std::uint32_t>(moveable, 0x30));
            Write(point, 0, packed);
        }
    }

    // Original function: FUN_00438F8C.
    bool MapMoveable_Pathfinder_Start(void* moveable, const void* destination, std::uint32_t radius, bool useResistance, bool useSectors, bool useGuides)
    {
        if (Read<std::uint8_t>(moveable, 0x2C) != 0) return false;
        MapPathfinderElement_StartPathfinder(GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId)), static_cast<std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition, destination, radius, useResistance, useSectors, useGuides, false);
        return true;
    }

    // Original function: FUN_00438FC4.
    bool MapMoveable_Pathfinder_Start(void* moveable, std::int32_t targetId, std::int32_t distance, std::uint32_t radius, bool useResistance, bool useSectors, bool useGuides)
    {
        if (Read<std::uint8_t>(moveable, 0x2C) != 0) return false;
        MapPathfinderElement_StartPathfinder(GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId)), static_cast<std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition, targetId, distance, radius, useResistance, useSectors, useGuides, false);
        return true;
    }

    // Original function: FUN_00438FFF.
    bool MapMoveable_Pathfinder_Stop(void* moveable)
    {
        const auto id = Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId);
        if (id == -1 || Read<std::uint8_t>(moveable, 0x2C) != 0) return false;
        MapPathfinderElement_StopPathfinder(GetUnsafePathfinderPtr(id));
        return true;
    }

    // Original function: FUN_0043961A. Revisions belong to the creature/marker, not its map slot.
    std::uint32_t MapMoveable_GetUniqueId(const void* moveable)
    {
        const auto id = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId);
        switch (Read<std::uint32_t>(moveable, 8))
        {
        case SwitchIds::MapMoveableType::Human: return Read<std::uint32_t>(CHumanArray::mStaticVars + id * CHumanArray::ElementSize, NC2Layout::HumanOffset::UniqueId);
        case SwitchIds::MapMoveableType::Animal: return Read<std::uint32_t>(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize, 8);
        case SwitchIds::MapMoveableType::Vehicle: return Read<std::uint32_t>(CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize, NC2Layout::VehicleOffset::UniqueId);
        case SwitchIds::MapMoveableType::Marker: return Read<std::uint32_t>(static_cast<const std::byte*>(gMarkerEntries) + id * 0x18u, 8);
        default: return NXBasics::IntegerRange::UInt32Maximum;
        }
    }

    // Original function: FUN_00439021.
    const void* MapMoveable_Pathfinder_GetPathfinderPtr(const void* moveable)
    {
        return GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, NC2Layout::MapMoveableOffset::PathfinderId));
    }

    // Original function: FUN_004394ad. The micro-point stores a signed 16-bit list head.
    void MapMoveable_l_Attachment_AttachToGameMap(void* moveable)
    {
        void* point = GameMap_GetMapMicroPointPtr(static_cast<const std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition);
        const auto id = Read<std::uint32_t>(moveable, 4);
        const auto previous = Read<std::int16_t>(point, 0x0A);
        Write<std::uint16_t>(point, 0x0A, static_cast<std::uint16_t>(id));
        Write<std::int32_t>(moveable, 0x24, previous);
    }

    // Original function: FUN_004394ca. An absent micro-list entry is ignored.
    void MapMoveable_l_Attachment_DetachFromGameMap(void* moveable)
    {
        void* point = GameMap_GetMapMicroPointPtr(static_cast<const std::byte*>(moveable) + NC2Layout::MapMoveableOffset::MapPosition);
        auto current = static_cast<std::int32_t>(Read<std::int16_t>(point, 0x0A));
        if (current == -1) return;
        const auto id = Read<std::int32_t>(moveable, 4);
        if (current == id)
        {
            Write<std::uint16_t>(point, 0x0A, static_cast<std::uint16_t>(Read<std::uint32_t>(moveable, 0x24)));
            return;
        }
        while (current != -1)
        {
            void* previous = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(current) * NC2Layout::RecordSize::MapMoveable;
            current = Read<std::int32_t>(previous, 0x24);
            if (current == id)
            {
                Write<std::int32_t>(previous, 0x24, Read<std::int32_t>(moveable, 0x24));
                return;
            }
        }
    }

    // Original function: FUN_00439516. Type 4 has no sector-list attachment.
    void MapMoveable_Attachment_AttachToSectorMap(void* moveable)
    {
        const auto type = Read<std::uint32_t>(moveable, 8);
        if (type < 1u || type > 3u) return;
        const SSectorGridCursor sector(Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::MapPosition));
        Write<SSectorGridCursor>(moveable, 0x18, sector);
        const auto id = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId);
        if (type == 1) Human_Sector_AttachHumanToSector(CHumanArray::mStaticVars + id * CHumanArray::ElementSize, sector);
        else if (type == 2) Animal_Sector_AttachAnimalToSector(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize, sector);
        else Vehicle_Sector_AttachVehicleToSector(CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize, sector);
    }

    // Original function: FUN_004395c3. Detachment uses the stored sector coordinates.
    void MapMoveable_Attachment_DetachFromSectorMap(void* moveable)
    {
        const auto type = Read<std::uint32_t>(moveable, 8);
        if (type < 1u || type > 3u) return;
        const auto sector = Read<SSectorGridCursor>(moveable, 0x18);
        const auto id = Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId);
        if (type == 1) Human_Sector_DetachHumanFromSector(CHumanArray::mStaticVars + id * CHumanArray::ElementSize, sector);
        else if (type == 2) Animal_Sector_DetachAnimalFromSector(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize, sector);
        else Vehicle_Sector_DetachVehicleFromSector(CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize, sector);
    }

    // Original function: FUN_0044186d. No ID or allocation check is performed.
    void* GetUnsafePathfinderPtr(std::int32_t id)
    {
        return static_cast<std::byte*>(gMapPathfinderEntries) + static_cast<std::uint32_t>(id) * 0x84u;
    }

    // Original function: FUN_00447804.
    void MapPathfinderElement_Init(void* pathfinder)
    {
        std::memset(pathfinder, 0, 132);
        Write<std::uint32_t>(pathfinder, 0x10, 0);
        Write<std::uint32_t>(pathfinder, 4, 0);
        Write<std::int32_t>(pathfinder, 0x2C, -1);
        Write<std::int32_t>(pathfinder, 0x1C, -1);
        Write<std::int32_t>(pathfinder, 0x24, -1);
        Write<std::uint8_t>(pathfinder, 0, 1);
        Write<std::uint32_t>(pathfinder, 0x0C, 5);
    }

    // Original function: FUN_0042ea28. Animals update the count before linking neighbours.
    void Animal_Sector_AttachAnimalToSector(void* animal, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const bool special = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 0x31 && Read<std::uint8_t>(animal, 0x5C) != 0;
        const auto offset = (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId) << 6) + (special ? 0x38u : 0x30u);
        const auto previous = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(animal, 4);
        Write<std::int32_t>(data, offset, id);
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) + 1u);
        Write<std::int32_t>(animal, 0x88, previous);
        if (previous != -1) Write<std::int32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(previous) * CAnimalArray::ElementSize, 0x84, id);
        Write<std::int32_t>(animal, 0x84, -1);
    }

    // Original function: FUN_0042eac4. Windows does not repair the next node's previous ID on non-head removal.
    void Animal_Sector_DetachAnimalFromSector(void* animal, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const bool special = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 0x31 && Read<std::uint8_t>(animal, 0x5C) != 0;
        const auto offset = (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId) << 6) + (special ? 0x38u : 0x30u);
        auto current = Read<std::int32_t>(data, offset);
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) - 1u);
        const auto id = Read<std::int32_t>(animal, 4);
        const auto next = Read<std::int32_t>(animal, 0x88);
        if (current == id)
        {
            Write<std::int32_t>(data, offset, next);
            if (next != -1) Write<std::int32_t>(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(next) * CAnimalArray::ElementSize, 0x84, -1);
            return;
        }
        for (;;)
        {
            void* previous = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(current) * CAnimalArray::ElementSize;
            current = Read<std::int32_t>(previous, 0x88);
            if (current == id)
            {
                Write<std::int32_t>(previous, 0x88, next);
                return;
            }
        }
    }

    // Original function: FUN_004375b9.
    void Vehicle_Sector_AttachVehicleToSector(void* vehicle, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const auto offset = (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId) << 6) + (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::JobType) == 0x36 ? 0x20u : 8u);
        const auto previous = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId);
        Write<std::int32_t>(data, offset, id);
        Write<std::int32_t>(vehicle, NC2Layout::VehicleOffset::NextSectorVehicleId, previous);
        if (previous != -1) Write<std::int32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(previous) * CVehicleArray::ElementSize, NC2Layout::VehicleOffset::PreviousSectorVehicleId, id);
        Write<std::int32_t>(vehicle, NC2Layout::VehicleOffset::PreviousSectorVehicleId, -1);
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) + 1u);
    }

    // Original function: FUN_00437689. The original traversal assumes membership in this list.
    void Vehicle_Sector_DetachVehicleFromSector(void* vehicle, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const auto offset = (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId) << 6) + (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::JobType) == 0x36 ? 0x20u : 8u);
        auto current = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId);
        const auto next = Read<std::int32_t>(vehicle, NC2Layout::VehicleOffset::NextSectorVehicleId);
        if (current == id)
        {
            Write<std::int32_t>(data, offset, next);
            if (next != -1) Write<std::int32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(next) * CVehicleArray::ElementSize, NC2Layout::VehicleOffset::PreviousSectorVehicleId, -1);
        }
        else
        {
            for (;;)
            {
                void* previous = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(current) * CVehicleArray::ElementSize;
                current = Read<std::int32_t>(previous, NC2Layout::VehicleOffset::NextSectorVehicleId);
                if (current == id)
                {
                    Write<std::int32_t>(previous, NC2Layout::VehicleOffset::NextSectorVehicleId, next);
                    break;
                }
            }
        }
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) - 1u);
    }

    // Original function: FUN_00441ac3. Soldiers and heroes share the military list.
    void Human_Sector_AttachHumanToSector(void* human, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const auto offset = (Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId) << 6) + (Human_IsJobSoldier(human) || Human_IsJobHero(human) ? 0x18u : 0u);
        const auto previous = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(human, 4);
        Write<std::int32_t>(data, offset, id);
        Write<std::int32_t>(human, 0xB58, previous);
        if (previous != -1) Write<std::int32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(previous) * CHumanArray::ElementSize, 0xB54, id);
        Write<std::int32_t>(human, 0xB54, -1);
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) + 1u);
    }

    // Original function: FUN_00441b9a. Non-head removal only updates the predecessor's next ID.
    void Human_Sector_DetachHumanFromSector(void* human, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const auto offset = (Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId) << 6) + (Human_IsJobSoldier(human) || Human_IsJobHero(human) ? 0x18u : 0u);
        auto current = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(human, 4);
        const auto next = Read<std::int32_t>(human, 0xB58);
        if (current == id)
        {
            Write<std::int32_t>(data, offset, next);
            if (next != -1) Write<std::int32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(next) * CHumanArray::ElementSize, 0xB54, -1);
        }
        else
        {
            for (;;)
            {
                void* previous = CHumanArray::mStaticVars + static_cast<std::uint32_t>(current) * CHumanArray::ElementSize;
                current = Read<std::int32_t>(previous, 0xB58);
                if (current == id)
                {
                    Write<std::int32_t>(previous, 0xB58, next);
                    break;
                }
            }
        }
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) - 1u);
    }
    // Original function: FUN_00438f0a.
    void MapMoveable_Attachment_StartTurn(void* moveable, std::int32_t direction)
    {
        if (Read<std::uint32_t>(moveable, 0x4C) == 0) Write(moveable, 0x20, direction);
        else if (Read<std::int32_t>(moveable, 0x20) != direction)
        {
            Write<std::uint32_t>(moveable, 0x3C, 0);
            Write<std::uint8_t>(moveable, 0x39, 1);
            Write(moveable, 0x40, direction);
        }
    }

    // Original function: FUN_00438f2c; Mac Attachment_PerformTurn.
    bool MapMoveable_Attachment_PerformTurn(void* moveable)
    {
        if (Read<std::uint8_t>(moveable, 0x39) == 0) return true;
        const auto elapsed = Read<std::uint32_t>(moveable, 0x3C) + 1u;
        Write(moveable, 0x3C, elapsed);
        if (NXBasics::Runtime_TimeSigned32(elapsed) < Read<std::int32_t>(moveable, 0x4C)) return false;
        Write<std::uint32_t>(moveable, 0x3C, 0);
        const auto direction = VE_HexagonDirection_NextDirection(Read<std::uint32_t>(moveable, 0x20), Read<std::uint32_t>(moveable, 0x40), Read<std::uint8_t>(moveable, 0x50) != 0);
        Write(moveable, 0x20, direction);
        if (direction != Read<std::uint32_t>(moveable, 0x40)) return false;
        Write<std::uint8_t>(moveable, 0x39, 0);
        return true;
    }
}
