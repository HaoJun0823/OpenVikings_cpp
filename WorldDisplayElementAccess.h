#pragma once

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2InGameGuiWorldElement
{
    void WorldDisplayElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle);
    void WorldDisplayElement_ValidateObjectUnderMouse(void* element);
    void WorldDisplayElement_DarkenBitmap_Do(const void* element, NXBasics::CBitmap* bitmap);
    bool WorldDisplayElement_MouseSelectingArea_IsAreaValid(const void* element);
}
