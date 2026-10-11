#include "C2AIConditions.h"
#include "C2AnimalAccess.h"
#include "C2GameControlNetwork.h"
#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2OverviewObjectIterators.h"
#include "C2PlayerStatisticAccess.h"
#include "C2VehicleAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0042a520.
    bool AIManager_EntryPoint_Condition_Set(void* manager, std::uint32_t player, std::uint32_t index, const void* condition)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_Condition_Set(handler, index, condition);
    }

    // Original function: FUN_0042a564.
    bool AIManager_EntryPoint_SetUnitLimit(void* manager, std::uint32_t player, std::uint32_t limit)
    {
        void* handler = Read<void*>(manager, player * 4u);
        if (handler == nullptr) return false;
        Write(handler, 0xAD38, limit);
        return true;
    }

    // Original function: FUN_0042a582.
    bool AIManager_EntryPoint_SetMaxUnitLimit(void* manager, std::uint32_t player, std::uint32_t limit)
    {
        void* handler = Read<void*>(manager, player * 4u);
        if (handler == nullptr) return false;
        Write(handler, 0xAD3C, limit);
        return true;
    }

    // Original function: FUN_0042a5a0; existing mission-action entry belongs to this condition block.
    bool AIManager_EntryPoint_Condition_ExternalActivate_SetFlag(void* manager, std::uint32_t player, std::uint32_t index, bool active)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_EntryPoint_Condition_ExternalActivate_SetFlag(handler, index, active);
    }

    // Original function: FUN_0042b5ed; only NOT and XOR constrain the number of operands.
    bool AIConditionToConditionCheck_Tool_CheckNumberOfConditions(const void* check)
    {
        const auto operation = Read<std::uint32_t>(check, 0);
        if (operation == 3) return Read<std::uint32_t>(check, 4) == 1;
        if (operation == 4) return Read<std::uint32_t>(check, 4) == 2;
        return true;
    }

    // Original function: FUN_0042b60c.
    void AICondition_Init(void* condition, std::uint32_t type, std::uint8_t flag)
    {
        NXBasics::Runtime_FillMemory(condition, 0, 0x48);
        Write(condition, 0x0C, type);
        Write(condition, 0x10, flag);
        Write<std::uint32_t>(condition, 4, 0xFFFFFFFFu);
        Write<std::uint32_t>(condition, 8, 0xFFFFFFFFu);
        Write(condition, 0x14, gGameTime.tick);
    }

    // Original function: FUN_0042b63e; the stored flag may inhibit a transition away from active.
    bool AICondition_SetCondition(void* condition, std::uint8_t active)
    {
        const auto previous = Read<std::uint8_t>(condition, 0);
        if (previous == active || (previous != 0 && Read<std::uint8_t>(condition, 0x10) != 0)) return false;
        Write(condition, 0, active);
        Write(condition, active != 0 ? 4u : 8u, gGameTime.tick);
        return true;
    }

    // Original function: FUN_00458dc4; preserve the raw active byte for XOR conditions.
    std::uint8_t AISinglePlayerHandler_Condition_IsActive(const void* handler, std::uint32_t index)
    {
        if (index == 100000) return 1;
        if (index == 100001 || index >= 100 || Read<std::uint32_t>(handler, index * 0x48u + 0x18u) == 0) return 0;
        return Read<std::uint8_t>(handler, index * 0x48u + 0x0Cu);
    }

    namespace
    {
        // Entire Windows DWORD region 00500F44..00500F7B, indexed by condition type.
        constexpr std::uint32_t gAIConditionCheckIntervals[] = {0, 1, 1, 1, 1, 1, 10, 10, 1, 1, 1, 1, 1, 0xFFFFFFFFu};
    }

    // Original function: FUN_00458df6; do at most ten passes, repeating while a condition changes.
    bool AISinglePlayerHandler_Condition_RecheckAll(void* handler, std::uint32_t tick)
    {
        bool anyChanged = false;
        for (std::uint32_t pass = 0; pass < 10; ++pass)
        {
            bool changed = false;
            std::uint32_t index = 0;
            do
            {
                void* condition = static_cast<std::byte*>(handler) + 0x0Cu + index * 0x48u;
                const auto type = Read<std::uint32_t>(condition, 0x0C);
                if (type != 0 && (tick == 0 || tick % gAIConditionCheckIntervals[type] == 0)) changed |= AISinglePlayerHandler_l_Condition_CheckForConditionChange(handler, condition);
                ++index;
            } while (index <= Read<std::uint32_t>(handler, 8));
            anyChanged |= changed;
            if (!changed) break;
        }
        return anyChanged;
    }

    // Original function: FUN_00458e5f; all twelve Windows jump-table branches at 00459260 are retained.
    bool AISinglePlayerHandler_l_Condition_CheckForConditionChange(void* handler, void* condition)
    {
        const auto type = Read<std::uint32_t>(condition, 0x0C);
        std::uint8_t active = 0;
        switch (type)
        {
        case 1: active = 1; break;
        case 2: active = gGameTime.tick >= Read<std::uint32_t>(condition, 0x18); break;
        case 3:
        {
            const auto operation = Read<std::uint32_t>(condition, 0x18);
            if (operation == 4)
            {
                const auto first = AISinglePlayerHandler_Condition_IsActive(handler, Read<std::uint32_t>(condition, 0x20));
                active = first ^ AISinglePlayerHandler_Condition_IsActive(handler, Read<std::uint32_t>(condition, 0x24));
            }
            else if (operation == 3) active = AISinglePlayerHandler_Condition_IsActive(handler, Read<std::uint32_t>(condition, 0x20)) == 0;
            else if (operation == 1 || operation == 2)
            {
                std::uint32_t index = 0;
                const auto count = Read<std::uint32_t>(condition, 0x1C);
                while (index < count)
                {
                    const auto value = AISinglePlayerHandler_Condition_IsActive(handler, Read<std::uint32_t>(condition, 0x20u + index * 4u));
                    if ((operation == 1 && value == 0) || (operation == 2 && value != 0)) break;
                    ++index;
                }
                active = operation == 1 ? index == count : index != count;
            }
            else return false;
            break;
        }
        case 4:
        {
            const auto index = Read<std::uint32_t>(condition, 0x18);
            const void* reference = static_cast<const std::byte*>(handler) + index * 0x48u + 0x0Cu;
            if (Read<std::uint32_t>(reference, 0x0C) != 0)
            {
                const auto time = Read<std::uint32_t>(reference, Read<std::uint8_t>(condition, 0x1C) != 0 ? 4u : 8u);
                if (time != 0xFFFFFFFFu) active = gGameTime.tick >= time + Read<std::uint32_t>(condition, 0x20);
            }
            break;
        }
        case 5: active = static_cast<std::uint32_t>(GameControl_Diplomacy_GetState(gGameControl, Read<std::int32_t>(condition, 0x18), Read<std::int32_t>(condition, 0x1C))) == Read<std::uint32_t>(condition, 0x20); break;
        case 6:
        {
            std::uint32_t center;
            MapMigPoint_Construct(&center, Read<std::int16_t>(condition, 0x18), Read<std::int16_t>(condition, 0x1A), false);
            CMapMoveableIterator iterator;
            for (auto id = iterator.GetFirstMapMoveableId(); id != -1; id = iterator.GetNextMapMoveableId())
            {
                const void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                const auto moveableType = Read<std::uint32_t>(moveable, 8);
                std::uint32_t player = 20, position = 0;
                std::uint8_t military = 0;
                if (moveableType == 1)
                {
                    const void* human = GetHumanPtr(Read<std::int32_t>(moveable, 0x54));
                    player = Read<std::uint32_t>(human, 0x10);
                    military = Human_IsJobSoldier(human);
                    position = Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
                }
                else if (moveableType == 2)
                {
                    const void* animal = GetAnimalPtr(Read<std::int32_t>(moveable, 0x54));
                    player = Read<std::uint32_t>(animal, 0x10);
                    if (player == 20) continue;
                    if (Read<std::uint32_t>(animal, 0x18) == 49) military = Read<std::uint8_t>(animal, 0x5C);
                    position = Read<std::uint32_t>(Animal_GetMapPositionRef(animal), 0);
                }
                else if (moveableType == 3)
                {
                    const void* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(moveable, 0x54) * CVehicleArray::ElementSize;
                    player = Read<std::uint32_t>(vehicle, 0x1E0);
                    military = Read<std::uint32_t>(vehicle, 0x1EC) == 54;
                    position = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
                }
                else if (moveableType == 4) continue;
                if (player == 0xFFFFFFFFu) continue;
                const auto owner = Read<std::uint32_t>(handler, 0), requestedPlayer = Read<std::uint32_t>(condition, 0x20);
                if (player == owner && requestedPlayer != owner) continue;
                if (requestedPlayer != 20 && requestedPlayer != player) continue;
                if (Read<std::uint8_t>(condition, 0x24) != 0 && GameControl_Diplomacy_GetState(gGameControl, owner, player) != 3) continue;
                if (Read<std::uint8_t>(condition, 0x25) != 0 && military == 0) continue;
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(static_cast<std::uint16_t>(position), static_cast<std::uint16_t>(position >> 16), static_cast<std::uint16_t>(center), static_cast<std::uint16_t>(center >> 16))) < Read<std::uint32_t>(condition, 0x1C)) { active = 1; break; }
            }
            break;
        }
        case 7:
        {
            std::uint32_t center;
            MapMigPoint_Construct(&center, Read<std::int16_t>(condition, 0x18), Read<std::int16_t>(condition, 0x1A), false);
            CHouseIterator iterator;
            const auto player = Read<std::uint32_t>(condition, 0x20);
            auto id = player == 20 ? iterator.GetFirstHouseId() : iterator.GetFirstHouseId(player);
            for (; id != -1; id = iterator.GetNextHouseId())
            {
                const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                const auto housePlayer = Read<std::uint32_t>(house, 0x0C);
                if (player == 20 && housePlayer == Read<std::uint32_t>(handler, 0)) continue;
                if (Read<std::uint8_t>(condition, 0x2C) != 0 && Read<std::uint32_t>(house, 0x2C) != 3) continue;
                const auto houseType = Read<std::uint32_t>(condition, 0x28);
                if (houseType != 0 && Read<std::uint32_t>(house, 0x14) != houseType) continue;
                if (Read<std::uint8_t>(condition, 0x24) != 0 && GameControl_Diplomacy_GetState(gGameControl, Read<std::int32_t>(handler, 0), housePlayer) != 3) continue;
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(house, 0x1C), Read<std::uint16_t>(house, 0x1E), static_cast<std::uint16_t>(center), static_cast<std::uint16_t>(center >> 16))) < Read<std::uint32_t>(condition, 0x1C)) { active = 1; break; }
            }
            break;
        }
        case 8: active = Read<std::uint8_t>(gGameControl, Read<std::uint32_t>(condition, 0x18) * 0x4A80u + Read<std::uint32_t>(condition, 0x1C) + 0x45F8u); break;
        case 9:
            if (gGameTime.tick <= 720) return false;
            active = Read<std::uint16_t>(PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, Read<std::uint32_t>(condition, 0x18)), 0) == 0;
            break;
        case 10: active = Read<std::uint16_t>(PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, Read<std::uint32_t>(condition, 0x1C)), 0x12) >= Read<std::uint32_t>(condition, 0x18); break;
        case 11: active = Read<std::uint8_t>(condition, 0x18); break;
        case 12:
            if (Read<std::uint8_t>(condition, 0) != 0) active = Read<std::uint32_t>(condition, 4) + Read<std::uint32_t>(condition, 0x1C) > gGameTime.tick;
            else if (Read<std::uint32_t>(condition, 4) != 0xFFFFFFFFu) active = Read<std::uint32_t>(condition, 8) + Read<std::uint32_t>(condition, 0x20) < gGameTime.tick;
            else active = Read<std::uint32_t>(condition, 0x14) + Read<std::uint32_t>(condition, 0x18) < gGameTime.tick;
            break;
        default: return false;
        }
        return AICondition_SetCondition(condition, active);
    }

    // Original function: FUN_00459290; invalid parameters create a disabled type-13 record, occupied slots reject replacement.
    bool AISinglePlayerHandler_Condition_Set(void* handler, std::uint32_t index, const void* condition)
    {
        if (index >= 100) return false;
        bool valid = true;
        const auto type = Read<std::uint32_t>(condition, 0x0C);
        auto playerIsValid = [](std::uint32_t player) { return NXBasics::Runtime_TimeSigned32(player) < 20 && Read<std::uint8_t>(gGameControl, player * 0x4A80u) != 0; };
        if (type == 3)
        {
            const auto count = Read<std::uint32_t>(condition, 0x1C);
            for (std::uint32_t operand = 0; operand < count; ++operand)
            {
                const auto reference = Read<std::uint32_t>(condition, 0x20u + operand * 4u);
                if (reference >= 100 && reference != 100000 && reference != 100001 && reference != 999999) return false;
            }
            valid = AIConditionToConditionCheck_Tool_CheckNumberOfConditions(static_cast<const std::byte*>(condition) + 0x18);
        }
        else if (type == 4) { if (Read<std::uint32_t>(condition, 0x18) >= 100) return false; }
        else if (type == 5)
        {
            // Windows still checks the second player after finding the first invalid.
            valid = playerIsValid(Read<std::uint32_t>(condition, 0x18));
            if (!playerIsValid(Read<std::uint32_t>(condition, 0x1C))) valid = false;
        }
        else if (type == 6 || type == 7)
        {
            const auto player = Read<std::uint32_t>(condition, 0x20);
            if (player != 20) valid = playerIsValid(player);
        }
        else if (type == 9) valid = playerIsValid(Read<std::uint32_t>(condition, 0x18));
        else if (type == 10) valid = playerIsValid(Read<std::uint32_t>(condition, 0x1C));
        void* target = static_cast<std::byte*>(handler) + index * 0x48u + 0x0Cu;
        if (Read<std::uint32_t>(target, 0x0C) != 0) return false;
        if (!valid) AICondition_Init(target, 13, 0);
        else
        {
            // Native REP MOVSD copies forward, including when records overlap.
            for (std::uint32_t offset = 0; offset < 0x48; offset += 4) Write(target, offset, Read<std::uint32_t>(condition, offset));
        }
        if (index > Read<std::uint32_t>(handler, 8)) Write(handler, 8, index);
        return true;
    }

    // Original function: FUN_004593c6; only ExternalActivate condition records accept a flag.
    bool AISinglePlayerHandler_EntryPoint_Condition_ExternalActivate_SetFlag(void* handler, std::uint32_t index, bool active)
    {
        if (index >= 100 || Read<std::uint32_t>(handler, index * 0x48u + 0x18u) != 11) return false;
        Write<std::uint8_t>(handler, index * 0x48u + 0x24u, active);
        return true;
    }
}
