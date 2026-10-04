#include "C2EngineLandscapeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2E2StaticDataTerrain.h"
#include "C2EngineDisplayAccess.h"
#include "XCrtFileAccess.h"

namespace NC2LogicExternals
{
    // Original function: FUN_00480712.
    const void* External_Landscape_GetLogicDataPtr(std::int32_t engineId) { return static_cast<const std::byte*>(NC2E2::gStaticLandscapeEntries) + static_cast<std::uint32_t>(engineId) * 0x8Cu + 0x38u; }
    // Original function: FUN_00480914.
    std::int32_t External_Misc_GetEngineWallElementLandscapeId() { return NC2E2::gStaticLandscapeStorage.wallIds[0]; }
    // Original function: FUN_0048091a.
    std::int32_t External_Misc_GetEngineWallGateLandscapeId(std::uint32_t gate, bool open) { return NC2E2::gStaticLandscapeStorage.gateIds[(gate % 3u) * 2u + (open ? 1u : 0u)]; }

    // Original function: FUN_0048075e.
    const NC2Logic::CMapArea* External_Landscape_GetAreaPtr(std::int32_t engineId, std::uint32_t valency, std::uint32_t areaKind)
    {
        if (engineId == -1) return nullptr;
        const auto* landscape = static_cast<const NC2E2::SE2StaticDataLandscape*>(NC2E2::gStaticLandscapeEntries) + engineId;
        const auto& list = landscape->areas[areaKind];
        if (list.count == 0) return nullptr;
        for (auto index = NXBasics::Runtime_TimeSigned32(list.count - 1u); index >= 0; --index)
        {
            if (list.entries[index].valency <= valency) return list.entries[index].area;
        }
        return nullptr;
    }

    // Original function: FUN_004807a9.
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(const void* point)
    {
        const auto* micro = NC2E2::EngineMap_GetConstMapMicroPointPtr(point);
        if (micro == nullptr) return -1;
        const auto id = NC2Lifetime::Read<std::uint16_t>(micro, 0);
        return id == 0xFFFFu ? -1 : static_cast<std::int32_t>(id);
    }

    // Original function: FUN_004807c8.
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(std::int32_t landscapeType, bool requireMapLandscape)
    {
        const std::int32_t count = NC2E2::gStaticLandscapeCount;
        if (count <= 0) return -1;
        for (std::int32_t index = 0; index < count; ++index)
        {
            // Original 0x8C layout is described by SE2StaticDataLandscape.
            const auto* record = static_cast<const std::byte*>(NC2E2::gStaticLandscapeEntries) + static_cast<std::uint32_t>(index) * 0x8Cu;
            if (NC2Lifetime::Read<std::int32_t>(record, 0x38) != landscapeType) continue;
            if (!requireMapLandscape || NC2Lifetime::Read<std::uint8_t>(record, 0x41) != 0) return index;
        }
        return -1;
    }
}
