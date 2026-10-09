#pragma once
#include <cstdint>

namespace NC2Logic
{
    std::uint32_t Human_GetLandscapeTypeAtActiveMapPosition(const void* human);
    std::uint32_t Human_GetLandscapeValencyAtActiveMapPosition(const void* human);
    std::uint32_t Human_GetLandscapeValencyAtTargetPosition(const void* human);
    void Human_l_StartTask_ExecuteJob_Collector(void* human);
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect(void* human);
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_NonBio(void* human, std::uint32_t harvest, std::uint32_t pickup, std::uint32_t store, std::uint32_t animation);
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_Bio(void* human, std::uint32_t harvest, std::uint32_t pickup, std::uint32_t store, std::uint32_t animation);
}
