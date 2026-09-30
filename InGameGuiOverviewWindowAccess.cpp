#include "InGameGuiOverviewWindowAccess.h"
#include "InGameGuiOverviewDisplayAccess.h"
#include "GuiToolsBase.h"
#include "InGameGuiManager.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"

#include <cstring>

extern void* __fastcall FUN_004b8d11(void* window, void* unusedEdx, const NXBasics::SRectangle* rectangle);
extern void __fastcall FUN_004785b1(void* window, void* unusedEdx, void* element);
extern void* __fastcall FUN_004b9520(void* object, void* unusedEdx, const NXBasics::SRectangle* rectangle, std::uint32_t graphicsId, const char* text, std::uint32_t tooltipId, void* formatObject);
extern void __fastcall FUN_004b95a4(void* object, void* unusedEdx, NXBasics::CPalette* palette);
extern void __fastcall FUN_004b959a(void* object, void* unusedEdx, std::uint32_t graphicsId);
extern void __fastcall FUN_004a2322(void* manager);
extern void* __fastcall FUN_004c9777(void* window, void* unusedEdx, std::uint32_t flags);
extern void __fastcall FUN_004c9824(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle);
extern bool __fastcall FUN_004788a7(void* object);
extern void __fastcall FUN_004786a7(void* object, void* unusedEdx, const std::int32_t* point);
extern void __fastcall FUN_00478720(void* object);
extern bool __fastcall FUN_0047873c(void* object, void* unusedEdx, const std::int32_t* point);
extern bool __fastcall FUN_0047879d(void* object);
extern bool __fastcall FUN_004788b1(void* object, void* unusedEdx, const std::int32_t* point);
extern void* __fastcall FUN_004788c2(void* object);
extern void* __fastcall FUN_00478ad8(void* object, void* unusedEdx, const std::int32_t* point, bool flag);
extern void __fastcall FUN_00478a65(void* object, void* unusedEdx, std::int32_t* point);
extern void __fastcall FUN_004788be(void* object);
extern bool __fastcall FUN_004b8659(void* object, void* unusedEdx, std::uint32_t argument);
extern bool __fastcall FUN_004788b6(void* object);

