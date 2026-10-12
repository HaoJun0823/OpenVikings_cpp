#include "SwitchIdentifiers.h"
#include "C2HumanSocialJobs.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanInitialization.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2HumanProduction.h"
#include "C2HumanTaskAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void Fail(void* human, std::uint32_t reason) { Write<std::uint32_t>(human, 0x36C, 3); Write(human, 0x374, reason); }
        bool IsWorking(const void* human) { const auto status = Read<std::uint32_t>(human, 0x36C); return status == 1 || status == 2; }
        bool HasUnsharedPosition(const void* human)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
            const auto moveable = Read<std::int16_t>(micro, 10);
            return moveable == Read<std::int32_t>(human, 12) && Read<std::int32_t>(gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(moveable) * 0x58u, 0x24) == -1;
        }
        void TargetHuman(void* human, const void* other)
        {
            Write<std::uint32_t>(human, 0xA68, 7);
            Write(human, 0xA7C, Read<std::uint32_t>(other, 4));
            Write(human, 0xA80, Read<std::uint32_t>(other, 8));
            Write(human, 0xA70, Read<std::uint32_t>(Human_GetMapPositionRef(other), 0));
            Write(human, 0xA6C, Read<std::uint32_t>(Human_GetMapPositionRef(other), 0));
        }
    }

    // Original function: FUN_0045e261; the first eligible human wins, then RNG is consumed.
    void* Human_FindHumanToEnjoy(const void* human)
    {
        CHumanIterator iterator;
        const auto job = Read<std::uint32_t>(human, 0x18);
        for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(human, 0x10), static_cast<const std::byte*>(human) + 0xB30, job == 18 || job == 29 ? 10 : 20); id != -1; id = iterator.GetNextHumanId())
        {
            auto* other = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (Read<std::uint32_t>(other, 4) == Read<std::uint32_t>(human, 4) || Read<std::uint8_t>(other, 0xB51) != 0 || Read<std::int32_t>(other, 0xAB0) > 8000 || !Human_IsJobNeedingSocial(other)) continue;
            Game_Random_GetNumber();
            return other;
        }
        return nullptr;
    }

    // Original function: FUN_0045e31a; Windows checks hitpoints, not religion.
    void* Human_FindHumanToBless(const void* human)
    {
        CHumanIterator iterator;
        const auto job = Read<std::uint32_t>(human, 0x18);
        for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(human, 0x10), static_cast<const std::byte*>(human) + 0xB30, job == 18 || job == 29 ? 10 : 20); id != -1; id = iterator.GetNextHumanId())
        {
            auto* other = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (Read<std::uint32_t>(other, 4) == Read<std::uint32_t>(human, 4) || Read<std::int32_t>(other, 0xAA0) > static_cast<std::int32_t>(Human_GetHumanAttributeMaximum_Hitpoints(other))) continue;
            Game_Random_GetNumber();
            return other;
        }
        return nullptr;
    }

    // Original function: FUN_0046f035.
    void Human_l_StartTask_ExecuteJob_Jester(void* human)
    {
        switch (Read<std::uint32_t>(human, 0xACC))
        {
        case SwitchIds::GoodCirculationMode::None: Human_l_StartTask_ExecuteJob_Jester_TryToJest(human); return;
        case SwitchIds::GoodCirculationMode::FillStockFetch: Human_l_StartTask_ExecuteJob_Jester_Collect(human); return;
        case SwitchIds::GoodCirculationMode::FillStockBringAway: Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(human); return;
        case SwitchIds::GoodCirculationMode::FlushStock: Fail(human, 0); return;
        }
    }

    // Original function: FUN_0046f06b.
    void Human_l_StartTask_ExecuteJob_Jester_Collect(void* human)
    {
        if (Human_IsAtTargetPoint(human))
        {
            if (Read<std::uint32_t>(human, 0xA68) == 7) Human_l_StartTask_ExecuteJob_Jester_Jest(human);
            else Human_l_StartTask_ExecuteJob_Tool_CollectWantedGood(human);
            return;
        }
        Human_StopCurrentGoodCirculationMode(human);
        Human_l_StartTask_ExecuteJob_Jester_TryToJest(human);
    }

    // Original function: FUN_0046f0a0.
    void Human_l_StartTask_ExecuteJob_Jester_TryToJest(void* human)
    {
        Human_Target_Reset(human);
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        auto* other = Human_FindHumanToEnjoy(human);
        if (other != nullptr && Human_l_StartAtomicWalk(human, Read<std::int32_t>(other, 12), 1))
        {
            Write<std::uint32_t>(human, 0xACC, 2);
            TargetHuman(human, other);
            return;
        }
        Fail(human, 6);
    }

    // Original function: FUN_0046f134.
    void Human_l_StartTask_ExecuteJob_Jester_Jest(void* human)
    {
        if (!HasUnsharedPosition(human)) { Human_l_StartTask_GoAway(human, 0, 0); return; }
        Human_Target_SetWorkPosition(human, Human_GetMapPositionRef(human));
        Human_l_StartAtomicAnimation(human, 16, Human_Target_GetDirectionTo(human));
        Write<std::uint32_t>(human, 0x36C, 1);
    }

    // Original function: FUN_0046f26d.
    void Human_l_StartTask_ExecuteJob_Druid(void* human)
    {
        if (Human_IsAttachedToWorkHouse(human))
        {
            const auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
            if (Read<std::uint32_t>(house, 0x2C) == 3)
            {
                if (Read<std::uint32_t>(house, 0x14) == 37) Human_l_StartTask_ExecuteJob_Druid_Priest(human);
                else Human_l_StartTask_ExecuteJob_Tool_GoodProducer(human);
                return;
            }
        }
        Human_l_StartTask_ExecuteJob_Druid_Healer(human);
    }

    // Original function: FUN_0046f2b2.
    void Human_l_StartTask_ExecuteJob_Druid_Priest(void* human)
    {
        if (!Human_Target_HasTarget(human) || Read<std::uint32_t>(human, 0xA68) != 7) Human_l_StartTask_ExecuteJob_Tool_FillTemple(human);
        if (IsWorking(human)) return;
        Human_l_StartTask_ExecuteJob_Druid_Healer(human);
        if (!IsWorking(human)) Fail(human, 2);
    }

    // Original function: FUN_0046f30b.
    void Human_l_StartTask_ExecuteJob_Druid_Healer(void* human)
    {
        if (Human_Target_IsValid(human)) Human_l_StartTask_ExecuteJob_Druid_Heal(human);
        else Human_l_StartTask_ExecuteJob_Druid_TryToHeal(human);
    }

    // Original function: FUN_0046f327.
    void Human_l_StartTask_ExecuteJob_Druid_TryToHeal(void* human)
    {
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        auto* other = Human_FindHumanToBless(human);
        if (other != nullptr && Human_l_StartAtomicWalk(human, Read<std::int32_t>(other, 12), 1))
        {
            Human_Target_Reset(human);
            TargetHuman(human, other);
            return;
        }
        Fail(human, 6);
    }

    // Original function: FUN_0046f3b1.
    void Human_l_StartTask_ExecuteJob_Druid_Heal(void* human)
    {
        if (!HasUnsharedPosition(human)) { Human_l_StartTask_GoAway(human, 0, 0); return; }
        Human_Target_SetWorkPosition(human, Human_GetMapPositionRef(human));
        Human_l_StartAtomicAnimation(human, 18, Human_Target_GetDirectionTo(human));
        Write<std::uint32_t>(human, 0x36C, 1);
    }

    // Original function: FUN_004757a6; the Windows entry contains dispatch, delivery and collection.
    void Human_l_StartTask_ExecuteJob_Tool_FillTemple(void* human)
    {
        if (Read<std::uint32_t>(human, 0xACC) == 0) { Human_l_StartTask_ExecuteJob_Tool_FillTemple_Fetch(human); return; }
        if (Read<std::uint32_t>(human, 0xAD4) == 0)
        {
            if (Human_IsAtTargetPoint(human))
            {
                if (Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(human)) return;
                Human_Target_Reset(human);
                Human_StopCurrentGoodCirculationMode(human);
            }
            else { Human_StopCurrentGoodCirculationMode(human); Human_l_StartTask_ExecuteJob_Tool_FillTemple_Fetch(human); }
            return;
        }
        std::uint32_t real = 0, work = 0;
        if (Human_IsAtTargetPoint(human))
        {
            auto* house = Human_GetHousePtrAtTargetPosition(human);
            if (house != nullptr)
            {
                if (!House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0xAD4), false) || House_Stock_IsFull(house, Read<std::uint32_t>(human, 0xAD4))) { Human_Target_Reset(human); return; }
            }
            else
            {
                if (Human_FindConsumerNearWorkCenter(human, Read<std::uint32_t>(human, 0xAD4), &real, &work) && Human_l_StartAtomicWalkAndBring(human, &real, &work)) return;
                if (!Map_GoodTool_CanBePutOnMap(Read<std::uint32_t>(human, 0xAD4), Human_Target_GetRealPositionRef(human)))
                {
                    if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
                    Write<std::uint32_t>(human, 0x36C, 3);
                    return;
                }
            }
            Human_l_StartAtomicAnimation(human, 23, 6);
            Write<std::uint32_t>(human, 0x36C, 1);
            return;
        }
        auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
        if (House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0xAD4), false) && House_Stock_HasDemandForGood(house, Read<std::uint32_t>(human, 0xAD4)) && Human_l_StartAtomicWalk(human, house + 0x1C, house + 0x24)) return;
        if (Human_FindConsumerNearWorkCenter(human, Read<std::uint32_t>(human, 0xAD4), &real, &work) && Human_l_StartAtomicWalkAndBring(human, &real, &work)) return;
        if (Human_FindPilePointNearby(human, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work)) return;
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_004759ab.
    void Human_l_StartTask_ExecuteJob_Tool_FillTemple_Fetch(void* human)
    {
        Human_Target_Reset(human);
        auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
        if (House_Stock_HasDemandForGood(house, 15))
        {
            std::uint32_t real = 0, work = 0;
            if (Human_FindGood_Complex(human, 15, &real, &work, Human_GetWorkCenterPointRef(human)))
            {
                Write<std::uint32_t>(human, 0xAD0, 15);
                if (Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return;
            }
        }
        Fail(human, 2);
    }

}
