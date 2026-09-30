#pragma once

#include <cstdint>

namespace NXBasics
{
    class CBitmap;
    struct SRectangle;
}

namespace NC2InGameGuiBase
{
    void* BaseBitmapToolElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId, std::int32_t depth);
    void BaseBitmapToolElement_Destruct(void* element);
    void BaseBitmapToolElement_Draw(const void* element, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
}
