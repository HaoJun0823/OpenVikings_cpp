#include "XGuiElementAccess.h"
#include "C2LifetimeAccess.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"

#include <cstring>


namespace NXBaseGui
{
    using namespace NC2Lifetime;
    static void* __fastcall BaseElement_Delete(void* element, void* unusedEdx, std::uint32_t flags);
    // Complete eighteen-entry base-element table 004F43F4..004F443B.
    const std::uintptr_t gBaseElementVirtualTable[18] = {
        reinterpret_cast<std::uintptr_t>(&BaseElement_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Draw),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Activate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_DeActivate),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonPressed),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ButtonReleased),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_GetRealActiveElementPtr),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify)
    };
    // Original 00504E58 DWORD: all exported xrefs assign then increment it.
    static std::uint32_t gNextElementSerial = 1;

    // Original function: FUN_00478936; serial assignment follows state/flags.
    static void BaseElement_ConstructState(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId)
    {
        std::memset(static_cast<std::byte*>(element) + 4, 0, 0x28);
        Write<std::uint32_t>(element, 0x14, 0);
        std::memcpy(static_cast<std::byte*>(element) + 4, rectangle, sizeof(*rectangle));
        flags |= 0x20u;
        if ((flags & 2u) != 0) flags |= 1u;
        Write(element, 0x18, flags);
        Write(element, 0x1C, elementId);
        Write(element, 0x28, gNextElementSerial);
        ++gNextElementSerial;
    }

    // Original function: FUN_004788c6.
    void* BaseElement_Construct(void* element, const NXBasics::SRectangle* rectangle, std::uint32_t flags, std::uint32_t elementId)
    {
        Write(element, 0, gBaseElementVirtualTable + 0);
        BaseElement_ConstructState(element, rectangle, flags, elementId);
        return element;
    }

    // Original function: FUN_00478917; base destruction preserves the table.
    void BaseElement_Destruct(void* element)
    {
        Write(element, 0, gBaseElementVirtualTable + 0);
        std::memset(static_cast<std::byte*>(element) + 4, 0, 0x28);
        Write<std::uint32_t>(element, 0x14, 0);
    }

    // Original function: FUN_004788e6.
    static void* __fastcall BaseElement_Delete(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        BaseElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }

    // Original function: FUN_004788a7.
    bool __fastcall BaseElement_HasSubElements(void* element) { return (Read<std::uint32_t>(element, 0x18) & 0x40u) != 0; }

    // Original function: FUN_004788b1; the base hit test accepts every local point.
    bool __fastcall BaseElement_IsHit(void*, void*, const std::int32_t*) { return true; }

    // Original function: FUN_004788b6.
    bool __fastcall BaseElement_ToolTip_AtOnce(void*) { return false; }

    // Original function: FUN_004788b9; the original consumes two stack arguments.
    std::uint32_t __fastcall BaseElement_UnresolvedVirtual40(void*, void*, std::uint32_t, std::uint32_t) { return 0; }

    // Original function: FUN_004788be; shared added-to-desktop/pressed-action slot.
    void __fastcall BaseElement_Notify(void*) {}

    // Original function: FUN_004788bf.
    void __fastcall BaseWindow_DrawOverlays(void*, void*, std::uint32_t, std::uint32_t) {}

    // Original function: FUN_004788c2.
    void* __fastcall BaseWindow_GetRealActiveElementPtr(void* window) { return Read<void*>(window, 0x64); }

    // Original function: FUN_00478983; the base renderer is an empty original body.
    void __fastcall BaseElement_Draw(void*, void*, NXBasics::CBitmap*, const NXBasics::SRectangle*) {}

    // Original function: FUN_00478986.
    void __fastcall BaseElement_Activate(void* element, void*, const std::int32_t*) { Write<std::uint32_t>(element, 0x14, 1); }

    // Original function: FUN_00478994.
    void __fastcall BaseElement_DeActivate(void* element) { Write<std::uint32_t>(element, 0x14, 0); }

    // Original function: FUN_0047899f.
    bool __fastcall BaseElement_ButtonPressed(void* element, void*, const std::int32_t*) { Write<std::uint32_t>(element, 0x14, 2); return true; }

    // Original function: FUN_004789af.
    bool __fastcall BaseElement_ButtonReleased(void* element)
    {
        const auto state = Read<std::uint32_t>(element, 0x14);
        if (state == 1) Write<std::uint32_t>(element, 0x14, 0);
        if (state != 2) return false;
        if ((Read<std::uint8_t>(element, 0x18) & 8u) != 0)
        {
            auto* parent = Read<CBaseWindow*>(element, 0x20);
            // Original adapter 0047867A forwards to the existing deferred close list.
            if (parent != nullptr) CDesktop::sTheObjectPtr->Window_AddToCloseList(parent);
        }
        Write<std::uint32_t>(element, 0x14, 1);
        using TPressedAction = void(__thiscall*)(void*);
        Read<TPressedAction*>(element, 0)[17](element);
        return true;
    }

    // Original function: FUN_00478a36; ret 14h establishes five stack DWORDs.
    bool __fastcall BaseElement_Message_Handle(void*, void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t) { return false; }

    // Original function: FUN_00478a65; unsigned subtraction preserves x86 wraparound.
    void __fastcall BaseElement_ParentToLocalPosition(void* element, void*, std::int32_t* point)
    {
        Write(point, 0, Read<std::uint32_t>(point, 0) - Read<std::uint32_t>(element, 4));
        Write(point, 4, Read<std::uint32_t>(point, 4) - Read<std::uint32_t>(element, 8));
    }

    // Original function: FUN_00478ad8.
    void* __fastcall BaseElement_FindElementOnPosition(void* element, void*, const std::int32_t* point, bool includeDisabled)
    {
        const auto flags = Read<std::uint32_t>(element, 0x18);
        if (!includeDisabled && ((flags & 0x400u) != 0 || (flags & 1u) == 0)) return nullptr;
        if ((flags & 0x200u) != 0) return nullptr;
        using THitTest = bool(__thiscall*)(void*, const std::int32_t*);
        return Read<THitTest*>(element, 0)[8](element, point) ? element : nullptr;
    }

    // Original function: FUN_0048e569; shared identity-returning virtual entry.
    void* __fastcall BaseElement_GetRealActiveElementPtr(void* element) { return element; }

    // Original function: FUN_004b8659.
    bool __fastcall BaseElement_ToolTip_Available(void*, void*, std::uint32_t) { return false; }

    // Original function: FUN_004bf2f4.
    bool __fastcall BaseElement_Hide_CanBeHidden(void*) { return true; }
}
