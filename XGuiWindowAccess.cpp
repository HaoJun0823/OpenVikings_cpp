#include "XGuiWindowAccess.h"
#include "C2LifetimeAccess.h"
#include "XGuiDesktop.h"
#include "XList.h"

#include <new>

namespace NXBaseGui
{
    using namespace NC2Lifetime;

    // Original function: FUN_004785b1; CBaseWindow::BW_Element_Add.
    void BaseWindow_Element_Add(void* window, void* element)
    {
        const auto flags = Read<std::uint32_t>(element, 0x18);
        if ((flags & 0x800u) != 0) return;
        Write(element, 0x18, flags | 0x800u);
        Write(element, 0x20, window);
        auto* bytes = static_cast<std::byte*>(window);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x3C))->InsertAtEnd(element);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x50))->InsertAtStart(element);
    }

    // Original function: FUN_00478687; CBaseWindow::BW_Internal_Hide.
    void BaseWindow_InternalHide(void* window)
    {
        if ((Read<std::uint32_t>(window, 0x18) & 0x200u) == 0) BaseElement_InternalHide(window);
    }

    // Original function: FUN_00478874; CBaseWindow::XGui_BE_Internal_Element_AddedToDesktop.
    void BaseWindow_NotifyAddedToDesktop(void* window)
    {
        auto* list = std::launder(reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(window) + 0x3C));
        NXBasics::CListBase::Iterator iterator(*list);
        using TAdded = void(__thiscall*)(void*);
        for (void* element = iterator.First(); element != nullptr; element = iterator.Next()) Read<TAdded*>(element, 0)[12](element);
    }

    // Original function: FUN_00478aa3; CBaseElement::BE_Internal_Element_Hide.
    void BaseElement_InternalHide(void* element)
    {
        if ((Read<std::uint32_t>(element, 0x18) & 0x200u) != 0) return;
        CDesktop::sTheObjectPtr->Element_Deactivate(static_cast<CBaseElement*>(element));
        Write<std::uint8_t>(element, 0x19, static_cast<std::uint8_t>(Read<std::uint8_t>(element, 0x19) | 2u));
    }
}
