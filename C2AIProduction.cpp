#include "C2ObjectLayout.h"
#include "C2AIProduction.h"
#include "C2AIBuildingAccess.h"
#include "C2AIObjectAccess.h"
#include "C2AIResourceAccess.h"
#include "C2AIVillageAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        std::byte* Producer(void* manager, std::uint32_t good, std::uint32_t producer) { return static_cast<std::byte*>(manager) + (producer + good * 2) * 20 + 20; }
        void* Human(const void* producer) { return CHumanArray::mStaticVars + Read<std::uint32_t>(producer, 0) * CHumanArray::ElementSize; }
        void* House(const void* producer) { return CHouseArray::mStaticVars + Read<std::uint32_t>(producer, 8) * CHouseArray::ElementSize; }
        void Command(void* human, std::uint8_t command, std::uint32_t first, std::uint32_t second) { Human_DoAddNewUserCommand(human, command, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 16)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), first, second); }
    }
    // Original function: FUN_004571ce; fixed availability bytes come from Windows tribe branches.
    void AIGoodManager_Init(void* manager, std::uint32_t tribe)
    {
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            auto* entry = static_cast<std::byte*>(manager) + good * 40;
            Write<std::int32_t>(entry, 20, -1); Write<std::int32_t>(entry, 28, -1); Write<std::uint8_t>(entry, 36, 0);
            Write<std::uint32_t>(entry, 16, 0); Write<std::uint8_t>(entry, 0, 0); Write<std::uint8_t>(entry, 10, 0); Write<std::uint8_t>(entry, 9, 0);
            Write<std::uint32_t>(entry, 12, 0); Write<std::uint8_t>(entry, 8, 0);
        }
        if (tribe != 2 && tribe != 3 && tribe != 4 && tribe != 7) return;
        const std::uint32_t shared[] = {0x30, 0xA8, 0x148, 0x1C0, 0x1E8, 0x210, 0x260, 0x328, 0x3F0, 0x440, 0x468, 0x490, 0x580, 0x5A8, 0x6C0};
        for (auto offset : shared) Write<std::uint8_t>(manager, offset, 1);
        if (tribe == 2) { Write<std::uint8_t>(manager, 0x418, 1); Write<std::uint8_t>(manager, 0x5F8, 1); Write<std::uint8_t>(manager, 0x698, 1); }
        else if (tribe == 3) { Write<std::uint8_t>(manager, 0x620, 1); Write<std::uint8_t>(manager, 0x670, 1); }
        else
        {
            const std::uint32_t extra[] = {0x418, 0x4E0, 0x530, 0x558, 0x620, 0x5D0};
            for (auto offset : extra) Write<std::uint8_t>(manager, offset, 1);
        }
    }
    // Original function: FUN_004576b4.
    std::int32_t AIGoodManager_l_House_SearchHouse(void* manager, std::uint32_t good, std::uint32_t index, void* village)
    {
        auto* producer = Producer(manager, good, index);
        if (AIGoodManager_Producer_IsHouseValid(producer)) return 1;
        const auto id = AIVillageManager_Good_GetWorkHouseId(village, good);
        if (id == -2) return -2;
        if (id == -1) { Write<std::int32_t>(producer, 0, -1); return 0; }
        Write(producer, 8, id); Write(producer, 12, Read<std::uint32_t>(House(producer), NC2Layout::HouseOffset::UniqueId));
        return 1;
    }
    // Original function: FUN_0045771e.
    bool AIGoodManager_l_GetWorkerForHouse(void* manager, std::uint32_t good, std::uint32_t index, const void* village)
    {
        auto* producer = Producer(manager, good, index);
        if (AIGoodManager_Producer_IsHumanValid(producer)) return true;
        const auto id = AIVillageManager_Good_GetWorkHumanId(village, House_Door_GetEntryPointRef(House(producer)));
        if (id == -1) return false;
        Write(producer, 0, id); Write(producer, 4, Read<std::uint32_t>(Human(producer), NC2Layout::HumanOffset::UniqueId));
        return true;
    }
    // Original function: FUN_0045778b.
    bool AIGoodManager_l_House_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t index, const void* resources)
    {
        auto* producer = Producer(manager, good, index);
        auto* house = House(producer);
        const auto job = Read<std::uint32_t>(resources, (good + 48) * 16);
        const auto id = Read<std::uint32_t>(producer, 0);
        if (!House_Inhabitants_IsFreeWorkSlotAvailable(house, job) && !House_Inhabitants_IsHumanAttachedAsWorker(house, id)) { AIGoodManager_Producer_Clear(producer); return false; }
        auto* human = Human(producer);
        if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == job)
        {
            if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::WorkAttachmentHouseId) != Read<std::uint32_t>(producer, 8)) Command(human, 28, Read<std::uint32_t>(producer, 8), Read<std::uint32_t>(producer, 12));
            else if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::ProducedGoodType) != good) Command(human, 22, good, 0);
            else return House_Stock_GetAmountOfGoods(house, good, true) != 0;
        }
        else if (Human_IsJobBuilder(human) && Human_IsAttachedToBuildSite(human)) Write<std::int32_t>(producer, 0, -1);
        else Command(human, 21, job, 0);
        return false;
    }
    // Original function: FUN_004578a3.
    bool AIGoodManager_l_Collect_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t index, const void* village, const void* resources)
    {
        if (!AIGoodManager_l_House_CheckHumanSettings(manager, good, index, resources)) return false;
        auto* producer = Producer(manager, good, index);
        std::uint32_t point = 0;
        if (!AIVillageManager_Good_GetWorkCenterPoint(village, good, &point)) { Write<std::int32_t>(producer, 0, -1); Write<std::int32_t>(producer, 8, -1); return false; }
        auto* human = Human(producer);
        if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::WorkCenter) == point) return true;
        Command(human, 17, MapMigPoint_GetLong(&point), 0);
        return false;
    }
    // Original function: FUN_0045754f.
    void AIGoodManager_DoOneLogicGameTick(void* manager, void* village, const void* resources)
    {
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            auto* entry = static_cast<std::byte*>(manager) + good * 40;
            Write(entry, 0, Read<std::uint8_t>(entry, 8));
            if (Read<std::uint8_t>(entry, 0) != 0) continue;
            const auto* data = static_cast<const std::byte*>(resources) + 0x2FC + good * 16;
            if (Read<std::uint32_t>(data, 0) == 1 && AIResourceManager_WasResourceDataUpdated(resources, good) != 0)
            {
                std::uint32_t point = 0;
                if (AIResourceManager_GetResourcePosition(resources, good, &point) && AIVillage_IsPointInside(village, &point)) { Write<std::uint8_t>(entry, 9, 0); Write<std::uint8_t>(entry, 10, 0); }
            }
            if (Read<std::uint8_t>(entry, 9) != 0)
            {
                Write(entry, 0, Read<std::uint8_t>(entry, 10));
                if (Read<std::uint32_t>(entry, 16) > 0) { AIGoodManager_Producer_Clear(entry + 20); Write<std::uint32_t>(entry, 16, 0); }
                continue;
            }
            if (Read<std::uint32_t>(entry, 12) == 0) { AIGoodManager_Producer_Clear(entry + 20); Write<std::uint32_t>(entry, 4, 0); continue; }
            const auto house = AIGoodManager_l_House_SearchHouse(manager, good, 0, village);
            if (house == 0) { AIVillageManager_Good_AskForNewHouse(village, good, resources); continue; }
            if (house != 1) continue;
            if (Read<std::uint32_t>(data, 4) == 0) { Write<std::uint8_t>(entry, 0, 1); Write(entry, 12, Read<std::uint32_t>(entry, 12) - 1); continue; }
            if (!AIGoodManager_l_GetWorkerForHouse(manager, good, 0, village)) continue;
            const auto kind = Read<std::uint32_t>(data, 0);
            const auto producing = kind == 4 ? AIGoodManager_l_House_CheckHumanSettings(manager, good, 0, resources) : kind == 1 ? AIGoodManager_l_Collect_CheckHumanSettings(manager, good, 0, village, resources) : false;
            if (producing)
            {
                const auto count = Read<std::uint32_t>(entry, 4) + 1;
                Write(entry, 4, count);
                if (count > 5) { Write<std::uint8_t>(entry, 0, 1); Write(entry, 12, Read<std::uint32_t>(entry, 12) - 1); }
            }
            Write<std::uint32_t>(entry, 16, 1);
        }
    }
}
