#include "C2HouseAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2StaticHouseTypeData.h"
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
#include "C2HumanJobAccess.h"
#include "C2HouseUserControl.h"
#include "C2GameEngineStartup.h"
#include "C2HumanGoods.h"
namespace NC2Logic
{
    // Original function: FUN_004322b6; homes do not expose worker attachments.
    bool House_Inhabitants_IsHumanAttachedAsWorker(const void* house, std::uint32_t human)
    {
        if (House_GetHouseMainType(house) == 2) return false;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(const_cast<void*>(house), human);
        return slot != nullptr && (NC2Lifetime::Read<std::uint8_t>(slot, 0) & 2u) != 0;
    }

    // Original function: FUN_0043264f; Windows-only outlined door reference.
    const void* House_Door_GetEntryPointRef(const void* house) { return static_cast<const std::byte*>(house) + 0x24; }

    // Original function: FUN_00432740; the native slot address is unchecked.
    void* House_Inhabitants_GetAttachedHumanPtr(void* house, std::uint32_t slot)
    {
        const auto* attachment = House_Inhabitants_GetAttachedHumanSlotPtrBySlotId(house, slot);
        if (attachment == nullptr) return nullptr;
        return CHumanArray::mStaticVars + NC2Lifetime::Read<std::uint32_t>(attachment, 4) * CHumanArray::ElementSize;
    }

    // Original function: FUN_0043322f; the fire counter comparison is signed.
    bool House_Religion_IsHolyFireActive(const void* house)
    {
        return NC2Lifetime::Read<std::uint8_t>(house, 0x7D0) == 0 && NC2Lifetime::Read<std::uint32_t>(house, 0x2C) == 3 && NC2Lifetime::Read<std::int32_t>(house, 0x794) > 0;
    }

    // Original function: FUN_00433a95; Windows-only outlined owner getter.
    std::uint32_t House_GetPlayerId(const void* house) { return NC2Lifetime::Read<std::uint32_t>(house, 0x0C); }
    // Original function: FUN_00433a99; Windows-only outlined map-position reference.
    const void* House_GetMapPositionRef(const void* house) { return static_cast<const std::byte*>(house) + 0x1C; }

    // Original function: FUN_00434259; native invalid overlay indices are ignored.
    void House_LandscapeOverlay_SetActive(void* house, std::int32_t overlay, bool enabled)
    {
        if (overlay >= 0 && overlay < 10) NC2Lifetime::Write<std::uint8_t>(house, 0x7BCu + static_cast<std::uint32_t>(overlay), enabled);
    }

    // Original function: FUN_004345f2; Windows-only outlined sector-list predicate.
    bool House_IsMilitarySectorType(std::uint32_t type) { return type == 40 || type == 41 || type == 39 || type == 1; }

    // Original function: FUN_004230b4; x86 masks the shift count to five bits.
    void House_SetBehaviourFlag(void* house, std::uint32_t bit, bool enabled)
    {
        const auto mask = 1u << (bit & 31u);
        const auto flags = NC2Lifetime::Read<std::uint32_t>(house, 0x7DC);
        NC2Lifetime::Write(house, 0x7DC, enabled ? flags | mask : flags & ~mask);
    }

    // Original function: FUN_00434c1a; the engine type supplies the repair threshold.
    bool House_HitPoints_IsDamaged(const void* house)
    {
        const auto* data = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        return NC2Lifetime::Read<std::int32_t>(house, 0x7C8) < NC2Lifetime::Read<std::int32_t>(data, 0x19Cu + NC2Lifetime::Read<std::uint32_t>(house, 0x20) * 4u);
    }

    // Original function: FUN_004341c0; the Windows bound comparison is signed.
    bool House_ActionOverlay_IsActive(const void* house, std::uint32_t overlay) { return NXBasics::Runtime_TimeSigned32(overlay) < 5 && NC2Lifetime::Read<std::uint32_t>(house, 0x7A8u + overlay * 4u) != 0; }
    // Original function: FUN_0043423e.
    bool House_LandscapeOverlay_IsActive(const void* house, std::int32_t overlay) { return overlay >= 0 && overlay < 10 && NC2Lifetime::Read<std::uint8_t>(house, 0x7BCu + static_cast<std::uint32_t>(overlay)) != 0; }

