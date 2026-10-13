#include "XNumericConstants.h"
#include "C2MapPathfinderSectorSearch.h"
#include "C2MapPathfinderSearch.h"
#include "C2GameMapAccess.h"
#include "C2MapGeometry.h"
#include "C2LifetimeAccess.h"
#include <cstring>

namespace NC2Logic
{
    using NC2Lifetime::Read;
    using NC2Lifetime::Write;
    namespace
    {
        std::int32_t PathfinderDistance(const void* source, const void* destination)
        {
            return VE_HexagonDirection_GetDistance(Read<std::int16_t>(source, 0), Read<std::int16_t>(source, 2), Read<std::int16_t>(destination, 0), Read<std::int16_t>(destination, 2));
        }
    }

    // Original function: FUN_004703AE.
    void CMapPathfinderSpecialSectorSearchElement::Init(void* pathfinder)
    {
        std::memset(pathfinder, 0, 96);
        Write<std::uint32_t>(pathfinder, 4, 5);
    }

    // Original function: FUN_004703CA.
    void CMapPathfinderSpecialSectorSearchElement::StartPathfinder(void* pathfinder, const void* source, const void* destination, std::uint32_t radius, bool useResistance, const std::int16_t* rectangle)
    {
        Write<std::uint32_t>(pathfinder, 0, 1);
        Write(pathfinder, 0x20, Read<std::uint32_t>(source, 0));
        Write(pathfinder, 0x24, Read<std::uint32_t>(destination, 0));
        if (Read<std::uint32_t>(source, 0) == Read<std::uint32_t>(destination, 0)) Write<std::uint32_t>(pathfinder, 0, 3);
        Write(pathfinder, 4, radius);
        Write<std::uint8_t>(pathfinder, 0x0C, useResistance ? 1 : 0);
        std::memcpy(static_cast<std::byte*>(pathfinder) + 0x28, rectangle, 8);
    }

