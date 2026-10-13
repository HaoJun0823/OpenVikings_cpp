#include "ReadableValues.h"
#include "InGameGuiConstructionSelectionWindow.h"
#include "InGameGuiManager.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiHelpWindow.h"
#include "C2HouseAccess.h"
#include "C2StaticHouseTypeData.h"
#include "C2StringAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2PlayerStatisticAccess.h"
#include "GuiListBoxElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiToolsBase.h"
#include "MainMenuGuiManager_Access.h"
#include "XGuiWindowAccess.h"
#include "XGuiDesktop.h"
#include "XCrtFileAccess.h"
#include <array>
#include <cstring>
#include <Windows.h>
#include <mmsystem.h>

namespace NC2InGameGuiMiscWindows
{
    using namespace NC2Lifetime;
    using namespace NC2InGameGuiBase;
    static void* __fastcall ConstructionSelectionWindow_Delete(void* window, void* unusedEdx, std::uint32_t flags);
    // Complete Windows table 004F75C0..004F760B.
    static const std::array<std::uintptr_t, 19> gConstructionSelectionWindowVirtualTable = []
    {
        std::array<std::uintptr_t, 19> table{};
        std::memcpy(table.data(), gBaseToolWindowVirtualTable, sizeof(table));
        table[0] = reinterpret_cast<std::uintptr_t>(&ConstructionSelectionWindow_Delete);
        table[2] = reinterpret_cast<std::uintptr_t>(&ConstructionSelectionWindow_XGui_BE_Message_Handle);
        return table;
    }();

