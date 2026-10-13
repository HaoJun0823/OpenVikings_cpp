#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2MapPathfinderSearch.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2GameState.h"
#include "C2MapMoveableAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2GameControlStartup.h"
#include "C2MapWalkSectorManager.h"
#include "C2GuideRouting.h"

namespace NC2Logic
{
    // Original function: FUN_00449706. The callback precedes queue insertion and marking.
    bool CMapPathfinderElement::SearchOverMap(const void* point, std::uint32_t radius, bool skipBlocked, MapSearchCallback callback, void* userData, bool useWalkCosts, std::int32_t playerId)
    {
        using NC2Lifetime::Read;
        using NC2Lifetime::Write;
        if (radius > 511u) return false;
        const bool recordStatistics = playerId >= 0 && playerId < 20;
        if (recordStatistics)
        {
            const std::uint32_t offset = static_cast<std::uint32_t>(playerId) * NC2Layout::RecordSize::GameControlPlayer + 0x4A24u;
            Write(gGameControl, offset, Read<std::uint32_t>(gGameControl, offset) + 1u);
        }
        SMapPathfinderManagerSearchOverMapCallbackData data{0, userData};
        if (callback(point, &data)) return true;

        std::uint32_t remaining = useWalkCosts ? radius * 2u + 12u : radius;
        GameMap_CalculationMap_Clear(point, static_cast<std::int32_t>(remaining + 2u));
        const std::uint32_t queueCount = useWalkCosts ? 12u : 2u;
        for (std::uint32_t index = 0; index < queueCount; ++index) sMapPathfinderStaticVariables[index].count = 0;
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(point, 0);
        GameMap_CalculationMap_SetValue(point, static_cast<std::int16_t>(remaining));

        std::uint32_t current = 0;
        std::uint32_t queued = 1;
        do
        {
            ++data.distance;
            --remaining;
            auto& source = sMapPathfinderStaticVariables[current];
            if (!useWalkCosts) sMapPathfinderStaticVariables[(current - 1u) & 1u].count = 0;
            for (std::uint32_t index = 0; index < source.count; ++index)
            {
                if (useWalkCosts) --queued;
                const void* microPoint = GameMap_GetConstMapMicroPointPtr(source.points + index);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    std::uint32_t next = source.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (GameMap_CalculationMap_GetValue(&next) != 0) continue;
                    std::uint32_t cost = 1;
                    if (useWalkCosts || skipBlocked)
                    {
                        const void* nextMicroPoint = GameMap_GetConstMapMicroPointPtr(&next);
                        const std::uint8_t flags = Read<std::uint8_t>(nextMicroPoint, 0x0C);
                        if (skipBlocked && (flags & NXBasics::BitMask::Bit7) != 0) continue;
                        if (useWalkCosts)
                        {
                            cost = (flags >> 3) & 15u;
                            if (cost == 0) continue;
                        }
                    }
                    if (callback(&next, &data)) return true;
                    auto& destination = sMapPathfinderStaticVariables[useWalkCosts ? (current + cost) % 12u : (current - 1u) & 1u];
                    if (destination.count == 4000u) continue;
                    destination.points[destination.count] = next;
                    ++destination.count;
                    if (useWalkCosts) ++queued;
                    GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>(remaining));
                }
            }
            if (useWalkCosts)
            {
                source.count = 0;
                if (queued == 0) break;
                current = (current + 1u) % 12u;
                if (remaining <= 12u) break;
            }
            else
            {
                current = (current + 1u) & 1u;
                if (sMapPathfinderStaticVariables[current].count == 0 || remaining <= 1u) break;
            }
        } while (true);

        if (recordStatistics)
        {
            const std::uint32_t offset = static_cast<std::uint32_t>(playerId) * NC2Layout::RecordSize::GameControlPlayer + 0x4A2Cu;
            Write(gGameControl, offset, Read<std::uint32_t>(gGameControl, offset) + 1u);
        }
        return false;
    }
    using NC2Lifetime::Read;
    using NC2Lifetime::Write;

    namespace
    {
        std::int32_t PathfinderDistance(const void* source, const void* destination)
        {
            return VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(destination, 0), Read<std::int16_t>(destination, 2));
        }
    }

    // Original function: FUN_00447837.
    void MapPathfinderElement_StartPathfinder(void* pathfinder, const void* source, const void* destination, std::uint32_t radius, bool useResistance, bool useSectors, bool useGuides, bool deterministic)
    {
        Write<std::uint32_t>(pathfinder, 4, 1);
        Write<std::uint32_t>(pathfinder, 8, 1);
        Write(pathfinder, 0x40, Read<std::uint32_t>(source, 0));
        Write(pathfinder, 0x44, Read<std::uint32_t>(destination, 0));
        if (Read<std::uint32_t>(pathfinder, 0x40) == Read<std::uint32_t>(pathfinder, 0x44)) Write<std::uint32_t>(pathfinder, 4, 3);
        Write(pathfinder, 0x0C, radius);
        Write<std::uint8_t>(pathfinder, 0x14, useResistance);
        Write<std::uint8_t>(pathfinder, 0x15, useSectors);
        Write<std::uint8_t>(pathfinder, 0x17, useGuides);
        Write<std::uint8_t>(pathfinder, 0x18, deterministic);
        const auto player = Read<std::int32_t>(pathfinder, 0x2C);
        if (player >= 0 && player < 20)
        {
            const auto offset = static_cast<std::uint32_t>(player) * NC2Layout::RecordSize::GameControlPlayer + 0x4A28u;
            Write(gGameControl, offset, Read<std::uint32_t>(gGameControl, offset) + 1u);
        }
    }

    // Original function: FUN_004478C1. Windows tests map attachment before the revision lookup.
    void MapPathfinderElement_StartPathfinder(void* pathfinder, const void* source, std::int32_t moveableId, std::int32_t distance, std::uint32_t radius, bool useResistance, bool useSectors, bool useGuides, bool deterministic)
    {
        Write<std::uint32_t>(pathfinder, 4, 1);
        Write<std::uint32_t>(pathfinder, 8, 2);
        Write(pathfinder, 0x40, Read<std::uint32_t>(source, 0));
        Write(pathfinder, 0x48, moveableId);
        const void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(moveableId) * NC2Layout::RecordSize::MapMoveable;
        if (Read<std::uint8_t>(moveable, 0x10) == 0)
        {
            Write<std::uint32_t>(pathfinder, 4, 0);
            return;
        }
        Write(pathfinder, 0x4C, MapMoveable_GetUniqueId(moveable));
        Write(pathfinder, 0x50, distance);
        std::uint32_t point = 0;
        MapMoveable_Attachment_GetNextMapPosition(moveable, &point);
        if (PathfinderDistance(static_cast<std::byte*>(pathfinder) + 0x40, &point) <= distance) Write<std::uint32_t>(pathfinder, 4, 3);
        Write(pathfinder, 0x0C, radius);
        Write<std::uint8_t>(pathfinder, 0x14, useResistance);
        Write<std::uint8_t>(pathfinder, 0x15, useSectors);
        Write<std::uint8_t>(pathfinder, 0x17, useGuides);
        Write<std::uint8_t>(pathfinder, 0x18, deterministic);
    }

    // Original function: FUN_0044796E.
    void MapPathfinderElement_StopPathfinder(void* pathfinder)
    {
        Write<std::uint32_t>(pathfinder, 4, 0);
        Write<std::int32_t>(pathfinder, 0x1C, -1);
        Write<std::int32_t>(pathfinder, 0x24, -1);
    }

    // Original function: FUN_0044797B. Returns -4 deferred, -3 stopped, -2 arrived, -1 failed.
    std::int32_t MapPathfinderElement_GetNextDirection(void* pathfinder)
    {
        const auto stop = [pathfinder](std::int32_t result) { MapPathfinderElement_StopPathfinder(pathfinder); return result; };
        const auto currentPoint = static_cast<std::byte*>(pathfinder) + 0x40;
        const auto destination = static_cast<std::byte*>(pathfinder) + 0x44;
        const auto target = [pathfinder]() -> const void*
        {
            const void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(pathfinder, 0x48) * NC2Layout::RecordSize::MapMoveable;
            if (Read<std::uint8_t>(moveable, 0) == 0) return nullptr;
            if (MapMoveable_GetUniqueId(moveable) != Read<std::uint32_t>(pathfinder, 0x4C)) return nullptr;
            if (Read<std::uint8_t>(moveable, 0x10) == 0) return nullptr;
            return moveable;
        };
        if (Read<std::uint32_t>(pathfinder, 4) == 0) return -3;
        if (Read<std::uint32_t>(pathfinder, 4) == 3u)
        {
            if (Read<std::uint32_t>(pathfinder, 8) == 2u)
            {
                const void* moveable = target();
                if (moveable == nullptr) return stop(-1);
                std::uint32_t point = 0;
                MapMoveable_Attachment_GetNextMapPosition(moveable, &point);
                if (PathfinderDistance(currentPoint, &point) <= Read<std::int32_t>(pathfinder, 0x50)) return stop(-2);
                Write<std::uint32_t>(pathfinder, 4, 1);
            }
            else if (Read<std::uint32_t>(pathfinder, 8) == 1u) return stop(-2);
        }
        if (Read<std::uint32_t>(pathfinder, 4) == 1u)
        {
            if (Read<std::uint32_t>(pathfinder, 8) == 2u)
            {
                const void* moveable = target();
                if (moveable == nullptr) return stop(-1);
                MapMoveable_Attachment_GetNextMapPosition(moveable, destination);
            }
            if (static_cast<std::uint32_t>(PathfinderDistance(destination, currentPoint)) > 6u && !GameControl_System_PathfinderLimit_IsSearchAllowed(gGameControl, Read<std::int32_t>(pathfinder, 0x2C))) return -4;
        }
        else if (Read<std::uint32_t>(pathfinder, 8) == 2u)
        {
            const void* moveable = target();
            if (moveable == nullptr) return stop(-1);
            std::uint32_t point = 0;
            MapMoveable_Attachment_GetNextMapPosition(moveable, &point);
            const auto distance = PathfinderDistance(currentPoint, destination);
            const auto changed = PathfinderDistance(&point, destination);
            if (distance <= 3 || (distance <= 10 && changed > 4) || (distance <= 20 && changed > 8) || (distance <= 50 && changed > 12) || (distance > 50 && changed > 20))
            {
                Write<std::uint32_t>(pathfinder, 4, 1);
                Write(destination, 0, point);
            }
        }
        if (Read<std::uint32_t>(pathfinder, 4) == 1u)
        {
            if (Read<std::uint32_t>(currentPoint, 0) == Read<std::uint32_t>(destination, 0)) return stop(-2);
            const auto sourceContinent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(currentPoint), 6);
            const auto destinationContinent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 6);
            if (sourceContinent != destinationContinent) return stop(-1);
            if (!MapPathfinderElement_l_FindWayAndBuildVectorList(pathfinder, currentPoint, destination, Read<std::uint32_t>(pathfinder, 0x0C))) return stop(-1);
            Write<std::uint32_t>(pathfinder, 4, 2);
        }
        auto index = Read<std::uint32_t>(pathfinder, 0x54);
        // Index 20 means all twenty runs were consumed. Do not read beyond the original array.
        if ((index == 20u || Read<std::uint8_t>(pathfinder, 0x5D + index * 2u) == 0) && Read<std::uint32_t>(pathfinder, 4) == 2u)
        {
            if (!MapPathfinderElement_l_FindWayAndBuildVectorList(pathfinder, currentPoint, destination, Read<std::uint32_t>(pathfinder, 0x0C))) return stop(-1);
            index = Read<std::uint32_t>(pathfinder, 0x54);
        }
        const auto direction = Read<std::uint8_t>(pathfinder, 0x5C + index * 2u);
        const auto count = static_cast<std::uint8_t>(Read<std::uint8_t>(pathfinder, 0x5D + index * 2u) - 1u);
        Write(pathfinder, 0x5D + index * 2u, count);
        if (count == 0) Write(pathfinder, 0x54, index + 1u);
        auto point = Read<std::uint32_t>(currentPoint, 0);
        MapMigPoint_MoveCursorUnwrapped(point, direction);
        Write(currentPoint, 0, point);
        const auto blockedAreaSteps = Read<std::uint32_t>(pathfinder, 0x58);
        if (blockedAreaSteps != 0) Write(pathfinder, 0x58, blockedAreaSteps - 1u);
        else
        {
            const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(currentPoint), 0x0C);
            if ((flags & NXBasics::BitMask::Bit7) != 0 || (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10))
            {
                MapMigPoint_MoveCursorUnwrapped(point, VE_HexagonDirection_OppositeDirection(direction));
                Write(currentPoint, 0, point);
                return stop(-1);
            }
        }
        if (Read<std::uint32_t>(pathfinder, 8) == 2u)
        {
            const auto distance = PathfinderDistance(currentPoint, destination);
            if (target() == nullptr) return stop(-1);
            if (distance <= Read<std::int32_t>(pathfinder, 0x50)) Write<std::uint32_t>(pathfinder, 4, 3);
        }
        else if (Read<std::uint32_t>(pathfinder, 8) == 1u && point == Read<std::uint32_t>(destination, 0)) Write<std::uint32_t>(pathfinder, 4, 3);
        return direction;
    }

    // Original function: FUN_00447CF1. Local, sector and guide searches retain their distinct native order.
    bool MapPathfinderElement_l_FindWayAndBuildVectorList(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) return false;
        const bool vehicle = Read<std::uint32_t>(pathfinder, 0x10) != 0;
        if (vehicle && (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) return false;
        bool found;
        if (vehicle)
        {
            if (Read<std::uint8_t>(pathfinder, 0x14) != 0) found = MapPathfinderElement_l_VehicleFindWayDirectedWithResitance(pathfinder, source, destination, radius);
            else found = MapPathfinderElement_l_VehicleFindWayWithLine(pathfinder, source, destination, radius) || MapPathfinderElement_l_VehicleFindWayDirected(pathfinder, source, destination, radius);
        }
        else
        {
            if (Read<std::uint8_t>(pathfinder, 0x14) != 0) found = MapPathfinderElement_l_FindWayDirectedWithResitance(pathfinder, source, destination, radius);
            else found = MapPathfinderElement_l_FindWayWithLine(pathfinder, source, destination, radius) || MapPathfinderElement_l_FindWayDirected(pathfinder, source, destination, radius);
        }
        if (!found)
        {
            const bool resistance = Read<std::uint8_t>(pathfinder, 0x14) != 0;
            if (Read<std::uint8_t>(pathfinder, 0x15) != 0)
            {
                std::uint32_t firstPoint = 0, secondPoint = 0;
                const bool land = GameMap_Continent_GetContinentTyp(Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6)) == 1;
                if (!CMapWalkSectorManager::GetNextSectorBasePoint(source, destination, Read<std::uint32_t>(pathfinder, 0x10), land, &firstPoint, &secondPoint, radius, Read<std::int32_t>(pathfinder, 0x2C))) return false;
                if (vehicle && resistance && !MapMigPoint_IsInsideMap(&firstPoint)) return false;
                if (firstPoint == Read<std::uint32_t>(source, 0)) return false;
                if (vehicle)
                {
                    if (resistance) found = MapPathfinderElement_l_VehicleFindWayDirectedWithResitance(pathfinder, source, &secondPoint, radius) || MapPathfinderElement_l_VehicleFindWayDirectedWithResitance(pathfinder, source, &firstPoint, radius);
                    else found = MapPathfinderElement_l_VehicleFindWayDirected(pathfinder, source, &secondPoint, radius) || MapPathfinderElement_l_VehicleFindWayWithLine(pathfinder, source, &firstPoint, radius) || MapPathfinderElement_l_VehicleFindWayDirected(pathfinder, source, &firstPoint, radius);
                }
                else
                {
                    if (resistance) found = MapPathfinderElement_l_FindWayDirectedWithResitance(pathfinder, source, &secondPoint, radius) || MapPathfinderElement_l_FindWayDirectedWithResitance(pathfinder, source, &firstPoint, radius);
                    else found = MapPathfinderElement_l_FindWayDirected(pathfinder, source, &secondPoint, radius) || MapPathfinderElement_l_FindWayWithLine(pathfinder, source, &firstPoint, radius) || MapPathfinderElement_l_FindWayDirected(pathfinder, source, &firstPoint, radius);
                }
            }
            else if (!vehicle && Read<std::uint8_t>(pathfinder, 0x17) != 0)
            {
                const auto guide = [](std::int32_t id) { return CGuideArray::mStaticVars + static_cast<std::intptr_t>(id) * CGuideArray::ElementSize; };
                auto previousGuide = Read<std::int32_t>(pathfinder, 0x24);
                auto sourceGuide = Read<std::int32_t>(pathfinder, 0x1C);
                if (previousGuide == sourceGuide && (!resistance || previousGuide != -1))
                {
                    Write<std::int32_t>(pathfinder, 0x24, -1);
                    Write<std::int32_t>(pathfinder, 0x1C, -1);
                    sourceGuide = -1;
                }
                bool cached = sourceGuide != -1 && Read<std::uint8_t>(guide(sourceGuide), 0) != 0 && Read<std::uint32_t>(guide(sourceGuide), 0x10) == Read<std::uint32_t>(pathfinder, 0x20);
                const auto currentGuide = Read<std::int32_t>(pathfinder, 0x24);
                cached = cached && currentGuide != -1 && Read<std::uint8_t>(guide(currentGuide), 0) != 0 && Read<std::uint32_t>(guide(currentGuide), 0x10) == Read<std::uint32_t>(pathfinder, 0x28) && Read<std::uint32_t>(guide(currentGuide), 0x0C) == Read<std::uint32_t>(guide(sourceGuide), 0x0C);
                if (cached) Write(pathfinder, 0x24, GuideManager_GetNextGuideId(gGuideManager, currentGuide, sourceGuide));
                else
                {
                    const auto next = GuideManager_GetNextGuideId(gGuideManager, Read<std::int32_t>(pathfinder, 0x2C), radius, source, destination, sourceGuide, previousGuide);
                    Write(pathfinder, 0x1C, sourceGuide);
                    Write(pathfinder, 0x24, next);
                    if (sourceGuide != -1) Write(pathfinder, 0x20, Read<std::uint32_t>(guide(sourceGuide), 0x10));
                }
                auto nextGuide = Read<std::int32_t>(pathfinder, 0x24);
                if (nextGuide == -1) return false;
                std::uint32_t point = 0;
                Guide_GetMovePoint(guide(nextGuide), &point);
                found = resistance ? MapPathfinderElement_l_FindWayDirectedWithResitance(pathfinder, source, &point, radius) : MapPathfinderElement_l_FindWayDirected(pathfinder, source, &point, radius);
                if (!found)
                {
                    Write(pathfinder, 0x24, previousGuide);
                    nextGuide = previousGuide;
                    Guide_GetMovePoint(guide(nextGuide), &point);
                    found = resistance ? MapPathfinderElement_l_FindWayDirectedWithResitance(pathfinder, source, &point, radius) : MapPathfinderElement_l_FindWayDirected(pathfinder, source, &point, radius);
                }
                if (found) Write(pathfinder, 0x28, Read<std::uint32_t>(guide(nextGuide), 0x10));
            }
            if (!found) return false;
        }
        Write<std::uint32_t>(pathfinder, 0x54, 0);
        Write<std::uint32_t>(pathfinder, 0x58, 0);
        auto point = Read<std::uint32_t>(pathfinder, 0x40);
        std::uint32_t runs = 0;
        std::uint32_t previous = 1000;
        bool blockedArea = (GameMap_CalculationMap_GetValue(&point) & NXBasics::BitMask::Bit9) != 0;
        while (true)
        {
            const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&point));
            if ((value & NXBasics::BitMask::WordExceptBits0Through9) == 0) break;
            std::uint32_t direction = 1000;
            std::uint32_t first = 1000;
            for (std::uint32_t candidate = 0; candidate < 6; ++candidate)
            {
                if ((value & (0x400u << candidate)) == 0) continue;
                if (first == 1000u) first = candidate;
                if (candidate == previous) direction = previous;
            }
            if (direction == 1000u) direction = first;
            if (static_cast<std::uint32_t>(PathfinderDistance(&point, static_cast<std::byte*>(pathfinder) + 0x44)) > 3u && (direction == 1u || direction == 2u || direction == 4u || direction == 5u) && Read<std::uint8_t>(pathfinder, 0x16) != 0)
            {
                auto next = point;
                MapMigPoint_MoveCursorUnwrapped(next, direction);
                const auto nextValue = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                if ((nextValue & NXBasics::BitMask::WordExceptBits0Through9) != 0)
                {
                    if (direction == 1u && (nextValue & NXBasics::BitMask::Bit12) != 0) direction = 7;
                    else if (direction == 2u && (nextValue & NXBasics::BitMask::Bit11) != 0) direction = 7;
                    else if (direction == 4u && (nextValue & NXBasics::BitMask::Bit15) != 0) direction = 6;
                    else if (direction == 5u && (nextValue & NXBasics::BitMask::Bit14) != 0) direction = 6;
                }
            }
            const auto countOffset = 0x5Du + (runs - 1u) * 2u;
            if (direction == previous && Read<std::uint8_t>(pathfinder, countOffset) != 255u) Write<std::uint8_t>(pathfinder, countOffset, static_cast<std::uint8_t>(Read<std::uint8_t>(pathfinder, countOffset) + 1u));
            else
            {
                if (runs == 20u) break;
                Write<std::uint8_t>(pathfinder, 0x5C + runs * 2u, static_cast<std::uint8_t>(direction));
                Write<std::uint8_t>(pathfinder, 0x5D + runs * 2u, 1);
                ++runs;
                previous = direction;
            }
            MapMigPoint_MoveCursorUnwrapped(point, direction);
            if (blockedArea && (GameMap_CalculationMap_GetValue(&point) & NXBasics::BitMask::Bit9) != 0) Write(pathfinder, 0x58, Read<std::uint32_t>(pathfinder, 0x58) + 1u);
            else blockedArea = false;
        }
        if (runs < 20u) Write<std::uint8_t>(pathfinder, 0x5D + runs * 2u, 0);
        Write<std::uint32_t>(pathfinder, 4, 2);
        return true;
    }

    // Original function: FUN_00448649. Unlike the vehicle helper, this tests walk-direction bits.
    bool MapPathfinderElement_l_CheckOtherDirection(void* pathfinder, std::uint32_t direction, void* point, const void* microPoint, std::uint32_t remaining)
    {
        auto other = Read<std::uint32_t>(pathfinder, 0x30);
        if (other == direction)
        {
            if (Read<std::uint32_t>(pathfinder, 0x3C) == 0) return false;
            other = Read<std::uint32_t>(pathfinder, 0x34);
        }
        else if (Read<std::uint32_t>(pathfinder, 0x38) == 0) return false;
        if ((Read<std::uint8_t>(microPoint, 7) & (1u << other)) == 0) return false;
        auto packed = Read<std::uint32_t>(point, 0);
        MapMigPoint_MoveCursorUnwrapped(packed, other);
        Write(point, 0, packed);
        const auto nextMicroPoint = GameMap_GetConstMapMicroPointPtr(point);
        const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(point));
        if ((Read<std::uint8_t>(nextMicroPoint, 0x0C) & NXBasics::BitMask::Bit7) != 0 && (value & NXBasics::BitMask::Bit9Unsigned) == 0) return false;
        GameMap_CalculationMap_SetValue(point, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | (0x400u << ((other + 3u) % 6u)) | (remaining - 1u)));
        return true;
    }

    // Original function: FUN_0044889C. This helper also checks the vehicle clearance.
    bool MapPathfinderElement_l_VehicleCheckOtherDirection(void* pathfinder, std::uint32_t direction, void* point, std::uint32_t remaining)
    {
        auto other = Read<std::uint32_t>(pathfinder, 0x30);
        if (other == direction)
        {
            if (Read<std::uint32_t>(pathfinder, 0x3C) == 0) return false;
            other = Read<std::uint32_t>(pathfinder, 0x34);
        }
        else if (Read<std::uint32_t>(pathfinder, 0x38) == 0) return false;
        auto packed = Read<std::uint32_t>(point, 0);
        MapMigPoint_MoveCursorUnwrapped(packed, other);
        Write(point, 0, packed);
        const auto nextMicroPoint = GameMap_GetConstMapMicroPointPtr(point);
        const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(point));
        const auto flags = Read<std::uint8_t>(nextMicroPoint, 0x0C);
        if (((flags & NXBasics::BitMask::Bit7) != 0 || (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) return false;
        GameMap_CalculationMap_SetValue(point, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | (0x400u << ((other + 3u) % 6u)) | (remaining - 1u)));
        return true;
    }

    // Original function: FUN_00448932. The two original queues mark the connected blocked area.
    void MapPathfinderElement_l_MakeMoveableArea(void* pathfinder, const void* point)
    {
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[1].count = 0;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(point, 0);
        GameMap_CalculationMap_SetValue(point, 0x200);
        std::uint32_t current = 0;
        do
        {
            auto& source = sMapPathfinderStaticVariables[current];
            auto& destination = sMapPathfinderStaticVariables[(current - 1u) & 1u];
            destination.count = 0;
            for (std::uint32_t index = 0; index < source.count; ++index)
            {
                const void* microPoint = GameMap_GetConstMapMicroPointPtr(source.points + index);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    auto next = source.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if ((GameMap_CalculationMap_GetValue(&next) & NXBasics::BitMask::Bit9) != 0) continue;
                    const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    if ((flags & NXBasics::BitMask::Bit7) == 0 && (flags & 7u) >= Read<std::uint32_t>(pathfinder, 0x10)) continue;
                    if (destination.count == 4000u) continue;
                    destination.points[destination.count++] = next;
                    GameMap_CalculationMap_SetValue(&next, 0x200);
                }
            }
            current = (current + 1u) & 1u;
        } while (sMapPathfinderStaticVariables[current].count != 0);
    }

    // Original function: FUN_00448A6B. Windows sign-extends the half-distance word.
    void MapPathfinderElement_l_GetPreferedDirections(void* pathfinder, const void* source, const void* destination)
    {
        const auto dx = static_cast<std::int32_t>(Read<std::int16_t>(destination, 0)) - Read<std::int16_t>(source, 0);
        const auto dy = static_cast<std::int32_t>(Read<std::int16_t>(destination, 2)) - Read<std::int16_t>(source, 2);
        auto x = dx < 0 ? -dx : dx;
        const auto y = dy < 0 ? -dy : dy;
        if ((y & 1) != 0 && ((Read<std::int16_t>(destination, 2) & 1) != 0 ? dx < 0 : dx > 0)) --x;
        const auto halfY = static_cast<std::int16_t>(y / 2);
        if (x > halfY)
        {
            Write<std::uint32_t>(pathfinder, 0x30, dy < 0 ? (dx < 0 ? 4u : 5u) : (dx < 0 ? 2u : 1u));
            Write<std::uint32_t>(pathfinder, 0x34, dx < 0 ? 3u : 0u);
        }
        else
        {
            Write<std::uint32_t>(pathfinder, 0x30, dy < 0 ? 5u : 1u);
            Write<std::uint32_t>(pathfinder, 0x34, dy < 0 ? 4u : 2u);
        }
    }

    // Original function: FUN_00448B1E. This is a distinct geometry routine, including weights.
    void MapPathfinderElement_l_GetPreferedDirectionsAndDistance(void* pathfinder, const void* source, const void* destination)
    {
        const auto dx = static_cast<std::int32_t>(Read<std::int16_t>(destination, 0)) - Read<std::int16_t>(source, 0);
        const auto dy = static_cast<std::int32_t>(Read<std::int16_t>(destination, 2)) - Read<std::int16_t>(source, 2);
        auto x = dx < 0 ? -dx : dx;
        const auto y = dy < 0 ? -dy : dy;
        if ((y & 1) != 0 && ((Read<std::int16_t>(destination, 2) & 1) != 0 ? dx < 0 : dx > 0)) --x;
        const auto halfY = static_cast<std::int16_t>(y / 2);
        std::int32_t firstWeight, secondWeight;
        if (x > halfY)
        {
            Write<std::uint32_t>(pathfinder, 0x30, dy < 0 ? (dx < 0 ? 4u : 5u) : (dx < 0 ? 2u : 1u));
            Write<std::uint32_t>(pathfinder, 0x34, dx < 0 ? 3u : 0u);
            firstWeight = y;
            secondWeight = x - halfY;
        }
        else
        {
            Write<std::uint32_t>(pathfinder, 0x30, dy < 0 ? 5u : 1u);
            Write<std::uint32_t>(pathfinder, 0x34, dy < 0 ? 4u : 2u);
            if (halfY != 0)
            {
                if (dx >= 0) { secondWeight = halfY - x; firstWeight = y - secondWeight; }
                else { firstWeight = halfY - x; secondWeight = y - firstWeight; }
            }
            else if (y != 0 && dx > 0) { firstWeight = y; secondWeight = 0; }
            else if (y != 0 && dx < 0) { firstWeight = 0; secondWeight = y; }
            else if (y != 0 && x == 0 && (Read<std::int16_t>(destination, 2) & 1) != 0) { firstWeight = y; secondWeight = 0; }
            else { firstWeight = 0; secondWeight = y; }
        }
        Write(pathfinder, 0x38, static_cast<std::uint32_t>(firstWeight));
        Write(pathfinder, 0x3C, static_cast<std::uint32_t>(secondWeight));
    }

    // Original function: FUN_00448460.
    bool MapPathfinderElement_l_FindWayWithLine(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius + 2u));
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(radius));
        auto point = Read<std::uint32_t>(destination, 0);
        do
        {
            MapPathfinderElement_l_GetPreferedDirectionsAndDistance(pathfinder, &point, source);
            const auto firstWeight = Read<std::uint32_t>(pathfinder, 0x38);
            const auto secondWeight = Read<std::uint32_t>(pathfinder, 0x3C);
            const bool first = Read<std::uint8_t>(pathfinder, 0x18) == 0 ? Game_Random_GetNumber() % (firstWeight + secondWeight) < firstWeight : firstWeight > secondWeight;
            const auto direction = Read<std::uint32_t>(pathfinder, first ? 0x30 : 0x34);
            auto alternative = point;
            const void* microPoint = GameMap_GetConstMapMicroPointPtr(&point);
            if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0)
            {
                if (!MapPathfinderElement_l_CheckOtherDirection(pathfinder, direction, &alternative, microPoint, radius)) return false;
                point = alternative;
            }
            else
            {
                MapMigPoint_MoveCursorUnwrapped(point, direction);
                const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 0x0C);
                const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&point));
                if (((nextFlags & NXBasics::BitMask::Bit7) != 0) && (value & NXBasics::BitMask::Bit9Unsigned) == 0)
                {
                    if (!MapPathfinderElement_l_CheckOtherDirection(pathfinder, direction, &alternative, microPoint, radius)) return false;
                    point = alternative;
                }
                else GameMap_CalculationMap_SetValue(&point, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | (0x400u << ((direction + 3u) % 6u)) | (radius - 1u)));
            }
            if (point == Read<std::uint32_t>(source, 0)) return true;
            --radius;
        } while (radius > 1u);
        return false;
    }

    // Original function: FUN_004486DC.
    bool MapPathfinderElement_l_VehicleFindWayWithLine(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C) & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius + 2u));
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0 || (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(radius));
        auto point = Read<std::uint32_t>(destination, 0);
        do
        {
            MapPathfinderElement_l_GetPreferedDirectionsAndDistance(pathfinder, &point, source);
            const auto firstWeight = Read<std::uint32_t>(pathfinder, 0x38);
            const auto secondWeight = Read<std::uint32_t>(pathfinder, 0x3C);
            const bool first = Read<std::uint8_t>(pathfinder, 0x18) == 0 ? Game_Random_GetNumber() % (firstWeight + secondWeight) < firstWeight : firstWeight > secondWeight;
            const auto direction = Read<std::uint32_t>(pathfinder, first ? 0x30 : 0x34);
            auto alternative = point;
            {
                MapMigPoint_MoveCursorUnwrapped(point, direction);
                const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 0x0C);
                const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&point));
                if (((nextFlags & NXBasics::BitMask::Bit7) != 0 || (nextFlags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) && (value & NXBasics::BitMask::Bit9Unsigned) == 0)
                {
                    if (!MapPathfinderElement_l_VehicleCheckOtherDirection(pathfinder, direction, &alternative, radius)) return false;
                    point = alternative;
                }
                else GameMap_CalculationMap_SetValue(&point, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | (0x400u << ((direction + 3u) % 6u)) | (radius - 1u)));
            }
            if (point == Read<std::uint32_t>(source, 0)) return true;
            --radius;
        } while (radius > 1u);
        return false;
    }

    // Original function: FUN_00448C55. Queue capacity and equal-cost direction masks are retained.
    bool MapPathfinderElement_l_FindWayDirected(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 4u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        for (std::uint32_t index = 0; index < 12; ++index) sMapPathfinderStaticVariables[index].count = 0;
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(destination, 0);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(budget));
        std::uint32_t current = 0;
        std::uint32_t queued = 1;
        do
        {
            const auto remaining = budget - 1u;
            auto& queue = sMapPathfinderStaticVariables[current];
            for (std::uint32_t index = 0; index < queue.count; ++index)
            {
                --queued;
                const void* microPoint = GameMap_GetConstMapMicroPointPtr(queue.points + index);
                MapPathfinderElement_l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = 0;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x30) && direction != Read<std::uint32_t>(pathfinder, 0x34)) cost += 4u;
                    const std::uint32_t oldCost = value & NXBasics::BitMask::Low9Bits;
                    if (oldCost >= budget - cost) continue;
                    const auto reverseMask = 0x400u << ((direction + 3u) % 6u);
                    if (remaining - cost == oldCost)
                    {
                        GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>(value | reverseMask));
                        continue;
                    }
                    auto& target = sMapPathfinderStaticVariables[(current + cost + 1u) % 12u];
                    if (target.count == 4000u) continue;
                    target.points[target.count++] = next;
                    ++queued;
                    GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | reverseMask | (remaining - cost)));
                    if (next == Read<std::uint32_t>(source, 0)) return true;
                }
            }
            queue.count = 0;
            if (queued == 0) break;
            current = (current + 1u) % 12u;
            budget = remaining;
        } while (budget > 13u);
        return (GameMap_CalculationMap_GetValue(source) & NXBasics::BitMask::Low9BitsSigned) != 0;
    }

    // Original function: FUN_00448EF0. Queue capacity and equal-cost direction masks are retained.
    bool MapPathfinderElement_l_VehicleFindWayDirected(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C) & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 4u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0 || (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        for (std::uint32_t index = 0; index < 12; ++index) sMapPathfinderStaticVariables[index].count = 0;
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(destination, 0);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(budget));
        std::uint32_t current = 0;
        std::uint32_t queued = 1;
        do
        {
            const auto remaining = budget - 1u;
            auto& queue = sMapPathfinderStaticVariables[current];
            for (std::uint32_t index = 0; index < queue.count; ++index)
            {
                --queued;
                MapPathfinderElement_l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInsideMap(&next)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0 || (nextFlags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = 0;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x30) && direction != Read<std::uint32_t>(pathfinder, 0x34)) cost += 4u;
                    const std::uint32_t oldCost = value & NXBasics::BitMask::Low9Bits;
                    if (oldCost >= budget - cost) continue;
                    const auto reverseMask = 0x400u << ((direction + 3u) % 6u);
                    if (remaining - cost == oldCost)
                    {
                        GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>(value | reverseMask));
                        continue;
                    }
                    auto& target = sMapPathfinderStaticVariables[(current + cost + 1u) % 12u];
                    if (target.count == 4000u) continue;
                    target.points[target.count++] = next;
                    ++queued;
                    GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | reverseMask | (remaining - cost)));
                    if (next == Read<std::uint32_t>(source, 0)) return true;
                }
            }
            queue.count = 0;
            if (queued == 0) break;
            current = (current + 1u) % 12u;
            budget = remaining;
        } while (budget > 13u);
        return (GameMap_CalculationMap_GetValue(source) & NXBasics::BitMask::Low9BitsSigned) != 0;
    }

    // Original function: FUN_004491AD. Queue capacity and equal-cost direction masks are retained.
    bool MapPathfinderElement_l_FindWayDirectedWithResitance(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 8u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        for (std::uint32_t index = 0; index < 16; ++index) sMapPathfinderStaticVariables[index].count = 0;
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(destination, 0);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(budget));
        std::uint32_t current = 0;
        std::uint32_t queued = 1;
        do
        {
            const auto remaining = budget - 1u;
            auto& queue = sMapPathfinderStaticVariables[current];
            for (std::uint32_t index = 0; index < queue.count; ++index)
            {
                --queued;
                const void* microPoint = GameMap_GetConstMapMicroPointPtr(queue.points + index);
                MapPathfinderElement_l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = (nextFlags >> 3) & 15u;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x30) && direction != Read<std::uint32_t>(pathfinder, 0x34)) cost += 4u;
                    const std::uint32_t oldCost = value & NXBasics::BitMask::Low9Bits;
                    if (oldCost >= budget - cost) continue;
                    const auto reverseMask = 0x400u << ((direction + 3u) % 6u);
                    if (remaining - cost == oldCost)
                    {
                        GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>(value | reverseMask));
                        continue;
                    }
                    auto& target = sMapPathfinderStaticVariables[(current + cost) & 15u];
                    if (target.count == 4000u) continue;
                    target.points[target.count++] = next;
                    ++queued;
                    GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | reverseMask | (remaining - cost)));
                    if (next == Read<std::uint32_t>(source, 0)) return true;
                }
            }
            queue.count = 0;
            if (queued == 0) break;
            current = (current + 1u) % 16u;
            budget = remaining;
        } while (budget > 17u);
        return (GameMap_CalculationMap_GetValue(source) & NXBasics::BitMask::Low9BitsSigned) != 0;
    }

    // Original function: FUN_00449447. Queue capacity and equal-cost direction masks are retained.
    bool MapPathfinderElement_l_VehicleFindWayDirectedWithResitance(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C) & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 8u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0 || (flags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) MapPathfinderElement_l_MakeMoveableArea(pathfinder, source);
        for (std::uint32_t index = 0; index < 16; ++index) sMapPathfinderStaticVariables[index].count = 0;
        sMapPathfinderStaticVariables[0].count = 1;
        sMapPathfinderStaticVariables[0].points[0] = Read<std::uint32_t>(destination, 0);
        GameMap_CalculationMap_SetValue(destination, static_cast<std::int16_t>(budget));
        std::uint32_t current = 0;
        std::uint32_t queued = 1;
        do
        {
            const auto remaining = budget - 1u;
            auto& queue = sMapPathfinderStaticVariables[current];
            for (std::uint32_t index = 0; index < queue.count; ++index)
            {
                --queued;
                MapPathfinderElement_l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInsideMap(&next)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0 || (nextFlags & 7u) < Read<std::uint32_t>(pathfinder, 0x10)) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = (nextFlags >> 3) & 15u;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x30) && direction != Read<std::uint32_t>(pathfinder, 0x34)) cost += 4u;
                    const std::uint32_t oldCost = value & NXBasics::BitMask::Low9Bits;
                    if (oldCost >= budget - cost) continue;
                    const auto reverseMask = 0x400u << ((direction + 3u) % 6u);
                    if (remaining - cost == oldCost)
                    {
                        GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>(value | reverseMask));
                        continue;
                    }
                    auto& target = sMapPathfinderStaticVariables[(current + cost) & 15u];
                    if (target.count == 4000u) continue;
                    target.points[target.count++] = next;
                    ++queued;
                    GameMap_CalculationMap_SetValue(&next, static_cast<std::int16_t>((value & NXBasics::BitMask::Bit9Unsigned) | reverseMask | (remaining - cost)));
                    if (next == Read<std::uint32_t>(source, 0)) return true;
                }
            }
            queue.count = 0;
            if (queued == 0) break;
            current = (current + 1u) % 16u;
            budget = remaining;
        } while (budget > 17u);
        return (GameMap_CalculationMap_GetValue(source) & NXBasics::BitMask::Low9BitsSigned) != 0;
    }

}
