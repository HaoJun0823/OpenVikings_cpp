#pragma once

#include <cstdint>

namespace NC2Logic
{
    bool Human_IsAbleToListenNow(const void* human);
    bool Human_IsDoingGoodie(const void* human);
    bool Human_StartWaitingForOtherHuman(void* human);
    bool Human_StartListening(void* human, void* speaker);
    void Human_Target_SetTargetPositions(void* human, const void* realPosition, const void* workPosition);
    std::uint32_t Human_Target_GetDirectionTo(const void* human);
    void* Human_FindHumanToTalkInTalkRange_Any(const void* human);
    void* Human_FindHumanToTalkInSearchRange_Any(const void* human);
    bool Human_FindMapPointNearby(const void* human, void* point);
    bool Human_FindPlaceToSleep(const void* human, void* realPosition, void* workPosition);
    bool Human_l_StartAtomicWalk(void* human, const void* realPosition, const void* workPosition);
    bool Human_l_StartAtomicWalk(void* human, std::int32_t moveableId, std::uint32_t distance);
    bool Human_HouseMisc_Leave(void* human);
    void Human_l_StartTask_WaitForOtherHuman(void* human);
    void Human_l_StartTask_Goodie(void* human);
    void Human_l_StartTask_Goodie_Helper_Baby(void* human);
    void Human_l_StartTask_Goodie_Helper_Child(void* human);
    void Human_l_StartTask_Goodie_Helper_WalkAround(void* human);
    void Human_l_StartTask_Goodie_Helper_Sleep(void* human);
    void Human_l_StartTask_Goodie_Helper_TalkToAny(void* human);
    void Human_l_FinishTask_Goodie(void* human);
    void Human_l_FailTask_Goodie(void* human);
}
