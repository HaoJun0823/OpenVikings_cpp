#include "XBobRendering.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XPixelFormatConverter.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mmintrin.h>

namespace NXBasics
{
    // Original function: FUN_004660c6
    void BobManager_PrintBobUsingTransparency(const void* bobManager, std::uint32_t bobId, CBitmap* bitmap, std::int32_t x, std::int32_t y, const SColorRGB* colour)
    {
        static_assert(sizeof(void*) == 4);

        const auto readWord = [] (const void* address) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, address, sizeof(value));
            return value;
        };

        const auto offsetAddress = [] (const void* address, std::uint32_t offset) -> const std::byte*
        {
            const std::uintptr_t result = reinterpret_cast<std::uintptr_t>(address) + offset;
            return reinterpret_cast<const std::byte*>(result);
        };

        const auto readPointer = [&] (const void* object, std::uint32_t offset) -> const std::byte*
        {
            const std::byte* value;
            std::memcpy(&value, offsetAddress(object, offset), sizeof(value));
            return value;
        };

        const auto signedValue = [] (std::uint32_t value) -> std::int32_t
        {
            return Runtime_TimeSigned32(value);
        };

        const auto blendChannel = [] (std::uint32_t destination, std::uint32_t source, std::uint32_t alpha) -> std::uint32_t
        {
            return (destination * (256u - alpha) + source * alpha) >> 8;
        };

        const std::uint32_t index = bobId - readWord(offsetAddress(bobManager, 4));

        if (index >= readWord(offsetAddress(bobManager, 8)))
            return;

        const std::byte* entry = offsetAddress(readPointer(bobManager, 0x24), index * 24u);

        if (readWord(entry) == 0)
            return;

        SRectangle placed(0, 0, 0, 0);
        std::memcpy(&placed, entry + 4, sizeof(placed));

        placed.x = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(x));
        placed.y = signedValue(static_cast<std::uint32_t>(placed.y) + static_cast<std::uint32_t>(y));

        SRectangle bounds(0, 0, 0, 0);
        std::memcpy(&bounds, offsetAddress(bitmap, 8), sizeof(bounds));

        if (!placed.IsTouching(bounds))
            return;

        SRectangle clipped = placed;
        clipped.CutInside(bounds);

        const bool wasClipped = clipped.x != placed.x || clipped.y != placed.y || clipped.width != placed.width || clipped.height != placed.height;

        const std::byte* rowTable = offsetAddress(readPointer(bobManager, 0x2C), readWord(entry + 0x14) * 4u);

        if (placed.y < 0)
            rowTable = offsetAddress(rowTable, (0u - static_cast<std::uint32_t>(placed.y)) * 4u);

        const std::int32_t placedRight = signedValue(static_cast<std::uint32_t>(placed.x) + static_cast<std::uint32_t>(placed.width) - 1u);
        const bool clipRight = placedRight >= signedValue(readWord(offsetAddress(bitmap, 0x10)));

        if (readWord(entry) != 1)
            return;

        const std::uint32_t depth = readWord(offsetAddress(bitmap, 4));

        if (depth == 0)
            return;

        if (depth != 1 && depth != 2)
        {
            _mm_empty();
            return;
        }

        std::uint32_t sourceColour = 0;
        std::uint32_t fastSourceColour = 0;
        std::uint32_t greenMask = 0;
        bool useMmx;

        if (depth == 2)
        {
            sourceColour = static_cast<std::uint32_t>(colour->blue);
            sourceColour |= static_cast<std::uint32_t>(colour->green) << 8;
            sourceColour |= static_cast<std::uint32_t>(colour->red) << 16;

            useMmx = FUN_00473abf();
        }
        else
        {
            const std::byte* converterBytes = reinterpret_cast<const std::byte*>(g16BitPixelConverter);
            greenMask = converterBytes[0x3A] != std::byte{ 0 } ? 0x07E0u : 0x03E0u;
            sourceColour = g16BitPixelConverter->PackColor(*colour);

            useMmx = FUN_00473abf();

            if (useMmx)
                fastSourceColour = g16BitPixelConverter->PackColor(*colour);
        }

        // The original reuses the colour argument slot in clipped pixel loops.
        const std::byte* fastColourAddress = reinterpret_cast<const std::byte*>(colour);

        const auto blendPixel = [&] (const std::byte* address, std::uint32_t alpha, bool fast)
        {
            if (depth == 2)
            {
                const std::uint32_t destination = readWord(address);
                const bool mmxPixel = useMmx && fast;
                const std::uint32_t source = mmxPixel ? fastSourceColour : sourceColour;
                const std::uint32_t channelCount = mmxPixel ? 4u : 3u;
                std::uint32_t result = 0;

                for (std::uint32_t channel = 0; channel < channelCount; ++channel)
                {
                    const std::uint32_t shift = channel * 8u;
                    const std::uint32_t destinationChannel = (destination >> shift) & 0xFFu;
                    const std::uint32_t sourceChannel = (source >> shift) & 0xFFu;

                    result |= blendChannel(destinationChannel, sourceChannel, alpha) << shift;
                }

                std::memcpy(const_cast<std::byte*>(address), &result, sizeof(result));
            }
            else
            {
                std::uint16_t destinationWord;
                std::memcpy(&destinationWord, address, sizeof(destinationWord));

                const std::uint32_t destination = destinationWord;
                const std::uint32_t inverseAlpha = 256u - alpha;
                std::uint32_t result;

                if (useMmx && fast)
                {
                    // Reproduce the independent MMX byte lanes without changing rounding.
                    const std::uint32_t otherMask = (~greenMask) & 0xFFFFu;
                    const std::uint32_t destinationOther = destination & otherMask;
                    const std::uint32_t sourceOther = fastSourceColour & otherMask;

                    const std::uint32_t lowByte = blendChannel(destinationOther & 0xFFu, sourceOther & 0xFFu, alpha);
                    const std::uint32_t highByte = blendChannel(destinationOther >> 8, sourceOther >> 8, alpha);
                    const std::uint32_t other = (lowByte | (highByte << 8)) & otherMask;

                    const std::uint32_t destinationGreen = (destination & greenMask) >> 4;
                    const std::uint32_t sourceGreen = (fastSourceColour & greenMask) >> 4;
                    const std::uint32_t green = ((destinationGreen * inverseAlpha + sourceGreen * alpha) >> 4) & greenMask;

                    result = other + green;
                }
                else
                {
                    // The scalar path uses 0xF800 even with the RGB555 green mask.
                    const std::uint32_t red = (((destination & 0xF800u) * inverseAlpha + (sourceColour & 0xF800u) * alpha) >> 8) & 0xF800u;
                    const std::uint32_t green = (((destination & greenMask) * inverseAlpha + (sourceColour & greenMask) * alpha) >> 8) & greenMask;
                    const std::uint32_t blue = ((destination & 0x1Fu) * inverseAlpha + (sourceColour & 0x1Fu) * alpha) >> 8;

                    result = red + green + blue;
                }

                const std::uint16_t resultWord = static_cast<std::uint16_t>(result);
                std::memcpy(const_cast<std::byte*>(address), &resultWord, sizeof(resultWord));
            }
        };

        std::uint32_t destinationY = static_cast<std::uint32_t>(clipped.y);
        const std::uint32_t pixelBytes = depth == 2 ? 4u : 2u;
        const std::uint32_t pixelPointerOffset = depth == 2 ? 0x2Cu : 0x28u;
        const std::int32_t clippedRight = signedValue(static_cast<std::uint32_t>(clipped.x) + static_cast<std::uint32_t>(clipped.width) - 1u);

        for (std::int32_t row = 0; row < clipped.height; ++row, ++destinationY)
        {
            const std::uint32_t descriptor = readWord(rowTable);
            rowTable = offsetAddress(rowTable, 4);

            if (descriptor == 0xFFFFFFFFu)
                continue;

            std::uint32_t currentX = static_cast<std::uint32_t>(placed.x) + (descriptor >> 16);
            const std::byte* input = offsetAddress(readPointer(bobManager, 0x28), descriptor & 0x003FFFFFu);

            bool fast = depth == 1 && !wasClipped;
            bool fastPrepared = false;
            const std::byte* output = nullptr;

            for (;;)
            {
                if (!fast && signedValue(currentX) >= 0 && !clipRight)
                    fast = true;

                if (fast && !fastPrepared)
                {
                    const std::uint32_t pixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY + currentX;
                    output = offsetAddress(readPointer(bitmap, pixelPointerOffset), pixelIndex * pixelBytes);

                    if (depth == 2 && useMmx)
                    {
                        // Preserve the original read from the reused argument slot.
                        fastSourceColour = readWord(fastColourAddress);
                    }

                    fastPrepared = true;
                }

                const std::uint8_t token = static_cast<std::uint8_t>(*input);
                input = offsetAddress(input, 1);

                if (token == 0)
                    break;

                const std::uint32_t count = token & 0x7Fu;

                if ((token & 0x80u) != 0)
                {
                    if (fast)
                        output = offsetAddress(output, count * pixelBytes);
                    else
                        currentX += count;

                    continue;
                }

                if (fast)
                {
                    for (std::uint32_t pixel = 0; pixel < count; ++pixel)
                    {
                        const std::uint32_t alpha = static_cast<std::uint8_t>(*input);
                        input = offsetAddress(input, 1);

                        blendPixel(output, alpha, true);
                        output = offsetAddress(output, pixelBytes);
                    }
                }
                else
                {
                    const std::uint32_t rowPixelIndex = readWord(offsetAddress(bitmap, 0x30)) * destinationY;
                    const std::byte* rowPixels = offsetAddress(readPointer(bitmap, pixelPointerOffset), rowPixelIndex * pixelBytes);
                    const std::byte* pixelAddress = offsetAddress(rowPixels, currentX * pixelBytes);

                    for (std::uint32_t pixel = 0; pixel < count; ++pixel)
                    {
                        if (signedValue(currentX) >= 0 && signedValue(currentX) <= clippedRight)
                        {
                            const std::uint32_t alpha = static_cast<std::uint8_t>(*input);
                            blendPixel(pixelAddress, alpha, false);
                        }

                        ++currentX;
                        input = offsetAddress(input, 1);
                        pixelAddress = offsetAddress(pixelAddress, pixelBytes);

                        if (depth == 2)
                            fastColourAddress = pixelAddress;
                    }
                }
            }
        }

        _mm_empty();
    }
}