#pragma once
#include <cstdint>
namespace NC2Logic
{
    void AIGuideManager_CalcVars_CreateGuideCalcArray(void* grid);
    bool AIGuideManager_IsGuideAlreadyBuild(const void* manager, std::uint32_t player, const void* point);
    void AIGuideManager_BuildGuideNearPosition(void* manager, std::uint32_t player, const void* point);
    void AIGuideManager_CheckAndBuildGuides(void* manager, std::uint32_t player, void* military);
    void AIGuideManager_l_CheckForNewGuideBuildPos(void* manager, std::uint32_t player);
}
