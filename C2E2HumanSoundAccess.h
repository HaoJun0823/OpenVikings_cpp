#pragma once

#include <cstdint>

namespace NC2E2
{
    void E2HumanSoundManager_ClearSeenHumanCounter(void* manager);
    void E2HumanSoundManager_IncreaseSeenCounter(void* manager, const void* human);
    void Tool_Sound_PlaySoundOrientedOnDisplay(const void* point, std::uint32_t soundId, const void* display, bool skipRunning, bool attenuate);
    void E2HumanSoundManager_PlayRespondingSound(const void* manager, std::uint32_t responseType, const void* human, const void* display);
    void E2HumanSoundManager_PlayRespondingSound(const void* manager, std::uint32_t responseType, std::int32_t humanId);
}
