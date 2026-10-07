#include "C2HumanWalking.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2GameControlNetwork.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2MapPathfinderSearch.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2UserMessageAccess.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    // Original function: FUN_0042c9c9; include a pending pathfinder start.
    bool Human_IsWalking(const void* human)
    {
        if (NC2Lifetime::Read<std::uint32_t>(human, 0xAB8) == 1) return true;
        const auto* moveable = gMapMoveableArrayStorage.entries + NC2Lifetime::Read<std::uint32_t>(human, 12) * 0x58u;
        return NC2Lifetime::Read<std::uint32_t>(MapMoveable_Pathfinder_GetPathfinderPtr(moveable), 4) == 1;
    }
    using namespace NC2Lifetime;

    namespace
    {
        struct FreeMapPoints { std::uint32_t count = 0; std::uint32_t points[8]{}; };
        struct VehicleFreeMapPoints { std::uint32_t minimumDistance; FreeMapPoints candidates; };

        bool IsFreeForStationaryMoveables(const void* point)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 12) & 0x80u) != 0) return false;
            for (auto id = static_cast<std::int32_t>(Read<std::int16_t>(micro, 10)); id != -1;)
            {
                const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                const auto state = Read<std::uint32_t>(MapMoveable_Pathfinder_GetPathfinderPtr(moveable), 4);
                if (state == 0 || state == 3) return false;
                id = Read<std::int32_t>(moveable, 0x24);
            }
            return true;
        }

        // Original function: FUN_0045d374; the second accepted point stops the search.
        bool __cdecl slHumanSearchNextFreeMapPointFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            if (!IsFreeForStationaryMoveables(point)) return false;
            auto& candidates = *static_cast<FreeMapPoints*>(callback->userData);
            candidates.points[candidates.count++] = Read<std::uint32_t>(point, 0);
            return candidates.count > 1;
        }

        // Original function: FUN_0045d45e; compare the path search distance before occupancy.
        bool __cdecl slHumanSearchNextVehicleFreeMapPointFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<VehicleFreeMapPoints*>(callback->userData);
            if (callback->distance < data.minimumDistance || !IsFreeForStationaryMoveables(point)) return false;
            data.candidates.points[data.candidates.count++] = Read<std::uint32_t>(point, 0);
            return data.candidates.count > 1;
        }
    }

    // Original function: FUN_0045d315; blocked traversal is allowed for finding an escape point.
    bool Human_FindNextFreeMapPoint(const void* human, void* point)
    {
        FreeMapPoints data;
        CMapPathfinderElement::SearchOverMap(Human_GetMapPositionRef(human), 40, false, slHumanSearchNextFreeMapPointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        Write(point, 0, data.points[Game_Random_GetNumber() % data.count]);
        return true;
    }

    // Original function: FUN_0045d3f9.
    bool Human_FindNextVehicleFreeMapPoint(const void* human, void* point, std::uint32_t minimumDistance)
    {
        VehicleFreeMapPoints data{minimumDistance, {}};
        CMapPathfinderElement::SearchOverMap(Human_GetMapPositionRef(human), 40, true, slHumanSearchNextVehicleFreeMapPointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.candidates.count == 0) return false;
        Write(point, 0, data.candidates.points[Game_Random_GetNumber() % data.candidates.count]);
        return true;
    }

    // Original function: FUN_0045d196.
    bool Human_l_Item_IsGoodTypeEquipped(const void* human, std::uint32_t good)
    {
        for (std::uint32_t slot = 0; slot < 4; ++slot)
        {
            const auto offset = 0xAF4u + slot * 8u;
            const auto equipped = Read<std::uint32_t>(human, offset);
            if (equipped != 0 && Read<std::int32_t>(human, offset + 4) > 0 && equipped == good) return true;
        }
        return false;
    }

    // Original function: FUN_0045d176.
    bool Human_Item_IsAmuletEquipped_Speed(const void* human) { return Human_l_Item_IsGoodTypeEquipped(human, 55); }

    // Original function: FUN_004739e1; preserve 32-bit wrapping before the signed clamp.
    std::int32_t Human_WalkSpeed_GetMoveSpeed(const void* human, std::uint32_t resistance)
    {
        auto duration = resistance * 2u + 4u;
        if (Human_HasEquippedShoes(human)) duration -= 2u;
        if (Human_Item_IsAmuletEquipped_Speed(human)) duration -= 2u;
        if (Read<std::uint32_t>(human, 0xAD4) != 0) ++duration;
        if (Read<std::int32_t>(human, 0xAAC) <= 2000) duration += 2u;
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        const auto job = Read<std::uint32_t>(human, 0x18);
        if (tribe == 3 && job == 32) duration -= 2u;
        if (tribe == 7) duration -= 2u;
        if (job == 1 || job == 2) duration *= 2u;
        else if (job == 3 || job == 4) duration += 2u;
        if (!Human_IsJobHero(human))
        {
            const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[tribe]);
            const auto* armor = static_cast<const SStaticArmorTypeData*>(gStaticArmorTypeData);
            const auto weight = static_cast<std::uint32_t>(weapons[Read<std::uint32_t>(human, 0xAE4)].weight) + static_cast<std::uint32_t>(armor[Read<std::uint32_t>(human, 0xAE8)].weight);
            duration += weight >> 1;
        }
        const auto flags = Read<std::uint32_t>(human, 0xB84);
        if ((flags & 0x200u) != 0) duration *= 2u;
        if ((flags & 0x20000u) != 0) duration -= 2u;
        const auto signedDuration = NXBasics::Runtime_TimeSigned32(duration);
        return signedDuration < 3 ? 3 : signedDuration;
    }

    // Original function: FUN_004739af.
    void Human_WalkSpeed_Update(void* human, std::uint32_t resistance)
    {
        void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
        const auto duration = Human_WalkSpeed_GetMoveSpeed(human, resistance);
        Write(moveable, 0x44, duration);
        Write<std::uint32_t>(moveable, 0x4C, 1);
        Write(moveable, 0x48, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(duration) + 9999u) / duration);
    }

    // Original function: FUN_004406c4; expiry uses unsigned time addition/comparison.
    void GameAssistantSingleHumanData_CommandHistory_Validate(void* data)
    {
        if (Read<std::uint8_t>(data, 4) == 0) return;
        bool changed = false;
        for (std::uint32_t index = 0; index < 10; ++index)
        {
            const auto offset = 8u + index * 0x14u;
            if (Read<std::uint32_t>(data, offset) != 0 && Read<std::uint32_t>(data, offset + 0x0C) + Read<std::uint32_t>(data, offset + 0x10) <= gGameTime.tick)
            {
                Write<std::uint32_t>(data, offset, 0);
                changed = true;
            }
        }
        if (!changed) return;
        bool any = false;
        for (std::uint32_t index = 0; index < 10; ++index) if (Read<std::uint32_t>(data, 8u + index * 0x14u) != 0) { any = true; break; }
        Write<std::uint8_t>(data, 4, any ? 1 : 0);
    }

    // Original function: FUN_00440690.
    bool GameAssistantSingleHumanData_CommandHistory_IsEquipCommandInArray(void* data)
    {
        GameAssistantSingleHumanData_CommandHistory_Validate(data);
        if (Read<std::uint8_t>(data, 4) == 0) return false;
        for (std::uint32_t index = 0; index < 10; ++index)
        {
            const auto type = Read<std::int32_t>(data, 8u + index * 0x14u);
            if (type >= 79 && type <= 87) return true;
        }
        return false;
    }

    // Original function: FUN_0042eec9.
    void Animal_l_DetachLeader(void* animal)
    {
        const auto id = Read<std::uint32_t>(animal, 0x34);
        if (id == 0xFFFFFFFFu) return;
        void* leader = CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize;
        if (Read<std::uint8_t>(leader, 0) != 0 && Read<std::uint32_t>(leader, 8) == Read<std::uint32_t>(animal, 0x38)) Write(leader, 0x3C, Read<std::uint32_t>(leader, 0x3C) - 1u);
        Write<std::uint32_t>(animal, 0x34, 0xFFFFFFFFu);
    }

    // Original function: FUN_0042f63f; transfer updates pathfinder ownership and sector lists.
    void Animal_ChangePlayerId(void* animal, std::uint32_t player)
    {
        void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(animal, 0x0C) * 0x58u;
        MapMoveable_Attachment_DetachFromSectorMap(moveable);
        Write<std::uint32_t>(animal, 0x50, 0);
        Write(animal, 0x10, player);
        void* path = GetUnsafePathfinderPtr(Read<std::int32_t>(moveable, 0x28));
        Write<std::uint32_t>(path, 0x24, 0xFFFFFFFFu);
        Write<std::uint32_t>(path, 0x1C, 0xFFFFFFFFu);
        Write(path, 0x2C, player);
        MapMoveable_Attachment_AttachToSectorMap(moveable);
        Animal_l_DetachLeader(animal);
    }

    // Original function: FUN_0044dc14; catchable animals become the exploring player's animals.
    void Map_AnimalTool_ChangePlayerId(std::uint32_t player, const void* point)
    {
        if (!MapMigPoint_IsInsideMap(point)) return;
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
        {
            const void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            if (Read<std::uint32_t>(moveable, 8) != 2) continue;
            void* animal = CAnimalArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CAnimalArray::ElementSize;
            const auto tribe = Read<std::uint32_t>(animal, 0x14);
            const auto* data = static_cast<const SStaticAnimalTypeData*>(gStaticAnimalTypeData) + tribe - 8u;
            if (data->catchable == 0) continue;
            const auto owner = Read<std::uint32_t>(animal, 0x10);
            if (owner == 20 || GameControl_Diplomacy_GetState(gGameControl, static_cast<std::int32_t>(player), static_cast<std::int32_t>(owner)) == 3) Animal_ChangePlayerId(animal, player);
        }
    }

    // Original function: FUN_0044dbbb; the radius excludes its outermost ring.
    void Map_AnimalTool_ChangePlayerId(std::uint32_t player, std::uint32_t point, std::uint32_t radius)
    {
        Map_AnimalTool_ChangePlayerId(player, &point);
        for (std::uint32_t ring = 1; ring < radius; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < ring; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(point, direction);
                    Map_AnimalTool_ChangePlayerId(player, &point);
                }
        }
    }

    // Original function: FUN_0043ae66; in-place ring traversal and byte continent identity.
    bool Tool_Map_GetNotBlockedPosition(void* point, bool avoidHouses)
    {
        const void* micro = GameMap_GetConstMapMicroPointPtr(point);
        if ((Read<std::uint8_t>(micro, 0x0C) & 0x80u) == 0 && (!avoidHouses || Read<std::uint8_t>(micro, 0) != 2)) return true;
        const auto continent = Read<std::uint8_t>(micro, 6);
        bool found = false;
        for (std::uint32_t ring = 1; ring < 10 && !found; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4, 1);
            for (std::uint32_t direction = 0; direction < 6 && !found; ++direction)
            {
                for (std::uint32_t step = 0; step < ring; ++step)
                {
                    micro = GameMap_GetConstMapMicroPointPtr(point);
                    if ((Read<std::uint8_t>(micro, 0x0C) & 0x80u) == 0 && Read<std::uint8_t>(micro, 6) == continent && (!avoidHouses || Read<std::uint8_t>(micro, 0) != 2)) { found = true; break; }
                    MapMigPoint_MoveCursorUnwrapped(point, direction, 1);
                }
            }
        }
        return found;
    }

    // Original function: FUN_0043aeff; only players 0..9 receive first-contact messages.
    void Tool_Map_CheckUserInformationAboutEnemies(std::uint32_t player, const void* point)
    {
        if (player >= 16) return;
        const auto seen = Read<std::uint16_t>(GameMap_GetConstMapMicroPointPtr(point), 8);
        for (std::uint32_t other = 0; other < 10; ++other)
        {
            if (other == player || (seen & (1u << other)) == 0) continue;
            void* otherData = static_cast<std::byte*>(gGameControl) + other * 0x4A80u;
            if (Read<std::uint8_t>(otherData, 0) == 0 || Read<std::uint8_t>(otherData, 0x45F8u + player) != 0) continue;
            Write<std::uint8_t>(otherData, 0x45F8u + player, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x29, true, other, player, MapMigPoint_GetLong(point));
            const void* ownData = static_cast<const std::byte*>(gGameControl) + player * 0x4A80u;
            if (Read<std::uint8_t>(ownData, 0x4994u + other) != 0) continue;
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 0x3C);
            Write(message, 8, other);
            Write(message, 0x0C, player);
            Write(message, 0x28, Read<std::uint32_t>(point, 0));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
    }

    // Partial original function: FUN_00462cd5; the military destination continuation is pending.
    bool Human_AI_DoNewMapPositionReached(void* human, bool destinationReached)
    {
        Write(human, 0x384, Read<std::uint32_t>(human, 0x384) + 1u);
        const void* position = Human_GetMapPositionRef(human);
        const void* micro = GameMap_GetConstMapMicroPointPtr(position);
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(human, 0x10), position, Read<std::uint32_t>(human, 0xB7C));
        if (Human_HasEquippedShoes(human)) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x24, true, MapMigPoint_GetLong(position), 5, 100);
        if (Human_IsAgeAdult(human) && Read<std::uint32_t>(human, 0x14) != 5 && (Read<std::uint8_t>(micro, 0x0D) & 2u) == 0)
        {
            const void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            auto next = Read<std::uint32_t>(position, 0);
            MapMigPoint_MoveCursorUnwrapped(next, Read<std::uint8_t>(moveable, 0x10) != 0 ? Read<std::uint32_t>(moveable, 0x20) : 2u);
            std::uint32_t macroPoint = 0;
            MapMagPoint_ConstructFromMig(&macroPoint, position, false);
            const void* macro = GameMap_GetConstMapMacroPointPtr(Read<std::int16_t>(&macroPoint, 0), Read<std::int16_t>(&macroPoint, 2));
            const auto kind = Read<std::uint8_t>(macro, 0);
            if (kind == 7 || (kind == 8 && Read<std::uint8_t>(macro, 1) == 8)) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x26, true, kind == 7 ? 2u : 3u, MapMigPoint_GetLong(position), MapMigPoint_GetLong(&next));
            else if (kind == 4) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x24, true, MapMigPoint_GetLong(position), 5, 100);
        }
        if (Human_IsJobScout(human)) Map_AnimalTool_ChangePlayerId(Read<std::uint32_t>(human, 0x10), Read<std::uint32_t>(position, 0), 3);
        auto resistance = (Read<std::uint8_t>(micro, 0x0C) >> 3) & 15u;
        Human_WalkSpeed_Update(human, resistance);
        if (Read<std::uint32_t>(human, 0xAD4) != 0) resistance *= 2u;
        const auto shoes = Read<std::uint32_t>(human, 0xADC);
        if (shoes != 0)
        {
            if ((Read<std::uint32_t>(human, 0xB84) & 0x1000u) == 0)
            {
                const auto remaining = NXBasics::Runtime_TimeSigned32(shoes - resistance);
                Write<std::int32_t>(human, 0xADC, remaining < 0 ? 0 : remaining > 10000 ? 10000 : remaining);
            }
        }
        else if (Human_IsAgeAdult(human) && (Read<std::uint32_t>(human, 0xB84) & 1u) == 0) Write(human, 0xAA8, Read<std::uint32_t>(human, 0xAA8) - resistance);
        if (!destinationReached) return Human_AI_DoNewMapPositionReached_DestinationNotReached(human);
        Human_AI_DoNewMapPositionReached_DestinationReached(human);
        return true;
    }

    // Partial original function: FUN_00463043; military enemy-search branches remain pending.
    bool Human_AI_DoNewMapPositionReached_DestinationNotReached(void* human)
    {
        if (!Human_l_IsAtomicInterruptable(human)) return true;
        if (Read<std::uint32_t>(human, 0x370) == 0 && Human_IsRefreshingInWorld(human) && Read<std::uint32_t>(human, 0x384) % 10u == 0)
        {
            Write<std::uint8_t>(human, 0xA61, 0);
            Human_AI_DoCheckForUrgentNeeds(human);
        }
        if ((Read<std::uint32_t>(human, 0x368) == 7 && Read<std::uint32_t>(human, 0x36C) == 0) || Read<std::uint32_t>(human, 0x368) == 13) return false;
        Human_DoExecuteAllPriorityUserCommandsWhichDoNotInterrupt(human);
        if (Read<std::uint32_t>(human, 0x368) != 23 && Human_UserControl_IsPriorityCommandInQueue(human))
        {
            Human_CurrentTask_DoInterrupt(human);
            return false;
        }
        return true;
    }
}
