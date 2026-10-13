#include "C2ObjectLayout.h"
#include "C2MilitaryTargetSearch.h"
#include "C2AnimalAccess.h"
#include "C2AnimalIterator.h"
#include "C2GameControlNetwork.h"
#include "C2GameState.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2OverviewObjectIterators.h"
#include "C2VehicleAccess.h"
#include "C2VehicleIterator.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0043a114; human and vehicle sector lists retain distinct native filters.
    std::uint32_t Tool_ObjectId_GetNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output, bool humans, bool ignoreHidden)
    {
        std::uint32_t enemies[20];
        std::uint32_t enemyCount = 0;
        for (std::uint32_t enemy = 0; enemy < 20; ++enemy)
        {
            if (Read<std::uint8_t>(gGameControl, enemy * NC2Layout::RecordSize::GameControlPlayer) != 0 && GameControl_Diplomacy_GetState(gGameControl, player, enemy) == 3) enemies[enemyCount++] = enemy;
        }
        if (enemyCount == 0) return 0;
        std::uint32_t count = 0;
        auto cutoff = maximumDistance + 1u;
        // Native output entries are {32-bit object ID, unsigned distance}; equal distances stay in discovery order.
        auto insert = [&](std::int32_t id, std::uint32_t distance)
        {
            if (count == 0)
            {
                Write(output, 0, id);
                Write(output, 4, distance);
                count = 1;
            }
            else
            {
                std::uint32_t index = 0;
                while (index < count && distance >= Read<std::uint32_t>(output, index * 8u + 4u)) ++index;
                if (count == limit)
                {
                    if (index == count) return;
                    for (auto shift = count - 1u; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                }
                else
                {
                    for (auto shift = count; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                    ++count;
                }
                Write(output, index * 8u, id);
                Write(output, index * 8u + 4u, distance);
            }
            if (count == limit) cutoff = Read<std::uint32_t>(output, count * 8u - 4u);
        };
        if (humans)
        {
            CHumanIterator iterator;
            for (std::uint32_t enemy = 0; enemy < enemyCount; ++enemy)
            {
                for (auto id = iterator.GetFirstSectorHumanId(enemies[enemy], Read<std::uint32_t>(point, 0), NXBasics::Runtime_TimeSigned32(maximumDistance), useSecondList); id != -1; id = iterator.GetNextSectorHumanId())
                {
                    const void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                    if (ignoreHidden && Human_Misc_IsHidden(human)) continue;
                    const void* position = Human_GetMapPositionRef(human);
                    const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2)));
                    if (distance > maximumDistance || distance < minimumDistance || !Human_IsAgeAdult(human) || distance >= cutoff) continue;
                    insert(id, distance);
                }
            }
        }
        else
        {
            CVehicleIterator iterator;
            for (std::uint32_t enemy = 0; enemy < enemyCount; ++enemy)
            {
                for (auto id = iterator.GetFirstSectorVehicleId(enemies[enemy], Read<std::uint32_t>(point, 0), NXBasics::Runtime_TimeSigned32(maximumDistance), useSecondList); id != -1; id = iterator.GetNextSectorVehicleId())
                {
                    const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
                    const void* position = Vehicle_GetMapPositionRef(vehicle);
                    const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2)));
                    if (distance > maximumDistance || distance < minimumDistance || distance >= cutoff) continue;
                    insert(id, distance);
                }
            }
        }
        return count;
    }

    // Original function: FUN_0043a4b1; the first candidate is included even when threshold addition wraps.
    std::int32_t Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold, bool humans, bool ignoreHidden)
    {
        void* output = NXBasics::Runtime_AllocateWithNewHandler(limit * 8u, 1);
        const auto count = Tool_ObjectId_GetNextEnemyMapMoveableInRange(player, point, minimumDistance, maximumDistance, useSecondList, limit, output, humans, ignoreHidden);
        std::int32_t result = -1;
        if (count != 0)
        {
            std::uint32_t eligible = 1;
            const auto cutoff = Read<std::uint32_t>(output, 4) + distanceThreshold;
            while (eligible < count && Read<std::uint32_t>(output, eligible * 8u + 4u) <= cutoff) ++eligible;
            result = Read<std::int32_t>(output, (Game_Random_GetNumber() % eligible) * 8u);
        }
        NXBasics::Runtime_Free(output);
        return result;
    }

    // Original function: FUN_0043a529; neutral player 20 is included unless it is the querying player.
    std::uint32_t Tool_Animals_GetNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, void* output)
    {
        std::uint32_t enemies[21];
        std::uint32_t enemyCount = 0;
        for (std::uint32_t enemy = 0; enemy <= 20; ++enemy)
        {
            if (enemy == player) continue;
            if (enemy != 20 && Read<std::uint8_t>(gGameControl, enemy * NC2Layout::RecordSize::GameControlPlayer) == 0) continue;
            if (player != 20 && enemy != 20 && GameControl_Diplomacy_GetState(gGameControl, player, enemy) != 3) continue;
            enemies[enemyCount++] = enemy;
        }
        if (enemyCount == 0) return 0;
        std::uint32_t count = 0;
        auto cutoff = maximumDistance + 1u;
        CAnimalIterator iterator;
        // Native output entries are {32-bit object ID, unsigned distance}; equal distances stay in discovery order.
        auto insert = [&](std::int32_t id, std::uint32_t distance)
        {
            if (count == 0)
            {
                Write(output, 0, id);
                Write(output, 4, distance);
                count = 1;
            }
            else
            {
                std::uint32_t index = 0;
                while (index < count && distance >= Read<std::uint32_t>(output, index * 8u + 4u)) ++index;
                if (count == limit)
                {
                    if (index == count) return;
                    for (auto shift = count - 1u; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                }
                else
                {
                    for (auto shift = count; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                    ++count;
                }
                Write(output, index * 8u, id);
                Write(output, index * 8u + 4u, distance);
            }
            if (count == limit) cutoff = Read<std::uint32_t>(output, count * 8u - 4u);
        };
        for (std::uint32_t enemy = 0; enemy < enemyCount; ++enemy)
        {
            for (auto id = iterator.GetFirstSectorAnimalId(enemies[enemy], Read<std::uint32_t>(point, 0), NXBasics::Runtime_TimeSigned32(maximumDistance), flag1, flag2); id != -1; id = iterator.GetNextSectorAnimalId())
            {
                const void* animal = CAnimalArray::mStaticVars + static_cast<std::uint32_t>(id) * CAnimalArray::ElementSize;
                const void* position = Animal_GetMapPositionRef(animal);
                const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2)));
                if (distance > maximumDistance || distance < minimumDistance || distance >= cutoff) continue;
                insert(id, distance);
            }
        }
        return count;
    }

    // Original function: FUN_0043a72d; the first candidate is included even when threshold addition wraps.
    std::int32_t Tool_Animals_GetOneOfNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, std::uint32_t distanceThreshold)
    {
        void* output = NXBasics::Runtime_AllocateWithNewHandler(limit * 8u, 1);
        const auto count = Tool_Animals_GetNextEnemyInRange(player, point, minimumDistance, maximumDistance, flag1, flag2, limit, output);
        std::int32_t result = -1;
        if (count != 0)
        {
            std::uint32_t eligible = 1;
            const auto cutoff = Read<std::uint32_t>(output, 4) + distanceThreshold;
            while (eligible < count && Read<std::uint32_t>(output, eligible * 8u + 4u) <= cutoff) ++eligible;
            result = Read<std::int32_t>(output, (Game_Random_GetNumber() % eligible) * 8u);
        }
        NXBasics::Runtime_Free(output);
        return result;
    }

    // Original function: FUN_0043a7a2; destroyed houses are excluded before range and insertion checks.
    std::uint32_t Tool_GetNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output)
    {
        std::uint32_t enemies[20];
        std::uint32_t enemyCount = 0;
        for (std::uint32_t enemy = 0; enemy < 20; ++enemy)
        {
            if (Read<std::uint8_t>(gGameControl, enemy * NC2Layout::RecordSize::GameControlPlayer) != 0 && GameControl_Diplomacy_GetState(gGameControl, player, enemy) == 3) enemies[enemyCount++] = enemy;
        }
        if (enemyCount == 0) return 0;
        std::uint32_t count = 0;
        auto cutoff = maximumDistance + 1u;
        CHouseIterator iterator;
        // Native output entries are {32-bit object ID, unsigned distance}; equal distances stay in discovery order.
        auto insert = [&](std::int32_t id, std::uint32_t distance)
        {
            if (count == 0)
            {
                Write(output, 0, id);
                Write(output, 4, distance);
                count = 1;
            }
            else
            {
                std::uint32_t index = 0;
                while (index < count && distance >= Read<std::uint32_t>(output, index * 8u + 4u)) ++index;
                if (count == limit)
                {
                    if (index == count) return;
                    for (auto shift = count - 1u; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                }
                else
                {
                    for (auto shift = count; shift > index; --shift)
                    {
                        Write(output, shift * 8u, Read<std::uint32_t>(output, (shift - 1u) * 8u));
                        Write(output, shift * 8u + 4u, Read<std::uint32_t>(output, (shift - 1u) * 8u + 4u));
                    }
                    ++count;
                }
                Write(output, index * 8u, id);
                Write(output, index * 8u + 4u, distance);
            }
            if (count == limit) cutoff = Read<std::uint32_t>(output, count * 8u - 4u);
        };
        for (std::uint32_t enemy = 0; enemy < enemyCount; ++enemy)
        {
            for (auto id = iterator.GetFirstSectorHouseId(enemies[enemy], point, maximumDistance, useSecondList); id != -1; id = iterator.GetNextSectorHouseId())
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (Read<std::uint8_t>(house, NC2Layout::HouseOffset::DemolitionFlag) != 0) continue;
                const void* position = static_cast<const std::byte*>(house) + NC2Layout::HouseOffset::MapPosition;
                const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2), Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2)));
                if (distance > maximumDistance || distance < minimumDistance || distance >= cutoff) continue;
                insert(id, distance);
            }
        }
        return count;
    }

    // Original function: FUN_0043a9b1; the first candidate is included even when threshold addition wraps.
    std::int32_t Tool_GetOneOfNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold)
    {
        void* output = NXBasics::Runtime_AllocateWithNewHandler(limit * 8u, 1);
        const auto count = Tool_GetNextEnemyHousesInRange(player, point, minimumDistance, maximumDistance, useSecondList, limit, output);
        std::int32_t result = -1;
        if (count != 0)
        {
            std::uint32_t eligible = 1;
            const auto cutoff = Read<std::uint32_t>(output, 4) + distanceThreshold;
            while (eligible < count && Read<std::uint32_t>(output, eligible * 8u + 4u) <= cutoff) ++eligible;
            result = Read<std::int32_t>(output, (Game_Random_GetNumber() % eligible) * 8u);
        }
        NXBasics::Runtime_Free(output);
        return result;
    }

}
