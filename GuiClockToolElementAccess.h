#pragma once

#include "XBasicTypes.h"

namespace NC2InGameGuiBase
{
    extern const std::uintptr_t gBaseToolElementVirtualTable[18];
    void* __fastcall BaseToolElement_Delete(void* element, void* unusedEdx, std::uint32_t flags);
    void* ToolClockElement_Construct(void* element, const NXBasics::SRectangle* rectangle);
}
