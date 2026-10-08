#include "XGuiDesktop.h"
#include "XGuiWindowAccess.h"
#include "C2LifetimeAccess.h"
#include "XList.h"

#include <new>

namespace NXBaseGui
{
    using namespace NC2Lifetime;

    // Original function: FUN_00477163.
    void CDesktop::Element_Add(CBaseElement* element, bool insertAtEnd)
    {
        const auto flags = Read<std::uint32_t>(element, 0x18);
        if ((flags & 0x800u) != 0) return;
        Write(element, 0x18, flags | 0x800u);
        std::launder(reinterpret_cast<NXBasics::SRectangle*>(reinterpret_cast<std::byte*>(element) + 4))->PlaceInside(mRectangle);
        auto* bytes = reinterpret_cast<std::byte*>(this);
        auto* drawList = std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x14));
        auto* inputList = std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x28));
        if (insertAtEnd)
        {
            drawList->InsertAtEnd(element);
            inputList->InsertAtStart(element);
        }
        else
        {
            drawList->InsertAtStart(element);
            inputList->InsertAtEnd(element);
        }
        using TAdded = void(__thiscall*)(void*);
        Read<TAdded*>(element, 0)[12](element);
    }

    // Original function: FUN_004771be; overlay draw/input lists are distinct from normal elements.
    void CDesktop::Element_AddOverlay(CBaseElement* element, bool insertAtEnd)
    {
        const auto flags = Read<std::uint32_t>(element, 0x18);
        if ((flags & 0x800u) != 0) return;
        Write(element, 0x18, flags | 0x800u);
        std::launder(reinterpret_cast<NXBasics::SRectangle*>(reinterpret_cast<std::byte*>(element) + 4))->PlaceInside(mRectangle);
        auto* bytes = reinterpret_cast<std::byte*>(this);
        auto* drawList = std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x3C));
        auto* inputList = std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x50));
        if (insertAtEnd) { drawList->InsertAtEnd(element); inputList->InsertAtStart(element); }
        else { drawList->InsertAtStart(element); inputList->InsertAtEnd(element); }
        Read<void(__thiscall**)(void*)>(element, 0)[12](element);
    }
    // Original function: FUN_00477219.
    void CDesktop::Window_Add(CBaseWindow* window)
    {
        if (window == nullptr) return;
        const auto flags = Read<std::uint32_t>(window, 0x18);
        if ((flags & 0x800u) != 0) return;
        Write<std::uint8_t>(window, 0x68, static_cast<std::uint8_t>(Read<std::uint8_t>(window, 0x68) | 1u));
        Write(window, 0x18, flags | 0x800u);
        std::launder(reinterpret_cast<NXBasics::SRectangle*>(reinterpret_cast<std::byte*>(window) + 4))->PlaceInside(mRectangle);
        auto* bytes = reinterpret_cast<std::byte*>(this);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x64))->InsertAtEnd(window);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x78))->InsertAtStart(window);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0x8C))->InsertAtEnd(window);
        std::launder(reinterpret_cast<NXBasics::CListBase*>(bytes + 0xA0))->InsertAtStart(window);
        BaseWindow_NotifyAddedToDesktop(window);
        using TAdded = void(__thiscall*)(void*);
        Read<TAdded*>(window, 0)[12](window);
        Element_Activate(reinterpret_cast<CBaseElement*>(window));
    }

    // Original function: FUN_004773f6.
    std::uint32_t CDesktop::Window_Hide(CBaseWindow* window)
    {
        using TCanBeHidden = bool(__thiscall*)(void*);
        if (window == nullptr || !Read<TCanBeHidden*>(window, 0)[15](window)) return 0;
        if (static_cast<void*>(Window_GetActivePtr()) == window)
        {
            using TDeactivate = void(__thiscall*)(void*);
            if (mActiveElement != nullptr)
            {
                auto* active = mActiveElement;
                Read<TDeactivate*>(active, 0)[5](active);
                mActiveElement = nullptr;
            }
            Read<TDeactivate*>(window, 0)[5](window);
        }
        BaseWindow_InternalHide(window);
        return Read<std::uint32_t>(window, 0x28);
    }

    // Original function: FUN_00477532.
    std::uint32_t CDesktop::Element_Hide(CBaseElement* element)
    {
        using TCanBeHidden = bool(__thiscall*)(void*);
        if (element == nullptr || !Read<TCanBeHidden*>(element, 0)[15](element)) return 0;
        Element_Deactivate(element);
        BaseElement_InternalHide(element);
        return Read<std::uint32_t>(element, 0x28);
    }

    // Original function: FUN_00477564.
    void CDesktop::Element_UnHide(CBaseElement* element) { if (element != nullptr) BaseElement_InternalUnHide(element); }

    // Original function: FUN_00477943; the original climbs at most one parent.
    CBaseElement* CDesktop::Window_GetActivePtr() const
    {
        auto* active = mActiveElement;
        if (active == nullptr || (Read<std::uint32_t>(active, 0x18) & 0x40u) != 0) return active;
        return Read<CBaseElement*>(active, 0x20);
    }
}
