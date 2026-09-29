#pragma once

namespace NXBasics
{
    class CPalette;

    // Original function: FUN_0040bebd
    CPalette* Picture_ExtractPalette(void* picture);

    // Original function: FUN_0040c5a6
    CPalette* XB_PictureTool_LoadPaletteOutOfPicture(const char* fileName);
}