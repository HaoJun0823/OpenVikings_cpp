#include "C2AIMovementMessages.h"
#include "C2GameControlNetwork.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanCommandAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapSectorGridCursor.h"
#include "C2MapWalkSectorManager.h"
#include "C2VehicleAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        // Complete native static region 00554D4C..00554D57: last point, initialization byte/padding, last tick.
        struct SNoWayMessageState { std::uint32_t point; std::uint8_t initialized; std::byte padding[3]; std::uint32_t tick; };
        static_assert(sizeof(SNoWayMessageState) == 12);
        SNoWayMessageState gNoWayMessageState{};
        // Original function: FUN_00472F48; the original empty point destructor returns no value.
        int __cdecl NoWayMessagePoint_OnExit() { return 0; }
        void* Soldier(void* handler, std::uint32_t slot) { return static_cast<std::byte*>(handler) + 0x616Cu + slot * 0x54u; }
        const void* Soldier(const void* handler, std::uint32_t slot) { return static_cast<const std::byte*>(handler) + 0x616Cu + slot * 0x54u; }
        std::uint32_t Distance(const void* a, const void* b) { return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(a, 0), Read<std::int16_t>(a, 2), Read<std::int16_t>(b, 0), Read<std::int16_t>(b, 2))); }
    }

    // Original function: FUN_0045B02A; only the single-soldier leader id is searched.
    std::int32_t AISinglePlayerHandler_l_SingleSoldier_GetSlotIdByCreatureId(const void* handler, std::uint32_t human)
    {
        const auto count = Read<std::uint32_t>(handler, 0x6168);
        for (std::uint32_t slot = 0; slot < count; ++slot) if (Read<std::uint32_t>(Soldier(handler, slot), 0) == human) return NXBasics::Runtime_TimeSigned32(slot);
        return -1;
    }

    // Original function: FUN_0045B346; unsigned distances and native 3/2 defence margins are retained.
    bool AISinglePlayerHandler_l_SingleSoldier_CheckIfIsInTaskRange(const void* handler, std::uint32_t slot, bool defence)
    {
        const auto* soldier = Soldier(handler, slot);
        const auto* position = Human_GetMapPositionRef(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0))));
        if (Read<std::uint8_t>(soldier, 0x10) != 0)
        {
            const auto range = defence ? 25u : Read<std::uint32_t>(handler, 0xA334) * 8u;
            return Distance(position, static_cast<const std::byte*>(handler) + 0xA32E) <= range;
        }
        const auto attachment = Read<std::uint32_t>(soldier, 0x38);
        if (attachment == 0xFFFFFFFFu) return true;
        const auto* task = static_cast<const std::byte*>(handler) + 0x2A50u + attachment * 0x58u;
        const auto* main = static_cast<const std::byte*>(handler) + 0x1C30u + Read<std::uint32_t>(task, 0x14) * 0x44u;
        const auto type = Read<std::uint32_t>(main, 8);
        if (type == 0) return true;
        std::uint32_t range, center;
        if (type == 2 && Read<std::uint32_t>(task, 0x48) == 1)
        {
            range = Read<std::uint32_t>(task, 0x4C);
            center = Read<std::uint32_t>(task, 0x50);
        }
        else
        {
            MapMigPoint_SetPosition(&center, Read<std::int16_t>(main, 0xC), Read<std::int16_t>(main, 0xE), false);
            range = Read<std::uint32_t>(main, 0x10);
        }
        if (defence) range = (range * 3u) >> 1;
        return Distance(&center, position) <= range;
    }

    // Original function: FUN_0045B4B6.
    void AISinglePlayerHandler_l_SingleSoldier_StartSelfDefenceMode(void* handler, std::uint32_t slot)
    {
        auto* soldier = Soldier(handler, slot);
        const auto* human = GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0)));
        Write<std::uint8_t>(soldier, 0x44, 1);
        Write(handler, 0xAD48u + slot * 4u, Read<std::uint32_t>(Human_GetMapPositionRef(human), 0));
    }

    // Original function: FUN_0045AB88.
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHuman(void* handler, std::uint32_t slot, std::uint32_t target, bool defence)
    {
        auto* soldier = Soldier(handler, slot);
        const auto* enemy = CHumanArray::mStaticVars + target * CHumanArray::ElementSize;
        if (Read<std::uint32_t>(soldier, 0x34) == 14 && Read<std::uint32_t>(soldier, 0x48) == 1 && Read<std::uint32_t>(soldier, 0x50) == Read<std::uint32_t>(enemy, 8) && Read<std::uint8_t>(soldier, 0x44) == static_cast<std::uint8_t>(defence)) return true;
        auto* human = const_cast<void*>(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0))));
        const auto player = Read<std::uint32_t>(enemy, 0x10);
        GameControl_Diplomacy_SetState(gGameControl, nullptr, Read<std::uint32_t>(handler, 0), player, 3);
        alignas(4) std::byte command[20]{};
        Write<std::uint8_t>(command, 0, 43);
        Write<std::uint8_t>(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)));
        Write(command, 0xC, target);
        Write(command, 0x10, Read<std::uint32_t>(enemy, 8));
        Human_DoAddNewUserCommand(human, command);
        Write<std::uint32_t>(soldier, 0x34, 14);
        Write(soldier, 0x4C, target);
        Write(soldier, 0x50, Read<std::uint32_t>(enemy, 8));
        Write<std::uint32_t>(soldier, 0x48, 1);
        if (defence) AISinglePlayerHandler_l_SingleSoldier_StartSelfDefenceMode(handler, slot);
        else Write<std::uint8_t>(soldier, 0x44, 0);
        return true;
    }

    // Original function: FUN_0045AC58.
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_AttackAnimal(void* handler, std::uint32_t slot, std::uint32_t target, bool defence)
    {
        auto* soldier = Soldier(handler, slot);
        const auto* enemy = CAnimalArray::mStaticVars + target * CAnimalArray::ElementSize;
        if (Read<std::uint32_t>(soldier, 0x34) == 14 && Read<std::uint32_t>(soldier, 0x48) == 3 && Read<std::uint32_t>(soldier, 0x50) == Read<std::uint32_t>(enemy, 8) && Read<std::uint8_t>(soldier, 0x44) == static_cast<std::uint8_t>(defence)) return true;
        auto* human = const_cast<void*>(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0))));
        const auto player = Read<std::uint32_t>(enemy, 0x10);
        if (player != 20) GameControl_Diplomacy_SetState(gGameControl, nullptr, Read<std::uint32_t>(handler, 0), player, 3);
        alignas(4) std::byte command[20]{};
        Write<std::uint8_t>(command, 0, 45);
        Write<std::uint8_t>(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)));
        Write(command, 0xC, target);
        Write(command, 0x10, Read<std::uint32_t>(enemy, 8));
        Human_DoAddNewUserCommand(human, command);
        Write<std::uint32_t>(soldier, 0x34, 14);
        Write(soldier, 0x4C, target);
        Write(soldier, 0x50, Read<std::uint32_t>(enemy, 8));
        Write<std::uint32_t>(soldier, 0x48, 3);
        if (defence) AISinglePlayerHandler_l_SingleSoldier_StartSelfDefenceMode(handler, slot);
        else Write<std::uint8_t>(soldier, 0x44, 0);
        return true;
    }

    // Original function: FUN_0045AD35.
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_AttackVehicle(void* handler, std::uint32_t slot, std::uint32_t target, bool defence)
    {
        auto* soldier = Soldier(handler, slot);
        const auto* enemy = CVehicleArray::mStaticVars + target * CVehicleArray::ElementSize;
        if (Read<std::uint32_t>(soldier, 0x34) == 14 && Read<std::uint32_t>(soldier, 0x48) == 2 && Read<std::uint32_t>(soldier, 0x50) == Read<std::uint32_t>(enemy, 8) && Read<std::uint8_t>(soldier, 0x44) == static_cast<std::uint8_t>(defence)) return true;
        auto* human = const_cast<void*>(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0))));
        const auto player = Read<std::uint32_t>(enemy, 0x1E0);
        GameControl_Diplomacy_SetState(gGameControl, nullptr, Read<std::uint32_t>(handler, 0), player, 3);
        alignas(4) std::byte command[20]{};
        Write<std::uint8_t>(command, 0, 46);
        Write<std::uint8_t>(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)));
        Write(command, 0xC, target);
        Write(command, 0x10, Read<std::uint32_t>(enemy, 8));
        Human_DoAddNewUserCommand(human, command);
        Write<std::uint32_t>(soldier, 0x34, 14);
        Write(soldier, 0x4C, target);
        Write(soldier, 0x50, Read<std::uint32_t>(enemy, 8));
        Write<std::uint32_t>(soldier, 0x48, 2);
        if (defence) AISinglePlayerHandler_l_SingleSoldier_StartSelfDefenceMode(handler, slot);
        else Write<std::uint8_t>(soldier, 0x44, 0);
        return true;
    }

    // Original function: FUN_0045AE0F.
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(void* handler, std::uint32_t slot, std::uint32_t target, bool defence)
    {
        auto* soldier = Soldier(handler, slot);
        const auto* enemy = CHouseArray::mStaticVars + target * CHouseArray::ElementSize;
        if (Read<std::uint32_t>(soldier, 0x34) == 14 && Read<std::uint32_t>(soldier, 0x48) == 4 && Read<std::uint32_t>(soldier, 0x50) == Read<std::uint32_t>(enemy, 8) && Read<std::uint8_t>(soldier, 0x44) == static_cast<std::uint8_t>(defence)) return true;
        auto* human = const_cast<void*>(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(soldier, 0))));
        const auto player = Read<std::uint32_t>(enemy, 0xC);
        GameControl_Diplomacy_SetState(gGameControl, nullptr, Read<std::uint32_t>(handler, 0), player, 3);
        alignas(4) std::byte command[20]{};
        Write<std::uint8_t>(command, 0, 44);
        Write<std::uint8_t>(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)));
        Write(command, 0xC, target);
        Write(command, 0x10, Read<std::uint32_t>(enemy, 8));
        Human_DoAddNewUserCommand(human, command);
        Write<std::uint32_t>(soldier, 0x34, 14);
        Write(soldier, 0x4C, target);
        Write(soldier, 0x50, Read<std::uint32_t>(enemy, 8));
        Write<std::uint32_t>(soldier, 0x48, 4);
        if (defence) AISinglePlayerHandler_l_SingleSoldier_StartSelfDefenceMode(handler, slot);
        else Write<std::uint8_t>(soldier, 0x44, 0);
        return true;
    }

    // Original function: FUN_00472CC0.
    bool AISingleSoldier_Tool_IsSelfDefending(const void* soldier) { return Read<std::uint32_t>(soldier, 0x34) == 14 && Read<std::uint8_t>(soldier, 0x44) != 0 && TargetData_IsValid(static_cast<const std::byte*>(soldier) + 0x48); }

    // Original function: FUN_00472CDE; the throttle is shared across all player handlers.
    void AISinglePlayerHandler_Messages_NoWayFound(void* handler, const void* source, const void* target)
    {
        if ((gNoWayMessageState.initialized & 1u) == 0)
        {
            gNoWayMessageState.initialized |= 1u;
            gNoWayMessageState.point = 0;
            NXBasics::Runtime_AtExit(NoWayMessagePoint_OnExit);
        }
        if (gNoWayMessageState.point == Read<std::uint32_t>(target, 0) && gNoWayMessageState.tick + 12u > gGameTime.tick) return;
        for (std::uint32_t index = 0; index <= Read<std::uint32_t>(handler, 0x1C2C); ++index)
        {
            auto* task = static_cast<std::byte*>(handler) + 0x1C30u + index * 0x44u;
            const auto type = Read<std::uint32_t>(task, 8);
            if (type != 1 && type != 2) continue;
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, Read<std::int16_t>(task, 0xC), Read<std::int16_t>(task, 0xE), false);
            if (point == Read<std::uint32_t>(target, 0) && (type == 1 || Read<std::uint8_t>(task, 0x34) != 1 || Read<std::uint32_t>(task, 0x38) == 1))
            {
                const auto sourceContinent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6);
                if (sourceContinent == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(target), 6) && AISinglePlayerHandler_l_Messages_GetNewValidPosition(handler, source, &point))
                {
                    Write<std::uint8_t>(task, 0x34, 1);
                    Write<std::uint32_t>(task, 0x38, type == 1 ? 3u : 1u);
                    Write(task, 0x3C, Read<std::uint32_t>(source, 0));
                    Write(task, 0x40, Read<std::uint32_t>(target, 0));
                    Write(task, 0xC, point);
                }
            }
            // For a type-2 task an incompatible pending mode skips the second endpoint too.
            else if (type == 2 && point == Read<std::uint32_t>(target, 0) && Read<std::uint8_t>(task, 0x34) == 1 && Read<std::uint32_t>(task, 0x38) != 1) continue;
            if (type != 2) continue;
            MapMigPoint_Construct(&point, Read<std::int16_t>(task, 0x1C), Read<std::int16_t>(task, 0x1E), false);
            if (point != Read<std::uint32_t>(target, 0) || (Read<std::uint8_t>(task, 0x34) == 1 && Read<std::uint32_t>(task, 0x38) != 2)) continue;
            const auto sourceContinent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6);
            if (sourceContinent != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(target), 6) || !AISinglePlayerHandler_l_Messages_GetNewValidPosition(handler, source, &point)) continue;
            Write<std::uint8_t>(task, 0x34, 1);
            Write<std::uint32_t>(task, 0x38, 2);
            Write(task, 0x3C, Read<std::uint32_t>(source, 0));
            Write(task, 0x40, Read<std::uint32_t>(target, 0));
            Write(task, 0x1C, point);
        }
        gNoWayMessageState.point = Read<std::uint32_t>(target, 0);
        gNoWayMessageState.tick = gGameTime.tick;
    }

    // Original function: FUN_00472F49; preserve the cumulative square cursor and first successful ring.
    bool AISinglePlayerHandler_l_Messages_GetNewValidPosition(void* handler, const void* source, void* target)
    {
        (void)handler;
        const SSectorGridCursor origin(Read<std::uint32_t>(source, 0));
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6);
        if (CMapWalkSectorManager::GetLandSectorDataPtr(origin)->continentId != continent) return false;
        SSectorGridCursor cursor(Read<std::uint32_t>(target, 0));
        std::uint32_t bestDistance = 10000, bestPoint = 0;
        for (std::uint32_t radius = 2; radius < 10; radius += 2)
        {
            cursor.MoveCursorUnwrapped(5);
            for (std::uint32_t direction = 0; direction < 8; direction += 2)
                for (std::uint32_t step = 0; step < radius; ++step)
                {
                    cursor.MoveCursorUnwrapped(direction);
                    if (!cursor.IsInsideMap() || !CMapWalkSectorManager::WayPossible(origin, cursor, 0, true, 20)) continue;
                    const auto* point = &CMapWalkSectorManager::GetLandSectorDataPtr(cursor)->basePoints[0];
                    const auto distance = Distance(point, source);
                    if (distance < bestDistance) { bestDistance = distance; bestPoint = Read<std::uint32_t>(point, 0); }
                }
            if (bestDistance != 10000) { Write(target, 0, bestPoint); return true; }
        }
        return false;
    }

    // Original function: FUN_004731CD.
    void AISinglePlayerHandler_Messages_Human_NoWayFoundToEnemy(void* handler, void* human, const void* target)
    {
        auto point = Read<std::uint32_t>(target, 0);
        if (!AISinglePlayerHandler_l_Messages_GetNewValidPosition(handler, Human_GetMapPositionRef(human), &point)) return;
        const auto player = Read<std::uint32_t>(human, 0x10), revision = Read<std::uint32_t>(human, 8), id = Read<std::uint32_t>(human, 4);
        Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(player), id, revision, MapMigPoint_GetLong(&point), 0);
    }

    // Original function: FUN_0047322D.
    void AISinglePlayerHandler_Messages_Soldier_StartSelfDefence(void* handler, std::uint32_t human, const void* target)
    {
        const auto slot = AISinglePlayerHandler_l_SingleSoldier_GetSlotIdByCreatureId(handler, human);
        if (slot == -1 || AISingleSoldier_Tool_IsSelfDefending(Soldier(handler, static_cast<std::uint32_t>(slot))) || !AISinglePlayerHandler_l_SingleSoldier_CheckIfIsInTaskRange(handler, static_cast<std::uint32_t>(slot), true)) return;
        switch (TargetData_GetTargetType(target))
        {
        case 1: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHuman(handler, slot, TargetData_GetObjectId(target), true); break;
        case 2: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackVehicle(handler, slot, TargetData_GetObjectId(target), true); break;
        case 3: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackAnimal(handler, slot, TargetData_GetObjectId(target), true); break;
        case 4: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(handler, slot, TargetData_GetObjectId(target), true); break;
        }
    }

    // Original function: FUN_004732D2; attacked soldiers outside their task range start self defence.
    void __fastcall AISinglePlayerHandler_Messages_SoldierAttacked(void* handler, void* unusedEdx, std::uint32_t human, const void* target)
    {
        (void)unusedEdx;
        const auto slot = AISinglePlayerHandler_l_SingleSoldier_GetSlotIdByCreatureId(handler, human);
        if (slot == -1 || AISingleSoldier_Tool_IsSelfDefending(Soldier(handler, static_cast<std::uint32_t>(slot))) || AISinglePlayerHandler_l_SingleSoldier_CheckIfIsInTaskRange(handler, static_cast<std::uint32_t>(slot), true)) return;
        switch (TargetData_GetTargetType(target))
        {
        case 1: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHuman(handler, slot, TargetData_GetObjectId(target), true); break;
        case 2: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackVehicle(handler, slot, TargetData_GetObjectId(target), true); break;
        case 3: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackAnimal(handler, slot, TargetData_GetObjectId(target), true); break;
        case 4: AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(handler, slot, TargetData_GetObjectId(target), true); break;
        }
    }

    // Original function: FUN_004622D6; notifications apply only to human moveables.
    void AIManager_Message_NoWayFound(void* manager, std::uint32_t moveable, const void* target)
    {
        const auto* entry = gMapMoveableArrayStorage.entries + moveable * 0x58u;
        if (Read<std::uint32_t>(entry, 8) != 1) return;
        const auto* human = GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(entry, 0x54)));
        if (auto* handler = Read<void*>(manager, Read<std::uint32_t>(human, 0x10) * 4u)) AISinglePlayerHandler_Messages_NoWayFound(handler, Human_GetMapPositionRef(human), target);
    }

    // Original function: FUN_00462361.
    void AIManager_Message_NoWayFoundToEnemy(void* manager, std::uint32_t moveable, const void* target)
    {
        const auto* entry = gMapMoveableArrayStorage.entries + moveable * 0x58u;
        if (Read<std::uint32_t>(entry, 8) != 1) return;
        auto* human = const_cast<void*>(GetHumanPtr(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(entry, 0x54))));
        if (auto* handler = Read<void*>(manager, Read<std::uint32_t>(human, 0x10) * 4u)) AISinglePlayerHandler_Messages_Human_NoWayFoundToEnemy(handler, human, target);
    }

    // Original function: FUN_00462398.
    void AIManager_Message_StartSelfDefence(void* manager, std::uint32_t player, std::uint32_t human, const void* target)
    {
        if (auto* handler = Read<void*>(manager, player * 4u)) AISinglePlayerHandler_Messages_Soldier_StartSelfDefence(handler, human, target);
    }
}
