#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2InGameGuiWorldElement
{
    void* WorldDisplayElement_Construct(void* element, const NXBasics::SRectangle* rectangle, bool active, bool inputEnabled);
    void WorldDisplayElement_Destruct(void* element);
    void WorldDisplayElement_FollowMode_End(void* element);
    void WorldDisplayElement_DarkenBitmap_Exit(void* element);
    void WorldDisplayElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle);
    void WorldDisplayElement_ValidateObjectUnderMouse(void* element);
    void WorldDisplayElement_DarkenBitmap_Do(const void* element, NXBasics::CBitmap* bitmap);
    bool WorldDisplayElement_MouseSelectingArea_IsAreaValid(const void* element);
}
