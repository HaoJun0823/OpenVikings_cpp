#pragma once

#include <cstdint>

namespace NC2Logic { class CMapArea; }

namespace NC2LogicExternals
{
    std::uint32_t External_Landscape_GetMaximumValency(std::int32_t engineId);
    std::int32_t External_Landscape_GetTransitionTarget(std::int32_t engineId, std::uint32_t transition);
    void External_Landscape_Clear(const void* point);
    void External_Landscape_InternallySetData(const void* point, std::int32_t engineId);
    const void* External_Landscape_GetLogicDataPtr(std::int32_t engineId);
    std::int32_t External_Misc_GetEngineWallElementLandscapeId();
    std::int32_t External_Misc_GetEngineWallGateLandscapeId(std::uint32_t gate, bool open);
    const NC2Logic::CMapArea* External_Landscape_GetAreaPtr(std::int32_t engineId, std::uint32_t valency, std::uint32_t areaKind);
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(const void* packedMigPoint);
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(std::int32_t landscapeType, bool requireMapLandscape);
}
