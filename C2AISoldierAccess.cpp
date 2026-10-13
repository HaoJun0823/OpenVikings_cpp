#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2AISoldierAccess.h"
#include "C2AIMovementMessages.h"
#include "C2AIHumanRequests.h"
#include "C2AITaskAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanEquipment.h"
#include "C2HumanJobAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2MilitaryTargetSearch.h"
#include "C2GameControlNetwork.h"
#include "C2MissionStartObjects.h"
#include "C2HumanIterator.h"
#include "C2VehicleAccess.h"
#include "C2VehicleIterator.h"
#include "C2MapMoveableAccess.h"
#include "C2MapGeometry.h"
#include "C2HumanWalking.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        std::byte* Soldier(void* handler, std::uint32_t i) { return static_cast<std::byte*>(handler) + 0x616C + i * 84; }
        std::byte* Human(std::uint32_t id) { return CHumanArray::mStaticVars + id * CHumanArray::ElementSize; }
        void CountWeapon(void* handler, const void* human)
        {
            const auto* weapon = static_cast<const std::byte*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 20)]) + Human_Equipment_GetWeaponType(human) * 0xC4;
            const auto offset = 0xA30C + Read<std::uint32_t>(weapon, 32) * 4;
            Write(handler, offset, Read<std::uint32_t>(handler, offset) + 1);
        }
        void RefreshNeeds(void* human)
        {
            // Windows constant region 004F3B38..004F3B77: maxima 10000, critical levels 1000.
            if (Read<std::int32_t>(human, 0xAA8) < 1000) Write<std::int32_t>(human, 0xAA8, 10000);
            if (Read<std::int32_t>(human, 0xAAC) < 1000) Write<std::int32_t>(human, 0xAAC, 10000);
        }
    }
    // Original function: FUN_00472c02.
    void AISingleSoldier_Init(void* soldier, std::uint32_t id)
    {
        NXBasics::Runtime_FillMemory(soldier, 0, 84);
        const auto* human = Human(id);
        Write(soldier, 0, id); Write(soldier, 4, Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId));
        Write<std::uint32_t>(soldier, 20, 0); Write<std::uint32_t>(soldier, 52, 1);
        Write(soldier, 8, Human_Equipment_GetWeaponType(human)); Write(soldier, 12, Human_Equipment_GetArmorType(human));
        Write<std::int32_t>(soldier, 56, -1);
    }
    // Original function: FUN_00472c55.
    void AISingleSoldier_Tool_StopVAAndVCTask(void* soldier)
    {
        auto* human = Human(Read<std::uint32_t>(soldier, 0));
        Write<std::uint32_t>(soldier, 20, 0); Write<std::uint32_t>(soldier, 52, 1);
        const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, NC2Layout::HumanOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveableSigned;
        std::uint32_t point = 0;
        MapMoveable_Attachment_GetNextMapPosition(moveable, &point);
        Human_DoAddNewUserCommand(human, 4, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId), 0, 0, point, 0);
        Write<std::int32_t>(soldier, 56, -1); Write<std::uint8_t>(soldier, 16, 0);
    }
    // Original function: FUN_004593ed.
    void AISinglePlayerHandler_l_SingleSoldiers_RecreateListCompletly(void* handler)
    {
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(handler) + 0x6168, 0, 0x41A4);
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(handler) + 0xAD48, 0, 0x320);
        AISinglePlayerHandler_l_SingleSoldiers_UpdateList(handler);
    }
    // Original function: FUN_00459422.
    void AISinglePlayerHandler_l_SingleSoldiers_UpdateList(void* handler)
    {
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(handler) + 0xA30C, 0, 32);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168);)
        {
            auto* soldier = Soldier(handler, i); const auto* human = Human(Read<std::uint32_t>(soldier, 0));
            const bool valid = Read<std::uint8_t>(human, NC2Layout::HumanOffset::AllocatedFlag) != 0 && (Human_IsJobSoldier(human) || Human_IsJobHero(human)) && Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId) == Read<std::uint32_t>(soldier, 4) && Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId) == Read<std::uint32_t>(handler, 0) && !Human_IsAttachedToWorkHouse(human) && !Human_IsAttachedToVehicle(human) && (Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags) & 32u) == 0;
            if (valid) { CountWeapon(handler, human); ++i; continue; }
            const auto remaining = Read<std::uint32_t>(handler, 0x6168) - i - 1;
            if (remaining != 0)
            {
                NXBasics::Runtime_MoveMemory(soldier, soldier + 84, remaining * 84);
                auto* point = static_cast<std::byte*>(handler) + 0xAD48 + i * 4;
                NXBasics::Runtime_MoveMemory(point, point + 4, remaining * 4);
            }
            Write(handler, 0x6168, Read<std::uint32_t>(handler, 0x6168) - 1);
        }
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(handler, 0), true); id != -1; id = iterator.GetNextSectorHumanId())
        {
            auto count = Read<std::uint32_t>(handler, 0x6168);
            if (count >= 200) return;
            auto* human = Human(id);
            if ((Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags) & 32u) != 0 || Human_IsAttachedToWorkHouse(human) || Human_IsAttachedToVehicle(human) || AISinglePlayerHandler_Misc_Human_CheckRequestsForCreature(handler, id)) continue;
            std::uint32_t i = 0;
            while (i < count && Read<std::uint32_t>(Soldier(handler, i), 0) != static_cast<std::uint32_t>(id)) ++i;
            if (i != count) continue;
            if (Human_IsJobSoldier(human))
            {
                std::byte command[20]{};
                Write<std::uint8_t>(command, 0, 41); Write<std::uint8_t>(command, 1, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId)); Write<std::uint32_t>(command, 12, 3);
                Human_DoAddNewUserCommand(human, command);
                Write<std::uint8_t>(command, 0, 42); Write<std::uint32_t>(command, 12, 0); Human_DoAddNewUserCommand(human, command);
            }
            AISingleSoldier_Init(Soldier(handler, count), id);
            CountWeapon(handler, human); Write(handler, 0x6168, count + 1);
        }
    }
    // Original function: FUN_0045969f.
    void AISinglePlayerHandler_l_SingleSoldiers_ClearInvalidTask(void* handler)
    {
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168); ++i)
        {
            auto* soldier = Soldier(handler, i);
            if (Read<std::uint32_t>(soldier, 20) == 0 || Read<std::uint8_t>(soldier, 16) != 0) continue;
            const auto count = Read<std::uint32_t>(handler, 0x2A48);
            std::uint32_t task = 0;
            for (; task < count; ++task)
            {
                const auto main = Read<std::uint32_t>(handler, 0x2A64 + task * 88);
                if (AITaskSimpleDescription_IsEqualTo(static_cast<std::byte*>(handler) + 0x1C38 + main * 68, soldier + 20)) { Write(soldier, 56, task); break; }
            }
            if (task == count || (Read<std::uint32_t>(handler, 0x2A50 + task * 88) == 2 && Read<std::uint32_t>(handler, 0x2A98 + task * 88) == 0)) AISingleSoldier_Tool_StopVAAndVCTask(soldier);
        }
    }
    // Original function: FUN_0045aac6.
    bool AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(void* handler, std::uint32_t slot, const void* destination, std::uint32_t radius)
    {
        auto* soldier = Soldier(handler, slot); auto* human = Human(Read<std::uint32_t>(soldier, 0));
        auto point = Read<std::uint32_t>(destination, 0);
        if (!Tool_Map_GetNotBlockedPosition(&point, Read<std::uint32_t>(soldier, 20) == 1)) return false;
        const auto* position = Human_GetMapPositionRef(human);
        if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(position[0], position[1], Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2))) >= radius)
        {
            Human_DoAddNewUserCommand(human, 4, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId), 0, 0, MapMigPoint_GetLong(&point), 0);
            Write<std::uint32_t>(soldier, 52, 2); Write(soldier, 60, Read<std::uint32_t>(destination, 0)); Write(soldier, 64, radius);
        }
        else Write<std::uint32_t>(soldier, 52, 1);
        return true;
    }
    // Original function: FUN_0045aee6.
    void AISinglePlayerHandler_l_SingleSoldiers_RefreshAllEnergyAndCondition(void* handler)
    {
        const auto player = Read<std::uint32_t>(handler, 0);
        CHumanIterator humans;
        for (auto id = humans.GetFirstSectorHumanId(player, true); id != -1; id = humans.GetNextSectorHumanId()) RefreshNeeds(Human(id));
        CVehicleIterator vehicles;
        for (auto id = vehicles.GetFirstSectorVehicleId(player, 0, 10000, true); id != -1; id = vehicles.GetNextSectorVehicleId())
        {
            auto* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) != 0) RefreshNeeds(Vehicle_Passengers_GetAttachedCommanderPtr(vehicle));
        }
    }
    // Original function: FUN_00473377.
    void AISinglePlayerHandler_l_Vehicles_RecreateListCompletly(void* handler)
    {
        Write<std::uint32_t>(handler, 0x5CB4, 0); AISinglePlayerHandler_l_Vehicles_UpdateList(handler);
    }
    // Original function: FUN_00473383.
    void AISinglePlayerHandler_l_Vehicles_UpdateList(void* handler)
    {
        auto* entries = static_cast<std::byte*>(handler) + 0x5CB8;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x5CB4);)
        {
            auto* entry = entries + i * 28;
            const auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(entry, 0) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AllocatedFlag) != 0 && Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId) == Read<std::uint32_t>(entry, 4) && Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId) == Read<std::uint32_t>(handler, 0)) { ++i; continue; }
            const auto remaining = Read<std::uint32_t>(handler, 0x5CB4) - i - 1;
            if (remaining != 0) NXBasics::Runtime_MoveMemory(entry, entry + 28, remaining * 28);
            Write(handler, 0x5CB4, Read<std::uint32_t>(handler, 0x5CB4) - 1);
        }
        CVehicleIterator iterator;
        for (auto id = iterator.GetFirstSectorVehicleId(Read<std::uint32_t>(handler, 0), 0, 10000, true); id != -1; id = iterator.GetNextSectorVehicleId())
        {
            const auto count = Read<std::uint32_t>(handler, 0x5CB4);
            std::uint32_t i = 0;
            while (i < count && Read<std::uint32_t>(entries + i * 28, 0) != static_cast<std::uint32_t>(id)) ++i;
            if (i != count) continue;
            auto* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
            std::byte command[20]{};
            Write<std::uint8_t>(command, 0, 111); Write<std::uint8_t>(command, 1, Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::PlayerId)); Write(command, 4, id); Write(command, 8, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId)); Write<std::uint32_t>(command, 12, 3);
            Vehicle_DoAddNewUserCommand(vehicle, command);
            auto* entry = entries + count * 28;
            Write(entry, 0, id); Write(entry, 4, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId)); Write<std::int32_t>(entry, 8, -1); Write<std::uint32_t>(entry, 12, 0);
            Write(handler, 0x5CB4, count + 1); if (count + 1 == 30) return;
        }
    }
    // Original function: FUN_004734de.
    void AISinglePlayerHandler_l_Vehicles_CheckForCommander(void* handler)
    {
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x5CB4); ++i)
        {
            auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(handler, 0x5CB8 + i * 28) * CVehicleArray::ElementSize;
            if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) != 0) continue;
            auto* human = AISinglePlayerHandler_l_SingleSoldier_GetBestSoldier(handler, Vehicle_GetMapPositionRef(vehicle), 20);
            if (human != nullptr) Human_DoAddNewUserCommand(human, 38, Read<std::uint8_t>(handler, 0), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId), Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId));
        }
    }

    namespace
    {
        std::uint32_t Distance(const void* a, const void* b)
        {
            return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(a, 0), Read<std::int16_t>(a, 2), Read<std::int16_t>(b, 0), Read<std::int16_t>(b, 2)));
        }
    }
    // Original function: FUN_0045a832.
    bool AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(void* handler, std::uint32_t slot, const void* point, std::uint32_t radius, bool houses, bool civilians, bool restrictCandidates, bool defence)
    {
        const auto* human = Human(Read<std::uint32_t>(Soldier(handler, slot), 0));
        const auto* weapon = static_cast<const std::byte*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType)]) + Human_Equipment_GetWeaponType(human) * 0xC4;
        const auto player = Read<std::uint32_t>(handler, 0);
        const auto limit = restrictCandidates ? 1u : 5u, distance = restrictCandidates ? 0u : 3u;
        const auto attackMoveable = [&](bool humans, bool secondList)
        {
            const auto id = Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, point, 0, radius, secondList, limit, distance, humans, false);
            if (id == -1) return false;
            return humans ? AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHuman(handler, slot, id, defence) : AISinglePlayerHandler_l_SingleSoldiers_Let_AttackVehicle(handler, slot, id, defence);
        };
        if (Read<std::uint8_t>(weapon, 0x58) != 0)
        {
            if (attackMoveable(false, true) || attackMoveable(true, true)) return true;
        }
        else if (attackMoveable(true, true) || attackMoveable(false, true)) return true;
        if (houses)
        {
            const auto id = Tool_GetOneOfNextEnemyHousesInRange(player, point, 0, radius, true, restrictCandidates ? 1u : 3u, restrictCandidates ? 0u : 5u);
            if (id != -1 && AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(handler, slot, id, defence)) return true;
        }
        if ((Read<std::uint32_t>(human, NC2Layout::HumanOffset::MiscFlags) & NXBasics::BitMask::Bit13Signed) == 0)
        {
            const auto id = Tool_Animals_GetOneOfNextEnemyInRange(player, point, 0, radius, true, false, limit, distance);
            if (id != -1 && AISinglePlayerHandler_l_SingleSoldiers_Let_AttackAnimal(handler, slot, id, defence)) return true;
        }
        if (civilians && (attackMoveable(true, false) || attackMoveable(false, false))) return true;
        if (houses && civilians)
        {
            const auto id = Tool_GetOneOfNextEnemyHousesInRange(player, point, 0, radius, false, limit, restrictCandidates ? 0u : 10u);
            if (id != -1 && AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(handler, slot, id, defence)) return true;
        }
        return false;
    }
    // Original function: FUN_00459765.
    void AISinglePlayerHandler_l_SingleSoldier_CheckFormOrAttack(void* handler, std::uint32_t taskIndex)
    {
        auto* task = static_cast<std::byte*>(handler) + 0x2A50 + taskIndex * 88;
        std::uint32_t target = 0;
        AITaskSimpleDescription_Tool_GetMapPosition(static_cast<std::byte*>(handler) + 0x1C38 + Read<std::uint32_t>(task, 20) * 68, &target);
        if (Read<std::uint32_t>(task, 0) != 2) return;
        const auto allocated = Read<std::uint32_t>(handler, 0xA500 + taskIndex * 4);
        std::uint32_t minimum, spread;
        if (Read<std::uint32_t>(task, 72) == 2) { minimum = allocated / 3; spread = allocated * 3; }
        else { minimum = allocated >> 1; spread = allocated * 2; Write<std::uint32_t>(task, 72, 2); }
        if (allocated <= 1) return;
        std::uint32_t nearTarget = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168); ++i)
        {
            if (Read<std::uint8_t>(handler, 0xAC70 + i) == 0 || Read<std::uint32_t>(Soldier(handler, i), 56) != taskIndex) continue;
            if (Distance(Human_GetMapPositionRef(Human(Read<std::uint32_t>(Soldier(handler, i), 0))), &target) <= Read<std::uint32_t>(task, 68)) ++nearTarget;
        }
        if (nearTarget >= minimum) return;
        std::uint32_t center = 0, nearest = 10000;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168); ++i)
        {
            auto* soldier = Soldier(handler, i);
            if (Read<std::uint8_t>(handler, 0xAC70 + i) == 0 || Read<std::uint32_t>(soldier, 56) != taskIndex) continue;
            const auto* position = Human_GetMapPositionRef(Human(Read<std::uint32_t>(soldier, 0)));
            const auto distance = Distance(soldier + 24, position);
            if (distance < nearest) { center = Read<std::uint32_t>(position, 0); nearest = distance; }
        }
        std::uint32_t together = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168); ++i)
        {
            auto* soldier = Soldier(handler, i);
            if (Read<std::uint8_t>(handler, 0xAC70 + i) == 0 || Read<std::uint32_t>(soldier, 56) != taskIndex) continue;
            if (Distance(Human_GetMapPositionRef(Human(Read<std::uint32_t>(soldier, 0))), &center) <= spread) ++together;
        }
        if (together < minimum) { Write<std::uint32_t>(task, 72, 1); Write(task, 76, allocated / 3); }
    }
    // Original function: FUN_0045998e.
    void AISinglePlayerHandler_l_SingleSoldiers_UpdateAttachedTasks(void* handler)
    {
        const auto soldiers = Read<std::uint32_t>(handler, 0x6168);
        if (soldiers == 0) return;
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(handler) + 0xA338, 0, 0xA00);
        const auto tasks = Read<std::uint32_t>(handler, 0x2A48);
        double remaining = static_cast<double>(soldiers);
        const auto combined = [&](std::uint32_t i) { return static_cast<std::byte*>(handler) + 0x2A50 + i * 88; };
        const auto round = [](double value) { if (!(value >= 0.0)) value = 0.0; return static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(value + 0.5)); };
        const auto candidates = [&](const void* description, const void* point)
        {
            Write<std::uint32_t>(handler, 0xA62C, 0);
            for (std::uint32_t i = 0; i < soldiers; ++i)
            {
                if (Read<std::uint8_t>(handler, 0xAC70 + i) != 0) continue;
                const auto* soldier = Soldier(handler, i); const auto* human = Human(Read<std::uint32_t>(soldier, 0));
                if (Human_IsJobHero(human) && Read<std::uint32_t>(description, 0) == 2) continue;
                std::uint32_t position = 0; Human_GetMapPosition(human, &position);
                auto score = Distance(point, &position);
                if (!AITaskSimpleDescription_IsEqualTo(static_cast<const std::byte*>(soldier) + 20, description)) score += 10000;
                if (Read<std::uint32_t>(soldier, 12) == 0) score += 10000;
                if (Read<std::uint32_t>(soldier, 8) == 0) score += 10000;
                const auto count = Read<std::uint32_t>(handler, 0xA62C);
                std::uint32_t at = 0;
                while (at < count && Read<std::uint32_t>(handler, 0xA634 + at * 8) <= score) ++at;
                for (std::uint32_t end = count; end > at; --end)
                {
                    Write(handler, 0xA630 + end * 8, Read<std::uint32_t>(handler, 0xA630 + (end - 1) * 8));
                    Write(handler, 0xA634 + end * 8, Read<std::uint32_t>(handler, 0xA634 + (end - 1) * 8));
                }
                Write(handler, 0xA630 + at * 8, i); Write(handler, 0xA634 + at * 8, score); Write(handler, 0xA62C, count + 1);
            }
        };
        std::byte firstCommand[20]{}; Write<std::uint8_t>(firstCommand, 0, 41);
        std::byte secondCommand[20]{}; Write<std::uint8_t>(secondCommand, 0, 41);
        const auto assign = [&](std::uint32_t taskId, const void* description, std::uint32_t count, void* command)
        {
            auto* task = combined(taskId);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto slot = Read<std::uint32_t>(handler, 0xA630 + i * 8); auto* soldier = Soldier(handler, slot);
                if (!AITaskSimpleDescription_IsEqualTo(soldier + 20, description))
                {
                    NXBasics::Runtime_CopyMemory(soldier + 20, description, 32);
                    const auto type = Read<std::uint32_t>(soldier, 20);
                    if (type == 2) Write(soldier, 28, Read<std::uint32_t>(task, 68));
                    if (type == 1 || type == 2)
                    {
                        auto* human = Human(Read<std::uint32_t>(soldier, 0));
                        Write<std::uint8_t>(command, 1, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId));
                        Write(command, 12, type == 1 ? 2u : Read<std::uint32_t>(soldier, 44));
                        Human_DoAddNewUserCommand(human, command);
                    }
                    Write<std::uint32_t>(soldier, 52, 1); Write(soldier, 56, taskId);
                }
                Write<std::uint8_t>(soldier, 16, 0); Write<std::uint8_t>(handler, 0xAC70 + slot, 1);
            }
            AISinglePlayerHandler_l_SingleSoldier_CheckFormOrAttack(handler, taskId);
        };
        for (;;)
        {
            double weight = 0.0;
            for (std::uint32_t i = 0; i < tasks; ++i)
            {
                const auto* task = combined(i);
                if (Read<std::uint8_t>(handler, 0xA5FA + i) != 0) continue;
                if (Read<std::uint32_t>(task, 0) != 2 && (Read<std::uint32_t>(task, 64) == 0 || Read<std::uint32_t>(task, 60) == 0)) continue;
                weight += Read<double>(task, 8); Write<std::uint8_t>(handler, 0xA5C8 + i, 1);
            }
            if (weight == 0.0) break;
            bool allocated = false;
            for (std::uint32_t i = 0; i < tasks; ++i)
            {
                if (Read<std::uint8_t>(handler, 0xA5FA + i) != 0 || Read<std::uint8_t>(handler, 0xA5C8 + i) == 0) continue;
                const auto* task = combined(i); auto count = round(static_cast<double>(soldiers) * Read<double>(task, 8) / weight);
                const auto maximum = Read<std::uint32_t>(task, 64);
                if (maximum != 0 && count > maximum) count = maximum;
                if (count < Read<std::uint32_t>(task, 60)) continue;
                Write<std::uint8_t>(handler, 0xA338 + i, 1); Write(handler, 0xA500 + i * 4, count); allocated = true; remaining -= static_cast<double>(count);
            }
            if (!allocated) break;
            for (std::uint32_t i = 0; i < tasks; ++i)
            {
                if (Read<std::uint8_t>(handler, 0xA5FA + i) != 0 || Read<std::uint8_t>(handler, 0xA338 + i) == 0) continue;
                const auto* description = static_cast<std::byte*>(handler) + 0x1C38 + Read<std::uint32_t>(combined(i), 20) * 68;
                std::uint32_t point = 0;
                if (!AITaskSimpleDescription_Tool_GetMapPosition(description, &point)) continue;
                candidates(description, &point); assign(i, description, Read<std::uint32_t>(handler, 0xA500 + i * 4), firstCommand);
                Write<std::uint8_t>(handler, 0xA5FA + i, 1);
            }
        }
        for (std::uint32_t i = 0; i < tasks; ++i)
        {
            if (Read<std::uint8_t>(handler, 0xA5FA + i) != 0) continue;
            const auto* task = combined(i); auto count = round(Read<double>(task, 8) * Read<std::uint32_t>(task, 64) * 0.01);
            const auto maximum = Read<std::uint32_t>(task, 64);
            if (maximum != 0 && count > maximum) count = maximum;
            if (count >= Read<std::uint32_t>(task, 60)) Write<std::uint8_t>(handler, 0xA338 + i, 1);
        }
        std::uint32_t totalPriority = 0;
        for (std::uint32_t i = 0; i < tasks; ++i) if (Read<std::uint8_t>(handler, 0xA5FA + i) == 0 && Read<std::uint8_t>(handler, 0xA338 + i) != 0) totalPriority += Read<std::uint32_t>(combined(i), 4);
        const auto priorityFactor = 100.0 / static_cast<std::int32_t>(totalPriority);
        for (std::uint32_t i = 0; i < tasks; ++i) if (Read<std::uint8_t>(handler, 0xA5FA + i) == 0 && Read<std::uint8_t>(handler, 0xA338 + i) != 0) Write(handler, 0xA370 + i * 8, static_cast<double>(Read<std::int32_t>(combined(i), 4)) * priorityFactor);
        for (std::uint32_t i = 0; i < tasks; ++i)
        {
            if (Read<std::uint8_t>(handler, 0xA5FA + i) != 0 || Read<std::uint8_t>(handler, 0xA338 + i) == 0) continue;
            const auto* task = combined(i); auto count = round(remaining * Read<double>(handler, 0xA370 + i * 8) * 0.01);
            const auto maximum = Read<std::uint32_t>(task, 64), minimum = Read<std::uint32_t>(task, 60);
            if (maximum != 0 && count > maximum) count = maximum;
            if (count < minimum) continue;
            const auto* description = static_cast<std::byte*>(handler) + 0x1C38 + Read<std::uint32_t>(task, 20) * 68;
            std::uint32_t point = 0;
            if (!AITaskSimpleDescription_Tool_GetMapPosition(description, &point)) continue;
            candidates(description, &point);
            if (Read<std::uint32_t>(handler, 0xA62C) < count) count = Read<std::uint32_t>(handler, 0xA62C);
            if (count < minimum) continue;
            if (maximum != 0 && count > maximum) count = maximum;
            Write(handler, 0xA500 + i * 4, count);
            if (count != 0) assign(i, description, count, secondCommand);
        }
        if (Read<std::uint8_t>(handler, 0xA32C) == 0) return;
        Tool_Map_GetNotBlockedPosition(static_cast<std::byte*>(handler) + 0xA32E, true);
        for (std::uint32_t i = 0; i < soldiers; ++i)
        {
            auto* soldier = Soldier(handler, i);
            if (Read<std::uint32_t>(soldier, 20) != 0) continue;
            auto* human = Human(Read<std::uint32_t>(soldier, 0));
            if (AISingleSoldier_Tool_IsSelfDefending(soldier) && Distance(static_cast<std::byte*>(handler) + 0xAD48 + i * 4, Human_GetMapPositionRef(human)) < 25) continue;
            if (Read<std::uint8_t>(soldier, 16) != 0) continue;
            NXBasics::Runtime_FillMemory(soldier + 20, 0, 32);
            Write<std::uint32_t>(soldier, 20, 1); Write(soldier, 24, Read<std::uint32_t>(handler, 0xA32E)); Write(soldier, 28, Read<std::uint32_t>(handler, 0xA334));
            Write<std::int32_t>(soldier, 56, -1); Write<std::uint8_t>(soldier, 16, 1); Write<std::uint32_t>(soldier, 52, 1);
            Write<std::uint8_t>(secondCommand, 1, Read<std::uint8_t>(human, NC2Layout::HumanOffset::PlayerId)); Write<std::uint32_t>(secondCommand, 12, 2); Human_DoAddNewUserCommand(human, secondCommand);
        }
    }

    // Original function: FUN_0045a2d4.
    void AISinglePlayerHandler_l_SingleSoldiers_WorkOnAttachedTasks(void* handler)
    {
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(handler, 0x6168); ++i)
        {
            auto* soldier = Soldier(handler, i); auto* human = Human(Read<std::uint32_t>(soldier, 0));
            const auto position = Read<std::uint32_t>(Human_GetMapPositionRef(human), 0);
            const auto type = Read<std::uint32_t>(soldier, 20);
            if (type == 0) { AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &position, 25, true, false, true, true); continue; }
            const auto busyWalking = [&]() { const auto atomic = Read<std::uint32_t>(human, NC2Layout::HumanOffset::CurrentTask); return atomic == 2 || atomic == 4 || atomic == 14 || atomic == 15; };
            auto state = Read<std::uint32_t>(soldier, 52);
            if (state == 2)
            {
                if (type == 2)
                {
                    const auto* task = static_cast<std::byte*>(handler) + 0x2A50 + Read<std::uint32_t>(soldier, 56) * 88;
                    const auto phase = Read<std::uint32_t>(task, 72);
                    if (phase == 2)
                    {
                        if (Distance(soldier + 60, soldier + 24) > 10) Write<std::uint32_t>(soldier, 52, 1);
                        else if (Distance(&position, soldier + 60) <= Read<std::uint32_t>(soldier, 64)) AISingleSoldier_Tool_StopVAAndVCTask(soldier);
                        else if (!busyWalking()) AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, soldier + 60, Read<std::uint32_t>(soldier, 64));
                    }
                    else if (phase == 1)
                    {
                        if (Distance(soldier + 60, task + 80) > 10 || Distance(&position, task + 80) <= Read<std::uint32_t>(task, 76)) Write<std::uint32_t>(soldier, 52, 1);
                        else if (!busyWalking()) AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, task + 80, Read<std::uint32_t>(task, 76));
                    }
                }
                else if (Distance(&position, soldier + 60) <= Read<std::uint32_t>(soldier, 64)) AISingleSoldier_Tool_StopVAAndVCTask(soldier);
                else if (!busyWalking()) AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, soldier + 60, Read<std::uint32_t>(soldier, 64));
            }
            else if (state == 16)
            {
                if ((Human_Equipment_GetWeaponType(human) == Read<std::uint32_t>(soldier, 8) && Human_Equipment_GetArmorType(human) == Read<std::uint32_t>(soldier, 12)) || Human_IsDoingNothing(human)) Write<std::uint32_t>(soldier, 52, 1);
            }
            if (Read<std::uint32_t>(soldier, 52) == 14)
            {
                if (TargetData_IsValid(soldier + 72))
                {
                    if (AISinglePlayerHandler_l_SingleSoldier_CheckIfIsInTaskRange(handler, i, true)) continue;
                    if (AISingleSoldier_Tool_IsSelfDefending(soldier) && Distance(static_cast<std::byte*>(handler) + 0xAD48 + i * 4, Human_GetMapPositionRef(human)) < 25) continue;
                }
                Write<std::uint32_t>(soldier, 52, 1);
            }
            if (Read<std::uint32_t>(soldier, 52) != 1 && Read<std::uint32_t>(human, NC2Layout::HumanOffset::CurrentTaskStatus) == 1) continue;
            if (AISingleSoldier_Tool_IsSelfDefending(soldier)) continue;
            Write<std::uint32_t>(soldier, 52, 1);
            // Reload the description: stopping a completed movement can clear the task above.
            const auto currentType = Read<std::uint32_t>(soldier, 20);
            if (currentType == 2)
            {
                const auto target = Read<std::uint32_t>(soldier, 24);
                const auto* task = static_cast<std::byte*>(handler) + 0x2A50 + Read<std::uint32_t>(soldier, 56) * 88;
                const auto phase = Read<std::uint32_t>(task, 72);
                if (phase == 2)
                {
                    const auto houseId = Logic_EntryPoint_House_GetOnPosition(&target);
                    if (houseId != -1)
                    {
                        const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseId) * CHouseArray::ElementSize;
                        if (GameControl_Diplomacy_GetState(gGameControl, Read<std::uint32_t>(handler, 0), Read<std::int32_t>(house, NC2Layout::HouseOffset::PlayerId)) == 3 && AISinglePlayerHandler_l_SingleSoldiers_Let_AttackHouse(handler, i, houseId, false)) continue;
                    }
                    if (Distance(&position, &target) <= Read<std::uint32_t>(soldier, 28))
                    {
                        if (AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &position, 25, true, false, false, true)) continue;
                        if (AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &position, 25, true, true, false, true)) continue;
                        if (AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &target, Read<std::uint32_t>(soldier, 28), true, true, false, false)) continue;
                    }
                    const auto radius = Read<std::uint32_t>(soldier, 28) >> 1;
                    if (Distance(&position, &target) > radius) AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, &target, radius);
                }
                else if (phase == 1)
                {
                    if (Distance(&position, task + 80) <= Read<std::uint32_t>(task, 76)) AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &position, 25, true, false, false, true);
                    else AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, task + 80, Read<std::uint32_t>(task, 76));
                }
            }
            else if (currentType == 1)
            {
                const auto target = Read<std::uint32_t>(soldier, 24);
                if (Distance(&position, &target) <= Read<std::uint32_t>(soldier, 28))
                {
                    if (AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &position, 25, true, false, false, true)) continue;
                    if (AISinglePlayerHandler_l_SingleSoldiers_CheckAttack(handler, i, &target, Read<std::uint32_t>(soldier, 28), true, true, false, false)) continue;
                }
                if (Distance(&position, &target) > 10) AISinglePlayerHandler_l_SingleSoldiers_Let_GotoPosition(handler, i, &target, Read<std::uint32_t>(soldier, 28) >> 1);
            }
        }
    }
}
