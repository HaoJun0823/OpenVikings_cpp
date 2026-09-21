#include "XPixelFormatConverter.h"

#include <cstdint>
#include <new>

namespace
{
    std::uint32_t FindFirstSetBit(std::uint32_t mask, std::uint32_t maximumBitCount)
    {
        for (std::uint32_t bit = 0; bit < maximumBitCount; ++bit)
        {
            if ((mask & (std::uint32_t{ 1 } << bit)) != 0)
            {
                return bit;
            }
        }

        // This matches the original fallback.
        return 0;
    }

    std::uint32_t CountConsecutiveSetBits(std::uint32_t mask, std::uint32_t firstBit, std::uint32_t maximumBitCount)
    {
        std::uint32_t count = 0;

        for (std::uint32_t bit = firstBit;
            bit < maximumBitCount;
            ++bit)
        {
            if ((mask & (std::uint32_t{ 1 } << bit)) == 0)
            {
                break;
            }

            ++count;
        }

        return count;
    }
}

namespace NXBasics
{
    // Original function: FUN_00410117
    std::uint16_t C16BitPixelConverter::PackColor(const SColorRGB& color) const
    {
        const std::uint32_t blue = color.blue;
        const std::uint32_t green = color.green;
        const std::uint32_t red = color.red;

        return PackColor(red, green, blue);
    }

    // Original function: FUN_004102e8
    std::uint32_t C32BitPixelConverter::PackColor(std::uint32_t red, std::uint32_t green, std::uint32_t blue) const
    {
        if (!mInitialized)
        {
            return 0;
        }

        // x86 masks each 32-bit shift count to five bits.
        const std::uint32_t packedRed = (red >> (mRedRightShift & 31u)) << (mRedLeftShift & 31u);
        const std::uint32_t packedGreen = (green >> (mGreenRightShift & 31u)) << (mGreenLeftShift & 31u);
        const std::uint32_t packedBlue = (blue >> (mBlueRightShift & 31u)) << (mBlueLeftShift & 31u);

        return packedRed | packedGreen | packedBlue;
    }

    // Original function: FUN_00410326
    std::uint32_t C32BitPixelConverter::PackColor(const SColorRGB& color) const
    {
        const std::uint32_t blue = color.blue;
        const std::uint32_t green = color.green;
        const std::uint32_t red = color.red;

        return PackColor(red, green, blue);
    }

    C16BitPixelConverter* g16BitPixelConverter = nullptr;
    C32BitPixelConverter* g32BitPixelConverter = nullptr;

    // Original function: FUN_0040fe27
    C16BitPixelConverter::C16BitPixelConverter() :
        mInitialized(false),
        mPadding01{},
        mUnknownBuffer(nullptr),
        mDarkenedColorTable(nullptr),
        mRedMask(0),
        mGreenMask(0),
        mBlueMask(0),
        mPadding12{},

        mRedRightShift(0),
        mGreenRightShift(0),
        mBlueRightShift(0),
        mRedLeftShift(0),
        mGreenLeftShift(0),
        mBlueLeftShift(0),
        mRedAdjustment(0),
        mGreenAdjustment(0),
        mBlueAdjustment(0),
        mMiddleGrayColor(0),
        mUsesRgb565(false),
        mUsesRgb555(false)
    {}

    // Original function: FUN_0040fe34
    C16BitPixelConverter::~C16BitPixelConverter()
    {
        ::operator delete(mUnknownBuffer);
        mUnknownBuffer = nullptr;

        delete[] mDarkenedColorTable;
        mDarkenedColorTable = nullptr;
    }

