#include "C2HouseBuildPosition.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameMapAccess.h"
#include "C2HumanIterator.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapArea.h"
#include "C2MapGeometry.h"
#include "C2StaticHouseTypeData.h"
#include "C2VehicleAccess.h"
#include "C2VehicleIterator.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

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
            const auto type = Read<std::uint32_t>(vehicle, 0x1E8);
            if ((type == 3 || type == 4) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) continue;
            if (Vehicle_Passengers_GetAttachedCommanderId(vehicle) != -1) return true;
        }
        return false;
    }

    // Original function: FUN_00440f70; player is deliberately unused and alignment mutates only the by-value point.
    bool Tool_House_IsAllowedToBeBuild(std::uint32_t, std::int32_t engineType, std::uint32_t stage, std::uint32_t point, bool ignoreTerrain, bool checkBioPattern)
    {
        if (engineType == -1 || !MapMigPoint_IsInsideMap(&point) || MapMigPoint_IsBorderPoint(&point)) return false;
        if (!ignoreTerrain && MapMicroPointData_Continent_GetContinentTyp(GameMap_GetConstMapMicroPointPtr(&point)) != 1) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 0) == 2) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 0) == 3) return false;
        MapMigPoint_MakeMacroGridAligned(&point);
        if (!MapMigPoint_IsOnMacroGrid(&point)) return false;
        const void* definition = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(engineType);
        const auto maximumStage = Read<std::uint32_t>(definition, 0x18);
        const auto* maximumArea = Read<const CMapArea*>(definition, 0x1Cu + maximumStage * 4u);
        if (!maximumArea->BuildBlock_Check(&point)) return false;
        if (!ignoreTerrain && !maximumArea->Check_AreAllPointsAroundOnBuildAllowedPattern(&point)) return false;
        if ((Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&point), 13) & 0x20u) != 0) return false;
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
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&workPoint), 0) == 2) return false;
        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&workPoint), 0) == 3) return false;
        return true;
    }
}
