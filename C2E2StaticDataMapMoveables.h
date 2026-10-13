#pragma once

#include "C2LifetimeAccess.h"
#include "C2RuntimeConstants.h"

namespace NC2E2
{
    struct SStaticMoveableAnimationListTag {};
    struct SStaticMoveableShadowListTag {};

    struct SMapMoveableFrameList
    {
        std::uint32_t count;
        std::uint32_t* frames;
    };

    struct SE2StaticDataMapMoveableAnimationPart
    {
        std::uint32_t type;
        std::uint32_t startPercent;
        std::uint32_t endPercent;
        union { std::int32_t walkDirection; std::int32_t atomicAction; std::int32_t overlayBobId; std::int32_t overlayLandscapeId; };
        union { std::int32_t walkGood; std::int32_t atomicSubId; std::int32_t overlayOffsetX; };
        union { std::int32_t walkEndOffsetX; std::int32_t atomicDirection; std::int32_t overlayOffsetY; };
        std::int32_t walkEndOffsetY;
    };

    struct SE2StaticDataMapMoveableAnimationData
    {
        std::uint32_t tribe;
        std::uint32_t job;
        std::uint32_t action;
        std::uint32_t inHouseSubId;
        std::int32_t animationMode;
        std::uint32_t bodySequenceOrPartCount;
        std::uint32_t headSequenceOrPartAddress;
        SMapMoveableFrameList frames;
        SMapMoveableFrameList directionFrames[8];
    };

    struct SE2StaticDataMapMoveableWalkData
    {
        std::uint32_t tribe;
        std::uint32_t job;
        std::uint32_t good;
        std::uint32_t walkSpeed;
        std::int32_t bodySequence;
        std::int32_t headSequence;
        SMapMoveableFrameList walkFrames[8];
        SMapMoveableFrameList turnFrames[8];
    };

    void* __fastcall E2StaticDataMapMoveablesManager_Construct(void* manager, void* unusedEdx);
    const SE2StaticDataMapMoveableAnimationData* StaticMapMoveables_GetAnimationDataPtr(void* manager, std::uint32_t tribe, std::uint32_t job, std::uint32_t action, std::uint32_t inHouseSubId);
    const SE2StaticDataMapMoveableWalkData* StaticMapMoveables_GetWalkDataPtr(void* manager, std::uint32_t tribe, std::uint32_t job, std::uint32_t good, std::uint32_t walkSpeed);

    static_assert(sizeof(SMapMoveableFrameList) == 8);
    static_assert(sizeof(SE2StaticDataMapMoveableAnimationPart) == 28);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, startPercent) == 4);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, endPercent) == 8);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, walkDirection) == 12);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, atomicAction) == 12);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, overlayBobId) == 12);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, overlayLandscapeId) == 12);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, walkGood) == 16);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, atomicSubId) == 16);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, overlayOffsetX) == 16);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, walkEndOffsetX) == 20);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, atomicDirection) == 20);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, overlayOffsetY) == 20);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationPart, walkEndOffsetY) == 24);
    static_assert(sizeof(SE2StaticDataMapMoveableAnimationData) == 100);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationData, animationMode) == 0x10);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationData, frames) == 0x1C);
    static_assert(offsetof(SE2StaticDataMapMoveableAnimationData, directionFrames) == 0x24);
    static_assert(sizeof(SE2StaticDataMapMoveableWalkData) == 152);
    static_assert(offsetof(SE2StaticDataMapMoveableWalkData, walkFrames) == 0x18);
    static_assert(offsetof(SE2StaticDataMapMoveableWalkData, turnFrames) == 0x58);
}
