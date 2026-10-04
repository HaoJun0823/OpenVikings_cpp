#include "C2MapMoveableAccess.h"
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
        if (!MapMoveable_Init(gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u, id, type, objectId))
        {
            MapMoveableArray_FreeMapMoveable(array, id);
            return -1;
        }
        return id;
    }

    // Original function: FUN_0040bd00. Element_Free leaves the allocation flag intact.
    void MapMoveableArray_FreeMapMoveable(CArrayHandler* array, std::int32_t id)
    {
        void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
        if (Read<std::uint8_t>(moveable, 0) != 0 && array->Element_Free(id, true)) MapMoveable_Exit(moveable);
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
        std::memset(moveable, 0, 0x58);
        Write<std::int32_t>(moveable, 0x24, -1);
        Write<std::int32_t>(moveable, 4, id);
        Write<std::uint8_t>(moveable, 0, 1);
        Write<std::uint32_t>(moveable, 8, type);
        Write<std::uint32_t>(moveable, 0x20, 2);
        const auto pathfinderId = MapPathfinderManager_AllocatePathfinder(static_cast<CArrayHandler*>(gMapPathfinderManager));
        Write<std::int32_t>(moveable, 0x28, pathfinderId);
        if (pathfinderId == -1)
        {
            MapMoveable_l_Misc_FreeAllAllocatedData(moveable);
            return false;
        }
        if (type >= 1u && type <= 4u) Write<std::int32_t>(moveable, 0x54, objectId);
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
        const auto id = Read<std::int32_t>(moveable, 0x28);
        if (id == -1) return;
        MapPathfinderManager_FreePathfinder(static_cast<CArrayHandler*>(gMapPathfinderManager), id);
        Write<std::int32_t>(moveable, 0x28, -1);
    }

    // Original function: FUN_00438ebe. The source point is copied after detachment.
    void MapMoveable_Attachment_AttachToMap(void* moveable, const void* packedMigPoint)
    {
        if (Read<std::uint8_t>(moveable, 0x10) != 0) MapMoveable_Attachment_DetachFromMap(moveable);
        const auto point = Read<std::uint32_t>(packedMigPoint, 0);
        Write<std::int32_t>(moveable, 0x24, -1);
        Write<std::uint32_t>(moveable, 0x12, point);
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

    // Original function: FUN_00439021.
    const void* MapMoveable_Pathfinder_GetPathfinderPtr(const void* moveable)
    {
        return GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
    }

    // Original function: FUN_004394ad. The micro-point stores a signed 16-bit list head.
    void MapMoveable_l_Attachment_AttachToGameMap(void* moveable)
    {
        void* point = GameMap_GetMapMicroPointPtr(static_cast<const std::byte*>(moveable) + 0x12);
        const auto id = Read<std::uint32_t>(moveable, 4);
        const auto previous = Read<std::int16_t>(point, 0x0A);
        Write<std::uint16_t>(point, 0x0A, static_cast<std::uint16_t>(id));
        Write<std::int32_t>(moveable, 0x24, previous);
    }

    // Original function: FUN_004394ca. An absent micro-list entry is ignored.
    void MapMoveable_l_Attachment_DetachFromGameMap(void* moveable)
    {
        void* point = GameMap_GetMapMicroPointPtr(static_cast<const std::byte*>(moveable) + 0x12);
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
            void* previous = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(current) * 0x58u;
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
        const SSectorGridCursor sector(Read<std::uint32_t>(moveable, 0x12));
        Write<SSectorGridCursor>(moveable, 0x18, sector);
        const auto id = Read<std::uint32_t>(moveable, 0x54);
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
        const auto id = Read<std::uint32_t>(moveable, 0x54);
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
        std::memset(pathfinder, 0, 0x84);
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
        const bool special = Read<std::uint32_t>(animal, 0x18) == 0x31 && Read<std::uint8_t>(animal, 0x5C) != 0;
        const auto offset = (Read<std::uint32_t>(animal, 0x10) << 6) + (special ? 0x38u : 0x30u);
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
        const bool special = Read<std::uint32_t>(animal, 0x18) == 0x31 && Read<std::uint8_t>(animal, 0x5C) != 0;
        const auto offset = (Read<std::uint32_t>(animal, 0x10) << 6) + (special ? 0x38u : 0x30u);
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
        const auto offset = (Read<std::uint32_t>(vehicle, 0x1E0) << 6) + (Read<std::uint32_t>(vehicle, 0x1EC) == 0x36 ? 0x20u : 8u);
        const auto previous = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(vehicle, 4);
        Write<std::int32_t>(data, offset, id);
        Write<std::int32_t>(vehicle, 0x514, previous);
        if (previous != -1) Write<std::int32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(previous) * CVehicleArray::ElementSize, 0x510, id);
        Write<std::int32_t>(vehicle, 0x510, -1);
        Write<std::uint32_t>(data, offset + 4, Read<std::uint32_t>(data, offset + 4) + 1u);
    }

    // Original function: FUN_00437689. The original traversal assumes membership in this list.
    void Vehicle_Sector_DetachVehicleFromSector(void* vehicle, const SSectorGridCursor& sector)
    {
        auto* data = GetSector(sector);
        const auto offset = (Read<std::uint32_t>(vehicle, 0x1E0) << 6) + (Read<std::uint32_t>(vehicle, 0x1EC) == 0x36 ? 0x20u : 8u);
        auto current = Read<std::int32_t>(data, offset);
        const auto id = Read<std::int32_t>(vehicle, 4);
        const auto next = Read<std::int32_t>(vehicle, 0x514);
        if (current == id)
        {
            Write<std::int32_t>(data, offset, next);
            if (next != -1) Write<std::int32_t>(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(next) * CVehicleArray::ElementSize, 0x510, -1);
        }
        else
        {
            for (;;)
            {
                void* previous = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(current) * CVehicleArray::ElementSize;
                current = Read<std::int32_t>(previous, 0x514);
                if (current == id)
                {
                    Write<std::int32_t>(previous, 0x514, next);
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
        const auto offset = (Read<std::uint32_t>(human, 0x10) << 6) + (Human_IsJobSoldier(human) || Human_IsJobHero(human) ? 0x18u : 0u);
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
        const auto offset = (Read<std::uint32_t>(human, 0x10) << 6) + (Human_IsJobSoldier(human) || Human_IsJobHero(human) ? 0x18u : 0u);
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
}
