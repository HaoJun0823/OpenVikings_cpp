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
    bool WorldDisplayElement_MouseSelectingArea_IsAreaValid(const void* element);
}