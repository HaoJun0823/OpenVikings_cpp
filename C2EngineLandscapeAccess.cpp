#include "C2EngineLandscapeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"

namespace NC2LogicExternals
{
    // Original function: FUN_004807c8.
    std::int32_t External_Landscape_GetEngineIdOfMapLandscape(std::int32_t landscapeType, bool requireMapLandscape)
    {
        const std::int32_t count = NC2E2::gStaticLandscapeCount;
        if (count <= 0) return -1;
        for (std::int32_t index = 0; index < count; ++index)
        {
            // Original engine record is 0x8C bytes; its remaining layout is unresolved.
            const auto* record = static_cast<const std::byte*>(NC2E2::gStaticLandscapeEntries) + static_cast<std::uint32_t>(index) * 0x8Cu;
            if (NC2Lifetime::Read<std::int32_t>(record, 0x38) != landscapeType) continue;
            if (!requireMapLandscape || NC2Lifetime::Read<std::uint8_t>(record, 0x41) != 0) return index;
        }
        return -1;
    }
}
