#pragma once

#include <cstdint>

namespace NC2Logic
{
    bool Tool_House_IsAllowedToBeBuild(std::uint32_t player, std::int32_t engineType, std::uint32_t stage, std::uint32_t point, bool ignoreTerrain, bool checkBioPattern);
    bool Tool_Human_IsAnyOfPlayerAround(std::uint32_t player, const void* point, std::int32_t radius, bool includeVehicles);
}
