#pragma once

#include <cstddef>
#include <cstdint>

namespace NC2Logic { class CMapArea; }

namespace NC2E2
{
    struct SE2StaticDataPatternData
    {
        char name[40];
        std::uint32_t groupCount;
        std::int32_t groupIds[3];
        std::int32_t logicType;
        std::int32_t textureId;
        std::int32_t coordinatesA[6];
        std::int32_t coordinatesB[6];
    };

    struct SStaticLandscapeArea
    {
        std::uint32_t valency;
        NC2Logic::CMapArea* area;
    };

    struct SStaticLandscapeAreaList
    {
        std::uint32_t count;
        SStaticLandscapeArea* entries;
    };

    struct SStaticLandscapeTransition
    {
        std::int32_t type;
        std::int32_t landscapeId;
    };

    struct SStaticLandscapeFrames
    {
        std::uint32_t valency;
        std::uint32_t count;
        std::int32_t* frames;
    };

    struct SE2StaticDataLandscape
    {
        char name[40];
        std::uint32_t groupCount;
        std::int32_t groupIds[3];
        std::int32_t logicType;
        std::int32_t maximumValency;
        std::uint8_t isWorkable;
        std::uint8_t isPileableOnMap;
        std::byte padding42[2];
        // Walk blocking, build blocking, work, additional attachment points.
        SStaticLandscapeAreaList areas[4];
        std::uint32_t transitionCount;
        SStaticLandscapeTransition* transitions;
        void* bobManagers[2];
        std::int32_t paletteId;
        // Set by default, cleared by gfxstatic/gfxdynamicbackground; name unresolved.
        std::uint8_t unresolved78;
        std::uint8_t isStatic;
        std::uint8_t dynamicBackground;
        std::uint8_t loopAnimation;
        std::uint8_t hasFireTransition;
        std::uint8_t userFxMatrix;
        std::uint8_t drawVoidEver;
        std::byte padding7F;
        std::uint32_t shadingFactor;
        std::uint32_t frameGroupCount;
        SStaticLandscapeFrames* frameGroups;
    };

    // Complete Windows static regions, including every cached special ID.
    struct SStaticPatternStorage
    {
        std::uint32_t count;
        void* entries;
        std::int32_t borderIds[2];
        std::int32_t roadOverlayId;
        std::int32_t houseOverlayId;
    };
    inline SStaticPatternStorage gStaticPatternStorage{}; // 00568A30..00568A47

    struct SStaticLandscapeStorage
    {
        std::int32_t count;
        void* entries;
        std::int32_t houseFireIds[5];
        std::int32_t smokeIds[5];
        std::int32_t incenseFireIds[5];
        std::int32_t wallIds[5];
        // Original order: gate 02 open/closed, gate 01 open/closed, gate 03 open/closed.
        std::int32_t gateIds[6];
    };
    inline SStaticLandscapeStorage gStaticLandscapeStorage{}; // 00568A78..00568AE7

    void* __fastcall E2StaticDataPatternManager_Construct(void* manager, void* unusedEdx);
    std::int32_t E2StaticDataPatternManager_GetPatternId(const char* name);
    void* __fastcall E2StaticDataLandscapesManager_Construct(void* manager, void* unusedEdx);
    std::int32_t E2StaticDataLandscapesManager_GetLandscapeId(const char* name);
    std::int32_t E2StaticDataPalettesManager_GetPalette256Id(const char* name);
    std::int32_t __fastcall E2StaticDataTexturesManager_GetTextureId(void* manager, void* unusedEdx, const char* fileName);

    static_assert(sizeof(SE2StaticDataPatternData) == 0x70);
    static_assert(offsetof(SE2StaticDataPatternData, groupCount) == 0x28);
    static_assert(offsetof(SE2StaticDataPatternData, textureId) == 0x3C);
    static_assert(offsetof(SE2StaticDataPatternData, coordinatesA) == 0x40);
    static_assert(offsetof(SE2StaticDataPatternData, coordinatesB) == 0x58);
    static_assert(sizeof(SStaticLandscapeArea) == 8);
    static_assert(sizeof(SStaticLandscapeAreaList) == 8);
    static_assert(sizeof(SStaticLandscapeTransition) == 8);
    static_assert(sizeof(SStaticLandscapeFrames) == 0x0C);
    static_assert(sizeof(SE2StaticDataLandscape) == 0x8C);
    static_assert(offsetof(SE2StaticDataLandscape, logicType) == 0x38);
    static_assert(offsetof(SE2StaticDataLandscape, areas) == 0x44);
    static_assert(offsetof(SE2StaticDataLandscape, transitions) == 0x68);
    static_assert(offsetof(SE2StaticDataLandscape, bobManagers) == 0x6C);
    static_assert(offsetof(SE2StaticDataLandscape, paletteId) == 0x74);
    static_assert(offsetof(SE2StaticDataLandscape, unresolved78) == 0x78);
    static_assert(offsetof(SE2StaticDataLandscape, shadingFactor) == 0x80);
    static_assert(offsetof(SE2StaticDataLandscape, frameGroups) == 0x88);
    static_assert(sizeof(SStaticPatternStorage) == 0x18);
    static_assert(offsetof(SStaticPatternStorage, borderIds) == 8);
    static_assert(sizeof(SStaticLandscapeStorage) == 0x70);
    static_assert(offsetof(SStaticLandscapeStorage, wallIds) == 0x44);
    static_assert(offsetof(SStaticLandscapeStorage, gateIds) == 0x58);
}
