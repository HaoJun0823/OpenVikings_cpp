#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SColorRGB;
    class CPalette;

    void BobManager_PrintBob(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette);
    void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette);
    void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour);
}