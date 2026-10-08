#include "C2HumanConstruction.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2WallRoadConstruction.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* WorkCenter(void* human) { return static_cast<std::byte*>(human) + 0xB30; }
        void Fail(void* human, std::uint32_t reason) { Write<std::uint32_t>(human, 0x36C, 3); Write(human, 0x374, reason); }
        void StartBuild(void* human, std::uint32_t action) { Human_l_StartAtomicAnimation(human, action, static_cast<std::int32_t>(Human_Target_GetDirectionTo(human))); Write<std::uint32_t>(human, 0x36C, 1); }
    }

    // Partial original function: FUN_0046c822; the house-building branch 00473FFF remains pending.
    void Human_l_StartTask_ExecuteJob_Builder(void* human)
    {
        if (Human_IsAttachedToWorkHouse(human)) return;
        if (Human_IsAttachedToRoadPoint(human)) { Human_l_StartTask_ExecuteJob_Tool_RoadBuilder(human); return; }
        if (Human_IsAttachedToWallPoint(human)) { Human_l_StartTask_ExecuteJob_Tool_WallBuilder(human); return; }
        Human_WorkCenterMarker_Remove(human);
        Fail(human, 2);
    }

    // Original function: FUN_004740d4.
    void Human_l_StartTask_ExecuteJob_Tool_RoadBuilder(void* human)
    {
        if (!Human_IsAttachedToRoadPoint(human)) { Fail(human, 2); return; }
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(WorkCenter(human)), 0) == 2) { RoadBuildManager_ClearRoadBuildFlagForAnyPlayer(gRoadBuildManager, WorkCenter(human)); return; }
        if (Read<std::uint32_t>(human, 0xACC) == 0) Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_TryToCollect(human);
        else if (Read<std::uint32_t>(human, 0xAD4) != 0) Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_BringAway(human);
        else Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_Collect(human);
    }

    // Original function: FUN_00474143.
    void Human_l_StartTask_ExecuteJob_Tool_WallBuilder(void* human)
    {
        if (!Human_IsAttachedToWallPoint(human)) { Fail(human, 2); return; }
        const auto* micro = GameMap_GetConstMapMicroPointPtr(WorkCenter(human));
        if (Read<std::uint8_t>(micro, 0) == 2) { WallBuildManager_ClearWallBuildFlagForAnyPlayer(gWallBuildManager, WorkCenter(human)); return; }
        if (Read<std::uint32_t>(human, 0xACC) != 0)
        {
            if (Read<std::uint32_t>(human, 0xAD4) != 0) Human_l_StartTask_ExecuteJob_Tool_WallBuilder_BringAway(human);
            else Human_l_StartTask_ExecuteJob_Tool_WallBuilder_Collect(human);
            return;
        }
        if ((Read<std::uint8_t>(micro, 13) & 8u) != 0) { Human_l_StartTask_ExecuteJob_Tool_WallBuilder_TryToCollect(human); return; }
        auto work = Read<std::uint32_t>(human, 0xB30);
        if (!GameMap_Misc_GetLandscapeWorkPosition(WorkCenter(human), &work)) { Fail(human, 7); return; }
        Human_l_StartTask_ExecuteJob_Tool_WallBuilder_Repair(human);
    }

    // Original function: FUN_00474516.
    void Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_BringAway(void* human)
    {
        if (Human_Target_HasTarget(human) && Read<std::uint32_t>(human, 0xA6C) != Read<std::uint32_t>(human, 0xB30)) { Human_Target_Reset(human); Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_BringAway(human); return; }
        if (Human_IsAtTargetPoint(human)) { StartBuild(human, 41); return; }
        if (Human_IsAtWorkCenter(human)) { Human_Target_SetTargetPositions(human, WorkCenter(human), WorkCenter(human)); StartBuild(human, 41); return; }
        if (!Human_l_StartAtomicWalk(human, WorkCenter(human), WorkCenter(human))) Fail(human, 1);
    }

    // Original function: FUN_004745ae.
    void Human_l_StartTask_ExecuteJob_Tool_WallBuilder_BringAway(void* human)
    {
        if (Human_Target_HasTarget(human) && Read<std::uint32_t>(human, 0xA6C) != Read<std::uint32_t>(human, 0xB30)) { Human_Target_Reset(human); Human_l_StartTask_ExecuteJob_Tool_WallBuilder_BringAway(human); return; }
        if (Human_IsAtTargetPoint(human)) { StartBuild(human, 42); return; }
        if (Human_IsAtWorkCenter(human)) { Human_Target_SetTargetPositions(human, WorkCenter(human), WorkCenter(human)); StartBuild(human, 42); return; }
        if (!Human_l_StartAtomicWalk(human, WorkCenter(human), WorkCenter(human))) Fail(human, 1);
    }

    // Original function: FUN_00474646.
    void Human_l_StartTask_ExecuteJob_Tool_WallBuilder_Repair(void* human)
    {
        if (Human_Target_HasTarget(human) && Read<std::uint32_t>(human, 0xA6C) != Read<std::uint32_t>(human, 0xB30)) { Human_Target_Reset(human); Human_l_StartTask_ExecuteJob_Tool_WallBuilder_BringAway(human); return; }
        if (Human_IsAtTargetPoint(human)) { StartBuild(human, 42); return; }
        auto work = Read<std::uint32_t>(human, 0xB30);
        if (!GameMap_Misc_GetLandscapeWorkPosition(WorkCenter(human), &work)) { Fail(human, 7); return; }
        Human_Target_SetTargetPositions(human, WorkCenter(human), &work);
        if (Human_IsAtTargetPoint(human)) StartBuild(human, 42);
        else if (!Human_l_StartAtomicWalk(human, WorkCenter(human), &work)) Fail(human, 1);
    }

    // Original function: FUN_004747b4.
    void Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_TryToCollect(void* human)
    {
        Human_Target_Reset(human);
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        Write<std::uint32_t>(human, 0xAD0, 3);
        std::uint32_t real = 0, work = 0;
        if (Human_FindGood_Complex(human, 3, &real, &work, WorkCenter(human)) && Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return;
        Fail(human, 4);
    }

    // Original function: FUN_00474833.
    void Human_l_StartTask_ExecuteJob_Tool_WallBuilder_TryToCollect(void* human)
    {
        Human_Target_Reset(human);
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        Write<std::uint32_t>(human, 0xAD0, 5);
        std::uint32_t real = 0, work = 0;
        if (Human_FindGood_Complex(human, 5, &real, &work, WorkCenter(human)) && Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return;
        Fail(human, 4);
    }

    // Original function: FUN_0047496e.
    void Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_Collect(void* human)
    {
        if (Human_IsAtTargetPoint(human))
        {
            if (Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(human)) return;
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            return;
        }
        Human_StopCurrentGoodCirculationMode(human);
        Human_l_StartTask_ExecuteJob_Tool_RoadBuilder_TryToCollect(human);
    }

    // Original function: FUN_004749a3.
    void Human_l_StartTask_ExecuteJob_Tool_WallBuilder_Collect(void* human)
    {
        if (Human_IsAtTargetPoint(human))
        {
            if (Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(human)) return;
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            return;
        }
        Human_StopCurrentGoodCirculationMode(human);
        Human_l_StartTask_ExecuteJob_Tool_WallBuilder_TryToCollect(human);
    }
}
