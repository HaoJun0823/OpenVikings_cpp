#include "SwitchIdentifiers.h"
#include "C2DelayedActionManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2MilitaryDamage.h"
#include "C2MilitaryTargetSearch.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2StaticVehicleTypeData.h"
#include "C2VehicleAccess.h"
#include "C2VehicleIterator.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Human(std::uint32_t id) { return CHumanArray::mStaticVars + id * CHumanArray::ElementSize; }
        void* Animal(std::uint32_t id) { return CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize; }
        void* Vehicle(std::uint32_t id) { return CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize; }
        void* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        bool Contains(const SDelayedHitObjectArray* targets, std::uint32_t type, std::uint32_t id)
        {
            for (std::uint32_t index = 0; index < targets->count; ++index) if (targets->entries[index].type == type && targets->entries[index].id == id) return true;
            return false;
        }
        bool Add(SDelayedHitObjectArray* targets, std::uint32_t type, std::uint32_t id)
        {
            if (targets->count >= 100u) return false;
            targets->entries[targets->count++] = { type, id };
            return true;
        }
    }

    // Original function: FUN_0041a097; execution can change the occupied-range bounds.
    void __fastcall DelayedActionManager_System_DoOneGameTick(void* manager, void*)
    {
        auto* array = static_cast<CArrayHandler*>(manager);
        for (auto index = static_cast<std::uint32_t>(array->mFirstScanIndex); index <= static_cast<std::uint32_t>(array->mLastScanIndex); ++index)
        {
            auto* action = Read<SDelayedAction*>(manager, 0x30) + index;
            if (action->allocated == 0) continue;
            ++action->elapsedTicks;
            if (action->elapsedTicks < action->delayTicks) continue;
            DelayedActionManager_l_DelayedAction_Execute(manager, action);
            if (array->Element_Free(static_cast<std::int32_t>(index), true)) (Read<SDelayedAction*>(manager, 0x30) + index)->allocated = 0;
        }
    }

    // Original function: FUN_0041a0ee; allocation failure executes the hit immediately.
    bool DelayedActionManager_DelayedAction_AddWeaponHit(void* manager, std::uint32_t delay, std::uint32_t attackerType, std::uint32_t attackerId, const void* point, std::uint32_t weapon)
    {
        SDelayedAction action{};
        action.allocated = 1;
        action.delayTicks = delay;
        action.type = 1;
        action.attackerType = attackerType;
        action.attackerId = attackerId;
        action.targetPosition = MapMigPoint_GetLong(point);
        action.weapon = weapon;
        if (attackerType == 4)
        {
            const auto* house = House(attackerId);
            action.attackerRevision = Read<std::uint32_t>(house, 8);
            action.player = Read<std::uint32_t>(house, 12);
            action.tribe = Read<std::uint32_t>(house, 0x18);
            action.sourcePosition = Read<std::uint32_t>(house, 0x1C);
        }
        else if (attackerType == 2)
        {
            const auto* vehicle = Vehicle(attackerId);
            action.attackerRevision = Read<std::uint32_t>(vehicle, 8);
            action.player = Read<std::uint32_t>(vehicle, 0x1E0);
            action.tribe = Read<std::uint32_t>(vehicle, 0x1E4);
            action.sourcePosition = MapMigPoint_GetLong(Vehicle_GetMapPositionRef(vehicle));
        }
        else if (attackerType == 1)
        {
            const auto* human = Human(attackerId);
            action.attackerRevision = Read<std::uint32_t>(human, 8);
            action.player = Read<std::uint32_t>(human, 0x10);
            action.tribe = Read<std::uint32_t>(human, 0x14);
            action.sourcePosition = MapMigPoint_GetLong(Human_GetMapPositionRef(human));
        }
        else return false;
        const auto index = static_cast<CArrayHandler*>(manager)->Element_Allocate();
        if (index == -1) { DelayedActionManager_l_DelayedAction_Execute(manager, &action); return false; }
        Read<SDelayedAction*>(manager, 0x30)[index] = action;
        return true;
    }

    // Original function: FUN_0041a1fe; area damage collects the center followed by its six neighbors.
    void DelayedActionManager_l_DelayedAction_Execute(void* manager, const SDelayedAction* action)
    {
        if (action->type != 1) return;
        const auto& weapon = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[action->tribe])[action->weapon];
        const auto originalPosition = action->targetPosition;
        auto position = originalPosition;
        SDelayedHitObjectArray targets;
        targets.count = 0;
        DelayedActionManager_FillHitObjectList(manager, &position, action, &targets);
        if (weapon.createsSmoke != 0) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x24, true, position, 0, weapon.smokeLifetime);
        if (weapon.damageType == 2)
        {
            MapMigPoint_MoveCursorUnwrapped(position, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
            {
                MapMigPoint_MoveCursorUnwrapped(position, direction);
                DelayedActionManager_FillHitObjectList(manager, &position, action, &targets);
                if (weapon.createsSmoke != 0) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x24, true, position, 0, weapon.smokeLifetime);
            }
        }
        if (DelayedActionManager_HitTargetsInList(manager, weapon.damageType == 2 ? targets.count : 1u, action, &targets) || MapMigPoint_IsBorderPoint(&originalPosition)) return;
        std::uint32_t macro = 0;
        MapMagPoint_ConstructFromMig(&macro, &originalPosition, false);
        const auto sound = weapon.soundTypesNoHit[Read<std::uint8_t>(GameMap_GetConstMapMacroPointPtr(&macro), 0)];
        if (sound != 0) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, sound, originalPosition, 0);
    }

    // Original function: FUN_0041a37c; retain native list order, duplicate rules, and capacity exits.
    void DelayedActionManager_FillHitObjectList(void*, const void* point, const SDelayedAction* action, SDelayedHitObjectArray* targets)
    {
        if (!MapMigPoint_IsInsideMap(point)) return;
        const auto& weapon = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[action->tribe])[action->weapon];
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
        {
            const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            const auto object = Read<std::uint32_t>(moveable, 0x54);
            switch (Read<std::uint32_t>(moveable, 8))
            {
            case SwitchIds::MapMoveableType::Human:
                if ((Read<std::uint32_t>(Human(object), 0x10) == action->player && weapon.hitSelf == 0) || Human_Misc_IsHidden(Human(object))) break;
                if (!Add(targets, 1, Read<std::uint32_t>(Human(object), 4))) return;
                break;
            case SwitchIds::MapMoveableType::Animal: if (!Add(targets, 3, Read<std::uint32_t>(Animal(object), 4))) return; break;
            case SwitchIds::MapMoveableType::Vehicle:
                if (Read<std::uint32_t>(Vehicle(object), 0x1E0) == action->player && weapon.hitSelf == 0) break;
                if (!Contains(targets, 2, Read<std::uint32_t>(Vehicle(object), 4)) && !Add(targets, 2, Read<std::uint32_t>(Vehicle(object), 4))) return;
                break;
            case SwitchIds::MapMoveableType::Marker: break;
            default: return;
            }
        }
        if (Read<std::uint8_t>(micro, 0) == 2)
        {
            const auto* house = House(Read<std::uint16_t>(micro, 4));
            if (Read<std::uint32_t>(house, 12) == action->player && weapon.hitSelf == 0) return;
            const auto id = Read<std::uint32_t>(house, 4);
            if (!Contains(targets, 4, id)) Add(targets, 4, id);
            return;
        }
        if (Read<std::uint8_t>(micro, 0) == 1 && Read<std::uint8_t>(micro, 2) != 0)
        {
            if (Read<std::uint8_t>(micro, 2) == 4 && action->weapon == 21)
            {
                if (Read<std::uint8_t>(micro, 3) >= 3)
                {
                    GameMap_MapMicroPoint_Landscape_ApplyTransition(point, 11);
                    CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, 10, MapMigPoint_GetLong(point), 0);
                }
                else { Logic_EntryPoint_Edit_Start(); GameMap_MapMicroPoint_Landscape_Clear(point); Logic_EntryPoint_Edit_Stop(); }
                return;
            }
            if (Tool_IsEnemyLandscapeOnPoint(weapon.hitSelf != 0 ? -1 : static_cast<std::int32_t>(action->player), point)) Add(targets, 5, MapMigPoint_GetLong(point));
            return;
        }
        if ((Read<std::uint8_t>(micro, 12) & 0x80u) != 0)
        {
            for (std::uint32_t radius = 1; radius < 5; ++radius)
            {
                auto cursor = MapMigPoint_GetLong(point);
                MapMigPoint_MoveCursorUnwrapped(&cursor, 4, radius);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < radius; ++step)
                    {
                        MapMigPoint_MoveCursorUnwrapped(cursor, direction);
                        if (!MapMigPoint_IsInsideMap(&cursor)) continue;
                        const auto* nearby = GameMap_GetConstMapMicroPointPtr(&cursor);
                        if (Read<std::uint8_t>(nearby, 0) != 2) continue;
                        const auto* house = House(Read<std::uint16_t>(nearby, 4));
                        if ((Read<std::uint32_t>(house, 12) == action->player && weapon.hitSelf == 0) || !House_IsPositionInHouse(house, point)) continue;
                        const auto id = Read<std::uint32_t>(house, 4);
                        if (!Contains(targets, 4, id) && !Add(targets, 4, id)) return;
                    }
            }
            return;
        }
        CVehicleIterator vehicles;
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            if (Read<std::uint8_t>(gGameControl, player * 0x4A80u) == 0 || player == action->player) continue;
            for (std::uint32_t group = 0; group < 2; ++group)
                for (auto id = vehicles.GetFirstSectorVehicleId(player, MapMigPoint_GetLong(point), 5, group == 0); id != -1; id = vehicles.GetNextSectorVehicleId())
                {
                    const auto* vehicle = Vehicle(static_cast<std::uint32_t>(id));
                    if ((Read<std::uint32_t>(vehicle, 0x1E0) == action->player && weapon.hitSelf == 0) || !Vehicle_IsPositionInsideVehicle(vehicle, point)) continue;
                    const auto object = Read<std::uint32_t>(vehicle, 4);
                    if (!Contains(targets, 2, object) && !Add(targets, 2, object)) return;
                }
        }
    }

    // Original function: FUN_0041a8b9; grant experience once after the entire target list.
    bool DelayedActionManager_HitTargetsInList(void*, std::uint32_t count, const SDelayedAction* action, const SDelayedHitObjectArray* targets)
    {
        bool hit = false;
        for (std::uint32_t index = 0; index < count && index < targets->count; ++index)
        {
            const auto& target = targets->entries[index];
            bool result = false;
            switch (target.type)
            {
            case SwitchIds::DamageTargetType::Human: result = Human_Military_StartGettingHitByWeapon(Human(target.id), &action->sourcePosition, action->tribe, action->weapon, action->attackerId, action->attackerRevision, action->attackerType, 0, true); break;
            case SwitchIds::DamageTargetType::Vehicle: result = Vehicle_Military_StartGettingHitByWeapon(Vehicle(target.id), action->tribe, action->weapon, action->attackerId, action->attackerRevision, action->attackerType, 0); break;
            case SwitchIds::DamageTargetType::Animal: result = Animal_Military_StartGettingHitByWeapon(Animal(target.id), action->tribe, action->weapon, action->attackerId, action->attackerRevision, action->attackerType, 0, true); break;
            case SwitchIds::DamageTargetType::House: result = House_Military_StartGettingHitByWeapon(House(target.id), action->tribe, action->weapon, action->attackerId, action->attackerRevision, action->attackerType, 0); break;
            case SwitchIds::DamageTargetType::Landscape: result = Tool_HitLandscapeOnPoint(action->player, action->tribe, action->weapon, &target.id); break;
            }
            if (result) hit = true;
        }
        if (hit && action->attackerType == 1)
        {
            auto* human = Human(action->attackerId);
            if (Read<std::uint8_t>(human, 0) != 0 && Read<std::uint32_t>(human, 8) == action->attackerRevision) Human_GainExperienceForWeapon(human, action->weapon, 1);
        }
        else if (hit && action->attackerType == 2)
        {
            const auto* vehicle = Vehicle(action->attackerId);
            if (Read<std::uint8_t>(vehicle, 0) != 0 && Read<std::uint32_t>(vehicle, 8) == action->attackerRevision) Vehicle_Military_GainExperience(vehicle);
        }
        return hit;
    }

    // Original function: FUN_00434b86; row endpoints use an inclusive native comparison.
    bool House_IsPositionInHouse(const void* house, const void* point)
    {
        const auto stage = Read<std::uint32_t>(house, 0x20) + (Read<std::uint32_t>(house, 0x2C) == 2 ? 1u : 0u);
        const auto* data = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        const auto* area = Read<const CMapArea*>(data, 0x20u + stage * 4u);
        if (area == nullptr) return false;
        for (std::uint32_t index = 0; index < area->rowCount; ++index)
        {
            std::uint32_t position;
            MapMigPoint_ConstructOffset(&position, static_cast<const std::byte*>(house) + 0x1C, area->rows[index].x, area->rows[index].y, false);
            const auto x = Read<std::int16_t>(&position, 0), targetX = Read<std::int16_t>(point, 0);
            if (Read<std::int16_t>(&position, 2) == Read<std::int16_t>(point, 2) && x <= targetX && NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(x) + area->rows[index].width) >= targetX) return true;
        }
        return false;
    }

    // Original function: FUN_004415d9.
    bool Vehicle_IsPositionInsideVehicle(const void* vehicle, const void* point)
    {
        const auto* position = Vehicle_GetMapPositionRef(vehicle);
        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2)));
        return static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize) >= distance;
    }

    // Original function: FUN_0043b1cc; each 100 damage points removes one landscape increment.
    bool Tool_HitLandscapeOnPoint(std::uint32_t player, std::uint32_t tribe, std::uint32_t weapon, const void* point)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
        if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0) return false;
        const auto owner = Read<std::int8_t>(micro, 4);
        const auto& data = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[tribe])[weapon];
        const auto increments = static_cast<std::uint32_t>(data.damageValues[7]) / 100u;
        bool hit = false;
        for (std::uint32_t index = 0; index < increments; ++index)
        {
            micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0) break;
            if (GameMap_MapMicroPoint_Landscape_ApplyTransition(point, 10) != -1) hit = true;
        }
        if (hit)
        {
            if (owner != -1) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x28, true, static_cast<std::uint32_t>(owner), player, 0);
            if (data.soundTypesHit[7] != 0) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, data.soundTypesHit[7], MapMigPoint_GetLong(point), 1);
        }
        return hit;
    }
}
