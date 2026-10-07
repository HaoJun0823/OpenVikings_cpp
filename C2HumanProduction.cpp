#include "C2HumanProduction.h"
#include "C2Debug.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2VehicleAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MissionStartObjects.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2StaticGoodTypeData.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        void* WorkHouse(const void* human) { return House(Read<std::uint32_t>(human, 0xB34)); }
        void* Real(void* human) { return static_cast<std::byte*>(human) + 0xA6C; }
        void* Work(void* human) { return static_cast<std::byte*>(human) + 0xA70; }
        void Fail(void* human, std::uint32_t code) { Write<std::uint32_t>(human, 0x36C, 3); Write(human, 0x374, code); }
        void Start(void* human, std::uint32_t animation) { Human_l_StartAtomicAnimation(human, animation, 6); Write<std::uint32_t>(human, 0x36C, 1); }
        bool InputsAvailable(void* house, const SStaticGoodTypeData& good)
        {
            for (std::uint32_t index = 0; index < good.distinctProductionInputCount; ++index) if (House_Stock_RequestAmountOfGoods(house, static_cast<std::uint32_t>(good.distinctProductionInputs[index]), false) < good.productionInputAmounts[index]) return false;
            return true;
        }
        bool WalkToConsumer(void* human)
        {
            std::uint32_t real = 0, work = 0;
            return Human_FindConsumerNearWorkCenter(human, Read<std::uint32_t>(human, 0xAD4), &real, &work) && Human_l_StartAtomicWalkAndBring(human, &real, &work);
        }
    }

    // Original function: FUN_0046c7be; a woman's household work requires a completed home.
    void Human_l_StartTask_ExecuteJob_Woman(void* human)
    {
        if (!Human_IsAttachedToHomeHouse(human) || Read<std::uint32_t>(House(Read<std::uint32_t>(human, 0xB28)), 0x2C) != 3) { Fail(human, 2); return; }
        if (Read<std::uint32_t>(human, 0xACC) == 0) Human_l_StartTask_ExecuteJob_Tool_FillHome_TryToFetch(human);
        else if (Read<std::uint32_t>(human, 0xAD4) != 0) Human_l_StartTask_ExecuteJob_Tool_FillHome_BringAway(human);
        else Human_l_StartTask_ExecuteJob_Tool_FillHome_Fetch(human);
    }

    // Original function: FUN_0047553c; household deliveries have their own stock semantics.
    void Human_l_StartTask_ExecuteJob_Tool_FillHome_BringAway(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        if (Human_IsAtTargetPoint(human))
        {
            auto* house = Human_GetHousePtrAtTargetPosition(human);
            if (house != nullptr)
            {
                if (House_Stock_CanStoreGoodType(house, good, false) && !House_Stock_IsFull(house, good)) Start(human, 23);
                else Human_Target_Reset(human);
                return;
            }
            if (WalkToConsumer(human)) return;
            if (Map_GoodTool_CanBePutOnMap(good, Real(human))) { Start(human, 23); return; }
        }
        else
        {
            auto* house = House(Read<std::uint32_t>(human, 0xB28));
            if (House_Stock_CanStoreGoodType(house, good, false) && House_Stock_HasDemandForGood(house, good) && Human_l_StartAtomicWalk(human, static_cast<std::byte*>(house) + 0x1C, static_cast<std::byte*>(house) + 0x24)) return;
            if (WalkToConsumer(human)) return;
        }
        std::uint32_t real = 0, work = 0;
        if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_004756eb; invalid collection destinations release reservations.
    void Human_l_StartTask_ExecuteJob_Tool_FillHome_Fetch(void* human)
    {
        if (Human_IsAtTargetPoint(human))
        {
            if (Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(human)) return;
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
        }
        else { Human_StopCurrentGoodCirculationMode(human); Human_l_StartTask_ExecuteJob_Tool_FillHome_TryToFetch(human); }
    }

    // Original function: FUN_00475720; keep the search's failure message code when walking fails.
    void Human_l_StartTask_ExecuteJob_Tool_FillHome_TryToFetch(void* human)
    {
        Human_Target_Reset(human);
        Fail(human, 2);
        if (!Human_IsAttachedToHomeHouse(human)) return;
        std::uint32_t real = 0, work = 0, good = 0;
        if (Human_FindGoodsDemandedForHomeHouse(human, &real, &work, &good, static_cast<std::byte*>(human) + 0xB30))
        {
            Write(human, 0xAD0, good);
            if (Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return;
        }
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_00473ec6; shared indoor producer entry point.
    void Human_l_StartTask_ExecuteJob_Tool_GoodProducer(void* human)
    {
        if (!Human_IsAttachedToWorkHouse(human) || Read<std::uint32_t>(human, 0x1C) == 0) { Fail(human, 2); return; }
        if (!Human_IsAbleToProduceGoodNow(human, Read<std::uint32_t>(human, 0x1C))) { Fail(human, 0); return; }
        switch (Read<std::uint32_t>(human, 0xACC))
        {
        case 2: Human_l_StartTask_ExecuteJob_Tool_FillStock_Fetch(human); return;
        case 4: Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(human); return;
        case 5: Human_l_StartTask_ExecuteJob_Tool_FlushStock_Fetch(human); return;
        case 0: break;
        default: return;
        }
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        if (Read<std::uint32_t>(human, 0x21Cu + Read<std::uint32_t>(human, 0x1C) * 4u) == 0) { Human_Production_Rotate(human); return; }
        auto* house = WorkHouse(human);
        const auto* job = static_cast<const std::byte*>(gStaticJobTypeData) + Read<std::uint32_t>(human, 0x18) * 0x88u;
        if ((Read<std::uint8_t>(job, 0x84) != 0 && Read<std::uint32_t>(house, 0x2C) != 3) || Read<std::uint8_t>(house, 0x798) != 0 || !House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0x1C), true)) { Fail(human, 2); return; }
        if (House_Stock_IsFull(house, Read<std::uint32_t>(human, 0x1C)))
        {
            Write(human, 0xAD0, Read<std::uint32_t>(human, 0x1C));
            Write<std::uint32_t>(human, 0xACC, 5);
            Human_l_StartTask_ExecuteJob_Tool_FlushStock_Fetch(human);
        }
        else Human_l_StartTask_ExecuteJob_Tool_TryToProduce(human);
    }

    // Original function: FUN_00474f17; returns cargo to construction, stock or map piles.
    void Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        if (Human_IsAtTargetPoint(human))
        {
            const auto id = Logic_EntryPoint_House_GetOnPosition(Real(human));
            if (id != -1)
            {
                auto* house = House(static_cast<std::uint32_t>(id));
                if (Read<std::uint32_t>(house, 0x2C) == 3)
                {
                    if ((House_Stock_CanStoreGoodType(house, good, false) && House_Stock_HasDemandForGood(house, good)) || (static_cast<std::uint32_t>(id) == Read<std::uint32_t>(human, 0xB34) && House_Stock_CanStoreGoodType(house, good, true) && House_Stock_HasDemandForGood(house, good))) { Start(human, 23); return; }
                }
                if (House_Construction_IsNeedingGood(house, good)) Start(human, 23);
                else Human_StopCurrentGoodCirculationMode(human);
                return;
            }
            if (good == 3 && Human_IsAttachedToRoadPoint(human)) { Start(human, 41); return; }
            if (good == 5 && Human_IsAttachedToWallPoint(human)) { Start(human, 42); return; }
            if (Human_IsAttachedToWorkHouse(human) && House_GetHouseMainType(WorkHouse(human)) != 1 && WalkToConsumer(human)) return;
            if (Map_GoodTool_CanBePutOnMap(good, Real(human))) { Start(human, 23); return; }
            std::uint32_t real = 0, work = 0;
            if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
            Write<std::uint32_t>(human, 0x36C, 3);
            return;
        }
        if (Human_IsAttachedToWorkHouse(human))
        {
            auto* house = Human_IsProducingVehicle(human) ? House(Read<std::uint32_t>(human, 0xB3C)) : WorkHouse(human);
            if (Read<std::uint32_t>(house, 0x2C) == 3)
            {
                if ((House_Stock_CanStoreGoodType(house, good, false) || House_Stock_CanStoreGoodType(house, good, true)) && House_Stock_HasDemandForGood(house, good) && !House_Stock_IsFullSoon(house, good) && Human_l_StartAtomicWalk(human, static_cast<std::byte*>(house) + 0x1C, static_cast<std::byte*>(house) + 0x24)) return;
            }
            else if (House_Construction_IsNeedingGood(house, good))
            {
                const auto* position = Human_GetMapPositionRef(human);
                const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(position), 6);
                std::uint32_t work = 0;
                if (!(House_Construction_GetNextWorkPoint(house, position, &work, true, Read<std::uint32_t>(human, 4), continent) || House_Construction_GetNextWorkPoint(house, position, &work, false, Read<std::uint32_t>(human, 4), continent)) || !Human_l_StartAtomicWalk(human, static_cast<std::byte*>(house) + 0x1C, &work)) Fail(human, 7);
                return;
            }
            Human_StopCurrentGoodCirculationMode(human);
            if (Read<std::uint32_t>(human, 0xACC) != 5) Write<std::uint32_t>(human, 0xACC, 4);
            if (House_GetHouseMainType(house) != 1 && WalkToConsumer(human)) return;
        }
        else if (good == 3 || good == 5)
        {
            const auto action = Human_IsAttachedToRoadPoint(human) ? 41u : Human_IsAttachedToWallPoint(human) ? 42u : 0u;
            if (action != 0)
            {
                auto real = Read<std::uint32_t>(human, 0xB30), work = real;
                if (action == 42 && !GameMap_Misc_GetLandscapeWorkPosition(&real, &work)) { Fail(human, 7); return; }
                Human_Target_SetTargetPositions(human, &real, &work);
                if (Human_IsAtTargetPoint(human)) Start(human, action);
                else if (!Human_l_StartAtomicWalk(human, &real, &work)) Fail(human, 1);
                return;
            }
        }
        std::uint32_t real = 0, work = 0;
        if (Human_FindPilePointNearWorkCenter(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
        real = work = 0;
        if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_004753cb; preserve Windows' >= test in the unreached branch.
    void Human_l_StartTask_ExecuteJob_Tool_FillStock_Fetch(void* human)
    {
        if (Human_IsAtTargetPoint(human))
        {
            if (Read<std::uint32_t>(human, 0xAD0) == 0) { Human_l_StartTask_ExecuteJob_Tool_CollectForProduction(human); return; }
            auto* house = Human_GetHousePtrAtTargetPosition(human);
            if (house != nullptr)
            {
                if (Read<std::uint32_t>(human, 0xA70) != Read<std::uint32_t>(house, 0x24)) C2Debug_WarningBox("standing at invalid position for interaction with house stock!");
                if (House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0xAD0), true) && House_Stock_RequestAmountOfGoods(house, Read<std::uint32_t>(human, 0xAD0), true) != 0)
                {
                    const auto type = Read<std::uint32_t>(house, 0x14);
                    Start(human, type == 10 ? 44 : type == 11 ? 45 : 22);
                    return;
                }
            }
            else if (Human_GetLandscapeTypeAtTargetPosition(human) == Read<std::uint32_t>(human, 0xA74)) { Start(human, 22); return; }
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
            return;
        }
        const auto& good = gStaticGoodTypeStorage.records[Read<std::uint32_t>(human, 0x1C)];
        auto* house = WorkHouse(human);
        Write<std::uint32_t>(human, 0xAD0, 0);
        for (std::uint32_t index = 0; index < good.distinctProductionInputCount; ++index)
        {
            Write(human, 0xAD0, static_cast<std::uint32_t>(good.distinctProductionInputs[index]));
            if (House_Stock_RequestAmountOfGoods(house, static_cast<std::uint32_t>(good.distinctProductionInputs[index]), false) >= good.productionInputAmounts[index] && Human_l_StartTask_ExecuteJob_Tool_CollectWantedGood(human)) return;
        }
        Write<std::uint32_t>(human, 0xACC, 0);
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_00475a3a; cargo-bearing flushing delegates to the bring-away leaf.
    void Human_l_StartTask_ExecuteJob_Tool_FlushStock_Fetch(void* human)
    {
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_ExecuteJob_Tool_FlushStock_BringAway(human); return; }
        auto* house = WorkHouse(human);
        if (Human_IsAtTargetPoint(human))
        {
            if (Read<std::uint32_t>(human, 0xA6C) == Read<std::uint32_t>(house, 0x1C) && House_Stock_IsFull(house, Read<std::uint32_t>(human, 0xAD0))) { Start(human, 22); return; }
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
            return;
        }
        if (!Human_FindConsumerNearWorkCenter(human, Read<std::uint32_t>(human, 0xAD0))) { Fail(human, 11); return; }
        Human_Target_SetTargetPositions(human, static_cast<std::byte*>(house) + 0x1C, static_cast<std::byte*>(house) + 0x24);
        if (Human_IsAtTargetPoint(human))
        {
            if (House_Stock_IsFull(house, Read<std::uint32_t>(human, 0xAD0))) { Start(human, 22); return; }
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
        }
        else if (Human_l_StartAtomicWalk(human, Real(human), Work(human))) return;
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_00475a4d; Windows uses ordinary walking for map-pile fallback.
    void Human_l_StartTask_ExecuteJob_Tool_FlushStock_BringAway(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        if (Human_IsAtTargetPoint(human))
        {
            const auto id = Logic_EntryPoint_House_GetOnPosition(Real(human));
            if (id != -1)
            {
                auto* house = House(static_cast<std::uint32_t>(id));
                if (House_Stock_CanStoreGoodType(house, good, false) && House_Stock_HasDemandForGood(house, good)) Start(human, 23);
                else Human_Target_Reset(human);
                return;
            }
            if (WalkToConsumer(human)) return;
            if (Map_GoodTool_CanBePutOnMap(good, Real(human))) { Start(human, 23); return; }
        }
        else if (Human_IsAttachedToWorkHouse(human) && House_GetHouseMainType(WorkHouse(human)) != 1 && WalkToConsumer(human)) return;
        std::uint32_t real = 0, work = 0;
        if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_00475d34.
    void Human_l_StartTask_ExecuteJob_Tool_TryToProduce(void* human)
    {
        if (InputsAvailable(WorkHouse(human), gStaticGoodTypeStorage.records[Read<std::uint32_t>(human, 0x1C)])) Human_l_StartTask_ExecuteJob_Tool_Produce(human);
        else Human_l_StartTask_ExecuteJob_Tool_CollectForProduction(human);
    }

    // Original function: FUN_00475da4; try deficient input types in their original order.
    void Human_l_StartTask_ExecuteJob_Tool_CollectForProduction(void* human)
    {
        Write<std::uint32_t>(human, 0xAD0, 0);
        Human_Target_Reset(human);
        const auto& good = gStaticGoodTypeStorage.records[Read<std::uint32_t>(human, 0x1C)];
        auto* house = WorkHouse(human);
        for (std::uint32_t index = 0; index < good.distinctProductionInputCount; ++index)
        {
            if (House_Stock_RequestAmountOfGoods(house, static_cast<std::uint32_t>(good.distinctProductionInputs[index]), false) >= good.productionInputAmounts[index]) continue;
            Write(human, 0xAD0, static_cast<std::uint32_t>(good.distinctProductionInputs[index]));
            if (Human_l_StartTask_ExecuteJob_Tool_CollectWantedGood(human)) return;
        }
        Write<std::uint32_t>(human, 0xACC, 0);
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_00475e31.
    bool Human_l_StartTask_ExecuteJob_Tool_CollectWantedGood(void* human)
    {
        std::uint32_t real = 0, work = 0;
        if (Human_FindGood_Complex(human, Read<std::uint32_t>(human, 0xAD0), &real, &work, static_cast<std::byte*>(human) + 0xB30) && Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return true;
        Fail(human, 3);
        return false;
    }

    // Original function: FUN_00475e9a; verify interaction position and inputs again at arrival.
    void Human_l_StartTask_ExecuteJob_Tool_Produce(void* human)
    {
        for (;;)
        {
            const auto& good = gStaticGoodTypeStorage.records[Read<std::uint32_t>(human, 0x1C)];
            auto* house = WorkHouse(human);
            if (Human_IsAtTargetPoint(human))
            {
                if (Read<std::uint32_t>(human, 0xA6C) != Read<std::uint32_t>(house, 0x1C)) { Human_Target_Reset(human); continue; }
                if (Read<std::uint32_t>(human, 0xA70) != Read<std::uint32_t>(house, 0x24)) C2Debug_WarningBox("standing at invalid position for interaction with house stock!");
                if (!InputsAvailable(house, good)) { Fail(human, 10); Human_Target_Reset(human); return; }
                Human_l_StartAtomicAnimation(human, static_cast<std::uint32_t>(good.atomicForProduction), -1);
                Write<std::uint32_t>(human, 0x36C, 1);
                House_ActionOverlay_Start(house, 3, static_cast<std::uint32_t>(gStaticAtomicAnimationStorage.records[Read<std::uint32_t>(human, 0xAC0)].length));
                House_ActionOverlay_Start(house, 4, static_cast<std::uint32_t>(gStaticAtomicAnimationStorage.records[Read<std::uint32_t>(human, 0xAC0)].length));
                return;
            }
            const auto real = Read<std::uint32_t>(house, 0x1C), work = Read<std::uint32_t>(house, 0x24);
            if (Human_l_StartAtomicWalk(human, &real, &work)) Human_Target_SetTargetPositions(human, &real, &work);
            else Fail(human, 0);
            return;
        }
    }

    // Original function: FUN_00469401; decimal amounts, tool wear and callbacks retain native order.
    void Human_l_PerformTaskEvent_Produce_Good(void* human)
    {
        auto* house = WorkHouse(human);
        const auto decimal = Human_l_JobEfficiency_CalculateOutputAmountDecimal(human);
        const auto amount = decimal / 10, fraction = decimal % 10;
        House_Stock_ModifyAmount(house, Read<std::uint32_t>(human, 0x1C), amount, fraction, false);
        if (Human_HasEquippedTools(human))
        {
            // Original Windows-only wear leaf: FUN_0046a3b3; PE constants are -100 and 10000.
            auto condition = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(human, 0xAE0) - 100u);
            Write<std::uint32_t>(human, 0xAE0, condition < 0 ? 0u : static_cast<std::uint32_t>(condition));
            if (condition >= 0 && (Read<std::uint32_t>(human, 0xAF0) == 31 || Read<std::uint32_t>(human, 0xAF0) == 32) && condition > 10000) Write<std::uint32_t>(human, 0xAE0, 10000);
        }
        if (TGoodTypes_IsFood(Read<std::uint32_t>(human, 0x1C))) Write<std::uint32_t>(human, 0xA64, 0);
        Human_l_JobExperience_Gain(human);
        House_Production_DidProduceOneGood(house);
        Human_Production_DidProduceGoods(human, amount, fraction);
        Write<std::uint32_t>(human, 0xACC, 0);
        Write<std::uint32_t>(human, 0xAD0, 0);
        Write<std::uint32_t>(human, 0x36C, 2);
    }

    // Partial original function: FUN_004691ce; animal breeding needs its allocation branch.
    void Human_l_PerformTaskEvent_Produce(void* human)
    {
        const auto id = Read<std::uint32_t>(human, 0x1C);
        if (id == 0) { Fail(human, 2); return; }
        if (id == 57 || id == 58) return;
        if (Human_IsAttachedToWorkHouse(human))
        {
            auto* house = WorkHouse(human);
            const auto& good = gStaticGoodTypeStorage.records[id];
            if (InputsAvailable(house, good) && !House_Stock_IsFull(house, id))
            {
                for (std::uint32_t index = 0; index < good.productionInputCount; ++index) House_Stock_ModifyAmount(house, static_cast<std::uint32_t>(good.productionInputs[index]), -1, 0, false);
                Human_l_PerformTaskEvent_Produce_Good(human);
            }
            else Write<std::uint32_t>(human, 0x36C, 3);
        }
        else Write<std::uint32_t>(human, 0x36C, 3);
        Human_Target_Reset(human);
    }
}
