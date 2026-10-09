#pragma once

#include <cstdint>

namespace NC2Logic
{
    std::uint32_t Tool_ObjectId_GetNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output, bool humans, bool ignoreHidden);
    std::int32_t Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold, bool humans, bool ignoreHidden);
    std::uint32_t Tool_Animals_GetNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, void* output);
    std::int32_t Tool_Animals_GetOneOfNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, std::uint32_t distanceThreshold);
    std::uint32_t Tool_GetNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output);
    std::int32_t Tool_GetOneOfNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold);
}
