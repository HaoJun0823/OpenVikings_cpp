#include "C2HouseAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2StaticHouseTypeData.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2HumanNeeds.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanCommandAccess.h"
#include "C2VehicleAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2CallbackManager.h"
#include "C2HumanInitialization.h"
namespace NC2Logic
{
    // Original function: FUN_004320ca; clear the mask before detaching human-side state.
    bool House_Inhabitants_DetachHuman(void* house, std::uint32_t humanId, std::uint32_t attachmentMask)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* slot = static_cast<std::byte*>(house) + 0x640u + index * 0x10u;
            if (Read<std::uint8_t>(slot, 0) == 0 || Read<std::uint32_t>(slot, 4) != humanId) continue;
            Write<std::uint8_t>(slot, 0, Read<std::uint8_t>(slot, 0) & ~attachmentMask);
            auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
            switch (attachmentMask)
            {
            case 1: Human_DetachFromHomeHouse(human); break;
            case 2: case 4: Human_DetachFromWorkHouse(human); break;
            case 8:
                Human_DetachFromWorkHouse(human);
                Write<std::uint32_t>(slot, 8, 0);
                Write<std::uint32_t>(slot, 12, 0);
                break;
            default: break;
            }
            if (Read<std::uint8_t>(slot, 0) == 0) Write<std::uint32_t>(slot, 4, 0xFFFFFFFFu);
            return true;
        }
        return false;
    }

    // Original function: FUN_00432dc1; future deliveries are added to current construction stock.
    bool House_Construction_IsNeedingFutureGood(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        for (std::int32_t i = 0; i < Read<std::int32_t>(house, 0x38); ++i)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(i) * 0x14u;
            if (Read<std::uint32_t>(house, offset) == good && Read<std::uint32_t>(house, offset + 4) != Read<std::uint32_t>(house, offset + 8) + Read<std::uint32_t>(house, offset + 12)) return true;
        }
        return false;
    }

    // Original function: FUN_00431e90.
    bool House_Stock_IsFullSoon(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(house, 0x2C) != 3) return !House_Construction_IsNeedingFutureGood(house, good);
        if (House_GetHouseMainType(house) == 2)
        {
            if (good == 29) return Read<std::uint8_t>(house, 0x780) == 0;
            if (good == 28) return Read<std::uint8_t>(house, 0x788) == 0;
            if (good == 15) return Read<std::uint8_t>(house, 0x790) == 0 || Read<std::uint32_t>(house, 0x20) < 2;
        }
        else if (good == 15 && (Read<std::uint32_t>(house, 0x14) == 1 || Read<std::uint32_t>(house, 0x14) == 37)) return false;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        return slot == -1 || Read<std::uint32_t>(house, 0x328u + static_cast<std::uint32_t>(slot) * 4u) >= House_Stock_GetMaximumAmountByStockId(house, static_cast<std::uint32_t>(slot));
    }

    // Original function: FUN_00431879; subtraction wraps as an unsigned Windows DWORD.
    std::uint32_t House_Stock_GetDemandForGood(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        const auto type = Read<std::uint32_t>(house, 0x14);
        const auto* data = House_GetHouseTypeDataPtr(house);
        if (data->mainType == 2)
        {
            if (good == 29) return Read<std::uint8_t>(house, 0x780) == 0 ? 0u : 100u - Read<std::uint32_t>(house, 0x784);
            if (good == 28) return Read<std::uint8_t>(house, 0x788) == 0 ? 0u : 100u - Read<std::uint32_t>(house, 0x78C);
            if (good == 15) return Read<std::uint8_t>(house, 0x790) == 0 || Read<std::uint32_t>(house, 0x20) < 2 ? 0u : 3000u - Read<std::uint32_t>(house, 0x794);
        }
        else if (good == 15 && type == 37) return 1u - Read<std::uint32_t>(house, 0x794);
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (slot == -1 || (data->mainType == 3 && type != 12 && type != 34 && data->stock[slot].unknown08 == 0)) return 0;
        return House_Stock_GetMaximumAmountByStockId(house, static_cast<std::uint32_t>(slot)) - Read<std::uint32_t>(house, 0x118u + static_cast<std::uint32_t>(slot) * 4u);
    }

    // Original function: FUN_004330bb; scan all construction interaction rows.
    bool House_Construction_GetNextWorkPoint(const void* house, const void* source, void* point, bool avoidOtherBuilders, std::uint32_t humanId, std::uint32_t continent)
    {
        using namespace NC2Lifetime;
        const auto stage = Read<std::uint32_t>(house, 0x20) + (Read<std::uint32_t>(house, 0x2C) == 2 ? 1u : 0u);
        const auto* definition = NC2E2::Engine2d_EntryPoint_GetHouseDevelopNamePtr(Read<std::uint32_t>(house, 0x10));
        const auto* area = Read<const CMapArea*>(definition, 0x34u + stage * 4u);
        if (area == nullptr) return false;
        std::uint32_t best = 1000000;
        for (std::uint32_t row = 0; row < area->rowCount; ++row)
        {
            std::uint32_t candidate;
            MapMigPoint_ConstructOffset(&candidate, static_cast<const std::byte*>(house) + 0x1C, area->rows[row].x, area->rows[row].y, false);
            for (std::uint32_t x = 0; x < area->rows[row].width; ++x)
            {
                if (MapMigPoint_IsInsideMap(&candidate))
                {
                    const auto* micro = GameMap_GetConstMapMicroPointPtr(&candidate);
                    if ((Read<std::uint8_t>(micro, 12) & 0x80u) == 0 && Read<std::uint8_t>(micro, 6) == continent)
                    {
                        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(&candidate, 0), Read<std::int16_t>(&candidate, 2)));
                        if (distance < best)
                        {
                            bool occupied = false;
                            if (avoidOtherBuilders)
                            {
                                for (std::uint32_t slot = 0; slot < 20; ++slot)
                                {
                                    const auto offset = 0x640u + slot * 0x10u;
                                    if ((Read<std::uint8_t>(house, offset) & 4u) == 0) continue;
                                    const auto id = Read<std::uint32_t>(house, offset + 4);
                                    if (id == humanId) continue;
                                    const auto* other = CHumanArray::mStaticVars + id * CHumanArray::ElementSize;
                                    if (Read<std::uint32_t>(Human_GetMapPositionRef(other), 0) == candidate && Read<std::uint32_t>(other, 0xAB8) != 1) { occupied = true; break; }
                                }
                            }
                            if (!occupied) { Write(point, 0, candidate); best = distance; }
                        }
                    }
                }
                Write<std::uint16_t>(&candidate, 0, Read<std::uint16_t>(&candidate, 0) + 1u);
            }
        }
        return best != 1000000;
    }

    // Original function: FUN_00434148; overlay counters are measured in twelve-tick intervals.
    void House_ActionOverlay_Start(void* house, std::int32_t overlay, std::uint32_t duration)
    {
        using namespace NC2Lifetime;
        if (overlay >= 5) return;
        const auto offset = 0x7A8u + static_cast<std::uint32_t>(overlay) * 4u;
        const auto counter = (duration + 11u) / 12u;
        const auto previous = Read<std::uint32_t>(house, offset);
        if (previous != 0) { if (counter > previous) Write(house, offset, counter); return; }
        Write(house, offset, counter);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1C, true, Read<std::uint32_t>(house, 4), static_cast<std::uint32_t>(overlay), 1);
    }

    // Original function: FUN_00433515; counters above ten represent unlimited production.
    void House_Production_DidProduceOneGood(void* house)
    {
        using namespace NC2Lifetime;
        const auto previous = Read<std::uint32_t>(house, 0x79C);
        if (previous > 10) return;
        Write(house, 0x79C, previous - 1u);
        House_l_Production_CheckProductionCounter(house);
    }
    // Original function: FUN_00432d92.
    bool House_Construction_IsNeedingGood(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        for (std::int32_t i = 0; i < Read<std::int32_t>(house, 0x38); ++i)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(i) * 0x14u;
            if (Read<std::uint32_t>(house, offset) == good && Read<std::uint32_t>(house, offset + 4) != Read<std::uint32_t>(house, offset + 8)) return true;
        }
        return false;
    }

    // Original function: FUN_00431de4.
    bool House_Stock_IsFull(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(house, 0x2C) != 3) return !House_Construction_IsNeedingGood(house, good);
        if (House_GetHouseMainType(house) == 2)
        {
            if (good == 29) return Read<std::uint8_t>(house, 0x780) == 0;
            if (good == 28) return Read<std::uint8_t>(house, 0x788) == 0;
            if (good == 15) return Read<std::uint8_t>(house, 0x790) == 0 || Read<std::uint32_t>(house, 0x20) < 2;
        }
        else if (good == 15 && (Read<std::uint32_t>(house, 0x14) == 1 || Read<std::uint32_t>(house, 0x14) == 37)) return false;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        return slot == -1 || Read<std::uint32_t>(house, 0x118u + static_cast<std::uint32_t>(slot) * 4u) >= House_Stock_GetMaximumAmountByStockId(house, static_cast<std::uint32_t>(slot));
    }

    // Original function: FUN_0043179d; enhancer thresholds and demand flags are Windows-specific.
    bool House_Stock_HasDemandForGood(const void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        const auto type = Read<std::uint32_t>(house, 0x14);
        const auto* data = House_GetHouseTypeDataPtr(house);
        if (data->mainType == 2)
        {
            if (good == 29) return Read<std::uint8_t>(house, 0x780) != 0 && Read<std::int32_t>(house, 0x784) < 100;
            if (good == 28) return Read<std::uint8_t>(house, 0x788) != 0 && Read<std::int32_t>(house, 0x78C) < 100;
            if (good == 15) return Read<std::uint8_t>(house, 0x790) != 0 && Read<std::uint32_t>(house, 0x20) >= 2 && Read<std::int32_t>(house, 0x794) < 3000;
        }
        else if (good == 15 && type == 37) return Read<std::int32_t>(house, 0x794) <= 0;
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (slot == -1) return false;
        if (data->mainType == 3 && type != 12 && type != 34 && data->stock[slot].unknown08 == 0) return false;
        return Read<std::uint32_t>(house, 0x118u + static_cast<std::uint32_t>(slot) * 4u) < House_Stock_GetMaximumAmountByStockId(house, static_cast<std::uint32_t>(slot));
    }

    // Original function: FUN_0043324d; inTemple/humanId are unused in Windows.
    bool House_Religion_GetNextPrayPoint(const void* house, const void* source, void* point, bool /*inTemple*/, std::uint32_t /*humanId*/, std::uint32_t continent)
    {
        using namespace NC2Lifetime;
        const auto stage = Read<std::uint32_t>(house, 0x20) + (Read<std::uint32_t>(house, 0x2C) == 2 ? 1u : 0u);
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDevelopNamePtr(Read<std::uint32_t>(house, 0x10));
        const auto* area = Read<const CMapArea*>(definition, 0x34u + stage * 4u);
        if (area == nullptr) return false;
        std::uint32_t best = 1000000;
        for (std::uint32_t row = 0; row < area->rowCount; ++row)
        {
            std::uint32_t candidate;
            MapMigPoint_ConstructOffset(&candidate, static_cast<const std::byte*>(house) + 0x1C, area->rows[row].x, area->rows[row].y, false);
            for (std::uint32_t x = 0; x < area->rows[row].width; ++x)
            {
                if (MapMigPoint_IsInsideMap(&candidate))
                {
                    const void* micro = GameMap_GetConstMapMicroPointPtr(&candidate);
                    if ((Read<std::uint8_t>(micro, 0x0C) & 0x80u) == 0 && Read<std::uint8_t>(micro, 6) == continent)
                    {
                        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(&candidate, 0), Read<std::int16_t>(&candidate, 2)));
                        if (distance < best) { Write(point, 0, candidate); best = distance; }
                    }
                }
                Write<std::uint16_t>(&candidate, 0, Read<std::uint16_t>(&candidate, 0) + 1u);
            }
        }
        return best != 1000000;
    }

    // Original function: FUN_004333e6; complete three-enhancer value region.
    bool House_HomeQualityEnhancer_ModifyValue(void* house, std::uint32_t enhancer, std::int32_t direction)
    {
        using namespace NC2Lifetime;
        if (enhancer > 2) return true;
        const auto offset = 0x784u + enhancer * 8u;
        const auto previous = Read<std::uint32_t>(house, offset);
        if (direction > 0)
        {
            const auto maximum = enhancer == 2 ? 5000 : 500;
            auto value = NXBasics::Runtime_TimeSigned32(previous + (enhancer == 2 ? 1000u : 100u));
            if (value > maximum) value = maximum;
            Write(house, offset, value);
            return true;
        }
        const auto amount = enhancer == 2 ? 3u : 5u;
        const auto value = NXBasics::Runtime_TimeSigned32(previous - amount);
        Write(house, offset, value);
        if (value >= 0) return true;
        Write(house, offset, previous);
        return false;
    }

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
    // Original function: FUN_00434274; Windows looks up the definition twice.
    void House_Misc_Blocks_SetToMap(const void* house)
    {
        const auto* type = NC2E2::Engine2d_EntryPoint_GetHouseDevelopNamePtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        NC2Lifetime::Read<const CMapArea*>(type, 0x20u + NC2Lifetime::Read<std::uint32_t>(house, 0x20) * 4u)->WalkBlock_Set(static_cast<const std::byte*>(house) + 0x1C, true);
        type = NC2E2::Engine2d_EntryPoint_GetHouseDevelopNamePtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        NC2Lifetime::Read<const CMapArea*>(type, 0x1C)->BuildBlock_Set(static_cast<const std::byte*>(house) + 0x1C, true);
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

    // Original function: FUN_004319d7; a read also updates unmet-demand counters.
    std::uint32_t House_Stock_RequestAmountOfGoods(void* house, std::uint32_t goodId, bool stockOnly)
    {
        using namespace NC2Lifetime;
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
        const auto amount = Read<std::uint32_t>(house, 0x118u + index * 4u);
        const auto offset = 0x430u + index * 4u;
        if (amount == 0) Write(house, offset, Read<std::uint32_t>(house, offset) + 1u);
        else
        {
            const auto demand = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(house, offset) - 10u);
            Write<std::int32_t>(house, offset, demand < 0 ? 0 : demand);
        }
        return amount;
    }

    // Original function: FUN_00432cc8.
    bool House_l_Construction_PutGoodIntoStock(void* house, std::uint32_t good)
    {
        using namespace NC2Lifetime;
        for (std::int32_t index = 0; index < Read<std::int32_t>(house, 0x38); ++index)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(index) * 0x14u;
            if (Read<std::uint32_t>(house, offset) != good || Read<std::uint32_t>(house, offset + 8) == Read<std::uint32_t>(house, offset + 4)) continue;
            Write(house, offset + 8, Read<std::uint32_t>(house, offset + 8) + 1u);
            Write(house, 0x108, Read<std::uint32_t>(house, 0x108) + 1u);
            const auto future = Read<std::uint32_t>(house, offset + 12);
            if (future != 0) Write(house, offset + 12, future - 1u);
            return true;
        }
        return false;
    }

    // Original function: FUN_00431b42; the outlined three-parameter overload.
    bool __fastcall House_Stock_ModifyAmount(void* house, void* /*unusedEdx*/, unsigned int good, unsigned int amount, bool construction)
    {
        return House_Stock_ModifyAmount(house, good, NXBasics::Runtime_TimeSigned32(amount), 0, construction);
    }

    // Original function: FUN_00431b58; fractional units have base ten, with a 1000-unit cap.
    bool House_Stock_ModifyAmount(void* house, std::uint32_t good, std::int32_t amount, std::int32_t fraction, bool construction)
    {
        using namespace NC2Lifetime;
        if (construction && Read<std::uint32_t>(house, 0x2C) != 3 && amount == 1 && fraction == 0 && House_l_Construction_PutGoodIntoStock(house, good)) return true;
        const auto typeId = Read<std::uint32_t>(house, 0x14);
        const auto* type = House_GetHouseTypeDataPtr(house);
        if (type->mainType == 2)
        {
            if (good == 15 || good == 28 || good == 29) return House_HomeQualityEnhancer_ModifyValue(house, good == 15 ? 2u : good == 28 ? 1u : 0u, amount);
            if (amount > 0 && TGoodTypes_IsFood(good) && House_HomeQualityEnhancer_ModifyValue(house, 1, -1)) amount = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(amount) + 1u);
        }
        else if (good == 15 && typeId == 37) return House_HomeQualityEnhancer_ModifyValue(house, 2, amount);
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (slot == -1) return false;
        const auto index = static_cast<std::uint32_t>(slot);
        auto remainder = Read<std::uint32_t>(house, 0x220u + index * 4u) + static_cast<std::uint32_t>(fraction);
        auto total = Read<std::uint32_t>(house, 0x118u + index * 4u) + static_cast<std::uint32_t>(amount);
        if (NXBasics::Runtime_TimeSigned32(remainder) >= 10)
        {
            const auto whole = remainder / 10u;
            remainder -= whole * 10u;
            total += whole;
        }
        if (NXBasics::Runtime_TimeSigned32(remainder) < 0)
        {
            const auto borrowed = (9u - remainder) / 10u;
            remainder += borrowed * 10u;
            total -= borrowed;
        }
        if (NXBasics::Runtime_TimeSigned32(total) >= 1000) { total = 1000; remainder = 0; }
        if (NXBasics::Runtime_TimeSigned32(total) < 0) return false;
        Write(house, 0x118u + index * 4u, total);
        Write(house, 0x220u + index * 4u, remainder);
        return true;
    }
}
