#include "InGameGuiBaseAccess.h"
#include "ApplicationGlobals.h"
#include "C2LifetimeAccess.h"
#include "XGuiDesktop.h"

#include <cstring>

// The shared base-window constructor, rendering and inherited virtual methods
// retain their genuine Windows bodies until their GUI families are reconstructed.
extern void* __fastcall FUN_0047841b(void*, void* unusedEdx, const NXBasics::SRectangle*, std::uint32_t, std::uint32_t);
extern void* __fastcall FUN_004c9313(void*, void* unusedEdx, std::uint32_t);
extern bool __fastcall FUN_004788a7(void*);
extern bool __fastcall FUN_00478a36(void*, void* unusedEdx, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern void __fastcall FUN_004b8d42(void*, void* unusedEdx, NXBasics::CBitmap*, const NXBasics::SRectangle*);
extern void __fastcall FUN_004786a7(void*, void* unusedEdx, const std::int32_t*);
extern void __fastcall FUN_00478720(void*);
extern bool __fastcall FUN_0047873c(void*, void* unusedEdx, const std::int32_t*);
extern bool __fastcall FUN_0047879d(void*);
extern bool __fastcall FUN_004788b1(void*, void* unusedEdx, const std::int32_t*);
extern void* __fastcall FUN_004788c2(void*);
extern void* __fastcall FUN_00478ad8(void*, void* unusedEdx, const std::int32_t*, bool);
extern void __fastcall FUN_00478a65(void*, void* unusedEdx, std::int32_t*);
extern void __fastcall FUN_004788be(void*);
extern bool __fastcall FUN_004b8659(void*, void* unusedEdx, std::uint32_t);
extern bool __fastcall FUN_004788b6(void*);
extern bool __fastcall FUN_004bf2f4(void*);
extern std::uint32_t __fastcall FUN_004788b9(void*, void* unusedEdx, std::uint32_t, std::uint32_t);
extern void __fastcall FUN_004788bf(void*, void* unusedEdx, std::uint32_t, std::uint32_t);

namespace NC2InGameGuiBase
{
    using namespace NC2Lifetime;

    // Complete nineteen-entry table at 004F52A4..004F52EF.
    const std::uintptr_t gBaseToolWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&FUN_004c9313),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a36),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8d42),
        reinterpret_cast<std::uintptr_t>(&FUN_004786a7),
        reinterpret_cast<std::uintptr_t>(&FUN_00478720),
        reinterpret_cast<std::uintptr_t>(&FUN_0047873c),
        reinterpret_cast<std::uintptr_t>(&FUN_0047879d),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b1),
        reinterpret_cast<std::uintptr_t>(&FUN_004788c2),
        reinterpret_cast<std::uintptr_t>(&FUN_00478ad8),
        reinterpret_cast<std::uintptr_t>(&FUN_00478a65),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004b8659),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6),
        reinterpret_cast<std::uintptr_t>(&FUN_004bf2f4),
        reinterpret_cast<std::uintptr_t>(&FUN_004788b9),
        reinterpret_cast<std::uintptr_t>(&FUN_004788be),
        reinterpret_cast<std::uintptr_t>(&FUN_004788bf)
    };

    // Original function: FUN_004b8543.
    void BaseToolDesktop_AddWindow(NXBaseGui::CBaseWindow* window, bool showWhileHidden)
    {
        NXBaseGui::CDesktop::sTheObjectPtr->Window_Add(window);
        if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 0) != 0 && !showWhileHidden)
            NXBaseGui::CDesktop::sTheObjectPtr->Window_Hide(window);
    }

    // Original function: FUN_004b8518; Windows normal desktop-element adapter.
    void BaseToolDesktop_AddElement(NXBaseGui::CBaseElement* element)
    {
        NXBaseGui::CDesktop::sTheObjectPtr->Element_Add(element, true);
        if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 0) != 0)
            NXBaseGui::CDesktop::sTheObjectPtr->Element_Hide(element);
    }

    // Original function: FUN_004b8d11; CBaseToolWindow constructor.
    void* BaseToolWindow_Construct(void* window, const NXBasics::SRectangle* rectangle)
    {
        FUN_0047841b(window, nullptr, rectangle, 0, 0);
        Write(window, 0, gBaseToolWindowVirtualTable + 0);
        std::memset(static_cast<std::byte*>(window) + 0x6C, 0, 4);
        Write<std::uint32_t>(window, 0x6C, 0);
        return window;
    }
}
