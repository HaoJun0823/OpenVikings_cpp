#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2AITaskAccess.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2MapGeometry.h"
#include "C2GameState.h"
#include "C2AIConditions.h"
#include "C2GameControlNetwork.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MissionStartObjects.h"
#include "C2OverviewObjectIterators.h"
#include "C2HumanIterator.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0044dd7f; task counts store the last occupied index.
    void AISinglePlayerHandler_MainTask_RecheckAll(void* handler)
    {
        const auto last = Read<std::uint32_t>(handler, 0x1C2C);
        for (std::uint32_t i = 0; i <= last; ++i)
        {
            auto* task = static_cast<std::byte*>(handler) + 0x1C30 + i * 0x44;
            const auto type = Read<std::uint32_t>(task, 8);
            if (type == 0) continue;
            const auto priority = AISinglePlayerHandler_Condition_IsActive(handler, Read<std::uint32_t>(task, 0x2C)) != 0 && Read<std::uint8_t>(task, 0) == 0 ? Read<std::uint32_t>(task, 0x28) : 0u;
            Write(task, 4, priority);
            if (priority == 0) continue;
            bool disable = false;
            if (type == 3)
            {
                std::uint32_t point;
                MapMigPoint_SetPosition(&point, Read<std::int16_t>(task, 12), Read<std::int16_t>(task, 14), false);
                disable = AISinglePlayerHandler_l_BuildHouses_AddBuildHouse(handler, Read<std::uint32_t>(task, 16), &point, Read<std::uint32_t>(task, 20), Read<std::uint32_t>(task, 24));
            }
            else if (type == 4)
            {
                GameControl_Diplomacy_SetState(gGameControl, nullptr, Read<std::uint32_t>(handler, 0), Read<std::uint32_t>(task, 16), Read<std::uint32_t>(task, 12));
                disable = true;
            }
            else if (type == 5)
            {
                for (std::uint32_t count = 0; count < Read<std::uint32_t>(task, 24); ++count)
                {
                    std::uint32_t point;
                    MapMigPoint_SetPosition(&point, Read<std::int16_t>(task, 12), Read<std::int16_t>(task, 14), false);
                    Logic_EntryPoint_Add_Human(Read<std::uint32_t>(handler, 0), Read<std::uint32_t>(task, 16), Read<std::uint32_t>(task, 20), &point, Read<std::uint32_t>(task, 32), 0);
                }
                disable = Read<std::uint8_t>(task, 28) != 0;
            }
            else if (type == 6)
            {
                CHumanIterator iterator;
                for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(handler, 0)); id != -1; id = iterator.GetNextHumanId()) CHumanArray::sTheObjectPtr->FreeHuman(id);
                disable = true;
            }
            else if (type == 7 || type == 8) disable = true;
            else continue;
            if (disable) Write<std::uint8_t>(task, 0, 1);
            Write<std::uint32_t>(task, 4, 0);
        }
        std::uint32_t priorities[64];
        std::uint32_t count = 0;
        Write<std::uint32_t>(handler, 0x2978, 0);
        for (std::uint32_t i = 0; i <= last; ++i)
        {
            const auto* task = static_cast<const std::byte*>(handler) + 0x1C30 + i * 0x44;
            if (Read<std::uint32_t>(task, 8) == 0 || Read<std::uint32_t>(task, 4) == 0) continue;
            Write(handler, 0x297C + count * 4, i);
            priorities[count++] = Read<std::uint32_t>(task, 4);
            Write(handler, 0x2978, count);
        }
        if (count > 1)
            for (std::uint32_t end = count - 1; end != 0; --end)
                for (std::uint32_t i = 0; i < end; ++i)
                    if (priorities[i] < priorities[i + 1])
                    {
                        const auto priority = priorities[i]; priorities[i] = priorities[i + 1]; priorities[i + 1] = priority;
                        const auto index = Read<std::uint32_t>(handler, 0x297C + i * 4);
                        Write(handler, 0x297C + i * 4, Read<std::uint32_t>(handler, 0x2980 + i * 4)); Write(handler, 0x2980 + i * 4, index);
                    }
        AISinglePlayerHandler_l_CombinedActiveMainTask_UpdateList(handler);
    }

    // Original function: FUN_0044e2c9; unlike recalculation, this appends without deduplication.
    void AISinglePlayerHandler_l_BuildHouses_SaveAllHousesIntoArray(void* handler)
    {
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(Read<std::uint32_t>(handler, 0)); id != -1; id = iterator.GetNextHouseId())
        {
            const auto count = Read<std::uint32_t>(handler, 0x3D60);
            const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (count == 100) break;
            Write(handler, 0x3D64 + count * 16, Read<std::uint32_t>(house, NC2Layout::HouseOffset::EngineType)); Write(handler, 0x3D68 + count * 16, Read<std::uint32_t>(house, NC2Layout::HouseOffset::MapPosition));
            Write(handler, 0x3D6C + count * 16, Read<std::uint32_t>(house, 0x20)); Write(handler, 0x3D70 + count * 16, Read<std::uint32_t>(house, 0x7CC));
            Write(handler, 0x3D60, count + 1);
        }
    }

    // Original function: FUN_0042b41e.
    bool AITaskSimpleDescription_IsEqualTo(const void* description, const void* other)
    {
        const auto type = Read<std::uint32_t>(description, 0);
        if (type != Read<std::uint32_t>(other, 0) || (type != 1 && type != 2)) return false;
        return NXBasics::Runtime_CompareMemory(static_cast<const std::byte*>(description) + 4, static_cast<const std::byte*>(other) + 4, type == 1 ? 16 : 24) == 0;
    }

    // Original function: FUN_0042b450.
    bool AITaskSimpleDescription_Tool_GetMapPosition(const void* description, void* point)
    {
        const auto type = Read<std::uint32_t>(description, 0);
        if (type != 1 && type != 2 && type != 3 && type != 5 && type != 8) return false;
        MapMigPoint_Construct(point, Read<std::uint16_t>(description, 4), Read<std::uint16_t>(description, 6), false);
        return true;
    }

    // Original function: FUN_0042b489.
    void AIMainTask_Init(void* task, std::uint32_t type, std::int32_t priority, std::uint32_t condition)
    {
        NXBasics::Runtime_FillMemory(task, 0, 0x34);
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(task) + 0x34, 0, 16);
        Write(task, 8, type); Write(task, 0x28, priority); Write(task, 0x2C, condition); Write(task, 0x30, gGameTime.tick);
    }

    // Original function: FUN_0042b4c7.
    void AIActiveCombinedMainTasks_Init(void* task, std::uint32_t mainTask, std::int32_t priority, const void* description)
    {
        NXBasics::Runtime_FillMemory(task, 0, 0x58);
        const auto type = Read<std::uint32_t>(description, 0);
        Write(task, 0, type); Write(task, 4, priority); Write(task, 0x14, mainTask); Write<std::uint32_t>(task, 0x10, 1);
        if (type == 1 || type == 2)
        {
            Write(task, 0x3C, Read<std::uint32_t>(description, 0xC)); Write(task, 0x40, Read<std::uint32_t>(description, 0x10)); Write(task, 0x44, Read<std::uint32_t>(description, 8));
            if (type == 2)
            {
                MapMigPoint_Construct(static_cast<std::byte*>(task) + 0x50, Read<std::uint16_t>(description, 0x14), Read<std::uint16_t>(description, 0x16), false);
                Write<std::uint32_t>(task, 0x48, 2);
            }
        }
    }

    // Original function: FUN_0042b543; the secondary center is averaged with truncation toward zero.
    bool AIActiveCombinedMainTasks_AddMainTask(void* task, std::uint32_t mainTask, std::int32_t priority, const void* description)
    {
        const auto count = Read<std::uint32_t>(task, 0x10);
        if (count == 10) return false;
        Write(task, 4, Read<std::uint32_t>(task, 4) + static_cast<std::uint32_t>(priority));
        Write(task, 0x14 + count * 4, mainTask); Write(task, 0x10, count + 1);
        const auto type = Read<std::uint32_t>(task, 0);
        if (type == 1 || type == 2)
        {
            Write(task, 0x3C, Read<std::uint32_t>(task, 0x3C) + Read<std::uint32_t>(description, 0xC));
            Write(task, 0x40, Read<std::uint32_t>(task, 0x40) + Read<std::uint32_t>(description, 0x10));
            const auto radius = Read<std::uint32_t>(description, 8);
            if (radius > Read<std::uint32_t>(task, 0x44)) Write(task, 0x44, radius);
            if (type == 2)
            {
                const auto x = (static_cast<std::int32_t>(Read<std::int16_t>(task, 0x50)) + Read<std::int16_t>(description, 0x14)) / 2;
                const auto y = (static_cast<std::int32_t>(Read<std::int16_t>(task, 0x52)) + Read<std::int16_t>(description, 0x16)) / 2;
                MapMigPoint_Construct(static_cast<std::byte*>(task) + 0x50, static_cast<std::int16_t>(x), static_cast<std::int16_t>(y), false);
            }
        }
        return true;
    }

    // Original function: FUN_0042b5d3.
    void AIConditionToConditionCheck_Tool_Init(void* check, std::uint32_t operation)
    {
        Write<std::uint32_t>(check, 4, 0); Write(check, 0, operation);
        for (std::uint32_t i = 0; i < 10; ++i) Write<std::uint32_t>(check, 8 + i * 4, NXBasics::IntegerRange::UInt32Maximum);
    }

    // Original function: FUN_0044e09a; Windows combines matching type-2 centers only.
    void AISinglePlayerHandler_l_CombinedActiveMainTask_UpdateList(void* handler)
    {
        Write<std::uint32_t>(handler, 0x2A48, 0);
        const auto count = Read<std::uint32_t>(handler, 0x2978);
        if (count == 0) return;
        for (std::uint32_t i = 0; i < count; ++i)
        {
            const auto id = Read<std::uint32_t>(handler, 0x297C + i * 4);
            const auto* main = static_cast<const std::byte*>(handler) + 0x1C30 + id * 0x44;
            const auto type = Read<std::uint32_t>(main, 8);
            bool add = true;
            if (type == 1 || type == 2)
                for (std::uint32_t j = 0; j < Read<std::uint32_t>(handler, 0x2A48); ++j)
                {
                    auto* combined = static_cast<std::byte*>(handler) + 0x2A50 + j * 0x58;
                    if (Read<std::uint32_t>(combined, 0) != type) continue;
                    const auto* first = static_cast<const std::byte*>(handler) + 0x1C30 + Read<std::uint32_t>(combined, 0x14) * 0x44;
                    if (type == 2 && Read<std::uint16_t>(main, 0xC) == Read<std::uint16_t>(first, 0xC) && Read<std::uint16_t>(main, 0xE) == Read<std::uint16_t>(first, 0xE) && AIActiveCombinedMainTasks_AddMainTask(combined, id, Read<std::int32_t>(main, 4), main + 8)) { add = false; break; }
                }
            if (add)
            {
                const auto slot = Read<std::uint32_t>(handler, 0x2A48);
                if (slot == 50) break;
                AIActiveCombinedMainTasks_Init(static_cast<std::byte*>(handler) + 0x2A50 + slot * 0x58, id, Read<std::int32_t>(main, 4), main + 8);
                Write(handler, 0x2A48, slot + 1);
            }
        }
        const auto combinedCount = Read<std::uint32_t>(handler, 0x2A48);
        std::uint32_t total = 0;
        for (std::uint32_t i = 0; i < combinedCount; ++i) total += Read<std::uint32_t>(handler, 0x2A54 + i * 0x58);
        if (total != 0)
            for (std::uint32_t i = 0; i < combinedCount; ++i) Write(handler, 0x2A58 + i * 0x58, static_cast<double>(Read<std::uint32_t>(handler, 0x2A54 + i * 0x58)) * 100.0 / total);
        for (std::uint32_t end = combinedCount; end > 1; --end)
            for (std::uint32_t i = 0; i + 1 < end; ++i)
            {
                auto* first = static_cast<std::byte*>(handler) + 0x2A50 + i * 0x58;
                if (Read<std::int32_t>(first, 4) >= Read<std::int32_t>(first, 0x5C)) continue;
                alignas(8) std::byte temporary[88];
                AIActiveCombinedMainTasks_CopyConstruct(temporary, first);
                for (std::uint32_t offset = 0; offset < 0x58; offset += 4) Write(first, offset, Read<std::uint32_t>(first, offset + 0x58));
                for (std::uint32_t offset = 0; offset < 0x58; offset += 4) Write(first, offset + 0x58, Read<std::uint32_t>(temporary, offset));
            }
    }

    // Original function: FUN_0044e26d; the native copy constructor leaves trailing padding untouched.
    void* AIActiveCombinedMainTasks_CopyConstruct(void* task, const void* other)
    {
        Write(task, 0, Read<std::uint32_t>(other, 0)); Write(task, 4, Read<std::uint32_t>(other, 4)); Write(task, 8, Read<double>(other, 8));
        for (std::uint32_t offset = 0x10; offset < 0x50; offset += 4) Write(task, offset, Read<std::uint32_t>(other, offset));
        Write(task, 0x50, Read<std::uint16_t>(other, 0x50)); Write(task, 0x52, Read<std::uint16_t>(other, 0x52));
        return task;
    }

    // Original function: FUN_0044e433; each store reloads the native count, retaining alias behavior.
    bool AISinglePlayerHandler_l_BuildHouses_AddBuildHouse(void* handler, std::uint32_t type, const void* point, std::uint32_t stage, std::uint32_t requested)
    {
        if (Read<std::uint32_t>(handler, 0x3D60) == 100) return false;
        Write(handler, 0x3D64 + Read<std::uint32_t>(handler, 0x3D60) * 16, type);
        Write(handler, 0x3D68 + Read<std::uint32_t>(handler, 0x3D60) * 16, Read<std::uint32_t>(point, 0));
        Write(handler, 0x3D6C + Read<std::uint32_t>(handler, 0x3D60) * 16, stage);
        Write(handler, 0x3D70 + Read<std::uint32_t>(handler, 0x3D60) * 16, requested);
        Write(handler, 0x3D60, Read<std::uint32_t>(handler, 0x3D60) + 1);
        return true;
    }

    // Original function: FUN_0042a53f; the player index is unchecked in Windows.
    bool AIManager_EntryPoint_MainTask_Add(void* manager, std::uint32_t player, const void* task)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_MainTask_Add(handler, task) != NXBasics::IntegerRange::UInt32Maximum;
    }

    // Original function: FUN_0042a5bf.
    bool AIManager_EntryPoint_SingleSoldier_SetDefaultPosition(void* manager, std::uint32_t player, const void* point, std::uint32_t radius)
    {
        void* handler = Read<void*>(manager, player * 4u);
        return handler != nullptr && AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(handler, point, radius);
    }

    // Original function: FUN_0044df73; the highest slot is updated before task validation.
    std::uint32_t AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(void* handler)
    {
        for (std::uint32_t slot = 0; slot < 50; ++slot)
        {
            if (Read<std::uint32_t>(handler, 0x1C38u + slot * 0x44u) != 0) continue;
            if (slot > Read<std::uint32_t>(handler, 0x1C2C)) Write(handler, 0x1C2C, slot);
            return slot;
        }
        return NXBasics::IntegerRange::UInt32Maximum;
    }

    // Original function: FUN_0044df9a; distinct primary/secondary border checks precede a forward DWORD copy.
    std::uint32_t AISinglePlayerHandler_MainTask_Add(void* handler, const void* task)
    {
        const auto slot = AISinglePlayerHandler_l_MainTask_GetFreeArraySlot(handler);
        if (slot == NXBasics::IntegerRange::UInt32Maximum) return slot;
        const auto type = Read<std::uint32_t>(task, 8);
        if (type == 1 || type == 2 || type == 3 || type == 5)
        {
            std::uint32_t point;
            MapMigPoint_SetPosition(&point, Read<std::int16_t>(task, 0x0C), Read<std::int16_t>(task, 0x0E), false);
            if (MapMigPoint_IsBorderPoint(&point)) return NXBasics::IntegerRange::UInt32Maximum;
            if (type == 2)
            {
                MapMigPoint_Construct(&point, Read<std::int16_t>(task, 0x1C), Read<std::int16_t>(task, 0x1E), false);
                if (MapMigPoint_IsBorderPoint(&point)) return NXBasics::IntegerRange::UInt32Maximum;
            }
        }
        void* destination = static_cast<std::byte*>(handler) + 0x1C30u + slot * 0x44u;
        for (std::uint32_t offset = 0; offset < 0x44; offset += 4) Write(destination, offset, Read<std::uint32_t>(task, offset));
        Write<std::uint8_t>(handler, 6, 1);
        return slot;
    }

    // Original function: FUN_0045afe1; only a non-border point on a nonzero continent type is accepted.
    bool AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(void* handler, const void* point, std::uint32_t radius)
    {
        if (MapMigPoint_IsBorderPoint(point)) return false;
        if (MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(point)) == 0) return false;
        Write<std::uint8_t>(handler, 0xA32C, 1);
        Write(handler, 0xA32E, Read<std::uint32_t>(point, 0));
        Write(handler, 0xA334, radius);
        return true;
    }
}
