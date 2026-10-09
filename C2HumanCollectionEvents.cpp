#include "C2HumanCollectionEvents.h"
#include "C2Debug.h"
#include "C2EngineLandscapeAccess.h"
#include "C2GameControlStartup.h"
#include "C2GameEngineStartup.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanLandscapeSearch.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2StaticGoodTypeData.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        const void* Real(const void* human) { return static_cast<const std::byte*>(human) + 0xA6C; }
        bool ValidateEventTarget(void* human)
        {
            if (Read<std::uint32_t>(human, 0xACC) == 0) Human_Target_Reset(human);
            if (Human_Target_IsValid(human)) return true;
            Write<std::uint32_t>(human, 0x36C, 3);
            Write<std::uint32_t>(human, 0x374, 0);
            return false;
        }
        void WearTools(void* human) { if (Human_HasEquippedTools(human)) Human_ChangeToolCondition(human, -100); }
        bool RetryCompleted(void* human)
        {
            Human_l_JobEfficiency_SetupNeededRetries(human);
            Write(human, 0x38C, Read<std::uint32_t>(human, 0x38C) + 1u);
            if (Read<std::int32_t>(human, 0x38C) >= Human_l_JobEfficiency_GetNeededRetries(human)) { Write<std::uint32_t>(human, 0x38C, 0); return true; }
            Write<std::uint32_t>(human, 0x36C, 2);
            WearTools(human);
            Human_StopCurrentGoodCirculationMode(human);
            return false;
        }
    }

    // Original function: FUN_0046949e.
    void Human_l_PerformTaskEvent_Transform(void* human)
    {
        if (!ValidateEventTarget(human)) return;
        if (Human_IsAttachedToWorkHouse(human))
        {
            const auto good = Read<std::uint32_t>(human, 0x1C);
            std::int32_t landscape = 0;
            switch (good)
            {
            case 4: landscape = 27; break;
            case 13: landscape = 33; break;
            case 14: landscape = 36; break;
            default:
                C2Debug_WarningBoxV("Production of this good is not possible at all: %s", gStaticGoodTypeStorage.records[good].name);
                Write<std::uint32_t>(human, 0x36C, 3);
                Human_StopCurrentGoodCirculationMode(human);
                return;
            }
            Logic_EntryPoint_Edit_Start();
            const auto engineId = NC2LogicExternals::External_Landscape_GetEngineIdOfMapLandscape(landscape, false);
            const auto success = NC2LogicExternals::External_Landscape_SetMapMicroPointLandscape(Real(human), engineId, 1, true, -1);
            Write<std::uint32_t>(human, 0x36C, success ? 2u : 3u);
            Logic_EntryPoint_Edit_Stop();
        }
        else Write<std::uint32_t>(human, 0x36C, 3);
        Human_StopCurrentGoodCirculationMode(human);
    }

    // Original function: FUN_00469565; native retries release future-stock reservations.
    void Human_l_PerformTaskEvent_Grow(void* human)
    {
        if (!ValidateEventTarget(human) || !RetryCompleted(human)) return;
        if (GameMap_MapMicroPoint_Landscape_ApplyTransition(Real(human), 11) != -1)
        {
            if (Read<std::uint32_t>(human, 0x1C) == 5) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, 10, MapMigPoint_GetLong(Real(human)), 0);
            Human_l_JobExperience_Gain(human);
            Write<std::uint32_t>(human, 0x36C, 2);
            WearTools(human);
            if (Human_IsAttachedToWorkHouse(human)) House_Production_DidProduceOneGood(CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize);
        }
        else Write<std::uint32_t>(human, 0x36C, 3);
        Human_StopCurrentGoodCirculationMode(human);
        Human_Target_Reset(human);
    }

    // Original function: FUN_0046968b; both landscape setter calls are present in Windows.
    void Human_l_PerformTaskEvent_SplitUp(void* human)
    {
        if (!ValidateEventTarget(human) || !RetryCompleted(human)) return;
        const auto engineId = GameMap_MapMicroPoint_Landscape_ApplyTransition(Real(human), 5);
        Human_StopCurrentGoodCirculationMode(human);
        std::uint32_t pile = 0;
        if (engineId == -1 || !Human_FindPilePointNearPosition(human, engineId, Human_GetMapPositionRef(human), &pile)) { Write<std::uint32_t>(human, 0x36C, 3); return; }
        GameMapModifyManager_StartGameMapModify();
        NC2LogicExternals::External_Landscape_SetMapMicroPointLandscape(&pile, engineId, 1, true, -1);
        GameMap_GetConstMapMicroPointPtr(&pile);
        NC2LogicExternals::External_Landscape_SetMapMicroPointLandscape(&pile, engineId, 1, true, -1);
        GameMapModifyManager_GameMapModified(10, &pile, 0);
        GameMapModifyManager_StopGameMapModify();
        WearTools(human);
        Human_l_JobExperience_Gain(human);
        Write<std::uint32_t>(human, 0x36C, 2);
    }

    // Original function: FUN_004697d2; cursor movement precedes each transition.
    void Human_l_PerformTaskEvent_Plant(void* human)
    {
        if (!ValidateEventTarget(human)) return;
        Write<std::uint32_t>(human, 0x38C, 0);
        auto cursor = Read<std::uint32_t>(Real(human), 0);
        MapMigPoint_MoveCursorUnwrapped(cursor, 4);
        bool changed = false;
        for (std::uint32_t direction = 0; direction < 6; ++direction)
        {
            MapMigPoint_MoveCursorUnwrapped(cursor, direction);
            if (MapMigPoint_IsInsideMap(&cursor) && GameMap_MapMicroPoint_Landscape_ApplyTransition(&cursor, 7) != -1) changed = true;
        }
        if (GameMap_MapMicroPoint_Landscape_ApplyTransition(Real(human), 7) != -1) changed = true;
        Write<std::uint32_t>(human, 0x36C, changed ? 2u : 3u);
        WearTools(human);
        Human_StopCurrentGoodCirculationMode(human);
    }
}
