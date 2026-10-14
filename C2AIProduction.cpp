#include "SwitchIdentifiers.h"
#include "C2RuntimeConstants.h"
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
        std::byte* Producer(void* manager, std::uint32_t good, std::uint32_t producer)
        {
            return static_cast<std::byte*>(manager) + (producer + good * NC2Runtime::AIGoodsProducerAddressing::GoodIndexScale) * NC2Layout::RecordSize::AIGoodProducer + NC2Layout::AIGoodOffset::Producer;
        }
        void* Human(const void* producer)
        {
            return CHumanArray::mStaticVars + Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HumanId) * CHumanArray::ElementSize;
        }
        void* House(const void* producer)
        {
            return CHouseArray::mStaticVars + Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HouseId) * CHouseArray::ElementSize;
        }
        void Command(void* human, std::uint8_t command, std::uint32_t first, std::uint32_t second)
        {
            Human_DoAddNewUserCommand(human, command, static_cast<std::uint8_t>(Read<std::uint32_t>(human, NC2Layout::HumanOffset::PlayerId)), Read<std::uint32_t>(human, NC2Layout::HumanOffset::HumanId), Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId), first, second);
        }
    }
    // Original function: FUN_004571ce; fixed availability bytes come from Windows tribe branches.
    void AIGoodManager_Init(void* manager, std::uint32_t tribe)
    {
        for (std::uint32_t good = 0; good < NC2Runtime::AIGoodsLimits::Goods; ++good)
        {
            auto* entry = static_cast<std::byte*>(manager) + good * NC2Layout::RecordSize::AIGood;
            Write<std::int32_t>(entry, NC2Layout::AIGoodOffset::Producer, -1);
            Write<std::int32_t>(entry, NC2Layout::AIGoodOffset::ProducerHouseId, -1);
            Write<std::uint8_t>(entry, 36, 0);
            Write<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProducerCount, 0);
            Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ProducingFlag, 0);
            Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::TraderBringFlag, 0);
            Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ImportingFlag, 0);
            Write<std::uint32_t>(entry, NC2Layout::AIGoodOffset::NeedCount, 0);
            Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ForceProducingFlag, 0);
        }
        if (tribe != 2 && tribe != 3 && tribe != 4 && tribe != 7) return;
        const std::uint32_t sharedForceProducingFlagOffsets[] = {
            NC2Layout::RecordSize::AIGood * 1 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 4 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 8 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 11 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 12 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 13 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 15 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 20 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 25 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 27 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 28 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 29 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 35 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 36 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 43 + NC2Layout::AIGoodOffset::ForceProducingFlag
        };
        for (auto offset : sharedForceProducingFlagOffsets) Write<std::uint8_t>(manager, offset, 1);
        if (tribe == 2)
        {
            Write<std::uint8_t>(manager, NC2Layout::RecordSize::AIGood * 26 + NC2Layout::AIGoodOffset::ForceProducingFlag, 1);
            Write<std::uint8_t>(manager, NC2Layout::RecordSize::AIGood * 38 + NC2Layout::AIGoodOffset::ForceProducingFlag, 1);
            Write<std::uint8_t>(manager, NC2Layout::RecordSize::AIGood * 42 + NC2Layout::AIGoodOffset::ForceProducingFlag, 1);
        }
        else if (tribe == 3)
        {
            Write<std::uint8_t>(manager, NC2Layout::RecordSize::AIGood * 39 + NC2Layout::AIGoodOffset::ForceProducingFlag, 1);
            Write<std::uint8_t>(manager, NC2Layout::RecordSize::AIGood * 41 + NC2Layout::AIGoodOffset::ForceProducingFlag, 1);
        }
        else
        {
            const std::uint32_t extraForceProducingFlagOffsets[] = {
            NC2Layout::RecordSize::AIGood * 26 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 31 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 33 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 34 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 39 + NC2Layout::AIGoodOffset::ForceProducingFlag,
            NC2Layout::RecordSize::AIGood * 37 + NC2Layout::AIGoodOffset::ForceProducingFlag
        };
            for (auto offset : extraForceProducingFlagOffsets) Write<std::uint8_t>(manager, offset, 1);
        }
    }
    // Original function: FUN_004576b4.
    std::int32_t AIGoodManager_l_House_SearchHouse(void* manager, std::uint32_t good, std::uint32_t index, void* village)
    {
        auto* producer = Producer(manager, good, index);
        if (AIGoodManager_Producer_IsHouseValid(producer)) return 1;
        const auto id = AIVillageManager_Good_GetWorkHouseId(village, good);
        if (id == -2) return -2;
        if (id == -1)
        {
            Write<std::int32_t>(producer, NC2Layout::AIGoodProducerOffset::HumanId, -1);
            return 0;
        }
        Write(producer, NC2Layout::AIGoodProducerOffset::HouseId, id);
        Write(producer, NC2Layout::AIGoodProducerOffset::HouseRevision, Read<std::uint32_t>(House(producer), NC2Layout::HouseOffset::UniqueId));
        return 1;
    }
    // Original function: FUN_0045771e.
    bool AIGoodManager_l_GetWorkerForHouse(void* manager, std::uint32_t good, std::uint32_t index, const void* village)
    {
        auto* producer = Producer(manager, good, index);
        if (AIGoodManager_Producer_IsHumanValid(producer)) return true;
        const auto id = AIVillageManager_Good_GetWorkHumanId(village, House_Door_GetEntryPointRef(House(producer)));
        if (id == -1) return false;
        Write(producer, NC2Layout::AIGoodProducerOffset::HumanId, id);
        Write(producer, NC2Layout::AIGoodProducerOffset::HumanRevision, Read<std::uint32_t>(Human(producer), NC2Layout::HumanOffset::UniqueId));
        return true;
    }
    // Original function: FUN_0045778b.
    bool AIGoodManager_l_House_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t index, const void* resources)
    {
        auto* producer = Producer(manager, good, index);
        auto* house = House(producer);
        const auto job = Read<std::uint32_t>(resources, (good + NC2Runtime::AIGoodsProducerAddressing::WorkerRequirementIndexBias) * NC2Layout::RecordSize::AIResourceGood);
        const auto id = Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HumanId);
        if (!House_Inhabitants_IsFreeWorkSlotAvailable(house, job) && !House_Inhabitants_IsHumanAttachedAsWorker(house, id))
        {
            AIGoodManager_Producer_Clear(producer);
            return false;
        }
        auto* human = Human(producer);
        if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::JobType) == job)
        {
            if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::WorkAttachmentHouseId) != Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HouseId)) Command(human, 28, Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HouseId), Read<std::uint32_t>(producer, NC2Layout::AIGoodProducerOffset::HouseRevision));
            else if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::ProducedGoodType) != good) Command(human, 22, good, 0);
            else return House_Stock_GetAmountOfGoods(house, good, true) != 0;
        }
        else if (Human_IsJobBuilder(human) && Human_IsAttachedToBuildSite(human)) Write<std::int32_t>(producer, NC2Layout::AIGoodProducerOffset::HumanId, -1);
        else Command(human, 21, job, 0);
        return false;
    }
    // Original function: FUN_004578a3.
    bool AIGoodManager_l_Collect_CheckHumanSettings(void* manager, std::uint32_t good, std::uint32_t index, const void* village, const void* resources)
    {
        if (!AIGoodManager_l_House_CheckHumanSettings(manager, good, index, resources)) return false;
        auto* producer = Producer(manager, good, index);
        std::uint32_t point = 0;
        if (!AIVillageManager_Good_GetWorkCenterPoint(village, good, &point))
        {
            Write<std::int32_t>(producer, NC2Layout::AIGoodProducerOffset::HumanId, -1);
            Write<std::int32_t>(producer, NC2Layout::AIGoodProducerOffset::HouseId, -1);
            return false;
        }
        auto* human = Human(producer);
        if (Read<std::uint32_t>(human, NC2Layout::HumanOffset::WorkCenter) == point) return true;
        Command(human, 17, MapMigPoint_GetLong(&point), 0);
        return false;
    }
    // Original function: FUN_0045754f.
    void AIGoodManager_DoOneLogicGameTick(void* manager, void* village, const void* resources)
    {
        for (std::uint32_t good = 0; good < NC2Runtime::AIGoodsLimits::Goods; ++good)
        {
            auto* entry = static_cast<std::byte*>(manager) + good * NC2Layout::RecordSize::AIGood;
            Write(entry, NC2Layout::AIGoodOffset::ProducingFlag, Read<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ForceProducingFlag));
            if (Read<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ProducingFlag) != 0) continue;
            const auto* data = static_cast<const std::byte*>(resources) + NC2Layout::AIResourceManagerOffset::GoodRequirements + good * NC2Layout::RecordSize::AIResourceGood;
            if (Read<std::uint32_t>(data, NC2Layout::AIResourceGoodOffset::Type) == 1 && AIResourceManager_WasResourceDataUpdated(resources, good) != 0)
            {
                std::uint32_t point = 0;
                if (AIResourceManager_GetResourcePosition(resources, good, &point) && AIVillage_IsPointInside(village, &point))
                {
                    Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ImportingFlag, 0);
                    Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::TraderBringFlag, 0);
                }
            }
            if (Read<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ImportingFlag) != 0)
            {
                Write(entry, NC2Layout::AIGoodOffset::ProducingFlag, Read<std::uint8_t>(entry, NC2Layout::AIGoodOffset::TraderBringFlag));
                if (Read<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProducerCount) > 0)
                {
                    AIGoodManager_Producer_Clear(entry + NC2Layout::AIGoodOffset::Producer);
                    Write<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProducerCount, 0);
                }
                continue;
            }
            if (Read<std::uint32_t>(entry, NC2Layout::AIGoodOffset::NeedCount) == 0)
            {
                AIGoodManager_Producer_Clear(entry + NC2Layout::AIGoodOffset::Producer);
                Write<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProductionConfirmationCount, 0);
                continue;
            }
            const auto house = AIGoodManager_l_House_SearchHouse(manager, good, 0, village);
            if (house == 0)
            {
                AIVillageManager_Good_AskForNewHouse(village, good, resources);
                continue;
            }
            if (house != 1) continue;
            if (Read<std::uint32_t>(data, NC2Layout::AIResourceGoodOffset::WorkerJob) == 0)
            {
                Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ProducingFlag, 1);
                Write(entry, NC2Layout::AIGoodOffset::NeedCount, Read<std::uint32_t>(entry, NC2Layout::AIGoodOffset::NeedCount) - 1);
                continue;
            }
            if (!AIGoodManager_l_GetWorkerForHouse(manager, good, 0, village)) continue;
            const auto kind = Read<std::uint32_t>(data, NC2Layout::AIResourceGoodOffset::Type);
            const auto producing = kind == 4 ? AIGoodManager_l_House_CheckHumanSettings(manager, good, 0, resources) : kind == 1 ? AIGoodManager_l_Collect_CheckHumanSettings(manager, good, 0, village, resources) : false;
            if (producing)
            {
                const auto count = Read<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProductionConfirmationCount) + 1;
                Write(entry, NC2Layout::AIGoodOffset::ProductionConfirmationCount, count);
                if (count > 5)
                {
                    Write<std::uint8_t>(entry, NC2Layout::AIGoodOffset::ProducingFlag, 1);
                    Write(entry, NC2Layout::AIGoodOffset::NeedCount, Read<std::uint32_t>(entry, NC2Layout::AIGoodOffset::NeedCount) - 1);
                }
            }
            Write<std::uint32_t>(entry, NC2Layout::AIGoodOffset::ProducerCount, 1);
        }
    }
}
