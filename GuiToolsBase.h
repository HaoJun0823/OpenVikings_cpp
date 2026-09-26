#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2GuiToolsBase
{
    void DrawTool_DrawFrame(void* context, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle, std::int32_t mode);
    void DrawTool_DrawFrame(NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle, std::int32_t mode);
    void FXTool_Chat();
}