    // Original function: FUN_0047041B.
    std::int32_t CMapPathfinderSpecialSectorSearchElement::GetNextDirection(void* pathfinder)
    {
        const auto state = Read<std::uint32_t>(pathfinder, 0);
        if (state == 0) return -3;
        if (state == 3) { Write<std::uint32_t>(pathfinder, 0, 0); return -2; }
        auto* source = static_cast<std::byte*>(pathfinder) + 0x20;
        auto* destination = static_cast<std::byte*>(pathfinder) + 0x24;
        const auto stop = [&]() { Write<std::uint32_t>(pathfinder, 0, 0); return -1; };
        if (state == 1)
        {
            if (Read<std::uint32_t>(source, 0) == Read<std::uint32_t>(destination, 0)) { Write<std::uint32_t>(pathfinder, 0, 0); return -2; }
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 6) != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 6)) return stop();
            if (!l_FindWayAndBuildVectorList(pathfinder, source, destination, Read<std::uint32_t>(pathfinder, 4))) return stop();
            Write<std::uint32_t>(pathfinder, 0, 2);
        }
        auto run = Read<std::uint32_t>(pathfinder, 0x30);
        if ((Read<std::uint8_t>(pathfinder, 0x39 + run * 2u) == 0 || run == 20u) && Read<std::uint32_t>(pathfinder, 0) == 2)
        {
            if (!l_FindWayAndBuildVectorList(pathfinder, source, destination, Read<std::uint32_t>(pathfinder, 4))) return stop();
            run = Read<std::uint32_t>(pathfinder, 0x30);
        }
        const auto direction = Read<std::uint8_t>(pathfinder, 0x38 + run * 2u);
        const auto count = static_cast<std::uint8_t>(Read<std::uint8_t>(pathfinder, 0x39 + run * 2u) - 1u);
        Write(pathfinder, 0x39 + run * 2u, count);
        if (count == 0) Write(pathfinder, 0x30, run + 1u);
        MapMigPoint_MoveCursorUnwrapped(source, direction, 1);
        const auto blocked = Read<std::uint32_t>(pathfinder, 0x34);
        if (blocked != 0) Write(pathfinder, 0x34, blocked - 1u);
        else if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C) & NXBasics::BitMask::Bit7) != 0)
        {
            MapMigPoint_MoveCursorUnwrapped(source, VE_HexagonDirection_OppositeDirection(direction), 1);
            return stop();
        }
        if (Read<std::uint32_t>(source, 0) == Read<std::uint32_t>(destination, 0)) Write<std::uint32_t>(pathfinder, 0, 3);
        return direction;
    }

    // Original function: FUN_00470535.
    bool CMapPathfinderSpecialSectorSearchElement::l_FindWayAndBuildVectorList(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) return false;
        bool found;
        if (Read<std::uint32_t>(pathfinder, 8) != 0)
        {
            if ((flags & 7u) < Read<std::uint32_t>(pathfinder, 8)) return false;
            found = Read<std::uint8_t>(pathfinder, 0x0C) != 0 ? l_VehicleFindWayDirectedWithResitance(pathfinder, source, destination, radius) : l_VehicleFindWayDirected(pathfinder, source, destination, radius);
        }
        else found = Read<std::uint8_t>(pathfinder, 0x0C) != 0 ? l_FindWayDirectedWithResitance(pathfinder, source, destination, radius) : l_FindWayDirected(pathfinder, source, destination, radius);
        if (!found) return false;
        Write<std::uint32_t>(pathfinder, 0x30, 0);
        Write<std::uint32_t>(pathfinder, 0x34, 0);
        auto point = Read<std::uint32_t>(pathfinder, 0x20);
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
            if (static_cast<std::uint32_t>(PathfinderDistance(&point, static_cast<std::byte*>(pathfinder) + 0x24)) > 3u && (direction == 1u || direction == 2u || direction == 4u || direction == 5u) && Read<std::uint8_t>(pathfinder, 0x0D) != 0)
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
            const auto countOffset = 0x39u + (runs - 1u) * 2u;
            if (direction == previous && Read<std::uint8_t>(pathfinder, countOffset) != 255u) Write<std::uint8_t>(pathfinder, countOffset, static_cast<std::uint8_t>(Read<std::uint8_t>(pathfinder, countOffset) + 1u));
            else
            {
                if (runs == 20u) break;
                Write<std::uint8_t>(pathfinder, 0x38 + runs * 2u, static_cast<std::uint8_t>(direction));
                Write<std::uint8_t>(pathfinder, 0x39 + runs * 2u, 1);
                ++runs;
                previous = direction;
            }
            MapMigPoint_MoveCursorUnwrapped(point, direction);
            if (blockedArea && (GameMap_CalculationMap_GetValue(&point) & NXBasics::BitMask::Bit9) != 0) Write(pathfinder, 0x34, Read<std::uint32_t>(pathfinder, 0x34) + 1u);
            else blockedArea = false;
        }
        if (runs < 20u) Write<std::uint8_t>(pathfinder, 0x39 + runs * 2u, 0);
        Write<std::uint32_t>(pathfinder, 0, 2);
        return true;
    }

    // Original function: FUN_00470735; this blocked-area search is independent of vehicle clearance.
    void CMapPathfinderSpecialSectorSearchElement::l_UnblockArea(void* pathfinder, const void* point)
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
                    if ((flags & NXBasics::BitMask::Bit7) == 0) continue;
                    if (destination.count == 4000u) continue;
                    destination.points[destination.count++] = next;
                    GameMap_CalculationMap_SetValue(&next, 0x200);
                }
            }
            current = (current + 1u) & 1u;
        } while (sMapPathfinderStaticVariables[current].count != 0);
    }

    // Original function: FUN_0043EC04; the special sector workspace has its own direction fields.
    void CMapPathfinderSpecialSectorSearchElement::l_GetPreferedDirections(void* pathfinder, const void* source, const void* destination)
    {
        const auto dx = static_cast<std::int32_t>(Read<std::int16_t>(destination, 0)) - Read<std::int16_t>(source, 0);
        const auto dy = static_cast<std::int32_t>(Read<std::int16_t>(destination, 2)) - Read<std::int16_t>(source, 2);
        auto x = dx < 0 ? -dx : dx;
        const auto y = dy < 0 ? -dy : dy;
        if ((y & 1) != 0 && ((Read<std::int16_t>(destination, 2) & 1) != 0 ? dx < 0 : dx > 0)) --x;
        const auto halfY = static_cast<std::int16_t>(y / 2);
        if (x > halfY)
        {
            Write<std::uint32_t>(pathfinder, 0x10, dy < 0 ? (dx < 0 ? 4u : 5u) : (dx < 0 ? 2u : 1u));
            Write<std::uint32_t>(pathfinder, 0x14, dx < 0 ? 3u : 0u);
        }
        else
        {
            Write<std::uint32_t>(pathfinder, 0x10, dy < 0 ? 5u : 1u);
            Write<std::uint32_t>(pathfinder, 0x14, dy < 0 ? 4u : 2u);
        }
    }

    // Original function: FUN_00470860. Rectangle, queue costs and equal-cost direction masks follow Windows.
    bool CMapPathfinderSpecialSectorSearchElement::l_FindWayDirected(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        const auto* rectangle = reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(pathfinder) + 0x28);
        if (!MapMigPoint_IsInside(source, rectangle) || !MapMigPoint_IsInside(destination, rectangle)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 4u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) l_UnblockArea(pathfinder, source);
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
                l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInside(&next, rectangle)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = 0;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x10) && direction != Read<std::uint32_t>(pathfinder, 0x14)) cost += 4u;
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

    // Original function: FUN_00470B42. Rectangle, queue costs and equal-cost direction masks follow Windows.
    bool CMapPathfinderSpecialSectorSearchElement::l_VehicleFindWayDirected(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        const auto* rectangle = reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(pathfinder) + 0x28);
        if (!MapMigPoint_IsInside(source, rectangle) || !MapMigPoint_IsInside(destination, rectangle)) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C) & 7u) < Read<std::uint32_t>(pathfinder, 8)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 4u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) l_UnblockArea(pathfinder, source);
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
                l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInsideMap(&next)) continue;
                    if (!MapMigPoint_IsInside(&next, rectangle)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if ((nextFlags & NXBasics::BitMask::Bit7) != 0 && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    if ((nextFlags & 7u) < Read<std::uint32_t>(pathfinder, 8)) continue;
                    std::uint32_t cost = 0;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x10) && direction != Read<std::uint32_t>(pathfinder, 0x14)) cost += 4u;
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

    // Original function: FUN_00470E3D. Rectangle, queue costs and equal-cost direction masks follow Windows.
    bool CMapPathfinderSpecialSectorSearchElement::l_FindWayDirectedWithResitance(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        const auto* rectangle = reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(pathfinder) + 0x28);
        if (!MapMigPoint_IsInside(source, rectangle) || !MapMigPoint_IsInside(destination, rectangle)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 8u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) l_UnblockArea(pathfinder, source);
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
                l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    if ((Read<std::uint8_t>(microPoint, 7) & (1u << direction)) == 0) continue;
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInside(&next, rectangle)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if (((nextFlags & NXBasics::BitMask::Bit7) != 0) && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    std::uint32_t cost = (nextFlags >> 3) & 15u;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x10) && direction != Read<std::uint32_t>(pathfinder, 0x14)) cost += 4u;
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

    // Original function: FUN_0047111E. Rectangle, queue costs and equal-cost direction masks follow Windows.
    bool CMapPathfinderSpecialSectorSearchElement::l_VehicleFindWayDirectedWithResitance(void* pathfinder, const void* source, const void* destination, std::uint32_t radius)
    {
        if (static_cast<std::uint32_t>(PathfinderDistance(source, destination)) > radius || radius > 511u) return false;
        const auto* rectangle = reinterpret_cast<const std::int16_t*>(static_cast<const std::byte*>(pathfinder) + 0x28);
        if (!MapMigPoint_IsInside(source, rectangle) || !MapMigPoint_IsInside(destination, rectangle)) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(destination), 0x0C) & 7u) < Read<std::uint32_t>(pathfinder, 8)) return false;
        GameMap_CalculationMap_Clear(destination, static_cast<std::int32_t>(radius * 2u));
        std::uint32_t budget = radius * 8u;
        if (budget > 511u) return false;
        GameMap_CalculationMap_SetValue(source, 0);
        const auto flags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(source), 0x0C);
        if ((flags & NXBasics::BitMask::Bit7) != 0) l_UnblockArea(pathfinder, source);
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
                l_GetPreferedDirections(pathfinder, queue.points + index, source);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                {
                    auto next = queue.points[index];
                    MapMigPoint_MoveCursorUnwrapped(next, direction);
                    if (!MapMigPoint_IsInsideMap(&next)) continue;
                    if (!MapMigPoint_IsInside(&next, rectangle)) continue;
                    const auto nextFlags = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&next), 0x0C);
                    const auto value = static_cast<std::uint16_t>(GameMap_CalculationMap_GetValue(&next));
                    if ((nextFlags & NXBasics::BitMask::Bit7) != 0 && (value & NXBasics::BitMask::Bit9Unsigned) == 0) continue;
                    if ((nextFlags & 7u) < Read<std::uint32_t>(pathfinder, 8)) continue;
                    std::uint32_t cost = (nextFlags >> 3) & 15u;
                    if (direction != Read<std::uint32_t>(pathfinder, 0x10) && direction != Read<std::uint32_t>(pathfinder, 0x14)) cost += 4u;
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


    // Original function: FUN_0044F14C; outlined member construction, no search-state initialization.
    void CMapPathfinderSpecialSectorSearchElement::ConstructMembers(void* pathfinder)
    {
        MapMigPoint_ConstructZero(static_cast<std::byte*>(pathfinder) + 0x20);
        MapMigPoint_ConstructZero(static_cast<std::byte*>(pathfinder) + 0x24);
        MapMigRectangle_ConstructZero(reinterpret_cast<std::int16_t*>(static_cast<std::byte*>(pathfinder) + 0x28));
    }

}
