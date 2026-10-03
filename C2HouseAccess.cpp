#include "C2HouseAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2StaticHouseTypeData.h"
#include "C2EngineDisplayAccess.h"
namespace NC2Logic
{
    // Original function: FUN_00432d14; production entries are 14h bytes and totals cap at FEh.
    static bool House_Production_ModifyFutureAmount(void* house, std::uint32_t goodId, std::int32_t delta)
    {
        using namespace NC2Lifetime;
        for (std::int32_t index = 0; index < Read<std::int32_t>(house, 0x38); ++index)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(index) * 0x14u;
            if (Read<std::uint32_t>(house, offset) != goodId) continue;
            const auto future = Read<std::uint32_t>(house, offset + 12);
            if (NXBasics::Runtime_TimeSigned32(future + static_cast<std::uint32_t>(delta)) < 0) return false;
            if (Read<std::uint32_t>(house, offset + 8) + future + static_cast<std::uint32_t>(delta) > 254u) continue;
            const auto byteDelta = static_cast<std::int8_t>(static_cast<std::uint8_t>(delta));
            Write(house, offset + 12, future + static_cast<std::uint32_t>(static_cast<std::int32_t>(byteDelta)));
            return true;
        }
        return false;
    }

    // Original function: FUN_004316ef; enhancer stocks are available only outside future-stock queries.
    bool House_Stock_CanStoreGoodType(const void* house, std::uint32_t goodId, bool futureStock)
    {
        using namespace NC2Lifetime;
        const auto* type = House_GetHouseTypeDataPtr(house);
        if (type->mainType == 2)
        {
            if (goodId == 15) return !futureStock && Read<std::uint8_t>(house, 0x790) != 0 && Read<std::uint32_t>(house, 0x20) >= 2u;
            if (goodId == 28) return !futureStock && Read<std::uint8_t>(house, 0x788) != 0;
            if (goodId == 29) return !futureStock && Read<std::uint8_t>(house, 0x780) != 0;
        }
        else if (!futureStock && goodId == 15 && Read<std::uint32_t>(house, 0x14) == 37) return true;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, goodId);
        if (slot == -1) return false;
        if (type->mainType != 3) return true;
        const auto productionOnly = Read<std::uint8_t>(type, 0x6Cu + static_cast<std::uint32_t>(slot) * 12u);
        return futureStock ? productionOnly == 0 : productionOnly != 0;
    }

    // Original function: FUN_00431d2d; negative wrapped totals fail, positive totals cap at 1000.
    bool House_Stock_ModifyFutureAmount(void* house, std::uint32_t goodId, std::int32_t delta, bool modifyProduction)
    {
        using namespace NC2Lifetime;
        if (modifyProduction && Read<std::uint32_t>(house, 0x2C) != 3 && (delta == 1 || delta == -1) && House_Production_ModifyFutureAmount(house, goodId, delta)) return true;
        if (House_GetHouseTypeDataPtr(house)->mainType == 2)
        {
            if (goodId == 15 || (NXBasics::Runtime_TimeSigned32(goodId) > 27 && NXBasics::Runtime_TimeSigned32(goodId) <= 29)) return true;
        }
        else if (goodId == 15 && Read<std::uint32_t>(house, 0x14) == 37) return true;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, goodId);
        if (slot == -1) return false;
        const auto offset = 0x328u + static_cast<std::uint32_t>(slot) * 4u;
        const auto value = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, offset) + static_cast<std::uint32_t>(delta));
        if (value < 0) return false;
        Write(house, offset, value > 1000 ? 1000 : value);
        return true;
    }
    // Original function: FUN_00434cb0; notify before storing the newly enabled flag.
    void House_SetDefenceModeFlag(void* house, bool enabled)
    {
        if (NC2Lifetime::Read<std::uint8_t>(house, 0x7D8) == 0 && enabled) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x18, true, NC2Lifetime::Read<std::uint32_t>(house, 0x0C), 3, 0);
        NC2Lifetime::Write(house, 0x7D8, static_cast<std::uint8_t>(enabled));
    }

    // Original function: FUN_0045bfec; Windows consists solely of RET 4.
    void House_DoNewUserCommand(void* /*house*/, const void* /*command*/) {}
    // Original function: FUN_00431cf1; the stock index is unchecked.
    std::uint32_t House_Stock_GetMaximumAmountByStockId(const void* house, std::uint32_t stock)
    {
        const auto mode = NC2Lifetime::Read<std::uint32_t>(gGameControl, 0x5D200);
        const auto multiplier = mode == 1 ? 2u : mode == 2 ? 5u : 1u;
        return NC2Lifetime::Read<std::uint32_t>(House_GetHouseTypeDataPtr(house), 0x68u + stock * 12u) * multiplier;
    }

    // Original function: FUN_00431fd3; only a signed upper bound is checked.
    void House_Stock_SetMinimumValue(void* house, std::uint32_t good, std::uint32_t minimum)
    {
        if (NXBasics::Runtime_TimeSigned32(good) >= 66) return;
        const auto stock = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (stock != -1) NC2Lifetime::Write(house, 0x538u + static_cast<std::uint32_t>(stock) * 4u, minimum);
    }

    // Original function: FUN_0043347c; the enhancer index is unchecked.
    bool House_HomeQualityEnhancer_SetAllowedFlag(void* house, std::uint32_t enhancer, bool allowed)
    {
        const auto offset = 0x780u + enhancer * 8u;
        if (NC2Lifetime::Read<std::uint8_t>(house, offset) == static_cast<std::uint8_t>(allowed)) return false;
        NC2Lifetime::Write(house, offset, static_cast<std::uint8_t>(allowed));
        return true;
    }

    // Original function: FUN_0043349a.
    void House_Production_SetStoppedFlag(void* house, bool stopped)
    {
        NC2Lifetime::Write(house, 0x798, static_cast<std::uint8_t>(stopped));
        if (!stopped && NC2Lifetime::Read<std::int32_t>(house, 0x79C) <= 0) NC2Lifetime::Write<std::uint32_t>(house, 0x79C, 1000000);
    }

    // Original function: FUN_0043352f; signed counter comparison.
    void House_l_Production_CheckProductionCounter(void* house)
    {
        if (NC2Lifetime::Read<std::int32_t>(house, 0x79C) > 0) return;
        NC2Lifetime::Write<std::uint32_t>(house, 0x79C, 0);
        NC2Lifetime::Write<std::uint8_t>(house, 0x798, 1);
    }

    // Original function: FUN_004334c0; the initial threshold comparison is unsigned.
    void House_Production_ChangeCounter(void* house, std::int32_t delta)
    {
        const auto counter = NC2Lifetime::Read<std::uint32_t>(house, 0x79C);
        if (counter > 10)
        {
            if (delta >= 0) return;
            NC2Lifetime::Write<std::uint32_t>(house, 0x79C, 10);
            House_l_Production_CheckProductionCounter(house);
            return;
        }
        NC2Lifetime::Write(house, 0x79C, counter + static_cast<std::uint32_t>(delta));
        House_l_Production_CheckProductionCounter(house);
        if (delta >= 0 && NC2Lifetime::Read<std::uint8_t>(house, 0x798) != 0) House_Production_SetStoppedFlag(house, false);
    }
    // Original function: FUN_00434345.
    void* House_Misc_WalkBlockArea_GetPtr(const void* house)
    {
        const void* type = NC2E2::Engine2d_EntryPoint_GetHouseDevelopNamePtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        return NC2Lifetime::Read<void*>(type, 0x20u + NC2Lifetime::Read<std::uint32_t>(house, 0x20) * 4u);
    }

    // Original function: FUN_004316cc.
    const SStaticHouseTypeData* House_GetHouseTypeDataPtr(const void* house)
    {
        return reinterpret_cast<const SStaticHouseTypeData*>(reinterpret_cast<std::uintptr_t>(gStaticHouseTypeData) + NC2Lifetime::Read<std::uint32_t>(house, 0x14) * 0x624u);
    }

    // Original function: FUN_004316dc.
    std::uint32_t House_GetHouseMainType(const void* house) { return static_cast<std::uint32_t>(House_GetHouseTypeDataPtr(house)->mainType); }

    // Original function: FUN_00431a88.
    float House_Stock_GetAmountOfGoodX(const void* house, std::uint32_t goodId, bool includeFutureStock)
    {
        using NC2Lifetime::Read;
        if (NXBasics::Runtime_TimeSigned32(goodId) >= 66) return 0.0f;
        const auto* type = House_GetHouseTypeDataPtr(house);
        if (!includeFutureStock)
        {
            if (type->mainType == 2)
            {
                if (goodId == 15) return static_cast<float>(Read<std::int32_t>(house, 0x794));
                if (goodId == 28) return static_cast<float>(Read<std::int32_t>(house, 0x78C));
                if (goodId == 29) return static_cast<float>(Read<std::int32_t>(house, 0x784));
            }
            else if (goodId == 15 && (Read<std::uint32_t>(house, 0x14) == 1 || Read<std::uint32_t>(house, 0x14) == 37)) return static_cast<float>(Read<std::int32_t>(house, 0x794));
        }
        const std::int32_t slot = House_l_Stock_GetStockSlotIdForGoodType(house, goodId);
        if (slot == -1) return 0.0f;
        const auto index = static_cast<std::uint32_t>(slot);
        if (type->mainType == 3 && includeFutureStock && Read<std::uint8_t>(type, 0x6Cu + index * 12u) != 0) return 0.0f;
        // Windows loads future stock unsigned, but FIADD reads current stock signed.
        // The future-stock term is included even when includeFutureStock is false.
        return static_cast<float>(static_cast<double>(Read<std::uint32_t>(house, 0x220u + index * 4u)) * static_cast<double>(0.1f) + static_cast<double>(Read<std::int32_t>(house, 0x118u + index * 4u)));
    }

    // Original function: FUN_00431f3c; CHouse::l_Stock_GetStockSlotIdForGoodType.
    std::int32_t House_l_Stock_GetStockSlotIdForGoodType(const void* house, std::uint32_t goodId)
    {
        const auto* type = House_GetHouseTypeDataPtr(house);
        const auto slot = NC2Lifetime::Read<std::int32_t>(type, 0x37Cu + goodId * 4u);
        if (slot != -1) return slot;
        if (goodId == 18 || goodId == 19 || goodId == 21 || goodId == 22) return type->stockIndices[16];
        if (goodId == 20) return type->stockIndices[17];
        return -1;
    }

    // Original function: FUN_00433064; signed division and unchecked denominator.
    std::int32_t House_Construction_GetPercentage(const void* house)
    {
        if (NC2Lifetime::Read<std::uint32_t>(house, 0x2C) == 3) return 100;
        return NXBasics::Runtime_TimeSigned32(NC2Lifetime::Read<std::uint32_t>(house, 0x30) * 100u) / NC2Lifetime::Read<std::int32_t>(house, 0x34);
    }
    // Original function: FUN_00431944; CHouse::Stock_GetAmountOfGoods.
    std::uint32_t House_Stock_GetAmountOfGoods(const void* house, std::uint32_t goodId, bool stockOnly)
    {
        using NC2Lifetime::Read;
        if (NXBasics::Runtime_TimeSigned32(goodId) >= 66) return 0;
        const auto typeId = Read<std::uint32_t>(house, 0x14);
        const auto* type = House_GetHouseTypeDataPtr(house);
        if (!stockOnly)
        {
            if (type->mainType == 2)
            {
                if (goodId == 15) return Read<std::uint32_t>(house, 0x794);
                if (goodId == 28) return Read<std::uint32_t>(house, 0x78C);
                if (goodId == 29) return Read<std::uint32_t>(house, 0x784);
            }
            else if (goodId == 15 && (typeId == 1 || typeId == 37)) return Read<std::uint32_t>(house, 0x794);
        }
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, goodId);
        if (slot == -1) return 0;
        const auto index = static_cast<std::uint32_t>(slot);
        if (type->mainType == 3 && stockOnly && Read<std::uint8_t>(type, 0x6Cu + index * 12u) != 0) return 0;
        return Read<std::uint32_t>(house, 0x118u + index * 4u);
    }
}
