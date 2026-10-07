#pragma once

#include <cstdint>

namespace NC2Logic
{
    struct SStaticAtomicAnimationData;
    struct SStaticAtomicAnimationEventData;
    bool Human_AreAtomicEventsReconstructed(const SStaticAtomicAnimationData& animation);
    void Human_l_PerformTaskEvent(void* human, const SStaticAtomicAnimationEventData& event);
    void Human_l_PerformTaskEvent_SendSocial(void* human, std::int32_t amount);
    void Human_l_PerformTaskEvent_SendReligion(void* human, std::int32_t amount);
    void Human_l_PerformTaskEvent_SendHitpoints(void* human, std::int32_t amount);
    void Human_l_PerformTaskEvent_PlaySoundFX(void* human, std::uint32_t sound);
    bool Human_IsInHomeHouse(const void* human);
    bool Human_IsInWorkHouse(const void* human);
    bool Human_IsJobIgnoringHomeHouse(const void* human);
    void Human_l_ResetAtomic(void* human);
}
