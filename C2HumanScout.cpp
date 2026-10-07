#include "C2HumanScout.h"
#include "C2GuideAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanTaskAccess.h"
#include "C2HumanWalking.h"
#include "C2HumanNeeds.h"
#include "C2VehicleAccess.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapPathfinderSearch.h"
#include "C2MissionStartObjects.h"
#include "C2UserMessageAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        const void* RealTarget(const void* human) { return static_cast<const std::byte*>(human) + 0xA6C; }
        const void* WorkTarget(const void* human) { return static_cast<const std::byte*>(human) + 0xA70; }
        const void* WorkCenter(const void* human) { return static_cast<const std::byte*>(human) + 0xB30; }
        void Queue(void* human, std::uint8_t opcode, std::uint32_t parameter, std::uint32_t revision) { Human_DoAddNewUserCommand(human, opcode, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), revision, parameter, 0); }
        struct SNotExploredPointSearch { std::uint32_t player; std::uint32_t count; std::uint32_t points[8]; };
        static_assert(sizeof(SNotExploredPointSearch) == 0x28);

        // Original function: FUN_0045d5d1; semantic name inferred from Windows.
        bool __cdecl SearchNotExploredPoint(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SNotExploredPointSearch*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 0x0C) & 0x80u) != 0 || data.player >= 16 || (Read<std::uint16_t>(micro, 8) & (1u << data.player)) != 0) return false;
            data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 8;
        }
    }

    // Original function: FUN_0043d3fe.
    std::uint32_t Human_DoExecuteUserCommand_BuildGuide(void* human, const void* point)
    {
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        if (!Human_IsJobScout(human)) return 1;
        Write<std::uint8_t>(human, 0x4F8, 0);
        Human_Target_Reset(human);
        Write(human, 0xB30, Read<std::uint32_t>(point, 0));
        Write<std::uint32_t>(human, 0x38C, 0);
        Human_WorkCenterMarker_Update(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 19);
        return 2;
    }

    // Original function: FUN_0043d45d.
    std::uint32_t Human_DoExecuteUserCommand_ExploreArea(void* human, const void* point)
    {
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        if (!Human_IsJobScout(human)) return 1;
        if (MapMigPoint_IsInsideMap(point))
        {
            Write(human, 0xB30, Read<std::uint32_t>(point, 0));
            Write<std::uint32_t>(human, 0x38C, 0);
            Human_WorkCenterMarker_Update(human);
            Human_Target_SetTargetPositions(human, point, point);
        }
        else if (Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) != Read<std::uint32_t>(WorkCenter(human), 0)) Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 20);
        return 2;
    }

    // Original function: FUN_00445a90.
    void Human_l_FinishTask_Misc(void* human)
    {
        Write<std::uint32_t>(human, 0xAD0, 0);
        Write<std::uint32_t>(human, 0xACC, 0);
        Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 8);
        if (InteractionTargetData_IsValid(static_cast<const std::byte*>(human) + 0xA84)) Human_Target_RestoreSecondaryTarget(human);
    }

    // Original function: FUN_0044621d; the queried command is opcode 18.
    void Human_l_FinishTask_BuildGuide(void* human)
    {
        if (!Human_UserControl_IsCommandTypeInQueue(human, 18)) { Human_l_FinishTask_Misc(human); return; }
        Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 8);
    }

    // Original function: FUN_0044624c.
    void Human_l_StartTask_BuildGuide(void* human)
    {
        Human_Target_SetTargetPositions(human, WorkCenter(human), WorkCenter(human));
        if (Human_IsAtTargetPoint(human))
        {
            Human_l_StartAtomicAnimation(human, 43, -1);
            Write<std::uint32_t>(human, 0x36C, 1);
            return;
        }
        if (Human_l_StartAtomicWalk(human, RealTarget(human), WorkTarget(human))) { Queue(human, 19, MapMigPoint_GetLong(WorkCenter(human)), Read<std::uint32_t>(human, 8)); return; }
        Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 7);
    }

    // Original function: FUN_004462e1.
    void Human_l_StartTask_ExploreArea(void* human)
    {
        if (Human_Target_IsValid(human))
        {
            if (Human_IsAtTargetPoint(human)) { Write<std::uint32_t>(human, 0x36C, 2); return; }
            if (Human_l_StartAtomicWalk(human, RealTarget(human), WorkTarget(human))) { Queue(human, 20, MapMigPoint_GetLong(WorkTarget(human)), Read<std::uint32_t>(human, 8)); return; }
        }
        else
        {
            std::uint32_t point = 0;
            if (!Human_FindNotExploredPointNearWorkCenter(human, &point)) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 2); return; }
            if (Human_l_StartAtomicWalk(human, &point, &point)) { Queue(human, 20, 0xFFFFFFFFu, Read<std::uint32_t>(human, 8)); return; }
        }
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 1);
    }

    // Original function: FUN_004463c7.
    void Human_l_FailTask_ExploreArea(void* human)
    {
        switch (Read<std::uint32_t>(human, 0x374))
        {
        case 1: Human_UserMessage_SendToUserMessage(human, 3); return;
        case 7: Human_UserMessage_SendToUserMessage(human, 8); return;
        case 2:
        {
            const auto houseId = Read<std::int32_t>(human, 0xB34);
            if (houseId == -1 || Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::intptr_t>(houseId) * CHouseArray::ElementSize, 0x2C) != 2) Human_UserMessage_SendToUserMessage(human, 11);
            if (MapMigPoint_IsInsideMap(WorkCenter(human)) && Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) != Read<std::uint32_t>(WorkCenter(human), 0)) Queue(human, 4, MapMigPoint_GetLong(WorkCenter(human)), 0);
            return;
        }
        default: Human_UserMessage_SendToUserMessage(human, 2); return;
        }
    }

    // Original function: FUN_00446468; 00445a6e shares the existing idle finish body.
    void Human_l_FinishTask_ExploreArea(void* human)
    {
        if (Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) != Read<std::uint32_t>(WorkCenter(human), 0)) Queue(human, 20, 0xFFFFFFFFu, Read<std::uint32_t>(human, 8));
        Human_l_FinishTask_Idle(human);
    }

    // Original function: FUN_0045d56f; Windows constant 004F4308 is 40.
    // No separate Mac identity was established for this Windows search function.
    bool Human_FindNotExploredPointNearWorkCenter(const void* human, void* point)
    {
        SNotExploredPointSearch data{};
        data.player = Read<std::uint32_t>(human, 0x10);
        CMapPathfinderElement::SearchOverMap(WorkCenter(human), 40, true, SearchNotExploredPoint, &data, true, static_cast<std::int32_t>(data.player));
        if (data.count == 0) return false;
        Write(point, 0, data.points[Game_Random_GetNumber() % data.count]);
        return true;
    }

    // Partial original function: FUN_00468f5d; scout animation 43 branch only.
    // House, wall and vehicle construction remain separate reconstruction targets.
    void Human_l_PerformTaskEvent_Build(void* human)
    {
        if (!Human_Target_IsValid(human)) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 7); return; }
        if (Human_IsJobScout(human) && Read<std::uint32_t>(human, 0xAB8) == 43 && GuideManager_InsertGuideAtPosition(gGuideManager, nullptr, Read<std::uint32_t>(human, 0x10), RealTarget(human), 1))
        {
            Human_l_JobExperience_Gain(human);
            Write<std::uint32_t>(human, 0x36C, 2);
        }
        else Write<std::uint32_t>(human, 0x36C, 3);
    }
}
