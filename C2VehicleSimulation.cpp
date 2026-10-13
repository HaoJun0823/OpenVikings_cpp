#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
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
        void* Moveable(const void* vehicle) { return gMapMoveableArrayStorage.entries + Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MapMoveableId) * NC2Layout::RecordSize::MapMoveable; }
        void SendVehicleMessage(const void* vehicle, std::uint32_t type)
        {
            std::byte message[772];
            ToUserMessage_Construct(message);
            Write(message, 0x20, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId));
            Write(message, 0x24, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId));
            Write(message, 0x2E0, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType));
            Write(message, 0, type);
            Write(message, 8, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        std::uint32_t Distance(const void* source, const void* target) { return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(target, 0), Read<std::int16_t>(target, 2))); }
        void* Vehicle(std::uint32_t id) { return CVehicleArray::mStaticVars + id * CVehicleArray::ElementSize; }
        std::byte* At(void* object, std::uint32_t offset) { return static_cast<std::byte*>(object) + offset; }
        std::uint32_t WeaponId(const void* vehicle) { return Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType) == 5 ? 21u : 0u; }
        const SStaticWeaponTypeData& Weapon(const void* vehicle) { return static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::TribeType)])[WeaponId(vehicle)]; }
        void MakeCommand(void* command, const void* vehicle, std::uint8_t opcode)
        {
            std::memset(command, 0, 20);
            Write(command, NC2Layout::UserNetworkCommandOffset::Opcode, opcode);
            Write(command, NC2Layout::UserNetworkCommandOffset::PlayerId, static_cast<std::uint8_t>(Read<std::uint32_t>(vehicle, 0x1E0)));
            Write(command, NC2Layout::UserNetworkCommandOffset::ObjectId, Read<std::uint32_t>(vehicle, 4));
            Write(command, NC2Layout::UserNetworkCommandOffset::ObjectRevision, Read<std::uint32_t>(vehicle, 8));
        }
    }

    // Original function: FUN_0043711D.
    void Vehicle_DoUpdateConditions(void* vehicle)
    {
        if (Read<std::int32_t>(vehicle, NC2Layout::VehicleOffset::Hitpoints) <= 0) Write<std::uint8_t>(Moveable(vehicle), 0x0C, 1);
    }

    // Original function: FUN_00438057; events execute before advancing the tick.
    void Vehicle_CurrentTask_DoPerform(void* vehicle)
    {
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction) == 0) { Vehicle_l_StartAtomicIdle(vehicle); return; }
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationRunningFlag) == 0) return;
        const auto* animation = gStaticAtomicAnimationStorage.records + Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationId);
        for (std::uint32_t index = 0; index < animation->eventCount; ++index)
            if (static_cast<std::uint32_t>(animation->events[index].time) == Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationFrame)) Vehicle_l_PerformTaskEvent(vehicle, &animation->events[index]);
        Write(vehicle, NC2Layout::VehicleOffset::AtomicAnimationFrame, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationFrame) + 1u);
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationFrame) != static_cast<std::uint32_t>(animation->length)) return;
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction) == 0x54)
        {
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag, 1);
        }
        if (animation->endDirection != -1) Write(Moveable(vehicle), 0x20, animation->endDirection);
        Vehicle_l_ResetAtomic(vehicle);
    }

    // Original function: FUN_0043810E; this outlined predicate has no established Mac name.
    bool Vehicle_IsTaskOrAttachmentActive(const void* vehicle)
    {
        const auto task = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask);
        return task == 5 || task == 6 || task == 4 || task == 3 || Vehicle_Attachment_IsInsideVehicle(vehicle);
    }

    // Original function: FUN_00438138.
    void Vehicle_l_Error_NoCommander(const void* vehicle) { SendVehicleMessage(vehicle, 0x33); }

    // Original function: FUN_00441826; arithmetic wraps before signed division.
    void Vehicle_WalkSpeed_Update(void* vehicle, std::uint32_t terrainSpeed)
    {
        auto ticks = terrainSpeed * 2u + 4u;
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType) == 5) ticks *= 2u;
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
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<std::byte*>(vehicle) + NC2Layout::VehicleOffset::DoorEntryPoint), 6);
        Vehicle_l_GetDoorPoint(vehicle, continent, Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0));
        if (!MapMigPoint_IsInsideMap(static_cast<std::byte*>(vehicle) + NC2Layout::VehicleOffset::DoorEntryPoint)) return false;
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget, 0);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget, 0);
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
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction, 0);
        Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::AtomicAnimationRunningFlag, 0);
    }

    // Original function: FUN_00437138.
    void Vehicle_DoUpdateAI(void* vehicle)
    {
        Vehicle_CurrentTask_DoPerform(vehicle);
        if (!Vehicle_l_IsAtomicInterruptable(vehicle)) return;
        if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 4);
        Vehicle_DoExecuteNextUserCommand(vehicle);
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction) == 1) return;
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) != 0)
        {
            if (Vehicle_Attachment_IsValid(vehicle) && !Vehicle_Attachment_IsInsideVehicle(vehicle))
            {
                if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask) != 3) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 6);
            }
            else if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::JobType) == 0x36 && !Vehicle_IsTaskOrAttachmentActive(vehicle))
            {
                if (!TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget)) && !TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget)))
                {
                    switch (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MilitaryMode))
                    {
                    case SwitchIds::VehicleMilitaryMode::SearchAroundPosition: Vehicle_l_Military_CheckForEnemys(vehicle, Vehicle_GetMapPositionRef(vehicle), 0, 40); break;
                    case SwitchIds::VehicleMilitaryMode::SearchAroundTarget: Vehicle_l_Military_CheckForEnemys(vehicle, At(vehicle, NC2Layout::VehicleOffset::MilitaryCenter), 0, 40); break;
                    case SwitchIds::VehicleMilitaryMode::SearchWithinWeaponRange: Vehicle_l_Military_CheckForEnemys(vehicle, At(vehicle, NC2Layout::VehicleOffset::MilitaryCenter), Weapon(vehicle).minimumRange, Weapon(vehicle).maximumRange); break;
                    }
                }
                if ((TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget)) || TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget))) && (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask) == 0 || Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask) == 2))
                {
                    if (!Vehicle_l_Passengers_MoveIn(vehicle)) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 3);
                    else if (Vehicle_l_Military_AttackTask(vehicle)) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 2);
                }
            }
        }
        switch (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask))
        {
        case SwitchIds::VehicleTask::Dock:
            if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction) != 0) return;
            SendVehicleMessage(vehicle, 0x32);
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            break;
        case SwitchIds::VehicleTask::Attack:
            if (TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget)) || TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget)))
            {
                if (Vehicle_l_Passengers_MoveIn(vehicle)) return;
                Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 3);
            }
            else Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            break;
        case SwitchIds::VehicleTask::WaitForPassengers:
            if (Vehicle_l_Passengers_MoveIn(vehicle)) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            break;
        case SwitchIds::VehicleTask::GetDraggingAnimal:
            if (!Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            else if (!Vehicle_Passengers_IsHavingDraggingAnimal(vehicle)) Vehicle_Passengers_TryToGetDraggingAnimal(vehicle);
            break;
        case SwitchIds::VehicleTask::Interrupted: Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0); break;
        case SwitchIds::VehicleTask::AttachToVehicle: Vehicle_l_StartAtomicAttachToVehicle(vehicle); return;
        }
        Vehicle_l_StartAtomicIdle(vehicle);
    }

    // Original function: FUN_004373C7.
    bool Vehicle_DoNewMapPositionReached(void* vehicle, bool destinationReached)
    {
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), Vehicle_GetMapPositionRef(vehicle), Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::ExplorationRadius));
        const auto* micro = GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle));
        Vehicle_WalkSpeed_Update(vehicle, (Read<std::uint8_t>(micro, 0x0C) >> 3) & 15u);
        Vehicle_l_SendAwayRadial(vehicle);
        Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag, 0);
        if (Vehicle_IsPriorityCommandInQueue(vehicle)) { Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction, 0); return false; }
        Vehicle_DoExecuteNextUserCommand(vehicle);
        if (!destinationReached)
        {
            if ((Read<std::uint8_t>(micro, 0x0C) & NXBasics::BitMask::Bit7) != 0 || (Read<std::uint8_t>(micro, 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) return true;
            if (!Vehicle_Passengers_IsCommanderInVehicle(vehicle)) { Vehicle_l_StartAtomicIdle(vehicle); return false; }
            return true;
        }
        if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask) == 1) { Vehicle_l_StartAtomicDock(vehicle); return true; }
        const auto type = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType);
        if ((type == 3 || type == 4) && !Vehicle_Passengers_IsCommanderInVehicle(vehicle))
        {
            const auto point = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
            const auto continent = Vehicle_l_GetLandContinentIdFromBorder(vehicle, point);
            if (continent == -1) MapMigPoint_Construct(At(vehicle, NC2Layout::VehicleOffset::DoorEntryPoint), -1, -1, false);
            else { Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag, 1); Vehicle_l_GetDoorPoint(vehicle, continent, point); }
        }
        Vehicle_l_Passengers_StopAllIdleTasks(vehicle);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::AtomicAction, 0);
        return true;
    }

    // Original function: FUN_00437514.
    void Vehicle_DoPathfinderFailed(void* vehicle)
    {
        SendVehicleMessage(vehicle, 0x32);
        Write(vehicle, NC2Layout::VehicleOffset::MilitaryCenter, Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0));
        if (Vehicle_l_IsAtomicInterruptable(vehicle)) Vehicle_l_ResetAtomic(vehicle);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget, 0);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget, 0);
    }

    // Original function: FUN_0043F52D.
    bool Vehicle_DoExecuteNextUserCommand(void* vehicle)
    {
        for (std::uint32_t index = 0; index < 22; ++index)
        {
            auto* command = At(vehicle, NC2Layout::VehicleOffset::UserCommandQueueSigned + index * 20);
            if (Read<std::uint8_t>(command, NC2Layout::UserNetworkCommandOffset::Opcode) != 0 && Vehicle_DoExecuteUserCommand(vehicle, command))
            {
                Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::UserCommandQueueSigned + index * 20, 0);
                return true;
            }
        }
        return false;
    }

    // Original function: FUN_0043F58C.
    bool Vehicle_DoExecuteUserCommand(void* vehicle, const void* command)
    {
        const auto opcode = Read<std::uint8_t>(command, NC2Layout::UserNetworkCommandOffset::Opcode);
        if (((opcode >= SwitchIds::UserCommand::VehicleGotoAndWait && opcode <= SwitchIds::UserCommand::VehicleAttackMapPosition) || opcode == SwitchIds::UserCommand::VehicleAttachToCarrier) && Vehicle_Attachment_IsValid(vehicle) && !Vehicle_l_UserCommands_DetachFromVehicle(vehicle)) return true;
        auto point = Read<std::uint32_t>(command, NC2Layout::UserNetworkCommandOffset::Parameter1);
        if (opcode == SwitchIds::UserCommand::VehicleGotoAndWait)
        {
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            return Vehicle_DoExecuteUserCommand_Goto(vehicle, &point);
        }
        if (opcode == SwitchIds::UserCommand::VehicleUnload)
        {
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            Vehicle_l_StartAtomicUnload(vehicle, true);
            return true;
        }
        if (opcode == SwitchIds::UserCommand::VehicleDock) return Vehicle_DoExecuteUserCommand_Dock(vehicle, &point);
        if (opcode >= SwitchIds::UserCommand::VehicleAttackHuman && opcode <= SwitchIds::UserCommand::VehicleAttackVehicle)
        {
            const auto id = Read<std::uint32_t>(command, NC2Layout::UserNetworkCommandOffset::Parameter1), revision = Read<std::uint32_t>(command, NC2Layout::UserNetworkCommandOffset::Parameter2);
            const std::uint32_t types[] = { 1, 4, 3, 2 };
            void* object = opcode == SwitchIds::UserCommand::VehicleAttackHuman ? static_cast<void*>(CHumanArray::mStaticVars + id * CHumanArray::ElementSize) : opcode == SwitchIds::UserCommand::VehicleAttackHouse ? static_cast<void*>(CHouseArray::mStaticVars + id * CHouseArray::ElementSize) : opcode == SwitchIds::UserCommand::VehicleAttackAnimal ? static_cast<void*>(CAnimalArray::mStaticVars + id * CAnimalArray::ElementSize) : Vehicle(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return true;
            Write(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTargetValue, id);
            Write(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTargetRevision, revision);
            Write(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget, types[opcode - SwitchIds::UserCommand::VehicleAttackHuman]);
        }
        else if (opcode == SwitchIds::UserCommand::VehicleAttackMapPosition)
        {
            if (!MapMigPoint_IsInsideMap(&point)) return true;
            const bool landscape = Tool_IsEnemyLandscapeOnPoint(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), &point);
            Write(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTargetValue, point);
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget, landscape ? 6 : 5);
            if (!landscape) Write<std::uint16_t>(vehicle, 0x52A, 0);
        }
        else if (opcode == SwitchIds::UserCommand::VehicleInterrupt)
        {
            Vehicle_l_Passengers_StopAllIdleTasks(vehicle);
            if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MilitaryCenter) != Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0) && !TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget)) && !TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget)))
            {
                std::byte next[20]; MakeCommand(next, vehicle, SwitchIds::UserCommand::VehicleGotoAndWait);
                Write(next, 0x0C, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MilitaryCenter));
                Vehicle_DoAddNewUserCommand(vehicle, next);
            }
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 5);
            return true;
        }
        else if (opcode == SwitchIds::UserCommand::VehicleAttachToCarrier)
        {
            auto* carrier = Vehicle(Read<std::uint32_t>(command, NC2Layout::UserNetworkCommandOffset::Parameter1));
            if (Read<std::uint8_t>(carrier, NC2Layout::VehicleOffset::AllocatedFlag) != 0 && Read<std::uint32_t>(carrier, NC2Layout::VehicleOffset::UniqueId) == Read<std::uint32_t>(command, NC2Layout::UserNetworkCommandOffset::Parameter2) && Vehicle_Passengers_CanVehicleBeAttached(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId)) && Vehicle_Passengers_AttachVehicle(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId)))
            {
                Write(vehicle, NC2Layout::VehicleOffset::CarrierVehicleId, Read<std::uint32_t>(carrier, NC2Layout::VehicleOffset::VehicleId));
                Write(vehicle, NC2Layout::VehicleOffset::CarrierVehicleRevision, Read<std::uint32_t>(carrier, NC2Layout::VehicleOffset::UniqueId));
            }
            return true;
        }
        else if (opcode == SwitchIds::UserCommand::VehicleDetachFromCarrier) return Vehicle_l_UserCommands_DetachFromVehicle(vehicle);
        else if (opcode == SwitchIds::UserCommand::VehicleMoveIntoCarrier)
        {
            if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) { SendVehicleMessage(vehicle, 0x35); return true; }
            if (!Vehicle_Passengers_AllPassengersInside(vehicle)) return false;
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 6);
            return true;
        }
        else return true;
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget, 0);
        return Vehicle_l_IsAtomicInterruptable(vehicle);
    }

    // Original function: FUN_0043F96C; the PE prologue resolves the misaligned export.
    bool Vehicle_l_UserCommands_DetachFromVehicle(void* vehicle)
    {
        auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CarrierVehicleId));
        if (Vehicle_Attachment_IsInsideVehicle(vehicle))
        {
            const auto type = Read<std::uint32_t>(carrier, NC2Layout::VehicleOffset::LogicType);
            if ((type == 3 || type == 4) && Read<std::uint8_t>(carrier, NC2Layout::VehicleOffset::DockedFlag) == 0) return false;
            if (Read<std::uint32_t>(carrier, NC2Layout::VehicleOffset::AtomicAction) == 1)
            {
                std::byte command[20]; MakeCommand(command, carrier, SwitchIds::UserCommand::VehicleGotoAndWait);
                Write(command, NC2Layout::UserNetworkCommandOffset::PlayerId, static_cast<std::uint8_t>(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId)));
                std::uint32_t point = 0;
                MapMoveable_Attachment_GetNextMapPosition(Moveable(carrier), &point);
                Write(command, NC2Layout::UserNetworkCommandOffset::Parameter1, point);
                Vehicle_DoAddNewUserCommand(carrier, command);
                return false;
            }
            return Vehicle_Passengers_MoveOut_v(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId));
        }
        if (!Vehicle_Passengers_DetachVehicle(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId))) SendVehicleMessage(vehicle, 0x39);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
        return true;
    }

    // Original function: FUN_0043FA9A.
    bool Vehicle_DoExecuteUserCommand_Goto(void* vehicle, const void* point)
    {
        if (Vehicle_Passengers_NeedsDraggingAnimal(vehicle)) { SendVehicleMessage(vehicle, 0x35); return true; }
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) == 0) { Vehicle_l_Error_NoCommander(vehicle); return true; }
        if (Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0) == Read<std::uint32_t>(point, 0)) { Vehicle_l_StartAtomicIdle(vehicle); return true; }
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6);
        if (continent != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(vehicle)), 6)) return true;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & 7u) < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) return true;
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget, 0);
        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget, 0);
        if (Vehicle_l_StartAtomicWalk(vehicle, point, false)) { Write(vehicle, NC2Layout::VehicleOffset::MilitaryCenter, Read<std::uint32_t>(point, 0)); return true; }
        const auto type = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType);
        if ((type == 3 || type == 4) && !Vehicle_Passengers_IsCommanderInVehicle(vehicle) && Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag) == 0) return true;
        return false;
    }

    // Original function: FUN_0043FBD5; the native perimeter advances before checking each point.
    bool Vehicle_DoExecuteUserCommand_Dock(void* vehicle, const void* point)
    {
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & NXBasics::BitMask::Bit7) != 0) return true;
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) == 0) { Vehicle_l_Error_NoCommander(vehicle); return true; }
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag) != 0 && !Vehicle_l_Passengers_MoveIn(vehicle)) { Vehicle_l_StartAtomicIdle(vehicle); return false; }
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
                Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 1);
                Write(vehicle, NC2Layout::VehicleOffset::DoorEntryPoint, Read<std::uint32_t>(point, 0));
                return true;
            }
        }
        if (!Vehicle_Passengers_IsCommanderInVehicle(vehicle))
        {
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 1);
            Write(vehicle, NC2Layout::VehicleOffset::DoorEntryPoint, Read<std::uint32_t>(point, 0));
            Vehicle_l_StartAtomicDock(vehicle);
        }
        else SendVehicleMessage(vehicle, 0x32);
        return true;
    }

    // Original function: FUN_0044C725; references across a continent receive detach commands.
    bool Vehicle_l_Passengers_MoveIn(void* vehicle)
    {
        const auto type = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType);
        if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag) == 0) return true;
        bool inside = true;
        std::uint32_t point = 0;
        Vehicle_Door_GetEntryPoint(vehicle, &point, 0);
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6);
        std::byte command[20]; MakeCommand(command, vehicle, 0x28);
        Write(command, NC2Layout::UserNetworkCommandOffset::Parameter1, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId));
        Write(command, NC2Layout::UserNetworkCommandOffset::Parameter2, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId));
        const auto humanSlots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) + 1u;
        for (std::uint32_t index = 0; index < humanSlots; ++index)
        {
            auto* slot = At(vehicle, NC2Layout::VehicleOffset::HumanPassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned);
            if (!HumanReference_IsAllocated(slot) || Read<std::uint8_t>(slot, 8) != 0) continue;
            auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(slot, 0) * CHumanArray::ElementSize;
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human)), 6) != continent)
            {
                Human_DoAddNewUserCommand(human, SwitchIds::UserCommand::DetachVehicle, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), Read<std::uint32_t>(human, NC2Layout::HumanOffset::HumanId), Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId), 0, 0);
                continue;
            }
            inside = false;
            if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::CurrentTaskParameter) == 0)
            {
                Write(command, NC2Layout::UserNetworkCommandOffset::ObjectId, Read<std::uint32_t>(human, NC2Layout::HumanOffset::HumanId));
                Write(command, NC2Layout::UserNetworkCommandOffset::ObjectRevision, Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId));
                Human_DoAddNewUserCommand(human, command);
                Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 3);
            }
        }
        Vehicle_Door_GetEntryPoint(vehicle, &point, 1);
        MakeCommand(command, vehicle, SwitchIds::UserCommand::VehicleMoveIntoCarrier);
        Write(command, NC2Layout::UserNetworkCommandOffset::Parameter1, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId));
        Write(command, NC2Layout::UserNetworkCommandOffset::Parameter2, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::UniqueId));
        for (std::uint32_t index = 0; index < static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->vehicleSlots); ++index)
        {
            auto* slot = At(vehicle, NC2Layout::VehicleOffset::VehiclePassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned);
            if (!VehicleReference_IsAllocated(slot) || Read<std::uint8_t>(slot, 8) != 0) continue;
            auto* passenger = Vehicle(Read<std::uint32_t>(slot, 0));
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Vehicle_GetMapPositionRef(passenger)), 6) != continent)
            {
                std::byte detach[20]; MakeCommand(detach, vehicle, SwitchIds::UserCommand::VehicleDetachFromCarrier);
                Vehicle_DoAddNewUserCommand(passenger, detach);
                continue;
            }
            inside = false;
            if (Read<std::uint32_t>(passenger, NC2Layout::VehicleOffset::CurrentTask) != 6)
            {
                Write(command, NC2Layout::UserNetworkCommandOffset::ObjectId, Read<std::uint32_t>(passenger, NC2Layout::VehicleOffset::VehicleId));
                Write(command, NC2Layout::UserNetworkCommandOffset::ObjectRevision, Read<std::uint32_t>(passenger, NC2Layout::VehicleOffset::UniqueId));
                Vehicle_DoAddNewUserCommand(passenger, command);
            }
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 3);
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
            auto* slot = At(vehicle, NC2Layout::VehicleOffset::VehiclePassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned);
            if (VehicleReference_IsAllocated(slot)) continue;
            Write<std::uint8_t>(slot, 8, 0);
            Write(slot, 0, Read<std::uint32_t>(passenger, NC2Layout::VehicleOffset::VehicleId));
            Write(slot, 4, Read<std::uint32_t>(passenger, NC2Layout::VehicleOffset::UniqueId));
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
        Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::VehiclePassengerInsideFlagsSigned + static_cast<std::uint32_t>(slot) * NC2Layout::RecordSize::VehiclePassengerSigned, 1);
        return true;
    }

    // Original function: FUN_0044CCC2.
    bool Vehicle_Passengers_CanVehicleMoveIn(const void* vehicle, std::uint32_t id)
    {
        if (Vehicle_Passengers_GetAttachedVehicleSlot(vehicle, id) == -1 && !Vehicle_Passengers_CanVehicleBeAttached(vehicle, id)) return false;
        const auto type = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType);
        if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag) == 0) return false;
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
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) == 0)
        {
            std::memcpy(At(vehicle, NC2Layout::VehicleOffset::HumanPassengersSigned + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) * NC2Layout::RecordSize::VehiclePassengerSigned), slot, 12);
            Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag, 1);
        }
        else
        {
            const auto slots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
            for (std::uint32_t index = 0; index < slots; ++index)
            {
                auto* destination = At(vehicle, NC2Layout::VehicleOffset::HumanPassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned);
                if (!HumanReference_IsAllocated(destination)) { std::memcpy(destination, slot, 12); break; }
            }
        }
        auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(slot, 0) * CHumanArray::ElementSize;
        const auto id = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId);
        Write(human, NC2Layout::HumanOffset::AttachedVehicleId, id);
        Write(human, NC2Layout::HumanOffset::AttachedVehicleRevision, Read<std::uint32_t>(Vehicle(id), NC2Layout::VehicleOffset::UniqueId));
        Write<std::uint32_t>(slot, 0, NXBasics::IntegerRange::UInt32Maximum);
        Write<std::uint8_t>(slot, 8, 0);
    }

    // Original function: FUN_0044DAC6; human transfers precede the final vehicle move-in and have no rollback.
    bool Vehicle_Attachment_EnterVehicle(void* vehicle)
    {
        auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CarrierVehicleId));
        if (!Vehicle_Passengers_CanVehicleMoveIn(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId))) return false;
        const auto slots = static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots);
        for (std::uint32_t index = 0; index < slots; ++index)
        {
            auto* slot = At(vehicle, NC2Layout::VehicleOffset::HumanPassengersSigned + index * NC2Layout::RecordSize::VehiclePassengerSigned);
            if (HumanReference_IsAllocated(slot)) Vehicle_Passengers_ExtractPassengerSlot(carrier, slot);
        }
        if (Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag) != 0)
        {
            Vehicle_Passengers_ExtractPassengerSlot(carrier, At(vehicle, NC2Layout::VehicleOffset::HumanPassengersSigned + static_cast<std::uint32_t>(Vehicle_GetVehicleTypeDataPtr(vehicle)->passengerSlots) * NC2Layout::RecordSize::VehiclePassengerSigned));
            Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::CommanderAttachedFlag, 0);
        }
        if (!Vehicle_Passengers_MoveIn_v(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId))) return false;
        MapMoveable_Attachment_DetachFromMap(Moveable(vehicle));
        Write<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::InsideVehicleFlag, 1);
        return true;
    }

    // Original function: FUN_00442903; priority lists precede ordinary lists, vehicles precede houses.
    bool Vehicle_l_Military_CheckForEnemys(void* vehicle, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance)
    {
        const auto* source = Vehicle_GetMapPositionRef(vehicle);
        for (std::uint32_t branch = 0; branch < 4; ++branch)
        {
            const bool house = (branch & 1u) != 0, priority = branch < 2;
            const auto player = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId);
            const auto id = house ? Tool_GetOneOfNextEnemyHousesInRange(player, point, minimumDistance, maximumDistance, priority, 5, 3) : Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, point, minimumDistance, maximumDistance, priority, 5, 3, false, false);
            if (id == -1) continue;
            const auto* candidate = house ? static_cast<const void*>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize) : Vehicle(id);
            if (TargetData_IsValid(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget)))
            {
                std::int16_t previous[2] = { 10000, 10000 };
                TargetData_GetMapPosition(At(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget), previous);
                const auto* candidatePoint = house ? static_cast<const std::byte*>(candidate) + 0x1C : Vehicle_GetMapPositionRef(candidate);
                if (Distance(source, previous) <= Distance(source, candidatePoint)) return false;
            }
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTarget, house ? 4 : 2);
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTargetValue, id);
            Write(vehicle, NC2Layout::VehicleOffset::SecondaryMilitaryTargetRevision, Read<std::uint32_t>(candidate, 8));
            return true;
        }
        return false;
    }

    // Original function: FUN_00442BD9.
    bool Vehicle_l_Military_AttackTask(void* vehicle)
    {
        for (std::uint32_t index = 0; index < 2; ++index)
        {
            auto* target = At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget + index * 12);
            if (!TargetData_IsValid(target)) { Write<std::uint32_t>(target, 0, 0); continue; }
            const auto player = TargetData_GetPlayerId(target);
            const auto relation = player == 20 || player == -1 ? 3 : GameControl_Diplomacy_GetState(gGameControl, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), player);
            if (relation == 1 || relation == 2) { Write<std::uint32_t>(target, 0, 0); continue; }
            const auto& weapon = Weapon(vehicle);
            std::uint32_t destination = 0;
            TargetData_GetMapPosition(target, &destination);
            auto source = Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0);
            if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MilitaryMode) == 2 && index == 1 && Distance(At(vehicle, NC2Layout::VehicleOffset::MilitaryCenter), &source) > 60) { Write<std::uint32_t>(target, 0, 0); return false; }
            const auto distance = Distance(&source, &destination);
            if (distance >= static_cast<std::uint32_t>(weapon.minimumRange) && distance <= static_cast<std::uint32_t>(weapon.maximumRange))
            {
                if (Vehicle_GoAway_HitByVehicle(vehicle, &source))
                {
                    std::uint32_t freePoint = 0;
                    if (Vehicle_l_GoAway_GetVehicleFreePos(vehicle, &freePoint)) { Vehicle_l_StartAtomicWalk(vehicle, &freePoint, false); return false; }
                }
                const auto direction = VE_HexagonDirection_GetDirectionFromPositions(Read<std::int16_t>(&source, 0), Read<std::int16_t>(&source, 2), Read<std::int16_t>(&destination, 0), Read<std::int16_t>(&destination, 2), true);
                if (Vehicle_l_StartAtomicAnimation(vehicle, 0x51, direction)) { Write(vehicle, NC2Layout::VehicleOffset::ActiveMilitaryTargetIndex, index); return true; }
                continue;
            }
            if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::MilitaryMode) == 3 && index == 1) { Write<std::uint32_t>(target, 0, 0); return false; }
            const bool tooClose = distance < static_cast<std::uint32_t>(weapon.minimumRange);
            const bool sameContinent = tooClose || Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&source), 6) == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&destination), 6);
            if (TargetData_GetTargetType(target) == 4) destination = Read<std::uint32_t>(CHouseArray::mStaticVars + TargetData_GetObjectId(target) * CHouseArray::ElementSize, NC2Layout::HouseOffset::DoorEntryPoint);
            if (sameContinent)
            {
                const auto minimum = tooClose ? static_cast<std::uint32_t>(weapon.minimumRange) : static_cast<std::uint32_t>(weapon.maximumRange) - 5u;
                if (Tool_Vehicle_GetNextMovePointNearPosition(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType), &destination, &source, weapon.maximumRange, minimum) && Vehicle_l_StartAtomicWalk(vehicle, &source, false)) return true;
            }
            else
            {
                std::uint32_t freePoint = 0;
                if (Vehicle_l_Military_ScanForAttackPoint(vehicle, &destination, &freePoint) && Vehicle_l_StartAtomicWalk(vehicle, &freePoint, false)) return true;
            }
            Write<std::uint32_t>(target, 0, 0);
            return false;
        }
        if (Distance(Vehicle_GetMapPositionRef(vehicle), At(vehicle, NC2Layout::VehicleOffset::MilitaryCenter)) > 3) { Vehicle_l_StartAtomicWalk(vehicle, At(vehicle, NC2Layout::VehicleOffset::MilitaryCenter), false); return true; }
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
            const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * NC2Layout::RecordSize::MapMoveable;
            if (Read<std::uint32_t>(moveable, 8) != 3) continue;
            const auto* other = Vehicle(Read<std::uint32_t>(moveable, NC2Layout::MapMoveableOffset::ObjectId));
            if (Read<std::uint32_t>(other, NC2Layout::VehicleOffset::PlayerId) == Read<std::uint32_t>(search.owner, 0x1E0) && Read<std::uint32_t>(other, NC2Layout::VehicleOffset::AtomicAction) == 0x51) return false;
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
        CMapPathfinderElement::SearchOverMap(&search.source, 5, true, slVehicleSearchNextFreeMilitaryMapPointFunction, &search, false, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId));
        if (search.count == 0) return false;
        Write(point, 0, search.points[Game_Random_GetNumber() % search.count]);
        return true;
    }

    // Original function: FUN_004628F4; callback emission precedes delayed damage and exploration.
    void Vehicle_l_PerformTaskEvent(void* vehicle, const SStaticAtomicAnimationEventData* event)
    {
        if (event->type != 0x19) return;
        auto* target = At(vehicle, NC2Layout::VehicleOffset::PrimaryMilitaryTarget + Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::ActiveMilitaryTargetIndex) * 12);
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
        DelayedActionManager_DelayedAction_AddWeaponHit(gDelayedActionManager, ticks, 2, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId), &destination, WeaponId(vehicle));
        GameMap_Exploration_DoExplore(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::PlayerId), &destination, 2);
    }

    // Original function: FUN_00462AD8; the original exported name is unresolved.
    void Vehicle_l_StartAtomicAttachToVehicle(void* vehicle)
    {
        bool cancel = !Vehicle_Attachment_IsValid(vehicle);
        if (!cancel)
        {
            auto* carrier = Vehicle(Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CarrierVehicleId));
            if (!Vehicle_Passengers_CanVehicleMoveIn(carrier, Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::VehicleId))) { SendVehicleMessage(vehicle, 0x36); cancel = true; }
            else
            {
                std::uint32_t point = 0;
                if (!Vehicle_Door_GetEntryPoint(carrier, &point, Vehicle_GetVehicleTypeDataPtr(vehicle)->logicSize)) { SendVehicleMessage(vehicle, 0x38); cancel = true; }
                else if (point == Read<std::uint32_t>(Vehicle_GetMapPositionRef(vehicle), 0))
                {
                    if (Vehicle_Passengers_AllPassengersInside(vehicle))
                    {
                        if (Vehicle_Attachment_EnterVehicle(vehicle)) { Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0); Vehicle_l_StartAtomicIdle(vehicle); return; }
                        cancel = true;
                    }
                    else
                    {
                        std::byte command[20]; MakeCommand(command, vehicle, SwitchIds::UserCommand::VehicleMoveIntoCarrier);
                        Vehicle_DoAddNewUserCommand(vehicle, command);
                        Vehicle_l_Passengers_MoveIn(vehicle);
                        Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 3);
                        return;
                    }
                }
                else if (Vehicle_l_StartAtomicWalk(vehicle, &point, false)) return;
                else if (Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask) == 3)
                {
                    std::byte command[20]; MakeCommand(command, vehicle, SwitchIds::UserCommand::VehicleMoveIntoCarrier);
                    Vehicle_DoAddNewUserCommand(vehicle, command);
                    return;
                }
                else cancel = true;
            }
        }
        if (cancel)
        {
            Vehicle_l_UserCommands_DetachFromVehicle(vehicle);
            Write<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::CurrentTask, 0);
            Vehicle_l_StartAtomicIdle(vehicle);
        }
    }
}