namespace NC2InGameGuiOverviewDisplay
{
    template<typename T> static T OverviewWindow_Read(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void OverviewWindow_Write(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    static void WorldOverviewStaticWindow_AddButtons(void* window);
    static void WorldOverviewStaticWindow_UpdateButtonGraphics(void* window);
    static void WorldOverviewStaticWindow_UpdateDisplayFlag(void* window);

    // Original function: FUN_004c97ac; XGui_BE_Message_Handle.
    static bool __fastcall WorldOverviewStaticWindow_MessageHandle(void* window, void* unusedEdx, std::int32_t message, std::int32_t param1, std::uint32_t param2, std::uint32_t param3, std::uint32_t param4)
    {
        (void)unusedEdx;
        (void)param2;
        (void)param3;
        (void)param4;
        if (message != 0x800) return false;
        switch (param1)
        {
        case 10000:
            NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
            break;
        case 10001:
        {
            std::uint32_t mode = OverviewWindow_Read<std::uint32_t>(window, 0x70) + 1u;
            OverviewWindow_Write(window, 0x70, mode);
            if (mode == 4) OverviewWindow_Write<std::uint32_t>(window, 0x70, 0);
            WorldOverviewStaticWindow_UpdateButtonGraphics(window);
            WorldOverviewStaticWindow_UpdateDisplayFlag(window);
            break;
        }
        case 10005:
            FUN_004a2322(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
            break;
        case 10010:
            WorldOverviewElement_ModifyZoom(OverviewWindow_Read<void*>(window, 0x74), -1);
            break;
        case 10011:
            WorldOverviewElement_ModifyZoom(OverviewWindow_Read<void*>(window, 0x74), 1);
            break;
        }
        return true;
    }

    // Complete original table at 004F78FC..004F7937. Window/base destruction,
    // title drawing and inherited GUI virtual methods remain original boundaries.
    static const std::uintptr_t gWorldOverviewStaticWindowVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&FUN_004c9777),
        reinterpret_cast<std::uintptr_t>(&FUN_004788a7),
        reinterpret_cast<std::uintptr_t>(&WorldOverviewStaticWindow_MessageHandle),
        reinterpret_cast<std::uintptr_t>(&FUN_004c9824),
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
        reinterpret_cast<std::uintptr_t>(&FUN_004788b6)
    };
    static_assert(sizeof(gWorldOverviewStaticWindowVirtualTable) == 0x3C);

    // Original function: FUN_004c9708; CWorldOverviewStaticGuiWindow constructor.
    void* WorldOverviewStaticWindow_Construct(void* window)
    {
        const std::int32_t y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(NInGameGuiManager::CInGameGuiManager::mStaticVars.displayHeight) - 120u);
        const NXBasics::SRectangle rectangle(0, y, 140, 120);
        FUN_004b8d11(window, nullptr, &rectangle);
        OverviewWindow_Write(window, 0, gWorldOverviewStaticWindowVirtualTable + 0);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 0x0C);
        OverviewWindow_Write<std::uint32_t>(window, 0x70, 0);
        WorldOverviewStaticWindow_AddButtons(window);
        WorldOverviewStaticWindow_UpdateDisplayFlag(window);
        void* element = OverviewWindow_Read<void*>(window, 0x74);
        using TMessage = bool(__thiscall*)(void*, std::int32_t, std::int32_t, std::uint32_t, std::uint32_t, std::uint32_t);
        auto* methods = *static_cast<TMessage**>(element);
        methods[2](element, 0x800, 12000, 0, 0, 0);
        return window;
    }

    // Original function: FUN_004c9860; l_GuiButtons_Add.
    static void WorldOverviewStaticWindow_AddButtons(void* window)
    {
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x816C, 1);
        const NXBasics::SRectangle overviewRectangle(4, 4, 112, 112);
        void* element = allocation == nullptr ? nullptr : WorldOverviewElement_Construct(allocation, &overviewRectangle, 0);
        OverviewWindow_Write(window, 0x74, element);
        FUN_004785b1(window, nullptr, element);
        struct SButtonParameters { std::int32_t y, width, height; std::uint32_t graphics, command, stringId; std::int32_t stringArray; };
        constexpr SButtonParameters buttons[] =
        {
            { 0, 30, 16, 0x39, 10000, 0x21, 12 },
            { 17, 30, 30, 0x3C, 10001, 0x22, 12 },
            { 47, 30, 30, 0x89, 10005, 0x23, 12 },
            { 92, 30, 30, 0x3A, 10010, 0x106, 4 },
            { 69, 30, 30, 0x3B, 10011, 0x105, 4 }
        };
        for (std::uint32_t index = 0; index < 5; ++index)
        {
            const auto& button = buttons[index];
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            void* control = nullptr;
            if (allocation != nullptr)
            {
                const char* text = NC2GuiToolsBase::CGuiBaseStringsManager::GetStringPtr(button.stringArray, button.stringId);
                const NXBasics::SRectangle rectangle(115, button.y, button.width, button.height);
                control = FUN_004b9520(allocation, nullptr, &rectangle, button.graphics, text, button.command, window);
            }
            if (index == 1) OverviewWindow_Write(window, 0x78, control);
            // FUN_004b9407 stores the center-graphics flag; the Windows path
            // still dereferences the result after a failed allocation.
            OverviewWindow_Write<std::uint8_t>(control, 0x34, 0);
            FUN_004b95a4(control, nullptr, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
            FUN_004785b1(window, nullptr, control);
        }
        WorldOverviewStaticWindow_UpdateButtonGraphics(window);
    }

    // Original function: FUN_004c9a80; l_GuiButtons_UpdateButtonGraphics.
    static void WorldOverviewStaticWindow_UpdateButtonGraphics(void* window)
    {
        const std::int32_t mode = OverviewWindow_Read<std::int32_t>(window, 0x70);
        std::uint32_t graphics = 0x3C;
        if (mode == 1) graphics = 0x3D;
        if (mode == 2) graphics = 0x3E;
        if (mode == 3) graphics = 0x3E;
        FUN_004b959a(OverviewWindow_Read<void*>(window, 0x78), nullptr, graphics);
    }

    // Original function: FUN_004c9aa8; l_Misc_UpdateDisplayFlag.
    static void WorldOverviewStaticWindow_UpdateDisplayFlag(void* window)
    {
        const std::uint32_t mode = OverviewWindow_Read<std::uint32_t>(window, 0x70);
        if (mode > 3) return;
        void* element = OverviewWindow_Read<void*>(window, 0x74);
        OverviewWindow_Write<std::uint8_t>(element, 0x806C, mode == 1 ? 0 : 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x806D, 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x806E, 0);
        OverviewWindow_Write<std::uint8_t>(element, 0x806F, mode == 1 ? 0 : 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8070, mode == 1 ? 0 : 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8072, mode == 1 ? 0 : 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8074, mode == 1 ? 0 : 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8044, 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8075, mode == 2 ? 1 : 0);
        OverviewWindow_Write<std::uint8_t>(element, 0x8044, 1);
        OverviewWindow_Write<std::uint8_t>(element, 0x8076, mode == 3 ? 1 : 0);
        OverviewWindow_Write<std::uint8_t>(element, 0x8044, 1);
    }
}
