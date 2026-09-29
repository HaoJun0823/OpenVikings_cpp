#include "XPictureAccess.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"

#include <cstddef>
#include <cstring>

namespace NXBasics
{
    // Original function: FUN_0040bebd
    CPalette* Picture_ExtractPalette(void* picture)
    {
        auto* pictureBytes = static_cast<std::byte*>(picture);

        CBitmap* bitmap;
        std::memcpy(&bitmap, pictureBytes, sizeof(bitmap));

        if (bitmap != nullptr)
            bitmap->SetPalettePtr(nullptr);

        CPalette* palette;
        std::memcpy(&palette, pictureBytes + 0x04, sizeof(palette));

        CPalette* emptyPalette = nullptr;
        std::memcpy(pictureBytes + 0x04, &emptyPalette, sizeof(emptyPalette));

        return palette;
    }

    // Original function: FUN_0040c5a6
    CPalette* XB_PictureTool_LoadPaletteOutOfPicture(const char* fileName)
    {
        void* picture = Runtime_AllocateWithNewHandler(0x0C, 1);

        if (picture != nullptr)
            picture = FUN_0040bd76(picture, fileName);

        CPalette* palette = Picture_ExtractPalette(picture);

        if (picture != nullptr)
        {
            FUN_0040be7d(picture);
            Runtime_Free(picture);
        }

        return palette;
    }
}