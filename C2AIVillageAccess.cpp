#include "C2AIVillageAccess.h"
#include "C2AIProduction.h"
#include "C2AIResourceAccess.h"
#include "C2AIObjectAccess.h"
#include "C2AIBuildingAccess.h"
#include "C2AIHumanRequests.h"
#include "C2HumanInitialization.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MissionStartObjects.h"
#include "C2VehicleAccess.h"
#include "C2EngineDisplayAccess.h"
#include "C2OverviewObjectIterators.h"
#include "C2HouseBuildPosition.h"
#include "C2GameMapAccess.h"
#include "C2AIGlobalGoods.h"
#include "C2GameState.h"
#include "C2HouseAccess.h"
#include "C2StaticHouseTypeData.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_00453c3a.
    void AIVillagesData_l_Village_Add(void* villages, std::uint32_t type, std::int32_t good, std::uint32_t player, const void* point, std::uint32_t radius)
    {
        if (Read<std::uint32_t>(villages, 0) == 8) return;
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0) continue;
            AIVillageManager_Init(slot + 12, type, good, Read<std::uint32_t>(villages, 4), player, point, radius);
            Write<std::uint8_t>(slot, 8, 1); Write(villages, 4, Read<std::uint32_t>(villages, 4) + 1); Write(villages, 0, Read<std::uint32_t>(villages, 0) + 1);
            return;
        }
    }
    // Original function: FUN_00453c90.
    void AIVillagesData_l_Village_Clear(void* villages, std::int32_t id)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) == 0 || Read<std::int32_t>(slot, 16) != id) continue;
            Write<std::uint8_t>(slot, 8, 0); Write(villages, 0, Read<std::uint32_t>(villages, 0) - 1); return;
        }
    }
    // Original function: FUN_00453648; the existing count is not reset by this entry.
    void AIVillagesData_Init(void* villages, std::uint32_t player, const void* point)
    {
        Write<std::uint32_t>(villages, 4, 0);
        for (std::uint32_t i = 0; i < 8; ++i) Write<std::uint8_t>(villages, i * 0xCF0 + 8, 0);
        AIVillagesData_l_Village_Add(villages, 3, 0, player, point, 50);
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstSectorHouseId(player, point, 10000, false); id != -1; id = iterator.GetNextSectorHouseId())
        {
            const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            const auto* door = House_Door_GetEntryPointRef(house);
            if (AIVillagesData_GetVillageId(villages, door) == -1) AIVillagesData_l_Village_Add(villages, 0, 0, player, door, 25);
        }
    }
    // Original function: FUN_00453809; retain the last attempted result when all villages fail.
    std::int32_t AIVillagesData_House_BuildHouse(void* villages, std::uint32_t house, bool disableOnFailure, MapSearchCallback callback, std::uint32_t userData)
    {
        std::int32_t result = 0;
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* slot = static_cast<std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) == 0) continue;
            result = AIVillageManager_House_BuildHouse(slot + 12, house, slot + 24, disableOnFailure, callback, userData);
            if (result != 0 && result != -3) break;
        }
        return result;
    }
    // Original function: FUN_0045384b.
    bool AIVillagesData_Military_IsBarrackBuild(const void* villages)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && AIHouseManager_Military_IsBarrackBuild(slot + 40)) return true;
        }
        return false;
    }
    // Original function: FUN_00453878.
    std::int32_t AIVillagesData_Military_GetBarrackHouseId(const void* villages)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* slot = static_cast<const std::byte*>(villages) + i * 0xCF0;
            if (Read<std::uint8_t>(slot, 8) != 0 && AIHouseManager_Military_IsBarrackBuild(slot + 40)) return Read<std::int32_t>(slot, 0x27C);
        }
        return -1;
    }
    // Original function: FUN_00453a07; unwrapped ring traversal preserves the original candidate order.
    bool AIVillagesData_l_GetVillageFreePoint(const void* villages, void* point, std::uint32_t radius)
    {
        auto candidate = Read<std::uint32_t>(point, 0);
        const auto player = Read<std::int32_t>(villages, 20);
        const auto tribe = player < 20 ? Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80 + 12) : 1u;
        std::uint32_t stage;
        const auto engine = NC2E2::Engine2d_EntryPoint_GetHouseId(tribe, 7, &stage);
        if (engine == -1) return false;
        const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(villages) + 24), 6);
        if (continent != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6)) return false;
        if (AIVillagesData_GetVillageId(villages, &candidate) == -1 && Tool_House_IsAllowedToBeBuild(player, engine, stage, candidate, false, false)) return true;
        for (std::uint32_t ring = 1; ring < radius; ++ring)
        {
            MapMigPoint_MoveCursorUnwrapped(candidate, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::uint32_t step = 0; step < ring; ++step)
                {
                    MapMigPoint_MoveCursorUnwrapped(candidate, direction);
                    if (!MapMigPoint_IsInsideMap(&candidate) || continent != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&candidate), 6) || AIVillagesData_GetVillageId(villages, &candidate) != -1) continue;
                    if (Tool_House_IsAllowedToBeBuild(player, engine, stage, candidate, false, false)) { Write(point, 0, candidate); return true; }
                }
        }
        return false;
    }
    // Original function: FUN_004538e6; successful requests are deferred until the scout confirms the position.
    std::int32_t AIVillagesData_Villages_CreateNew(void* villages, std::uint32_t type, std::uint32_t good, void* resources, void* invalidAreas)
    {
        if (Read<std::uint8_t>(villages, 0x6790) != 0 || Read<std::uint32_t>(villages, 0) >= 8) return -2;
        std::uint32_t point = 0;
        if (type == 0)
        {
            point = Read<std::uint32_t>(villages, 24);
            const auto maximum = gMapPointDimensions.microHeight > gMapPointDimensions.microWidth ? gMapPointDimensions.microHeight : gMapPointDimensions.microWidth;
            if (!AIVillagesData_l_GetVillageFreePoint(villages, &point, static_cast<std::uint32_t>(static_cast<std::int32_t>(maximum)))) return 0;
        }
        else if (type == 1)
        {
            if (!AIResourceManager_GetResourcePosition(resources, good, &point)) return 0;
            for (;;)
            {
                const auto player = Read<std::uint32_t>(AIVillagesData_GetVillageDataById(villages, 0), 8);
                if (!AI_IsEnemyObjectAround(player, &point, 25)) break;
                std::uint32_t area[2] = {point, 50};
                AIInvalidAreaManager_AddArea(invalidAreas, area);
                AIResourceManager_UpdateGoodResource(resources, good, invalidAreas, villages);
                if (!AIResourceManager_GetResourcePosition(resources, good, &point)) return 0;
            }
            if (!AIVillagesData_l_GetVillageFreePoint(villages, &point, 25)) return 0;
        }
        else if (type == 2 && !AIVillagesData_l_GetVillageFreePoint(villages, &point, 25)) return 0;
        Write(villages, 0x6788, type); Write(villages, 0x6792, point); Write(villages, 0x678C, good);
        Write<std::uint8_t>(villages, 0x6790, 1); Write<std::uint8_t>(villages, 0x6791, 0);
        return 1;
    }
    namespace
    {
        // Windows stack candidate list: count plus five (stage, engine type) pairs.
        struct SWorkHouseCandidates
        {
            std::uint32_t count = 0;
            std::uint32_t entries[10];
            // Original function: FUN_00471988; return false once all five slots are occupied.
            bool Add(std::int32_t engine, std::uint32_t stage)
            {
                if (count >= 5) return false;
                entries[count * 2 + 1] = static_cast<std::uint32_t>(engine); entries[count * 2] = stage; ++count;
                return true;
            }
            // Original function: FUN_00471938.
            std::int32_t AddNext(std::uint32_t tribe, std::uint32_t logic, std::int32_t previous)
            {
                std::uint32_t stage = logic;
                const auto engine = previous == -1 ? NC2E2::Engine2d_EntryPoint_GetHouseId(tribe, logic, &stage) : NC2E2::Engine2d_EntryPoint_GetNextHouseId(previous, tribe, logic, &stage);
                if (engine == -1 || count >= 5) return -1;
                entries[count * 2 + 1] = static_cast<std::uint32_t>(engine); entries[count * 2] = stage; ++count;
                return engine;
            }
        };
        static_assert(sizeof(SWorkHouseCandidates) == 44);
        std::uint32_t VillageTribe(const void* village)
        {
            const auto player = Read<std::int32_t>(village, 8);
            return player < 20 ? Read<std::uint32_t>(gGameControl, static_cast<std::uint32_t>(player) * 0x4A80 + 12) : 1u;
        }
        void* House(std::int32_t id) { return CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize; }
    }
    // Original function: FUN_0047161d.
    std::int32_t AIVillageManager_Good_GetWorkHouseId(void* village, std::uint32_t good)
    {
        const auto player = Read<std::uint32_t>(village, 8);
        const auto* high = Read<void*>(gAIManager, 0x50 + player * 4);
        const auto* data = static_cast<const std::byte*>(high) + 0x6AB0 + good * 16;
        SWorkHouseCandidates candidates;
        const auto addTypes = [&](std::uint32_t logic)
        {
            std::uint32_t stage;
            auto engine = NC2E2::Engine2d_EntryPoint_GetHouseId(VillageTribe(village), logic, &stage);
            if (engine != -1 && candidates.Add(engine, stage))
                while (engine != -1) engine = candidates.AddNext(VillageTribe(village), logic, engine);
        };
        if (Read<std::uint32_t>(data, 0) == 1) { addTypes(7); addTypes(1); }
        else addTypes(Read<std::uint32_t>(data, 8));
        CHouseIterator iterator;
        for (std::uint32_t list = 0; list < 2; ++list)
            for (auto id = iterator.GetFirstSectorHouseId(player, static_cast<std::byte*>(village) + 12, Read<std::uint32_t>(village, 16), list != 0); id != -1; id = iterator.GetNextSectorHouseId())
            {
                auto* house = House(id);
                if (Read<std::uint8_t>(house, 0x7D0) != 0) continue;
                std::uint32_t match = 0;
                while (match < candidates.count && (candidates.entries[match * 2 + 1] != Read<std::uint32_t>(house, 16) || candidates.entries[match * 2] > Read<std::uint32_t>(house, 32))) ++match;
                if (match == candidates.count) continue;
                if (Read<std::uint32_t>(house, 44) != 3) return -2;
                const auto job = Read<std::uint32_t>(data, 4);
                if (job == 0 || House_Inhabitants_IsFreeWorkSlotAvailable(house, job)) return id;
                House_GetHouseTypeDataPtr(house);
                for (std::uint32_t slot = 0; slot < 20; ++slot)
                {
                    if (Read<std::uint8_t>(House_Inhabitants_GetAttachedHumanSlotPtrBySlotId(house, slot), 0) == 0) continue;
                    auto* human = House_Inhabitants_GetAttachedHumanPtr(house, slot);
                    if (Read<std::uint32_t>(human, 24) != job) continue;
                    std::uint32_t target[3] = {1, Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8)};
                    if (AIHighLevelHandler_IsObjectUsed(high, target)) continue;
                    Human_DoAddNewUserCommand(human, 29, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 16)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
                    return -2;
                }
            }
        return -1;
    }
    // Original function: FUN_004719ac; collection houses may be attempted while input goods are missing.
    std::int32_t AIVillageManager_Good_AskForNewHouse(void* village, std::uint32_t good, const void* resources)
    {
        const auto* data = static_cast<const std::byte*>(resources) + good * 16;
        const auto collecting = Read<std::uint32_t>(data, 0x2FC) == 1;
        const auto logic = collecting ? 7u : Read<std::uint32_t>(data, 0x304);
        std::uint32_t stage;
        const auto engine = NC2E2::Engine2d_EntryPoint_GetHouseId(VillageTribe(village), logic, &stage);
        if (engine == -1) return 0;
        const auto player = Read<std::uint32_t>(village, 8);
        auto* center = static_cast<std::byte*>(village) + 12;
        const auto radius = Read<std::uint32_t>(village, 16);
        CHouseIterator iterator;
        for (std::uint32_t list = 0; list < 2; ++list)
            for (auto id = iterator.GetFirstSectorHouseId(player, center, radius, list != 0); id != -1; id = iterator.GetNextSectorHouseId())
            {
                auto* house = House(id);
                if (Read<std::uint32_t>(house, 16) != static_cast<std::uint32_t>(engine)) continue;
                if (Read<std::uint32_t>(house, 44) != 3) return -2;
                if (Read<std::uint32_t>(house, 32) < stage && AIVillageManager_House_TryToUpgradeHouse(village, house) != 0) return -2;
            }
        auto* goods = static_cast<std::byte*>(village) + 0x274;
        if (Read<std::uint8_t>(village, 0x25 + logic) == 0)
        {
            AIGoodManager_StopProduction(goods, good); AIGoodManager_ImportThisGoodType(goods, good);
            if (good == 4) AIGoodManager_SetTraderBringFlag(goods, 4, true);
            return 0;
        }
        auto* build = reinterpret_cast<SHouseBuild*>(static_cast<std::byte*>(village) + 0xCC4);
        if (build->IsValid()) return -2;
        const auto* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engine);
        bool supplied = true;
        for (std::uint32_t level = 0; level < stage + 1; ++level)
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(definition, 0x48 + level * 64); ++i)
                if (!AIGoodManager_IsProducingGood(goods, Read<std::uint32_t>(definition, 0x4C + level * 64 + i * 4), true)) supplied = false;
        if (!collecting && !supplied) return -1;
        void* sourceHouse = nullptr;
        const auto outputLogic = Read<std::uint32_t>(data, 0x304);
        if (Read<std::uint32_t>(resources, (outputLogic + 0x55) * 24) != 0)
            for (auto id = iterator.GetFirstSectorHouseId(player, center, radius, false); id != -1; id = iterator.GetNextSectorHouseId())
            {
                auto* house = House(id);
                if (AIResourceManager_IsExportingHouseType(resources, outputLogic, Read<std::uint32_t>(house, 20))) { sourceHouse = house; break; }
            }
        if (sourceHouse == nullptr)
            for (auto id = iterator.GetFirstSectorHouseId(player, center, radius, false); id != -1; id = iterator.GetNextSectorHouseId())
            {
                auto* house = House(id);
                const auto type = Read<std::uint32_t>(house, 20);
                if (type == 1 || type == 7 || type == 8 || type == 9) continue;
                if (House_Production_IsAbleToProduceGood(house, good, false)) { sourceHouse = house; break; }
            }
        std::uint32_t point = 0;
        const auto terrain = Read<std::uint8_t>(data, 0x308) != 0;
        const auto found = sourceHouse != nullptr && AIVillageManager_l_House_GetPossibleBuildPosition(village, House_Door_GetEntryPointRef(sourceHouse), engine, stage, &point, terrain, nullptr, 0);
        if (!found && !AIVillageManager_l_House_GetPossibleBuildPosition(village, center, engine, stage, &point, terrain, nullptr, 0)) { Write<std::uint8_t>(village, 0x25 + logic, 0); return 0; }
        if (AI_IsEnemyObjectAround(player, &point, 30)) return -2;
        build->BuildHouse(player, &point, engine, stage, false);
        return 1;
    }
    // Original function: FUN_0042905a.
    std::int32_t AIHighLevelHandler_Tool_GetUnusedHuman(const void* handler, const void* point)
    {
        std::int32_t best = -1;
        std::uint32_t distance = 10000;
        const auto player = Read<std::uint32_t>(handler, 0);
        const auto* single = Read<void*>(gAIManager, player * 4);
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstSectorHumanId(player, false); id != -1; id = iterator.GetNextSectorHumanId())
        {
            if (AISinglePlayerHandler_Misc_Human_CheckRequestsForCreature(single, id)) continue;
            const auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if ((Read<std::uint32_t>(human, 0xB84) & 32u) != 0 || Human_IsJobSoldier(human) || Human_IsSexFemale(human) || !Human_IsAgeAdult(human) || (Human_IsJobBuilder(human) && Human_IsAttachedToBuildSite(human))) continue;
            const auto* position = Human_GetMapPositionRef(human);
            const auto candidate = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2), Read<std::int16_t>(point, 0), Read<std::int16_t>(point, 2)));
            if (candidate >= distance) continue;
            std::uint32_t target[3] = {1, static_cast<std::uint32_t>(id), Read<std::uint32_t>(human, 8)};
            if (AIHighLevelHandler_IsObjectUsed(handler, target)) continue;
            distance = candidate; best = id;
        }
        return best;
    }
    // Original function: FUN_00471418.
    void AIVillageManager_Init(void* village, std::uint32_t type, std::int32_t good, std::uint32_t id, std::uint32_t player, const void* point, std::uint32_t radius)
    {
        Write(village, 0xCE4, type); Write(village, 0xCE8, good); Write(village, 12, Read<std::uint32_t>(point, 0));
        Write<std::int32_t>(village, 20, -1); Write(village, 16, radius); Write(village, 4, id); Write(village, 8, player);
        AIHouseManager_Init(static_cast<std::byte*>(village) + 28);
        const auto tribe = static_cast<std::int32_t>(player) < 20 ? Read<std::uint32_t>(gGameControl, player * 0x4A80 + 12) : 1u;
        AIGoodManager_Init(static_cast<std::byte*>(village) + 0x274, tribe);
        Write<std::uint8_t>(village, 0xCC4, 0); Write<std::uint8_t>(village, 0xCD0, 0); Write<std::int32_t>(village, 0xCE0, -1); Write<std::uint8_t>(village, 0, 1);
    }
    // Original function: FUN_0047154e.
    void AIVillageManager_CheckAndSetGoodData(void* village, const void* resources)
    {
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            if (Read<std::uint32_t>(resources, 0x2FC + good * 16) != 1) continue;
            std::uint32_t point = 0;
            if (!AIVillageManager_Good_GetWorkCenterPoint(village, good, &point) && Read<std::uint32_t>(village, 0x280 + good * 40) != 0) AIGoodManager_ImportThisGoodType(static_cast<std::byte*>(village) + 0x274, good);
        }
        AIGoodManager_DoOneLogicGameTick(static_cast<std::byte*>(village) + 0x274, village, resources);
    }
    // Original function: FUN_004715c4.
    bool AIVillageManager_Good_GetWorkCenterPoint(const void* village, std::uint32_t good, void* point)
    {
        const auto* high = Read<void*>(gAIManager, 0x50 + Read<std::uint32_t>(village, 8) * 4);
        return AIResourceManager_GetResourcePosition(static_cast<const std::byte*>(high) + 0x67B4, good, point) && AIVillage_IsPointInside(village, point);
    }
    // Original function: FUN_00471e40.
    std::int32_t AIVillageManager_Good_GetWorkHumanId(const void* village, const void* point)
    {
        return AIHighLevelHandler_Tool_GetUnusedHuman(Read<void*>(gAIManager, 0x50 + Read<std::uint32_t>(village, 8) * 4), point);
    }
    // Original function: FUN_004726bf.
    void AIHouseManager_Init(void* manager)
    {
        NXBasics::Runtime_FillMemory(static_cast<std::byte*>(manager) + 9, 1, 55);
        Write<std::int32_t>(manager, 0x254, -1); Write<std::uint8_t>(manager, 0, 0); Write<std::uint32_t>(manager, 0x40, 0); Write<std::uint32_t>(manager, 0xC0, 0); Write<std::uint8_t>(manager, 8, 0);
    }
    // Original function: FUN_004728ce.
    void AIHouseManager_ClearAllNoHouseCanBeBuildFlags(void* manager) { NXBasics::Runtime_FillMemory(static_cast<std::byte*>(manager) + 9, 1, 55); }
    // Original function: FUN_00472a6a.
    bool AIHouseManager_Military_IsBarrackBuild(const void* manager)
    {
        const auto id = Read<std::int32_t>(manager, 0x254);
        return id != -1 && Read<std::uint8_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize, 0) != 0;
    }
    namespace
    {
        // Original function: FUN_00472157.
        const void* AIVillageManager_GetCenterPoint(const void* village) { return static_cast<const std::byte*>(village) + 12; }
        // Original function: FUN_00471fd3.
        std::int32_t AIVillageManager_l_CheckForStock(void* village)
        {
            if (Read<std::uint32_t>(village, 0x5C) != 0) return 1;
            if (Read<std::uint8_t>(village, 0x60) != 0) return -2;
            if (Read<std::uint8_t>(village, 0x2C) == 0) return 0;
            std::uint32_t stage, point = 0;
            const auto engine = NC2E2::Engine2d_EntryPoint_GetHouseId(VillageTribe(village), 7, &stage);
            if (engine == -1) return -2;
            if (!AIVillageManager_l_House_GetPossibleBuildPosition(village, static_cast<std::byte*>(village) + 12, engine, stage, &point, false, nullptr, 0)) { Write<std::uint8_t>(village, 0x2C, 0); return 0; }
            auto* build = reinterpret_cast<SHouseBuild*>(static_cast<std::byte*>(village) + 0xCC4); build->active = 0;
            const auto player = Read<std::uint32_t>(village, 8);
            if (AI_IsEnemyObjectAround(player, &point, 30)) return -2;
            const bool fill = Read<std::uint32_t>(village, 0xCE4) == 3 && gGameTime.tick < 300;
            build->BuildHouse(player, &point, engine, stage, fill); return 1;
        }
        // Original function: FUN_004720a6.
        bool AIVillageManager_l_RemoveBuildingNearCenterPoint(void* village)
        {
            CHouseIterator houses; std::uint32_t distance = 10000; std::int32_t best = -1;
            for (auto id = houses.GetFirstSectorHouseId(Read<std::uint32_t>(village, 8), AIVillageManager_GetCenterPoint(village), Read<std::uint32_t>(village, 16), false); id != -1; id = houses.GetNextSectorHouseId())
            {
                const auto candidate = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(House(id), 28), Read<std::uint16_t>(House(id), 30), Read<std::uint16_t>(village, 12), Read<std::uint16_t>(village, 14)));
                if (candidate < distance) { distance = candidate; best = id; }
            }
            if (best == -1) return false;
            House_Misc_Destroy_House(House(best), nullptr); return true;
        }
        // Original function: FUN_0047215b.
        std::int32_t AIVillageManager_l_HomeAttachment_Update(void* village)
        {
            bool sent = false; std::uint32_t homes = 0, full = 0;
            const auto player = Read<std::uint32_t>(village, 8); const auto radius = Read<std::uint32_t>(village, 16);
            CHouseIterator houses;
            for (auto id = houses.GetFirstSectorHouseId(player, AIVillageManager_GetCenterPoint(village), radius, false); id != -1; id = houses.GetNextSectorHouseId())
            {
                if (sent) return -2;
                auto* house = House(id);
                if (House_GetHouseMainType(house) != 2 || Read<std::uint8_t>(house, 0x7D0) != 0) continue;
                ++homes;
                if (Read<std::uint32_t>(house, 44) != 3) continue;
                const auto* data = House_GetHouseTypeDataPtr(house);
                for (std::uint32_t slot = 0; slot < 20; ++slot)
                {
                    const auto flags = Read<std::uint8_t>(House_Inhabitants_GetAttachedHumanSlotPtrBySlotId(house, slot), 0);
                    if (flags == 0 && House_Inhabitants_GetNumberOfFamiliesLivingInHome(house, nullptr) < static_cast<std::uint32_t>(data->homeSize))
                    {
                        void* best = nullptr; std::uint32_t distance = 10000; CHumanIterator humans;
                        for (auto humanId = humans.GetFirstSectorHumanId(player, Read<std::uint32_t>(AIVillageManager_GetCenterPoint(village), 0), radius, false); humanId != -1; humanId = humans.GetNextSectorHumanId())
                        {
                            auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * CHumanArray::ElementSize;
                            if (!Human_IsAgeAdult(human) || !Human_IsSexMale(human) || (Read<std::uint32_t>(human, 0xB84) & 32) == 0 || !Human_IsAttachedToWorkHouse(human)) continue;
                            const auto* workPoint = static_cast<const std::byte*>(House(Read<std::int32_t>(human, 0xB34))) + 36;
                            if (!AIVillage_IsPointInside(village, workPoint)) continue;
                            const auto candidate = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(workPoint, 0), Read<std::uint16_t>(workPoint, 2), Read<std::uint16_t>(house, 36), Read<std::uint16_t>(house, 38)));
                            if (Human_IsAttachedToHomeHouse(human))
                            {
                                if (Read<std::uint32_t>(human, 0xB28) == Read<std::uint32_t>(house, 4)) continue;
                                const auto* homePoint = static_cast<const std::byte*>(House(Read<std::int32_t>(human, 0xB28))) + 36;
                                const auto oldDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(workPoint, 0), Read<std::uint16_t>(workPoint, 2), Read<std::uint16_t>(homePoint, 0), Read<std::uint16_t>(homePoint, 2)));
                                if (candidate >= oldDistance) continue;
                            }
                            if (candidate < distance) { distance = candidate; best = human; }
                        }
                        if (best) Human_DoAddNewUserCommand(best, 26, static_cast<std::uint8_t>(player), Read<std::uint32_t>(best, 4), Read<std::uint32_t>(best, 8), Read<std::uint32_t>(house, 4), Read<std::uint32_t>(house, 8));
                        return 1;
                    }
                    if (flags == 0) continue;
                    if ((flags & 1) != 0)
                    {
                        auto* human = House_Inhabitants_GetAttachedHumanPtr(house, slot);
                        if (Human_IsAgeAdult(human) && Human_IsSexMale(human) && (Read<std::uint32_t>(human, 0xB84) & 32) != 0)
                        {
                            const auto inVillage = Human_IsAttachedToWorkHouse(human) ? AIVillage_IsPointInside(village, static_cast<const std::byte*>(House(Read<std::int32_t>(human, 0xB34))) + 36) : Human_IsMarried(human);
                            if (!inVillage) { Human_DoAddNewUserCommand(human, 27, static_cast<std::uint8_t>(player), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0); sent = true; }
                        }
                    }
                }
                if (House_Inhabitants_GetNumberOfFamiliesLivingInHome(house, nullptr) == static_cast<std::uint32_t>(data->homeSize)) ++full;
            }
            if (sent) return -2;
            return full == homes ? 0 : 1;
        }
        // Original function: FUN_0047245f.
        void AIVillageManager_l_ExpandHomes(void* village)
        {
            const auto player = Read<std::uint32_t>(village, 8); CHouseIterator houses;
            for (auto id = houses.GetFirstSectorHouseId(player, AIVillageManager_GetCenterPoint(village), Read<std::uint32_t>(village, 16), false); id != -1; id = houses.GetNextSectorHouseId())
            {
                auto* house = House(id);
                if (House_GetHouseMainType(house) != 2 || Read<std::uint8_t>(house, 0x7D0) != 0) continue;
                if (Read<std::uint32_t>(house, 44) != 3 || AIVillageManager_House_TryToUpgradeHouse(village, house) != 0) return;
            }
            auto* build = reinterpret_cast<SHouseBuild*>(static_cast<std::byte*>(village) + 0xCC4);
            if (build->IsValid()) return;
            std::int32_t bestEngine = -1; std::uint32_t bestStage = 0, bestType = 0;
            for (std::uint32_t type = 2; type <= 6; ++type)
            {
                if (Read<std::uint8_t>(village, 0x25 + type) == 0) continue;
                std::uint32_t stage; const auto engine = NC2E2::Engine2d_EntryPoint_GetHouseId(VillageTribe(village), type, &stage);
                if (engine == -1) continue;
                const auto* data = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engine); bool goods = true;
                for (std::uint32_t level = 0; level < stage + 1; ++level)
                    for (std::uint32_t i = 0; i < Read<std::uint32_t>(data, 0x48 + level * 64); ++i)
                        if (!AIGoodManager_IsProducingGood(static_cast<std::byte*>(village) + 0x274, Read<std::uint32_t>(data, 0x4C + level * 64 + i * 4), true)) goods = false;
                if (!goods) break;
                bestEngine = engine; bestStage = stage; bestType = type;
            }
            if (bestEngine == -1) return;
            std::uint32_t point = 0;
            if (!AIVillageManager_l_House_GetPossibleBuildPosition(village, AIVillageManager_GetCenterPoint(village), bestEngine, bestStage, &point, false, nullptr, 0)) { Write<std::uint8_t>(village, 0x25 + bestType, 0); return; }
            if (!AI_IsEnemyObjectAround(player, &point, 30)) build->BuildHouse(player, &point, bestEngine, bestStage, false);
        }
        // Original function: FUN_004726eb; both sector lists contribute to the remembered count.
        void AIHouseManager_l_UpdateHouseList(void* manager, std::uint32_t player, const void* center, std::uint32_t radius)
        {
            Write<std::int32_t>(manager, 0x254, -1); Write<std::uint32_t>(manager, 0x40, 0); Write<std::uint32_t>(manager, 0xC0, 0);
            Write<std::uint8_t>(manager, 0, 0); Write<std::uint8_t>(manager, 0x44, 0);
            std::uint32_t count = 0; CHouseIterator houses;
            for (std::uint32_t list = 0; list < 2; ++list)
                for (auto id = houses.GetFirstSectorHouseId(player, center, radius, list != 0); id != -1; id = houses.GetNextSectorHouseId())
                {
                    ++count; auto* house = House(id); const auto type = Read<std::uint32_t>(house, 20);
                    if (list != 0)
                    {
                        const auto towers = Read<std::uint32_t>(manager, 0xC0);
                        if ((type == 40 || type == 41 || type == 1) && towers < 100) { Write(manager, 0xC4 + towers * 4, Read<std::uint32_t>(house, 28)); Write(manager, 0xC0, towers + 1); }
                        if (type == 39 && Read<std::uint32_t>(house, 44) == 3) { Write(manager, 0x254, id); continue; }
                    }
                    const auto main = House_GetHouseMainType(house);
                    if (main == 1)
                    {
                        if (Read<std::uint8_t>(house, 0x7D0) != 0 || Read<std::uint32_t>(house, 44) != 3) Write<std::uint8_t>(manager, 0x44, 1);
                        else
                        {
                            const auto stocks = Read<std::uint32_t>(manager, 0x40);
                            if (stocks != 10)
                            {
                                Write(manager, 0x48 + stocks * 12, Read<std::uint32_t>(house, 28)); Write(manager, 0x4C + stocks * 12, Read<std::uint32_t>(house, 4)); Write(manager, 0x50 + stocks * 12, Read<std::uint32_t>(house, 8)); Write(manager, 0x40, stocks + 1);
                            }
                        }
                    }
                    else if (list == 0 && main == 2) Write<std::uint8_t>(manager, 0, 1);
                }
            if (count != 0) Write<std::uint8_t>(manager, 8, 0);
            else if (Read<std::uint32_t>(manager, 4) > 0) Write<std::uint8_t>(manager, 8, 1);
            std::uint32_t type = 0;
            for (; type < 55; ++type) if (Read<std::uint8_t>(manager, 9 + type) == 0) break;
            if (type == 55) Write(manager, 4, count);
            else if (count < Read<std::uint32_t>(manager, 4)) { Write(manager, 4, count); AIHouseManager_ClearAllNoHouseCanBeBuildFlags(manager); }
        }
        // Original function: FUN_00472929; apply transition 11 to chest landscapes 85/86.
        void AIHouseManager_l_OpenChestsNearHouses(std::uint32_t player, const void* center, std::uint32_t radius)
        {
            CHouseIterator houses;
            for (auto id = houses.GetFirstSectorHouseId(player, center, radius, false); id != -1; id = houses.GetNextSectorHouseId())
            {
                auto point = Read<std::uint32_t>(House(id), 28);
                for (std::uint32_t ring = 1; ring < 20; ++ring)
                {
                    MapMigPoint_MoveCursorUnwrapped(point, 4);
                    for (std::uint32_t direction = 0; direction < 6; ++direction)
                        for (std::uint32_t step = 0; step < ring; ++step)
                        {
                            MapMigPoint_MoveCursorUnwrapped(point, direction);
                            if (!MapMigPoint_IsInsideMap(&point)) continue;
                            const auto* micro = GameMap_GetConstMapMicroPointPtr(&point);
                            const auto type = Read<std::uint8_t>(micro, 2);
                            if (Read<std::uint8_t>(micro, 0) == 1 && (type == 85 || type == 86)) GameMap_MapMicroPoint_Landscape_ApplyTransition(&point, 11);
                        }
                }
            }
        }
        // Original function: FUN_004728e1.
        void AIHouseManager_DoOneLogicTick(void* manager, std::uint32_t player, const void* center, std::uint32_t radius)
        {
            AIHouseManager_l_UpdateHouseList(manager, player, center, radius);
            if ((gGameTime.tick / 12) % 60 == 0 && gGameStaticVars.networkGame != 0) AIHouseManager_l_OpenChestsNearHouses(player, center, radius);
        }
    }
    // Original function: FUN_0047149a.
    void AIVillageManager_DoOneLogicTick(void* village, std::uint32_t tick)
    {
        auto* houses = static_cast<std::byte*>(village) + 28;
        if (tick % 600 == 0) AIHouseManager_ClearAllNoHouseCanBeBuildFlags(houses);
        AIHouseManager_DoOneLogicTick(houses, Read<std::uint32_t>(village, 8), AIVillageManager_GetCenterPoint(village), Read<std::uint32_t>(village, 16));
        const auto stock = AIVillageManager_l_CheckForStock(village);
        if (stock == -2) return;
        if (stock == 0 && !AIVillageManager_l_RemoveBuildingNearCenterPoint(village)) Write<std::uint8_t>(village, 36, 1);
        if (Read<std::uint8_t>(houses, 0) == 0) AIVillageManager_l_ExpandHomes(village);
        else
        {
            auto* goods = static_cast<std::byte*>(village) + 0x274;
            if (AIGoodManager_IsProducingGood(goods, 4, true) && AIGoodManager_IsProducingGoodRealy(goods, 4)) AIGoodManager_IsProducingGood(goods, 19, true);
        }
        if (AIVillageManager_l_HomeAttachment_Update(village) == 0) AIVillageManager_l_ExpandHomes(village);
        AITowerManager_DoOneLogicGameTick(static_cast<std::byte*>(village) + 0xCE0, Read<std::uint32_t>(village, 8), houses, village);
    }
    // Original function: FUN_00453712.
    void AIVillagesData_DoOneLogicTick(void* villages, const void* resources, void* goods, std::uint32_t tick)
    {
        auto* bytes = static_cast<std::byte*>(villages);
        if (Read<std::uint8_t>(villages, 0x6790) != 0 && Read<std::uint8_t>(villages, 0x6791) != 0)
        {
            AIVillagesData_l_Village_Add(villages, Read<std::uint32_t>(villages, 0x6788), Read<std::int32_t>(villages, 0x678C), Read<std::uint32_t>(villages, 20), bytes + 0x6792, 25); Write<std::uint8_t>(villages, 0x6790, 0);
        }
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* entry = bytes + i * 0xCF0;
            if (Read<std::uint8_t>(entry, 8) == 0) continue;
            if (Read<std::uint32_t>(entry, 0xCF0) != 3 && Read<std::uint8_t>(entry, 0x30) != 0) AIVillagesData_l_Village_Clear(villages, Read<std::int32_t>(entry, 16));
            else AIVillageManager_DoOneLogicTick(entry + 12, tick);
        }
        AIGlobalGoods_DoOneLogicGameTick(goods, villages);
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            auto* entry = bytes + i * 0xCF0;
            if (Read<std::uint8_t>(entry, 8) != 0) AIVillageManager_CheckAndSetGoodData(entry + 12, resources);
        }
    }
}
