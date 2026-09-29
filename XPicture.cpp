#include "XPicture.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XDiagnostics.h"
#include "XMemory.h"
#include "XPalette.h"

#include <cstring>
#include <cstddef>
#include <new>

namespace NXBasics
{
    // Original format table: 12-byte records, format value at +0x08.
    // The original table contents have not been supplied yet.
    extern const std::byte DAT_004f3668[];

    // Original function: FUN_0040bd76
    CPicture::CPicture(const char* fileName) : mBitmap(nullptr), mPalette(nullptr), mOwnsResources(0), mReserved09{}
    {
        char extension[0x100];
        Runtime_SplitPath(fileName, nullptr, nullptr, nullptr, extension);

        for (char* character = extension; *character != '\0'; ++character)
        {
            const auto signedCharacter = static_cast<std::int8_t>(*character);
            *character = static_cast<char>(Runtime_ToLower(static_cast<std::int32_t>(signedCharacter)));
        }

        CMemory* memory = new (std::nothrow) CMemory(fileName);

        std::uint32_t format = 2;

        for (const std::byte* entry = DAT_004f3668; ; entry += 0x0C)
        {
            std::uint32_t entryFormat;
            std::memcpy(&entryFormat, entry + 0x08, sizeof(entryFormat));

            if (entryFormat == 2)
            {
                break;
            }

            if (std::strncmp(extension, reinterpret_cast<const char*>(entry), 3) == 0)
            {
                format = entryFormat;
            }
        }

        switch (format)
        {
            case 0:
                l_UnPackPCX(memory);
                break;

            case 1:
                l_UnPackBMP(memory);
                break;

            case 2:
                ReportFatalError("NXBasics::CPicture::CPicture\n\nunknown GFX format: %s", extension);
                break;
        }

        delete memory;

        mBitmap->SetPalettePtr(mPalette);
        mOwnsResources = 1;
    }

    // Original function: FUN_0040be7d
    CPicture::~CPicture()
    {
        if (mOwnsResources != 0)
        {
            if (mBitmap != nullptr)
            {
                delete mBitmap;
                mBitmap = nullptr;
            }

            if (mPalette != nullptr)
            {
                delete mPalette;
                mPalette = nullptr;
            }
        }
    }

    // Original function: FUN_0040bebd
    CPalette* CPicture::ExtractPalette()
    {
        if (mBitmap != nullptr)
        {
            mBitmap->SetPalettePtr(nullptr);
        }

        CPalette* palette = mPalette;
        mPalette = nullptr;
        return palette;
    }

    // Interpret the low 16 bits exactly as an x86 signed word.
    static std::int32_t Picture_SignedWord(std::uint32_t value)
    {
        value &= 0xFFFFu;
        return value < 0x8000u ? static_cast<std::int32_t>(value) : static_cast<std::int32_t>(value) - 0x10000;
    }

    // Original function: FUN_0040bed6
    void CPicture::l_UnPackPCX(CMemory* memory)
    {
        const std::uint8_t* source = memory->mData;

        const auto readWord = [source] (std::uint32_t offset) -> std::uint32_t
        {
            return static_cast<std::uint32_t>(source[offset]) | (static_cast<std::uint32_t>(source[offset + 1u]) << 8u);
        };

        const std::uint32_t widthBits = ((readWord(0x08) - readWord(0x04)) & 0xFFFFu) + 1u;
        const std::uint32_t heightBits = ((readWord(0x0A) - readWord(0x06)) & 0xFFFFu) + 1u;
        const std::uint32_t paddedWidthBits = (widthBits + 1u) & ~1u;

        const std::int32_t width = Picture_SignedWord(widthBits);
        const std::int32_t height = Picture_SignedWord(heightBits);
        const std::int32_t paddedWidth = Picture_SignedWord(paddedWidthBits);
        const bool hasPadding = (widthBits & 1u) != 0;

        mBitmap = new (std::nothrow) CBitmap(width, height, 0);

        CBitmap* paddedBitmap = nullptr;

        if (hasPadding)
        {
            paddedBitmap = new (std::nothrow) CBitmap(paddedWidth, height, 0);
        }

        CBitmap* decodeBitmap = hasPadding ? paddedBitmap : mBitmap;
        std::uint8_t* row = decodeBitmap->mRawMemory;
        std::uint32_t sourceOffset = 0x80;

        for (std::int32_t y = 0; y < height; ++y)
        {
            std::uint8_t* output = row;
            std::uint32_t decodedCount = 0;

            do
            {
                const std::uint8_t token = source[sourceOffset++];

                if (token > 0xBF)
                {
                    const std::uint32_t runLength = token & 0x3Fu;
                    const std::uint8_t value = source[sourceOffset++];

                    std::memset(output, value, runLength);
                    output += runLength;
                    decodedCount += runLength;
                }
                else
                {
                    *output++ = token;
                    ++decodedCount;
                }
            } while (Picture_SignedWord(decodedCount) < paddedWidth);

            row += paddedWidth;
        }

        if (hasPadding)
        {
            paddedBitmap->CopyIntoBitmap(mBitmap, 0, 0);
            delete paddedBitmap;
        }

        mPalette = new (std::nothrow) CPalette();

        const std::uint8_t* paletteSource = source + (memory->mSize - 0x300u);

        for (std::uint32_t index = 0; index < 256; ++index)
        {
            mPalette->mColors[index][2] = *paletteSource++;
            mPalette->mColors[index][1] = *paletteSource++;
            mPalette->mColors[index][0] = *paletteSource++;
        }
    }

