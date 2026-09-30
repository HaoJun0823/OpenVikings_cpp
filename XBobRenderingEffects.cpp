#include "XBobRendering.h"
#include "XBasicTypes.h"
#include "XCPU.h"
#include "XCrtFileAccess.h"
#include "XPaletteAccess.h"
#include "XPixelFormatConverter.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mmintrin.h>

namespace NXBasics
{
    namespace
    {
        std::byte* BobAddress(const void* base, std::uint32_t offset)
        {
            return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(base)) + offset));
        }

        template<typename T>
        T BobRead(const void* base, std::uint32_t offset = 0)
        {
            T value;
            std::memcpy(&value, BobAddress(base, offset), sizeof(value));
            return value;
        }

        std::byte* BobPointer(const void* base, std::uint32_t offset)
        {
            return reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(BobRead<std::uint32_t>(base, offset)));
        }

        struct BobDrawRegion
        {
            SRectangle placed{ 0, 0, 0, 0 };
            SRectangle clipped{ 0, 0, 0, 0 };
            std::byte* rows = nullptr;
            std::uint32_t type = 0;
            bool wasClipped = false;
            bool clipRight = false;
        };

        bool PrepareBobDraw(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, BobDrawRegion& region)
        {
            static_assert(sizeof(void*) == 4);
            const std::uint32_t index = bobId - BobRead<std::uint32_t>(bobManager, 4);
            if (index >= BobRead<std::uint32_t>(bobManager, 8))
                return false;

            const std::byte* entry = BobAddress(BobPointer(bobManager, 0x24), index * 24u);
            region.type = BobRead<std::uint32_t>(entry);
            if (region.type == 0)
                return false;

            std::memcpy(&region.placed, entry + 4, sizeof(SRectangle));
            region.placed.x = Runtime_TimeSigned32(static_cast<std::uint32_t>(region.placed.x) + static_cast<std::uint32_t>(x));
            region.placed.y = Runtime_TimeSigned32(static_cast<std::uint32_t>(region.placed.y) + static_cast<std::uint32_t>(y));
            SRectangle bounds(0, 0, 0, 0);
            std::memcpy(&bounds, BobAddress(bitmap, 8), sizeof(bounds));
            if (!region.placed.IsTouching(bounds))
                return false;

            region.clipped = region.placed;
            // FUN_004104d6 has the same cuts as CutInside and returns whether a cut occurred.
            region.clipped.CutInside(bounds);
            region.wasClipped = !(region.clipped == region.placed);
            region.rows = BobAddress(BobPointer(bobManager, 0x2C), BobRead<std::uint32_t>(entry, 0x14) * 4u);
            if (region.placed.y < 0)
                region.rows = BobAddress(region.rows, (0u - static_cast<std::uint32_t>(region.placed.y)) * 4u);

            const std::int32_t right = Runtime_TimeSigned32(static_cast<std::uint32_t>(region.placed.x) + static_cast<std::uint32_t>(region.placed.width) - 1u);
            region.clipRight = right >= BobRead<std::int32_t>(bitmap, 0x10);
            return true;
        }

        template<typename DrawPixel>
        void DrawBobRuns(const void* bobManager, CBitmap* bitmap, const BobDrawRegion& region, std::uint32_t sourceBytes, bool whollyUnchecked, bool useBytePitch, DrawPixel drawPixel)
        {
            const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
            const std::uint32_t pixelBytes = depth == 1 ? 2u : 4u;
            const std::byte* pixels = BobPointer(bitmap, depth == 1 ? 0x28u : 0x2Cu);
            const std::uint32_t stride = BobRead<std::uint32_t>(bitmap, 0x30);
            const std::uint32_t rowStep = useBytePitch ? BobRead<std::uint32_t>(bitmap, 0x38) : stride * pixelBytes;
            const std::int32_t right = Runtime_TimeSigned32(static_cast<std::uint32_t>(region.clipped.x) + static_cast<std::uint32_t>(region.clipped.width) - 1u);
            std::byte* destinationRow = BobAddress(pixels, stride * static_cast<std::uint32_t>(region.clipped.y) * pixelBytes);
            const std::byte* rows = region.rows;

            for (std::int32_t row = 0; row < region.clipped.height; ++row)
            {
                const std::uint32_t descriptor = BobRead<std::uint32_t>(rows);
                rows = BobAddress(rows, 4);
                if (descriptor != 0xFFFFFFFFu)
                {
                    std::uint32_t currentX = static_cast<std::uint32_t>(region.placed.x) + (descriptor >> 22);
                    const std::byte* source = BobAddress(BobPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);
                    bool unchecked = whollyUnchecked || (!region.clipRight && Runtime_TimeSigned32(currentX) >= 0);
                    for (;;)
                    {
                        const std::uint8_t token = BobRead<std::uint8_t>(source);
                        source = BobAddress(source, 1);
                        if (token == 0)
                            break;
                        const std::uint32_t count = token & 0x7Fu;
                        if ((token & 0x80u) != 0)
                            currentX += count;
                        else
                        {
                            for (std::uint32_t pixel = 0; pixel < count; ++pixel)
                            {
                                // Windows checks against zero, even when bounds.x is nonzero.
                                const std::int32_t signedX = Runtime_TimeSigned32(currentX);
                                if (unchecked || (signedX >= 0 && signedX <= right))
                                    drawPixel(BobAddress(destinationRow, currentX * pixelBytes), source, unchecked, static_cast<std::uint32_t>(region.clipped.y) + static_cast<std::uint32_t>(row), currentX, pixel, count);
                                source = BobAddress(source, sourceBytes);
                                ++currentX;
                            }
                        }
                        // The switch to the unchecked path occurs between runs.
                        if (!unchecked && !region.clipRight && Runtime_TimeSigned32(currentX) >= 0)
                            unchecked = true;
                    }
                }
                destinationRow = BobAddress(destinationRow, rowStep);
            }
        }

        std::uint32_t ScaleMmxColour(std::uint32_t colour, std::uint32_t brightness)
        {
            const std::uint32_t half = brightness >> 1;
            const std::uint32_t repeated = half | ((half >> 16) | (half << 16));
            std::uint32_t result = 0;
            for (std::uint32_t channel = 0; channel < 4; ++channel)
            {
                const std::uint32_t factor = (repeated >> ((channel & 1u) * 16u)) & 0xFFFFu;
                const std::uint32_t scaled = static_cast<std::uint16_t>(((colour >> (channel * 8u)) & 0xFFu) * factor) >> 6;
                result |= (std::min)(scaled, 255u) << (channel * 8u);
            }
            return result;
        }

        std::uint32_t PackMmxHighColour(std::uint32_t colour, bool rgb565)
        {
            const std::uint32_t mask = rgb565 ? 63u : 31u;
            return ((colour >> 3) & mask) | ((colour >> (rgb565 ? 5u : 6u)) & (mask << 5)) | ((colour >> (rgb565 ? 8u : 9u)) & (mask << (rgb565 ? 11u : 10u)));
        }

        std::uint32_t ScalePaletteColour(const void* palette, std::uint8_t index, std::uint32_t brightness, const void* converter, bool highColour)
        {
            const std::byte* colour = BobAddress(palette, 4u + static_cast<std::uint32_t>(index) * 4u);
            std::uint32_t result = 0;
            for (std::uint32_t channel = 0; channel < 3; ++channel)
            {
                const std::uint32_t rgbChannel = 2u - channel;
                const std::uint32_t value = static_cast<std::uint32_t>(Runtime_TimeSigned32(BobRead<std::uint8_t>(colour, rgbChannel) * brightness) >> 7);
                const std::uint32_t mask = highColour ? BobRead<std::uint16_t>(converter, 0x0Cu + channel * 2u) : BobRead<std::uint32_t>(converter, 4u + channel * 4u);
                const std::uint32_t right = BobRead<std::uint32_t>(converter, (highColour ? 0x14u : 0x10u) + channel * 4u) & 31u;
                const std::uint32_t left = BobRead<std::uint32_t>(converter, (highColour ? 0x20u : 0x1Cu) + channel * 4u) & 31u;
                result |= value > 255u ? mask : (value >> right) << left;
            }
            return result;
        }
    }

    // Original function: FUN_00463f7c
    void BobManager_PrintBob_Remapped(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, const void* remapping)
    {
        BobDrawRegion region;
        if (!PrepareBobDraw(bobManager, bobId, bitmap, x, y, region) || region.type != 1u)
            return;
        const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;
        const void* table = depth == 1u ? static_cast<const void*>(Palette_GetHighColorTablePtr(palette)) : Palette_GetTrueColorTablePtr(palette);
        if (table == nullptr)
            return;
        DrawBobRuns(bobManager, bitmap, region, 1, depth == 1u && !region.wasClipped, false, [&](std::byte* destination, const std::byte* source, bool, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)
        {
            const std::uint32_t index = BobRead<std::uint8_t>(remapping, BobRead<std::uint8_t>(source));
            const std::uint32_t bytes = depth == 1u ? 2u : 4u;
            std::memcpy(destination, BobAddress(table, index * bytes), bytes);
        });
    }

    // Original function: FUN_00466d39
    void BobManager_PrintBob_UsingShadedAlpha(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, std::int32_t brightness)
    {
        BobDrawRegion region;
        if (!PrepareBobDraw(bobManager, bobId, bitmap, x, y, region) || region.type != 4u)
            return;
        const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
        {
            _mm_empty();
            return;
        }
        const void* table = depth == 1u ? static_cast<const void*>(Palette_GetHighColorTablePtr(palette)) : Palette_GetTrueColorTablePtr(palette);
        if (table == nullptr)
            return;
        const bool rgb565 = depth == 1u && BobRead<std::uint8_t>(g16BitPixelConverter, 0x3A) != 0;
        const std::uint32_t greenMask = rgb565 ? 0x7E0u : 0x3E0u;
        const bool mmx = CPU_HasMMX();
        const std::uint32_t* trueTable = nullptr;
        const std::uint32_t* conversionTable = nullptr;
        if (depth == 1u && mmx)
        {
            trueTable = Palette_GetTrueColorTablePtr(palette);
            conversionTable = g16BitPixelConverter->GetTrueColorConvertTablePtr();
        }
        const std::uint32_t factor = static_cast<std::uint32_t>(brightness);
        DrawBobRuns(bobManager, bitmap, region, 2, depth == 1u && !region.wasClipped, false, [&](std::byte* destination, const std::byte* source, bool unchecked, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)
        {
            const std::uint32_t index = BobRead<std::uint8_t>(source);
            const std::uint32_t alpha = BobRead<std::uint8_t>(source, 1);
            std::uint32_t result = 0;
            if (mmx && unchecked)
            {
                const std::uint32_t foreground = ScaleMmxColour(depth == 1u ? trueTable[index] : BobRead<std::uint32_t>(table, index * 4u), factor);
                const std::uint32_t background = depth == 1u ? conversionTable[BobRead<std::uint16_t>(destination)] : BobRead<std::uint32_t>(destination);
                for (std::uint32_t channel = 0; channel < 4; ++channel)
                {
                    const std::uint32_t shift = channel * 8u;
                    const std::uint32_t sum = ((background >> shift) & 255u) * (256u - alpha) + ((foreground >> shift) & 255u) * alpha;
                    result |= ((std::min)(sum, 65535u) >> 8) << shift;
                }
                if (depth == 1u)
                    result = PackMmxHighColour(result, rgb565);
            }
            else
            {
                const std::uint32_t foreground = depth == 1u ? BobRead<std::uint16_t>(table, index * 2u) : BobRead<std::uint32_t>(table, index * 4u);
                const std::uint32_t background = depth == 1u ? BobRead<std::uint16_t>(destination) : BobRead<std::uint32_t>(destination);
                const std::uint32_t masks[3] = { depth == 1u ? 0xF800u : 0xFF0000u, depth == 1u ? greenMask : 0xFF00u, depth == 1u ? 0x1Fu : 0xFFu };
                for (std::uint32_t channel = 0; channel < 3; ++channel)
                {
                    const std::uint32_t mask = masks[channel];
                    const std::uint32_t sum = (((foreground & mask) * alpha) >> 7) * factor + (background & mask) * (256u - alpha);
                    const std::uint32_t limited = (std::min)(sum, mask << 8);
                    result += channel == 2 ? limited >> 8 : (limited >> 8) & mask;
                }
            }
            std::memcpy(destination, &result, depth == 1u ? 2u : 4u);
        });
        _mm_empty();
    }

    // Original function: FUN_00498128
    void BobManager_PrintBob_UsingEffectMatrix(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y)
    {
        BobDrawRegion region;
        if (!PrepareBobDraw(bobManager, bobId, bitmap, x, y, region) || region.type != 1u)
            return;
        const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;
        const std::uint32_t bytes = depth == 1u ? 2u : 4u;
        DrawBobRuns(bobManager, bitmap, region, 1, depth == 1u && !region.wasClipped, false, [&](std::byte* destination, const std::byte* source, bool, std::uint32_t destinationY, std::uint32_t destinationX, std::uint32_t, std::uint32_t)
        {
            const std::uint32_t sourceY = destinationY + BobRead<std::uint8_t>(source);
            if (Runtime_TimeSigned32(sourceY) < BobRead<std::int32_t>(bitmap, 0x14))
            {
                const std::uint32_t index = sourceY * BobRead<std::uint32_t>(bitmap, 0x30) + destinationX;
                const std::byte* displaced = BobAddress(BobPointer(bitmap, depth == 1u ? 0x28u : 0x2Cu), index * bytes);
                const std::uint32_t pixel = depth == 1u ? BobRead<std::uint16_t>(displaced) : BobRead<std::uint32_t>(displaced);
                std::memcpy(destination, &pixel, bytes);
            }
        });
    }

    // Original function: FUN_0049bf5a
    void BobManager_PrintBob_Shade_MMX(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, CPalette* palette, std::int32_t brightness)
    {
        BobDrawRegion region;
        if (!PrepareBobDraw(bobManager, bobId, bitmap, x, y, region) || region.type != 1u)
            return;
        if (!CPU_HasMMX())
        {
            BobManager_PrintBob(bobManager, bobId, bitmap, x, y, palette);
            return;
        }
        const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;
        const bool rgb565 = depth == 1u && BobRead<std::uint8_t>(g16BitPixelConverter, 0x3A) != 0;
        DrawBobRuns(bobManager, bitmap, region, 1, !region.wasClipped, !region.wasClipped, [&](std::byte* destination, const std::byte* source, bool, std::uint32_t, std::uint32_t, std::uint32_t pixel, std::uint32_t count)
        {
            std::uint32_t result;
            if (region.wasClipped)
                result = ScalePaletteColour(palette, BobRead<std::uint8_t>(source), static_cast<std::uint32_t>(brightness), depth == 1u ? static_cast<const void*>(g16BitPixelConverter) : g32BitPixelConverter, depth == 1u);
            else
            {
                result = ScaleMmxColour(BobRead<std::uint32_t>(palette, 4u + BobRead<std::uint8_t>(source) * 4u), static_cast<std::uint32_t>(brightness));
                if (depth == 1u)
                {
                    result = PackMmxHighColour(result, rgb565);
                    // Paired pixels pass through PSRLW/PACKSSDW/PSLLW and lose bit zero.
                    if (pixel < (count & ~1u))
                        result &= 0xFFFEu;
                }
            }
            std::memcpy(destination, &result, depth == 1u ? 2u : 4u);
        });
        if (!region.wasClipped)
            _mm_empty();
    }

    // Original function: FUN_0049c80f
    void BobManager_PrintBob_FX_DrawArea(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour, std::int32_t strength)
    {
        BobDrawRegion region;
        if (!PrepareBobDraw(bobManager, bobId, bitmap, x, y, region) || (region.type != 1u && region.type != 4u))
            return;
        const std::uint32_t depth = BobRead<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;
        const std::uint32_t foreground = depth == 1u ? g16BitPixelConverter->PackColor(*colour) : g32BitPixelConverter->PackColor(*colour);
        const std::uint32_t greenMask = depth == 1u && BobRead<std::uint8_t>(g16BitPixelConverter, 0x3A) != 0 ? 0x07E007E0u : 0x03E003E0u;
        const std::uint32_t alpha = static_cast<std::uint32_t>(strength);
        const std::uint32_t inverseAlpha = (depth == 1u && region.wasClipped ? 255u : 256u) - alpha;
        DrawBobRuns(bobManager, bitmap, region, region.type == 1u ? 1u : 2u, depth == 1u && !region.wasClipped, false, [&](std::byte* destination, const std::byte*, bool, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t)
        {
            const std::uint32_t background = depth == 1u ? BobRead<std::uint16_t>(destination) : BobRead<std::uint32_t>(destination);
            const std::uint32_t redMask = depth == 1u ? 0xF800u : 0xFF0000u;
            const std::uint32_t middleMask = depth == 1u ? greenMask : 0xFF00u;
            const std::uint32_t blueMask = depth == 1u ? 0x1Fu : 0xFFu;
            const std::uint32_t red = (((background & redMask) * inverseAlpha + (foreground & redMask) * alpha) >> 8) & redMask;
            const std::uint32_t green = (((background & middleMask) * inverseAlpha + (foreground & middleMask) * alpha) >> 8) & middleMask;
            const std::uint32_t blue = ((background & blueMask) * inverseAlpha + (foreground & blueMask) * alpha) >> 8;
            const std::uint32_t result = red + green + blue;
            std::memcpy(destination, &result, depth == 1u ? 2u : 4u);
        });
    }
}
