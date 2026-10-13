#include "XNumericConstants.h"
#include "C2AIGuideAccess.h"
#include "C2AIMilitaryAccess.h"
#include "C2AIObjectAccess.h"
#include "C2EngineMapTerrainAccess.h"
#include "C2GameMapAccess.h"
#include "C2GuideAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2MapPathfinderSearch.h"
#include "C2MapSectorAttachment.h"
#include "C2MapSectorGridCursor.h"
#include "C2OverviewObjectIterators.h"
#include "C2VehicleAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        // Complete Windows static region 005125A0..005125F3: ten link IDs and points.
        struct SGuideCandidates { std::uint32_t count; std::uint32_t links[10]; std::uint32_t points[10]; } sGuideCandidates{};
        static_assert(sizeof(SGuideCandidates) == 84);
        std::uint8_t sGuideCandidateInitFlags = 0; // 005125F4.
        // Original function: FUN_00429e61.
        void AIGuideCandidateList_Construct(SGuideCandidates* candidates)
        {
            for (std::uint32_t i = 0; i < 10; ++i) { Write<std::uint16_t>(&candidates->points[i], 2, 0); Write<std::uint16_t>(&candidates->points[i], 0, 0); }
        }
        // Original function: FUN_00429e79; Windows registers this empty exit callback.
        int __cdecl AIGuideCandidateList_OnExit() { return 0; }
        struct SGuideBuildSearch { const void* source; const void* destination; std::uint32_t result; std::uint32_t distance; std::uint32_t player; std::int32_t danger; };
        static_assert(sizeof(SGuideBuildSearch) == 24);
        std::uint32_t Distance(const void* first, const void* second)
        {
            return static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(first, 0), Read<std::uint16_t>(first, 2), Read<std::uint16_t>(second, 0), Read<std::uint16_t>(second, 2)));
        }
        // Original function: FUN_00429d9c; player and danger belong to the caller.
        void* AIGuideBuildSearch_Construct(SGuideBuildSearch* search, const void* source, const void* destination)
        {
            search->destination = destination; search->source = source;
            Write<std::uint16_t>(&search->result, 2, 0); Write<std::uint16_t>(&search->result, 0, 0);
            search->distance = Distance(source, destination); return search;
        }
        // Original function: FUN_00429ddb.
        bool __cdecl slAIHighLevelSearchGuideBuildPos(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* data)
        {
            auto* search = static_cast<SGuideBuildSearch*>(data->userData);
            if (data->distance < 16) return false;
            const auto distance = Distance(point, search->destination);
            const auto terrain = (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 12) >> 3) & 15;
            const auto score = distance + terrain * terrain;
            if (score > search->distance || !Tool_Guide_IsAllowedToBeBuild(search->player, point, true)) return false;
            const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(point, search->player);
            if (danger >= search->danger) return false;
            search->result = Read<std::uint32_t>(point, 0); search->distance = score; search->danger = danger; return false;
        }
        // Original function: FUN_00429ef8.
        bool __cdecl slAIGuideManagerSearchGuideBuildPosNearPos(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* data)
        {
            auto* search = static_cast<SGuideBuildSearch*>(data->userData);
            if (!Tool_Guide_IsAllowedToBeBuild(search->player, point, true)) return false;
            const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(point, search->player);
            if (danger >= search->danger) return false;
            search->result = Read<std::uint32_t>(point, 0); search->danger = danger; return false;
        }
        void* GridEntry(const void* manager, const void* position)
        {
            const SSectorGridCursor sector(Read<std::uint32_t>(position, 0));
            return static_cast<std::byte*>(Read<void*>(manager, 0x20)) + static_cast<std::uint32_t>(sector.y * SSectorGridCursor::sGridWidth + sector.x) * 12;
        }
    }
    // Original function: FUN_00429e7a.
    bool AIGuideManager_IsGuideAlreadyBuild(const void*, std::uint32_t player, const void* point)
    {
        CGuideIterator guides; return guides.GetFirstGuideId(player, point, 20) != -1;
    }
    // Original function: FUN_00429ea5.
    void AIGuideManager_BuildGuideNearPosition(void* manager, std::uint32_t player, const void* point)
    {
        SGuideBuildSearch search; AIGuideBuildSearch_Construct(&search, point, point);
        search.player = player; search.danger = 10000;
        CMapPathfinderElement::SearchOverMap(point, 40, true, slAIGuideManagerSearchGuideBuildPosNearPos, &search, true, player);
        if (search.danger != 10000) Write(manager, 16, search.result);
    }
    // Original function: FUN_00429613.
    void AIGuideManager_CheckAndBuildGuides(void* manager, std::uint32_t player, void* military)
    {
        AIGuideManager_CalcVars_CreateGuideCalcArray(static_cast<std::byte*>(manager) + 24);
        if (Read<std::uint32_t>(manager, 24) == 0 || Read<std::uint32_t>(manager, 28) == 0) return;
        auto* destination = static_cast<std::byte*>(manager) + 16;
        if (Read<std::uint16_t>(destination, 0) != NXBasics::IntegerRange::UInt16MaximumSigned && !Tool_Guide_IsAllowedToBeBuild(player, destination, true)) Write<std::uint16_t>(destination, 0, NXBasics::IntegerRange::UInt16MaximumSigned);
        if (Read<std::uint16_t>(destination, 0) == NXBasics::IntegerRange::UInt16MaximumSigned)
        {
            NXBasics::Runtime_FillMemory(Read<void*>(manager, 32), 0, Read<std::uint32_t>(manager, 24) * Read<std::uint32_t>(manager, 28) * 12);
            CGuideIterator guides;
            for (auto id = guides.GetFirstGuideId(player); id != -1; id = guides.GetNextGuideId())
            {
                const auto* guide = CGuideArray::mStaticVars + static_cast<std::uint32_t>(id) * CGuideArray::ElementSize;
                auto* entry = GridEntry(manager, guide + 20);
                if (Read<std::uint8_t>(entry, 1) == 0) { Write<std::uint8_t>(entry, 1, 1); Write(entry, 4, Read<std::uint32_t>(guide, 8)); }
            }
            CHouseIterator houses;
            for (auto id = houses.GetFirstHouseId(player); id != -1; id = houses.GetNextHouseId())
            {
                const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                if (Read<std::uint8_t>(house, 0x7D0) != 0) continue;
                auto* entry = GridEntry(manager, house + 28);
                if (Read<std::uint8_t>(entry, 0) == 0) { Write<std::uint8_t>(entry, 0, 1); Write(entry, 8, Read<std::uint32_t>(house, 4)); }
            }
            AIGuideManager_l_CheckForNewGuideBuildPos(manager, player);
        }
        if (Read<std::uint16_t>(destination, 0) == NXBasics::IntegerRange::UInt16MaximumSigned)
        {
            if (Read<std::int32_t>(manager, 12) != -1) { AIMilitaryManager_MilitaryPoint_Clear(military, Read<std::int32_t>(manager, 12)); Write<std::int32_t>(manager, 12, -1); }
            AIScout_Clear(manager); return;
        }
        if (!AIScout_IsValid(manager))
        {
            if (Read<std::int32_t>(manager, 12) != -1) { AIMilitaryManager_MilitaryPoint_Clear(military, Read<std::int32_t>(manager, 12)); Write<std::int32_t>(manager, 12, -1); }
            Write<std::uint8_t>(manager, 8, 1); return;
        }
        auto* human = CHumanArray::mStaticVars + Read<std::uint32_t>(manager, 0) * CHumanArray::ElementSize;
        if (Read<std::int32_t>(manager, 12) == -1)
        {
            std::uint32_t target[3]{1, Read<std::uint32_t>(manager, 0), Read<std::uint32_t>(manager, 4)};
            Write(manager, 12, AIMilitaryManager_MilitaryPoint_Add(military, 1, 0, target, 40, 1, 1));
        }
        if (AIMilitaryManager_Military_GetCivilianDangerValueForPosition(Human_GetMapPositionRef(human), player) > 10) { AIScout_MoveToPos(manager, Human_GetMapPositionRef(human)); return; }
        if (!AIScout_CheckAndChangeJob(manager)) return;
        if (AIMilitaryManager_Military_GetCivilianDangerValueForPosition(destination, player) > 20)
        {
            std::byte target[12]{}; Write<std::uint32_t>(target, 0, 5); Write(target, 4, Read<std::uint32_t>(destination, 0)); Write<std::uint16_t>(target, 10, 0);
            AIMilitaryManager_MilitaryPoint_Add(military, 2, 0, target, 60, 4, 4); return;
        }
        const auto owner = Read<std::uint32_t>(human, 16);
        Human_DoAddNewUserCommand(human, 19, static_cast<std::uint8_t>(owner), 0, 0, MapMigPoint_GetLong(destination), 0);
    }
    // Original function: FUN_004298ae.
    void AIGuideManager_l_CheckForNewGuideBuildPos(void* manager, std::uint32_t player)
    {
        if ((sGuideCandidateInitFlags & 1) == 0) { sGuideCandidateInitFlags |= 1; AIGuideCandidateList_Construct(&sGuideCandidates); NXBasics::Runtime_AtExit(AIGuideCandidateList_OnExit); }
        sGuideCandidates.count = 0; Write<std::uint8_t>(manager, 20, 0);
        for (std::int32_t y = 0; y < SSectorGridCursor::sGridHeight; ++y)
            for (std::int32_t x = 0; x < SSectorGridCursor::sGridWidth; ++x)
            {
                const auto* entry = static_cast<const std::byte*>(Read<void*>(manager, 32)) + (y * SSectorGridCursor::sGridWidth + x) * 12;
                if (Read<std::uint8_t>(entry, 1) == 0) continue;
                const auto* guide = CGuideArray::mStaticVars + Read<std::uint32_t>(entry, 4) * CGuideArray::ElementSize;
                std::uint32_t i = 0;
                for (; i < sGuideCandidates.count; ++i) if (sGuideCandidates.links[i] == Read<std::uint32_t>(guide, 12)) break;
                if (i != sGuideCandidates.count || sGuideCandidates.count >= 10) continue;
                sGuideCandidates.links[i] = Read<std::uint32_t>(guide, 12); sGuideCandidates.points[i] = Read<std::uint32_t>(guide, 20); ++sGuideCandidates.count;
            }
        auto* pathManager = static_cast<CArrayHandler*>(gMapPathfinderManager);
        const auto pathId = MapPathfinderManager_AllocatePathfinder(pathManager);
        if (pathId == -1) return;
        for (std::uint32_t i = 0; i < sGuideCandidates.count; ++i)
            for (std::uint32_t j = i + 1; j < sGuideCandidates.count; ++j)
            {
                auto* first = &sGuideCandidates.points[i]; auto* second = &sGuideCandidates.points[j];
                if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(first), 6) != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(second), 6)) continue;
                auto distance = Distance(first, second);
                CGuideIterator guides;
                for (auto id = guides.GetFirstGuideId(player); id != -1; id = guides.GetNextGuideId())
                {
                    const auto* guide = CGuideArray::mStaticVars + static_cast<std::uint32_t>(id) * CGuideArray::ElementSize;
                    const auto link = Read<std::uint32_t>(guide, 12);
                    if (link == sGuideCandidates.links[i])
                    {
                        const auto candidate = Distance(guide + 20, second);
                        if (candidate <= distance) { *first = Read<std::uint32_t>(guide, 20); distance = candidate; }
                    }
                    else if (link == sGuideCandidates.links[j])
                    {
                        const auto candidate = Distance(guide + 20, first);
                        if (candidate <= distance) { *second = Read<std::uint32_t>(guide, 20); distance = candidate; }
                    }
                }
                auto* path = GetUnsafePathfinderPtr(pathId);
                MapPathfinderElement_StartPathfinder(path, first, second, 40, true, true, false, false);
                auto point = *first;
                for (auto direction = MapPathfinderElement_GetNextDirection(path); direction >= 0; direction = MapPathfinderElement_GetNextDirection(path))
                {
                    MapMigPoint_MoveCursorUnwrapped(point, direction);
                    if (Tool_Guide_IsAllowedToBeBuild(player, &point, true)) { Write(manager, 16, point); MapPathfinderManager_FreePathfinder(pathManager, pathId); return; }
                }
                SGuideBuildSearch search; AIGuideBuildSearch_Construct(&search, first, second); search.player = player; search.danger = 10000;
                CMapPathfinderElement::SearchOverMap(first, 40, true, slAIHighLevelSearchGuideBuildPos, &search, true, player);
                if (search.danger != 10000) { Write(manager, 16, search.result); MapPathfinderManager_FreePathfinder(pathManager, pathId); return; }
                MapPathfinderElement_StartPathfinder(path, first, second, 40, true, true, false, false);
                point = *first;
                Logic_EntryPoint_Edit_Start();
                for (auto direction = MapPathfinderElement_GetNextDirection(path); direction >= 0; direction = MapPathfinderElement_GetNextDirection(path))
                {
                    MapMigPoint_MoveCursorUnwrapped(point, direction); Logic_EntryPoint_Edit_MapMicroPoint_Road_Set(&point, true);
                }
                Logic_EntryPoint_Edit_Stop();
            }
        MapPathfinderManager_FreePathfinder(pathManager, pathId); Write<std::uint8_t>(manager, 20, 1);
        for (std::int32_t row = 0; row < SSectorGridCursor::sGridHeight; ++row)
            for (std::int32_t column = 0; column < SSectorGridCursor::sGridWidth; ++column)
            {
                const auto* entry = static_cast<const std::byte*>(Read<void*>(manager, 32)) + (row * SSectorGridCursor::sGridWidth + column) * 12;
                if (Read<std::uint8_t>(entry, 0) == 0 || Read<std::uint8_t>(entry, 1) != 0) continue;
                const auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(entry, 8) * CHouseArray::ElementSize;
                const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(house + 28), 6);
                const auto x = column * 20, y = row * 20;
                std::uint32_t point = 0; MapMigPoint_Construct(&point, static_cast<std::int16_t>(x + 10), static_cast<std::int16_t>(y + 10), false);
                if (MapMigPoint_IsInsideMap(&point) && Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6) == continent && Tool_Guide_IsAllowedToBeBuild(player, &point, true)) { Write(manager, 16, point); return; }
                for (std::uint32_t radius = 1; radius < 20; ++radius)
                {
                    MapMigPoint_MoveCursorUnwrapped(point, 4);
                    for (std::uint32_t direction = 0; direction < 6; ++direction)
                        for (std::uint32_t step = 0; step < radius; ++step)
                        {
                            MapMigPoint_MoveCursorUnwrapped(point, direction);
                            if (!MapMigPoint_IsInsideMap(&point)) continue;
                            const auto px = Read<std::int16_t>(&point, 0), py = Read<std::int16_t>(&point, 2);
                            if (px < x || px > x + 20 || py < y || py > y + 20) continue;
                            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 6) == continent && Tool_Guide_IsAllowedToBeBuild(player, &point, true)) { Write(manager, 16, point); return; }
                        }
                }
            }
    }
}
