#include "C2HumanTaskAccess.h"
#include "C2AIMovementMessages.h"
#include "C2MilitaryTargetSearch.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanNeeds.h"
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
#include "C2MissionStartObjects.h"
#include "C2HumanWalking.h"
#include "C2HumanAtomicEvents.h"
#include "C2HumanGoods.h"
#include "C2HumanProduction.h"
#include "C2HumanMapCollection.h"
#include "C2HumanConstruction.h"
#include "C2HumanEquipment.h"
#include "C2OverviewObjectIterators.h"
#include "C2VehicleAccess.h"
#include "C2UserMessageAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2HumanFamily.h"
#include "C2HumanScout.h"
#include "C2HumanChest.h"
#include "C2HumanMilitaryJobs.h"
#include "C2HumanSocialJobs.h"
#include "C2HumanAnimalJobs.h"
#include "C2HumanTransportJobs.h"
#include "C2HumanTraderJobs.h"
#include "C2HumanVehicleJobs.h"
#include "C2HumanHuntingJobs.h"
#include "C2HumanAttackTasks.h"
#include "C2StaticActorTypeDataAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        void* Moveable(const void* human) { return gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u; }
        const void* Position(const void* human) { return Human_GetMapPositionRef(human); }
        const void* JobData(const void* human) { return static_cast<const std::byte*>(gStaticJobTypeData) + Read<std::uint32_t>(human, 0x18) * 0x88u; }

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

    // Partial original function: FUN_004433d1. Civilian needs, family, equipment and goodies
    // and scout/attack tasks are connected; remaining vehicle task trees are pending.
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
        if (task == 7)
        {
            Write<std::uint32_t>(human, 0x378, 0);
            switch (Read<std::uint32_t>(human, 0x370))
            {
            case 1: Human_l_StartTask_FulfillNeeds_Food(human); break;
            case 2: Human_l_StartTask_FulfillNeeds_Stamina(human); break;
            case 3: Human_l_StartTask_FulfillNeeds_Social(human); break;
            case 4: Human_l_StartTask_FulfillNeeds_Religion(human); break;
            default: break;
            }
            return;
        }
        if (task == 16) { Human_l_StartTask_Equip(human); return; }
        if (task == 14) { Human_l_StartTask_Attack(human); return; }
        if (task == 18) { Human_l_StartTask_OpenChest(human); return; }
        if (task == 19) { Human_l_StartTask_BuildGuide(human); return; }
        if (task == 20) { Human_l_StartTask_ExploreArea(human); return; }
        if (task == 21) { Human_l_StartTask_MoveInsideVehicle(human); return; }
        if (task == 5) { Human_l_StartTask_Marry(human); return; }
        if (task == 11) { Human_l_StartTask_GetChild(human, 11, 15); return; }
        if (task == 12) { Human_l_StartTask_GetChild(human, 12, 16); return; }
        if (task == 3) { Human_l_StartTask_GoHome(human); return; }
        if (task == 17) { Human_l_StartTask_Equip_DropItem(human, Read<std::uint32_t>(human, 0xAD0)); return; }
        if (task != 1 && task != 8 && task != 9) return;
        if (!Human_CanExecuteQueuedCommands(human)) return;
        if (Human_DoExecuteNextUserCommand(human))
        {
            Write<std::uint32_t>(human, 0x37C, 0);
            if (Read<std::uint32_t>(human, 0x36C) == (task == 1 ? 0u : 1u) && Read<std::uint32_t>(human, 0x368) == 1)
            {
                Human_Target_Reset(human);
                Write<std::uint32_t>(human, 0x368, task == 9 ? 9 : 8);
                Write<std::uint32_t>(human, 0x36C, 0);
            }
            return;
        }
        if (Read<std::uint32_t>(human, 0x368) != task) return;
        if (task == 8 && Read<std::uint32_t>(human, 0xA64) != 0 && TGoodTypes_IsFood(Read<std::uint32_t>(human, 0x1C))) Write(human, 0xA64, Read<std::uint32_t>(human, 0xA64) - 1u);
        else Human_AI_DoCheckForUrgentNeeds(human);
        if (Read<std::uint32_t>(human, 0x368) == 7) return;
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

    // Original function: FUN_004436c5; all 29 entries retain the native preamble.
    void Human_CurrentTask_DoFinish(void* human)
    {
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (task != 1)
        {
            Write<std::uint32_t>(human, 0x380, 0);
            if (!Human_IsDoingGoodie(human)) Write<std::uint32_t>(human, 0x37C, 0);
        }
        if (task < 1 || task > 29) return;
        if (task == 10) return; // Original table entry 00443790 is a no-op after the preamble.
        if (task == 3) { Write<std::uint32_t>(human, 0x368, 1); Write<std::uint32_t>(human, 0x36C, 0); }
        else if (task == 8) Human_l_FinishTask_ExecuteJob(human);
        else if (task == 14 || task == 15) Human_l_FinishTask_Attack(human);
        else if (task == 5) Human_l_FinishTask_Marry(human);
        // Only task 18 uses 0044621D; guide task 19 finishes through the ordinary idle entry.
        else if (task == 18) Human_l_FinishTask_BuildGuide(human);
        else if (task == 20) Human_l_FinishTask_ExploreArea(human);
        else if (task == 21) Human_l_FinishTask_MoveInsideVehicle(human);
        else if (task == 16 || task == 17) Human_l_FinishTask_Equip(human);
        else if (task == 2 || task == 23)
        {
            if (task == 23) Human_Target_Reset(human);
            Human_l_FinishTask_Goto(human);
        }
        else if (task == 4)
        {
            const void* micro = GameMap_GetConstMapMicroPointPtr(Position(human));
            if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0 && Read<std::uint32_t>(CHouseArray::mStaticVars + Read<std::uint16_t>(micro, 4) * CHouseArray::ElementSize, 0x2C) == 3) Human_HouseMisc_Enter(human);
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 1);
        }
        else if (task == 24) Write<std::uint32_t>(human, 0x36C, 0);
        else if (task == 7) Human_l_FinishTask_FulfillNeeds(human);
        else if (task >= 25 && task <= 29) Human_l_FinishTask_Goodie(human);
        else if (task == 9 && Read<std::uint32_t>(human, 0x378) != 0) Write<std::uint32_t>(human, 0x36C, 0);
        else Human_l_FinishTask_Idle(human);
    }

    // Original function: FUN_00445cb2; clear cargo only for the native failure cases.
    void Human_l_FailTask_Idle(void* human)
    {
        const auto failure = Read<std::uint32_t>(human, 0x374);
        if (failure != 7 && (failure != 11 || Read<std::uint32_t>(human, 0xAD4) == 0)) return;
        if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
        Write<std::uint32_t>(human, 0xAD4, 0);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
    }

    // Original function: FUN_00444a36.
    void Human_l_StartTask_GoAway(void* human, std::uint32_t point, std::uint32_t radius)
    {
        Human_StopCurrentGoodCirculationMode(human);
        const auto found = radius != 0 ? Human_FindNextVehicleFreeMapPoint(human, &point, radius) : Human_FindNextFreeMapPoint(human, &point);
        if (found && Human_l_StartAtomicWalkAndWait(human, &point)) Write<std::uint32_t>(human, 0x368, 23);
        else
        {
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 1);
            Human_l_StartTask_Idle(human);
        }
    }

    // Original function: FUN_00445DA5; the user message precedes the AI notification.
    void Human_l_FailTask_Goto(void* human)
    {
        switch (Read<std::uint32_t>(human, 0x374))
        {
        case 1:
            Human_UserMessage_SendToUserMessage(human, 3);
            if (Human_GetPlayerType(human) == 2) AIManager_Message_NoWayFound(gAIManager, Read<std::uint32_t>(human, 0xC), static_cast<const std::byte*>(human) + 0xA6C);
            return;
        case 3: Human_UserMessage_SendToUserMessage_CannotFindGood_Generic(human, Read<std::uint32_t>(human, 0xAD0)); return;
        case 4: Human_UserMessage_SendToUserMessage_CannotFindGood_ForBuild(human, Read<std::uint32_t>(human, 0xAD0)); return;
        case 5: Human_UserMessage_SendToUserMessage_CannotFindGood_ForHome(human, Read<std::uint32_t>(human, 0xAD0)); return;
        case 6: Human_UserMessage_SendToUserMessage(human, 7); return;
        case 7: Human_UserMessage_SendToUserMessage(human, 8); return;
        default: Human_UserMessage_SendToUserMessage(human, 2); return;
        }
    }

    // Original function: FUN_00445acf; finite-production rotation precedes threshold messages.
    void Human_l_FailTask_ExecuteJob(void* human)
    {
        const auto job = Read<std::int32_t>(human, 0x18);
        if (Read<std::uint32_t>(human, 0x380) >= 2 && job >= 8 && job <= 30 && Read<std::uint32_t>(human, 0x1C) != 0)
        {
            std::uint32_t productionCount = 0;
            for (std::uint32_t good = 0; good < 66; ++good) { const auto count = Read<std::int32_t>(human, 0x21Cu + good * 4u); if (count > 0 && count <= 11) ++productionCount; }
            if (productionCount >= 2) { Write<std::uint32_t>(human, 0x380, 0); Human_Production_CouldNotProduceCurrentGood(human); }
        }
        if (Read<std::uint32_t>(human, 0x380) >= 5)
        {
            Write<std::uint32_t>(human, 0x380, 0);
            switch (Read<std::uint32_t>(human, 0x374))
            {
            case 0: if (!Human_IsAttachedToLearnHouse(human)) Human_UserMessage_SendToUserMessage(human, 2); break;
            case 1: Human_UserMessage_SendToUserMessage(human, 3); break;
            case 2:
                if (Human_IsJobBuilder(human))
                {
                    std::uint32_t real = 0, work = 0;
                    if (Human_FindDamagedWallNearWorkCenter(human, &real, &work))
                    {
                        Write(human, 0xB30, real);
                        Write<std::uint32_t>(human, 0x38C, 0);
                        Human_WorkCenterMarker_Update(human);
                        Human_Target_SetTargetPositions(human, &real, &work);
                        return;
                    }
                }
                if (job >= 8 && job <= 30)
                {
                    const auto id = Read<std::uint32_t>(human, 0xB34);
                    if (id == 0xFFFFFFFFu || Read<std::uint32_t>(CHouseArray::mStaticVars + id * CHouseArray::ElementSize, 0x2C) != 2) Human_UserMessage_SendToUserMessage(human, 11);
                }
                break;
            case 3: Human_UserMessage_SendToUserMessage_CannotFindGood_Generic(human, Read<std::uint32_t>(human, 0xAD0)); break;
            case 4: Human_UserMessage_SendToUserMessage_CannotFindGood_ForBuild(human, Read<std::uint32_t>(human, 0xAD0)); break;
            case 5: Human_UserMessage_SendToUserMessage_CannotFindGood_ForHome(human, Read<std::uint32_t>(human, 0xAD0)); break;
            case 6: Human_UserMessage_SendToUserMessage(human, 7); break;
            case 7: Human_UserMessage_SendToUserMessage(human, 8); break;
            case 8: Human_UserMessage_SendToUserMessage(human, 9); break;
            case 9: Human_UserMessage_SendToUserMessage(human, 10); break;
            case 10: break; // Windows 0045BFEC returns without emitting a message.
            case 11:
            {
                const auto good = Read<std::uint32_t>(human, 0x1C);
                if (Read<std::uint32_t>(gGameControl, 0x5D200) == 2 && (good == 16 || good == 17 || good == 19 || good == 20 || good == 21 || good == 23)) break;
                // Original message function: FUN_0045bfef.
                std::byte message[0x304];
                ToUserMessage_Construct(message);
                Write<std::uint32_t>(message, 0, 13);
                Write(message, 0x10, Read<std::uint32_t>(human, 4));
                Write(message, 8, Read<std::uint32_t>(human, 0x10));
                Write(message, 0x30, good);
                ToUserMessage_FinishMessage(message);
                NC2LogicExternals::External_Misc_SendToUserMessage(message);
                break;
            }
            case 13:
                Human_UserMessage_SendToUserMessage(human, 14);
                if (Human_IsAttachedToLearnHouse(human)) House_Inhabitants_DetachHuman(CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize, Read<std::uint32_t>(human, 4), 8);
                break;
            case 14: Human_UserMessage_SendToUserMessage(human, 15); break;
            case 15: Human_UserMessage_SendToUserMessage(human, 16); break;
            default: Human_UserMessage_SendToUserMessage(human, 2); break;
            }
        }
        Write<std::uint32_t>(human, 0xAD0, 0);
    }

    // Original function: FUN_00443807; AI pursuit retries and failure notifications retain native order.
    void Human_CurrentTask_DoFailure(void* human)
    {
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
        Write(human, 0x380, Read<std::uint32_t>(human, 0x380) + 1u);
        bool doJob = true, checkUrgent = true;
        if (task == 8) { doJob = false; Human_l_FailTask_ExecuteJob(human); }
        else if (task == 14) Human_l_FailTask_Attack(human);
        else if (task == 15)
        {
            auto* target = static_cast<std::byte*>(human) + 0xB64;
            if (Read<std::uint32_t>(human, 0x370) != 0 || (!TargetData_IsValid(target) && !TargetData_IsValid(static_cast<std::byte*>(human) + 0xB70))) Human_l_FailTask_Attack(human);
            else
            {
                const auto type = TargetData_GetTargetType(target);
                if (type == 6)
                {
                    const auto& weapon = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)])[Read<std::uint32_t>(human, 0xAE4)];
                    if (weapon.maximumRange == 1 && TargetData_MapPos_NextAttackPoint(target))
                    {
                        std::uint32_t point = 0;
                        TargetData_MapPos_GetAttackPosition(target, &point);
                        if (Human_l_StartAtomicWalk(human, &point, &point)) { Write<std::uint32_t>(human, 0x368, 15); Write<std::uint32_t>(human, 0x36C, 1); return; }
                    }
                }
                else if (Human_GetPlayerType(human) == 2)
                {
                    Write<std::uint32_t>(human, 0xB70, 0);
                    std::uint32_t point = 0;
                    if (Tool_GetOneOfNearestEnemyLandscapes(Read<std::uint32_t>(human, 0x10), Human_GetMapPositionRef(human), 0, 18, true, 1, 0, &point))
                    {
                        Write(target, 4, point);
                        Write<std::uint32_t>(target, 0, 6);
                        Write<std::uint32_t>(human, 0x368, 14);
                        Write<std::uint32_t>(human, 0x36C, 0);
                        return;
                    }
                    if (TargetData_IsValid(target))
                    {
                        TargetData_GetMapPosition(target, &point);
                        AIManager_Message_NoWayFoundToEnemy(gAIManager, Read<std::uint32_t>(human, 0xC), &point);
                    }
                }
                Human_UserMessage_SendToUserMessage(human, 3);
                Write<std::uint32_t>(human, 0xB64, 0);
                Write<std::uint32_t>(human, 0xB70, 0);
                doJob = false;
            }
        }
        else if (task == 7)
        {
            const auto need = Read<std::uint32_t>(human, 0x370);
            if (need >= 2 && need <= 4) doJob = false;
            else if (need == 1) { doJob = TGoodTypes_IsFood(Read<std::uint32_t>(human, 0x1C)); Write<std::uint32_t>(human, 0xA64, 10); }
            checkUrgent = false;
            Human_l_FailTask_FulfillNeeds(human);
        }
        else if (task == 2 || task == 4)
        {
            doJob = false;
            if (Read<std::uint32_t>(human, 0x370) != 0)
            {
                Human_l_FailTask_FulfillNeeds(human);
                const auto need = Read<std::uint32_t>(human, 0x370);
                if (need != 0)
                {
                    Write<std::uint32_t>(human, 0x368, 7);
                    Write<std::uint32_t>(human, 0x36C, 0);
                    if (need == 1) Human_l_StartTask_FulfillNeeds_Food(human);
                    else if (need == 2) Human_l_StartTask_FulfillNeeds_Stamina(human);
                    else if (need == 3) Human_l_StartTask_FulfillNeeds_Social_Monologuize(human);
                    else if (need == 4) Human_l_StartTask_FulfillNeeds_Religion(human);
                    return;
                }
            }
            else Human_l_FailTask_Goto(human);
        }
        else if (task == 1) Human_l_FailTask_Idle(human);
        else if (task == 3) Human_l_FailTask_GoHome(human);
        else if (task == 5) Human_l_FailTask_Marry(human);
        else if (task == 11) Human_l_FailTask_GetBoy(human);
        else if (task == 12) Human_l_FailTask_GetGirl(human);
        else if (task == 22) Human_l_FailTask_Misc(human);
        else if (task == 16) Human_l_FailTask_Equip(human);
        else if (task == 17) Human_l_FailTask_Drop(human);
        else if (task == 18) { Human_Target_Reset(human); Write<std::uint32_t>(human, 0x368, 1); Write<std::uint32_t>(human, 0x36C, 0); }
        else if (task == 19) Human_UserMessage_SendToUserMessage(human, 2);
        else if (task == 20) Human_l_FailTask_ExploreArea(human);
        else if (task == 21) { Write<std::uint32_t>(human, 0x368, 1); Write<std::uint32_t>(human, 0x36C, 0); }
        else if (task == 23)
        {
            Human_l_FailTask_Goto(human);
            doJob = false;
            if (Read<std::uint32_t>(human, 0x370) != 0) Human_l_FailTask_FulfillNeeds(human);
        }
        else if (task >= 24 && task <= 29) Human_l_FailTask_Goodie(human);
        if (Human_IsJobSoldier(human))
        {
            if ((task == 2 || task == 4) && Read<std::uint32_t>(human, 0x374) == 1)
            {
                if (!Human_IsAttachedToWorkHouse(human))
                {
                    Write(human, 0xB30, Read<std::uint32_t>(Position(human), 0));
                    Write<std::uint32_t>(human, 0x38C, 0);
                    Human_WorkCenterMarker_Update(human);
                }
                else if (Human_GetPlayerType(human) == 2) Human_DoAddNewUserCommand(human, 29, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), Read<std::uint32_t>(human, 0xB34), Read<std::uint32_t>(human, 0xB38));
            }
            if (Read<std::uint32_t>(human, 0x370) != 0 && Human_IsRefreshingInWorld(human)) doJob = false;
        }
        if (Read<std::int32_t>(human, 0x380) > 10) { Write<std::uint32_t>(human, 0x380, 0); doJob = false; }
        Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0xAD0, 0);
        Write<std::uint32_t>(human, 0x374, 2);
        if (doJob)
        {
            Write<std::uint32_t>(human, 0x368, 8);
            Write<std::uint32_t>(human, 0x36C, 0);
            if (checkUrgent) Human_AI_DoCheckForUrgentNeeds(human);
            if (Read<std::uint32_t>(human, 0x368) == 7) return;
            Write<std::uint32_t>(human, 0x370, 0);
            if (Read<std::uint32_t>(human, 0x378) == 0) { Human_l_StartTask_ExecuteJob(human); return; }
        }
        if (Read<std::uint32_t>(human, 0x378) != 0)
        {
            Write<std::uint32_t>(human, 0x368, 9);
            Write<std::uint32_t>(human, 0x36C, 0);
            Human_l_StartTask_WaitForOtherHuman(human);
        }
        else
        {
            Write<std::uint32_t>(human, 0x368, 1);
            Write<std::uint32_t>(human, 0x36C, 0);
            Human_l_StartTask_Idle(human);
        }
    }

    // Original function: FUN_004445c9; nearby valid opponents transfer to task 14.
    void Human_l_StartTask_Idle(void* human)
    {
        if (!Human_IsJobSoldier(human) && !Human_IsJobHero(human) && TargetData_IsValid(static_cast<const std::byte*>(human) + 0xB64))
        {
            std::uint32_t point = 0;
            TargetData_GetMapPosition(static_cast<const std::byte*>(human) + 0xB64, &point);
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(Position(human), 0), Read<std::int16_t>(Position(human), 2));
            if (distance < 50) { Write<std::uint32_t>(human, 0x368, 14); return; } // Game.exe 004F4300.
            Write<std::uint32_t>(human, 0xB64, 0);
        }
        void* moveable = Moveable(human);
        const auto idleCount = Read<std::uint32_t>(human, 0x37C) + 1u;
        Write(human, 0x37C, idleCount);
        if (Read<std::uint8_t>(moveable, 0x10) != 0)
        {
            if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
            const void* micro = GameMap_GetConstMapMicroPointPtr(Position(human));
            const auto soldierAtWork = Human_IsJobSoldier(human) && Human_IsAttachedToWorkHouse(human) && Human_IsInWorkHouse(human);
            if (Read<std::uint8_t>(human, 0xA60) == 0 && !soldierAtWork)
            {
                if (Read<std::uint8_t>(micro, 0) == 2 && Read<std::uint8_t>(CHouseArray::mStaticVars + Read<std::uint16_t>(micro, 4) * CHouseArray::ElementSize, 0x7D8) != 0)
                {
                    if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
                    Human_l_StartAtomicAnimation(human, 5, -1);
                    Write<std::uint32_t>(human, 0x36C, 1);
                    return;
                }
                const auto occupant = Read<std::int16_t>(micro, 0x0A);
                if (occupant != Read<std::int32_t>(human, 0x0C) || Read<std::int32_t>(moveable, 0x24) != -1)
                {
                    CMapMoveableIterator iterator;
                    for (auto id = iterator.GetFirstMapMoveableId(Position(human)); id != -1; id = iterator.GetNextMapMoveableId())
                    {
                        if (id == Read<std::int32_t>(human, 0x0C)) continue;
                        const auto* other = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                        if (Read<std::uint32_t>(other, 8) == 1 && Read<std::uint32_t>(CHumanArray::mStaticVars + Read<std::uint32_t>(other, 0x54) * CHumanArray::ElementSize, 0x368) == 23) continue;
                        Human_DoAddNewUserCommand(human, 5, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), 0, 0, 0, 0);
                        Human_DoExecuteNextUserCommand(human);
                        Write<std::uint8_t>(human, 0xA60, 1);
                        if (Read<std::uint32_t>(human, 0x368) != 1) return;
                    }
                }
            }
            if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
            if (idleCount > 2u && Read<std::uint8_t>(micro, 0) == 2 && !soldierAtWork) { Human_l_StartTask_GoAway(human, 0, 0); return; }
        }
        if (Read<std::uint32_t>(human, 0xAD4) != 0 && Read<std::uint8_t>(human, 0xB51) != 0)
        {
            auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
            const auto good = Read<std::uint32_t>(human, 0xAD4);
            if (Vehicle_Stock_CanStoreGoodType(vehicle, good) && !Vehicle_Stock_IsFullSoon(vehicle))
            {
                if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
                Vehicle_Stock_ModifyFutureAmount(vehicle, good, 1, Read<std::uint32_t>(human, 8));
                Vehicle_Stock_ModifyAmount(vehicle, Read<std::uint32_t>(human, 0xAD4), 1, Read<std::uint32_t>(human, 8));
                Write<std::uint32_t>(human, 0xAD4, 0);
                CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
            }
        }
        if (Read<std::uint32_t>(human, 0xAD4) != 0)
        {
            Write<std::uint32_t>(human, 0xAD4, 0);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0);
        }
        const auto atomic = Read<std::uint32_t>(human, 0xAB8);
        if ((atomic == 0 || atomic == 1) && idleCount >= 10u && (Read<std::uint32_t>(human, 0xB84) & 2u) == 0 && Read<std::uint32_t>(human, 0xB64) == 0 && !Human_IsAttachedToVehicle(human))
        {
            // Empty eligible queues were checked by the startup dispatcher.
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(Position(human), 0), Read<std::int16_t>(Position(human), 2), Read<std::int16_t>(human, 0xB30), Read<std::int16_t>(human, 0xB32));
            if (distance >= 5)
            {
                auto point = Read<std::uint32_t>(human, 0xB30);
                if (Tool_Map_GetNotBlockedPosition(&point, true) && Human_l_StartAtomicWalk(human, &point, &point)) { Write<std::uint32_t>(human, 0x37C, 0); return; }
            }
            else if (Human_IsAbleToDoGoodieNow(human))
            {
                Write<std::uint32_t>(human, 0x368, 24);
                return;
            }
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

    // Original function: FUN_0046f76b.
    void Human_l_FinishTask_ExecuteJob(void* human)
    {
        Human_UpdateCurrentJobType(human);
        Human_WorkCenterMarker_Update(human);
        const auto job = Read<std::uint32_t>(human, 0x18);
        if (job != 7 && job != 8) Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
    }

    // Partial original function: FUN_0046c39e; household, learning, indoor production, collection, farming, fishing, social jobs, builders, ground military and idle jobs.
    // Hunter work is connected; the remaining vehicle-dependent dispatch is pending.
    void Human_l_StartTask_ExecuteJob(void* human)
    {
        Write<std::uint32_t>(human, 0x370, 0);
        const auto circulation = Read<std::uint32_t>(human, 0xACC);
        if (circulation == 0 && Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        if ((Read<std::uint32_t>(human, 0xB84) & 2u) != 0)
        {
            if (circulation != 0) Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 1);
            Human_l_StartTask_Idle(human);
            return;
        }
        if (Human_IsDefendingHouse(human, -1))
        {
            if (Read<std::uint32_t>(human, 0xACC) != 0) Human_StopCurrentGoodCirculationMode(human);
            if (Read<std::uint32_t>(human, 0xAD4) != 0) Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human);
            else { Write<std::uint32_t>(human, 0x36C, 0); Write<std::uint32_t>(human, 0x368, 1); Human_l_StartTask_Idle(human); }
            return;
        }
        if (Human_IsAttachedToLearnHouse(human)) { Human_l_StartTask_ExerciseJob(human); return; }
        const auto attached = Human_IsAttachedToWorkHouse(human);
        if ((Read<std::uint8_t>(JobData(human), 0x83) != 0 && !attached) || (attached && Read<std::uint8_t>(JobData(human), 0x84) != 0 && Read<std::uint32_t>(CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize, 0x2C) != 3))
        {
            Write<std::uint32_t>(human, 0x36C, 3);
            Write<std::uint32_t>(human, 0x374, 2);
            return;
        }
        if (!Human_l_IsAtomicInterruptable(human)) return;
        if (Read<std::uint32_t>(human, 0xACC) == 0 && Read<std::uint8_t>(human, 0xA60) == 0)
        {
            const auto tool = Read<std::uint32_t>(human, 0xAF0), shoes = Read<std::uint32_t>(human, 0xAEC);
            if (tool != 0 && Read<std::uint32_t>(human, 0xAE0) == 0) Human_DoAddNewUserCommand(human, 80, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), 0, 0, tool, 0xFFFFFFFFu);
            else if (shoes != 0 && Read<std::uint32_t>(human, 0xADC) == 0) Human_DoAddNewUserCommand(human, 79, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), 0, 0, shoes, 0xFFFFFFFFu);
            if ((tool != 0 && Read<std::uint32_t>(human, 0xAE0) == 0) || (shoes != 0 && Read<std::uint32_t>(human, 0xADC) == 0))
            {
                Human_DoExecuteNextUserCommand(human);
                Write<std::uint8_t>(human, 0xA60, 1);
            }
            if (Read<std::uint32_t>(human, 0x368) != 8) return;
        }
        if (Read<std::uint32_t>(human, 0xA68) != 0)
        {
            if (!Human_Target_IsValid(human)) { Human_StopCurrentGoodCirculationMode(human); Human_Target_Reset(human); }
            else if (!Human_IsAtTargetPoint(human))
            {
                const auto target = Read<std::uint32_t>(human, 0xA68);
                if (target == 2)
                {
                    Write(human, 0xAD0, Read<std::uint32_t>(human, 0xA78));
                    if (Human_l_StartAtomicWalkAndFetch(human, static_cast<std::byte*>(human) + 0xA6C, static_cast<std::byte*>(human) + 0xA70, false)) return;
                }
                else if (Human_l_StartAtomicWalk(human, static_cast<std::byte*>(human) + 0xA6C, static_cast<std::byte*>(human) + 0xA70)) { Write(human, 0xA68, target); return; }
                Human_Target_Reset(human);
            }
        }
        if (Human_IsAttachedToVehicle(human))
        {
            const auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
            const auto vehicleJob = Read<std::uint32_t>(vehicle, 0x1EC);
            if (!(Human_IsJobCarrier(human) && (vehicleJob == 50 || vehicleJob == 51)) && !Human_IsJobTrader(human))
            {
                Write<std::uint32_t>(human, 0x368, Read<std::uint8_t>(human, 0xB51) != 0 ? 1u : 21u);
                Write<std::uint32_t>(human, 0x36C, 0);
                return;
            }
        }
        const auto job = Read<std::uint32_t>(human, 0x18);
        switch (job)
        {
        case 5: Human_l_StartTask_ExecuteJob_Woman(human); return;
        case 7: Human_l_StartTask_ExecuteJob_Builder(human); return;
        case 8: Human_l_StartTask_ExecuteJob_Collector(human); return;
        case 9: case 10: Human_l_StartTask_ExecuteJob_Carpenter(human); return;
        case 15: Human_l_StartTask_ExecuteJob_Hunter(human); return;
        case 16: Human_l_StartTask_ExecuteJob_Breeder(human); return;
        case 18: Human_l_StartTask_ExecuteJob_Farmer(human); return;
        case 22: Human_l_StartTask_ExecuteJob_Fisher(human); return;
        case 24: Human_l_StartTask_ExecuteJob_Carrier(human); return;
        case 25:
            Human_l_StartTask_ExecuteJob_Trader(human);
            if (Human_IsAttachedToVehicle(human) && Read<std::uint32_t>(human, 0x36C) == 3 && Read<std::uint32_t>(human, 0x374) == 2)
            {
                Write<std::uint32_t>(human, 0x368, Read<std::uint8_t>(human, 0xB51) != 0 ? 1u : 21u);
                Write<std::uint32_t>(human, 0x36C, 0);
            }
            return;
        case 28: Human_l_StartTask_ExecuteJob_Jester(human); return;
        case 29: Human_l_StartTask_ExecuteJob_HerbGuy(human); return;
        case 30: Human_l_StartTask_ExecuteJob_Druid(human); return;
        case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 40: case 41: case 42: case 43: case 44: case 45: case 46: case 47:
            Human_l_StartTask_ExecuteJob_Soldier(human);
            return;
        case 11: case 12: case 13: case 14: case 17: case 19: case 20: case 21:
            Human_l_StartTask_ExecuteJob_Tool_GoodProducer(human);
            return;
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
        if (Read<std::uint32_t>(human, 0xACC) != 0 && Read<std::uint32_t>(human, 0xA68) != 0 && !Human_Target_IsValid(human))
        {
            const auto target = Read<std::uint32_t>(human, 0xA68);
            if (target == 2 && Read<std::uint32_t>(human, 0xAD0) == 0) Write(human, 0xAD0, gStaticDependencyGoodByLandscape[Human_GetLandscapeTypeAtTargetPosition(human)]);
            Write<std::uint32_t>(human, 0x36C, 3);
            Write<std::uint32_t>(human, 0x374, target == 1 ? 1 : target == 2 ? 3 : target == 3 || target == 4 ? 7 : target == 7 ? 6 : 0);
        }
        void* moveable = Moveable(human);
        if (Read<std::uint8_t>(moveable, 0x10) != 0 && (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Position(human)), 0x0C) & 0x80u) != 0)
        {
            Human_CurrentTask_DoInterrupt(human);
            Human_l_StartTask_GoAway(human, 0, 0);
        }
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(human) + 0xB30), 0x0C) & 0x80u) != 0 && !Human_IsAttachedToWallPoint(human))
        {
            auto workCenter = Read<std::uint32_t>(human, 0xB30);
            if (Tool_Map_GetNotBlockedPosition(&workCenter, false))
            {
                Write(human, 0xB30, workCenter);
                Write<std::uint32_t>(human, 0x38C, 0);
                Human_WorkCenterMarker_Update(human);
            }
        }
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
            else if (status == 3) Human_CurrentTask_DoFailure(human);
            else return;
            if (Read<std::uint32_t>(human, 0x368) == task && Read<std::uint32_t>(human, 0x36C) == status) return;
        }
    }
}