    // Original function: FUN_004c647e; Windows allocation 0x3A8.
    void* ConstructionSelectionWindow_Construct(void* window, std::uint32_t specialItemType, std::uint32_t specialItem)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayWidth) - 500u) / 2, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayHeight) - 420u) / 2, 500, 420);
        BaseToolWindow_Construct(window, &rectangle);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 824);
        Write(window, 0, gConstructionSelectionWindowVirtualTable.data());
        Write(window, 0x7C, specialItemType); Write(window, 0x80, specialItem);
        ConstructionSelectionWindow_l_GuiButtons_Add(window);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(window);
        ConstructionSelectionWindow_XGui_BE_Message_Handle(window, nullptr, 0x800, 0x7EE, 0, 0, 0);
        return window;
    }

    // Original function: FUN_004c651e; combined destructor/deleting-destructor entry.
    static void* __fastcall ConstructionSelectionWindow_Delete(void* window, void*, std::uint32_t flags)
    {
        Write(window, 0, gConstructionSelectionWindowVirtualTable.data());
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_UnRegister(window);
        Write(window, 0, gBaseToolWindowVirtualTable + 0);
        NXBaseGui::BaseWindow_Destruct(window);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
        return window;
    }

    // Original function: FUN_004c6554; both native query IDs have distinct semantics.
    bool __fastcall ConstructionSelectionWindow_XGui_BE_Message_Handle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
    {
        const auto rebuild = [window]
        {
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            Write<void*>(window, 0x74, nullptr);
            ConstructionSelectionWindow_l_GuiButtons_Add(window);
        };
        if (message == 0x1000)
        {
            if (command == 0x7DA) return true;
            if (command != 0x7DB) return false;
            if (param1 != Read<std::uint32_t>(window, 0x7C) || param2 != Read<std::uint32_t>(window, 0x80))
            {
                Write(window, 0x7C, param1); Write(window, 0x80, param2); rebuild();
            }
            return true;
        }
        if (message != 0x800) return false;
        if (command >= 10000 && command < 10100) { Write(window, 0x70, command - 10000); rebuild(); return true; }
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        auto* input = NC2InGameGuiInputManager::gInputManager;
        const auto select = [&](std::uint32_t type, std::uint32_t stage)
        {
            NC2InGameGuiInputManager::Selection_SelectEngineHouseType(input, type, stage, Read<std::uint32_t>(window, 0x7C), Read<std::uint32_t>(window, 0x80));
            NC2InGameGuiInputManager::InputMode_Set(input, 12);
        };
        if (command == 10100) { select(param2, param3); manager->MiscWindows_ConstructionSelectionWindow_Close(); }
        else if (command == 10101) manager->MiscWindows_HelpWindow_Open_HouseInfo(param2, param3);
        else if (command == 10102) manager->MiscWindows_HelpWindow_Open_GoodInfo(param2);
        else if (command == 0x7EE)
        {
            if (timeGetTime() > Read<std::uint32_t>(window, 0x78) + 2000u) ConstructionSelectionWindow_l_GuiButtons_UpdateListBox(window);
            if (Read<std::uint32_t>(window, 0x7C) != 0 && Read<std::uint32_t>(window, 0x84) == 1)
            {
                select(Read<std::uint32_t>(window, 0x88), Read<std::uint32_t>(window, 0x8C));
                NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
            }
        }
        else if (command >= 10200 && command <= 10202)
        {
            const std::int32_t modes[3]{14, 16, 18};
            NC2InGameGuiInputManager::InputMode_Set(input, modes[command - 10200]);
            manager->MiscWindows_ConstructionSelectionWindow_Close();
        }
        else if (command == 10300) manager->MiscWindows_GlobalGameSettingsWindow_Open(true, false);
        else if (command == 33331)
        {
            return Read<std::uint32_t>(window, 0x14) == 1 && static_cast<std::uint8_t>(param2) != 0 && NC2InGameGuiBaseListBox::BaseListBoxElement_ScrollOffset_CenterOnEntry_StartingWithLetter(Read<void*>(window, 0x74), static_cast<char>(param2));
        }
        return true;
    }

    // Original function: FUN_004c672b; five filters, list viewport and normal-mode footer.
    void __fastcall ConstructionSelectionWindow_l_GuiButtons_Add(void* window)
    {
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(248, 1);
        NXBasics::SRectangle rectangle(2, 2, NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, 0xC) - 22u), 14);
        void* element = allocation == nullptr ? nullptr : WindowMoverButton_Construct(allocation, &rectangle, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(Read<std::uint32_t>(window, 0x7C) == 0 ? 0 : 1));
        NXBaseGui::BaseWindow_Element_Add(window, element);
        allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
        rectangle = NXBasics::SRectangle(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, 0xC) - 18u), 2, 16, 14);
        element = allocation == nullptr ? nullptr : WindowCloserButton_Construct(allocation, nullptr, &rectangle);
        NXBaseGui::BaseWindow_Element_Add(window, element);
        const auto button = [window](const NXBasics::SRectangle& area, std::uint32_t caption, std::uint32_t command, bool selected)
        {
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            void* child = memory == nullptr ? nullptr : BaseToolTextButton_ConstructToggle(memory, nullptr, &area, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(caption), command, window, selected);
            NXBaseGui::BaseWindow_Element_Add(window, child);
        };
        rectangle = NXBasics::SRectangle(4, 22, 492 / 5 - 2, 22);
        for (std::uint32_t page = 0; page < 5; ++page)
        {
            button(rectangle, 2 + page, 10000 + page, Read<std::uint32_t>(window, 0x70) == page);
            rectangle.x += rectangle.width + 2;
        }
        rectangle = NXBasics::SRectangle(4, 54, 468, 354 - (Read<std::uint32_t>(window, 0x7C) == 0 ? 26 : 0));
        allocation = NXBasics::Runtime_AllocateWithNewHandler(116, 1);
        void* list = allocation == nullptr ? nullptr : ToolListBoxElement_Construct(allocation, nullptr, &rectangle);
        Write(window, 0x74, list);
        NXBaseGui::BaseWindow_Element_Add(window, list);
        ConstructionSelectionWindow_l_GuiButtons_UpdateListBox(window);
        rectangle.x = 476; rectangle.width = 20;
        allocation = NXBasics::Runtime_AllocateWithNewHandler(60, 1);
        element = allocation == nullptr ? nullptr : ToolListBoxControllerElement_Construct(allocation, nullptr, &rectangle, list, false, true);
        NXBaseGui::BaseWindow_Element_Add(window, element);
        if (Read<std::uint32_t>(window, 0x7C) != 0) return;
        rectangle = NXBasics::SRectangle(4, rectangle.y + rectangle.height + 10, 121, 20);
        for (std::uint32_t index = 0; index < 3; ++index)
        {
            button(rectangle, 7 + index, 10200 + index, false);
            rectangle.x += rectangle.width + 2;
        }
        rectangle.x += 2;
        button(rectangle, 501, 10300, false);
    }

    // Original function: FUN_004c6af6; availability, sorting, costs and single-result special items.
    void __fastcall ConstructionSelectionWindow_l_GuiButtons_UpdateListBox(void* window)
    {
        Write<std::uint32_t>(window, 0x84, 0);
        void* list = Read<void*>(window, 0x74);
        if (list == nullptr) return;
        const auto scroll = Read<std::int32_t>(list, 0x34);
        NC2InGameGuiBaseListBox::BaseListBoxElement_Element_DeleteAll(list);
        const auto player = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();
        const auto* playerData = static_cast<const std::byte*>(NC2Logic::gGameControl) + static_cast<std::uint32_t>(player) * ReadableValues::RecordSize::GameControlPlayer;
        struct SHouseEntry { std::uint32_t type, stage; };
        auto* entries = static_cast<SHouseEntry*>(NXBasics::Runtime_AllocateWithNewHandler(static_cast<std::uint32_t>(NC2E2::gStaticHouseCount) * 40u, 1));
        std::uint32_t count = 0;
        const auto page = Read<std::uint32_t>(window, 0x70), specialType = Read<std::uint32_t>(window, 0x7C);
        for (std::int32_t type = 0; type < NC2E2::gStaticHouseCount; ++type)
        {
            const auto* engine = static_cast<const std::byte*>(NC2E2::gStaticHouseEntries) + static_cast<std::uint32_t>(type) * 0x8BCu;
            const auto tribe = Read<std::uint32_t>(engine, 0x50);
            for (std::uint32_t stage = 0; stage < Read<std::uint32_t>(engine, 0x54); ++stage)
            {
                const auto logic = Read<std::uint32_t>(engine, 0x3Cu + stage * 4u);
                if (Read<std::uint8_t>(playerData, 0x2820u + tribe * 55u + logic) == 0) continue;
                const auto main = NC2Logic::gStaticHouseTypeStorage.records[logic].mainType;
                const bool buildable = logic != 1 && main != 6 && main != 7 && Read<std::uint8_t>(playerData, 0x3126u + tribe * 55u + logic) != 0;
                bool include = false;
                if (specialType != 0)
                {
                    if (tribe == 1)
                    {
                        if (specialType == 2) include = buildable;
                        else if (specialType > 2 && specialType <= 4) include = logic == Read<std::uint32_t>(window, 0x80);
                    }
                }
                else if (buildable)
                {
                    include = page == 1 ? main == 3 : page == 2 ? main == 1 : page == 3 ? main == 2 : page == 4 ? main == 5 || logic == 39 : true;
                }
                if (!include) continue;
                entries[count++] = {static_cast<std::uint32_t>(type), stage};
                const auto matches = Read<std::uint32_t>(window, 0x84);
                if (matches < 100)
                {
                    Write(window, 0x88u + matches * 8u, type); Write(window, 0x8Cu + matches * 8u, stage); Write(window, 0x84, matches + 1);
                }
            }
        }
        if (count != 0) NXBasics::Runtime_QuickSort(entries, count, sizeof(SHouseEntry), lsSortHouseTypeCompareFunction);
        const bool expanded = page != 0;
        NXBasics::SRectangle row(0, 0, Read<std::int32_t>(list, 0xC), expanded ? 240 : 32);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto entry = entries[index];
            const auto* engine = static_cast<const std::byte*>(NC2E2::gStaticHouseEntries) + entry.type * 0x8BCu;
            const auto* name = NC2Logic::StringAccess_GetHouseNamePtr(entry.type, entry.stage, 1);
            std::int32_t unfinished = 0;
            const auto total = NC2Logic::Tool_PlayerStatistics_CountHousesOfLogicType(player, Read<std::uint32_t>(engine, 0x3Cu + entry.stage * 4u), &unfinished);
            char caption[600], text[180]; caption[0] = '\0';
            if (expanded)
            {
                NXBasics::Runtime_PrintString(text, "{setx:240}{sety:70}{gfxhouse:%d,%d}", entry.type, entry.stage);
                std::strcat(caption, text);
            }
            NXBasics::Runtime_PrintString(text, "{setx:4}{sety:%d}%s - (%d+%d)", expanded ? -100 : 0, name, total - unfinished, unfinished);
            std::strcat(caption, text);
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            void* element = allocation == nullptr ? nullptr : BaseToolTextButton_Construct(allocation, nullptr, &row, caption, 10100, window);
            Write<std::uint8_t>(element, 0x54, 0); Write(element, 0x4C, entry.type); Write(element, 0x50, entry.stage);
            BaseListBoxElement_Element_Add(list, nullptr, element);
            NXBasics::SRectangle control(row.width, row.y + 2, 0, 28);
            NC2Logic::SHouseConstructionGoods goods{};
            NC2Logic::House_Construction_CalculateGoodsNeeded(entry.type, entry.stage, false, &goods);
            for (std::int32_t slot = 0; slot < goods.count; ++slot)
            {
                const auto good = goods.entries[slot].good, amount = goods.entries[slot].amount;
                char tooltip[132], graphic[32];
                NXBasics::Runtime_PrintString(graphic, "{gfxgood:%d,%d}", good, amount);
                NXBasics::Runtime_PrintString(tooltip, "%d %s", amount, NC2Logic::StringAccess_GetGoodNamePtr(good, amount));
                control.x -= 30; control.width = 28;
                allocation = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
                element = allocation == nullptr ? nullptr : BaseToolTextButton_Construct(allocation, nullptr, &control, graphic, 10102, window);
                Write<std::uint32_t>(element, 0x50, 0); Write<std::uint8_t>(element, 0x54, 0); Write(element, 0x4C, good);
                BaseToolTextButton_SetToolTipString(element, tooltip); Write<std::uint8_t>(element, 0x48, 1);
                BaseListBoxElement_Element_Add(list, nullptr, element);
            }
            control.x -= 22; control.width = 20;
            allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            element = allocation == nullptr ? nullptr : BaseToolGfxButton_ConstructWithToolTip(allocation, nullptr, &control, 143, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(10), 10101, window);
            Write(element, 0x48, entry.type); Write(element, 0x4C, entry.stage); Write<std::uint8_t>(element, 0x35, 1);
            BaseListBoxElement_Element_Add(list, nullptr, element);
            row.y += row.height + 2;
        }
        if (count == 0)
        {
            const NXBasics::SRectangle empty(0, 0, Read<std::int32_t>(list, 0xC), 28);
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(256, 1);
            void* element = allocation == nullptr ? nullptr : BaseToolTextElement_Construct(allocation, nullptr, &empty, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(12), false, false);
            BaseListBoxElement_Element_Add(list, nullptr, element);
        }
        NXBasics::Runtime_Free(entries);
        NC2InGameGuiBaseListBox::BaseListBoxElement_ScrollOffset_Set(list, 0, scroll);
        Write<std::uint32_t>(window, 0x78, timeGetTime());
    }
}