    // Original function: FUN_00432289; only an exact worker role is refreshed.
    void House_Inhabitants_UpdateAllWorkers(void* house)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            const auto offset = 0x640u + slot * 0x10u;
            if (Read<std::uint8_t>(house, offset) == 2) Human_Production_Update(CHumanArray::mStaticVars + Read<std::uint32_t>(house, offset + 4) * CHumanArray::ElementSize);
        }
    }

    // Original function: FUN_0043290f.
    bool __fastcall House_Construction_UpgradeStart(void* house, void* unusedEdx)
    {
        (void)unusedEdx;
        if (!House_Construction_CanBeUpgraded(house)) return false;
        House_l_Construction_InitConstruction(house, NC2Lifetime::Read<std::uint32_t>(house, 0x20) + 1u, true);
        House_Inhabitants_UpdateAllWorkers(house);
        House_SetDefenceModeFlag(house, false);
        return true;
    }

    // Original function: FUN_0043293f; leftover delivery stock is returned inside one map-modification bracket.
    bool __fastcall House_Construction_UpgradeStop(void* house, void* unusedEdx)
    {
        using namespace NC2Lifetime;
        (void)unusedEdx;
        if (Read<std::uint32_t>(house, 0x2C) != 2) return false;
        GameMapModifyManager_StartGameMapModify();
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            const auto amount = Read<std::uint32_t>(house, offset + 8) - Read<std::uint32_t>(house, offset + 16);
            const auto good = Read<std::uint32_t>(house, offset);
            // Outlined getter 0043264F and copy 0040AD55 return the by-value door point at +0x24.
            if (amount != 0) Map_GoodTool_PutOnMap(good, amount, Read<std::uint32_t>(house, 0x24), 10, false);
        }
        GameMapModifyManager_StopGameMapModify();
        Write<std::uint32_t>(house, 0x2C, 3);
        House_Inhabitants_UpdateAllWorkers(house);
        return true;
    }

    // Original function: FUN_00432a14; the completed development stage remains unchanged during an upgrade.
    void House_l_Construction_InitConstruction(void* house, std::uint32_t developmentStage, bool upgrade)
    {
        using namespace NC2Lifetime;
        Write<std::uint32_t>(house, 0x2C, upgrade ? 2u : 1u);
        House_Construction_CalculateGoodsNeeded(Read<std::uint32_t>(house, 0x10), developmentStage, upgrade, reinterpret_cast<SHouseConstructionGoods*>(static_cast<std::byte*>(house) + 0x38));
        House_l_Construction_TransferFutureStock(house);
        House_l_Construction_UpdateGoodsBeingBrought(house);
        const auto steps = Read<std::uint32_t>(house, 0x104) * 30u;
        Write<std::uint32_t>(house, 0x30, 0);
        Write<std::uint32_t>(house, 0x110, 0);
        Write(house, 0x34, steps);
    }

    // Original function: FUN_00432b32; stock quantities use the native signed clamp after DWORD arithmetic.
    void House_l_Construction_TransferFutureStock(void* house)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(house, 0x2C) == 3)
        {
            for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
            {
                const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
                const auto reserved = Read<std::uint32_t>(house, offset + 12);
                if (reserved == 0) continue;
                const auto stock = House_l_Stock_GetStockSlotIdForGoodType(house, Read<std::uint32_t>(house, offset));
                if (stock == -1) continue;
                const auto stockOffset = 0x328u + static_cast<std::uint32_t>(stock) * 4u;
                auto amount = static_cast<std::int32_t>(Read<std::uint32_t>(house, stockOffset) - reserved);
                if (amount < 0) amount = 0;
                else if (amount > 1000) amount = 1000;
                Write<std::uint32_t>(house, offset + 12, 0);
                Write(house, stockOffset, amount);
            }
            return;
        }
        for (std::int32_t slot = 0; slot < Read<std::int32_t>(house, 0x38); ++slot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(slot) * 20u;
            const auto stock = House_l_Stock_GetStockSlotIdForGoodType(house, Read<std::uint32_t>(house, offset));
            if (stock == -1) continue;
            const auto stockOffset = 0x328u + static_cast<std::uint32_t>(stock) * 4u;
            const auto reserved = Read<std::uint32_t>(house, offset + 12);
            auto amount = static_cast<std::int32_t>(Read<std::uint32_t>(house, stockOffset) - Read<std::uint32_t>(house, 0x118u + static_cast<std::uint32_t>(stock) * 4u) + reserved);
            if (amount < 0) amount = 0;
            else if (amount > 1000) amount = 1000;
            Write(house, offset + 12, amount);
            Write(house, stockOffset, Read<std::uint32_t>(house, stockOffset) - (static_cast<std::uint32_t>(amount) - reserved));
        }
    }

    // Original function: FUN_00432bf8; delivery states 5 and off-site state 4 terminate the current good's scan.
    void House_l_Construction_UpdateGoodsBeingBrought(void* house)
    {
        using namespace NC2Lifetime;
        for (std::int32_t goodSlot = 0; goodSlot < Read<std::int32_t>(house, 0x38); ++goodSlot)
        {
            const auto offset = 0x3Cu + static_cast<std::uint32_t>(goodSlot) * 20u;
            std::uint32_t amount = 0;
            for (std::uint32_t slot = 0; slot < 20; ++slot)
            {
                const auto role = Read<std::uint8_t>(house, 0x640u + slot * 16u);
                if (role == 0 || role == 1) continue;
                const void* human = CHumanArray::mStaticVars + Read<std::uint32_t>(house, 0x644u + slot * 16u) * CHumanArray::ElementSize;
                if (Read<std::uint8_t>(human, 0) == 0) continue;
                const auto state = Read<std::uint32_t>(human, 0xACC);
                if (state == 0 || Read<std::uint32_t>(human, 0xAD0) != Read<std::uint32_t>(house, offset)) continue;
                if (state == 5) break;
                if (state == 4 && MapMigPoint_IsInsideMap(Human_GetMapPositionRef(human)))
                {
                    const auto* point = Human_GetMapPositionRef(human);
                    if (point[0] != Read<std::uint16_t>(house, 0x1C) || point[1] != Read<std::uint16_t>(house, 0x1E)) break;
                }
                ++amount;
            }
            if (amount != Read<std::uint32_t>(house, offset + 12)) Write(house, offset + 12, amount);
        }
    }

    // Original function: FUN_00431f85.
    std::uint32_t House_Stock_GetGoodRequestCounter(const void* house, std::uint32_t good) { if (static_cast<std::int32_t>(good) >= 66) return 0; const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house,good); return slot == -1 ? 0 : NC2Lifetime::Read<std::uint32_t>(house,0x430u + static_cast<std::uint32_t>(slot) * 4u); }
    // Original function: FUN_00431fac.
    std::uint32_t House_Stock_GetMinimumValue(const void* house, std::uint32_t good) { if (static_cast<std::int32_t>(good) >= 66) return 0; const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house,good); return slot == -1 ? 0 : NC2Lifetime::Read<std::uint32_t>(house,0x538u + static_cast<std::uint32_t>(slot) * 4u); }
    // Original function: FUN_00441228; food categories resolve to the first usable stock.
    std::uint32_t Tool_House_GetAmountOfGoods(const void* house, std::uint32_t* good, bool stockOnly)
    {
        const auto food = [](std::uint32_t type) { return type == 16 || type == 17 || type == 18 || type == 19 || type == 20 || type == 21 || type == 22 || type == 23 || type == 56; };
        if (!food(*good)) return House_Stock_CanStoreGoodType(house, *good, stockOnly) ? House_Stock_GetAmountOfGoods(house, *good, stockOnly) : 0;
        const bool drink = *good == 17 || *good == 20;
        for (std::uint32_t candidate = 16; candidate < 24; ++candidate)
        {
            const bool sameCategory = drink ? candidate == 17 || candidate == 20 : candidate == 16 || candidate == 18 || candidate == 19 || candidate == 21 || candidate == 22 || candidate == 23 || candidate == 56;
            if (sameCategory && House_Stock_CanStoreGoodType(house, candidate, stockOnly)) { *good = candidate; return House_Stock_GetAmountOfGoods(house, candidate, stockOnly); }
        }
        return 0;
    }
    // Original function: FUN_00432a64; aggregate repeated goods without changing first-seen order.
    void House_Construction_CalculateGoodsNeeded(std::uint32_t engineType, std::uint32_t developmentStage, bool onlyCurrentStage, SHouseConstructionGoods* goods)
    {
        const auto* names = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType);
        std::memset(goods, 0, sizeof(*goods));
        const auto first = onlyCurrentStage ? developmentStage : 0u;
        if (first > developmentStage) return;
        auto stage = first;
        do
        {
            const auto count = NC2Lifetime::Read<std::uint32_t>(names, 0x48u + stage * 0x40u);
            for (std::uint32_t index = 0; index < count; ++index)
            {
                const auto good = NC2Lifetime::Read<std::uint32_t>(names, 0x4Cu + stage * 0x40u + index * 4u);
                std::int32_t slot = 0;
                while (slot < goods->count && goods->entries[slot].good != good) ++slot;
                if (slot < goods->count) { ++goods->entries[slot].amount; ++goods->total; }
                else if (goods->count != 10)
                {
                    goods->entries[goods->count].good = good;
                    goods->entries[goods->count].amount = 1;
                    ++goods->total; ++goods->count;
                }
            }
        } while (stage++ != developmentStage);
    }
    // Original function: FUN_004322e6; shared by initialization and school commands.
    bool House_Inhabitants_IsHumanAttachedAsStudent(const void* house, std::uint32_t human)
    {
        if (House_GetHouseMainType(house) != 4) return false;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(const_cast<void*>(house), human);
        return slot != nullptr && (NC2Lifetime::Read<std::uint8_t>(slot, 0) & 8u) != 0;
    }

    // Original function: FUN_00432653.
    void House_Inhabitants_SetStudentJob(void* house, std::uint32_t human, std::uint32_t job)
    {
        if (auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, human)) NC2Lifetime::Write(slot, 8, job);
    }

    // Original function: FUN_0043266a.
    void House_Inhabitants_SetStudentGood(void* house, std::uint32_t human, std::uint32_t good)
    {
        if (auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, human)) NC2Lifetime::Write(slot, 12, good);
    }
    // Original function: FUN_00431ffa; slots retain their training fields when reused.
    bool House_Inhabitants_AttachHuman(void* house, std::uint32_t human, std::uint32_t mask)
    {
        using namespace NC2Lifetime;
        if (!House_Inhabitants_CanHumanBeAttached(house, human, mask)) return false;
        if (auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, human))
        {
            Write<std::uint8_t>(slot, 0, Read<std::uint8_t>(slot, 0) | mask);
            return true;
        }
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* slot = static_cast<std::byte*>(house) + 0x640u + index * 16u;
            if (Read<std::uint8_t>(slot, 0) != 0) continue;
            Write(slot, 4, human);
            Write<std::uint8_t>(slot, 0, static_cast<std::uint8_t>(mask));
            return true;
        }
        if (mask != 1) return false;
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* slot = static_cast<std::byte*>(house) + 0x640u + index * 16u;
            if (Read<std::uint8_t>(slot, 0) != 4) continue;
            Write<std::uint8_t>(slot, 0, 0);
            Human_DetachFromWorkHouse(CHumanArray::mStaticVars + Read<std::uint32_t>(slot, 4) * CHumanArray::ElementSize);
            Write(slot, 4, human);
            Write<std::uint8_t>(slot, 0, 1);
            return true;
        }
        return false;
    }

    // Original function: FUN_00432316; combined attachment masks are not accepted here.
    bool House_Inhabitants_CanHumanBeAttached(void* house, std::uint32_t human, std::uint32_t mask)
    {
        switch (mask)
        {
        case 1: return House_Inhabitants_CanHumanBeAttachedAsHomie(house, human);
        case 2: return House_Inhabitants_CanHumanBeAttachedAsWorker(house, human);
        case 4: return House_Inhabitants_CanHumanBeAttachedAsBuilder(house, human);
        case 8: return House_Inhabitants_CanHumanBeAttachedAsStudent(house, human);
        default: return false;
        }
    }

    // Original function: FUN_00432359; relatives share a home independently of its family limit.
    bool House_Inhabitants_CanHumanBeAttachedAsHomie(void* house, std::uint32_t humanId)
    {
        using namespace NC2Lifetime;
        if (!House_l_Inhabitants_CanHumanBeAttached_BaseCheck(house, humanId) || House_GetHouseMainType(house) != 2) return false;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, humanId);
        if (slot != nullptr && (Read<std::uint8_t>(slot, 0) & 1u) != 0) return true;
        const auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
        for (const auto offset : {0xB14u, 0xB18u, 0xB24u, 0xB20u})
        {
            const auto relative = Read<std::uint32_t>(human, offset);
            if (relative != 0xFFFFFFFFu && Read<std::uint32_t>(CHumanArray::mStaticVars + relative * CHumanArray::ElementSize, 0xB28) == Read<std::uint32_t>(house, 4)) return true;
        }
        return House_Inhabitants_GetNumberOfFamiliesLivingInHome(house, nullptr) < static_cast<std::uint32_t>(House_GetHouseTypeDataPtr(house)->homeSize);
    }

    // Original function: FUN_00432439; queued job changes participate in worker capacity checks.
    bool House_Inhabitants_CanHumanBeAttachedAsWorker(void* house, std::uint32_t humanId)
    {
        using namespace NC2Lifetime;
        if (!House_l_Inhabitants_CanHumanBeAttached_BaseCheck(house, humanId)) return false;
        const auto* data = House_GetHouseTypeDataPtr(house);
        if (data->mainType == 2) return false;
        const auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, humanId);
        const auto attached = slot != nullptr && (Read<std::uint8_t>(slot, 0) & 2u) != 0;
        const auto job = Human_UserControl_GetNextJobType(human);
        std::int32_t worker = 0;
        while (worker < data->workerCount && static_cast<std::uint32_t>(data->workers[worker].jobType) != job) ++worker;
        if (worker == data->workerCount) return false;
        if (!attached && House_Inhabitants_GetNumberOfAttachedWorkersOfPassedJob(house, job) >= data->workers[worker].unknown04) return false;
        if (job == 30 && Read<std::uint32_t>(house, 0x14) == 37) return true;
        const auto good = Read<std::uint32_t>(human, 0x1C);
        if (good == 0 || good == 65) return true;
        const auto produced = Tool_Human_GetReallyProducedGoodType(Read<std::uint32_t>(human, 0x18), good);
        return House_Production_IsAbleToProduceGood(house, produced, true) || House_Production_IsAbleToProduceGood(house, produced, false);
    }

    // Original function: FUN_00432523.
    bool House_Inhabitants_CanHumanBeAttachedAsStudent(void* house, std::uint32_t human)
    {
        using namespace NC2Lifetime;
        if (!House_l_Inhabitants_CanHumanBeAttached_BaseCheck(house, human) || House_GetHouseMainType(house) != 4) return false;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, human);
        if (slot != nullptr && (Read<std::uint8_t>(slot, 0) & 8u) != 0) return true;
        return House_Inhabitants_GetNumberOfAttachedHumans(house, 8) < static_cast<std::uint32_t>(House_GetHouseTypeDataPtr(house)->schoolSize);
    }

    // Original function: FUN_00432578; completed undamaged houses reject builders.
    bool House_Inhabitants_CanHumanBeAttachedAsBuilder(void* house, std::uint32_t human)
    {
        using namespace NC2Lifetime;
        if (!House_l_Inhabitants_CanHumanBeAttached_BaseCheck(house, human) || Human_UserControl_GetNextJobType(CHumanArray::mStaticVars + human * CHumanArray::ElementSize) != 7) return false;
        if (Read<std::uint32_t>(house, 0x2C) == 3 && House_Repair_IsFinished(house)) return false;
        const auto* slot = House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(house, human);
        if (slot != nullptr && (Read<std::uint8_t>(slot, 0) & 4u) != 0) return true;
        return static_cast<std::int32_t>(House_Inhabitants_GetNumberOfAttachedHumans(house, 4)) < House_Construction_GetTotalNumberOfBuildersAllowed(house);
    }

    // Original function: FUN_004325e8; only attached moveables require a continent match.
    bool House_l_Inhabitants_CanHumanBeAttached_BaseCheck(const void* house, std::uint32_t humanId)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(house, 0x7D0) != 0) return false;
        const auto* human = CHumanArray::mStaticVars + humanId * CHumanArray::ElementSize;
        if (Read<std::uint32_t>(human, 0x10) != Read<std::uint32_t>(house, 12)) return false;
        const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 12) * 0x58u;
        return Read<std::uint8_t>(moveable, 0x10) == 0 || Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human)), 6) == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(house) + 0x1C), 6);
    }

    // Original function: FUN_004326e5.
    std::uint32_t House_Inhabitants_GetNumberOfAttachedHumans(const void* house)
    {
        std::uint32_t count = 0;
        for (std::uint32_t slot = 0; slot < 20; ++slot) if (NC2Lifetime::Read<std::uint8_t>(house, 0x640u + slot * 16u) != 0) ++count;
        return count;
    }

    // Original function: FUN_004326fd.
    std::uint32_t House_Inhabitants_GetNumberOfAttachedHumans(const void* house, std::uint32_t mask)
    {
        using namespace NC2Lifetime;
        std::uint32_t count = 0;
        for (std::uint32_t slot = 0; slot < 20; ++slot) if ((Read<std::uint8_t>(house, 0x640u + slot * 16u) & mask) != 0) ++count;
        return count;
    }

    // Original function: FUN_00432720; any builder bit excludes a slot from this count.
    std::uint32_t House_Inhabitants_GetNumberOfAttachedHumans_IgnoreBuilders(const void* house)
    {
        using namespace NC2Lifetime;
        std::uint32_t count = 0;
        for (std::uint32_t slot = 0; slot < 20; ++slot) { const auto mask = Read<std::uint8_t>(house, 0x640u + slot * 16u); if (mask != 0 && (mask & 4u) == 0) ++count; }
        return count;
    }

    // Original function: FUN_00432764; the first earlier relative determines each family ID.
    std::uint32_t House_Inhabitants_GetNumberOfFamiliesLivingInHome(const void* house, std::uint32_t* families)
    {
        using namespace NC2Lifetime;
        if (families != nullptr) for (std::uint32_t slot = 0; slot < 20; ++slot) families[slot] = 0xFFFFFFFFu;
        if (House_GetHouseMainType(house) != 2) return 0;
        std::uint32_t count = 0;
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            if ((Read<std::uint8_t>(house, 0x640u + slot * 16u) & 1u) == 0) continue;
            if (count == 0) { count = 1; if (families != nullptr) families[slot] = 0; continue; }
            const auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(house, 0x644u + slot * 16u) * CHumanArray::ElementSize;
            bool related = false;
            for (std::uint32_t previous = 0; previous < slot; ++previous)
            {
                if ((Read<std::uint8_t>(house, 0x640u + previous * 16u) & 1u) == 0) continue;
                const auto id = Read<std::uint32_t>(house, 0x644u + previous * 16u);
                for (const auto offset : {0xB14u, 0xB18u, 0xB24u, 0xB20u}) if (Read<std::uint32_t>(human, offset) != 0xFFFFFFFFu && Read<std::uint32_t>(human, offset) == id) related = true;
                if (related) { if (families != nullptr) families[slot] = families[previous]; break; }
            }
            if (!related) { if (families != nullptr) families[slot] = count; ++count; }
        }
        return count;
    }

    // Original function: FUN_004328b7.
    std::int32_t House_Inhabitants_GetNumberOfAttachedWorkersOfPassedJob(const void* house, std::uint32_t job)
    {
        using namespace NC2Lifetime;
        std::int32_t count = 0;
        for (std::uint32_t slot = 0; slot < 20; ++slot) if ((Read<std::uint8_t>(house, 0x640u + slot * 16u) & 2u) != 0 && Human_UserControl_GetNextJobType(CHumanArray::mStaticVars + Read<std::uint32_t>(house, 0x644u + slot * 16u) * CHumanArray::ElementSize) == job) ++count;
        return count;
    }

    // Original function: FUN_00433079; construction stock limits builders and available slots.
    std::int32_t House_Construction_GetTotalNumberOfBuildersAllowed(const void* house)
    {
        using namespace NC2Lifetime;
        std::uint32_t total = 0;
        for (std::int32_t index = 0; index < Read<std::int32_t>(house, 0x38); ++index) total += Read<std::uint32_t>(house, 0x40u + static_cast<std::uint32_t>(index) * 20u);
        if (Read<std::int32_t>(house, 0x38) <= 0 || total < 5) { if (!House_Repair_IsFinished(house)) total = 5; }
        const auto available = 20 - static_cast<std::int32_t>(House_Inhabitants_GetNumberOfAttachedHumans_IgnoreBuilders(house));
        return NXBasics::Runtime_TimeSigned32(total) <= available ? NXBasics::Runtime_TimeSigned32(total) : available;
    }

    // Original function: FUN_0043339a; Windows returns true for a full hitpoint value.
    bool House_Repair_IsFinished(const void* house)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(house, 0x2C) == 1) return true;
        const auto* engine = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        return Read<std::int32_t>(house, 0x7C8) >= Read<std::int32_t>(engine, 0x19Cu + Read<std::uint32_t>(house, 0x20) * 4u);
    }

    // Original function: FUN_00432681.
    void* House_Inhabitants_GetAttachedHumanSlotPtrBySlotId(void* house, std::uint32_t slot) { return static_cast<std::byte*>(house) + 0x640u + slot * 16u; }
    // Original function: FUN_004326c0.
    std::int32_t House_Inhabitants_GetAttachedHumanSlotIdByHumanId(const void* house, std::uint32_t human) { for (std::uint32_t slot = 0; slot < 20; ++slot) if (NC2Lifetime::Read<std::uint8_t>(house,0x640u + slot * 16u) && NC2Lifetime::Read<std::uint32_t>(house,0x644u + slot * 16u) == human) return static_cast<std::int32_t>(slot); return -1; }

    // Original function: FUN_00432690.
    void* House_Inhabitants_GetAttachedHumanSlotPtrByHumanId(void* house, std::uint32_t human)
    {
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* slot = static_cast<std::byte*>(house) + 0x640u + index * 0x10u;
            if (NC2Lifetime::Read<std::uint8_t>(slot, 0) != 0 && NC2Lifetime::Read<std::uint32_t>(slot, 4) == human) return slot;
        }
        return nullptr;
    }

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
        const auto* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
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
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
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

    // Original function: FUN_00431c84.
    std::uint32_t House_Stock_GetMaximumAmount(const void* house, std::uint32_t good, bool futureStock)
    {
        if (NXBasics::Runtime_TimeSigned32(good) >= 66) return 0;
        const auto* data = House_GetHouseTypeDataPtr(house);
        const auto slot = House_l_Stock_GetStockSlotIdForGoodType(house, good);
        if (slot == -1 || (NC2Lifetime::Read<std::uint32_t>(data, 0) == 3 && futureStock && NC2Lifetime::Read<std::uint8_t>(data, 0x6Cu + static_cast<std::uint32_t>(slot) * 12u) != 0)) return 0;
        return House_Stock_GetMaximumAmountByStockId(house, static_cast<std::uint32_t>(slot));
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
        const auto* type = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        NC2Lifetime::Read<const CMapArea*>(type, 0x20u + NC2Lifetime::Read<std::uint32_t>(house, 0x20) * 4u)->WalkBlock_Set(static_cast<const std::byte*>(house) + 0x1C, true);
        type = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
        NC2Lifetime::Read<const CMapArea*>(type, 0x1C)->BuildBlock_Set(static_cast<const std::byte*>(house) + 0x1C, true);
    }

    // Original function: FUN_00434345.
    void* House_Misc_WalkBlockArea_GetPtr(const void* house)
    {
        const void* type = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(NC2Lifetime::Read<std::uint32_t>(house, 0x10));
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
