#pragma once

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2InGameGuiWorldElement
{
    void WorldDisplayElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle);
}