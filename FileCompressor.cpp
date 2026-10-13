#include "C2DataIds.h"
#include "FileCompressor.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NNetwork
{
    namespace
    {
        struct SCompressedFileHeader
        {
            std::uint32_t signature;       // +0x00
            std::uint32_t headerSize;      // +0x04
            std::uint32_t version;         // +0x08
            std::uint32_t originalSize;    // +0x0C
            std::uint32_t compressedSize;  // +0x10; includes the header
            std::uint8_t method;           // +0x14
            std::uint8_t marker;           // +0x15
            std::uint8_t reserved[6];      // +0x16
        };

        static_assert(sizeof(SCompressedFileHeader) == 28);
        static_assert(offsetof(SCompressedFileHeader, method) == 0x14);
        static_assert(offsetof(SCompressedFileHeader, marker) == 0x15);
    }

    // Original function: FUN_004e0b3a
    CFileCompressor::CFileCompressor() : mInput(nullptr), mOutput(nullptr), mTemporary(nullptr)
    {}

    // Original function: FUN_004e0b4e
    std::int32_t CFileCompressor::CompressFile(const char* fileName)
    {
        SCompressedFileHeader header{};

        mInput = NXBasics::Runtime_OpenStream(fileName, "rb");

        if (mInput == nullptr)
        {
            return 1;
        }

        header.signature = NC2DataIds::CompressedFileHeader::Signature;
        header.headerSize = 0x1Cu;
        header.version = 1;

        std::uint32_t frequencies[256]{};
        std::uint32_t byteCount = 0;

        for (std::int32_t value = NXBasics::Runtime_ReadStreamCharacter(mInput); value != -1; value = NXBasics::Runtime_ReadStreamCharacter(mInput))
        {
            ++frequencies[value];
            ++byteCount;
        }

        header.originalSize = byteCount;

        std::uint32_t marker = 0;

        for (std::uint32_t value = 0; value < 256; ++value)
        {
            if (frequencies[value] == 0)
            {
                marker = value;
                break;
            }

            if (frequencies[value] < frequencies[marker])
            {
                marker = value;
            }
        }

        NXBasics::Runtime_SeekStream(mInput, 0, 0);
        header.marker = static_cast<std::uint8_t>(marker);

        mTemporary = NXBasics::Runtime_CreateTemporaryStream();

        if (mTemporary == nullptr)
        {
            NXBasics::Runtime_CloseStream(mInput);
            return 1;
        }

        std::int32_t result = CompressRle(mInput, mTemporary, header.marker);

        if (result != 0)
        {
            NXBasics::Runtime_CloseStream(mInput);
            NXBasics::Runtime_CloseStream(mTemporary);
            return result;
        }

        header.method = 1;

        NXBasics::Runtime_SeekStream(mInput, 0, 2);
        NXBasics::Runtime_SeekStream(mTemporary, 0, 2);

        header.originalSize = static_cast<std::uint32_t>(NXBasics::Runtime_TellStream(mInput));
        header.compressedSize = static_cast<std::uint32_t>(NXBasics::Runtime_TellStream(mTemporary)) + 0x1Cu;

        if (header.originalSize < header.compressedSize)
        {
            NXBasics::Runtime_CloseStream(mInput);
            NXBasics::Runtime_CloseStream(mTemporary);
            return 5;
        }

        char outputFileName[260]{};

        std::strcpy(outputFileName, fileName);
        std::strcat(outputFileName, ".c2p");

        mOutput = NXBasics::Runtime_OpenStream(outputFileName, "wb+");

        if (mOutput == nullptr)
        {
            result = 1;
        }
        else
        {
            if (NXBasics::Runtime_WriteStream(&header, 0x1Cu, 1u, mOutput) != 1u)
            {
                result = 2;
            }
            else
            {
                NXBasics::Runtime_SeekStream(mTemporary, 0, 0);

                for (std::int32_t value = NXBasics::Runtime_ReadStreamCharacter(mTemporary); value != -1; value = NXBasics::Runtime_ReadStreamCharacter(mTemporary))
                {
                    NXBasics::Runtime_WriteStreamCharacter(value, mOutput);
                }
            }

            NXBasics::Runtime_CloseStream(mOutput);
        }

        NXBasics::Runtime_CloseStream(mInput);
        NXBasics::Runtime_CloseStream(mTemporary);

        return result;
    }

    // Original function: FUN_004e0d5c
    std::int32_t CFileCompressor::DecompressFile(const char* fileName)
    {
        SCompressedFileHeader header{};

        mInput = NXBasics::Runtime_OpenStream(fileName, "rb");

        if (mInput == nullptr)
        {
            return 1;
        }

        // Preserve the original unchecked header read.
        NXBasics::Runtime_ReadStream(&header, 0x1Cu, 1u, mInput);

        if (header.signature != NC2DataIds::CompressedFileHeader::Signature)
        {
            NXBasics::Runtime_CloseStream(mInput);
            return 3;
        }

        char outputFileName[260]{};

        std::strncpy(outputFileName, fileName, sizeof(outputFileName));

        // The original removes four characters without checking the suffix or length.
        const auto fileNameLength = static_cast<std::ptrdiff_t>(std::strlen(fileName));
        outputFileName[fileNameLength - 4] = '\0';

        mOutput = NXBasics::Runtime_OpenStream(outputFileName, "wb+");

        if (mOutput == nullptr)
        {
            NXBasics::Runtime_CloseStream(mInput);
            return 1;
        }

        const std::int32_t decodeResult = DecompressRle(mInput, mOutput, header.marker);
        const std::int32_t result = decodeResult != 0 ? 4 : 0;

        NXBasics::Runtime_CloseStream(mInput);
        NXBasics::Runtime_CloseStream(mOutput);

        // The original removes the input even when decodeResult indicates failure.
        NXBasics::Runtime_RemoveFile(fileName);

        return result;
    }

    // Original function: FUN_004e0e57
    std::int32_t CFileCompressor::CompressRle(NXBasics::RuntimeFormatStream* input, NXBasics::RuntimeFormatStream* output, std::uint8_t marker)
    {
        NXBasics::Runtime_SeekStream(input, 0, 0);
        NXBasics::Runtime_SeekStream(output, 0, 0);

        std::int32_t previous = -1;
        std::int32_t repetitions = 0;

        for (std::int32_t current = NXBasics::Runtime_ReadStreamCharacter(input); current != -1; current = NXBasics::Runtime_ReadStreamCharacter(input))
        {
            if (current == previous && repetitions < 254)
            {
                ++repetitions;
                continue;
            }

            if (previous == marker)
            {
                for (std::int32_t index = 0; index < repetitions + 1; ++index)
                {
                    NXBasics::Runtime_WriteStreamCharacter(marker, output);
                    NXBasics::Runtime_WriteStreamCharacter(marker, output);
                    NXBasics::Runtime_WriteStreamCharacter(1, output);
                }
            }
            else if (repetitions > 2)
            {
                NXBasics::Runtime_WriteStreamCharacter(marker, output);
                NXBasics::Runtime_WriteStreamCharacter(previous, output);
                NXBasics::Runtime_WriteStreamCharacter(repetitions + 1, output);
            }
            else if (previous != -1)
            {
                for (std::int32_t index = 0; index < repetitions + 1; ++index)
                {
                    NXBasics::Runtime_WriteStreamCharacter(previous, output);
                }
            }

            previous = current;
            repetitions = 0;
        }

        // Preserve the original missing marker escaping in the final short run.
        if (repetitions > 2)
        {
            NXBasics::Runtime_WriteStreamCharacter(marker, output);
            NXBasics::Runtime_WriteStreamCharacter(previous, output);
            NXBasics::Runtime_WriteStreamCharacter(repetitions + 1, output);
        }
        else if (previous != -1)
        {
            for (std::int32_t index = 0; index < repetitions + 1; ++index)
            {
                NXBasics::Runtime_WriteStreamCharacter(previous, output);
            }
        }

        NXBasics::Runtime_SeekStream(input, 0, 0);
        NXBasics::Runtime_SeekStream(output, 0, 0);

        return 0;
    }

    // Original function: FUN_004e0f99
    std::int32_t CFileCompressor::DecompressRle(NXBasics::RuntimeFormatStream* input, NXBasics::RuntimeFormatStream* output, std::uint8_t marker)
    {
        for (std::int32_t current = NXBasics::Runtime_ReadStreamCharacter(input); current != -1; current = NXBasics::Runtime_ReadStreamCharacter(input))
        {
            if (current == marker)
            {
                const std::int32_t value = NXBasics::Runtime_ReadStreamCharacter(input);
                const auto count = static_cast<std::uint8_t>(NXBasics::Runtime_ReadStreamCharacter(input));

                for (std::uint32_t index = 0; index < count; ++index)
                {
                    NXBasics::Runtime_WriteStreamCharacter(value, output);
                }
            }
            else
            {
                NXBasics::Runtime_WriteStreamCharacter(current, output);
            }
        }

        return 0;
    }
}