    // Original function: FUN_0040c054
    void CPicture::l_UnPackBMP(CMemory* memory)
    {
        const std::uint8_t* source = memory->mData;

        // Preserve the original 54-byte header layout for the decoder calls.
        alignas(4) std::byte header[0x36]{};

        const auto readWord = [source] (std::uint32_t offset) -> std::uint32_t
        {
            return static_cast<std::uint32_t>(source[offset]) | (static_cast<std::uint32_t>(source[offset + 1u]) << 8u);
        };

        const auto readDword = [&readWord] (std::uint32_t offset) -> std::uint32_t
        {
            return readWord(offset) | (readWord(offset + 2u) << 16u);
        };

        const auto storeDword = [&header] (std::uint32_t offset, std::uint32_t value)
        {
            std::memcpy(header + offset, &value, sizeof(value));
        };

        const std::uint32_t fileSize = readDword(0x02);
        const std::uint32_t pixelOffset = readDword(0x0A);
        const std::uint32_t widthBits = readDword(0x12);
        const std::uint32_t heightBits = readDword(0x16);
        const std::uint16_t bitsPerPixel = static_cast<std::uint16_t>(readWord(0x1C));
        const std::uint32_t compression = readDword(0x1E);
        const std::uint32_t colorsUsed = readDword(0x2E);

        storeDword(0x02, fileSize);
        storeDword(0x0A, pixelOffset);
        storeDword(0x12, widthBits);
        storeDword(0x16, heightBits);
        std::memcpy(header + 0x1C, &bitsPerPixel, sizeof(bitsPerPixel));
        storeDword(0x1E, compression);
        storeDword(0x2E, colorsUsed);

        std::uint8_t* pixelData = memory->mData + pixelOffset;
        const std::uint32_t paletteEntryCount = colorsUsed != 0 ? colorsUsed : 256u;

        if (bitsPerPixel == 8)
        {
            mPalette = new (std::nothrow) CPalette();

            const std::uint8_t* paletteSource = source + 0x36;
            const std::int32_t signedEntryCount = Runtime_TimeSigned32(paletteEntryCount);

            for (std::int32_t index = 0; index < signedEntryCount; ++index)
            {
                mPalette->mColors[index][0] = *paletteSource++;
                mPalette->mColors[index][1] = *paletteSource++;
                mPalette->mColors[index][2] = *paletteSource++;
                ++paletteSource;
            }
        }

        if (compression == 1)
        {
            mBitmap = static_cast<CBitmap*>(FUN_0040c4c1(this, header, pixelData));
            return;
        }

        if (compression == 0)
        {
            switch (bitsPerPixel)
            {
                case 8:
                    mBitmap = static_cast<CBitmap*>(FUN_0040c1b8(this, header, pixelData));
                    return;

                case 16:
                    mBitmap = static_cast<CBitmap*>(FUN_0040c247(this, header, pixelData));
                    return;

                case 24:
                    mBitmap = static_cast<CBitmap*>(FUN_0040c310(this, header, pixelData));
                    return;

                case 32:
                    mBitmap = static_cast<CBitmap*>(FUN_0040c3fc(this, header, pixelData));
                    return;
            }
        }

        ReportFatalError("NXBasics::CPicture::l_UnPackBMP\n\nThe bitmap to load was compressed");
    }
}