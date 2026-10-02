#pragma once

namespace NXBaseGui
{
    void BaseWindow_Element_Add(void* window, void* element);
    void BaseWindow_NotifyAddedToDesktop(void* window);
    void BaseWindow_InternalHide(void* window);
    void BaseElement_InternalHide(void* element);
}
