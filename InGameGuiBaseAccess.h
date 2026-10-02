#pragma once

#include "XBasicTypes.h"

namespace NXBaseGui { class CBaseElement; class CBaseWindow; }

namespace NC2InGameGuiBase
{
    extern const std::uintptr_t gBaseToolWindowVirtualTable[19];
    void* BaseToolWindow_Construct(void* window, const NXBasics::SRectangle* rectangle);
    void BaseToolDesktop_AddElement(NXBaseGui::CBaseElement* element);
    void BaseToolDesktop_AddWindow(NXBaseGui::CBaseWindow* window, bool showWhileHidden);
}