    // Original function: FUN_0040fe5d
    void C16BitPixelConverter::SetColorMasks(std::uint16_t redMask, std::uint16_t greenMask, std::uint16_t blueMask)
    {
        mRedMask = redMask;
        mGreenMask = greenMask;
        mBlueMask = blueMask;

        mRedLeftShift = FindFirstSetBit(redMask, 16);

        const std::uint32_t redBitCount = CountConsecutiveSetBits(redMask, mRedLeftShift, 16);

        mRedRightShift = 8 - redBitCount;

        mRedAdjustment = static_cast<std::int32_t>(mRedLeftShift + redBitCount) - 5;

        mGreenLeftShift = FindFirstSetBit(greenMask, 16);

        const std::uint32_t greenBitCount = CountConsecutiveSetBits(greenMask, mGreenLeftShift, 16);

        mGreenRightShift = 8 - greenBitCount;
        mGreenAdjustment = static_cast<std::int32_t>(mGreenLeftShift + greenBitCount) - 5;
        mBlueLeftShift = FindFirstSetBit(blueMask, 16);

        const std::uint32_t blueBitCount = CountConsecutiveSetBits(blueMask, mBlueLeftShift, 16);

        mBlueRightShift = 8 - blueBitCount;
        mBlueAdjustment = static_cast<std::int32_t>(mBlueLeftShift + blueBitCount) - 5;

        mInitialized = true;

        mMiddleGrayColor = PackColor(0x7F, 0x7F, 0x7F);
        mUsesRgb565 = redMask == 0xF800 && greenMask == 0x07E0 && blueMask == 0x001F;
        mUsesRgb555 = redMask == 0x7C00 && greenMask == 0x03E0 && blueMask == 0x001F;

        ::operator delete(mUnknownBuffer);
        mUnknownBuffer = nullptr;

        delete[] mDarkenedColorTable;
        mDarkenedColorTable = new (std::nothrow) std::uint16_t[0x10000];

        if (mDarkenedColorTable == nullptr)
        {
            return;
        }

        for (std::uint32_t color = 0; color < 0x10000; ++color)
        {
            const std::uint32_t red = ((color & mRedMask) >> mRedLeftShift) * 2 / 3;
            const std::uint32_t green = ((color & mGreenMask) >> mGreenLeftShift) * 2 / 3;
            const std::uint32_t blue = ((color & mBlueMask) >> mBlueLeftShift) * 2 / 3;
            const std::uint32_t packedRed = red << mRedLeftShift;
            const std::uint32_t packedGreen = green << mGreenLeftShift;

            /*
             * The original adds 1/16 of red + blue to the blue
             * component before placing it in the destination mask.
             */
            const std::uint32_t adjustedBlue = blue + ((red + blue) >> 4);
            const std::uint32_t packedBlue = adjustedBlue << mBlueLeftShift;

            mDarkenedColorTable[color] = static_cast<std::uint16_t>(packedRed | packedGreen | packedBlue);
        }
    }

    // Original function: FUN_004100d8
    std::uint16_t C16BitPixelConverter::PackColor(std::uint32_t red, std::uint32_t green, std::uint32_t blue) const
    {
        if (!mInitialized)
        {
            return 0;
        }

        // x86 masks each 32-bit shift count to five bits.
        const std::uint32_t packedRed = (red >> (mRedRightShift & 31u)) << (mRedLeftShift & 31u);
        const std::uint32_t packedGreen = (green >> (mGreenRightShift & 31u)) << (mGreenLeftShift & 31u);
        const std::uint32_t packedBlue = (blue >> (mBlueRightShift & 31u)) << (mBlueLeftShift & 31u);

        return static_cast<std::uint16_t>(packedRed | packedGreen | packedBlue);
    }

    // Original function: FUN_0041022c
    C32BitPixelConverter::C32BitPixelConverter() : mInitialized(false), mPadding01{}, mRedMask(0), mGreenMask(0), mBlueMask(0), mRedRightShift(0), mGreenRightShift(0), mBlueRightShift(0), mRedLeftShift(0), mGreenLeftShift(0), mBlueLeftShift(0) {}

    // Original function: FUN_00410232
    void C32BitPixelConverter::SetColorMasks(std::uint32_t redMask, std::uint32_t greenMask, std::uint32_t blueMask)
    {
        mRedMask = redMask;
        mRedLeftShift = FindFirstSetBit(redMask, 32);

        const std::uint32_t redBitCount = CountConsecutiveSetBits(redMask, mRedLeftShift, 32);

        mRedRightShift = 8 - redBitCount;
        mGreenMask = greenMask;
        mGreenLeftShift = FindFirstSetBit(greenMask, 32);

        const std::uint32_t greenBitCount = CountConsecutiveSetBits(greenMask, mGreenLeftShift, 32);

        mGreenRightShift = 8 - greenBitCount;
        mBlueMask = blueMask;
        mBlueLeftShift = FindFirstSetBit(blueMask, 32);

        const std::uint32_t blueBitCount = CountConsecutiveSetBits(blueMask, mBlueLeftShift, 32);

        mBlueRightShift = 8 - blueBitCount;

        mInitialized = true;
    }
}
