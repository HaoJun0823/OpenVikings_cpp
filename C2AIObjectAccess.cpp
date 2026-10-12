#include "C2AIObjectAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanJobAccess.h"
#include "C2HumanWalking.h"
#include "C2VehicleAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Human(const void* record) { return CHumanArray::mStaticVars + Read<std::uint32_t>(record, 0) * CHumanArray::ElementSize; }
        void* Good(void* manager, std::uint32_t good) { return static_cast<std::byte*>(manager) + good * 0x28u; }
    }

    // Original function: FUN_00428b63.
    bool AIHighLevelHandler_IsObjectUsed(const void* handler, const void* target)
    {
        const auto* bytes = static_cast<const std::byte*>(handler);
        if (AIScout_IsObjectUsed(bytes + 0x94E8, target)) return true;
        if (AIScout_IsObjectUsed(bytes + 0xAA18, target)) return true;
        if (AITradeManager_IsObjectUsed(bytes + 0x74D4, target)) return true;
        return AIVillagesData_IsObjectUsed(bytes + 0x1C, target);
    }

    // Original function: FUN_0045360e.
    bool AITradeManager_IsObjectUsed(const void* manager, const void* target)
    {
        if (Read<std::uint32_t>(target, 0) != 1) return false;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(manager, 0); ++i)
            if (Read<std::uint32_t>(manager, 4 + i * 16) == Read<std::uint32_t>(target, 4) && Read<std::uint32_t>(manager, 8 + i * 16) == Read<std::uint32_t>(target, 8)) return true;
        return false;
    }

    // Original function: FUN_004536df.
    bool AIVillagesData_IsObjectUsed(const void* villages, const void* target)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0u;
            if (Read<std::uint8_t>(slot, 8) != 0 && AIVillageManager_IsObjectUsed(slot + 0xC, target)) return true;
        }
        return false;
    }

    // Original function: FUN_00453cc3.
    void AIScout_Init(void* scout) { Write<std::uint32_t>(scout, 0, 0xFFFFFFFFu); Write<std::uint8_t>(scout, 8, 0); }

    // Original function: FUN_00453ccb.
    void AIScout_Set(void* scout, std::int32_t human)
    {
        Write(scout, 0, human);
        Write(scout, 4, Read<std::uint32_t>(CHumanArray::mStaticVars + static_cast<std::uint32_t>(human) * CHumanArray::ElementSize, 8));
        Write<std::uint8_t>(scout, 8, 0);
    }

    // Original function: FUN_00453ceb.
    bool AIScout_IsObjectUsed(const void* scout, const void* target)
    {
        return Read<std::uint32_t>(target, 0) == 1 && Read<std::uint32_t>(target, 4) == Read<std::uint32_t>(scout, 0) && Read<std::uint32_t>(target, 8) == Read<std::uint32_t>(scout, 4);
    }

    // Original function: FUN_00453d0c.
    void AIScout_Clear(void* scout)
    {
        if (!AIGoodManager_Producer_IsHumanValid(scout)) return;
        auto* human = Human(scout);
        Human_DoAddNewUserCommand(human, 21, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), 0, 0, 6, 0);
        Write<std::uint32_t>(scout, 0, 0xFFFFFFFFu);
    }

    // Original function: FUN_00453d3c.
    bool AIScout_CheckAndChangeJob(const void* scout)
    {
        auto* human = Human(scout);
        if (Human_IsJobScout(human)) return true;
        Human_DoAddNewUserCommand(human, 21, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), 0, 0, 27, 0);
        return false;
    }

    // Original function: FUN_00453d71; Windows static 004f4300 stores distance 50.
    bool AIScout_IsNearPos(const void* scout, const void* point)
    {
        const auto* human = Human(scout);
        const auto* position = Human_GetMapPositionRef(human);
        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(point, 0), Read<std::uint16_t>(point, 2), Read<std::uint16_t>(position, 0), Read<std::uint16_t>(position, 2)));
        return distance <= 50 && !Human_IsWalking(human);
    }

    // Original function: FUN_00453dc5.
    void AIScout_MoveToPos(const void* scout, const void* point)
    {
        auto* human = Human(scout);
        const auto revision = Read<std::uint32_t>(human, 8), id = Read<std::uint32_t>(human, 4), player = Read<std::uint32_t>(human, 0x10);
        Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(player), id, revision, MapMigPoint_GetLong(point), 0);
    }

    // Original function: FUN_0045739b; every non-human target uses the house fields.
    bool AIGoodManager_IsObjectUsed(const void* manager, const void* target)
    {
        const auto offset = Read<std::uint32_t>(target, 0) == 1 ? 0u : 8u;
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            const auto* entry = static_cast<const std::byte*>(manager) + good * 0x28u;
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 16); ++i)
            {
                const auto* producer = entry + 20 + i * 20;
                if (Read<std::uint32_t>(producer, offset) == Read<std::uint32_t>(target, 4) && Read<std::uint32_t>(producer, offset + 4) == Read<std::uint32_t>(target, 8)) return true;
            }
        }
        return false;
    }

    // Original function: FUN_004715fe.
    bool AIVillageManager_IsObjectUsed(const void* village, const void* target) { return AIGoodManager_IsObjectUsed(static_cast<const std::byte*>(village) + 0x274, target); }

    // Original function: FUN_004574ac.
    std::uint8_t AIGoodManager_IsProducingGoodRealy(const void* manager, std::uint32_t good)
    {
        const auto* entry = static_cast<const std::byte*>(manager) + good * 0x28u;
        if (Read<std::uint8_t>(entry, 9) != 0 && Read<std::uint8_t>(entry, 10) != 0) return 0;
        return Read<std::uint8_t>(entry, 0);
    }

    // Original function: FUN_004574cc.
    bool AIGoodManager_Producer_IsHouseValid(void* producer)
    {
        const auto id = Read<std::uint32_t>(producer, 8);
        if (id == 0xFFFFFFFFu) return false;
        const auto* house = CHouseArray::mStaticVars + id * CHouseArray::ElementSize;
        if (Read<std::uint8_t>(house, 0) != 0 && Read<std::uint32_t>(house, 8) == Read<std::uint32_t>(producer, 12) && Read<std::uint8_t>(house, 0x7D0) == 0) return true;
        Write<std::uint32_t>(producer, 8, 0xFFFFFFFFu);
        return false;
    }

    // Original function: FUN_00457500.
    bool AIGoodManager_Producer_IsHumanValid(void* producer)
    {
        const auto id = Read<std::uint32_t>(producer, 0);
        if (id == 0xFFFFFFFFu) return false;
        const auto* human = Human(producer);
        if (Read<std::uint8_t>(human, 0) != 0 && Read<std::uint32_t>(human, 8) == Read<std::uint32_t>(producer, 4)) return true;
        Write<std::uint32_t>(producer, 0, 0xFFFFFFFFu);
        return false;
    }

    // Original function: FUN_00457529.
    bool AIGoodManager_Producer_IsHumanValidCheck(const void* producer)
    {
        if (Read<std::uint32_t>(producer, 0) == 0xFFFFFFFFu) return false;
        const auto* human = Human(producer);
        return Read<std::uint8_t>(human, 0) != 0 && Read<std::uint32_t>(human, 8) == Read<std::uint32_t>(producer, 4);
    }

    // Windows shares the producer validity entries at 00457500 and 00457529 with scout records.
    bool AIScout_IsValid(void* scout) { return AIGoodManager_Producer_IsHumanValid(scout); }
    bool AIScout_IsValidCheck(const void* scout) { return AIGoodManager_Producer_IsHumanValidCheck(scout); }

    // Original function: FUN_00457958.
    void AIGoodManager_StopProduction(void* manager, std::uint32_t good)
    {
        auto* entry = Good(manager, good);
        Write<std::uint32_t>(entry, 12, 0);
        Write<std::uint8_t>(entry, 0, 0);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 16); ++i) AIGoodManager_Producer_Clear(static_cast<std::byte*>(entry) + 20 + i * 20);
        Write<std::uint32_t>(entry, 16, 0);
    }

    // Original function: FUN_00457992.
    void AIGoodManager_Producer_Clear(void* producer)
    {
        Write<std::uint32_t>(producer, 8, 0xFFFFFFFFu);
        if (AIGoodManager_Producer_IsHumanValid(producer))
        {
            auto* human = Human(producer);
            if (Human_IsAttachedToWorkHouse(human)) Human_DoAddNewUserCommand(human, 22, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
        }
        Write<std::uint32_t>(producer, 0, 0xFFFFFFFFu);
    }

    // Original function: FUN_004579d8.
    void AIGoodManager_ImportThisGoodType(void* manager, std::uint32_t good)
    {
        auto* entry = Good(manager, good);
        Write<std::uint8_t>(entry, 9, 1);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 16); ++i) AIGoodManager_Producer_Clear(static_cast<std::byte*>(entry) + 20 + i * 20);
        Write<std::uint32_t>(entry, 16, 0);
    }

    // Original function: FUN_00457a0d.
    void AIGoodManager_SetTraderBringFlag(void* manager, std::uint32_t good, bool active) { Write<std::uint8_t>(manager, good * 0x28u + 10, active); }

    // Original function: FUN_00457a1f.
    std::uint32_t AIGoodManager_GetNumberOfUsedHumans(const void* manager)
    {
        std::uint32_t count = 0;
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            const auto* entry = static_cast<const std::byte*>(manager) + good * 0x28u;
            if (Read<std::uint8_t>(entry, 0) == 0) continue;
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 16); ++i) if (AIGoodManager_Producer_IsHumanValidCheck(entry + 20 + i * 20)) ++count;
        }
        return count;
    }
}
