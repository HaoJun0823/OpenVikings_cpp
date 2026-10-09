#pragma once

#include <cstdint>

namespace NC2Logic
{
    bool Tool_IsEnemyLandscapeOnPoint(std::int32_t player, const void* point);
    bool Tool_FillArrayFromPoint(std::int32_t player, const void* source, const void* point, void* list);
    bool Tool_GetNextNearestEnemyLandscapeInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool sameContinent, void* list);
    bool Tool_GetOneOfNearestEnemyLandscapes(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool sameContinent, std::uint32_t limit, std::uint32_t distanceThreshold, void* output);
    std::uint32_t Tool_ObjectId_GetNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output, bool humans, bool ignoreHidden);
    std::int32_t Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold, bool humans, bool ignoreHidden);
    std::uint32_t Tool_Animals_GetNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, void* output);
    std::int32_t Tool_Animals_GetOneOfNextEnemyInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool flag1, bool flag2, std::uint32_t limit, std::uint32_t distanceThreshold);
    std::uint32_t Tool_GetNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, void* output);
    std::int32_t Tool_GetOneOfNextEnemyHousesInRange(std::uint32_t player, const void* point, std::uint32_t minimumDistance, std::uint32_t maximumDistance, bool useSecondList, std::uint32_t limit, std::uint32_t distanceThreshold);
}
