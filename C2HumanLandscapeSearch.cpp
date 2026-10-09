#include "C2HumanLandscapeSearch.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2HumanMapCollection.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2MapGeometry.h"
#include "C2MapMoveableAccess.h"
#include "C2MapResourceManagers.h"
#include "C2MapPathfinderSearch.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        std::uint32_t Radius(const void* human) { const auto job = Read<std::uint32_t>(human, 0x18); return job == 18 || job == 29 ? 10u : 20u; }
        bool TryCandidatePoints(std::uint32_t count, const std::uint32_t* points, void* real, void* work)
        {
            Write(real, 0, points[0]);
            if (GameMap_Misc_GetLandscapeWorkPosition(real, work)) return true;
            for (std::uint32_t retry = 0; retry < count * 2u; ++retry)
            {
                Write(real, 0, points[Game_Random_GetNumber() % count]);
                if (GameMap_Misc_GetLandscapeWorkPosition(real, work)) return true;
            }
            return false;
        }

        // Original function: FUN_0045e745; true ends SearchOverMap.
        bool __cdecl slHumanSearchLandscapesFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchLandscapesData*>(callback->userData);
            if (data.firstDistance >= 0 && callback->distance - static_cast<std::uint32_t>(data.firstDistance) > 10u) return true;
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if (Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 2) == 0) return false;
            for (std::uint32_t index = 0; index < data.typeCount; ++index)
            {
                auto& entry = data.entries[index];
                if (entry.landscape != Read<std::uint8_t>(micro, 2) || (entry.minimumValency > 1u && entry.minimumValency > Read<std::uint8_t>(micro, 3))) continue;
                std::uint32_t work = 0;
                if (!GameMap_Misc_GetLandscapeWorkPosition(point, &work) || entry.count >= 10u) return false;
                entry.points[entry.count++] = Read<std::uint32_t>(point, 0);
                ++data.candidateCount;
                if (entry.enabled != 0)
                {
                    ++data.selectedCount;
                    if (data.firstDistance < 0) data.firstDistance = static_cast<std::int32_t>(callback->distance);
                    return data.selectedCount >= 10u;
                }
            }
            return false;
        }

        struct SHumanSearchBioLandscapeData
        {
            // Original constructor: FUN_00460e90; only point constructors run here.
            SHumanSearchBioLandscapeData() { for (auto& point : points) point = 0; }
            std::uint32_t landscape;
            std::uint32_t unknown04;
            std::uint32_t count;
            std::uint32_t points[10];
        };
        static_assert(sizeof(SHumanSearchBioLandscapeData) == 0x34);

        // Original function: FUN_0045e586.
        bool __cdecl slHumanSearchLandscapeFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchBioLandscapeData*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto landscape = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0u;
            if (landscape != data.landscape) return false;
            data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 10u;
        }

        struct SHumanSearchGrowPatternData
        {
            // Original constructor: FUN_0045ee05; only point fields are initialized.
            SHumanSearchGrowPatternData() { for (auto& point : points) point = 0; }
            std::uint32_t count;
            std::uint32_t points[5];
        };
        static_assert(sizeof(SHumanSearchGrowPatternData) == 0x18);

        // Original function: FUN_0045edd4.
        bool __cdecl slHumanSearchGrowPatternFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            if (!Human_CanPlantBioAtMapPosition(point)) return false;
            auto& data = *static_cast<SHumanSearchGrowPatternData*>(callback->userData);
            data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 5u;
        }

        struct SHumanSearchGrowLandscapeData
        {
            // Original constructor: FUN_0045efa7; only point fields are initialized.
            SHumanSearchGrowLandscapeData() { for (auto& point : mature) point = 0; for (auto& point : immature) point = 0; }
            std::uint32_t landscape, limit, count, matureCount, immatureCount, minimumValency;
            std::uint32_t mature[25], immature[25];
        };
        static_assert(sizeof(SHumanSearchGrowLandscapeData) == 0xE0);
        static_assert(offsetof(SHumanSearchGrowLandscapeData, immature) == 0x7C);

        struct SHumanSearchShorePointData
        {
            // Original constructor: FUN_0045f150; only point fields are initialized.
            SHumanSearchShorePointData() { for (auto& point : work) point = 0; for (auto& point : real) point = 0; }
            std::uint32_t count, work[8], real[8];
        };
        static_assert(sizeof(SHumanSearchShorePointData) == 0x44);

        // Original function: FUN_0045f053.
        bool __cdecl slHumanSearchShorePointFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 12) & 0x80u) != 0) return false;
            auto id = static_cast<std::int32_t>(Read<std::int16_t>(micro, 10));
            while (id != -1)
            {
                const auto* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                const auto type = Read<std::uint32_t>(MapMoveable_Pathfinder_GetPathfinderPtr(moveable), 4);
                if (type == 0 || type == 3) return false;
                id = Read<std::int32_t>(moveable, 0x24);
            }
            const auto ways = Read<std::uint8_t>(micro, 7);
            if ((ways & 0x3Fu) == 0x3Fu) return false;
            auto& data = *static_cast<SHumanSearchShorePointData*>(callback->userData);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
            {
                if ((ways & (1u << direction)) != 0) continue;
                auto adjacent = Read<std::uint32_t>(point, 0);
                MapMigPoint_MoveCursorUnwrapped(&adjacent, direction, 1);
                if (MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(&adjacent)) != 2u || !static_cast<CMapFishManager*>(gMapFishManager)->IsFishNearMapPosition(&adjacent, 20)) continue;
                data.work[data.count] = Read<std::uint32_t>(point, 0);
                data.real[data.count++] = adjacent;
                if (data.count == 8u) return true;
            }
            return false;
        }

        // Original function: FUN_0045ef2f.
        bool __cdecl slHumanSearchGrowLandscapeFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchGrowLandscapeData*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto kind = Read<std::uint8_t>(micro, 0);
            if ((kind == 1 ? Read<std::uint8_t>(micro, 2) : 0u) != data.landscape) return false;
            const auto valency = kind == 1 ? Read<std::uint8_t>(micro, 3) : 0u;
            if (kind == 1 && valency >= 5) data.mature[data.matureCount++] = Read<std::uint32_t>(point, 0);
            else if (valency <= data.minimumValency)
            {
                if (valency < data.minimumValency) { data.immatureCount = 0; data.minimumValency = valency; }
                data.immature[data.immatureCount++] = Read<std::uint32_t>(point, 0);
            }
            return ++data.count == data.limit;
        }

        // Original function: FUN_0045ec4a.
        bool __cdecl slHumanSearchBioLandscapeFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchBioLandscapeData*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            const auto type = Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0u;
            if (type != data.landscape || Read<std::uint8_t>(micro, 0) != 1 || Read<std::uint8_t>(micro, 3) < 3) return false;
            data.points[data.count++] = Read<std::uint32_t>(point, 0);
            return data.count == 10u;
        }

        struct SHumanSearchPilePointData
        {
            // Original constructor: FUN_0045d9e3.
            SHumanSearchPilePointData() : point(0) {}
            std::int32_t engineId;
            std::uint32_t count;
            std::uint32_t point;
        };
        static_assert(sizeof(SHumanSearchPilePointData) == 12);

        // Original function: FUN_0045d98d; the original callback accepts one point.
        bool __cdecl FindPilePointCallback(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* callback)
        {
            auto& data = *static_cast<SHumanSearchPilePointData*>(callback->userData);
            const auto* micro = GameMap_GetConstMapMicroPointPtr(point);
            if ((Read<std::uint8_t>(micro, 0xC) & 0x80u) != 0) return false;
            const auto kind = Read<std::uint8_t>(micro, 0);
            if (kind == 2 || kind == 3 || (kind == 1 && Read<std::uint8_t>(micro, 2) != 0 && Read<std::uint8_t>(micro, 2) != 1)) return false;
            data.point = Read<std::uint32_t>(point, 0);
            return ++data.count == 1u;
        }
    }

    // Original function: FUN_0045d92d; consumes a random number even for one candidate.
    bool Human_FindPilePointNearPosition(const void* human, std::int32_t engineId, const void* source, void* point)
    {
        SHumanSearchPilePointData data;
        data.engineId = engineId;
        data.count = 0;
        CMapPathfinderElement::SearchOverMap(source, 40, true, FindPilePointCallback, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto index = Game_Random_GetNumber() % data.count;
        Write(point, 0, Read<std::uint32_t>(&data, 8u + index * 4u));
        return true;
    }

    // Original function: FUN_0045e663.
    bool Human_FindLandscapesNearPos_Sorted(const void* human, SHumanSearchLandscapesData& data, void* real, void* work, const void* source)
    {
        if (data.scanned == 0)
        {
            CMapPathfinderElement::SearchOverMap(source, Radius(human), false, slHumanSearchLandscapesFunction, &data, true, Read<std::int32_t>(human, 0x10));
            data.scanned = 1;
            if (data.selectedCount == 0) return false;
        }
        if (data.candidateCount == 0 || data.typeCount == 0) return false;
        for (std::uint32_t index = 0; index < data.typeCount; ++index)
        {
            const auto& entry = data.entries[index];
            if (entry.enabled != 0 && entry.count != 0 && TryCandidatePoints(entry.count, entry.points, real, work)) return true;
        }
        return false;
    }

    // Original function: FUN_0045e9c8.
    bool Human_FindLandscapesNearPos_NoBlock(const void* human, SHumanSearchLandscapesData& data, void* real, void* work, const void* source)
    {
        if (data.scanned == 0)
        {
            CMapPathfinderElement::SearchOverMap(source, Radius(human), true, slHumanSearchLandscapesFunction, &data, true, Read<std::int32_t>(human, 0x10));
            data.scanned = 1;
            if (data.selectedCount == 0) return false;
        }
        if (data.candidateCount == 0 || data.typeCount == 0) return false;
        for (std::uint32_t index = 0; index < data.typeCount; ++index)
        {
            const auto& entry = data.entries[index];
            if (entry.enabled != 0 && entry.count != 0 && TryCandidatePoints(entry.count, entry.points, real, work)) return true;
        }
        return false;
    }

    // Original function: FUN_0045e83d; unlike Sorted, this does not set scanned.
    bool Human_FindLandscapesNearPos_Random(const void* human, SHumanSearchLandscapesData& data, void* real, void* work, const void* source)
    {
        if (data.scanned == 0)
        {
            CMapPathfinderElement::SearchOverMap(source, Radius(human), false, slHumanSearchLandscapesFunction, &data, true, Read<std::int32_t>(human, 0x10));
            if (data.selectedCount == 0) return false;
        }
        if (data.candidateCount == 0 || data.typeCount == 0) return false;
        std::uint32_t indices[64];
        std::uint32_t count = 0;
        for (std::uint32_t index = 0; index < data.typeCount; ++index) if (data.entries[index].enabled != 0 && data.entries[index].count != 0) indices[count++] = index;
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto other = Game_Random_GetNumber() % count;
            if (other != index) { const auto saved = indices[index]; indices[index] = indices[other]; indices[other] = saved; }
        }
        for (std::uint32_t index = 0; index < count; ++index)
        {
            SHumanSearchLandscapeEntry entry;
            entry.CopyCandidatesFrom(data.entries[indices[index]]);
            if (TryCandidatePoints(entry.count, entry.points, real, work)) return true;
        }
        return false;
    }

    // Original function: FUN_0045e98a; the leading enabled byte is not copied.
    void SHumanSearchLandscapeEntry::CopyCandidatesFrom(const SHumanSearchLandscapeEntry& source)
    {
        landscape = source.landscape;
        minimumValency = source.minimumValency;
        count = source.count;
        for (std::uint32_t index = 0; index < 10; ++index) points[index] = source.points[index];
    }

    // Original function: FUN_0045eba4; the bio search passes player -1.
    bool Human_FindBioLandscapeNearWorkCenter(const void* human, std::uint32_t landscape, void* real, void* work)
    {
        SHumanSearchBioLandscapeData data;
        data.landscape = landscape;
        data.count = 0;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, Radius(human), false, slHumanSearchBioLandscapeFunction, &data, true, -1);
        return data.count != 0 && TryCandidatePoints(data.count, data.points, real, work);
    }

    // Original function: FUN_0045e5c9; every attempt consumes a random number.
    bool Human_FindLandscapeNearWorkCenter(const void* human, std::uint32_t landscape, void* real, void* work)
    {
        SHumanSearchBioLandscapeData data;
        data.landscape = landscape;
        data.count = 0;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, Radius(human), false, slHumanSearchLandscapeFunction, &data, true, Read<std::int32_t>(human, 0x10));
        for (std::uint32_t retry = 0; retry < data.count * 2u; ++retry)
        {
            Write(real, 0, data.points[Game_Random_GetNumber() % data.count]);
            if (GameMap_Misc_GetLandscapeWorkPosition(real, work)) return true;
        }
        return false;
    }

    // Original function: FUN_0045ec9d; native work-position cursor moves accumulate.
    bool Human_FindGrowPatternNearWorkCenter(const void* human, void* real, void* work)
    {
        SHumanSearchGrowPatternData data;
        data.count = 0;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, 10, true, slHumanSearchGrowPatternFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        Write(real, 0, data.points[Game_Random_GetNumber() % data.count]);
        Write(work, 0, Read<std::uint32_t>(real, 0));
        auto direction = Game_Random_GetNumber() % 6u;
        auto cursor = Read<std::uint32_t>(work, 0);
        MapMigPoint_MoveCursorUnwrapped(cursor, direction);
        Write(work, 0, cursor);
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(real), 7) & (1u << direction)) != 0) return true;
        const auto start = direction + 2u >= 6u ? direction - 3u : direction + 2u;
        for (direction = start; direction < 6u; ++direction)
        {
            MapMigPoint_MoveCursorUnwrapped(cursor, direction);
            Write(work, 0, cursor);
            if (!MapMigPoint_IsInsideMap(work)) continue;
            const auto bit = (direction + 4u) % 6u;
            if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(real), 7) & (1u << bit)) != 0) return true;
        }
        for (direction = 0; direction < start; ++direction)
        {
            MapMigPoint_MoveCursorUnwrapped(cursor, direction);
            Write(work, 0, cursor);
            if (!MapMigPoint_IsInsideMap(work)) continue;
            const auto bit = (direction + 4u) % 6u;
            if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(real), 7) & (1u << bit)) != 0) return true;
        }
        Write(work, 0, Read<std::uint32_t>(real, 0));
        return true;
    }

    // Original function: FUN_0045ee1d.
    bool Human_FindGrowLandscapeNearWorkCenter(const void* human, std::uint32_t landscape, void* real, void* work)
    {
        SHumanSearchGrowLandscapeData data;
        data.landscape = landscape;
        data.limit = 25;
        data.count = data.matureCount = data.immatureCount = 0;
        data.minimumValency = 255;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, 10, true, slHumanSearchGrowLandscapeFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count < data.limit && Human_FindGrowPatternNearWorkCenter(human, real, work)) return true;
        if (data.matureCount != 0)
        {
            Write(real, 0, data.mature[Read<std::uint32_t>(human, 8) % data.matureCount]);
            if (GameMap_Misc_GetLandscapeWorkPosition(real, work)) return true;
            Write(real, 0, data.mature[Game_Random_GetNumber() % data.matureCount]);
            if (GameMap_Misc_GetLandscapeWorkPosition(real, work)) return true;
        }
        if (data.immatureCount == 0) return false;
        Write(real, 0, data.immature[Game_Random_GetNumber() % data.immatureCount]);
        return GameMap_Misc_GetLandscapeWorkPosition(real, work);
    }

    // Original function: FUN_0045efd4.
    bool Human_FindShorePointForFishing(const void* human, void* real, void* work)
    {
        SHumanSearchShorePointData data;
        data.count = 0;
        CMapPathfinderElement::SearchOverMap(static_cast<const std::byte*>(human) + 0xB30, Radius(human), true, slHumanSearchShorePointFunction, &data, true, Read<std::int32_t>(human, 0x10));
        if (data.count == 0) return false;
        const auto index = Game_Random_GetNumber() % data.count;
        Write(real, 0, data.real[index]);
        Write(work, 0, data.work[index]);
        return true;
    }

    // Original function: FUN_00460e03; other entry fields remain untouched until Add/SetCurrent.
    SHumanSearchLandscapesData::SHumanSearchLandscapesData() : firstDistance(-1), typeCount(0), scanned(0), candidateCount(0), selectedCount(0)
    {
        for (auto& entry : entries) for (auto& point : entry.points) point = 0;
    }

    // Original function: FUN_00460e3a.
    void SHumanSearchLandscapesData::Add(std::uint32_t landscape)
    {
        if (typeCount >= 50u) return;
        auto& entry = entries[typeCount];
        entry.landscape = landscape;
        entry.count = 0;
        entry.minimumValency = 1;
        ++typeCount;
    }

    // Original function: FUN_00460e6c; cached candidates are deliberately retained.
    void SHumanSearchLandscapesData::SetCurrent(std::uint32_t landscape)
    {
        for (std::uint32_t index = 0; index < typeCount; ++index) entries[index].enabled = entries[index].landscape == landscape;
    }

    // Original function: FUN_00476bb2.
    void SHumanSearchLandscapesData::AddBio(std::uint32_t landscape)
    {
        if (typeCount >= 50u) return;
        auto& entry = entries[typeCount];
        entry.landscape = landscape;
        entry.count = 0;
        entry.minimumValency = 3;
        ++typeCount;
    }
}
