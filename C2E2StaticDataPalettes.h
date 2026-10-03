#pragma once

#include "XPalette.h"
#include "XRemapTable.h"

#include <cstddef>
#include <cstdint>

namespace NC2E2
{
    struct SE2StaticDataPalette256
    {
        char name[0x20];
        NXBasics::CPalette* palette;             // +0x20; owned.
        NXBasics::CPalette* preshadedPalettes;    // +0x24; 31 owned objects after a DWORD cookie.
        NXBasics::CRemapTable* remapTable;        // +0x28; owned.
        SE2StaticDataPalette256* remapTarget;     // +0x2C; borrowed record in the same array.
    };

    struct SE2StaticDataPalette16
    {
        char name[0x20];
        std::int32_t palette256Id;
        std::uint32_t colorRange;
    };

    // Complete Windows static region 005550E0..005550FF.
    struct SStaticDataPalettes
    {
        std::int32_t palette256Count;
        SE2StaticDataPalette256* palettes256;
        std::int32_t palette16Count;
        SE2StaticDataPalette16* palettes16;
        std::int32_t fishPaletteId;
        std::int32_t guiBubblesPaletteId;
        NXBasics::CPalette* blackPalette;
        NXBasics::CPalette* vertexColorsPalette;
    };
    inline SStaticDataPalettes gStaticPalettes{};

    void* __fastcall E2StaticDataPalettesManager_Construct(void* manager, void* unusedEdx);
    std::int32_t E2StaticDataPalettesManager_GetPalette256Id(const char* name);
    std::int32_t E2StaticDataPalettesManager_GetPalette16Id(const char* name);

    static_assert(sizeof(SE2StaticDataPalette256) == 0x30);
    static_assert(offsetof(SE2StaticDataPalette256, palette) == 0x20);
    static_assert(offsetof(SE2StaticDataPalette256, preshadedPalettes) == 0x24);
    static_assert(offsetof(SE2StaticDataPalette256, remapTable) == 0x28);
    static_assert(offsetof(SE2StaticDataPalette256, remapTarget) == 0x2C);
    static_assert(sizeof(SE2StaticDataPalette16) == 0x28);
    static_assert(offsetof(SE2StaticDataPalette16, palette256Id) == 0x20);
    static_assert(offsetof(SE2StaticDataPalette16, colorRange) == 0x24);
    static_assert(sizeof(SStaticDataPalettes) == 0x20);
    static_assert(offsetof(SStaticDataPalettes, palettes256) == 0x04);
    static_assert(offsetof(SStaticDataPalettes, palette16Count) == 0x08);
    static_assert(offsetof(SStaticDataPalettes, palettes16) == 0x0C);
    static_assert(offsetof(SStaticDataPalettes, fishPaletteId) == 0x10);
    static_assert(offsetof(SStaticDataPalettes, guiBubblesPaletteId) == 0x14);
    static_assert(offsetof(SStaticDataPalettes, blackPalette) == 0x18);
    static_assert(offsetof(SStaticDataPalettes, vertexColorsPalette) == 0x1C);
}
