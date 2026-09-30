#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SColorRGB;
    class CPalette;

    std::uint32_t BobManager_GetBobType(const void* bobManager, std::uint32_t bobId);
    void BobManager_PrintBob(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette);
    void BobManager_PrintBob_Remapped(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, const void* remapping);
    void BobManager_PrintBob_UsingEffectMatrix(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y);
    void BobManager_PrintBob_Shade_MMX(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, std::int32_t brightness);
    void BobManager_PrintBob_FX_DrawArea(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour, std::int32_t strength);
    void BobManager_PrintBob_UsingShadedAlpha(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, std::int32_t brightness);
    void BobManager_PrintBob_DoubleByteBobNormal(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette);
    void BobManager_PrintBob_Shadow(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y);
    void BobManager_PrintBob_Shadow16(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, std::uint32_t mode);
    void BobManager_PrintBob_Shadow32(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, std::uint32_t mode);
    void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette);
    void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour);
}
