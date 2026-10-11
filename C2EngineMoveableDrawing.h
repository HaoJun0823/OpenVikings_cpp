#pragma once

#include "C2E2StaticDataMapMoveables.h"

namespace NXBasics
{
    class CBitmap;
    class CPalette;
    struct SColorRGB;
}

namespace NC2E2
{
    // Shared Windows dynamic drawing prefix; humans extend it with two palettes.
    struct SE2DynamicDataMapMoveableData
    {
        std::uint32_t bodyBobId;
        std::uint32_t headBobId;
        std::uint32_t animationTribe;
        std::uint32_t animationJob;
        std::uint32_t animationAction;
        std::uint32_t animationRevision;
        std::uint32_t walkTribe;
        std::uint32_t walkJob;
        std::uint32_t walkGood;
        std::uint32_t walkSpeed;
        void* bodyBobManager;
        void* headBobManager;
        void* shadowBobManager;
        const SE2StaticDataMapMoveableAnimationData* animation;
        const SE2StaticDataMapMoveableWalkData* walk;
        std::byte subtypeOrFlags[8];
    };

    struct SMapMoveableAnimationOverlays
    {
        std::uint32_t behindCount;
        const SE2StaticDataMapMoveableAnimationPart* behind[5];
        std::uint32_t frontCount;
        const SE2StaticDataMapMoveableAnimationPart* front[5];
    };

    struct S2DEngineDisplaySearchObjectBobData
    {
        struct SBob { void* manager; std::uint32_t id; std::int32_t offsetX; std::int32_t offsetY; };
        std::uint32_t count;
        SBob bobs[20];
        bool AddBob(void* manager, std::uint32_t id, std::int32_t offsetX, std::int32_t offsetY);
    };

    void DrawAtom_MapMoveable_GetBobData(std::uint32_t moveableId, const void* display, S2DEngineDisplaySearchObjectBobData* output, std::int32_t playerId);
    void DrawAtom_Landscape_GetBobData(const void* graphic, std::uint32_t variant, std::uint32_t phase, S2DEngineDisplaySearchObjectBobData* output);
    std::uint32_t DrawAtom_Tool_GetSelectedBrightness();

    void E2DynamicDataHumanManager_HumanXtructed(std::uint32_t humanId, bool created, bool fromSavegame);
    void E2DynamicDataHumanManager_HumanJobChanged(std::uint32_t humanId, std::uint32_t oldJob, std::uint32_t newJob);
    void E2DynamicDataHumanManager_HumanArmorChanged(std::uint32_t humanId, std::uint32_t armor);
    void sl_DoUpdateMapMoveableDynamicBobData(const void* moveable, void* dynamicData, std::uint32_t tribe, std::uint32_t job, std::uint32_t good, bool atomicActive, std::uint32_t action, std::uint32_t done, std::uint32_t total, std::uint32_t inHouseRevision, bool overrideAction, const void* display, std::int32_t& offsetX, std::int32_t& offsetY, SMapMoveableAnimationOverlays* overlays, bool ignorePathfinder);
    const NXBasics::SColorRGB* DrawAtom_Tool_GetMarkingColorRef(std::uint32_t markingType, std::int32_t playerId);
    bool sl_DoDrawMapMoveable(const void* moveable, void* dynamicData, std::uint32_t tribe, std::uint32_t job, std::uint32_t good, bool atomicActive, std::uint32_t action, std::uint32_t done, std::uint32_t total, std::uint32_t inHouseRevision, NXBasics::CPalette* bodyPalette, NXBasics::CPalette* headPalette, NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, std::int32_t brightness, std::uint32_t markingType, std::int32_t playerId, bool overrideAction, bool centered, bool transparent, const NXBasics::SColorRGB* effectColor, std::int32_t bodyRange, std::int32_t headRange, const void* display, bool ignorePathfinder);

    static_assert(sizeof(SE2DynamicDataMapMoveableData) == 0x44);
    static_assert(offsetof(SE2DynamicDataMapMoveableData, bodyBobManager) == 0x28);
    static_assert(offsetof(SE2DynamicDataMapMoveableData, animation) == 0x34);
    static_assert(offsetof(SE2DynamicDataMapMoveableData, subtypeOrFlags) == 0x3C);
    static_assert(sizeof(SMapMoveableAnimationOverlays) == 0x30);
    static_assert(sizeof(S2DEngineDisplaySearchObjectBobData) == 0x144);
    static_assert(sizeof(S2DEngineDisplaySearchObjectBobData::SBob) == 0x10);
    static_assert(offsetof(SMapMoveableAnimationOverlays, frontCount) == 0x18);
}
