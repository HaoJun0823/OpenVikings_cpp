#include "InGameGuiStringOutputWindow.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiManager.h"
#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "GuiToolsBase.h"
#include "NetworkShutdown.h"
#include "StringInputHandler.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"
#include "XGuiWindowAccess.h"
#include "XStringArray.h"

#include <new>
#include <Windows.h>
#include <mmsystem.h>

// Shared graphic-button/clock construction, base-window destruction and framing.
extern void* __fastcall FUN_004b9961(void*, void* unusedEdx, const NXBasics::SRectangle*);
extern void* __fastcall FUN_004b99b8(void*, void* unusedEdx, const NXBasics::SRectangle*);
extern void __fastcall FUN_0047849a(void*);
extern void __fastcall FUN_004b8d42(void*, void* unusedEdx, NXBasics::CBitmap*, const NXBasics::SRectangle*);
extern bool __fastcall FUN_004788a7(void*);
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

namespace NC2InGameGuiMiscWindows
{
    using namespace NC2Lifetime;

    static void StringOutputWindow_String_Init(void* window);
    static void StringOutputWindow_String_Exit(void* window);
    static void* __fastcall StringOutputWindow_Delete(void* window, void* unusedEdx, std::uint32_t flags);
    static bool __fastcall StringOutputWindow_MessageHandle(void* window, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    static void __fastcall StringOutputWindow_Draw(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);

    // Complete nineteen-entry table 004F7620..004F766B. Its inherited slots
    // preserve the base-tool-window methods, including slots 15..18.
    static const std::uintptr_t gStringOutputWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&StringOutputWindow_Delete),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&StringOutputWindow_MessageHandle),
        reinterpret_cast<std::uintptr_t>(&StringOutputWindow_Draw),
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

    static std::int32_t AddWrapped(std::int32_t value, std::uint32_t amount)
    {
        return NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(value) + amount);
    }

    static const NXBasics::SColorRGB* FontColour255(const NXBasics::CPalette* palette)
    {
        return reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(palette) + 0x400);
    }

    // Original function: FUN_004c7030; CStringOutputWindow constructor.
    void* StringOutputWindow_Construct(void* window)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const NXBasics::SRectangle rectangle(170, AddWrapped(state.displayHeight, 0xFFFFFFC0u), AddWrapped(state.displayWidth, 0xFFFFFE48u), 64);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        Write(window, 0, gStringOutputWindowVirtualTable + 0);
        Write<std::uint32_t>(window, 0x70, 0);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        void* closer = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle closeRectangle(AddWrapped(Read<std::int32_t>(window, 0xC), 0xFFFFFFEEu), 2, 16, 14);
            closer = FUN_004b9961(allocation, nullptr, &closeRectangle);
        }
        NXBaseGui::BaseWindow_Element_Add(window, closer);
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x50, 1);
        void* clock = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle clockRectangle(AddWrapped(Read<std::int32_t>(window, 0xC), 0xFFFFFFE8u), AddWrapped(Read<std::int32_t>(window, 0x10), 0xFFFFFFE8u), 20, 20);
            clock = FUN_004b99b8(allocation, nullptr, &clockRectangle);
        }
        Write<std::uint8_t>(clock, 0x30, 1);
        NXBaseGui::BaseWindow_Element_Add(window, clock);
        StringOutputWindow_String_Init(window);
        return window;
    }

    // Original function: FUN_004c70fe; deleting destructor, preserving both phases.
    static void* __fastcall StringOutputWindow_Delete(void* window, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        Write(window, 0, gStringOutputWindowVirtualTable + 0);
        StringOutputWindow_String_Exit(window);
        Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
        FUN_0047849a(window);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
        return window;
    }

    // Original function: FUN_004c712d.
    static bool __fastcall StringOutputWindow_MessageHandle(void* window, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
    {
        (void)unusedEdx;
        (void)param2;
        (void)param3;
        if (message == 0x1000) return command == 0x7DC;
        if (message != 0x800) return false;
        if (command == 0x4E20)
            Read<NXBasics::CStringArray*>(window, 0x70)->InsertString(reinterpret_cast<const char*>(param1));
        else if (command == 0x4E21)
        {
            StringOutputWindow_String_Exit(window);
            StringOutputWindow_String_Init(window);
        }
        return true;
    }

    // Original function: FUN_004c7181.
    static void __fastcall StringOutputWindow_Draw(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        FUN_004b8d42(window, nullptr, bitmap, rectangle);
        auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        if (NNetwork::Network_IsInSession())
        {
            const auto seconds = NC2Logic::gGameTime.tick / 12u;
            const auto minutes = seconds / 60u;
            char elapsed[100];
            NXBasics::Runtime_PrintString(elapsed, "%d:%2.2d:%2.2d", static_cast<std::int32_t>(minutes / 60u), static_cast<std::int32_t>(minutes % 60u), static_cast<std::int32_t>(seconds % 60u));
            const auto* colour = FontColour255(resources.fontDimmedPalette);
            auto* font = resources.font08;
            const auto textWidth = NXBasics::Font_GetPixelWidth(font, elapsed);
            const auto x = AddWrapped(Read<std::int32_t>(window, 0xC), 0u - static_cast<std::uint32_t>(textWidth) - 2u);
            bitmap->Text_Print(font, colour, x, 20, elapsed);
        }
        auto* font = resources.font08;
        std::int32_t y = 0;
        if (NC2Logic::gGameStaticVars.networkWait != 0)
        {
            const auto* colour = FontColour255(resources.fontRedPalette);
            const char* waiting = NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x122);
            bitmap->Text_Print(font, colour, 4, 0, waiting);
            y = 14;
        }
        auto* input = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetStringInputHandler();
        if (input->mEditing != 0)
        {
            bitmap->Text_Print(font, FontColour255(resources.fontWhitePalette), 4, y, ">");
            bitmap->Text_Print(font, FontColour255(resources.fontWhitePalette), 14, y, input->GetString());
            const auto shade = static_cast<std::uint8_t>(((timeGetTime() / 200u) & 1u) != 0 ? 255 : 100);
            const NXBasics::SColorRGB cursorColour(shade, shade, shade);
            const auto cursorX = NXBasics::Runtime_TimeSigned32(input->GetCursorPixelPosition(font) + 14u);
            bitmap->Draw_VerticalLine(cursorX, static_cast<std::uint32_t>(y) + 2u, 10, cursorColour);
            y = AddWrapped(y, 12);
        }
        auto index = Read<NXBasics::CStringArray*>(window, 0x70)->GetNumberOfStrings() - 1u;
        while (NXBasics::Runtime_TimeSigned32(index) >= 0 && y < Read<std::int32_t>(window, 0x10))
        {
            const auto* colour = FontColour255(resources.fontWhitePalette);
            const char* text = Read<NXBasics::CStringArray*>(window, 0x70)->GetStringPtr(index);
            bitmap->Text_Print(font, colour, 4, y, text);
            --index;
            y = AddWrapped(y, 12);
        }
    }

    // Original function: FUN_004c733e.
    static void StringOutputWindow_String_Init(void* window)
    {
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x20, 1);
        auto* strings = allocation == nullptr ? nullptr : new (allocation) NXBasics::CStringArray(1);
        Write(window, 0x70, strings);
    }

    // Original function: FUN_004c735f; the original leaves +70 unchanged.
    static void StringOutputWindow_String_Exit(void* window)
    {
        auto* strings = Read<NXBasics::CStringArray*>(window, 0x70);
        if (strings != nullptr) delete strings;
    }
}
