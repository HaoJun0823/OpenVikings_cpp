#include "C2HumanAtomicEvents.h"
#include "C2HumanWeaponEvents.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2VehicleAccess.h"
#include "C2HumanGoods.h"
#include "C2HumanProduction.h"
#include "C2HumanCollectionEvents.h"
#include "C2HumanFamily.h"
#include "C2HumanScout.h"
#include "C2HumanChest.h"
#include "C2GameControlStartup.h"
#include "C2HouseAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2Debug.h"
#include "XDiagnostics.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        void* House(std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; }
        void* TargetHuman(const void* human) { return CHumanArray::mStaticVars + Read<std::uint32_t>(human, 0xA7C) * CHumanArray::ElementSize; }
        void AddAttribute(void* human, std::uint32_t offset, std::uint32_t amount) { Write(human, offset, Read<std::uint32_t>(human, offset) + amount); }

        // Windows 00463AD2; age/ignore-needs predicate.
        bool CanChangeNeeds(const void* human) { return Human_IsAgeAdult(human) && (Read<std::uint32_t>(human, 0xB84) & 1u) == 0; }
    }

    // Original function: FUN_0042c93f.
    bool Human_IsJobIgnoringHomeHouse(const void* human) { return Read<std::uint8_t>(gStaticJobTypeData, Read<std::uint32_t>(human, 0x18) * 0x88u + 0x81u) != 0; }

    // Original function: FUN_0042cd5d; PE bytes recover the misaligned export.
    bool Human_IsInHomeHouse(const void* human)
    {
        const auto id = Read<std::uint32_t>(human, 0xB28);
        return id != 0xFFFFFFFFu && Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(House(id), 0x24);
    }

    // Original function: FUN_0042cd98.
    bool Human_IsInWorkHouse(const void* human)
    {
        const auto id = Read<std::uint32_t>(human, 0xB34);
        return id != 0xFFFFFFFFu && Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(House(id), 0x24);
    }

    // Original function: FUN_0045cca7.
    void Human_l_ResetAtomic(void* human)
    {
        Write<std::uint32_t>(human, 0xAB8, 0);
        Write<std::uint8_t>(human, 0xABC, 0);
    }

    // Original function: FUN_00468b68; Windows invokes callback 0x17.
    void Human_l_PerformTaskEvent_PlaySoundFX(void* human, std::uint32_t sound)
    {
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x17, true, sound, MapMigPoint_GetLong(Human_GetMapPositionRef(human)), 0);
    }

    // Implementation boundary: advance animations only when their event leaves
    // are connected. Weapon-hit event 25 includes delayed projectile damage.
    bool Human_AreAtomicEventsReconstructed(const void* human, const SStaticAtomicAnimationData& animation)
    {
        for (std::uint32_t index = 0; index < animation.eventCount; ++index)
        {
            const auto type = static_cast<std::uint32_t>(animation.events[index].type);
            if (type < 1u || type > 36u) continue; // Original default is a no-op.
            switch (type)
            {
            case 1: case 2: case 3: case 4: case 5: case 6: case 7: case 8: case 9: case 10:
            case 14: case 17: case 22: case 23: case 25: case 26: case 34: case 35: case 36:
                break;
            case 11: case 12: case 24: case 27: case 28: case 29: case 30: case 31: case 32: break;
            case 13: case 15: case 16: case 18: case 19: break;
            case 21: break;
            case 20: break;
            case 33: break;
            default: return false;
            }
        }
        return true;
    }

    // Partial original function: FUN_00468678; complete selector table 00468AD8..
    // 00468B67. Building events use the complete native context dispatcher.
    void Human_l_PerformTaskEvent(void* human, const SStaticAtomicAnimationEventData& event)
    {
        const auto amount = static_cast<std::uint32_t>(event.parameter);
        switch (event.type)
        {
        case 1:
            if (CanChangeNeeds(human))
            {
                auto delta = amount;
                if (Human_IsInHomeHouse(human))
                {
                    if (House_HomeQualityEnhancer_ModifyValue(House(Read<std::uint32_t>(human, 0xB28)), 0, -1)) delta *= 2u;
                }
                else if (!Human_IsJobIgnoringHomeHouse(human)) delta = static_cast<std::uint32_t>(event.parameter / 2);
                AddAttribute(human, 0xAAC, delta);
            }
            return;
        case 2:
            if (CanChangeNeeds(human)) AddAttribute(human, 0xAA8, Human_IsInHomeHouse(human) ? amount * 2u : amount);
            return;
        case 3:
        {
            const auto action = Read<std::uint32_t>(human, 0xAB8);
            if ((action == 14 || action == 15) && !Human_Target_IsValid(human) && Human_l_IsAtomicInterruptable(human))
            {
                if (Read<std::uint32_t>(human, 0xA68) == 7)
                {
                    void* other = TargetHuman(human);
                    if (Read<std::uint8_t>(other, 0) != 0 && Read<std::uint32_t>(other, 8) == Read<std::uint32_t>(human, 0xA80)) Human_StopWaitingForOtherHuman(other);
                }
                Human_CurrentTask_DoInterrupt(human);
                Human_l_ResetAtomic(human);
                return;
            }
            if (CanChangeNeeds(human)) AddAttribute(human, 0xAB0, amount);
            return;
        }
        case 4:
            if (CanChangeNeeds(human)) AddAttribute(human, 0xAB4, amount);
            return;
        case 5:
            if (CanChangeNeeds(human) && (event.parameter > 0 || (Read<std::uint32_t>(human, 0xB84) & 8u) == 0)) AddAttribute(human, 0xAA0, amount);
            return;
        case 6: case 7:
            NXBasics::ReportWarning("l_PerformTaskEvent_SendHitpoints: not implemented yet!");
            Human_StopCurrentGoodCirculationMode(human);
            return;
        case 8: Human_l_PerformTaskEvent_SendSocial(human, event.parameter); return;
        case 9: Human_l_PerformTaskEvent_SendReligion(human, event.parameter); return;
        case 10: Human_l_PerformTaskEvent_SendHitpoints(human, event.parameter); return;
        case 11: Human_l_PerformTaskEvent_Pickup(human); return;
        case 12: Human_l_PerformTaskEvent_Pileup(human); return;
        case 13: Human_l_PerformTaskEvent_SplitUp(human); return;
        case 15: Human_l_PerformTaskEvent_Transform(human); return;
        case 16: Human_l_PerformTaskEvent_Plant(human); return;
        case 18: Human_l_PerformTaskEvent_Grow(human); return;
        case 19: Human_l_PerformTaskEvent_CatchFish(human); return;
        case 21: Human_l_PerformTaskEvent_Produce(human); return;
        case 20: Human_l_PerformTaskEvent_Build(human); return;
        case 24: Human_l_PerformTaskEvent_GiveBirth(human); return;
        case 25: Human_l_PerformTaskEvent_Attack(human); return;
        case 27:
        {
            if (Human_IsAttachedToWorkHouse(human))
            {
                void* house = House(Read<std::uint32_t>(human, 0xB34));
                const auto output = Human_l_JobEfficiency_CalculateOutputAmountDecimal(human);
                if (House_Stock_ModifyAmount(house, amount, output / 10, output % 10, false)) Write<std::uint32_t>(human, 0x36C, 2);
            }
            else { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 2); }
            return;
        }
        case 28:
            if ((Read<std::uint32_t>(human, 0xB84) & 0x800u) == 0 && (Read<std::uint32_t>(human, 0x36C) == 1 || Read<std::uint32_t>(human, 0x36C) == 2)) Human_l_JobExperience_Gain(human);
            return;
        case 29:
            if (Read<std::uint32_t>(human, 0x36C) == 1 || Read<std::uint32_t>(human, 0x36C) == 2)
            {
                if (Read<std::uint32_t>(human, 0xAB8) == 89) Human_l_JobTraining_Train(human, amount);
                else if (Read<std::uint32_t>(human, 0xAB8) == 90) Human_l_JobTraining_Exercise(human, amount);
            }
            return;
        case 30: Human_l_PerformTaskEvent_ChangeGold(human, event.parameter); return;
        case 31: Human_l_PerformTaskEvent_ChangeWealth(human, event.parameter); return;
        case 32: Human_l_PerformTaskEvent_DidProduceOneGood(human); return;
        case 33: Human_l_PerformTaskEvent_OpenChest(human); return;
        case 14: C2Debug_WarningBox("cAtomicAnimationEventTypeMerge not handled yet!"); return;
        case 17: C2Debug_WarningBox("cAtomicAnimationEventTypeShrink not handled yet!"); return;
        case 22: Human_HouseMisc_Leave(human); return;
        case 23: Human_HouseMisc_Enter(human); return;
        case 34: Human_l_PerformTaskEvent_PlaySoundFX(human, amount); return;
        case 35:
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x22, true, Read<std::uint32_t>(human, 4), amount, 0);
            return;
        case 36:
        {
            std::uint32_t position;
            if (Human_IsInHomeHouse(human)) position = Read<std::uint32_t>(House(Read<std::uint32_t>(human, 0xB28)), 0x1C);
            else if (Human_IsInWorkHouse(human)) position = Read<std::uint32_t>(House(Read<std::uint32_t>(human, 0xB34)), 0x1C);
            else return;
            MapMigPoint_MoveCursorUnwrapped(position, 1);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x25, true, amount, MapMigPoint_GetLong(&position), 0);
            return;
        }
        default: return;
        }
    }

    // Original function: FUN_0046a1bf; positive values deliberately do nothing.
    void Human_l_PerformTaskEvent_ChangeGold(void* human, std::int32_t amount)
    {
        if (amount >= 0) return;
        if (Human_IsAttachedToLearnHouse(human) && Human_Misc_IsHidden(human))
        {
            auto* house = House(Read<std::uint32_t>(human, 0xB34));
            if (Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(house, 0x24) && House_Production_IsAbleToProduceGood(house, 8, false))
            {
                if (!House_Stock_ModifyAmount(house, 8, amount, 0, false)) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 13); Write<std::uint32_t>(human, 0xAD0, 7); }
                return;
            }
        }
        Write<std::uint32_t>(human, 0x374, 0);
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_0046a291.
    void Human_l_PerformTaskEvent_ChangeWealth(void* human, std::int32_t amount)
    {
        if (GameControl_Wealth_ChangeAmount(gGameControl, Read<std::int32_t>(human, 0x10), amount)) Write<std::uint32_t>(human, 0x36C, 2);
        else { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 13); Write<std::uint32_t>(human, 0xAD0, 7); }
    }

    // Original function: FUN_0046a2d9; update counters only after successful completion.
    void Human_l_PerformTaskEvent_DidProduceOneGood(void* human)
    {
        if (Read<std::uint32_t>(human, 0x36C) != 2) return;
        if (Human_IsAttachedToWorkHouse(human)) House_Production_DidProduceOneGood(House(Read<std::uint32_t>(human, 0xB34)));
        Human_Production_DidProduceGoods(human, 1u);
    }

    // Original function: FUN_0046a0be. The unsafe pointer/revision check does not
    // inspect the allocation byte and is intentionally different from Target_IsValid.
    void Human_l_PerformTaskEvent_SendSocial(void* human, std::int32_t amount)
    {
        void* other = TargetHuman(human);
        if (other == nullptr || Read<std::uint32_t>(other, 8) != Read<std::uint32_t>(human, 0xA80)) return;
        AddAttribute(other, 0xAB0, static_cast<std::uint32_t>(amount));
        if (amount < 0) return;
        Human_DoAddNewUserCommand(other, 12, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
        if (Read<std::uint32_t>(other, 0x368) == 1 && Human_l_IsAtomicInterruptable(other)) Human_CurrentTask_DoInterrupt(other);
    }

    // Original function: FUN_0046a121; Windows PE constant 004F3B50 is 8000.
    void Human_l_PerformTaskEvent_SendReligion(void* human, std::int32_t amount)
    {
        void* other = TargetHuman(human);
        if (other == nullptr || Read<std::uint32_t>(other, 8) != Read<std::uint32_t>(human, 0xA80)) { Human_Target_Reset(human); return; }
        if (Read<std::int32_t>(other, 0xAB4) <= 8000) AddAttribute(other, 0xAB4, static_cast<std::uint32_t>(amount));
    }

    // Original function: FUN_0046a165; the receiver maximum is checked before adding.
    void Human_l_PerformTaskEvent_SendHitpoints(void* human, std::int32_t amount)
    {
        void* other = TargetHuman(human);
        if (other == nullptr || Read<std::uint32_t>(other, 8) != Read<std::uint32_t>(human, 0xA80)) { Human_Target_Reset(human); return; }
        const auto hitpoints = Read<std::int32_t>(other, 0xAA0);
        if (hitpoints < static_cast<std::int32_t>(Human_GetHumanAttributeMaximum_Hitpoints(other)) && (amount > 0 || (Read<std::uint32_t>(other, 0xB84) & 8u) == 0)) AddAttribute(other, 0xAA0, static_cast<std::uint32_t>(amount));
    }
}
