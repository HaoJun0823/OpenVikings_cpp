#include "XSystemManager.h"
#include "OriginalStrings.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XBitmapTools.h"
#include "XCrtFileAccess.h"
#include "XDiagnostics.h"
#include "XFileAccess.h"
#include "XFontMetrics.h"
#include "XPixelFormatConverter.h"
#include "XPaletteAccess.h"
#include "XMemory.h"

#include <cstdarg>
#include <cstring>
#include <new>

namespace NXBasics
{
    template <typename T>

    T* BitmapPixelAddress(T* pixels, std::uint32_t index)
    {
        // Preserve the original 32-bit byte-offset arithmetic.
        const std::uint32_t offset = index * static_cast<std::uint32_t>(sizeof(T));
        return reinterpret_cast<T*>(reinterpret_cast<std::uintptr_t>(pixels) + offset);
    }

    std::int32_t ScaleBitmapCoordinate(std::int32_t sourceExtent, std::int32_t coordinate, std::int32_t destinationExtent)
    {
        // IMUL keeps the low 32 bits; IDIV then interprets them as signed.
        const std::uint32_t productBits = static_cast<std::uint32_t>(sourceExtent) * static_cast<std::uint32_t>(coordinate);
        std::int32_t product;
        std::memcpy(&product, &productBits, sizeof(product));
        return product / destinationExtent;
    }

    // Original function: LAB_0040c775
    std::uint32_t CBitmap::Storable_GetVersion()
    {
        return 0;
    }

    // Original function: LAB_0040d00a
    std::uint32_t CBitmap::Storable_GetId()
    {
        return 0x3F3;
    }

    // Original function: FUN_0040c624
    CBitmap::CBitmap(std::int32_t width, std::int32_t height, std::int32_t depth) : mRectangle(0, 0, 0, 0)
    {
        l_InitObject();
        SRectangle rectangle(0, 0, width, height);
        l_ConstructNonVirtual(rectangle, depth);
    }

    // Original function: FUN_0040c65f
    CBitmap::CBitmap(CBitmap* sourceBitmap, const SRectangle& rectangle) : mRectangle(0, 0, 0, 0)
    {
        l_InitObject();
        l_ConstructVirtual(sourceBitmap, rectangle);
    }

    // Original function: FUN_0040c6bf
    CBitmap::~CBitmap()
    {
        auto* memory = static_cast<CMemory*>(mMemory);

        if (memory != nullptr)
        {
            delete memory;
            mMemory = nullptr;
        }
    }

    // Original function: FUN_0040c734
    void CBitmap::Storable_SaveData(void* file)
    {
        if (mIsVirtual != 0)
        {
            NXBasics::ReportFatalError(OriginalStrings::CannotSaveVirtualBitmap);
        }

        File_Write(file, &mDepth, 0x04);
        File_Write(file, &mRectangle, 0x10);
        static_cast<CMemory*>(mMemory)->Storable_Save(file);
    }

    // Original function: FUN_0040c778
    void CBitmap::l_ConstructNonVirtual(const SRectangle& rectangle, std::int32_t depth)
    {
        const std::uint32_t pixelCount = static_cast<std::uint32_t>(rectangle.width) * static_cast<std::uint32_t>(rectangle.height);

        std::uint32_t allocationSize;

        if (depth == 1)
        {
            mDepth = 1;
            allocationSize = pixelCount * 2u;
        }
        else if (depth == 2)
        {
            mDepth = 2;
            allocationSize = pixelCount * 4u;
        }
        else
        {
            mDepth = 0;
            allocationSize = pixelCount;
        }

        void* memoryStorage = ::operator new(0x0C, std::nothrow);
        void* memory = nullptr;

        if (memoryStorage != nullptr)
        {
            memory = ::new (memoryStorage) CMemory(allocationSize);
        }

        mMemory = memory;

        l_SetRawMemoryPtr();

        const std::uint32_t rowPixelCount = static_cast<std::uint32_t>(rectangle.width);

        mRowByteCount = mBytesPerPixel * rowPixelCount;
        mRowPixelCount = rowPixelCount;

        mRectangle.SetVariables(0, 0, rectangle.width, rectangle.height);

        mRightBits = static_cast<std::uint32_t>(mRectangle.x) + static_cast<std::uint32_t>(mRectangle.width) - 1u;
        mBottomBits = static_cast<std::uint32_t>(mRectangle.y) + static_cast<std::uint32_t>(mRectangle.height) - 1u;

        mIsVirtual = 0;
        mSourceX = 0;
        mSourceY = 0;
    }

