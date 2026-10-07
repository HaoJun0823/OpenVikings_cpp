#include "C2HumanTaskAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HouseAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        void* Moveable(const void* human) { return gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u; }
        const void* Position(const void* human) { return static_cast<const std::byte*>(Moveable(human)) + 0x12; }
        const void* JobData(const void* human) { return static_cast<const std::byte*>(gStaticJobTypeData) + Read<std::uint32_t>(human, 0x18) * 0x88u; }

        // The empty/skipped branches of DoExecuteNextUserCommand (0043BD97) have
        // no side effects. An eligible nonempty queue remains a dispatch boundary.
        bool CanContinueWithoutExecutingCommand(const void* human)
        {
            const auto circulation = Read<std::uint32_t>(human, 0xACC);
            if (Read<std::uint8_t>(human, 0xA60) != 0 || (circulation != 0 && circulation != 6) || Read<std::uint32_t>(human, 0x378) != 0) return true;
            for (std::uint32_t slot = 0; slot < 87; ++slot) if (Read<std::uint8_t>(human, 0x390u + slot * 20u) != 0) return false;
            return true;
        }

        // Established no-request branches of AI_DoCheckForUrgentNeeds (00463509).
        // Do not mark A61 when a request could require unimplemented item/need work.
        bool CheckUrgentNeedsWithoutStartingTask(void* human)
        {
            if (!Human_l_IsAtomicInterruptable(human) || Human_UserControl_IsExecutingUserCommand(human) || Read<std::uint8_t>(human, 0xA61) != 0) return true;
            if (!Human_IsAgeAdult(human) || (Read<std::uint32_t>(human, 0xB84) & 1u) != 0)
            {
                Write<std::uint8_t>(human, 0xA61, 1);
                Write<std::uint32_t>(human, 0x370, 0);
                return true;
            }
            // The four original thresholds at 004F3B58..004F3B67 are 2000.
            if (Read<std::int32_t>(human, 0xAA8) <= 2000 || Read<std::int32_t>(human, 0xAAC) <= 2000) return false;
            if (Read<std::int32_t>(human, 0xAB4) <= 2000 && Human_IsJobNeedingReligion(human)) return false;
            if (Read<std::int32_t>(human, 0xAB0) <= 2000 && Human_IsJobNeedingSocial(human)) return false;
            Write<std::uint8_t>(human, 0xA61, 1);
            return true;
        }
    }

    // Original function: FUN_0042cb92; Mac IsAbleToDoGoodieNow.
    bool Human_IsAbleToDoGoodieNow(const void* human)
    {
        if (Human_IsJobSoldier(human) || Human_IsJobHero(human)) return false;
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (task != 1 && (task < 24 || task > 29)) return false;
        return Read<std::uint32_t>(human, 0xAB8) != 23 && !Human_Misc_IsHidden(human) && Read<std::uint32_t>(human, 0xAD4) == 0 && Human_l_IsAtomicInterruptable(human);
    }

    // Original function: FUN_0042ceaf.
    void Human_Target_ResetSecondaryTarget(void* human) { Write<std::uint32_t>(human, 0xA84, 0); }

    // Original function: FUN_004369c8.
    bool Human_UserControl_IsExecutingUserCommand(const void* human) { return Read<std::uint32_t>(human, 0x368) == 4 || Human_UserControl_IsPriorityCommandInQueue(human); }

    // Partial original function: FUN_004433d1. Idle/job, waiting and goodie startup branches
    // are connected; queued commands, urgent requests and other task types remain pending.
    void Human_CurrentTask_DoStartUp(void* human)
    {
        if (!Human_l_IsAtomicInterruptable(human)) return;
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (task >= 24 && task <= 29)
        {
            Human_l_StartTask_Goodie(human);
            return;
        }
        if (task == 2)
        {
            Write<std::uint32_t>(human, 0x368, 2);
            Write<std::uint32_t>(human, 0x36C, 1);
            return;
        }
        if (task != 1 && task != 8 && task != 9) return;
        if (!CanContinueWithoutExecutingCommand(human)) return;
        // Task 8's food-producer postponement branch remains pending.
        if (task == 8 && Read<std::uint32_t>(human, 0xA64) != 0) return;
        if (!CheckUrgentNeedsWithoutStartingTask(human)) return;
        if (task == 9)
        {
            Human_l_StartTask_WaitForOtherHuman(human);
            return;
        }
        if (task == 8) Write<std::uint32_t>(human, 0x370, 0);
        if (Read<std::uint32_t>(human, 0x378) != 0)
        {
            Write<std::uint32_t>(human, 0x368, 9);
            Write<std::uint32_t>(human, 0x36C, 0);
            return;
        }
        if (task == 1) Human_l_StartTask_Idle(human);
        else Human_l_StartTask_ExecuteJob(human);
    }

    // Partial original function: FUN_004436c5; idle, waiting/listening and goodie
    // branches retain the native preamble. Other task-specific finish routines are pending.
    void Human_CurrentTask_DoFinish(void* human)
    {
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (task != 1 && task != 9 && task != 13 && (task < 25 || task > 29)) return;
        if (task != 1)
        {
            Write<std::uint32_t>(human, 0x380, 0);
            if (!Human_IsDoingGoodie(human)) Write<std::uint32_t>(human, 0x37C, 0);
        }
        if (task >= 25 && task <= 29) Human_l_FinishTask_Goodie(human);
        else if (task == 9 && Read<std::uint32_t>(human, 0x378) != 0) Write<std::uint32_t>(human, 0x36C, 0);
        else Human_l_FinishTask_Idle(human);
    }

    // Partial original function: FUN_004445c9. The ordinary idle animation branch
    // is connected; combat, crowded/blocked positions, goods and autonomous goodie/walk
    // calls are explicit boundaries rather than invented successful operations.
    void Human_l_StartTask_Idle(void* human)
    {
        if (Read<std::uint32_t>(human, 0xACC) != 0) return; // Reservation-release/circulation branches pending here.
        if (!Human_IsJobSoldier(human) && !Human_IsJobHero(human) && Read<std::uint32_t>(human, 0xB64) != 0) return;
        void* moveable = Moveable(human);
        const auto idleCount = Read<std::uint32_t>(human, 0x37C) + 1u;
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            const void* micro = GameMap_GetConstMapMicroPointPtr(Position(human));
            if (Read<std::uint8_t>(micro, 0) == 2) return; // Defense-house/crowding/escape branches pending.
            if (Read<std::uint8_t>(human, 0xA60) == 0)
            {
                const auto occupant = Read<std::int16_t>(micro, 0x0A);
                if (occupant != Read<std::int32_t>(human, 0x0C)) return;
                if (Read<std::int32_t>(moveable, 0x24) != -1) return;
            }
        }
        if (Read<std::uint32_t>(human, 0xAD4) != 0) return;
        const auto atomic = Read<std::uint32_t>(human, 0xAB8);
        if ((atomic == 0 || atomic == 1) && idleCount >= 10u && (Read<std::uint32_t>(human, 0xB84) & 2u) == 0 && Read<std::uint32_t>(human, 0xB64) == 0 && !Human_IsAttachedToVehicle(human))
        {
            // Empty eligible queues were checked by the startup dispatcher.
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(moveable, 0x12), Read<std::int16_t>(moveable, 0x14), Read<std::int16_t>(human, 0xB30), Read<std::int16_t>(human, 0xB32));
            if (distance >= 5) return; // Tool_Map_GetNotBlockedPosition/l_StartAtomicWalk pending.
            if (Human_IsAbleToDoGoodieNow(human))
            {
                Write(human, 0x37C, idleCount);
                if (Read<std::uint8_t>(moveable, 0x10) != 0)
                {
                    if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
                    Write<std::uint32_t>(human, 0xAD0, 0);
                }
                Write<std::uint32_t>(human, 0x368, 24);
                return;
            }
        }
        Write(human, 0x37C, idleCount);
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
        }
        if (atomic == 0 || atomic == 1)
        {
            std::uint32_t action = 5;
            if (idleCount < 5u) action = Game_Random_GetNumber() % 3u + 2u;
            else if (idleCount >= 10u && !Human_IsJobSoldier(human) && !Human_IsJobHero(human)) action = idleCount < 15u ? 6u : 7u;
            Human_l_StartAtomicAnimation(human, action, -1);
        }
        Write<std::uint32_t>(human, 0x36C, 1);
    }

    // Original function: FUN_00445a6e. The idle counter is deliberately retained.
    void Human_l_FinishTask_Idle(void* human)
    {
        Human_Target_Reset(human);
        Human_Target_ResetSecondaryTarget(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 8);
    }

    // Partial original function: FUN_0046c39e. Native idle-only job cases and the
    // disabled-job branch are connected; production/learning/equipment paths are pending.
    void Human_l_StartTask_ExecuteJob(void* human)
    {
        Write<std::uint32_t>(human, 0x370, 0);
        const auto circulation = Read<std::uint32_t>(human, 0xACC);
        if (circulation == 0 && Read<std::uint32_t>(human, 0xAD4) != 0) return;
        if ((Read<std::uint32_t>(human, 0xB84) & 2u) != 0)
        {
            if (circulation != 0) Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 1);
            Human_l_StartTask_Idle(human);
            return;
        }
        // Do not bypass the defense/target or equipment/learning branches.
        if (circulation != 0 || Read<std::uint32_t>(human, 0xA68) != 0 || Read<std::uint32_t>(human, 0xAD4) != 0) return;
        const void* micro = GameMap_GetConstMapMicroPointPtr(Position(human));
        if (Read<std::uint8_t>(micro, 0) == 2) return;
        if (Human_IsAttachedToLearnHouse(human) || Human_IsAttachedToWorkHouse(human) || Read<std::uint8_t>(JobData(human), 0x83) != 0) return;
        if (!Human_l_IsAtomicInterruptable(human)) return;
        if (Read<std::uint8_t>(human, 0xA60) == 0 && ((Read<std::uint32_t>(human, 0xAF0) != 0 && Read<std::uint32_t>(human, 0xAE0) == 0) || (Read<std::uint32_t>(human, 0xAEC) != 0 && Read<std::uint32_t>(human, 0xADC) == 0))) return;
        if (Human_IsAttachedToVehicle(human)) return;
        const auto job = Read<std::uint32_t>(human, 0x18);
        switch (job)
        {
        case 1: case 2: case 3: case 4:
            Write<std::uint32_t>(human, 0x37C, 10); // Original constant 004F3C34.
            [[fallthrough]];
        case 6: case 23: case 26: case 27:
            Write<std::uint32_t>(human, 0x368, 1);
            Write<std::uint32_t>(human, 0x36C, 0);
            break;
        default: return;
        }
    }

    // Original function: FUN_00463346. Performing an atomic precedes checking
    // interruptibility and entering the task machine in the same game tick.
    void Human_AI_DoUpdateAI(void* human)
    {
        Human_CurrentTask_DoPerform(human);
        if (Human_l_IsAtomicInterruptable(human)) Human_AI_DoUpdateTaskMachine(human);
    }

    // Partial original function: FUN_00463362. Supported finish/start transitions
    // run in one tick. Pending dispatch leaves state intact and yields instead of spinning.
    void Human_AI_DoUpdateTaskMachine(void* human)
    {
        if (Read<std::uint32_t>(human, 0xACC) != 0 && Read<std::uint32_t>(human, 0xA68) != 0) return;
        void* moveable = Moveable(human);
        if (Read<std::uint8_t>(moveable, 0x10) != 0 && (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Position(human)), 0x0C) & 0x80u) != 0) return;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(human) + 0xB30), 0x0C) & 0x80u) != 0 && !Human_IsAttachedToWallPoint(human)) return;
        for (;;)
        {
            const auto task = Read<std::uint32_t>(human, 0x368);
            const auto status = Read<std::uint32_t>(human, 0x36C);
            if (status == 1)
            {
                Write<std::uint8_t>(human, 0xA60, 0);
                Write<std::uint8_t>(human, 0xA61, 0);
                Write<std::uint32_t>(human, 0x374, 2);
                return;
            }
            if (status == 0) Human_CurrentTask_DoStartUp(human);
            else if (status == 2) Human_CurrentTask_DoFinish(human);
            else return; // CurrentTask_DoFailure and unsupported statuses remain pending.
            if (Read<std::uint32_t>(human, 0x368) == task && Read<std::uint32_t>(human, 0x36C) == status) return;
        }
    }
}
