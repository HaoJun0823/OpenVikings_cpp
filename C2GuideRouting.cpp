#include "XNumericConstants.h"
#include "C2GuideRouting.h"
#include "C2MapPathfinderSearch.h"
#include "C2MapGeometry.h"
#include "C2GameMapAccess.h"
#include "C2OverviewObjectIterators.h"
#include "C2LogicObjectArrays.h"
#include "C2LifetimeAccess.h"
#include <cstring>

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Guide(std::int32_t id) { return CGuideArray::mStaticVars + static_cast<std::intptr_t>(id) * CGuideArray::ElementSize; }
        std::int32_t Distance(const void* a, const void* b) { return VE_HexagonDirection_GetDistance(Read<std::int16_t>(a, 0), Read<std::int16_t>(a, 2), Read<std::int16_t>(b, 0), Read<std::int16_t>(b, 2)); }

        // Original function: FUN_0041B360. The search stops at its second owned, strictly nearby guide.
        bool __cdecl slGuideSearch(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) != 3) return false;
            auto* guide = Guide(Read<std::uint16_t>(micro, 2));
            auto& data = *static_cast<SGuidePointSearch*>(callback->userData);
            if (Distance(static_cast<std::byte*>(guide) + 0x14, &data.point) >= data.radius || Read<std::uint32_t>(guide, 4) != data.player) return false;
            data.guideIds[data.count++] = Read<std::int32_t>(guide, 8);
            return data.count == 2;
        }
    }

    // Original function: FUN_00447677.
    void Guide_GetMovePoint(const void* guide, void* point)
    {
        const auto position = Read<std::uint32_t>(guide, 0x14);
        Write(point, 0, position);
        const auto directions = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 7);
        for (std::uint32_t direction = 0; direction < 6; ++direction)
        {
            if ((directions & (1u << direction)) == 0) continue;
            Write(point, 0, position);
            MapMigPoint_MoveCursorUnwrapped(point, direction, 1);
            if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 0x0C) & NXBasics::BitMask::Bit7) == 0) break;
        }
    }

    // Original function: FUN_0041B30C.
    void GuideManager_GetNearestGuideIds(void* manager, const void* point, std::uint32_t radius, SGuidePointSearch& data)
    {
        CGuideIterator iterator;
        if (iterator.GetFirstGuideId(data.player, point, radius) == -1) return;
        CMapPathfinderElement::SearchOverMap(point, radius, true, slGuideSearch, &data, true, -1);
        GuideManager_l_CleanSearchArrayAndSort(manager, data, point);
    }

    // Original function: FUN_0041B3F3.
    void GuideManager_GetRawNearestGuideIds(void* manager, const void* point, std::uint32_t radius, SGuidePointSearch& data)
    {
        CGuideIterator iterator;
        if (iterator.GetFirstGuideId(data.player, point, radius) != -1) CMapPathfinderElement::SearchOverMap(point, radius, true, slGuideSearch, &data, true, -1);
    }

    // Original function: FUN_0041B515. Native ordering prefers the first matching source/target link system.
    std::int32_t GuideManager_GetNextGuideId(void* manager, std::int32_t player, std::uint32_t radius, const void* source, const void* destination, std::int32_t& sourceGuideId, std::int32_t& targetGuideId)
    {
        sourceGuideId = -1;
        SGuidePointSearch targetSearch;
        targetSearch.player = static_cast<std::uint32_t>(player);
        targetSearch.count = 0;
        targetSearch.point = Read<std::uint32_t>(destination, 0);
        targetSearch.radius = NXBasics::Runtime_TimeSigned32(radius);
        GuideManager_GetNearestGuideIds(manager, destination, radius, targetSearch);
        if (targetSearch.count == 0) return -1;
        SGuidePointSearch sourceSearch;
        sourceSearch.player = static_cast<std::uint32_t>(player);
        sourceSearch.count = 0;
        sourceSearch.point = Read<std::uint32_t>(source, 0);
        sourceSearch.radius = NXBasics::Runtime_TimeSigned32(radius);
        GuideManager_GetRawNearestGuideIds(manager, source, radius, sourceSearch);
        if (sourceSearch.count == 0) return -1;
        std::int32_t selectedTarget = -1, selectedSource = -1;
        for (std::int32_t target = 0; target < targetSearch.count && selectedTarget == -1; ++target)
        {
            for (std::int32_t candidate = 0; candidate < sourceSearch.count && selectedTarget == -1; ++candidate)
            {
                auto* guide = Guide(sourceSearch.guideIds[candidate]);
                std::uint32_t position = 0;
                Guide_GetMovePoint(guide, &position);
                if (Distance(&position, source) > NXBasics::Runtime_TimeSigned32(radius) || Read<std::uint32_t>(guide, 0x0C) != Read<std::uint32_t>(Guide(targetSearch.guideIds[target]), 0x0C)) continue;
                selectedTarget = targetSearch.guideIds[target];
                selectedSource = sourceSearch.guideIds[candidate];
            }
        }
        if (selectedTarget == -1) return -1;
        std::uint32_t position = 0;
        Guide_GetMovePoint(Guide(selectedTarget), &position);
        if (selectedTarget == selectedSource)
        {
            if (position == Read<std::uint32_t>(source, 0)) return -1;
            sourceGuideId = selectedSource;
            return selectedTarget;
        }
        const auto next = GuideManager_l_GetNextGuideIdNearSourceGuide(manager, selectedTarget, selectedSource);
        targetGuideId = selectedTarget;
        sourceGuideId = selectedSource;
        for (std::int32_t index = 0; index < sourceSearch.count; ++index) if (sourceSearch.guideIds[index] == next) return next;
        return position == Read<std::uint32_t>(source, 0) ? next : selectedTarget;
    }

    // Original function: FUN_0041B6C7.
    std::int32_t GuideManager_GetNextGuideId(void* manager, std::int32_t guideId, std::int32_t otherGuideId)
    {
        return guideId == otherGuideId ? -1 : GuideManager_l_GetNextGuideIdNearSourceGuide(manager, guideId, otherGuideId);
    }

    // Original function: FUN_0041B6E3. Collapse same-system entries by distance, then compact at most two slots.
    void GuideManager_l_CleanSearchArrayAndSort(void* manager, SGuidePointSearch& data, const void* point)
    {
        for (std::int32_t index = data.count - 1; index >= 0; --index)
        {
            if (data.guideIds[index] == -1) continue;
            const auto* guide = Guide(data.guideIds[index]);
            for (std::int32_t earlier = 0; earlier < index; ++earlier)
            {
                if (data.guideIds[earlier] == -1 || Read<std::uint32_t>(Guide(data.guideIds[earlier]), 0x0C) != Read<std::uint32_t>(guide, 0x0C)) continue;
                const auto firstDistance = static_cast<std::uint32_t>(Distance(static_cast<const std::byte*>(Guide(data.guideIds[earlier])) + 0x14, point));
                const auto secondDistance = static_cast<std::uint32_t>(Distance(static_cast<const std::byte*>(guide) + 0x14, point));
                if (firstDistance < secondDistance) data.guideIds[index] = -1;
                else data.guideIds[earlier] = -1;
                --data.count;
                break;
            }
        }
        std::int32_t written = 0;
        for (std::int32_t scanned = 0; scanned < 2 && written < data.count; ++scanned) if (data.guideIds[scanned] != -1) data.guideIds[written++] = data.guideIds[scanned];
    }

    // Original function: FUN_0041B7E8. Search from the second guide and return the predecessor of the first.
    std::int32_t GuideManager_l_GetNextGuideIdNearSourceGuide(void* manager, std::int32_t guideId, std::int32_t otherGuideId)
    {
        auto* first = Guide(otherGuideId);
        if (Read<std::int32_t>(first, 8) == guideId) return -1;
        GuideManager_l_InitializeScan(manager);
        Write(manager, 8, first);
        Write<std::uint32_t>(manager, 4, Read<std::uint32_t>(manager, 4) + 1u);
        auto* visited = Read<std::uint8_t*>(manager, 0x1364);
        visited[Read<std::uint32_t>(first, 8)] = 1;
        std::uint32_t pending = 1, bucket = 0;
        for (;;)
        {
            const auto offset = 4u + bucket * 0x7Cu;
            for (std::int32_t slot = 0; slot < Read<std::int32_t>(manager, offset); ++slot)
            {
                auto* guide = Read<void*>(manager, offset + 4u + static_cast<std::uint32_t>(slot) * 4u);
                --pending;
                for (std::int32_t link = 0; link < Read<std::int32_t>(guide, 0x78); ++link)
                {
                    const auto linkOffset = 0x38u + static_cast<std::uint32_t>(link) * 8u;
                    const auto id = Read<std::int32_t>(guide, linkOffset + 4);
                    if (visited[id] == 1) continue;
                    auto* candidate = Guide(id);
                    if (Read<std::int32_t>(candidate, 8) == guideId) return Read<std::int32_t>(guide, 8);
                    const auto targetBucket = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(guide, linkOffset) + bucket) % 40;
                    const auto targetOffset = 4u + static_cast<std::uint32_t>(targetBucket) * 0x7Cu;
                    const auto count = Read<std::uint32_t>(manager, targetOffset);
                    Write(manager, targetOffset + 4u + count * 4u, candidate);
                    Write(manager, targetOffset, count + 1u);
                    ++pending;
                    visited[id] = 1;
                }
            }
            if (pending == 0) return -1;
            Write<std::uint32_t>(manager, offset, 0);
            if (++bucket == 40) bucket = 0;
        }
    }

    // Original function: FUN_0041B90D.
    void GuideManager_l_InitializeScan(void* manager)
    {
        std::memset(Read<void*>(manager, 4964), 0, static_cast<std::uint32_t>(CGuideArray::sTheObjectPtr->mCapacity));
        for (std::uint32_t bucket = 0; bucket < 40; ++bucket) Write<std::uint32_t>(manager, 4u + bucket * 0x7Cu, 0);
    }
}
