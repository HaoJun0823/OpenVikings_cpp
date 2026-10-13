#include "ReadableValues.h"
#include "C2PlayerStatisticAccess.h"
#include "C2HumanIterator.h"
#include "C2OverviewObjectIterators.h"
#include "C2LogicObjectArrays.h"
#include "C2HouseAccess.h"
#include "C2StaticHouseTypeData.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "XCrtFileAccess.h"
#include <cstring>
namespace NC2Logic
{
    using NC2Lifetime::Read;
    using NC2Lifetime::Write;
    // Original function: FUN_0041acf8.
    SPlayerStatisticSampleData* PlayerStatisticManager_GetStatistics(void* manager, std::int32_t time, std::uint32_t player)
    {
        if (time < 0) return nullptr;
        auto* history = Read<SPlayerStatisticSampleData*>(manager, 8u + player * 4u);
        if (history == nullptr) return Read<SPlayerStatisticSampleData*>(manager, 0x58u + player * 4u);
        const auto sampleTime = Read<std::uint32_t>(manager, 4);
        const auto current = Read<std::uint32_t>(manager, 0);
        if (static_cast<std::uint32_t>(time) >= sampleTime) return history + current;
        const auto distance = (sampleTime - static_cast<std::uint32_t>(time) + 59u) / 60u;
        if (distance > 598) return nullptr;
        auto index = static_cast<std::int32_t>(current - distance);
        if (index < 0) index += 600;
        return history + index;
    }
    // Original function: FUN_0041ad54.
    SPlayerStatisticSampleData* PlayerStatisticManager_GetCurrentStatistics(void* manager, std::uint32_t player) { return PlayerStatisticManager_GetStatistics(manager, static_cast<std::int32_t>(gGameTime.tick / 12u), player); }
    // Original function: FUN_00440b4c.
    std::int32_t Tool_PlayerStatistics_CountHumansOfJobExecuting(std::uint32_t player, std::uint32_t job)
    {
        std::int32_t count = 0; CHumanIterator iterator;
        for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId()) if (Read<std::uint32_t>(GetHumanPtr(id), 0x18) == job) ++count;
        return count;
    }
    // Original function: FUN_00440b92; complete Windows jump table 00440C6D..00440CAC.
    std::int32_t SPlayerStatisticSampleData::GetNumber(std::int32_t type) const
    {
        if (type >= 16 && type <= 81) return goods[type - 16];
        if (type >= 0 && type <= 14) return Read<std::uint16_t>(this, static_cast<std::uint32_t>(type) * 2u);
        if (type == 15) return static_cast<std::int32_t>(goodTotal);
        if (type >= 82 && type <= 84) return static_cast<std::int32_t>(values114[type - 82]);
        if (type >= 85 && type <= 86) return static_cast<std::int32_t>(values130[type - 85]);
        return 0;
    }
    // Original function: FUN_00440af8.
    std::int32_t Tool_PlayerStatistics_CountHousesOfLogicType(std::uint32_t player, std::uint32_t type, std::int32_t* unfinished)
    {
        *unfinished = 0;
        std::int32_t total = 0;
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId())
        {
            const auto* house = static_cast<const std::byte*>(CHouseArray::mStaticVars) + static_cast<std::uint32_t>(id) * 0x7E0u;
            if (Read<std::uint32_t>(house, 0x14) != type) continue;
            ++total;
            if (Read<std::uint32_t>(house, 0x2C) != 3) ++*unfinished;
        }
        return total;
    }
    // Original function: FUN_0041aa59.
    void* __fastcall PlayerStatisticManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gPlayerStatisticManager = manager;
        std::memset(manager, 0, 168);
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            const auto* playerData = static_cast<const std::byte*>(gGameControl) + player * ReadableValues::RecordSize::GameControlPlayer;
            if (Read<std::uint8_t>(playerData, 0) == 0) continue;
            if (Read<std::uint32_t>(playerData, 4) == 1)
            {
                void* history = NXBasics::Runtime_AllocateWithNewHandler(187200, 1);
                Write<void*>(manager, 8u + player * 4u, history);
                std::memset(history, 0, 187200);
            }
            else
            {
                void* sample = NXBasics::Runtime_AllocateWithNewHandler(312, 1);
                Write<void*>(manager, 0x58u + player * 4u, sample);
                std::memset(sample, 0, 312);
            }
        }
        PlayerStatisticManager_CreateStatistics(manager);
        return manager;
    }
    // Original function: FUN_004408ee.
    void Tool_PlayerStatistics_CountHumans(std::uint32_t player, std::uint16_t* counts)
    {
        std::memset(counts, 0, 20);
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstHumanId(player); id != -1; id = iterator.GetNextHumanId())
        {
            const auto* human = GetHumanPtr(id);
            ++counts[0];
            if (Human_IsAgeAdult(human))
            {
                if (Human_IsSexMale(human))
                {
                    ++counts[1];
                    if (!Human_IsAttachedToHomeHouse(human)) { ++counts[3]; ++counts[8]; }
                    if (!Human_IsAttachedToWorkHouse(human)) ++counts[2];
                    if (Human_IsJobSoldier(human)) ++counts[9];
                }
                else
                {
                    ++counts[4];
                    if (!Human_IsAttachedToHomeHouse(human)) { ++counts[5]; ++counts[8]; }
                }
            }
            else
            {
                ++counts[6];
                if (!Human_IsAttachedToHomeHouse(human)) { ++counts[7]; ++counts[8]; }
            }
        }
    }
    // Original function: FUN_004409c1.
    void Tool_PlayerStatistics_CountHouses(std::uint32_t player, std::uint16_t* counts)
    {
        std::memset(counts, 0, 120);
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId())
        {
            const auto* house = reinterpret_cast<const std::byte*>(CHouseArray::mStaticVars) + static_cast<std::uint32_t>(id) * 0x7E0u;
            ++counts[0];
            const bool completed = Read<std::uint32_t>(house, 0x2C) == 3;
            if (completed) ++counts[5u + Read<std::uint32_t>(house, 0x14)];
            if (House_GetHouseMainType(house) == 2) ++counts[completed ? 1 : 2];
            else ++counts[completed ? 3 : 4];
        }
    }
    // Original function: FUN_00440a53.
    void Tool_PlayerStatistics_CountGoods(std::uint32_t player, void* counts)
    {
        std::memset(counts, 0, 136);
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(player); id != -1; id = iterator.GetNextHouseId())
        {
            const auto* house = reinterpret_cast<const std::byte*>(CHouseArray::mStaticVars) + static_cast<std::uint32_t>(id) * 0x7E0u;
            const auto* type = House_GetHouseTypeDataPtr(house);
            for (std::uint32_t stock = 0; stock < type->stockCount; ++stock)
            {
                const auto good = static_cast<std::uint32_t>(type->stock[stock].goodType);
                const auto amount = House_Stock_GetAmountOfGoods(house, good, false);
                Write<std::uint32_t>(counts, 0, Read<std::uint32_t>(counts, 0) + amount);
                auto value = static_cast<std::uint32_t>(Read<std::uint16_t>(counts, 4u + good * 2u)) + amount;
                if (value > ReadableValues::IntegerRange::UInt16Maximum) value = ReadableValues::IntegerRange::UInt16Maximum;
                Write<std::uint16_t>(counts, 4u + good * 2u, static_cast<std::uint16_t>(value));
            }
        }
    }
    // Original function: FUN_0041adaf.
    void PlayerStatisticManager_CreateStatisticSamples(std::uint32_t player, SPlayerStatisticSampleData* sample)
    {
        std::memset(sample, 0, 312);
        Tool_PlayerStatistics_CountHumans(player, sample->humans);
        Tool_PlayerStatistics_CountHouses(player, sample->houses);
        Tool_PlayerStatistics_CountGoods(player, &sample->goodTotal);
        const auto* playerData = static_cast<const std::byte*>(gGameControl) + player * ReadableValues::RecordSize::GameControlPlayer;
        sample->values114[0] = Read<std::uint32_t>(playerData, 0x4A14);
        sample->values114[1] = Read<std::uint32_t>(playerData, 0x4A18);
        sample->values114[2] = Read<std::uint32_t>(playerData, 0x4A1C);
        sample->values130[0] = Read<std::uint32_t>(playerData, 0x460C);
        sample->values130[1] = Read<std::uint32_t>(playerData, 0x4610);
    }
    // Original function: FUN_0041ad6f.
    void PlayerStatisticManager_CreateStatistics(void* manager)
    {
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            auto* history = Read<std::byte*>(manager, 8u + player * 4u);
            if (history != nullptr) PlayerStatisticManager_CreateStatisticSamples(player, reinterpret_cast<SPlayerStatisticSampleData*>(history + Read<std::uint32_t>(manager, 0) * 0x138u));
            auto* current = Read<SPlayerStatisticSampleData*>(manager, 0x58u + player * 4u);
            if (current != nullptr) PlayerStatisticManager_CreateStatisticSamples(player, current);
        }
    }
    // Original function: FUN_0041acd6.
    void __fastcall PlayerStatisticManager_UpdateStatistics(void* manager, void* /*unusedEdx*/, std::uint32_t time)
    {
        const auto index = Read<std::uint32_t>(manager, 0) + 1u;
        Write<std::uint32_t>(manager, 0, index == 600 ? 0 : index);
        PlayerStatisticManager_CreateStatistics(manager);
        Write<std::uint32_t>(manager, 4, time);
    }
}