    // Original function: FUN_0040c836
    void CBitmap::l_ConstructVirtual(CBitmap* sourceBitmap, const SRectangle& rectangle)
    {
        mIsVirtual = 1;
        mMemory = nullptr;
        mRawMemory = nullptr;
        mHighColorMemory = nullptr;
        mTrueColorMemory = nullptr;

        mDepth = sourceBitmap->mDepth;

        SRectangle clippedRectangle(rectangle);

        if (sourceBitmap->mRawMemory == nullptr)
        {
            return;
        }

        if (!clippedRectangle.IsTouching(sourceBitmap->mRectangle))
        {
            return;
        }

        clippedRectangle.CutInside(sourceBitmap->mRectangle);

        const std::int32_t sourceY = clippedRectangle.y;
        const std::uint32_t pixelOffset = sourceBitmap->mRowPixelCount * static_cast<std::uint32_t>(sourceY) + static_cast<std::uint32_t>(clippedRectangle.x);

        // Preserve the original 32-bit address arithmetic.
        if (mDepth == 1)
        {
            const std::uint32_t byteOffset = pixelOffset * 2u;

            mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + byteOffset);
            mHighColorMemory = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mHighColorMemory) + byteOffset);
            mTrueColorMemory = nullptr;
        }
        else if (mDepth == 2)
        {
            const std::uint32_t byteOffset = pixelOffset * 4u;

            mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + byteOffset);
            mTrueColorMemory = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mTrueColorMemory) + byteOffset);
            mHighColorMemory = nullptr;
        }
        else
        {
            mRawMemory = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(sourceBitmap->mRawMemory) + pixelOffset);
            mHighColorMemory = nullptr;
            mTrueColorMemory = nullptr;
        }

        mSourceX = clippedRectangle.x;
        mSourceY = sourceY;
        mSourceBitmap = sourceBitmap;

        mRectangle.SetVariables(0, 0, clippedRectangle.width, clippedRectangle.height);

        mRightBits = static_cast<std::uint32_t>(mRectangle.x) + static_cast<std::uint32_t>(mRectangle.width) - 1u;
        mBottomBits = static_cast<std::uint32_t>(mRectangle.y) + static_cast<std::uint32_t>(mRectangle.height) - 1u;

        mRowPixelCount = sourceBitmap->mRowPixelCount;
        mBytesPerPixel = sourceBitmap->mBytesPerPixel;
        mRowByteCount = sourceBitmap->mRowByteCount;
    }

    // Original function: FUN_0040c931
    void CBitmap::l_InitObject()
    {
        std::memset(reinterpret_cast<std::byte*>(this) + 0x04, 0, 0x50);
        mDepth = 0;
        mRectangle.Init();
    }

    // Original function: FUN_0040c953
    void CBitmap::l_SetRawMemoryPtr()
    {
        const auto* memoryBytes = static_cast<const std::byte*>(mMemory);

        if (mDepth == 1)
        {
            std::memcpy(&mRawMemory, memoryBytes + 0x08, sizeof(mRawMemory));

            std::uint16_t* highColorMemory;
            std::memcpy(&highColorMemory, memoryBytes + 0x08, sizeof(highColorMemory));

            mTrueColorMemory = nullptr;
            mHighColorMemory = highColorMemory;
            mBytesPerPixel = 2;
        }
        else if (mDepth == 2)
        {
            std::memcpy(&mRawMemory, memoryBytes + 0x08, sizeof(mRawMemory));

            std::uint32_t* trueColorMemory;
            std::memcpy(&trueColorMemory, memoryBytes + 0x08, sizeof(trueColorMemory));

            mHighColorMemory = nullptr;
            mTrueColorMemory = trueColorMemory;
            mBytesPerPixel = 4;
        }
        else
        {
            std::uint8_t* rawMemory;
            std::memcpy(&rawMemory, memoryBytes + 0x08, sizeof(rawMemory));

            mHighColorMemory = nullptr;
            mTrueColorMemory = nullptr;
            mRawMemory = rawMemory;
            mBytesPerPixel = 1;
        }
    }

    // Original function: FUN_0040c9a8
    void CBitmap::Fill(std::uint8_t colorIndex)
    {
        std::uint8_t* pixels = mRawMemory;

        if (pixels == nullptr)
        {
            return;
        }

        switch (mDepth)
        {
        case 0:
            XB_Tool_Byte_FillBlock(pixels, static_cast<std::uint32_t>(mRectangle.width), mRectangle.height, mRowPixelCount, colorIndex);
            break;

        case 1:
        {
            const std::uint16_t packedColor = GetHighColorWord(colorIndex);
            Fill(packedColor);
            break;
        }

        case 2:
        {
            const std::uint32_t packedColor = GetTrueColorWord(colorIndex);
            Fill(packedColor);
            break;
        }
        }
    }

    // Original function: FUN_0040ca04
    void CBitmap::Fill(std::uint16_t color)
    {
        std::uint16_t* pixels = mHighColorMemory;

        if (pixels == nullptr || mDepth != 1)
        {
            return;
        }

        XB_Tool_Word_FillBlock(pixels, mRectangle.width, mRectangle.height, mRowPixelCount, color);
    }

    // Original function: FUN_0040ca2a
    void CBitmap::Fill(std::uint32_t color)
    {
        std::uint32_t* pixels = mTrueColorMemory;

        if (pixels == nullptr || mDepth != 2)
        {
            return;
        }

        XB_Tool_Long_FillBlock(pixels, mRectangle.width, mRectangle.height, mRowPixelCount, color);
    }

    // Original function: FUN_0040ca50
    void CBitmap::Fill(const SColorRGB& color)
    {
        switch (mDepth)
        {
        case 0:
        {
            void* palette = mPalette;

            if (palette == nullptr)
            {
                palette = NXBasics::CXBSystemManager::sPalettePtr;
            }

            const std::uint8_t colorIndex = Palette_FindMatchingColor(palette, color);
            Fill(colorIndex);
            break;
        }

        case 1:
        {
            const std::uint16_t packedColor = g16BitPixelConverter->PackColor(color);
            Fill(packedColor);
            break;
        }

        case 2:
        {
            const std::uint32_t packedColor = g32BitPixelConverter->PackColor(color);
            Fill(packedColor);
            break;
        }
        }
    }

    // Original function: FUN_0040cb4c
    void CBitmap::CopyIntoBitmap(CBitmap* destination, std::int32_t destinationX, std::int32_t destinationY) const
    {
        enum class CopyMode
        {
            None,
            Byte,
            Word,
            Long,
            ByteToWord,
            ByteToLong
        };

        CopyMode mode = CopyMode::None;
        const std::uint16_t* highColorTable = nullptr;
        const std::uint32_t* trueColorTable = nullptr;

        if (mDepth == 0 && destination->mDepth == 0)
        {
            if (mRawMemory == nullptr || destination->mRawMemory == nullptr)
                return;

            mode = CopyMode::Byte;
        }
        else if (mDepth == 1 && destination->mDepth == 1)
        {
            if (mHighColorMemory == nullptr || destination->mHighColorMemory == nullptr)
                return;

            mode = CopyMode::Word;
        }
        else if (mDepth == 2 && destination->mDepth == 2)
        {
            if (mTrueColorMemory == nullptr || destination->mTrueColorMemory == nullptr)
                return;

            mode = CopyMode::Long;
        }
        else if (mDepth == 0 && destination->mDepth == 1)
        {
            highColorTable = GetHighColorTablePtr();

            if (highColorTable == nullptr)
                return;

            if (mRawMemory == nullptr || destination->mHighColorMemory == nullptr)
                return;

            mode = CopyMode::ByteToWord;
        }
        else if (mDepth == 0 && destination->mDepth == 2)
        {
            trueColorTable = GetTrueColorTablePtr();

            if (trueColorTable == nullptr)
                return;

            if (mRawMemory == nullptr || destination->mTrueColorMemory == nullptr)
                return;

            mode = CopyMode::ByteToLong;
        }

        if (mode != CopyMode::None)
        {
            const SRectangle destinationBounds(0, 0, destination->mRectangle.width, destination->mRectangle.height);
            SRectangle area(destinationX, destinationY, mRectangle.width, mRectangle.height);

            if (!area.IsTouching(destinationBounds))
                return;

            area.CutInside(destinationBounds);

            const std::uint32_t sourceX = static_cast<std::uint32_t>(area.x) - static_cast<std::uint32_t>(destinationX);
            const std::uint32_t sourceY = static_cast<std::uint32_t>(area.y) - static_cast<std::uint32_t>(destinationY);
            const std::uint32_t sourceIndex = sourceY * mRowPixelCount + sourceX;
            const std::uint32_t destinationIndex = static_cast<std::uint32_t>(area.y) * destination->mRowPixelCount + static_cast<std::uint32_t>(area.x);

            switch (mode)
            {
            case CopyMode::Byte:
                XB_Tool_Byte_CopyBlock(BitmapPixelAddress(mRawMemory, sourceIndex), BitmapPixelAddress(destination->mRawMemory, destinationIndex), static_cast<std::uint32_t>(area.width), area.height, mRowPixelCount, destination->mRowPixelCount);
                break;

            case CopyMode::Word:
                XB_Tool_Word_CopyBlock(BitmapPixelAddress(mHighColorMemory, sourceIndex), BitmapPixelAddress(destination->mHighColorMemory, destinationIndex), static_cast<std::uint32_t>(area.width), area.height, mRowPixelCount, destination->mRowPixelCount);
                break;

            case CopyMode::Long:
                XB_Tool_Long_CopyBlock(BitmapPixelAddress(mTrueColorMemory, sourceIndex), BitmapPixelAddress(destination->mTrueColorMemory, destinationIndex), static_cast<std::uint32_t>(area.width), area.height, mRowPixelCount, destination->mRowPixelCount);
                break;

            case CopyMode::ByteToWord:
                XB_Tool_ByteToWord_CopyBlock(BitmapPixelAddress(mRawMemory, sourceIndex), BitmapPixelAddress(destination->mHighColorMemory, destinationIndex), area.width, area.height, mRowPixelCount, destination->mRowPixelCount, highColorTable);
                break;

            case CopyMode::ByteToLong:
                XB_Tool_ByteToLong_CopyBlock(BitmapPixelAddress(mRawMemory, sourceIndex), BitmapPixelAddress(destination->mTrueColorMemory, destinationIndex), area.width, area.height, mRowPixelCount, destination->mRowPixelCount, trueColorTable);
                break;

            default:
                break;
            }

            return;
        }

        // These conversion paths ignore the requested destination position.
        if (mDepth == 1 && destination->mDepth == 2)
        {
            if (mRectangle == destination->mRectangle)
            {
                const auto* highColorConverter = g16BitPixelConverter;
                const auto* trueColorConverter = g32BitPixelConverter;

                for (std::int32_t y = 0; y < mRectangle.height; ++y)
                {
                    for (std::int32_t x = 0; x < mRectangle.width; ++x)
                    {
                        const std::uint16_t pixel = Draw_GetPixel_Word(x, y);

                        // The original uses adjustment fields at +0x2C/+0x30/+0x34.
                        const std::uint32_t red = ((static_cast<std::uint32_t>(pixel) & highColorConverter->mRedMask) >> (static_cast<std::uint32_t>(highColorConverter->mRedAdjustment) & 31u)) << 3;
                        const std::uint32_t green = ((static_cast<std::uint32_t>(pixel) & highColorConverter->mGreenMask) >> (static_cast<std::uint32_t>(highColorConverter->mGreenAdjustment) & 31u)) << 3;
                        const std::uint32_t blue = ((static_cast<std::uint32_t>(pixel) & highColorConverter->mBlueMask) >> (static_cast<std::uint32_t>(highColorConverter->mBlueAdjustment) & 31u)) << 3;
                        const std::uint32_t color = trueColorConverter->PackColor(red, green, blue);

                        const std::uint32_t index = static_cast<std::uint32_t>(y) * destination->mRowPixelCount + static_cast<std::uint32_t>(x);
                        std::memcpy(BitmapPixelAddress(destination->mTrueColorMemory, index), &color, sizeof(color));
                    }
                }
            }
        }

        if (mDepth == 2 && destination->mDepth == 1)
        {
            if (!(mRectangle == destination->mRectangle))
                return;

            for (std::int32_t y = 0; y < mRectangle.height; ++y)
            {
                for (std::int32_t x = 0; x < mRectangle.width; ++x)
                {
                    const std::uint32_t pixel = Draw_GetPixel_Long(x, y);
                    const std::uint32_t red = (pixel >> 16) & 0xFFu;
                    const std::uint32_t green = (pixel >> 8) & 0xFFu;
                    const std::uint32_t blue = pixel & 0xFFu;
                    const std::uint16_t color = g16BitPixelConverter->PackColor(red, green, blue);

                    const std::uint32_t index = static_cast<std::uint32_t>(y) * destination->mRowPixelCount + static_cast<std::uint32_t>(x);
                    std::memcpy(BitmapPixelAddress(destination->mHighColorMemory, index), &color, sizeof(color));
                }
            }
        }
    }

    // Original function: FUN_0040d56b
    void CBitmap::CopyIntoBitmap_FitIn(CBitmap* destination) const
    {
        if (mRawMemory == nullptr)
            return;

        if (destination->mRawMemory == nullptr)
            return;

        const std::int32_t destinationWidth = destination->mRectangle.width;
        const std::int32_t destinationHeight = destination->mRectangle.height;

        enum class ScaleMode
        {
            Byte,
            ByteToWord,
            ByteToLong,
            Word,
            Long
        };

        ScaleMode mode;
        const std::uint16_t* highColorTable = nullptr;
        const std::uint32_t* trueColorTable = nullptr;

        if (mDepth == 0 && destination->mDepth == 0)
        {
            mode = ScaleMode::Byte;
        }
        else if (mDepth == 0 && destination->mDepth == 1)
        {
            highColorTable = GetHighColorTablePtr();

            if (highColorTable == nullptr)
                return;

            mode = ScaleMode::ByteToWord;
        }
        else if (mDepth == 0 && destination->mDepth == 2)
        {
            trueColorTable = GetTrueColorTablePtr();

            if (trueColorTable == nullptr)
                return;

            mode = ScaleMode::ByteToLong;
        }
        else if (mDepth == 1 && destination->mDepth == 1)
        {
            mode = ScaleMode::Word;
        }
        else if (mDepth == 2 && destination->mDepth == 2)
        {
            mode = ScaleMode::Long;
        }
        else
        {
            return;
        }

        if (mode == ScaleMode::Byte || mode == ScaleMode::ByteToWord)
        {
            for (std::int32_t y = 0; y < destinationHeight; ++y)
            {
                const std::int32_t sourceY = ScaleBitmapCoordinate(mRectangle.height, y, destinationHeight);
                const std::uint32_t sourceRowIndex = static_cast<std::uint32_t>(sourceY) * mRowPixelCount;
                const auto* sourceRow = BitmapPixelAddress(mRawMemory, sourceRowIndex);

                const std::uint32_t destinationRowIndex = static_cast<std::uint32_t>(y) * destination->mRowPixelCount;
                std::uintptr_t destinationAddress;

                if (mode == ScaleMode::Byte)
                    destinationAddress = reinterpret_cast<std::uintptr_t>(BitmapPixelAddress(destination->mRawMemory, destinationRowIndex));
                else
                    destinationAddress = reinterpret_cast<std::uintptr_t>(BitmapPixelAddress(destination->mHighColorMemory, destinationRowIndex));

                for (std::int32_t x = 0; x < destinationWidth; ++x)
                {
                    const std::int32_t sourceX = ScaleBitmapCoordinate(mRectangle.width, x, destinationWidth);
                    const std::uint8_t index = *BitmapPixelAddress(sourceRow, static_cast<std::uint32_t>(sourceX));

                    if (mode == ScaleMode::Byte)
                    {
                        *reinterpret_cast<std::uint8_t*>(destinationAddress) = index;
                        ++destinationAddress;
                    }
                    else
                    {
                        const std::uint16_t color = highColorTable[index];
                        std::memcpy(reinterpret_cast<void*>(destinationAddress), &color, sizeof(color));
                        destinationAddress += 2u;
                    }
                }
            }

            return;
        }

        for (std::int32_t y = 0; y < destinationHeight; ++y)
        {
            for (std::int32_t x = 0; x < destinationWidth; ++x)
            {
                const std::int32_t sourceX = ScaleBitmapCoordinate(mRectangle.width, x, destinationWidth);
                const std::int32_t sourceY = ScaleBitmapCoordinate(mRectangle.height, y, destinationHeight);

                if (mode == ScaleMode::ByteToLong)
                {
                    const std::uint8_t index = Draw_GetPixel(sourceX, sourceY);
                    const std::uint32_t color = trueColorTable[index];
                    const std::uint32_t destinationIndex = static_cast<std::uint32_t>(y) * destination->mRowPixelCount + static_cast<std::uint32_t>(x);
                    std::memcpy(BitmapPixelAddress(destination->mTrueColorMemory, destinationIndex), &color, sizeof(color));
                }
                else if (mode == ScaleMode::Word)
                {
                    const std::uint16_t color = Draw_GetPixel_Word(sourceX, sourceY);
                    const std::uint32_t destinationIndex = static_cast<std::uint32_t>(y) * destination->mRowPixelCount + static_cast<std::uint32_t>(x);
                    std::memcpy(BitmapPixelAddress(destination->mHighColorMemory, destinationIndex), &color, sizeof(color));
                }
                else
                {
                    const std::uint32_t color = Draw_GetPixel_Long(sourceX, sourceY);
                    const std::uint32_t destinationIndex = static_cast<std::uint32_t>(y) * destination->mRowPixelCount + static_cast<std::uint32_t>(x);
                    std::memcpy(BitmapPixelAddress(destination->mTrueColorMemory, destinationIndex), &color, sizeof(color));
                }
            }
        }
    }

    // Original function: FUN_0040d9ee
    std::uint8_t CBitmap::Draw_GetPixel(std::int32_t x, std::int32_t y) const
    {
        const auto* pixels = mRawMemory;

        if (pixels == nullptr)
            return 0;

        // The cached bounds are compared as signed 32-bit values.
        std::int32_t right;
        std::memcpy(&right, &mRightBits, sizeof(right));

        if (x < mRectangle.x || x > right)
            return 0;

        std::int32_t bottom;
        std::memcpy(&bottom, &mBottomBits, sizeof(bottom));

        if (y < mRectangle.y || y > bottom)
            return 0;

        if (mDepth != 0)
            return 0;

        // Coordinates are used directly, without subtracting the rectangle origin.
        const std::uint32_t index = static_cast<std::uint32_t>(y) * mRowPixelCount + static_cast<std::uint32_t>(x);
        const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(pixels) + index;

        std::uint8_t result;
        std::memcpy(&result, reinterpret_cast<const void*>(address), sizeof(result));
        return result;
    }

    // Original function: FUN_0040da2b
    std::uint16_t CBitmap::Draw_GetPixel_Word(std::int32_t x, std::int32_t y) const
    {
        const auto* pixels = mHighColorMemory;

        if (pixels == nullptr)
            return 0;

        std::int32_t right;
        std::memcpy(&right, &mRightBits, sizeof(right));

        if (x < mRectangle.x || x > right)
            return 0;

        std::int32_t bottom;
        std::memcpy(&bottom, &mBottomBits, sizeof(bottom));

        if (y < mRectangle.y || y > bottom)
            return 0;

        if (mDepth != 1)
            return 0;

        // Preserve 32-bit wraparound before adding the base address.
        const std::uint32_t index = static_cast<std::uint32_t>(y) * mRowPixelCount + static_cast<std::uint32_t>(x);
        const std::uint32_t byteOffset = index * 2u;
        const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(pixels) + byteOffset;

        std::uint16_t result;
        std::memcpy(&result, reinterpret_cast<const void*>(address), sizeof(result));
        return result;
    }

    // Original function: FUN_0040da6a
    std::uint32_t CBitmap::Draw_GetPixel_Long(std::int32_t x, std::int32_t y) const
    {
        const auto* pixels = mTrueColorMemory;

        if (pixels == nullptr)
            return 0;

        std::int32_t right;
        std::memcpy(&right, &mRightBits, sizeof(right));

        if (x < mRectangle.x || x > right)
            return 0;

        std::int32_t bottom;
        std::memcpy(&bottom, &mBottomBits, sizeof(bottom));

        if (y < mRectangle.y || y > bottom)
            return 0;

        if (mDepth != 2)
            return 0;

        const std::uint32_t index = static_cast<std::uint32_t>(y) * mRowPixelCount + static_cast<std::uint32_t>(x);
        const std::uint32_t byteOffset = index * 4u;
        const std::uintptr_t address = reinterpret_cast<std::uintptr_t>(pixels) + byteOffset;

        std::uint32_t result;
        std::memcpy(&result, reinterpret_cast<const void*>(address), sizeof(result));
        return result;
    }

    // Original function: FUN_0040daa7
    void CBitmap::Draw_Box(SRectangle rectangle, std::uint8_t colorIndex)
    {
        if (mRawMemory == nullptr)
        {
            return;
        }

        rectangle.Validate();

        if (!rectangle.IsTouching(mRectangle))
        {
            return;
        }

        rectangle.CutInside(mRectangle);

        switch (mDepth)
        {
        case 0:
        {
            const std::uint32_t rowPixelCount = mRowPixelCount;
            const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);

            auto* pixels = reinterpret_cast<std::uint8_t*>(reinterpret_cast<std::uintptr_t>(mRawMemory) + pixelOffset);

            XB_Tool_Byte_FillBlock(pixels, static_cast<std::uint32_t>(rectangle.width), rectangle.height, rowPixelCount, colorIndex);
            break;
        }

        case 1:
        {
            const std::uint32_t pixelOffset = mRowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
            const std::uint32_t byteOffset = pixelOffset * 2u;

            auto* pixels = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(mHighColorMemory) + byteOffset);

            const std::uint16_t packedColor = GetHighColorWord(colorIndex);

            XB_Tool_Word_FillBlock(pixels, rectangle.width, rectangle.height, mRowPixelCount, packedColor);
            break;
        }

        case 2:
        {
            const std::uint32_t pixelOffset = mRowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
            const std::uint32_t byteOffset = pixelOffset * 4u;

            auto* pixels = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(mTrueColorMemory) + byteOffset);

            const std::uint32_t packedColor = GetTrueColorWord(colorIndex);

            XB_Tool_Long_FillBlock(pixels, rectangle.width, rectangle.height, mRowPixelCount, packedColor);
            break;
        }
        }
    }

    // Original function: FUN_0040db69
    void CBitmap::Draw_Box(SRectangle rectangle, std::uint16_t color)
    {
        rectangle.Validate();

        if (mRawMemory == nullptr || mDepth != 1)
        {
            return;
        }

        if (!rectangle.IsTouching(mRectangle))
        {
            return;
        }

        rectangle.CutInside(mRectangle);

        const std::uint32_t rowPixelCount = mRowPixelCount;
        const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
        const std::uint32_t byteOffset = pixelOffset * 2u;

        auto* pixels = reinterpret_cast<std::uint16_t*>(reinterpret_cast<std::uintptr_t>(mHighColorMemory) + byteOffset);

        XB_Tool_Word_FillBlock(pixels, rectangle.width, rectangle.height, rowPixelCount, color);
    }

    // Original function: FUN_0040dbc6
    void CBitmap::Draw_Box(SRectangle rectangle, std::uint32_t color)
    {
        rectangle.Validate();

        if (mRawMemory == nullptr || mDepth != 2)
        {
            return;
        }

        if (!rectangle.IsTouching(mRectangle))
        {
            return;
        }

        rectangle.CutInside(mRectangle);

        const std::uint32_t rowPixelCount = mRowPixelCount;
        const std::uint32_t pixelOffset = rowPixelCount * static_cast<std::uint32_t>(rectangle.y) + static_cast<std::uint32_t>(rectangle.x);
        const std::uint32_t byteOffset = pixelOffset * 4u;

        auto* pixels = reinterpret_cast<std::uint32_t*>(reinterpret_cast<std::uintptr_t>(mTrueColorMemory) + byteOffset);

        XB_Tool_Long_FillBlock(pixels, rectangle.width, rectangle.height, rowPixelCount, color);
    }

    // Original function: FUN_0040dffe
    void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint8_t colorIndex)
    {
        // Preserve the coordinate's original 32-bit representation.
        std::int32_t rectangleY;
        std::memcpy(&rectangleY, &y, sizeof(rectangleY));

        SRectangle rectangle(x, rectangleY, 1, length);

        Draw_Box(rectangle, colorIndex);
    }

    // Original function: FUN_0040e037
    void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint16_t color)
    {
        // Preserve the coordinate's original 32-bit representation.
        std::int32_t rectangleY;
        std::memcpy(&rectangleY, &y, sizeof(rectangleY));

        SRectangle rectangle(x, rectangleY, 1, length);

        Draw_Box(rectangle, color);
    }

    // Original function: FUN_0040e070
    void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, std::uint32_t color)
    {
        // Preserve the coordinate's original 32-bit representation.
        std::int32_t rectangleY;
        std::memcpy(&rectangleY, &y, sizeof(rectangleY));

        SRectangle rectangle(x, rectangleY, 1, length);

        Draw_Box(rectangle, color);
    }

    // Original function: FUN_0040e0a9
    void CBitmap::Draw_VerticalLine(std::int32_t x, std::uint32_t y, std::int32_t length, const SColorRGB& color)
    {
        switch (mDepth)
        {
        case 0:
        {
            void* palette = mPalette;

            if (palette == nullptr)
            {
                palette = NXBasics::CXBSystemManager::sPalettePtr;
            }

            const std::uint8_t colorIndex = Palette_FindMatchingColor(palette, color);
            Draw_VerticalLine(x, y, length, colorIndex);
            break;
        }

        case 1:
        {
            const std::uint16_t packedColor = g16BitPixelConverter->PackColor(color);
            Draw_VerticalLine(x, y, length, packedColor);
            break;
        }

        case 2:
        {
            const std::uint32_t packedColor = g32BitPixelConverter->PackColor(color);
            Draw_VerticalLine(x, y, length, packedColor);
            break;
        }
        }
    }

    // Original function: FUN_0040ed72
    void CBitmap::Text_Print(CFont* font, const SColorRGB* colour, std::int32_t x, std::int32_t y, const char* text)
    {
        if (text == nullptr)
            return;

        const std::uint32_t initialX = static_cast<std::uint32_t>(x);
        std::uint32_t currentX = initialX;
        std::uint32_t currentY = static_cast<std::uint32_t>(y);

        for (;;)
        {
            const std::uint8_t character = static_cast<std::uint8_t>(*text++);

            if (character == 0)
                return;

            if (character == 10 || character == 13)
            {
                // The original also folds two consecutive LF characters.
                if (static_cast<std::uint8_t>(*text) == 10)
                    ++text;

                std::uint32_t lineHeight;
                std::memcpy(&lineHeight, reinterpret_cast<const std::byte*>(font) + 8, sizeof(lineHeight));

                currentX = initialX;
                currentY += lineHeight + 2u;
                continue;
            }

            if (character == 9 || character == 32 || character == 95)
            {
                // Underscore follows the same spacing branch as space and tab.
                std::uint32_t distance = static_cast<std::uint32_t>(mFixedXDistance);

                if (distance == 0)
                    distance = static_cast<std::uint32_t>(Font_GetCharacterWidth(font, 0x69));

                currentX += distance;
                continue;
            }

            if (character < 32)
                continue;

            const std::uint32_t characterWidth = static_cast<std::uint32_t>(Font_GetCharacterWidth(font, character));
            const std::uint32_t fixedDistance = static_cast<std::uint32_t>(mFixedXDistance);

            if (fixedDistance != 0)
            {
                const std::int32_t difference = Runtime_TimeSigned32(fixedDistance - characterWidth);
                const std::uint32_t characterX = currentX + static_cast<std::uint32_t>(difference / 2);

                Font_PrintCharacter(font, colour, this, character, Runtime_TimeSigned32(characterX), Runtime_TimeSigned32(currentY));

                // Reload the field after drawing, as the original does.
                currentX += static_cast<std::uint32_t>(mFixedXDistance);
            }
            else
            {
                Font_PrintCharacter(font, colour, this, character, Runtime_TimeSigned32(currentX), Runtime_TimeSigned32(currentY));
                currentX += characterWidth;
            }
        }
    }

    // Original function: FUN_0040ee47
    void __cdecl CBitmap::Text_PrintV(CFont* font, const SColorRGB* colour, std::int32_t x, std::int32_t y, const char* format, ...)
    {
        char text[1000];

        va_list arguments;
        va_start(arguments, format);
        Runtime_FormatString(text, 999, format, arguments);
        va_end(arguments);

        text[999] = '\0';
        Text_Print(font, colour, x, y, text);
    }

    // Original function: FUN_0040ee96
    std::uint16_t CBitmap::GetHighColorWord(std::uint8_t colorIndex) const
    {
        const std::uint32_t paletteIndex = colorIndex;
        void* palette = mPalette;

        if (palette == nullptr)
        {
            palette = NXBasics::CXBSystemManager::sPalettePtr;
        }

        return Palette_GetHighColorWord(palette, paletteIndex);
    }

    // Original function: FUN_0040eeb1
    const std::uint16_t* CBitmap::GetHighColorTablePtr() const
    {
        void* palette = mPalette;

        if (palette == nullptr)
        {
            palette = NXBasics::CXBSystemManager::sPalettePtr;
        }

        return Palette_GetHighColorTablePtr(palette);
    }

    // Original function: FUN_0040eec8
    std::uint32_t CBitmap::GetTrueColorWord(std::uint8_t colorIndex) const
    {
        const std::uint32_t paletteIndex = colorIndex;
        void* palette = mPalette;

        if (palette == nullptr)
        {
            palette = NXBasics::CXBSystemManager::sPalettePtr;
        }

        return Palette_GetTrueColorWord(palette, paletteIndex);
    }

    // Original function: FUN_0040eee3
    const std::uint32_t* CBitmap::GetTrueColorTablePtr() const
    {
        void* palette = mPalette;

        if (palette == nullptr)
        {
            palette = NXBasics::CXBSystemManager::sPalettePtr;
        }

        return Palette_GetTrueColorTablePtr(palette);
    }

    // Original function: FUN_004e5e48
    void CBitmap::operator delete(void* allocation) noexcept
    {
        ::operator delete(allocation);
    }
}
