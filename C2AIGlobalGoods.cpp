#include "C2AIGlobalGoods.h"
#include "C2AIBuildingAccess.h"
#include "C2AIObjectAccess.h"
#include "C2AIResourceAccess.h"
#include "C2LifetimeAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Good(void* manager, std::uint32_t good) { return static_cast<std::byte*>(manager) + good * 0x50; }
        const void* Good(const void* manager, std::uint32_t good) { return static_cast<const std::byte*>(manager) + good * 0x50; }
        void* VillageGoods(void* villages, std::int32_t id) { return static_cast<std::byte*>(AIVillagesData_GetVillageDataById(villages, id)) + 0x274; }
    }

    // Original function: FUN_004538b5.
    bool AIVillagesData_House_CanBuildAnyFurther(const void* villages, std::uint32_t house)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && Read<std::uint8_t>(slot, 0x31 + house) != 0) return true;
        }
        return false;
    }

    // Original function: FUN_00453b68.
    std::uint32_t AIVillagesData_GetNumberOfUsedHumans(const void* villages)
    {
        std::uint32_t count = 0;
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0) count += AIGoodManager_GetNumberOfUsedHumans(slot + 0x280);
        }
        return count;
    }

    // Original function: FUN_00453b97; recursive need increments stop at the first producing village.
    bool AIVillagesData_Good_IsProducingGood(void* villages, std::uint32_t good, bool includeInputs)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && AIGoodManager_IsProducingGood(slot + 0x280, good, includeInputs)) return true;
        }
        return false;
    }

    // Original function: FUN_00456da5.
    void AIGlobalGoods_Init(void* manager)
    {
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            auto* entry = Good(manager, good);
            Write<std::int32_t>(entry, 0, -1); Write<std::uint32_t>(entry, 4, 0); Write<std::uint32_t>(entry, 0x28, 0); Write<std::uint32_t>(entry, 0x2C, 0);
        }
    }

    // Original function: FUN_00456dc0; the skip byte is read from the first village throughout the outer loop.
    void AIGlobalGoods_DoOneLogicGameTick(void* manager, void* villages)
    {
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            auto* entry = Good(manager, good);
            Write<std::int32_t>(entry, 0, -1); Write<std::uint32_t>(entry, 4, 0); Write<std::uint32_t>(entry, 0x28, 0); Write<std::uint32_t>(entry, 0x2C, 0);
        }
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) == 0) continue;
            const auto id = Read<std::int32_t>(slot, 16);
            auto* goods = slot + 0x280;
            for (std::uint32_t good = 0; good < 66; ++good)
            {
                if (Read<std::uint8_t>(villages, 0x288 + good * 0x28) != 0 || good == 19 || good == 4 || good == 1 || good == 11) continue;
                auto* local = goods + good * 0x28;
                std::uint32_t action = 0;
                if (AIGoodManager_IsProducingGoodRealy(goods, good) != 0) action = AIGlobalGoods_l_AddProducer(manager, id, good, villages);
                else if (Read<std::uint8_t>(local, 9) != 0) action = AIGlobalGoods_l_AddImporting(manager, id, good, villages);
                else if (Read<std::uint32_t>(local, 12) != 0) action = AIGlobalGoods_l_AddNeed(manager, id, good, villages);
                else if (Read<std::int32_t>(Good(manager, good), 0) == -1) Write(Good(manager, good), 0, id);
                switch (action)
                {
                case 1: AIGoodManager_ImportThisGoodType(goods, good); AIGoodManager_SetTraderBringFlag(goods, good, true); break;
                case 2: AIGoodManager_SetTraderBringFlag(goods, good, true); break;
                case 3: AIGoodManager_StopProduction(goods, good); break;
                case 4: AIGoodManager_IsProducingGood(goods, good, true); break;
                case 5: AIGoodManager_SetTraderBringFlag(goods, good, false); break;
                default: break;
                }
            }
        }
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            auto* entry = Good(manager, good);
            const auto producer = Read<std::uint32_t>(entry, 0x28);
            if (producer == 0)
            {
                if (Read<std::uint32_t>(entry, 0x2C) == 0) continue;
                for (std::uint32_t i = 1; i < Read<std::uint32_t>(entry, 0x2C); ++i) AIGoodManager_StopProduction(VillageGoods(villages, Read<std::int32_t>(entry, 0x30 + i * 4)), good);
            }
            else if (Read<std::uint32_t>(entry, 4) != 0 && producer != 1 && Read<std::int32_t>(entry, 0) != -1)
            {
                const auto id = Read<std::int32_t>(entry, 0);
                AIGoodManager_IsProducingGood(VillageGoods(villages, id), good, true);
                const auto count = Read<std::uint32_t>(entry, 0x2C);
                if (count < 8) { Write(entry, 0x30 + count * 4, Read<std::int32_t>(entry, 0)); Write(entry, 0x2C, count + 1); }
            }
        }
    }

    // Original function: FUN_00456fb8.
    bool AIGlobalGoods_IsNeedingVillage(const void* manager, std::uint32_t good)
    {
        const auto* entry = Good(manager, good);
        return Read<std::uint32_t>(entry, 4) != 0 && Read<std::uint32_t>(entry, 0x28) != 1 && Read<std::uint32_t>(entry, 0x2C) == 0;
    }

    // Original function: FUN_00456fe0.
    std::uint32_t AIGlobalGoods_l_AddNeed(void* manager, std::int32_t village, std::uint32_t good, void* villages)
    {
        auto* entry = Good(manager, good);
        if (Read<std::uint32_t>(entry, 0x28) == 1) return 1;
        if (Read<std::int32_t>(entry, 0) != -1)
        {
            AIGoodManager_IsProducingGood(VillageGoods(villages, Read<std::int32_t>(entry, 0)), good, true);
            const auto count = Read<std::uint32_t>(entry, 0x2C);
            if (count < 8) { Write(entry, 0x30 + count * 4, Read<std::int32_t>(entry, 0)); Write(entry, 0x2C, count + 1); }
            Write<std::int32_t>(entry, 0, -1);
            return 3;
        }
        const auto count = Read<std::uint32_t>(entry, 0x2C);
        if (count < 8) { Write(entry, 0x30 + count * 4, village); Write(entry, 0x2C, count + 1); }
        return 0;
    }

    // Original function: FUN_0045704b.
    std::uint32_t AIGlobalGoods_l_AddProducer(void* manager, std::int32_t village, std::uint32_t good, void* villages)
    {
        auto* entry = Good(manager, good);
        if (Read<std::uint32_t>(entry, 0x28) == 1) { AIGoodManager_StopProduction(VillageGoods(villages, village), good); return 1; }
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 0x2C); ++i)
        {
            const auto id = Read<std::int32_t>(entry, 0x30 + i * 4);
            AIGoodManager_ImportThisGoodType(VillageGoods(villages, id), good);
            AIGoodManager_SetTraderBringFlag(VillageGoods(villages, Read<std::int32_t>(entry, 0x30 + i * 4)), good, true);
        }
        Write<std::uint32_t>(entry, 0x28, 1); Write<std::uint32_t>(entry, 0x2C, 0); Write(entry, 0, village);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 4); ++i) AIGoodManager_SetTraderBringFlag(VillageGoods(villages, Read<std::int32_t>(entry, 8 + i * 4)), good, true);
        return 4;
    }

    // Original function: FUN_00457110.
    std::uint32_t AIGlobalGoods_l_AddImporting(void* manager, std::int32_t village, std::uint32_t good, void* villages)
    {
        auto* entry = Good(manager, good);
        const auto count = Read<std::uint32_t>(entry, 4);
        if (count < 8) { Write(entry, 8 + count * 4, village); Write(entry, 4, count + 1); }
        if (Read<std::uint32_t>(entry, 0x28) != 1) return 5;
        AIGoodManager_IsProducingGood(VillageGoods(villages, Read<std::int32_t>(entry, 0)), good, true);
        return 2;
    }
}
