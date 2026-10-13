#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2MilitaryDamage.h"
#include "C2AnimalAccess.h"
#include "C2AnimalIterator.h"
#include "C2GameControlNetwork.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanWalking.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2StaticActorTypeDataAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0045d186.
    bool Human_Item_IsAmuletEquipped_Strength(const void* human) { return Human_l_Item_IsGoodTypeEquipped(human, 52); }

    // Original function: FUN_0045d18e.
    bool Human_Item_IsAmuletEquipped_CriticalHit(const void* human) { return Human_l_Item_IsGoodTypeEquipped(human, 54); }

    // Original function: FUN_00442188; weapon category chooses the experience field.
    std::uint32_t Human_GetExperienceForEquippedWeapon(const void* human)
    {
        const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType)]);
        const auto category = weapons[Read<std::uint32_t>(human, NC2Layout::HumanOffset::EquippedWeapon)].mainType;
        const auto offset = category == 1 ? 0x140u : category == 2 ? 0x144u : category == 3 ? 0x148u : category == 6 ? 0x150u : category == 7 ? 0x154u : 0u;
        return offset != 0 ? Read<std::uint32_t>(human, offset) : 0;
    }

    // Original function: FUN_0043b2af; bonuses apply to the attacker, in native RNG order.
    std::int32_t Tool_Military_GetDamage(std::int32_t damage, const void* attacker)
    {
        if (TargetData_GetTargetType(attacker) != 1 || !TargetData_IsValid(attacker)) return damage;
        const auto* human = CHumanArray::mStaticVars + TargetData_GetObjectId(attacker) * CHumanArray::ElementSize;
        auto amount = static_cast<std::uint32_t>(damage);
        if (Human_Item_IsAmuletEquipped_Strength(human)) amount = static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(amount * 3u) >> 1);
        if (Human_Item_IsAmuletEquipped_CriticalHit(human) && Game_Random_GetNumber() % 100u < 20u) amount *= 2u;
        return NXBasics::Runtime_TimeSigned32(amount);
    }

    // Original function: FUN_0043a059; the cumulative ring keeps the first strict maximum.
    bool Tool_Misc_GetFleePointFarestAwayFromTargetPointRelativToSourcePoint(const void* source, const void* target, void* result, std::uint32_t radius)
    {
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6);
        auto point = Read<std::uint32_t>(source, 0);
        MapMigPoint_MoveCursorUnwrapped(&point, 4, radius);
        std::uint32_t maximum = 0;
        for (std::uint32_t direction = 0; direction < 6; ++direction)
            for (std::uint32_t step = 0; step < radius; ++step)
            {
                MapMigPoint_MoveCursorUnwrapped(point, direction);
                if (MapMigPoint_IsBorderPoint(&point)) continue;
                const auto* micro = GameMap_GetConstMapMicroPointPtr(&point);
                if ((Read<std::uint8_t>(micro, 12) & NXBasics::BitMask::Bit7) != 0 || Read<std::uint8_t>(micro, 6) != continent || Read<std::uint8_t>(micro, 0) == 2) continue;
                const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(target, 0), Read<std::int16_t>(target, 2)));
                if (distance <= maximum) continue;
                maximum = distance;
                Write(result, 0, point);
            }
        return maximum != 0;
    }

    // Original function: FUN_0044723b; sector membership changes before resetting the timer.
    void Animal_l_Aggressive_GetAngry(void* animal)
    {
        const auto& type = static_cast<const SStaticAnimalTypeData*>(gStaticAnimalTypeData)[Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType) - 8u];
        if (type.getAngry == 0) return;
        if (Read<std::uint8_t>(animal, 0x5C) == 0)
        {
            auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(animal, 12) * NC2Layout::RecordSize::MapMoveable;
            MapMoveable_Attachment_DetachFromSectorMap(moveable);
            Write<std::uint8_t>(animal, 0x5C, 1);
            MapMoveable_Attachment_AttachToSectorMap(moveable);
        }
        Write(animal, 0x60, type.angryGameTime);
    }

    // Original function: FUN_004464c9; propagation notifies matching animals once without recursive propagation.
    bool Animal_Military_StartGettingHitByWeapon(void* animal, std::uint32_t tribe, std::uint32_t weapon, std::uint32_t attackerId, std::uint32_t attackerRevision, std::uint32_t attackerType, std::uint32_t power, bool propagate)
    {
        const auto& data = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[tribe])[weapon];
        const auto limitedPower = power > 100u ? 100u : power;
        auto damage = static_cast<std::uint32_t>(data.damageValues[2]) * 200u / (200u - limitedPower);
        std::uint32_t attacker[3]{};
        attacker[0] = attackerType;
        if (attackerType == 6) attacker[1] = attackerId;
        else { attacker[1] = attackerId; attacker[2] = attackerRevision; }
        damage = static_cast<std::uint32_t>(Tool_Military_GetDamage(NXBasics::Runtime_TimeSigned32(damage), attacker));
        const auto player = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId);
        if (Read<std::uint32_t>(gGameControl, NC2Layout::GameControlOffset::DifficultyLevel) != 0 && player != 20 && (player == 0 || GameControl_Player_IsNetworkPlayer(gGameControl, NXBasics::Runtime_TimeSigned32(player)))) damage = static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(damage) / 2);
        if (NXBasics::Runtime_TimeSigned32(damage) > 0)
        {
            if ((Read<std::uint32_t>(animal, 0x90) & 8u) == 0) Write(animal, 0x64, Read<std::uint32_t>(animal, 0x64) - damage);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x24, true, MapMigPoint_GetLong(Animal_GetMapPositionRef(animal)), 0, 12);
            if (data.soundTypesHit[2] != 0) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, data.soundTypesHit[2], MapMigPoint_GetLong(Animal_GetMapPositionRef(animal)), 1);
            if (Read<std::int32_t>(animal, 0x64) <= 0 && weapon == 19) Write<std::int32_t>(animal, 0x40, -100000);
        }
        Animal_l_Aggressive_GetAngry(animal);
        if (!TargetData_IsValid(attacker)) return false;
        std::uint32_t position = 0, flee = 10000u | (10000u << 16);
        TargetData_GetMapPosition(attacker, &position);
        const auto aggressive = Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::AnimalType) == 49 && Read<std::uint8_t>(animal, 0x5C) != 0;
        if ((attackerType == 1 || attackerType == 3) && aggressive && TargetData_GetPlayerId(attacker) != Read<std::int32_t>(animal, NC2Layout::AnimalOffset::PlayerId))
        {
            if (!TargetData_IsValid(static_cast<const std::byte*>(animal) + 0x44))
            {
                const auto* leader = CAnimalArray::mStaticVars + Read<std::uint32_t>(animal, 0x34) * CAnimalArray::ElementSize;
                if (!Animal_l_IsLeaderValid(animal) || Read<std::uint32_t>(leader, 0x1C) != 4)
                {
                    Write<std::uint32_t>(animal, 0x20, 0);
                    for (std::uint32_t index = 0; index < 3; ++index) Write(animal, 0x44u + index * 4u, attacker[index]);
                    Write<std::uint32_t>(animal, 0x1C, 8);
                }
            }
        }
        else if (Read<std::uint32_t>(animal, 0x34) == Read<std::uint32_t>(animal, 4) && Tool_Misc_GetFleePointFarestAwayFromTargetPointRelativToSourcePoint(Animal_GetMapPositionRef(animal), &position, &flee, 20))
        {
            Write(animal, 0x2C, flee);
            Write<std::uint32_t>(animal, 0x1C, 4);
            Write<std::uint32_t>(animal, 0x20, 0);
        }
        if (propagate)
        {
            CAnimalIterator iterator;
            for (auto id = iterator.GetFirstAnimalId(Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::PlayerId), Animal_GetMapPositionRef(animal), 20); id != -1; id = iterator.GetNextAnimalId())
            {
                if (static_cast<std::uint32_t>(id) == Read<std::uint32_t>(animal, 4)) continue;
                auto* other = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize;
                if (Read<std::uint32_t>(other, 0x14) == Read<std::uint32_t>(animal, NC2Layout::AnimalOffset::TribeType)) Animal_Military_StartGettingHitByWeapon(other, tribe, 0, attackerId, attackerRevision, attackerType, 0, false);
            }
        }
        return NXBasics::Runtime_TimeSigned32(damage) > 0;
    }
}