namespace NInGameGuiManager
{
    // Original function: FUN_004a24a4.
    void CInGameGuiManager::MiscWindows_ConstructionSelectionWindow_Toggle(std::uint32_t type, std::uint32_t item)
    {
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        auto* window = desktop->Message_SendToAllWindows(0x7DB, type, item, 0);
        if (window != nullptr) desktop->Window_AddToCloseList(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
        else MiscWindows_ConstructionSelectionWindow_Open(type, item);
    }

    // Original function: FUN_004a24e6; this window does not close other large windows.
    void CInGameGuiManager::MiscWindows_ConstructionSelectionWindow_Open(std::uint32_t type, std::uint32_t item)
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7DB, type, item, 0) != nullptr) return;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(936, 1);
        void* window = allocation == nullptr ? nullptr : NC2InGameGuiMiscWindows::ConstructionSelectionWindow_Construct(allocation, type, item);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }

    // Original function: FUN_004a2533.
    void CInGameGuiManager::MiscWindows_ConstructionSelectionWindow_Close()
    {
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        auto* window = desktop->Message_SendToAllWindows(0x7DA, 0, 0, 0);
        if (window != nullptr) desktop->Window_Close(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
    }

    // Original function: FUN_004a255a; sign-extend the Windows char parameter.
    bool CInGameGuiManager::MiscWindows_ConstructionSelectionWindow_SendKey(std::uint32_t key, char character)
    {
        void* window = NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7DA, 0, 0, 0);
        if (window == nullptr) return false;
        using TMessage = bool(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
        auto* methods = NC2Lifetime::Read<TMessage*>(window, 0);
        return methods[2](window, 0x800, 33331, key, static_cast<std::uint32_t>(static_cast<std::int32_t>(character)), 0);
    }
}
