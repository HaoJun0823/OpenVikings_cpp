#pragma once
#include "XBasicTypes.h"

namespace NC2InGameGuiBase
{
    void* __fastcall BaseToolTextElement_Construct(void* element, void* unusedEdx, const NXBasics::SRectangle* rectangle, const char* caption, bool fillBackground, bool wrapText);
    void* __fastcall BaseToolTextButton_Construct(void* element, void* unusedEdx, const NXBasics::SRectangle* rectangle, const char* caption, std::uint32_t command, void* handler);
    void* __fastcall BaseToolTextButton_ConstructWithToolTip(void* element, void* unusedEdx, const NXBasics::SRectangle* rectangle, const char* caption, const char* tooltip, std::uint32_t command, void* handler);
    void* __fastcall BaseToolTextButton_ConstructToggle(void* element, void* unusedEdx, const NXBasics::SRectangle* rectangle, const char* caption, std::uint32_t command, void* handler, bool selected);
    void* __fastcall BaseToolTextButton_ConstructToggleWithToolTip(void* element, void* unusedEdx, const NXBasics::SRectangle* rectangle, const char* caption, const char* tooltip, std::uint32_t command, void* handler, bool selected);
}
