#pragma once

#include <cstdint>

namespace NXBasics
{
    struct RuntimeFormatStream;
}

namespace NNetwork
{
    // Descriptive reconstruction name; the original class name is not established.
    class CFileCompressor
    {
    public:
        // Original function: FUN_004e0b3a
        CFileCompressor();

        // Original function: FUN_004e0b4e
        std::int32_t CompressFile(const char* fileName);

        // Original function: FUN_004e0d5c
        std::int32_t DecompressFile(const char* fileName);

    private:
        // Original function: FUN_004e0e57; the original does not use ECX.
        static std::int32_t CompressRle(NXBasics::RuntimeFormatStream* input, NXBasics::RuntimeFormatStream* output, std::uint8_t marker);

        // Original function: FUN_004e0f99; the original does not use ECX.
        static std::int32_t DecompressRle(NXBasics::RuntimeFormatStream* input, NXBasics::RuntimeFormatStream* output, std::uint8_t marker);

        NXBasics::RuntimeFormatStream* mInput;       // +0x00
        NXBasics::RuntimeFormatStream* mOutput;      // +0x04
        NXBasics::RuntimeFormatStream* mTemporary;   // +0x08
    };

    static_assert(sizeof(void*) == 4, "CFileCompressor requires an x86 build");
    static_assert(sizeof(CFileCompressor) == 12, "CFileCompressor must be 12 bytes");
}