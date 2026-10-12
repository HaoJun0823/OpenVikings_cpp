#include "SwitchIdentifiers.h"
#include "C2VehicleSimulation.h"
#include "C2VehicleAccess.h"
#include "C2VehicleLifecycle.h"
#include "C2VehicleAnimalPassengers.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2StaticVehicleTypeData.h"
#include "C2StaticTribeTypeData.h"
#include "C2DefaultInteraction.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2MapPathfinderSearch.h"
#include "C2MilitaryAttackPointSearch.h"
#include "C2OverviewObjectIterators.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameControlNetwork.h"
#include "C2MapMoveableAccess.h"
#include "C2MapGeometry.h"
#include "C2UserMessageAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2MilitaryTargetSearch.h"
#include "C2DelayedActionManager.h"
#include "C2HumanWalking.h"
#include "C2HumanInitialization.h"
#include "C2HumanTaskAccess.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Moveable(const void* vehicle) { return gMapMoveableArrayStorage.entries + Read<std::uint32_t>(vehicle, 0x1DC) * 0x58u; }
        void SendVehicleMessage(const void* vehicle, std::uint32_t type)
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write(message, 0x20, Read<std::uint32_t>(vehicle, 4));
            Write(message, 0x24, Read<std::uint32_t>(vehicle, 8));
            Write(message, 0x2E0, Read<std::uint32_t>(vehicle, 0x1E8));
            Write(message, 0, type);
            Write(message, 8, Read<std::uint32_t>(vehicle, 0x1E0));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        std::uint32_t Distance(const void* source, const void* target) { return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(target, 0), Read<std::int16_t>(target, 2))); }
        void* Vehicle(std::uint32_t id) { return CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize; }
        std::byte* At(void* object, std::uint32_t offset) { return static_cast<std::byte*>(object) + offset; }
        std::uint32_t WeaponId(const void* vehicle) { return Read<std::uint32_t>(vehicle, 0x1E8) == 5 ? 21u : 0u; }
        const SStaticWeaponTypeData& Weapon(const void* vehicle) { return static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(vehicle, 0x1E4)])[WeaponId(vehicle)]; }
        void MakeCommand(void* command, const void* vehicle, std::uint8_t opcode)
        {
            std::memset(command, 0, 20);
            Write(command, 0, opcode);
            Write(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(vehicle, 0x1E0)));
            Write(command, 4, Read<std::uint32_t>(vehicle, 4));
            Write(command, 8, Read<std::uint32_t>(vehicle, 8));
        }
    }

    // Original function: FUN_0043711D.
    void Vehicle_DoUpdateConditions(void* vehicle)
    {
        if (Read<std::int32_t>(vehicle, 0x0C) <= 0) Write<std::uint8_t>(Moveable(vehicle), 0x0C, 1);
    }

    // Original function: FUN_00438057; events execute before advancing the tick.
    void Vehicle_CurrentTask_DoPerform(void* vehicle)
    {
        if (Read<std::uint32_t>(vehicle, 0x4FC) == 0) { Vehicle_l_StartAtomicIdle(vehicle); return; }
        if (Read<std::uint8_t>(vehicle, 0x500) == 0) return;
        const auto* animation = gStaticAtomicAnimationStorage.records + Read<std::uint32_t>(vehicle, 0x504);
        for (std::uint32_t index = 0; index < animation->eventCount; ++index)
            if (static_cast<std::uint32_t>(animation->events[index].time) == Read<std::uint32_t>(vehicle, 0x508)) Vehicle_l_PerformTaskEvent(vehicle, &animation->events[index]);
        Write(vehicle, 0x508, Read<std::uint32_t>(vehicle, 0x508) + 1u);
        if (Read<std::uint32_t>(vehicle, 0x508) != static_cast<std::uint32_t>(animation->length)) return;
        if (Read<std::uint32_t>(vehicle, 0x4FC) == 0x54)
        {
            Write<std::uint32_t>(vehicle, 0x1C8, 0);
            Write<std::uint8_t>(vehicle, 0x1D0, 1);
        }
        if (animation->endDirection != -1) Write(Moveable(vehicle), 0x20, animation->endDirection);
        Vehicle_l_ResetAtomic(vehicle);
    }

    // Original function: FUN_0043810E; this outlined predicate has no established Mac name.
    bool Vehicle_IsTaskOrAttachmentActive(const void* vehicle)
    {
        const auto task = Read<std::uint32_t>(vehicle, 0x1C8);
        return task == 5 || task == 6 || task == 4 || task == 3 || Vehicle_Attachment_IsInsideVehicle(vehicle);
    }

    // Original function: FUN_00438138.
    void Vehicle_l_Error_NoCommander(const void* vehicle) { SendVehicleMessage(vehicle, 0x33); }

    // Original function: FUN_00441826; arithmetic wraps before signed division.
    void Vehicle_WalkSpeed_Update(void* vehicle, std::uint32_t terrainSpeed)
    {
        auto ticks = terrainSpeed * 2u + 4u;
        if (Read<std::uint32_t>(vehicle, 0x1E8) == 5) ticks *= 2u;
        if (NXBasics::Runtime_TimeSigned32(ticks) < 3) ticks = 3;
        const auto progress = NXBasics::Runtime_TimeSigned32(ticks + 9999u) / NXBasics::Runtime_TimeSigned32(ticks);
        Write(Moveable(vehicle), 0x44, ticks);
        Write<std::uint32_t>(Moveable(vehicle), 0x4C, 2);
        Write(Moveable(vehicle), 0x48, progress);
    }

    // Original function: FUN_0046255B.
    bool Vehicle_l_StartAtomicDock(void* vehicle)
    {
        Vehicle_GetVehicleTypeDataPtr(vehicle);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<std::byte*>(vehicle) + 0x1CC), 6);
        Vehicle_l_GetDoorPoint(vehicle, continent, Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0));
        if (!MapMigPoint_IsInsideMap(static_cast<std::byte*>(vehicle) + 0x1CC)) return false;
        Write<std::uint32_t>(vehicle, 0x520, 0);
        Write<std::uint32_t>(vehicle, 0x52C, 0);
        Vehicle_l_StartAtomicAnimation(vehicle, 0x54, -1);
        return true;
    }

    // Original function: FUN_004625C4.
    bool Vehicle_l_StartAtomicUnload(void* vehicle, bool includeCommander)
    {
        if (!Vehicle_Door_IsEntryPointOnLand(vehicle, 0)) return false;
        Vehicle_Passengers_MoveAllOut(vehicle, includeCommander);
        return true;
    }

    // Original function: FUN_0046262D.
    void Vehicle_l_ResetAtomic(void* vehicle)
    {
        Write<std::uint32_t>(vehicle, 0x4FC, 0);
        Write<std::uint8_t>(vehicle, 0x500, 0);
    }

    // Original function: FUN_00437138.
    void Vehicle_DoUpdateAI(void* vehicle)
    {
        Vehicle_CurrentTask_DoPerform(vehicle);
        if (!Vehicle_l_IsAtomicInterruptable(vehicle)) return;
        if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) Write<std::uint32_t>(vehicle, 0x1C8, 4);
        Vehicle_DoExecuteNextUserCommand(vehicle);
        if (Read<std::uint32_t>(vehicle, 0x4FC) == 1) return;
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0)
        {
            if (Vehicle_Attachment_IsValid(vehicle) && !Vehicle_Attachment_IsInsideVehicle(vehicle))
            {
                if (Read<std::uint32_t>(vehicle, 0x1C8) != 3) Write<std::uint32_t>(vehicle, 0x1C8, 6);
            }
            else if (Read<std::uint32_t>(vehicle, 0x1EC) == 0x36 && !Vehicle_IsTaskOrAttachmentActive(vehicle))
            {
                if (!TargetData_IsValid(At(vehicle, 0x520)) && !TargetData_IsValid(At(vehicle, 0x52C)))
                {
                    switch (Read<std::uint32_t>(vehicle, 0x53C))
                    {
                    case SwitchIds::VehicleMilitaryMode::SearchAroundPosition: Vehicle_l_Military_CheckForEnemys(vehicle, Vehicle_GetMapPositionRef(vehicle), 0, 40); break;
                    case SwitchIds::VehicleMilitaryMode::SearchAroundTarget: Vehicle_l_Military_CheckForEnemys(vehicle, At(vehicle, 0x1D6), 0, 40); break;
                    case SwitchIds::VehicleMilitaryMode::SearchWithinWeaponRange: Vehicle_l_Military_CheckForEnemys(vehicle, At(vehicle, 0x1D6), Weapon(vehicle).minimumRange, Weapon(vehicle).maximumRange); break;
                    }
                }
                if ((TargetData_IsValid(At(vehicle, 0x520)) || TargetData_IsValid(At(vehicle, 0x52C))) && (Read<std::uint32_t>(vehicle, 0x1C8) == 0 || Read<std::uint32_t>(vehicle, 0x1C8) == 2))
                {
                    if (!Vehicle_l_Passengers_MoveIn(vehicle)) Write<std::uint32_t>(vehicle, 0x1C8, 3);
                    else if (Vehicle_l_Military_AttackTask(vehicle)) Write<std::uint32_t>(vehicle, 0x1C8, 2);
                }
            }
        }
        switch (Read<std::uint32_t>(vehicle, 0x1C8))
        {
        case SwitchIds::VehicleTask::Dock:
            if (Read<std::uint32_t>(vehicle, 0x4FC) != 0) return;
            SendVehicleMessage(vehicle, 0x32);
            Write<std::uint32_t>(vehicle, 0x1C8, 0);
            break;
        case SwitchIds::VehicleTask::Attack:
            if (TargetData_IsValid(At(vehicle, 0x520)) || TargetData_IsValid(At(vehicle, 0x52C)))
            {
                if (Vehicle_l_Passengers_MoveIn(vehicle)) return;
                Write<std::uint32_t>(vehicle, 0x1C8, 3);
            }
            else Write<std::uint32_t>(vehicle, 0x1C8, 0);
            break;
        case SwitchIds::VehicleTask::WaitForPassengers:
            if (Vehicle_l_Passengers_MoveIn(vehicle)) Write<std::uint32_t>(vehicle, 0x1C8, 0);
            break;
        case SwitchIds::VehicleTask::GetDraggingAnimal:
            if (!Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) Write<std::uint32_t>(vehicle, 0x1C8, 0);
            else if (!Vehicle_Passengers_IsHavingDraggingAnimal(vehicle)) Vehicle_Passengers_TryToGetDraggingAnimal(vehicle);
            break;
        case SwitchIds::VehicleTask::Interrupted: Write<std::uint32_t>(vehicle, 0x1C8, 0); break;
        case SwitchIds::VehicleTask::AttachToVehicle: Vehicle_l_StartAtomicAttachToVehicle(vehicle); return;
        }
        Vehicle_l_StartAtomicIdle(vehicle);
    }

    // Original function: FUN_004373C7.
    bool Vehicle_DoNewMapPositionReached(void* vehicle, bool destinationReached)
    {
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(vehicle, 0x1E0), Vehicle_GetMapPositionRef(vehicle), Read<std::uint32_t>(vehicle, 0x518));
        const auto* micro = GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle));
        Vehicle_WalkSpeed_Update(vehicle, (Read<std::uint8_t>(micro, 0x0C) >> 3) & 15u);
        Vehicle_l_SendAwayRadial(vehicle);
        Write<std::uint8_t>(vehicle, 0x1D0, 0);
        if (Vehicle_IsPriorityCommandInQueue(vehicle)) { Write<std::uint32_t>(vehicle, 0x4FC, 0); return false; }
        Vehicle_DoExecuteNextUserCommand(vehicle);
        if (!destinationReached)
        {
            if ((Read<std::uint8_t>(micro, 0x0C) & 0x80u) != 0 || (Read<std::uint8_t>(micro, 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) return true;
            if (!Vehicle_Passengers_IsCommanderInVehicle(vehicle)) { Vehicle_l_StartAtomicIdle(vehicle); return false; }
            return true;
        }
        if (Read<std::uint32_t>(vehicle, 0x1C8) == 1) { Vehicle_l_StartAtomicDock(vehicle); return true; }
        const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
        if ((type == 3 || type == 4) && !Vehicle_Passengers_IsCommanderInVehicle(vehicle))
        {
            const auto point = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
            const auto continent = Vehicle_l_GetLandContinentIdFromBorder(vehicle, point);
            if (continent == -1) MapMigPoint_Construct(At(vehicle, 0x1CC), -1, -1, false);
            else { Write<std::uint8_t>(vehicle, 0x1D0, 1); Vehicle_l_GetDoorPoint(vehicle, continent, point); }
        }
        Vehicle_l_Passengers_StopAllIdleTasks(vehicle);
        Write<std::uint32_t>(vehicle, 0x4FC, 0);
        return true;
    }

    // Original function: FUN_00437514.
    void Vehicle_DoPathfinderFailed(void* vehicle)
    {
        SendVehicleMessage(vehicle, 0x32);
        Write(vehicle, 0x1D6, Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0));
        if (Vehicle_l_IsAtomicInterruptable(vehicle)) Vehicle_l_ResetAtomic(vehicle);
        Write<std::uint32_t>(vehicle, 0x520, 0);
        Write<std::uint32_t>(vehicle, 0x52C, 0);
    }

    // Original function: FUN_0043F52D.
    bool Vehicle_DoExecuteNextUserCommand(void* vehicle)
    {
        for (std::uint32_t index = 0; index < 22; ++index)
        {
            auto* command = At(vehicle, 0x10 + index * 20);
            if (Read<std::uint8_t>(command, 0) != 0 && Vehicle_DoExecuteUserCommand(vehicle, command))
            {
                Write<std::uint8_t>(vehicle, 0x10 + index * 20, 0);
                return true;
            }
        }
        return false;
    }

    // Original function: FUN_0043F58C.
    bool Vehicle_DoExecuteUserCommand(void* vehicle, const void* command)
    {
        const auto opcode = Read<std::uint8_t>(command, 0);
        if (((opcode >= 0x65 && opcode <= 0x6C) || opcode == 0x71) && Vehicle_Attachment_IsValid(vehicle) && !Vehicle_l_UserCommands_DetachFromVehicle(vehicle)) return true;
        auto point = Read<std::uint32_t>(command, 0x0C);
        if (opcode == 0x65) { Write<std::uint32_t>(vehicle, 0x1C8, 0); return Vehicle_DoExecuteUserCommand_Goto(vehicle, &point); }
        if (opcode == 0x66) { Write<std::uint32_t>(vehicle, 0x1C8, 0); Vehicle_l_StartAtomicUnload(vehicle, true); return true; }
        if (opcode == 0x67) return Vehicle_DoExecuteUserCommand_Dock(vehicle, &point);
        if (opcode >= 0x68 && opcode <= 0x6B)
        {
            const auto id = Read<std::uint32_t>(command, 0x0C), revision = Read<std::uint32_t>(command, 0x10);
            const std::uint32_t types[] = { 1, 4, 3, 2 };
            void* object = opcode == 0x68 ? static_cast<void*>(CHumanArray::mStaticVars + id * CHumanArray::ElementSize) : opcode == 0x69 ? static_cast<void*>(CHouseArray::mStaticVars + id * CHouseArray::ElementSize) : opcode == 0x6A ? static_cast<void*>(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize) : Vehicle(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return true;
            Write(vehicle, 0x524, id);
            Write(vehicle, 0x528, revision);
            Write(vehicle, 0x520, types[opcode - 0x68]);
        }
        else if (opcode == 0x6C)
        {
            if (!MapMigPoint_IsInsideMap(&point)) return true;
            const bool landscape = Tool_IsEnemyLandscapeOnPoint(Read<std::uint32_t>(vehicle, 0x1E0), &point);
            Write(vehicle, 0x524, point);
            Write<std::uint32_t>(vehicle, 0x520, landscape ? 6 : 5);
            if (!landscape) Write<std::uint16_t>(vehicle, 0x52A, 0);
        }
        else if (opcode == 0x70)
        {
            Vehicle_l_Passengers_StopAllIdleTasks(vehicle);
            if (Read<std::uint32_t>(vehicle, 0x1D6) != Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0) && !TargetData_IsValid(At(vehicle, 0x520)) && !TargetData_IsValid(At(vehicle, 0x52C)))
            {
                std::byte next[20]; MakeCommand(next, vehicle, 0x65);
                Write(next, 0x0C, Read<std::uint32_t>(vehicle, 0x1D6));
                Vehicle_DoAddNewUserCommand(vehicle, next);
            }
            Write<std::uint32_t>(vehicle, 0x1C8, 5);
            return true;
        }
        else if (opcode == 0x71)
        {
            auto* carrier = Vehicle(Read<std::uint32_t>(command, 0x0C));
            if (Read<std::uint8_t>(carrier, 0) != 0 && Read<std::uint32_t>(carrier, 8) == Read<std::uint32_t>(command, 0x10) && Vehicle_Passengers_CanVehicleBeAttached(carrier, Read<std::uint32_t>(vehicle, 4)) && Vehicle_Passengers_AttachVehicle(carrier, Read<std::uint32_t>(vehicle, 4)))
            {
                Write(vehicle, 0x540, Read<std::uint32_t>(carrier, 4));
                Write(vehicle, 0x544, Read<std::uint32_t>(carrier, 8));
            }
            return true;
        }
        else if (opcode == 0x72) return Vehicle_l_UserCommands_DetachFromVehicle(vehicle);
        else if (opcode == 0x73)
        {
            if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) { SendVehicleMessage(vehicle, 0x35); return true; }
            if (!Vehicle_Passengers_AllPassengersInside(vehicle)) return false;
            Write<std::uint32_t>(vehicle, 0x1C8, 6);
            return true;
        }
        else return true;
        Write<std::uint32_t>(vehicle, 0x52C, 0);
        return Vehicle_l_IsAtomicInterruptable(vehicle);
    }

    // Original function: FUN_0043F96C; the PE prologue resolves the misaligned export.
    bool Vehicle_l_UserCommands_DetachFromVehicle(void* vehicle)
    {
        auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, 0x540));
        if (Vehicle_Attachment_IsInsideVehicle(vehicle))
        {
            const auto type = Read<std::uint32_t>(carrier, 0x1E8);
            if ((type == 3 || type == 4) && Read<std::uint8_t>(carrier, 0x1D0) == 0) return false;
            if (Read<std::uint32_t>(carrier, 0x4FC) == 1)
            {
                std::byte command[20]; MakeCommand(command, carrier, 0x65);
                Write(command, 1, static_cast<std::uint8_t>(Read<std::uint32_t>(vehicle, 0x1E0)));
                std::uint32_t point = 0;
                MapMoveable_Attachment_GetNextMapPosition(Moveable(carrier), &point);
                Write(command, 0x0C, point);
                Vehicle_DoAddNewUserCommand(carrier, command);
                return false;
            }
            return Vehicle_Passengers_MoveOut_v(carrier, Read<std::uint32_t>(vehicle, 4));
        }
        if (!Vehicle_Passengers_DetachVehicle(carrier, Read<std::uint32_t>(vehicle, 4))) SendVehicleMessage(vehicle, 0x39);
        Write<std::uint32_t>(vehicle, 0x1C8, 0);
        return true;
    }

    // Original function: FUN_0043FA9A.
    bool Vehicle_DoExecuteUserCommand_Goto(void* vehicle, const void* point)
    {
        if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) { SendVehicleMessage(vehicle, 0x35); return true; }
        if (Read<std::uint8_t>(vehicle, 0x2EC) == 0) { Vehicle_l_Error_NoCommander(vehicle); return true; }
        if (Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0) == Read<std::uint32_t>(point, 0)) { Vehicle_l_StartAtomicIdle(vehicle); return true; }
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6);
        if (continent != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle)), 6)) return true;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) return true;
        Write<std::uint32_t>(vehicle, 0x520, 0);
        Write<std::uint32_t>(vehicle, 0x52C, 0);
        if (Vehicle_l_StartAtomicWalk(vehicle, point, false)) { Write(vehicle, 0x1D6, Read<std::uint32_t>(point, 0)); return true; }
        const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
        if ((type == 3 || type == 4) && !Vehicle_Passengers_IsCommanderInVehicle(vehicle) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) return true;
        return false;
    }

    // Original function: FUN_0043FBD5; the native perimeter advances before checking each point.
    bool Vehicle_DoExecuteUserCommand_Dock(void* vehicle, const void* point)
    {
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & 0x80u) != 0) return true;
        if (Read<std::uint8_t>(vehicle, 0x2EC) == 0) { Vehicle_l_Error_NoCommander(vehicle); return true; }
        if (Read<std::uint8_t>(vehicle, 0x1D0) != 0 && !Vehicle_l_Passengers_MoveIn(vehicle)) { Vehicle_l_StartAtomicIdle(vehicle); return false; }
        const auto* type = Vehicle_GetVehicleTypeDataPtr(vehicle);
        auto candidate = Read<std::uint32_t>(point, 0);
        MapMigPoint_MoveCursorUnwrapped(&candidate, 4, type->passengerVectorY);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle)), 6);
        for (std::uint32_t direction = 0; direction < 6; ++direction)
        {
            for (std::int32_t step = 0; step < type->passengerVectorY; ++step)
            {
                MapMigPoint_MoveCursorUnwrapped(candidate, direction);
                if (!MapMigPoint_IsInsideMap(&candidate)) continue;
                const auto* micro = GameMap_GetConstMapMicroPointPtr(&candidate);
                if (Read<std::uint8_t>(micro, 6) != continent || (Read<std::uint8_t>(micro, 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) continue;
                if (candidate == Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0)) return true;
                if (!Vehicle_l_StartAtomicWalk(vehicle, &candidate, false)) continue;
                Write<std::uint32_t>(vehicle, 0x1C8, 1);
                Write(vehicle, 0x1CC, Read<std::uint32_t>(point, 0));
                return true;
            }
        }
        if (!Vehicle_Passengers_IsCommanderInVehicle(vehicle))
        {
            Write<std::uint32_t>(vehicle, 0x1C8, 1);
            Write(vehicle, 0x1CC, Read<std::uint32_t>(point, 0));
            Vehicle_l_StartAtomicDock(vehicle);
        }
        else SendVehicleMessage(vehicle, 0x32);
        return true;
    }

    // Original function: FUN_0044C725; references across a continent receive detach commands.
    bool Vehicle_l_Passengers_MoveIn(void* vehicle)
    {
        const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
        if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) return true;
        bool inside = true;
        std::uint32_t point = 0;
        Vehicle_Door_GetEntryPoint(vehicle, &point, 0);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
        std::byte command[20]; MakeCommand(command, vehicle, 0x28);
        Write(command, 0x0C, Read<std::uint32_t>(vehicle, 4));
        Write(command, 0x10, Read<std::uint32_t>(vehicle, 8));
        const auto humanSlots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) + 1u;
        for (std::uint32_t index = 0; index < humanSlots; ++index)
        {
            auto* slot = At(vehicle, 0x1F0 + index * 12);
            if (!HumanReference_IsAllocated(slot) || Read<std::uint8_t>(slot, 8) != 0) continue;
            auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(slot, 0) * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human)), 6) != continent)
            {
                Human_DoAddNewUserCommand(human, 0x27, Read<std::uint32_t>(vehicle, 0x1E0), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
                continue;
            }
            inside = false;
            if (Read<std::uint32_t>(human, 0x370) == 0)
            {
                Write(command, 4, Read<std::uint32_t>(human, 4));
                Write(command, 8, Read<std::uint32_t>(human, 8));
                Human_DoAddNewUserCommand(human, command);
                Write<std::uint32_t>(vehicle, 0x1C8, 3);
            }
        }
        Vehicle_Door_GetEntryPoint(vehicle, &point, 1);
        MakeCommand(command, vehicle, 0x73);
        Write(command, 0x0C, Read<std::uint32_t>(vehicle, 4));
        Write(command, 0x10, Read<std::uint32_t>(vehicle, 8));
        for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots); ++index)
        {
            auto* slot = At(vehicle, 0x2FC + index * 12);
            if (!VehicleReference_IsAllocated(slot) || Read<std::uint8_t>(slot, 8) != 0) continue;
            auto* passenger = Vehicle(Read<std::uint32_t>(slot, 0));
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(passenger)), 6) != continent)
            {
                std::byte detach[20]; MakeCommand(detach, vehicle, 0x72);
                Vehicle_DoAddNewUserCommand(passenger, detach);
                continue;
            }
            inside = false;
            if (Read<std::uint32_t>(passenger, 0x1C8) != 6)
            {
                Write(command, 4, Read<std::uint32_t>(passenger, 4));
                Write(command, 8, Read<std::uint32_t>(passenger, 8));
                Vehicle_DoAddNewUserCommand(passenger, command);
            }
            Write<std::uint32_t>(vehicle, 0x1C8, 3);
        }
        return inside;
    }

    // Original function: FUN_0044C9DA.
    bool Vehicle_Passengers_AttachVehicle(void* vehicle, std::uint32_t id)
    {
        if (!Vehicle_Passengers_CanVehicleBeAttached(vehicle, id)) return false;
        const auto* passenger = Vehicle(id);
        const auto slots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots);
        for (std::uint32_t index = 0; index < slots; ++index)
        {
            auto* slot = At(vehicle, 0x2FC + index * 12);
            if (VehicleReference_IsAllocated(slot)) continue;
            Write<std::uint8_t>(slot, 8, 0);
            Write(slot, 0, Read<std::uint32_t>(passenger, 4));
            Write(slot, 4, Read<std::uint32_t>(passenger, 8));
            return true;
        }
        return false;
    }

    // Original function: FUN_0044CC0E.
    bool Vehicle_Passengers_MoveIn_v(void* vehicle, std::uint32_t id)
    {
        const auto slot = Vehicle_Passengers_GetAttachedVehicleSlot(vehicle, id);
        if (slot == -1) return false;
        const auto* passenger = Vehicle(id);
        std::uint32_t point = 0;
        Vehicle_Door_GetEntryPoint(vehicle, &point, Vehicle_GetVehicleTypeDataPtr(passenger)->logicSize);
        if (Read<std::uint32_t>(Vehicle_GetMapPositionRef(passenger), 0) != point) return false;
        Write<std::uint8_t>(vehicle, 0x304 + static_cast<std::uint32_t>(slot) * 12, 1);
        return true;
    }

    // Original function: FUN_0044CCC2.
    bool Vehicle_Passengers_CanVehicleMoveIn(const void* vehicle, std::uint32_t id)
    {
        if (Vehicle_Passengers_GetAttachedVehicleSlot(vehicle, id) == -1 && !Vehicle_Passengers_CanVehicleBeAttached(vehicle, id)) return false;
        const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
        if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) return false;
        const auto* passenger = Vehicle(id);
        std::uint32_t point = 0;
        Vehicle_Door_GetEntryPoint(vehicle, &point, Vehicle_GetVehicleTypeDataPtr(passenger)->logicSize);
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(passenger)), 6) != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6)) return false;
        const auto available = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) - Vehicle_Passengers_GetNumberOfAttachedHumans(vehicle, true) + 1u;
        return available >= Vehicle_Passengers_GetNumberOfAttachedHumans(passenger, true);
    }

    // Original function: FUN_0044CD83; the source reference is cleared after transferring ownership.
    void Vehicle_Passengers_ExtractPassengerSlot(void* vehicle, void* slot)
    {
        if (Read<std::uint8_t>(vehicle, 0x2EC) == 0)
        {
            std::memcpy(At(vehicle, 0x1F0 + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) * 12), slot, 12);
            Write<std::uint8_t>(vehicle, 0x2EC, 1);
        }
        else
        {
            const auto slots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
            for (std::uint32_t index = 0; index < slots; ++index)
            {
                auto* destination = At(vehicle, 0x1F0 + index * 12);
                if (!HumanReference_IsAllocated(destination)) { std::memcpy(destination, slot, 12); break; }
            }
        }
        auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(slot, 0) * CHumanArray::ElementSize;
        const auto id = Read<std::uint32_t>(vehicle, 4);
        Write(human, 0xB48, id);
        Write(human, 0xB4C, Read<std::uint32_t>(Vehicle(id), 8));
        Write<std::uint32_t>(slot, 0, 0xFFFFFFFFu);
        Write<std::uint8_t>(slot, 8, 0);
    }

    // Original function: FUN_0044DAC6; human transfers precede the final vehicle move-in and have no rollback.
    bool Vehicle_Attachment_EnterVehicle(void* vehicle)
    {
        auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, 0x540));
        if (!Vehicle_Passengers_CanVehicleMoveIn(carrier, Read<std::uint32_t>(vehicle, 4))) return false;
        const auto slots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index < slots; ++index)
        {
            auto* slot = At(vehicle, 0x1F0 + index * 12);
            if (HumanReference_IsAllocated(slot)) Vehicle_Passengers_ExtractPassengerSlot(carrier, slot);
        }
        if (Read<std::uint8_t>(vehicle, 0x2EC) != 0)
        {
            Vehicle_Passengers_ExtractPassengerSlot(carrier, At(vehicle, 0x1F0 + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) * 12));
            Write<std::uint8_t>(vehicle, 0x2EC, 0);
        }
        if (!Vehicle_Passengers_MoveIn_v(carrier, Read<std::uint32_t>(vehicle, 4))) return false;
        MapMoveable_Attachment_DetachFromMap(Moveable(vehicle));
        Write<std::uint8_t>(vehicle, 0x548, 1);
        return true;
    }

    // Original function: FUN_00442903; priority lists precede ordinary lists, vehicles precede houses.
    bool Vehicle_l_Military_CheckForEnemys(void* vehicle, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance)
    {
        const auto* source = Vehicle_GetMapPositionRef(vehicle);
        for (std::uint32_t branch = 0; branch < 4; ++branch)
        {
            const bool house = (branch & 1u) != 0, priority = branch < 2;
            const auto player = Read<std::uint32_t>(vehicle, 0x1E0);
            const auto id = house ? Tool_GetOneOfNextEnemyHousesInRange(player, point, minimumDistance, maximumDistance, priority, 5, 3) : Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, point, minimumDistance, maximumDistance, priority, 5, 3, false, false);
            if (id == -1) continue;
            const auto* candidate = house ? static_cast<const void*>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize) : Vehicle(id);
            if (TargetData_IsValid(At(vehicle, 0x52C)))
            {
                std::int16_t previous[2] = { 10000, 10000 };
                TargetData_GetMapPosition(At(vehicle, 0x52C), previous);
                const auto* candidatePoint = house ? static_cast<const std::byte*>(candidate) + 0x1C : Vehicle_GetMapPositionRef(candidate);
                if (Distance(source, previous) <= Distance(source, candidatePoint)) return false;
            }
            Write<std::uint32_t>(vehicle, 0x52C, house ? 4 : 2);
            Write<std::uint32_t>(vehicle, 0x530, id);
            Write(vehicle, 0x534, Read<std::uint32_t>(candidate, 8));
            return true;
        }
        return false;
    }

    // Original function: FUN_00442BD9.
    bool Vehicle_l_Military_AttackTask(void* vehicle)
    {
        for (std::uint32_t index = 0; index < 2; ++index)
        {
            auto* target = At(vehicle, 0x520 + index * 12);
            if (!TargetData_IsValid(target)) { Write<std::uint32_t>(target, 0, 0); continue; }
            const auto player = TargetData_GetPlayerId(target);
            const auto relation = player == 20 || player == -1 ? 3 : GameControl_Diplomacy_GetState(gGameControl, Read<std::uint32_t>(vehicle, 0x1E0), player);
            if (relation == 1 || relation == 2) { Write<std::uint32_t>(target, 0, 0); continue; }
            const auto& weapon = Weapon(vehicle);
            std::uint32_t destination = 0;
            TargetData_GetMapPosition(target, &destination);
            auto source = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
            if (Read<std::uint32_t>(vehicle, 0x53C) == 2 && index == 1 && Distance(At(vehicle, 0x1D6), &source) > 60) { Write<std::uint32_t>(target, 0, 0); return false; }
            const auto distance = Distance(&source, &destination);
            if (distance >= static_cast<std::uint32_t>(weapon.minimumRange) && distance <= static_cast<std::uint32_t>(weapon.maximumRange))
            {
                if (Vehicle_GoAway_HitByVehicle(vehicle, &source))
                {
                    std::uint32_t freePoint = 0;
                    if (Vehicle_l_GoAway_GetVehicleFreePos(vehicle, &freePoint)) { Vehicle_l_StartAtomicWalk(vehicle, &freePoint, false); return false; }
                }
                const auto direction = VE_HexagonDirection_GetDirectionFromPositions(Read<std::int16_t>(&source, 0), Read<std::int16_t>(&source, 2), Read<std::int16_t>(&destination, 0), Read<std::int16_t>(&destination, 2), true);
                if (Vehicle_l_StartAtomicAnimation(vehicle, 0x51, direction)) { Write(vehicle, 0x538, index); return true; }
                continue;
            }
            if (Read<std::uint32_t>(vehicle, 0x53C) == 3 && index == 1) { Write<std::uint32_t>(target, 0, 0); return false; }
            const bool tooClose = distance < static_cast<std::uint32_t>(weapon.minimumRange);
            const bool sameContinent = tooClose || Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&source), 6) == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&destination), 6);
            if (TargetData_GetTargetType(target) == 4) destination = Read<std::uint32_t>(CHouseArray::mStaticVars + TargetData_GetObjectId(target) * CHouseArray::ElementSize, 0x24);
            if (sameContinent)
            {
                const auto minimum = tooClose ? static_cast<std::uint32_t>(weapon.minimumRange) : static_cast<std::uint32_t>(weapon.maximumRange) - 5u;
                if (Tool_Vehicle_GetNextMovePointNearPosition(Read<std::uint32_t>(vehicle, 0x1E8), &destination, &source, weapon.maximumRange, minimum) && Vehicle_l_StartAtomicWalk(vehicle, &source, false)) return true;
            }
            else
            {
                std::uint32_t freePoint = 0;
                if (Vehicle_l_Military_ScanForAttackPoint(vehicle, &destination, &freePoint) && Vehicle_l_StartAtomicWalk(vehicle, &freePoint, false)) return true;
            }
            Write<std::uint32_t>(target, 0, 0);
            return false;
        }
        if (Distance(Vehicle_GetMapPositionRef(vehicle), At(vehicle, 0x1D6)) > 3) { Vehicle_l_StartAtomicWalk(vehicle, At(vehicle, 0x1D6), false); return true; }
        return false;
    }

    // Original function: FUN_00442FF7.
    bool __cdecl slVehicleSearchNextFreeMilitaryMapPointFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
    {
        auto& search = *static_cast<SAttackPointSearch*>(callback->userData);
        const auto distance = Distance(&search.source, point);
        if (distance > search.bestDistance) return true;
        const auto targetDistance = Distance(&search.target, point);
        if (targetDistance < search.minimumRange || targetDistance > search.maximumRange) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(search.owner)->logicSize)) return false;
        CMapMoveableIterator iterator;
        for (auto id = iterator.GetFirstMapMoveableId(point); id != -1; id = iterator.GetNextMapMoveableId())
        {
            if (static_cast<std::uint32_t>(id) == Read<std::uint32_t>(search.owner, 0x1DC)) continue;
            const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
            if (Read<std::uint32_t>(moveable, 8) != 3) continue;
            const auto* other = Vehicle(Read<std::uint32_t>(moveable, 0x54));
            if (Read<std::uint32_t>(other, 0x1E0) == Read<std::uint32_t>(search.owner, 0x1E0) && Read<std::uint32_t>(other, 0x4FC) == 0x51) return false;
        }
        search.bestDistance = distance;
        search.points[search.count++] = Read<std::uint32_t>(point, 0);
        return search.count == 6;
    }

    // Original function: FUN_00442F26.
    bool Vehicle_l_Military_ScanForAttackPoint(void* vehicle, const void* source, void* point)
    {
        const auto& weapon = Weapon(vehicle);
        SAttackPointSearch search;
        search.owner = vehicle;
        search.maximumRange = weapon.maximumRange;
        search.minimumRange = weapon.minimumRange;
        search.target = Read<std::uint32_t>(source, 0);
        search.source = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
        CMapPathfinderElement::SearchOverMap(&search.source, 5, true, slVehicleSearchNextFreeMilitaryMapPointFunction, &search, false, Read<std::uint32_t>(vehicle, 0x1E0));
        if (search.count == 0) return false;
        Write(point, 0, search.points[Game_Random_GetNumber() % search.count]);
        return true;
    }

    // Original function: FUN_004628F4; callback emission precedes delayed damage and exploration.
    void Vehicle_l_PerformTaskEvent(void* vehicle, const SStaticAtomicAnimationEventData* event)
    {
        if (event->type != 0x19) return;
        auto* target = At(vehicle, 0x520 + Read<std::uint32_t>(vehicle, 0x538) * 12);
        if (!TargetData_IsValid(target) || !Vehicle_Passengers_IsCommanderInVehicle(vehicle)) return;
        const auto& weapon = Weapon(vehicle);
        auto source = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
        std::uint32_t destination = 0;
        TargetData_GetMapPosition(target, &destination);
        const auto distance = Distance(&destination, &source);
        const auto chance = Game_Random_GetNumber() % 100u;
        const auto experience = Read<std::uint32_t>(Vehicle_Passengers_GetAttachedCommanderPtr(vehicle), 0x154);
        if (chance > experience + 10u)
        {
            const auto spread = (chance - experience - 10u) * (distance >> 2) / chance;
            const auto divider = NXBasics::Runtime_TimeSigned32(spread + 1u), half = NXBasics::Runtime_TimeSigned32(spread) / 2;
            const auto y = static_cast<std::int16_t>(Game_Random_GetNumber()) % divider - half;
            const auto x = static_cast<std::int16_t>(Game_Random_GetNumber()) % divider - half;
            MapMigPoint_AddOffset(&destination, static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
        }
        const auto ticks = (Distance(&destination, &source) << 3) / static_cast<std::uint32_t>(weapon.speed);
        std::uint32_t callback[9] = { source, destination, 0, 0, 0, 0, 0, static_cast<std::uint32_t>(weapon.munitionType), ticks };
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x23, true, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(callback)), 0, 0);
        DelayedActionManager_DelayedAction_AddWeaponHit(gDelayedActionManager, ticks, 2, Read<std::uint32_t>(vehicle, 4), &destination, WeaponId(vehicle));
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(vehicle, 0x1E0), &destination, 2);
    }

    // Original function: FUN_00462AD8; the original exported name is unresolved.
    void Vehicle_l_StartAtomicAttachToVehicle(void* vehicle)
    {
        bool cancel = !Vehicle_Attachment_IsValid(vehicle);
        if (!cancel)
        {
            auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, 0x540));
            if (!Vehicle_Passengers_CanVehicleMoveIn(carrier, Read<std::uint32_t>(vehicle, 4))) { SendVehicleMessage(vehicle, 0x36); cancel = true; }
            else
            {
                std::uint32_t point = 0;
                if (!Vehicle_Door_GetEntryPoint(carrier, &point, Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) { SendVehicleMessage(vehicle, 0x38); cancel = true; }
                else if (point == Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0))
                {
                    if (Vehicle_Passengers_AllPassengersInside(vehicle))
                    {
                        if (Vehicle_Attachment_EnterVehicle(vehicle)) { Write<std::uint32_t>(vehicle, 0x1C8, 0); Vehicle_l_StartAtomicIdle(vehicle); return; }
                        cancel = true;
                    }
                    else
                    {
                        std::byte command[20]; MakeCommand(command, vehicle, 0x73);
                        Vehicle_DoAddNewUserCommand(vehicle, command);
                        Vehicle_l_Passengers_MoveIn(vehicle);
                        Write<std::uint32_t>(vehicle, 0x1C8, 3);
                        return;
                    }
                }
                else if (Vehicle_l_StartAtomicWalk(vehicle, &point, false)) return;
                else if (Read<std::uint32_t>(vehicle, 0x1C8) == 3)
                {
                    std::byte command[20]; MakeCommand(command, vehicle, 0x73);
                    Vehicle_DoAddNewUserCommand(vehicle, command);
                    return;
                }
                else cancel = true;
            }
        }
        if (cancel)
        {
            Vehicle_l_UserCommands_DetachFromVehicle(vehicle);
            Write<std::uint32_t>(vehicle, 0x1C8, 0);
            Vehicle_l_StartAtomicIdle(vehicle);
        }
    }
}
