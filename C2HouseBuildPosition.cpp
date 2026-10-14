#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2HouseBuildPosition.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2HumanIterator.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2MapPathfinderSearch.h"
#include "C2StaticHouseTypeData.h"
#include "C2VehicleAccess.h"
#include "C2VehicleIterator.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original function: FUN_0044dcac; the center is a by-value Windows position.
    void Map_HouseBuildForbiddenFlagTool_SetArea(std::uint32_t point, std::int32_t radius, std::uint8_t forbidden)
    {
        const auto setPoint = [&]()
        {
            if (!MapMigPoint_IsInsideMap(&point)) return;
            void* micro = GameMap_GetMapMicroPointPtr(&point);
            Write<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::TerrainFlags, (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::TerrainFlags) & NXBasics::BitMask::ByteExceptBit5) | ((forbidden & 1u) << 5));
        };
        setPoint();
        for (std::int32_t ring = 1; ring <= radius; ring = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(ring) + 1u))
        {
            MapMigPoint_MoveCursorUnwrapped(point, 4);
            for (std::uint32_t direction = 0; direction < 6; ++direction)
                for (std::int32_t step = 0; step < ring; ++step)
                {
                    setPoint();
                    MapMigPoint_MoveCursorUnwrapped(point, direction);
                }
        }
    }

    // Original function: FUN_0044dd43; shared by mission and AI house placement.
    bool __cdecl Tool_HouseBuildPointSearchEvaluationFunction(const void* point, const SMapPathfinderManagerSearchOverMapCallbackData* data)
    {
        auto* search = static_cast<std::uint32_t*>(data->userData);
        if (!Tool_House_IsAllowedToBeBuild(search[0], NXBasics::Runtime_TimeSigned32(search[1]), search[2], Read<std::uint32_t>(point, 0), false, false)) return false;
        search[3] = Read<std::uint32_t>(point, 0);
        return true;
    }

    // Original function: FUN_00441151; stage work-area points retain WORD coordinate wrapping.
    bool Tool_House_IsWorkPointOnContinent(const void* point, std::int32_t engineType, std::uint32_t stage, std::uint32_t continent, void* workPoint)
    {
        if (!MapMigPoint_IsInsideMap(point) || MapMigPoint_IsBorderPoint(point) || !MapMigPoint_IsOnMacroGrid(point)) return false;
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType);
        const auto* area = Read<const CMapArea*>(definition, 0x34u + stage * 4u);
        for (std::uint32_t row = 0; row < area->rowCount; ++row)
        {
            std::uint32_t candidate;
            MapMigPoint_ConstructOffset(&candidate, point, area->rows[row].x, area->rows[row].y, false);
            for (std::uint32_t column = 0; column < area->rows[row].width; ++column)
            {
                if (MapMigPoint_IsInsideMap(&candidate))
                {
                    const void* micro = GameMap_GetConstMapMicroPointPtr(&candidate);
                    if (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::ContinentId) == continent && (Read<std::uint8_t>(micro,NC2Layout::MapMicroPointOffset::WalkingFlags) & NXBasics::BitMask::Bit7) == 0) { Write(workPoint, 0, candidate); return true; }
                }
                Write<std::uint16_t>(&candidate, NC2Layout::MapCoordinatePairOffset::X, static_cast<std::uint16_t>(Read<std::uint16_t>(&candidate, NC2Layout::MapCoordinatePairOffset::X) + 1u));
            }
        }
        return false;
    }

    // Original function: FUN_0043aa23; player humans in both sector lists precede commander vehicles.
    bool Tool_Human_IsAnyOfPlayerAround(std::uint32_t player, const void* point, std::int32_t radius, bool includeVehicles)
    {
        CHumanIterator humans;
        if (humans.GetFirstSectorHumanId(player, Read<std::uint32_t>(point, 0), radius, false) != -1) return true;
        if (humans.GetFirstSectorHumanId(player, Read<std::uint32_t>(point, 0), radius, true) != -1) return true;
        if (!includeVehicles) return false;
        CVehicleIterator vehicles;
        for (auto id = vehicles.GetFirstSectorVehicleId(player, Read<std::uint32_t>(point, 0), radius, false); id != -1; id = vehicles.GetNextSectorVehicleId())
        {
            const void* vehicle = CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * CVehicleArray::ElementSize;
            const auto type = Read<std::uint32_t>(vehicle, NC2Layout::VehicleOffset::LogicType);
            if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, NC2Layout::VehicleOffset::DockedFlag) == 0) continue;
            if (Vehicle_Passengers_GetAttachedCommanderId(vehicle) != -1) return true;
        }
        return false;
    }

    // Original function: FUN_00440f70; player is deliberately unused and alignment mutates only the by-value point.
    bool Tool_House_IsAllowedToBeBuild(std::uint32_t, std::int32_t engineType, std::uint32_t stage, std::uint32_t point, bool ignoreTerrain, bool checkBioPattern)
    {
        if (engineType == -1 || !MapMigPoint_IsInsideMap(&point) || MapMigPoint_IsBorderPoint(&point)) return false;
        if (!ignoreTerrain && MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(&point)) != 1) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point),NC2Layout::MapMicroPointOffset::ObjectType) == 2) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point),NC2Layout::MapMicroPointOffset::ObjectType) == 3) return false;
        MapMigPoint_MakeMacroGridAligned(&point);
        if (!MapMigPoint_IsOnMacroGrid(&point)) return false;
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType);
        const auto maximumStage = Read<std::uint32_t>(definition, 0x18);
        const auto* maximumArea = Read<const CMapArea*>(definition, 0x1Cu + maximumStage * 4u);
        if (!maximumArea->BuildBlock_Check(&point)) return false;
        if (!ignoreTerrain && !maximumArea->Check_AreAllPointsAroundOnBuildAllowedPattern(&point)) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point),NC2Layout::MapMicroPointOffset::TerrainFlags) & NXBasics::BitMask::Bit5) != 0) return false;
        const auto* baseArea = Read<const CMapArea*>(definition, 0x1C);
        if (!baseArea->WalkBlock_Check(&point)) return false;
        if (!ignoreTerrain && !baseArea->Check_AreAllPointsOnSameLandContinent(&point)) return false;
        if (!baseArea->Check_NoGuideOnArea(&point)) return false;
        if (!baseArea->Check_NoHouseBuildForbiddenFlag(&point)) return false;
        if (checkBioPattern && gStaticHouseTypeStorage.records[Read<std::uint32_t>(definition, stage * 4u)].buildOnBioPattern != 0 && !baseArea->Check_AreAllPointsAroundOnBioRegrowPattern(&point)) return false;
        std::uint32_t workPoint;
        MapMigPoint_ConstructOffset(&workPoint, &point, Read<std::int16_t>(definition, 0x188u + stage * 4u), Read<std::int16_t>(definition, 0x18Au + stage * 4u), false);
        if (!MapMigPoint_IsInsideMap(&workPoint)) return false;
        if (!ignoreTerrain && MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(&workPoint)) != 1) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&workPoint),NC2Layout::MapMicroPointOffset::ObjectType) == 2) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&workPoint),NC2Layout::MapMicroPointOffset::ObjectType) == 3) return false;
        return true;
    }
}
