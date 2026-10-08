#include "XGuiWindowAccess.h"
#include "C2LifetimeAccess.h"
#include "XGuiDesktop.h"
#include "XGuiLists.h"
#include "XBitmap.h"

#include <new>

namespace NXBaseGui
{
    using namespace NC2Lifetime;

    using TElementList = NXBasics::CList<CBaseElement>;
    static TElementList* WindowList(void* window, std::uint32_t offset) { return std::launder(reinterpret_cast<TElementList*>(static_cast<std::byte*>(window) + offset)); }
    static void* __fastcall BaseWindow_Delete(void* window, void*, std::uint32_t flags);
    // Complete Windows table 004F43A8..004F43F3.
    const std::uintptr_t gBaseWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&BaseWindow_Delete),
        reinterpret_cast<std::uintptr_t>(&BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Draw),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_Activate),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_DeActivate),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&BaseWindow_DrawOverlays)
    };

    // Original function: FUN_0047846e; construction precedes the original reset.
    static void BaseWindow_ConstructVars(void* window)
    {
        new (static_cast<std::byte*>(window) + 0x3C) TElementList(false);
        new (static_cast<std::byte*>(window) + 0x50) TElementList(false);
    }

    // Original function: FUN_004784c1; Windows resets the entire 0x40-byte region,
    // including the previously constructed list headers, before setting the area.
    static void BaseWindow_ConstructState(void* window, const NXBasics::SRectangle* rectangle)
    {
        std::memset(static_cast<std::byte*>(window) + 0x2C, 0, 0x40);
        const NXBasics::SRectangle area(0, 0, rectangle->width, rectangle->height);
        Write(window, 0x2C, area);
    }

    // Original function: FUN_0047841b.
    void* __fastcall BaseWindow_Construct(void* window, void*, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId)
    {
        BaseElement_Construct(window, rectangle, flags | 0x43u, elementId);
        BaseWindow_ConstructVars(window);
        Write(window, 0, gBaseWindowVirtualTable + 0);
        BaseWindow_ConstructState(window, rectangle);
        return window;
    }

    // Original function: FUN_00478452.
    static void* __fastcall BaseWindow_Delete(void* window, void*, std::uint32_t flags)
    {
        BaseWindow_Destruct(window);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
        return window;
    }

    // Original function: FUN_004784eb.
    static void BaseWindow_DestructState(void* window)
    {
        BaseWindow_Element_DeleteAll(window);
        CDesktop::sTheObjectPtr->Element_Remove(static_cast<CBaseElement*>(window));
        std::memset(static_cast<std::byte*>(window) + 0x2C, 0, 0x40);
    }

    // Original function: FUN_0047849a.
    void __fastcall BaseWindow_Destruct(void* window)
    {
        Write(window, 0, gBaseWindowVirtualTable + 0);
        BaseWindow_DestructState(window);
        WindowList(window, 0x50)->~TElementList();
        WindowList(window, 0x3C)->~TElementList();
        BaseElement_Destruct(window);
    }

    // Original function: FUN_004785e8.
    void BaseWindow_Element_Remove(void* window, void* element)
    {
        Write<std::uint8_t>(element, 0x19, Read<std::uint8_t>(element, 0x19) & 0xF7u);
        if (Read<void*>(window, 0x64) == element)
        {
            using TDeactivate = void(__thiscall*)(void*);
            Read<TDeactivate*>(element, 0)[5](element);
            Write<void*>(window, 0x64, nullptr);
        }
        CDesktop::sTheObjectPtr->Element_Remove(static_cast<CBaseElement*>(element));
        WindowList(window, 0x3C)->l_Base_RemoveElement(element);
        WindowList(window, 0x50)->l_Base_RemoveElement(element);
    }

    // Original function: FUN_00478627; removal occurs even before the null check.
    void BaseWindow_Element_Delete(void* window, void* element)
    {
        BaseWindow_Element_Remove(window, element);
        if (element != nullptr)
        {
            using TDelete = void*(__thiscall*)(void*, std::uint32_t);
            Read<TDelete*>(element, 0)[0](element, 1);
        }
    }

    // Original function: FUN_00478642.
    void BaseWindow_Element_DeleteAll(void* window)
    {
        while (void* element = WindowList(window, 0x3C)->l_Base_GetStartElement()) BaseWindow_Element_Delete(window, element);
    }

    // Original function: FUN_004787ac; point containment uses inclusive, wrapped
    // right/bottom bounds exactly as Windows SPoint::IsInside (004398AF).
    void* BaseWindow_FindElementOnPosition(void* window, const std::int32_t* point, bool includeDisabled)
    {
        NXBasics::CListBase::Iterator iterator(*WindowList(window, 0x50));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
        {
            const auto x = Read<std::int32_t>(child, 4), y = Read<std::int32_t>(child, 8);
            const auto right = static_cast<std::int32_t>(Read<std::uint32_t>(child, 4) + Read<std::uint32_t>(child, 0xC) - 1u);
            const auto bottom = static_cast<std::int32_t>(Read<std::uint32_t>(child, 8) + Read<std::uint32_t>(child, 0x10) - 1u);
            if (point[0] < x || point[0] > right || point[1] < y || point[1] > bottom) continue;
            std::int32_t localPoint[2] = {point[0], point[1]};
            using TConvert = void(__thiscall*)(void*, std::int32_t*);
            Read<TConvert*>(child, 0)[11](child, localPoint);
            using TFind = void*(__thiscall*)(void*, const std::int32_t*, bool);
            if (void* hit = Read<TFind*>(child, 0)[10](child, localPoint, includeDisabled)) return hit;
        }
        return nullptr;
    }

    // Original function: FUN_004786a7.
    void __fastcall BaseWindow_Activate(void* window, void*, const std::int32_t* point)
    {
        BaseElement_Activate(window, nullptr, point);
        CDesktop::sTheObjectPtr->Window_BringToFront(static_cast<CBaseWindow*>(window));
        std::int32_t localPoint[2] = {point[0], point[1]};
        using TConvert = void(__thiscall*)(void*, std::int32_t*);
        Read<TConvert*>(window, 0)[11](window, localPoint);
        void* hit = BaseWindow_FindElementOnPosition(window, localPoint, false);
        void* active = Read<void*>(window, 0x64);
        using TDeactivate = void(__thiscall*)(void*);
        if (hit == nullptr)
        {
            if (active != nullptr) { Read<TDeactivate*>(active, 0)[5](active); Write<void*>(window, 0x64, nullptr); }
        }
        else if (hit != active)
        {
            if (active != nullptr) Read<TDeactivate*>(active, 0)[5](active);
            Write(window, 0x64, hit);
            using TActivate = void(__thiscall*)(void*, const std::int32_t*);
            Read<TActivate*>(hit, 0)[4](hit, localPoint);
        }
    }

    // Original function: FUN_00478720.
    void __fastcall BaseWindow_DeActivate(void* window)
    {
        void* active = Read<void*>(window, 0x64);
        if (active != nullptr)
        {
            using TDeactivate = void(__thiscall*)(void*);
            Read<TDeactivate*>(active, 0)[5](active);
            Write<void*>(window, 0x64, nullptr);
        }
        BaseElement_DeActivate(window);
    }

    // Original function: FUN_0047873c; the press callback receives the window's
    // local point, while the search itself applies each child's virtual conversion.
    bool __fastcall BaseWindow_ButtonPressed(void* window, void*, const std::int32_t* point)
    {
        if (Read<std::uint32_t>(window, 0x14) != 1) return false;
        std::int32_t localPoint[2] = {point[0], point[1]};
        using TConvert = void(__thiscall*)(void*, std::int32_t*);
        Read<TConvert*>(window, 0)[11](window, localPoint);
        void* hit = BaseWindow_FindElementOnPosition(window, localPoint, false);
        if (hit == nullptr) return false;
        const auto flags = Read<std::uint32_t>(hit, 0x18);
        // BE_CanBePressed (00478A4F) rejects disabled elements before bit 1.
        if ((flags & 0x400u) != 0 || (flags & 2u) == 0) return false;
        if (hit != Read<void*>(window, 0x64)) return true;
        using TPress = bool(__thiscall*)(void*, const std::int32_t*);
        return Read<TPress*>(hit, 0)[6](hit, localPoint);
    }

    // Original function: FUN_0047879d.
    bool __fastcall BaseWindow_ButtonReleased(void* window)
    {
        void* active = Read<void*>(window, 0x64);
        if (active == nullptr) return false;
        using TRelease = bool(__thiscall*)(void*);
        return Read<TRelease*>(active, 0)[7](active);
    }

    // Original function: FUN_00478511; child draw order follows the forward list.
    void BaseWindow_DrawAllChildElements(void* window, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle& rectangle)
    {
        NXBasics::CListBase::Iterator iterator(*WindowList(window, 0x3C));
        for (void* child = iterator.First(); child != nullptr; child = iterator.Next())
        {
            if ((Read<std::uint32_t>(child, 0x18) & 0x200u) != 0) continue;
            auto area = BaseElement_GetRectangle(child);
            if (!rectangle.IsTouching(area)) continue;
            area.CutInside(rectangle);
            NXBasics::CBitmap clipped(bitmap, area);
            area.x = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(area.x) - Read<std::uint32_t>(child, 4));
            area.y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(area.y) - Read<std::uint32_t>(child, 8));
            using TDraw = void(__thiscall*)(void*, NXBasics::CBitmap*, const NXBasics::SRectangle*);
            Read<TDraw*>(child, 0)[3](child, &clipped, &area);
        }
    }

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
    // Original function: FUN_00478ac2; an already visible element is not written.
    void BaseElement_InternalUnHide(void* element)
    {
        const auto flags = Read<std::uint32_t>(element, 0x18);
        if ((flags & 0x200u) != 0) Write(element, 0x18, flags & ~0x200u);
    }
}
