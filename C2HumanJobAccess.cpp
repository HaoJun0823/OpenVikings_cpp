#include "C2HumanJobAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2CallbackManager.h"
#include "C2GameControlStartup.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapMoveableAccess.h"
#include "C2MissionStartObjects.h"
#include "C2StaticTribeTypeData.h"
#include "C2UserMessageAccess.h"

#include <cstring>
#include <initializer_list>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    namespace
    {
        const STribeJobEnables& JobEnables(const void* human)
        {
            return gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].jobEnables[Read<std::uint32_t>(human, 0x18)];
        }
        void AppendMessageItem(void* message, std::uint32_t countOffset, std::uint32_t item)
        {
            const auto count = Read<std::uint32_t>(message, countOffset);
            Write<std::uint32_t>(message, countOffset + 4u + count * 4u, item);
            Write<std::uint32_t>(message, countOffset, count + 1u);
        }
    }

    // Original function: FUN_00441aad.
    bool Human_Misc_IsHidden(const void* human) { return Read<std::uint8_t>(human, 0xB50) != 0 || Read<std::uint8_t>(human, 0xB51) != 0; }

    // Original function: FUN_004355b5; the first queued job-change command overrides the current job.
    std::uint32_t Human_UserControl_GetNextJobType(const void* human)
    {
        for (std::uint32_t slot = 0; slot < 87; ++slot) if (Read<std::uint8_t>(human, 0x390u + slot * 20u) == 21) return Read<std::uint32_t>(human, 0x39Cu + slot * 20u);
        return Read<std::uint32_t>(human, 0x18);
    }

    // Original function: FUN_004622b9.
    std::uint32_t Tool_Human_GetReallyProducedGoodType(std::uint32_t job, std::uint32_t good)
    {
        if (job == 15 && good == 56) return 21;
        return good == 22 ? 16u : good;
    }

    // Original function: FUN_004369e1; separate scans retain command precedence.
    bool Human_UserControl_DoesWantToMarry(const void* human)
    {
        if (Read<std::uint32_t>(human, 0x368) == 5) return true;
        for (const auto opcode : {13u, 14u})
            for (std::uint32_t index = 0; index < 87; ++index) if (Read<std::uint8_t>(human, 0x390u + index * 20u) == opcode) return true;
        return false;
    }

    // Original function: FUN_00436a19; only job 5 enters the child-command scans.
    bool Human_UserControl_DoesWantToGetChild(const void* human)
    {
        if (Read<std::uint32_t>(human, 0x18) != 5) return false;
        const auto task = Read<std::uint32_t>(human, 0x368);
        if (task == 11 || task == 12) return true;
        for (const auto opcode : {15u, 16u})
            for (std::uint32_t index = 0; index < 87; ++index) if (Read<std::uint8_t>(human, 0x390u + index * 20u) == opcode) return true;
        return false;
    }

    // Original function: FUN_004421e2, including unsigned product wrap before division.
    std::uint32_t Human_Tool_Gui_GetExperienceForEquippedWeapon(const void* human)
    {
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        const auto weapon = Read<std::uint32_t>(human, 0xAE4);
        const auto category = Read<std::uint32_t>(gStaticWeaponTypeArrays[tribe], weapon * 0xC4u + 0x20);
        std::uint32_t experience = 0;
        if (category == 1) experience = Read<std::uint32_t>(human, 0x140);
        else if (category == 2) experience = Read<std::uint32_t>(human, 0x144);
        else if (category == 3) experience = Read<std::uint32_t>(human, 0x148);
        else if (category == 6) experience = Read<std::uint32_t>(human, 0x150);
        else if (category == 7) experience = Read<std::uint32_t>(human, 0x154);
        const auto percentage = experience * 100u / 100u;
        return percentage > 100u ? 100u : percentage;
    }

    // Original function: FUN_0046224c; the final matching experience IDs win.
    void Tool_Human_GetExperienceType_GainedViaWorking(std::int32_t job, std::uint32_t good, std::uint32_t& general, std::uint32_t& specific)
    {
        general = 0; specific = 0;
        if (job < 7 || job > 30) return;
        const auto* table = static_cast<const std::byte*>(gStaticHumanJobExperienceTable00510A3C) + static_cast<std::uint32_t>(job) * 0x2Cu;
        const auto count = Read<std::uint32_t>(table, 0);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto id = Read<std::uint32_t>(table, 4u + index * 4u);
            const auto* data = static_cast<const std::byte*>(gStaticHumanJobExperienceData) + id * 0x34u;
            const auto firstGood = Read<std::uint32_t>(data, 0x24);
            if (firstGood == 0) general = id;
            else if (firstGood == good || (Read<std::uint32_t>(data, 0x28) != 0 && Read<std::uint32_t>(data, 0x28) == good)) specific = id;
        }
    }

    // Original function: FUN_00442047.
    void Human_GainExperienceForEquippedWeapon(void* human, std::uint32_t amount)
    {
        if ((Read<std::uint32_t>(human, 0xB84) & 0x800u) == 0) Human_GainExperienceForWeapon(human, Read<std::uint32_t>(human, 0xAE4), amount);
    }

    // Original function: FUN_0044206c; weapon-category experience uses unsigned saturation.
    void Human_GainExperienceForWeapon(void* human, std::uint32_t weapon, std::uint32_t amount)
    {
        if ((Read<std::uint32_t>(human, 0xB84) & 0x800u) != 0) return;
        const auto category = Read<std::uint32_t>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)], weapon * 0xC4u + 0x20u);
        const auto offset = category == 1 ? 0x140u : category == 2 ? 0x144u : category == 3 ? 0x148u : category == 6 ? 0x150u : category == 7 ? 0x154u : 0u;
        if (offset == 0) return;
        const auto previous = Read<std::uint32_t>(human, offset);
        Write(human, offset, previous > 10000u - amount ? 10000u : previous + amount);
    }

    // Original function: FUN_00442420.
    void Human_l_JobExperience_Gain(void* human)
    {
        if ((Read<std::uint32_t>(human, 0xB84) & 0x800u) != 0) return;
        auto good = Read<std::uint32_t>(human, 0x1C);
        if (good == 65) good = Read<std::uint32_t>(human, 0x20);
        std::uint32_t general, specific;
        Tool_Human_GetExperienceType_GainedViaWorking(Read<std::int32_t>(human, 0x18), good, general, specific);
        auto increment = [human](std::uint32_t offset) { const auto previous = Read<std::uint32_t>(human, offset); Write(human, offset, previous > 9999u ? 10000u : previous + 1u); };
        if (general != 0) increment(0x24u + general * 4u);
        if (specific != 0 && specific != general) increment(0x24u + specific * 4u);
        const auto job = Read<std::int32_t>(human, 0x18);
        if ((job >= 31 && job <= 35) || (job >= 40 && job <= 41))
        {
            increment(0x138);
            Human_GainExperienceForWeapon(human, Read<std::uint32_t>(human, 0xAE4), 1);
        }
    }

    // Original function: FUN_0044b9db; increasing stops at 11, decreasing stops at zero.
    void Human_Production_ChangeCounter(void* human, std::uint32_t good, std::int32_t amount)
    {
        const auto offset = 0x21Cu + good * 4u;
        if (amount > 0)
        {
            while (amount-- > 0 && Read<std::int32_t>(human, offset) < 11) Write(human, offset, Read<std::uint32_t>(human, offset) + 1u);
        }
        else while (amount++ < 0 && Read<std::int32_t>(human, offset) > 0) Write(human, offset, Read<std::uint32_t>(human, offset) - 1u);
    }

    // Original function: FUN_0044b8fc; rotation also resets the production failure counter.
    bool Human_Production_Rotate(void* human)
    {
        const auto initial = Read<std::uint32_t>(human, 0x218);
        for (;;)
        {
            auto index = Read<std::uint32_t>(human, 0x218) + 1u;
            if (NXBasics::Runtime_TimeSigned32(index) >= 66) index = 0;
            Write(human, 0x218, index);
            const auto counter = Read<std::int32_t>(human, 0x21Cu + index * 4u);
            if (counter > 0 && counter <= 11) { Write(human, 0x1C, index); Write<std::uint32_t>(human, 0x38C, 0); return true; }
            if (index == initial) break;
        }
        Write<std::uint32_t>(human, 0x218, 0);
        Write<std::uint32_t>(human, 0x1C, 0);
        Write<std::uint32_t>(human, 0x38C, 0);
        return false;
    }

    // Original fraction overload: FUN_0044b871; round at five decimal tenths.
    void Human_Production_DidProduceGoods(void* human, std::int32_t amount, std::int32_t fraction)
    {
        if (fraction >= 5) amount = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(amount) + 1u);
        Human_Production_DidProduceGoods(human, static_cast<std::uint32_t>(amount));
    }

    // Original function: FUN_0044b888.
    void Human_Production_DidProduceGoods(void* human, std::uint32_t amount)
    {
        const auto good = Read<std::uint32_t>(human, 0x1C);
        if (Read<std::uint32_t>(human, 0x21Cu + good * 4u) != 11) Human_Production_ChangeCounter(human, good, NXBasics::Runtime_TimeSigned32(0u - amount));
        if (Human_IsJobBuilder(human)) return;
        // Original message functions: FUN_0045c608 and FUN_0045c667.
        if (Read<std::uint32_t>(human, 0x21Cu + good * 4u) == 0 && good != 0)
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 24);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            Write(message, 0x30, good);
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        if (!Human_Production_Rotate(human))
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 25);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
    }

    // Original function: FUN_0044b8dd.
    void Human_Production_CouldNotProduceCurrentGood(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0x1C);
        // Original message function: FUN_0045c6b5.
        if (good != 0)
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 26);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            Write(message, 0x30, good);
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        if (!Human_Production_Rotate(human))
        {
            // Original message function: FUN_0045c714.
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 27);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
    }

    // Original function: FUN_004422e9; tool quality is checked by HasEquippedTools.
    std::int32_t Human_l_JobEfficiency_CalculateOutputAmountDecimal(const void* human)
    {
        std::uint32_t factor = 100;
        if (Human_HasEquippedTools(human))
        {
            if (Read<std::uint32_t>(human, 0xAF0) == 31) factor = 125;
            else if (Read<std::uint32_t>(human, 0xAF0) == 32) factor = 175;
        }
        const auto experience = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(Human_JobEfficiency_CalculateExperienceFactor(human)) * 15u) / 100;
        return NXBasics::Runtime_TimeSigned32(factor * 10u) / 100 + experience;
    }

    // Original function: FUN_00442335; signed and unsigned divisions match their native stages.
    std::int32_t Human_JobEfficiency_CalculateExperienceFactor(const void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0x1C);
        std::uint32_t general, specific;
        Tool_Human_GetExperienceType_GainedViaWorking(Read<std::int32_t>(human, 0x18), good == 65 ? Read<std::uint32_t>(human, 0x20) : good, general, specific);
        const auto id = specific != 0 ? specific : general;
        if (id == 0) return 0;
        const auto weighted = Read<std::uint32_t>(human, 0x24u + id * 4u) * Read<std::uint32_t>(gStaticHumanJobExperienceData, id * 0x34u + 0x2C) / 100u;
        const auto divisor = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(weighted * 100u) / 5) + 100u);
        const auto value = NXBasics::Runtime_TimeSigned32(100u - static_cast<std::uint32_t>(10000 / divisor));
        const auto percentage = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(value) * 100u) / 98;
        return percentage < 0 ? 0 : percentage > 100 ? 100 : percentage;
    }

    // Original function: FUN_0042c506. Signed player comparison; negative IDs remain unchecked.
    std::uint32_t Human_GetPlayerType(const void* human)
    {
        const auto player = Read<std::int32_t>(human, 0x10);
        if (player >= 20) return 0;
        return Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u + 4u);
    }
    // Original function: FUN_0042c571.
    bool Human_IsAgeBaby(const void* human) { const auto job = Read<std::uint32_t>(human, 0x18); return job == 1 || job == 2; }
    // Original function: FUN_0042c5be.
    bool Human_IsJobBuilder(const void* human) { return Read<std::uint32_t>(human, 0x18) == 7; }
    // Original function: FUN_0042c5db.
    bool Human_IsJobScout(const void* human) { return Read<std::uint32_t>(human, 0x18) == 27; }
    // Original function: FUN_0042c5e5.
    bool Human_IsJobHunter(const void* human) { return Read<std::uint32_t>(human, 0x18) == 15; }
    // Original function: FUN_0042c5ef.
    bool Human_IsJobBreeder(const void* human) { return Read<std::uint32_t>(human, 0x18) == 16; }
    // Original function: FUN_0042c5f9.
    bool Human_IsJobTrader(const void* human) { const auto job = Read<std::uint32_t>(human, 0x18); return job == 25 || job == 26; }
    // Original function: FUN_0042c60d.
    bool Human_IsJobCarrier(const void* human) { return Read<std::uint32_t>(human, 0x18) == 24; }
    // Original function: FUN_0042c617.
    bool Human_IsJobFisher(const void* human) { const auto job = Read<std::uint32_t>(human, 0x18); return job == 22 || job == 23; }
    // Original function: FUN_0042c878.
    bool Human_IsProducingVehicle(const void* human) { const auto good = Read<std::int32_t>(human, 0x1C); return good >= 59 && good <= 63; }
    // Original function: FUN_0042d471.
    void Human_Equipment_SetShoeType(void* human, std::uint32_t good) { Write<std::uint32_t>(human, 0xAEC, good); }

    // Original function: FUN_0042d6ad.
    bool Human_CanEquipShoes(const void* human)
    {
        if (!Human_IsSexMale(human)) return false;
        const auto job = Read<std::int32_t>(human, 0x18);
        return job < 1 || job > 4;
    }
    // Original function: FUN_0042d6cf. Both initial tool checks call this same Windows function.
    bool Human_CanEquipToolIron(const void* human)
    {
        const auto job = Read<std::int32_t>(human, 0x18);
        return (job >= 8 && job <= 30) || job == 7;
    }

    // Original function: FUN_0042d798.
    bool Human_DoesExperienceAllowJobChange(const void* human, std::uint32_t job)
    {
        const auto& requirement = gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].jobNeeds[job];
        std::uint32_t experience = 0;
        for (std::uint32_t i = 0; i < requirement.jobCount; ++i) experience += Read<std::uint32_t>(human, 0x24u + static_cast<std::uint32_t>(requirement.jobs[i]) * 4u);
        return experience >= requirement.requiredExperience;
    }
    // Original function: FUN_0042d7dd.
    bool Human_DoesExperienceAllowGoodProduction(const void* human, std::uint32_t good)
    {
        const auto& requirement = gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].goodNeeds[good];
        std::uint32_t experience = 0;
        for (std::uint32_t i = 0; i < requirement.jobCount; ++i) experience += Read<std::uint32_t>(human, 0x24u + static_cast<std::uint32_t>(requirement.jobs[i]) * 4u);
        return experience >= requirement.requiredExperience;
    }
    // Original function: FUN_0042d822.
    bool Human_DoesExperienceAllowHouseBuilding(const void* human, std::uint32_t house)
    {
        const auto& requirement = gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].houseNeeds[house];
        std::uint32_t experience = 0;
        for (std::uint32_t i = 0; i < requirement.jobCount; ++i) experience += Read<std::uint32_t>(human, 0x24u + static_cast<std::uint32_t>(requirement.jobs[i]) * 4u);
        return experience >= requirement.requiredExperience;
    }
    // Original function: FUN_0042d867.
    bool Human_DoesExperienceAllowEquipment(const void* human, std::uint32_t good)
    {
        const auto& requirement = gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].equipmentNeeds[good];
        std::uint32_t experience = 0;
        for (std::uint32_t i = 0; i < requirement.jobCount; ++i) experience += Read<std::uint32_t>(human, 0x24u + static_cast<std::uint32_t>(requirement.jobs[i]) * 4u);
        return experience >= requirement.requiredExperience;
    }
    // Original function: FUN_0042d912.
    bool Human_DoesEducationAllowJobChange(const void* human, std::uint32_t job) { return Read<std::uint32_t>(human, 0x158) >= gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].jobTraining[job]; }
    // Original function: FUN_0042d938.
    bool Human_DoesEducationAllowGoodProduction(const void* human, std::uint32_t good) { return Read<std::uint32_t>(human, 0x158) >= gStaticTribeTypeData[Read<std::uint32_t>(human, 0x14)].goodTraining[good]; }

    // Original function: FUN_0042d9fa.
    bool Human_IsAbleToProduceGoodNow(const void* human, std::uint32_t good)
    {
        if (good == 0) return true;
        if (Read<std::uint8_t>(human, 0x195u + good) == 0 || !Human_IsCapableToProduceGood(human, good)) return false;
        const auto houseId = Read<std::uint32_t>(human, 0xB34);
        const auto job = Read<std::uint32_t>(human, 0x18);
        if (houseId == 0xFFFFFFFFu) return Read<std::uint8_t>(gStaticJobTypeData, job * 0x88u + 0x83u) == 0;
        const void* house = CHouseArray::mStaticVars + houseId * CHouseArray::ElementSize;
        if (Read<std::uint8_t>(gStaticJobTypeData, job * 0x88u + 0x84u) != 0 && Read<std::uint32_t>(house, 0x2C) != 3) return false;
        return House_Production_IsAbleToProduceGood(house, good, true) || House_Production_IsAbleToProduceGood(house, good, false);
    }
    // Original function: FUN_0042d96c; military jobs also require the matching equipped weapon.
    bool Human_IsAbleToDoJob(const void* human, std::uint32_t job)
    {
        if (Read<std::uint8_t>(human, 0x15Cu + job) == 0) return false;
        const auto weapon = Read<std::int32_t>(human, 0xAE4);
        switch (job)
        {
        case 31: return weapon < 4 || weapon > 17;
        case 32: return weapon == 4;
        case 33: return weapon == 5;
        case 34: return weapon == 7;
        case 35: return weapon == 8;
        case 40: return weapon == 16;
        case 41: return weapon == 17;
        default: return true;
        }
    }

    // Original function: FUN_0042dac1.
    bool Human_IsCapableToProduceGood(const void* human, std::uint32_t good)
    {
        if (good == 0 || Read<std::uint8_t>(human, 0x195u + good) == 0) return false;
        const auto& enables = JobEnables(human);
        for (std::uint32_t i = 0; i < enables.goodCount; ++i) if (static_cast<std::uint32_t>(enables.goods[i]) == good) return true;
        return false;
    }
    // Original function: FUN_0042db3e.
    std::uint32_t Human_GetHumanAttributeMaximum_Hitpoints(const void* human) { return Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32 ? 20000u : 5000u; }
    // Original function: FUN_0042db55.
    std::int32_t Human_GetHumanAttributeMargin_Hitpoints(const void* human) { return Read<std::uint32_t>(human, 0x14) == 3 && Read<std::uint32_t>(human, 0x18) == 32 ? 2880 : 720; }

    // Original function: FUN_004316ef.
    bool House_Production_IsAbleToProduceGood(const void* house, std::uint32_t good, bool primary)
    {
        const auto type = Read<std::uint32_t>(house, 0x14);
        const auto* data = reinterpret_cast<const std::byte*>(gStaticHouseTypeData) + type * 0x624u;
        const auto category = Read<std::uint32_t>(data, 0);
        if (category == 2)
        {
            if (good == 29) return !primary && Read<std::uint8_t>(house, 0x780) != 0;
            if (good == 28) return !primary && Read<std::uint8_t>(house, 0x788) != 0;
            if (good == 15) return !primary && Read<std::uint8_t>(house, 0x790) != 0 && Read<std::uint32_t>(house, 0x20) >= 2;
        }
        else if (!primary && good == 15 && type == 37) return true;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (slot == -1) return false;
        if (category != 3) return true;
        const bool slotPrimary = Read<std::uint8_t>(data, (static_cast<std::uint32_t>(slot) * 3u + 0x1Bu) * 4u) != 0;
        return primary ? !slotPrimary : slotPrimary;
    }

    // Original function: FUN_0044a45d.
    void Human_SetupCurrentJobType(void* human)
    {
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        const auto job = Read<std::uint32_t>(human, 0x18);
        Write<std::uint32_t>(message, 0x10, Read<std::uint32_t>(human, 4));
        const auto player = Read<std::uint32_t>(human, 0x10);
        Write<std::uint32_t>(message, 0, 0x12);
        Write<std::uint32_t>(message, 8, player);
        Write<std::uint8_t>(human, 0x194, Read<std::uint8_t>(gStaticJobTypeData, job * 0x88u + 0x85u) != 0 ? 1 : 0);
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        auto* playerData = static_cast<std::byte*>(gGameControl) + player * 0x4A80u;
        const auto& enables = JobEnables(human);

        const auto enableGood = [&](std::uint32_t unlockedJob, std::uint32_t good)
        {
            if (unlockedJob == 15)
            {
                if (good == 56) Write<std::uint8_t>(human, 0x1CD, 1);
            }
            else if (unlockedJob != 16 || good == 57 || good == 58) Write<std::uint8_t>(human, 0x195u + good, 1);
            const auto availableOffset = 0x1D4Cu + tribe * 66u + good;
            if (Read<std::uint8_t>(playerData, availableOffset) == 0)
            {
                Write<std::uint8_t>(playerData, availableOffset, 1);
                AppendMessageItem(message, 0x2C, good);
            }
        };
        const auto requirementsMet = [&](std::uint32_t house)
        {
            STribeBuildRequirement requirement;
            std::memcpy(&requirement, &gStaticTribeTypeData[tribe].houseBuildNeeds[house], 0x198);
            for (std::uint32_t i = 0; i < requirement.jobCount; ++i) if (Read<std::uint8_t>(playerData, 0x948u + tribe * 56u + static_cast<std::uint32_t>(requirement.jobs[i])) == 0) return false;
            for (std::uint32_t i = 0; i < requirement.goodCount; ++i) if (Read<std::uint8_t>(playerData, 0x1D4Cu + tribe * 66u + static_cast<std::uint32_t>(requirement.goods[i])) == 0) return false;
            return true;
        };
        const auto enableHouse = [&](std::uint32_t house)
        {
            PlayerAllowedThings_MarkHouseAvailable(playerData + 0x18, tribe, house);
            AppendMessageItem(message, 0x1FC, house);
        };

        for (std::uint32_t i = 0; i < enables.jobCount; ++i)
        {
            const auto unlockedJob = static_cast<std::uint32_t>(enables.jobs[i]);
            if (!Human_DoesExperienceAllowJobChange(human, unlockedJob) && !Human_DoesEducationAllowJobChange(human, unlockedJob) && Human_GetPlayerType(human) != 2) continue;
            auto* currentPlayer = static_cast<std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            const auto currentTribe = Read<std::uint32_t>(human, 0x14);
            if (Read<std::uint8_t>(currentPlayer, 0x18u + currentTribe * 56u + unlockedJob) == 0) continue;
            Write<std::uint8_t>(human, 0x15Cu + unlockedJob, 1);
            const auto availableOffset = 0x948u + currentTribe * 56u + unlockedJob;
            if (Read<std::uint8_t>(currentPlayer, availableOffset) != 0) continue;
            Write<std::uint8_t>(currentPlayer, availableOffset, 1);
            AppendMessageItem(message, 0x138, unlockedJob);
            const auto& unlocked = gStaticTribeTypeData[currentTribe].jobEnables[unlockedJob];
            for (std::uint32_t j = 0; j < unlocked.goodCount; ++j)
            {
                const auto good = static_cast<std::uint32_t>(unlocked.goods[j]);
                if (Read<std::uint8_t>(human, 0x195u + good) != 0 || Read<std::uint8_t>(currentPlayer, 0x1278u + currentTribe * 66u + good) == 0) continue;
                if (Human_DoesExperienceAllowGoodProduction(human, good) || Human_DoesEducationAllowGoodProduction(human, good) || Human_GetPlayerType(human) == 2) enableGood(unlockedJob, good);
            }
            for (std::uint32_t house = 2; house < 55; ++house)
            {
                if (Read<std::uint8_t>(currentPlayer, 0x2820u + currentTribe * 55u + house) == 0 || Read<std::uint8_t>(currentPlayer, 0x3126u + currentTribe * 55u + house) != 0) continue;
                if (Read<std::uint32_t>(gStaticHouseTypeData, house * 0x624u) == 7 || !requirementsMet(house)) continue;
                enableHouse(house);
            }
        }
        for (std::uint32_t i = 0; i < enables.goodCount; ++i)
        {
            const auto good = static_cast<std::uint32_t>(enables.goods[i]);
            if (Read<std::uint8_t>(playerData, 0x1278u + tribe * 66u + good) == 0) continue;
            if (Read<std::uint8_t>(human, 0x195u + good) != 0 && Read<std::uint8_t>(playerData, 0x1D4Cu + tribe * 66u + good) != 0) continue;
            if (!Human_DoesExperienceAllowGoodProduction(human, good) && !Human_DoesEducationAllowGoodProduction(human, good) && Human_GetPlayerType(human) != 2) continue;
            enableGood(Human_IsJobHunter(human) ? 15u : (Human_IsJobBreeder(human) ? 16u : 0u), good);
        }
        for (std::uint32_t i = 0; i < enables.houseCount; ++i)
        {
            const auto house = static_cast<std::uint32_t>(enables.houses[i]);
            if (!Human_DoesExperienceAllowHouseBuilding(human, house) && Human_GetPlayerType(human) != 2) continue;
            if (Read<std::uint8_t>(playerData, 0x2820u + tribe * 55u + house) == 0 || Read<std::uint8_t>(playerData, 0x3126u + tribe * 55u + house) != 0) continue;
            if (Read<std::uint32_t>(gStaticHouseTypeData, house * 0x624u) == 7) continue;
            const bool allowed = requirementsMet(house);
            if (Human_GetPlayerType(human) == 2 || allowed) enableHouse(house);
        }
        auto good = Read<std::uint32_t>(human, 0x1C);
        if (good != 0 && !Human_IsCapableToProduceGood(human, good))
        {
            Write<std::uint32_t>(human, 0x218, 0);
            Write<std::uint32_t>(human, 0x1C, 0);
            Write<std::uint32_t>(human, 0x38C, 0);
        }
        good = Read<std::uint32_t>(human, 0x1C);
        if ((good == 0 || Read<std::uint8_t>(human, 0x195u + good) == 0) && enables.defaultGood != 0)
        {
            const auto preferred = static_cast<std::uint32_t>(enables.defaultGood);
            if (Human_IsAbleToProduceGoodNow(human, preferred)) Human_DoExecuteUserCommand_ChangeProducedGood(human, preferred);
            else for (std::uint32_t i = 0; i < enables.goodCount; ++i) if (Human_IsAbleToProduceGoodNow(human, static_cast<std::uint32_t>(enables.goods[i]))) { Human_DoExecuteUserCommand_ChangeProducedGood(human, static_cast<std::uint32_t>(enables.goods[i])); break; }
        }
        std::memset(static_cast<std::byte*>(human) + 0x1E0, 0, 0x38);
        Write<std::uint8_t>(human, 0x1DF, Human_IsAgeAdult(human) && Human_IsSexMale(human) && !Human_IsJobHero(human) ? 1 : 0);
        if (Read<std::uint8_t>(human, 0x1DF) != 0)
        {
            // Windows uses goodCount here, not the adjacent equipmentCount field.
            for (std::uint32_t i = 0; i < enables.goodCount; ++i)
            {
                const auto item = static_cast<std::uint32_t>(enables.equipment[i]);
                if (Read<std::uint8_t>(human, 0x1E0u + item) != 0) continue;
                if ((Human_DoesExperienceAllowEquipment(human, item) || Human_GetPlayerType(human) == 2) && Read<std::uint8_t>(playerData, 0x3C78u + tribe * 56u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
            }
            if (Human_CanEquipShoes(human))
            {
                Write<std::uint8_t>(human, 0x1FE, 1);
                if (Human_CanEquipToolIron(human)) Write<std::uint8_t>(human, 0x1FF, 1);
                if (Human_CanEquipToolIron(human)) Write<std::uint8_t>(human, 0x200, 1);
            }
            for (std::uint32_t item = 43; item <= 55; ++item) if (Read<std::uint8_t>(playerData, 0x3C78u + tribe * 56u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
            if (Human_IsJobSoldier(human))
            {
                for (std::uint32_t nextJob = 31; nextJob <= 41; ++nextJob) if (Read<std::uint8_t>(playerData, 0x18u + tribe * 56u + nextJob) != 0) Write<std::uint8_t>(human, 0x15Cu + nextJob, 1);
                for (std::uint32_t item = 37; item <= 42; ++item) if (Read<std::uint8_t>(playerData, 0x3C78u + tribe * 56u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
                for (std::uint32_t item = 33; item <= 36; ++item) if (Read<std::uint8_t>(playerData, 0x3C78u + tribe * 56u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
            }
            else if (Human_IsJobHero(human))
            {
                for (std::uint32_t nextJob = 42; nextJob <= 47; ++nextJob) if (Read<std::uint8_t>(playerData, 0x18u + tribe * 56u + nextJob) != 0) Write<std::uint8_t>(human, 0x15Cu + nextJob, 1);
                for (std::uint32_t item = 33; item <= 36; ++item) if (Read<std::uint8_t>(playerData, 0x3C78u + tribe * 56u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
            }
        }
        // Original PE constants: 004F3B8C/90/94/98 = 10/5/20/20.
        Write<std::uint32_t>(human, 0xB7C, Human_IsJobScout(human) ? 20u : (Human_IsJobHero(human) ? 20u : (Human_IsAgeAdult(human) ? 10u : 5u)));
        Human_DoCheckAllUserCommands(human);
        if (Read<std::uint32_t>(message, 0x138) != 0 || Read<std::uint32_t>(message, 0x1FC) != 0 || Read<std::uint32_t>(message, 0x2C) != 0 || Read<std::uint32_t>(message, 0x2DC) != 0)
        {
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
            CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, Read<std::uint32_t>(human, 0x10), 8, 0);
        }
    }

    // Original function: FUN_0044ad2e; update only newly unlocked entries and retain native message order.
    void Human_UpdateCurrentJobType(void* human)
    {
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        Write<std::uint32_t>(message, 0, 18);
        const auto& enables = JobEnables(human);
        const auto requirementsMet = [human](std::uint32_t house)
        {
            const auto tribe = Read<std::uint32_t>(human, 0x14);
            const auto* player = static_cast<const std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            STribeBuildRequirement requirement;
            std::memcpy(&requirement, &gStaticTribeTypeData[tribe].houseBuildNeeds[house], 0x198);
            for (std::uint32_t i = 0; i < requirement.jobCount; ++i) if (Read<std::uint8_t>(player, 0x948u + tribe * 56u + static_cast<std::uint32_t>(requirement.jobs[i])) == 0) return false;
            for (std::uint32_t i = 0; i < requirement.goodCount; ++i) if (Read<std::uint8_t>(player, 0x1D4Cu + tribe * 66u + static_cast<std::uint32_t>(requirement.goods[i])) == 0) return false;
            return true;
        };
        for (std::uint32_t i = 0; i < enables.jobCount; ++i)
        {
            const auto job = static_cast<std::uint32_t>(enables.jobs[i]);
            if (Read<std::uint8_t>(human, 0x15Cu + job) != 0) continue;
            if (!Human_DoesExperienceAllowJobChange(human, job) && !Human_DoesEducationAllowJobChange(human, job) && Human_GetPlayerType(human) != 2) continue;
            const auto tribe = Read<std::uint32_t>(human, 0x14);
            auto* player = static_cast<std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            if (Read<std::uint8_t>(player, 0x18u + tribe * 56u + job) == 0) continue;
            Write<std::uint8_t>(human, 0x15Cu + job, 1);
            if (Read<std::uint8_t>(player, 0x948u + tribe * 56u + job) != 0) continue;
            Write<std::uint8_t>(player, 0x948u + tribe * 56u + job, 1);
            AppendMessageItem(message, 0x138, job);
            const auto& unlocked = gStaticTribeTypeData[tribe].jobEnables[job];
            for (std::uint32_t j = 0; j < unlocked.goodCount; ++j)
            {
                const auto good = static_cast<std::uint32_t>(unlocked.goods[j]);
                if (Read<std::uint8_t>(human, 0x195u + good) != 0 || Read<std::uint8_t>(player, 0x1278u + tribe * 66u + good) == 0) continue;
                if (!Human_DoesExperienceAllowGoodProduction(human, good) && !Human_DoesEducationAllowGoodProduction(human, good) && Human_GetPlayerType(human) != 2) continue;
                if (job == 15) { if (good == 56) Write<std::uint8_t>(human, 0x1CD, 1); }
                else if (job != 16 || good == 57 || good == 58) Write<std::uint8_t>(human, 0x195u + good, 1);
                if (Read<std::uint8_t>(player, 0x1D4Cu + tribe * 66u + good) == 0)
                {
                    Write<std::uint8_t>(player, 0x1D4Cu + tribe * 66u + good, 1);
                    AppendMessageItem(message, 0x2C, good);
                }
            }
            for (std::uint32_t house = 2; house < 55; ++house)
            {
                if (Read<std::uint8_t>(player, 0x2820u + tribe * 55u + house) == 0 || Read<std::uint8_t>(player, 0x3126u + tribe * 55u + house) != 0 || Read<std::uint32_t>(gStaticHouseTypeData, house * 0x624u) == 7 || !requirementsMet(house)) continue;
                PlayerAllowedThings_MarkHouseAvailable(player + 0x18, tribe, house);
                AppendMessageItem(message, 0x1FC, house);
            }
        }
        for (std::uint32_t i = 0; i < enables.goodCount; ++i)
        {
            const auto good = static_cast<std::uint32_t>(enables.goods[i]);
            if (Read<std::uint8_t>(human, 0x195u + good) != 0) continue;
            if (!Human_DoesExperienceAllowGoodProduction(human, good) && !Human_DoesEducationAllowGoodProduction(human, good) && Human_GetPlayerType(human) != 2) continue;
            const auto tribe = Read<std::uint32_t>(human, 0x14);
            auto* player = static_cast<std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            if (Read<std::uint8_t>(player, 0x1278u + tribe * 66u + good) == 0) continue;
            if ((!Human_IsJobHunter(human) || good == 56) && (!Human_IsJobBreeder(human) || good == 57 || good == 58))
            {
                Write<std::uint8_t>(human, 0x195u + good, 1);
                Human_Production_SetCounter(human, good, Human_IsAbleToProduceGoodNow(human, good) ? 0u : 14u);
            }
            if (Read<std::uint8_t>(player, 0x1D4Cu + tribe * 66u + good) == 0)
            {
                Write<std::uint8_t>(player, 0x1D4Cu + tribe * 66u + good, 1);
                AppendMessageItem(message, 0x2C, good);
            }
        }
        for (std::uint32_t i = 0; i < enables.houseCount; ++i)
        {
            const auto house = static_cast<std::uint32_t>(enables.houses[i]);
            if (!Human_DoesExperienceAllowHouseBuilding(human, house) && Human_GetPlayerType(human) != 2) continue;
            const auto tribe = Read<std::uint32_t>(human, 0x14);
            auto* player = static_cast<std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
            if (Read<std::uint8_t>(player, 0x2820u + tribe * 55u + house) == 0 || Read<std::uint8_t>(player, 0x3126u + tribe * 55u + house) != 0 || Read<std::uint32_t>(gStaticHouseTypeData, house * 0x624u) == 7) continue;
            const auto allowed = requirementsMet(house);
            if (Human_GetPlayerType(human) != 2 && !allowed) continue;
            PlayerAllowedThings_MarkHouseAvailable(player + 0x18, tribe, house);
            AppendMessageItem(message, 0x1FC, house);
        }
        // Windows retains the goods-loop index when selecting the equipment pointer
        // and never advances that pointer (0044B309..0044B387).
        if (enables.equipmentCount != 0)
        {
            const auto item = Read<std::uint32_t>(&enables, 0x338u + enables.goodCount * 4u);
            for (std::uint32_t i = 0; i < enables.equipmentCount; ++i)
            {
                if (Read<std::uint8_t>(human, 0x1E0u + item) != 0) continue;
                if (!Human_DoesExperienceAllowEquipment(human, item) && Human_GetPlayerType(human) != 2) continue;
                const auto tribe = Read<std::uint32_t>(human, 0x14);
                const auto* player = static_cast<const std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
                if (Read<std::uint8_t>(player, 0x3C78u + tribe * 56u + item) == 0) continue;
                Write<std::uint8_t>(human, 0x1E0u + item, 1);
                // Original message function: FUN_0045c1ea.
                std::byte equipMessage[0x304];
                ToUserMessage_Construct(equipMessage);
                Write<std::uint32_t>(equipMessage, 0, 23);
                Write(equipMessage, 0x10, Read<std::uint32_t>(human, 4));
                Write(equipMessage, 8, Read<std::uint32_t>(human, 0x10));
                Write(equipMessage, 0x30, item);
                ToUserMessage_FinishMessage(equipMessage);
                NC2LogicExternals::External_Misc_SendToUserMessage(equipMessage);
            }
        }
        if (Read<std::uint32_t>(message, 0x138) != 0 || Read<std::uint32_t>(message, 0x1FC) != 0 || Read<std::uint32_t>(message, 0x2C) != 0 || Read<std::uint32_t>(message, 0x2DC) != 0)
        {
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
    }

    // Original function: FUN_0044a2ab.
    void Human_SetJobType(void* human, std::uint32_t job)
    {
        Write<std::uint32_t>(human, 0x38C, 0);
        const auto oldJob = Read<std::uint32_t>(human, 0x18);
        if (oldJob != job) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1F, true, Read<std::uint32_t>(human, 4), oldJob, job);
        if (Human_IsAttachedToWorkHouse(human))
        {
            void* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
            if (Human_IsAttachedToLearnHouse(human)) House_Inhabitants_DetachHuman(house, Read<std::uint32_t>(human, 4), 8);
            else
            {
                House_Inhabitants_DetachHuman(house, Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 0x18) == 7 ? 4 : 2);
                if (House_Inhabitants_AttachHuman(house, Read<std::uint32_t>(human, 4), job == 7 ? 4 : 2))
                {
                    Human_l_AssignWorkHouse(human, Read<std::uint32_t>(house, 4));
                    Write<std::uint32_t>(human, 0x38C, 0);
                    Write<std::uint32_t>(human, 0xB30, Read<std::uint32_t>(house, 0x24));
                    Human_WorkCenterMarker_Update(human);
                }
            }
        }
        if (Human_IsJobTrader(human) && job != 25)
        {
            const auto merchant = Read<std::int32_t>(human, 0xB44);
            if (merchant != -1) { HumanMerchantArray_FreeMerchant(gHumanMerchantArray, merchant); Write<std::int32_t>(human, 0xB44, -1); }
        }
        else if (job == 25 && Read<std::int32_t>(human, 0xB44) == -1) Write<std::int32_t>(human, 0xB44, HumanMerchantArray_AllocateMerchant(gHumanMerchantArray, Read<std::uint32_t>(human, 4)));
        if (job == 15) Write<std::uint32_t>(human, 0xAE4, 19);
        else if (Human_IsJobHunter(human)) Write<std::uint32_t>(human, 0xAE4, 1);
        const auto currentJob = Read<std::int32_t>(human, 0x18);
        const auto signedJob = static_cast<std::int32_t>(job);
        if ((currentJob >= 31 && currentJob <= 47) || (signedJob >= 31 && signedJob <= 47))
        {
            void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            if (Read<std::uint8_t>(moveable, 0x10) != 0)
            {
                const auto point = Read<std::uint32_t>(moveable, 0x12);
                MapMoveable_Attachment_DetachFromMap(moveable);
                Write<std::uint32_t>(human, 0x18, job);
                MapMoveable_Attachment_AttachToMap(moveable, &point);
            }
            // An unattached military transition does not write the job in Windows.
        }
        else Write<std::uint32_t>(human, 0x18, job);
        Human_SetupCurrentJobType(human);
        Human_Production_Setup(human);
    }

    // Original function: FUN_00449f48.
    void Human_SetupPlayerSpecificStuff(void* human)
    {
        const auto* player = static_cast<const std::byte*>(gGameControl) + Read<std::uint32_t>(human, 0x10) * 0x4A80u;
        const auto tribe = Read<std::uint32_t>(human, 0x14);
        if (Human_IsSexMale(human))
        {
            for (std::uint32_t job = 6; job <= 41; ++job)
            {
                if (Read<std::uint8_t>(player, 0x18u + tribe * 56u + job) != 0)
                {
                    if (Human_DoesExperienceAllowJobChange(human, job)) Write<std::uint8_t>(human, 0x15Cu + job, 1);
                }
                else Write<std::uint8_t>(human, 0x15Cu + job, 0);
            }
        }
        else Write<std::uint8_t>(human, 0x161, Read<std::uint8_t>(player, 0x1Du + tribe * 56u) != 0 ? 1 : 0);
        if (Human_IsJobSoldier(human))
        {
            for (std::uint32_t job = 31; job <= 41; ++job) if (Read<std::uint8_t>(player, 0x18u + tribe * 56u + job) != 0) Write<std::uint8_t>(human, 0x15Cu + job, 1);
            for (std::uint32_t item = 37; item <= 42; ++item) if (Read<std::uint8_t>(player, 0x1278u + tribe * 66u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
            for (std::uint32_t item = 33; item <= 36; ++item) if (Read<std::uint8_t>(player, 0x1278u + tribe * 66u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
        }
        else if (Human_IsJobHero(human))
        {
            for (std::uint32_t job = 42; job <= 47; ++job) if (Read<std::uint8_t>(player, 0x18u + tribe * 56u + job) != 0) Write<std::uint8_t>(human, 0x15Cu + job, 1);
            for (std::uint32_t item = 33; item <= 36; ++item) if (Read<std::uint8_t>(player, 0x1278u + tribe * 66u + item) != 0) Write<std::uint8_t>(human, 0x1E0u + item, 1);
        }
    }

    // Original function: FUN_0044b5fe.
    void Human_Production_Setup(void* human)
    {
        for (std::uint32_t good = 1; good < 66; ++good)
        {
            std::uint32_t state;
            if (Read<std::uint8_t>(human, 0x195u + good) == 0) state = 12;
            else if (!Human_IsCapableToProduceGood(human, good)) state = 13;
            else state = Human_IsAbleToProduceGoodNow(human, good) ? 0u : 14u;
            Write<std::uint32_t>(human, 0x21Cu + good * 4u, state);
        }
        const auto& enables = JobEnables(human);
        if (enables.defaultGood == 0) return;
        const auto preferred = static_cast<std::uint32_t>(enables.defaultGood);
        if (Human_IsAbleToProduceGoodNow(human, preferred)) Write<std::uint32_t>(human, 0x21Cu + preferred * 4u, 11);
        else for (std::uint32_t i = 0; i < enables.goodCount; ++i) if (Human_IsAbleToProduceGoodNow(human, static_cast<std::uint32_t>(enables.goods[i]))) { Write<std::uint32_t>(human, 0x21Cu + static_cast<std::uint32_t>(enables.goods[i]) * 4u, 11); break; }
    }
    // Original function: FUN_0044b9bb.
    std::uint32_t Human_Production_GetCounter(const void* human, std::uint32_t good) { return Read<std::uint32_t>(human, 0x21Cu + good * 4u); }

    // Original function: FUN_0043e4b0; CHuman::ChangeProducedGoodType.
    void Human_ChangeProducedGoodType(void* human, std::uint32_t good)
    {
        if (!Human_IsCapableToProduceGood(human, good)) return;
        const auto& enables = JobEnables(human);
        for (std::uint32_t i = 0; i < enables.goodCount; ++i)
        {
            const auto item = static_cast<std::uint32_t>(enables.goods[i]);
            if (Read<std::int32_t>(human, 0x21Cu + item * 4u) <= 11 && Read<std::int32_t>(human, 0x21Cu + item * 4u) >= 1) Human_Production_SetCounter(human, item, 0);
        }
        Human_Production_SetCounter(human, good, 11);
        Write<std::uint32_t>(human, 0x38C, 0);
        Write(human, 0x218, good);
        Write(human, 0x1C, good);
    }
    // Original function: FUN_004622b9; Windows outlined conversion, inlined in the Mac caller.
    static std::uint32_t Map_GoodTool_GetRealGoodType(std::uint32_t job, std::uint32_t good)
    {
        if (job == 15 && good == 56) return 21;
        return good == 22 ? 16u : good;
    }
    // Original function: FUN_0043dcb0; returns 0/1/2, not a Boolean.
    std::uint32_t Human_DoExecuteUserCommand_ChangeProducedGood(void* human, std::uint32_t good)
    {
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        if (!Human_IsCapableToProduceGood(human, good)) return 1;
        if (Human_IsProducingVehicle(human)) { Write<std::int32_t>(human, 0xB3C, -1); Write<std::uint32_t>(human, 0xB40, 0); }
        Human_ChangeProducedGoodType(human, good);
        if (Human_IsJobBuilder(human) && !Human_IsAttachedToWorkHouse(human))
        {
            if (Human_IsAttachedToRoadPoint(human)) BuildManager_DetachBuilder(gRoadBuildManager, Read<std::uint32_t>(human, 0x10), static_cast<std::byte*>(human) + 0xB30);
            if (Human_IsAttachedToWallPoint(human)) BuildManager_DetachBuilder(gWallBuildManager, Read<std::uint32_t>(human, 0x10), static_cast<std::byte*>(human) + 0xB30);
        }
        if (Human_IsAttachedToWorkHouse(human) && !Human_IsAttachedToLearnHouse(human) && good != 0 && good != 65)
        {
            const auto realGood = Map_GoodTool_GetRealGoodType(Read<std::uint32_t>(human, 0x18), good);
            const void* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
            if (!House_Production_IsAbleToProduceGood(house, realGood, false) && !House_Production_IsAbleToProduceGood(house, realGood, true)) Human_DoExecuteUserCommand_DetachWorkHouse(human);
        }
        return 2;
    }
    // Original function: FUN_0044b6dc.
    void Human_Production_Update(void* human)
    {
        const auto& enables = JobEnables(human);
        for (std::uint32_t i = 0; i < enables.goodCount; ++i)
        {
            const auto good = static_cast<std::uint32_t>(enables.goods[i]);
            if (Read<std::uint8_t>(human, 0x195u + good) != 0) Human_Production_SetCounter(human, good, Human_IsAbleToProduceGoodNow(human, good) ? 0u : 14u);
        }
        const auto preferred = static_cast<std::uint32_t>(enables.defaultGood);
        if (preferred != 0)
        {
            if (Human_IsAbleToProduceGoodNow(human, preferred)) Human_Production_SetCounter(human, preferred, 0);
            else for (std::uint32_t i = 0; i < enables.goodCount; ++i) if (Human_IsAbleToProduceGoodNow(human, static_cast<std::uint32_t>(enables.goods[i]))) { Human_Production_SetCounter(human, static_cast<std::uint32_t>(enables.goods[i]), 0); break; }
        }
        const auto current = Read<std::uint32_t>(human, 0x1C);
        if (current != 0 && Read<std::uint8_t>(human, 0x195u + current) != 0 && Human_Production_GetCounter(human, current) != 0) return;
        if (preferred == 0) return;
        std::uint32_t selected = preferred;
        if (!Human_IsAbleToProduceGoodNow(human, preferred))
        {
            std::uint32_t i;
            for (i = 0; i < enables.goodCount; ++i) if (Human_IsAbleToProduceGoodNow(human, static_cast<std::uint32_t>(enables.goods[i]))) { selected = static_cast<std::uint32_t>(enables.goods[i]); break; }
            if (i == enables.goodCount) return;
        }
        Human_Production_SetCounter(human, selected, 11);
        Write<std::uint32_t>(human, 0x38C, 0);
        Write(human, 0x218, selected);
        Write(human, 0x1C, selected);
    }
}
