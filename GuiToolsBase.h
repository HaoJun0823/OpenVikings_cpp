#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
    struct SColorRGB;
}

namespace NC2GuiToolsBase
{
    void DrawTool_DrawFrame(void* context, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle, std::int32_t mode);
    void DrawTool_DrawFrame(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode);
    void DrawTool_PrintStringX(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y, const void* font, const NXBasics::SColorRGB* color, const char* text);
    void FXTool_Chat();
}