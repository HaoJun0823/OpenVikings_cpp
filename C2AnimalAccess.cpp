#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2AnimalAccess.h"
#include "C2GameControlNetwork.h"
#include "C2LogicObjectArrays.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2AnimalIterator.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanInitialization.h"
#include "C2HumanWalking.h"
#include "C2InteractionTargetAccess.h"
#include "C2MapGeometry.h"
#include "C2HouseAccess.h"
#include "C2MapSectorAttachment.h"
#include "C2MapSectorGridCursor.h"
#include "C2OverviewObjectIterators.h"
#include "C2MapMoveableAccess.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2StaticTribeTypeData.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Animal(std::int32_t id) { return CAnimalArray::mStaticVars + static_cast<std::intptr_t>(id) * CAnimalArray::ElementSize; }
        void* Moveable(const void* animal) { return gMapMoveableArrayStorage.entries + Read<std::uint32_t>(animal, 12) * NC2Layout::RecordSize::MapMoveable; }
        const SStaticAnimalTypeData& Type(const void* animal) { return static_cast<const SStaticAnimalTypeData*>(gStaticAnimalTypeData)[Read<std::uint32_t>(animal, 0x14) - 8u]; }
    }

    // Original function: FUN_0044710b; neutral animals bypass diplomacy, owned animals do so only on selection.
    bool Animal_Misc_CanBeAttackedByHuman(const void* animal, std::int32_t humanId, bool allowOwnAnimals)
    {
        if (Type(animal).cannotBeAttacked != 0) return false;
        const auto player = Read<std::int32_t>(animal, NC2Layout::AnimalOffset::PlayerId);
        if (player == 20) return true;
        const auto humanPlayer = Read<std::int32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * CHumanArray::ElementSize, 0x10);
        if (player == humanPlayer && allowOwnAnimals) return true;
        return Logic_EntryPoint_GetDiplomacyState(humanPlayer, player) == 3;
    }

    // Original function: FUN_0040ad95; the manager owns allocation and callbacks.
    std::int32_t Logic_EntryPoint_Add_Animal(std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags) { return CAnimalArray::sTheObjectPtr->AllocateAnimal(player, tribe, job, point, missionCheck, flags); }

    // Original function: FUN_0041c2a9; only non-neutral requests evict a neutral animal.
    std::int32_t CAnimalArray::AllocateAnimal(std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags)
    {
        if (!MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point)) return -1;
        auto id = Element_Allocate();
        if (id == -1)
        {
            if (player == 20) return -1;
            CAnimalIterator iterator;
            const auto neutral = iterator.GetFirstAnimalId(20);
            if (neutral == -1) return -1;
            FreeAnimal(neutral);
            id = Element_Allocate();
        }
        if (!Animal_Init(Animal(id), id, player, tribe, job, point, missionCheck, flags)) { FreeAnimal(id); return -1; }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(6, true, id, 1, 0);
        return id;
    }

    // Original function: FUN_0041c358; notify before exiting the object.
    void CAnimalArray::FreeAnimal(std::int32_t id)
    {
        auto* animal = Animal(id);
        if (Read<std::uint8_t>(animal, 0) == 0 || !Element_Free(id, true)) return;
        if (Read<std::int32_t>(animal, 12) != -1) CCallbackManager::sTheObjectPtr->ExecuteCallback(6, false, id, 0, 0);
        Animal_Exit(animal);
    }

    // Original function: FUN_0042dce9; retain random-call and initial-atomic order.
    bool Animal_Init(void* animal, std::int32_t id, std::uint32_t player, std::uint32_t tribe, std::uint32_t job, const void* point, std::uint32_t missionCheck, std::uint32_t flags)
    {
        if (Read<std::uint8_t>(animal, 0) != 0) return false;
        std::memset(animal, 0, CAnimalArray::ElementSize);
        Write<std::uint8_t>(animal, 0, 1);
        Write(animal, 4, id);
        Write(animal, 8, Game_UniqueId_Generate());
        Write<std::int32_t>(animal, 0x34, -1);
        Write(animal, NC2Layout::AnimalOffset::PlayerId, player);
        Write(animal, NC2Layout::AnimalOffset::TribeType, tribe);
        Write(animal, NC2Layout::AnimalOffset::AnimalType, job);
        const auto moveableId = MapMoveableArray_AllocateMapMoveable(gMapMoveableArrayStorage.manager, 2, id);
        Write(animal, 12, moveableId);
        if (moveableId == -1) return false;
        auto* moveable = Moveable(animal);
        MapMoveable_Attachment_AttachToMap(moveable, point);
        const auto start = Read<std::uint32_t>(moveable, 0x12);
        Write(animal, 0x2C, start);
        Write(animal, 0x28, start);
        Write(animal, 0x24, start);
        Write(animal, 0x64, Animal_HitPoints_GetMaximum(animal));
        Write(animal, 0x68, Game_Random_GetNumber() % 400u);
        Write<std::uint32_t>(animal, 0x6C, 0);
        auto* path = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        const auto speed = Type(animal).moveSpeed;
        Write(moveable, 0x44, speed);
        Write(moveable, 0x48, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(speed) + 9999u) / speed);
        Write(path, 0x2C, player);
        Write<std::int32_t>(path, 0x24, -1);
        Write<std::int32_t>(path, 0x1C, -1);
        Animal_l_StartAtomicAnimation(animal, 2, -1);
        Write<std::uint32_t>(animal, 0x1C, 1);
        Write<std::uint32_t>(animal, 0x20, 1);
        Write<std::int32_t>(animal, 0x34, -1);
        Write(animal, 0x8C, missionCheck);
        const auto delay = Game_Random_GetNumber() % 3u;
        Write(animal, 0x90, flags);
        Write(animal, 0x30, delay);
        Animal_l_Aggressive_Check(animal);
        return true;
    }

    // Original function: FUN_0042de20; preserve cadaver editing and callback ownership.
    void Animal_Exit(void* animal)
    {
        if (Animal_l_IsLeaderValid(animal)) Animal_l_DetachLeader(animal);
        if (Read<std::uint8_t>(animal, 0) == 0) return;
        const auto moveableId = Read<std::int32_t>(animal, 12);
        auto size = static_cast<std::uint32_t>(Type(animal).maximumCadaverSize);
        if (Read<std::uint8_t>(&gGameStaticVars, 0x1B0) == 0 && moveableId != -1 && size != 0 && Read<std::uint8_t>(Moveable(animal), 0x10) != 0)
        {
            const auto* point = Animal_GetMapPositionRef(animal);
            if (Read<std::uint32_t>(animal, 0x40) == 0xFFFE7960u)
            {
                const auto engine = NC2LogicExternals::External_Misc_GetEngineCadaverLandscapeId(Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType), 79);
                if (engine != -1)
                {
                    Logic_EntryPoint_Edit_Start();
                    if (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 48) size >>= 1;
                    Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(point, 79, size, engine, 0, -1);
                    Logic_EntryPoint_Edit_Stop();
                }
            }
            else
            {
                size /= 3u;
                if (size == 0) size = 1;
                Logic_EntryPoint_Edit_Start();
                if (Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 48) size >>= 1;
                const auto engine = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(44, true);
                Logic_EntryPoint_Edit_MapMicroPoint_Landscape_SetType(point, 44, size, engine, 0, -1);
                Logic_EntryPoint_Edit_Stop();
            }
        }
        if (moveableId != -1) MapMoveableArray_FreeMapMoveable(gMapMoveableArrayStorage.manager, moveableId);
        Write<std::uint8_t>(animal, 0, 0);
    }

    // Original function: FUN_0042e82c; unsupported atomic types return false.
    bool Animal_l_StartAtomicAnimation(void* animal, std::uint32_t action, std::int32_t direction)
    {
        if (action == 1 || !Animal_l_IsAtomicInterruptable(animal)) return false;
        auto job = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType);
        std::uint32_t animation;
        for (;;)
        {
            const auto* data = static_cast<const std::byte*>(gStaticJobTypeData) + job * NC2Layout::RecordSize::JobType;
            if (Read<std::uint8_t>(data, 0x24u + action) == 0) return false;
            animation = gStaticTribeTypeData[Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType)].atomicIds[job][action];
            if (animation != NXBasics::IntegerRange::UInt32Maximum) break;
            job = Read<std::uint32_t>(data, 0x20);
            if (job == 0) return false;
        }
        Write<std::uint32_t>(animal, 0x7C, 0);
        Write(animal, 0x80, Read<std::uint32_t>(animal, 0x80) + 1u);
        Write(animal, 0x78, animation);
        Write(animal, 0x70, action);
        Write<std::uint8_t>(animal, 0x74, 1);
        const auto preferred = gStaticAtomicAnimationData[animation].startDirection;
        if (preferred != -1) MapMoveable_Attachment_StartTurn(Moveable(animal), preferred);
        else if (direction != -1) MapMoveable_Attachment_StartTurn(Moveable(animal), direction);
        return true;
    }

    // Original function: FUN_0042e9ae; no active animation is interruptible.
    bool Animal_l_IsAtomicInterruptable(const void* animal)
    {
        if (Read<std::uint32_t>(animal, 0x70) == 1) return Read<std::uint8_t>(Moveable(animal), 0x2C) == 0;
        return Read<std::uint8_t>(animal, 0x74) == 0 || gStaticAtomicAnimationData[Read<std::uint32_t>(animal, 0x78)].interruptable != 0;
    }

    // Original function: FUN_0042ebc1; leader attachment types must agree.
    bool Animal_l_IsLeaderValid(const void* animal)
    {
        const auto id = Read<std::int32_t>(animal, 0x34);
        if (id == -1) return false;
        const auto* leader = Animal(id);
        if (Read<std::uint8_t>(leader, 0) == 0 || Read<std::uint32_t>(leader, 8) != Read<std::uint32_t>(animal, 0x38) || Read<std::uint32_t>(leader, 0x10) != Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId) || Animal_IsAttachedToVehicle(leader)) return false;
        if (!Animal_IsAttachedToHouse(animal)) return !Animal_IsAttachedToHouse(leader);
        return Animal_IsAttachedToHouse(leader) && TargetData_GetUniqueId(static_cast<const std::byte*>(leader) + 0x50) == TargetData_GetUniqueId(static_cast<const std::byte*>(animal) + 0x50);
    }

    // Original function: FUN_0042f2ad.
    bool Animal_IsAttachedToHouse(const void* animal) { const auto* target = static_cast<const std::byte*>(animal) + 0x50; return TargetData_GetTargetType(target) == 4 && TargetData_IsValid(target); }
    // Original function: FUN_0042f2d1.
    bool Animal_IsAttachedToVehicle(const void* animal) { const auto* target = static_cast<const std::byte*>(animal) + 0x50; return TargetData_GetTargetType(target) == 2 && TargetData_IsValid(target); }

    // Original function: FUN_0042f2f5; even an unallocated house triggers center update.
    void Animal_AttachToHouse(void* animal, std::int32_t houseId)
    {
        const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseId) * CHouseArray::ElementSize;
        if (Read<std::uint8_t>(house, NC2Layout::HouseOffset::AllocatedFlag) != 0)
        {
            Write(animal, 0x54, houseId);
            Write(animal, 0x58, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
            Write<std::uint32_t>(animal, 0x50, 4);
            Animal_l_DetachLeader(animal);
        }
        Animal_l_MoveWorkCenterPoint(animal);
    }

    // Original function: FUN_0042f331; a walking animal keeps its task and reports failure after attachment.
    bool Animal_AttachToVehicle(void* animal, std::int32_t vehicleId)
    {
        const auto* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(vehicleId) * CVehicleArray::ElementSize;
        if (Read<std::uint8_t>(vehicle, 0) != 0)
        {
            Write(animal, 0x54, vehicleId);
            Write(animal, 0x58, Read<std::uint32_t>(vehicle, 8));
            Write<std::uint32_t>(animal, 0x50, 2);
        }
        if (Read<std::uint32_t>(animal, 0x70) == 1) return false;
        if (Read<std::uint32_t>(animal, 0x1C) != 9) { Write<std::uint32_t>(animal, 0x20, 0); Write<std::uint32_t>(animal, 0x1C, 9); }
        return true;
    }

    // Original function: FUN_0042f376.
    void Animal_l_MoveWorkCenterPoint(void* animal)
    {
        if (Animal_IsAttachedToVehicle(animal)) return;
        if (Animal_IsAttachedToHouse(animal))
        {
            const auto id = TargetData_GetObjectId(static_cast<const std::byte*>(animal) + 0x50);
            Write(animal, 0x24, Read<std::uint32_t>(CHouseArray::mStaticVars + id * CHouseArray::ElementSize, NC2Layout::HouseOffset::DoorEntryPoint));
            return;
        }
        const auto player = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId);
        if (player != 20)
        {
            const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Animal_GetMapPositionRef(animal)), 6);
            CHouseIterator iterator;
            for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId())
            {
                const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (House_GetHouseMainType(house) != 1 || Read<std::uint8_t>(house, NC2Layout::HouseOffset::DemolitionFlag) != 0 || Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(house) + NC2Layout::HouseOffset::MapPosition), 6) != continent) continue;
                Write(animal, 0x54, id);
                Write(animal, 0x58, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                Write<std::uint32_t>(animal, 0x50, 4);
                Animal_l_DetachLeader(animal);
                return;
            }
        }
        if (Type(animal).ignoreHouses != 0) return;
        const auto* manager = CGameMapSectorAttachmentManager::sTheObjectPtr;
        const auto occupied = [manager, player](const SSectorGridCursor& cursor, bool stopAtFirst) -> std::uint32_t
        {
            const auto index = static_cast<std::uint32_t>(cursor.y) * static_cast<std::uint32_t>(manager->GetGridWidth()) + static_cast<std::uint32_t>(cursor.x);
            const auto* sector = manager->GetSectorData() + index * 0x540u;
            std::uint32_t total = 0;
            for (std::uint32_t other = 0; other < 20; ++other)
            {
                if (other == player || Read<std::uint8_t>(gGameControl, other * NC2Layout::RecordSize::GameControlPlayer) == 0) continue;
                const auto first = Read<std::uint32_t>(sector, other * 0x40u + 0x14), second = Read<std::uint32_t>(sector, other * 0x40u + 0x2C);
                if (stopAtFirst && (first != 0 || second != 0)) return 1;
                total += first + second;
            }
            return total;
        };
        const SSectorGridCursor center(Read<std::uint32_t>(animal, 0x24));
        if (occupied(center, true) == 0) return;
        const SSectorGridCursor active(Read<std::uint32_t>(Animal_GetMapPositionRef(animal), 0));
        auto best = active;
        auto minimum = occupied(active, false);
        for (std::uint32_t direction = 0; direction < 8; ++direction)
        {
            auto candidate = center;
            candidate.MoveCursorUnwrapped(direction);
            if (!candidate.IsInsideMap()) continue;
            const auto count = occupied(candidate, false);
            if (count < minimum) { best = candidate; minimum = count; }
        }
        Write(animal, 0x24, static_cast<std::uint32_t>(static_cast<std::uint16_t>(best.x * 20)) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(best.y * 20)) << 16));
        auto* point = static_cast<std::byte*>(animal) + 0x24;
        if (MapMigPoint_IsBorderPoint(point)) MapMigPoint_ConstructOffset(point, point, 10, 10, false);
        Tool_Map_GetNotBlockedPosition(point, true);
    }

    // Original function: FUN_00461521.
    void Animal_GoIntoHouseInteractionMode(void* animal)
    {
        if (Read<std::uint32_t>(animal, 0x1C) == 10) return;
        Write<std::uint32_t>(animal, 0x20, 0);
        Write<std::uint32_t>(animal, 0x1C, 10);
    }

    // Original function: FUN_004471ca; aggression changes sector-list membership.
    void Animal_l_Aggressive_Check(void* animal)
    {
        if (Type(animal).aggressive != 0 && Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 49)
        {
            if (Read<std::uint8_t>(animal, 0x5C) == 0)
            {
                MapMoveable_Attachment_DetachFromSectorMap(Moveable(animal));
                Write<std::uint8_t>(animal, 0x5C, 1);
                MapMoveable_Attachment_AttachToSectorMap(Moveable(animal));
            }
        }
        else if (Read<std::uint32_t>(animal, 0x60) != 0) Write(animal, 0x60, Read<std::uint32_t>(animal, 0x60) - 1u);
        else if (Read<std::uint8_t>(animal, 0x5C) != 0)
        {
            MapMoveable_Attachment_DetachFromSectorMap(Moveable(animal));
            Write<std::uint8_t>(animal, 0x5C, 0);
            MapMoveable_Attachment_AttachToSectorMap(Moveable(animal));
        }
    }

    // Original function: FUN_0042e9e8.
    std::uint32_t Animal_GetCurrentAtomicAnimationLengthInTicks(const void* animal) { return NC2Lifetime::Read<std::uint32_t>(gStaticAtomicAnimationData, NC2Lifetime::Read<std::uint32_t>(animal, 0x78) * 0x138u + 0x34); }
    // Original function: FUN_0042f6a2; forced house exit only clears the hidden byte.
    void Animal_HouseMisc_Exit(void* animal) { Write<std::uint8_t>(animal, 0x94, 0); }

    // Original function: FUN_0042f6aa; animal type IDs index the native biased table.
    std::uint32_t Animal_HitPoints_GetMaximum(const void* animal)
    {
        const auto offset = NC2Lifetime::Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType) * 0x40u - (NC2Lifetime::Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 48 ? 0x1CCu : 0x1C8u);
        return NC2Lifetime::Read<std::uint32_t>(gStaticAnimalTypeData, offset);
    }
    // Original function: FUN_0042e9fc; the native helper copies the position without allocation or attachment checks.
    bool Animal_GetMapPosition(const void* animal, void* point)
    {
        Write(point, 0, Read<std::uint32_t>(Moveable(animal), 0x12));
        return true;
    }

    // Original function: FUN_0042ef84; the native work-center reference is the leader's +2Ch point.
    const void* Animal_GetAnimalWorkCenterPointRef(const void* animal)
    {
        return static_cast<const std::byte*>(Animal(Read<std::int32_t>(animal, 0x34))) + 0x2C;
    }

    // Original function: FUN_0042ea17
    const void* Animal_GetMapPositionRef(const void* animal)
    {
        std::uint32_t index;
        std::memcpy(&index, static_cast<const std::byte*>(animal) + NC2Layout::AnimalOffset::MapMoveableId, sizeof(index));

        const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(gMapMoveableArrayStorage.entries));
        const std::uint32_t positionAddress = baseAddress + index * 0x58u + 0x12u;

        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(positionAddress));
    }
}
