#pragma once

#include <cstdint>

namespace NC2Logic
{
    inline constexpr std::uint32_t cSpeedOptionArray[7] = {0, 1, 12, 24, 36, 48, 60};
    inline constexpr std::uint32_t cAutosaveOptionArray[4] = {0, 3600, 10800, 21600};
    void Autosave_Start(std::uint32_t intervalTicks);
    void l_Autosave_Do();
    double Logic_EntryPoint_GameTime_GetSpeed();
    std::uint32_t GameTime_Network_GetHighestStoredLatency();
    std::uint32_t GameTime_Network_LatencyToFrequency(std::uint32_t latency);
    void GameTime_Update();
    void GameTime_DoOneGameTick();
    void GameTime_ResetNetworkSynchronization();
    void GameTime_SetNetworkWaitFlag(bool waiting);
    void Logic_EntryPoint_GameTime_Update();
}
