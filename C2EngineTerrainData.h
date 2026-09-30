#pragma once

#include <cstddef>

namespace NXBasics
{
    class CBitmap;
    class CPalette;
}

namespace NC2E2
{
    // Complete texture static region: 00568AF0..00568AF7.
    // CE2StaticDataTexturesManager owns the arrays; its loader remains original.
    struct SStaticDataTextures
    {
        NXBasics::CBitmap** bitmaps;
        NXBasics::CPalette** palettes;
    };
    extern SStaticDataTextures gStaticDataTextures;

    // Engine entry-point palette ownership at 005550FC remains original.
    extern NXBasics::CPalette* gVertexColorsPalette;
    static_assert(sizeof(SStaticDataTextures) == 8);
}

namespace NC2Logic
{
    // CStaticTrianglePatternTypeDataManager::mStaticVars at 005112E0.
    // Eleven 0x30-byte records, with the overview color at record +0x2C.
    // Allocation/loading/freeing remain in the original owning manager.
    extern std::byte* gStaticTrianglePatternTypes;
}
