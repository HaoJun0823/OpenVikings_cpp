#include "XBitmapFileBMP.h"
#include "XBitmap.h"
#include "XFile.h"
#include "XCrtFileAccess.h"
#include "C2LifetimeAccess.h"

#include <cstring>
#include <new>

namespace NXBasics
{
    // Original function: FUN_004984eb.
    CBitmapFileBMP::CBitmapFileBMP() : mFile(nullptr), mRowsWritten(0), mWidth(0), mHeight(0) {}

    // Original function: FUN_004984fc; bottom-up bands stop at the declared image height.
    void CBitmapFileBMP::WriteBitmap(const CBitmap* bitmap)
    {
        using namespace NC2Lifetime;
        if (mFile == nullptr || Read<std::uint32_t>(bitmap, 4) != 2 || Read<std::int32_t>(bitmap, 0x10) != mWidth) return;
        const auto rowBytes = Runtime_TimeSigned32((static_cast<std::uint32_t>(mWidth) * 24u + 31u) & ~31u) / 8;
        auto* row = static_cast<std::uint8_t*>(Runtime_Allocate(static_cast<std::uint32_t>(rowBytes)));
        std::memset(row, 0, rowBytes);
        for (std::int32_t y = Read<std::int32_t>(bitmap, 0x14) - 1; y >= 0 && mRowsWritten < mHeight; --y)
        {
            // 0040F522 resolves this pointer from +2C and the pixel stride at +30.
            const auto* pixels = Read<const std::uint8_t*>(bitmap, 0x2C) + static_cast<std::uint32_t>(y) * Read<std::uint32_t>(bitmap, 0x30) * 4u;
            for (std::int32_t x = 0; x < mWidth; ++x) std::memcpy(row + x * 3, pixels + x * 4, 3);
            mFile->Write(row, rowBytes);
            ++mRowsWritten;
        }
        if (row != nullptr) Runtime_Free(row);
    }

    // Original function: FUN_004985c1; explicit little-endian packed 14-byte file header.
    void CBitmapFileBMP::Open(std::int32_t width, std::int32_t height, const char* filename)
    {
        using namespace NC2Lifetime;
        mRowsWritten = 0; mWidth = width; mHeight = height;
        std::byte info[40]{};
        Write<std::uint32_t>(info, 0, 40); Write(info, 4, width); Write(info, 8, height);
        Write<std::uint16_t>(info, 12, 1); Write<std::uint16_t>(info, 14, 24);
        const auto rowBytes = Runtime_TimeSigned32((static_cast<std::uint32_t>(width) * 24u + 31u) & ~31u) / 8;
        const auto imageBytes = static_cast<std::uint32_t>(rowBytes) * static_cast<std::uint32_t>(height);
        Write(info, 20, imageBytes);
        std::byte header[14]{};
        Write<std::uint16_t>(header, 0, 0x4D42); Write(header, 2, imageBytes + 54u); Write<std::uint32_t>(header, 10, 54);
        if (mFile != nullptr) { delete mFile; mFile = nullptr; }
        void* memory = Runtime_Allocate(sizeof(CFile));
        mFile = memory == nullptr ? nullptr : new (memory) CFile(filename, false);
        mFile->OpenForWriting(true);
        mFile->Write(header, sizeof(header)); mFile->Write(info, sizeof(info));
    }

    // Original function: FUN_00498697; repeated closes are explicitly supported.
    void CBitmapFileBMP::Close()
    {
        if (mFile != nullptr) { delete mFile; mFile = nullptr; }
    }
}
