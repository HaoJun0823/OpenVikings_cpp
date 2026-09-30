#pragma once

#include <cstdint>

namespace NC2E2
{
    // Original singletons: 00568a50 (CE2StaticDataPatternManager) and
    // 00568aec (CE2StaticDataLandscapesManager); manager bodies remain untranslated.
    extern void* gStaticDataPatternManager;
    extern void* gStaticDataLandscapesManager;

    std::int32_t StaticDataGroups_FindIndex(const void* groupData, const char* name);
    std::int32_t StaticDataGroups_AddName(void* groupData, const char* name);
}
