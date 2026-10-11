#include "InGameGuiSupportAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiBaseAccess.h"
#include "InGameGuiSelectionWindowAccess.h"
#include "C2EngineDisplayAccess.h"
#include "InGameGuiManager.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2EngineObjectData.h"
#include "C2EngineDrawingAccess.h"
#include "C2StringAccess.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "XBitmap.h"
#include "XBobRendering.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XGuiWindowAccess.h"
#include "XMKeyManager.h"

#include <cstring>
#include <new>

namespace NC2InGameGuiMessages
{
    using namespace NC2Lifetime;
    namespace
    {
        extern const std::uintptr_t gMessagePresentationWindowVirtualTable[19];
        CMessageManager* Manager() { return static_cast<CMessageManager*>(gMessageManager); }

        // Original function: FUN_004d0ff3; Control suppresses only the camera jump.
        void MessagePresentationWindow_l_Misc_Select(void* window)
        {
            const auto* message = Manager()->Message_GetPtr(Read<std::uint32_t>(window, 0x70));
            if (message == nullptr) return;
            if ((Read<std::uint32_t>(NXSysKeyManager::CKeyManager::sTheObjectPtr, 0) & 2u) == 0)
            {
                std::uint32_t point = 0;
                if (SingleMessageDisplayed_GetMapPosition(message, &point)) NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(2004, point, 0, 0);
            }
            auto* input = NC2InGameGuiInputManager::gInputManager;
            if (Read<std::int32_t>(message, 0x18) != -1) NC2InGameGuiInputManager::Selection_SelectSingleHuman(input, Read<std::int32_t>(message, 0x18));
            if (Read<std::int32_t>(message, 0x20) != -1) NC2InGameGuiInputManager::Selection_SelectHouse(input, Read<std::int32_t>(message, 0x20));
            if (Read<std::int32_t>(message, 0x28) != -1) NC2InGameGuiInputManager::Selection_SelectSingleVehicle(input, Read<std::int32_t>(message, 0x28));
        }

        // Original function: FUN_004d09c3.
        void MessagePresentationWindow_l_GuiElements_Add(void* window)
        {
            using namespace NC2InGameGuiBase;
            auto* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xF8, 1);
            const NXBasics::SRectangle headline(2, 2, Read<std::int32_t>(window, 0xC) - 22, 14);
            auto* element = allocation == nullptr ? nullptr : WindowMoverButton_Construct(allocation, &headline, NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr(0));
            NXBaseGui::BaseWindow_Element_Add(window, element);
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            const NXBasics::SRectangle close(Read<std::int32_t>(window, 0xC) - 18, 2, 16, 14);
            element = allocation == nullptr ? nullptr : WindowCloserButton_Construct(allocation, nullptr, &close);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
            const NXBasics::SRectangle confirm(4, 214, 210, 22);
            element = allocation == nullptr ? nullptr : BaseToolTextButton_Construct(allocation, nullptr, &confirm, NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr(1), 10000, window);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            Manager()->Message_GetPtr(Read<std::uint32_t>(window, 0x70));
        }

        NC2InGameGuiBase::CDynamicSubButtonsHandler& Buttons(void* window) { return *std::launder(reinterpret_cast<NC2InGameGuiBase::CDynamicSubButtonsHandler*>(static_cast<std::byte*>(window) + 0x74)); }

        // Original function: FUN_004d09aa; remove controls before resetting the button registry.
        void MessagePresentationWindow_l_GuiElements_Update(void* window)
        {
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            Buttons(window).AllButtonsHaveBeenRemoved();
            MessagePresentationWindow_l_GuiElements_Add(window);
        }
        // Original function: FUN_004d0ab0; an unchanged message still performs selection.
        void MessagePresentationWindow_ChangeToMessage(void* window, std::uint32_t uniqueId)
        {
            if (Read<std::uint32_t>(window, 0x70) != uniqueId) { Write(window, 0x70, uniqueId); MessagePresentationWindow_l_GuiElements_Update(window); }
            MessagePresentationWindow_l_Misc_Select(window);
        }

        // Original function: FUN_004d0975; queued closure clears the owner's pointer.
        void* __fastcall MessagePresentationWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            Write(window, 0, gMessagePresentationWindowVirtualTable + 0);
            Manager()->PresentationWindow_Closed();
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
            NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }

        // Original function: FUN_004d0acf; all native informational context commands.
        bool __fastcall MessagePresentationWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t first, std::uint32_t second)
        {
            if (message == 0x20)
            {
                Manager()->Message_Remove(Read<std::uint32_t>(window, 0x70), true);
                NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
                return false;
            }
            if (message != 0x800) return false;
            if (command == 10000) Manager()->Message_Remove(Read<std::uint32_t>(window, 0x70), true);
            else if (command == 10100) MessagePresentationWindow_l_Misc_Select(window);
            else if (command == 10200) NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_HelpWindow_Open_GoodInfo(first);
            else if (command == 10201) NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_HelpWindow_Open_HouseInfo(first, second);
            return true;
        }

        // Original function: FUN_004d0b5e; the papyrus is printed into the root bitmap.
        void __fastcall MessagePresentationWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle*)
        {
            auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
            auto* root = bitmap;
            std::int32_t x = 0, y = 0;
            while (root->GetSourceBitmap() != nullptr) { x += root->GetSourceX(); y += root->GetSourceY(); root = root->GetSourceBitmap(); }
            NXBasics::BobManager_PrintBob(resources.windowBobManager, 70, root, x, y, resources.papyrusPalette);
            const auto* message = Manager()->Message_GetPtr(Read<std::uint32_t>(window, 0x70));
            if (message == nullptr) { NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window)); return; }
            const auto* color = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontDarkPalette) + 0x400);
            const auto humanId = Read<std::int32_t>(message, 0x18);
            const char* name = nullptr;
            if (humanId != -1)
            {
                name = reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries + static_cast<std::uint32_t>(humanId) * 0x8C8u + 0x85Cu);
                const auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanId) * NC2Logic::CHumanArray::ElementSize;
                NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(human, 0xC), bitmap, 30, 200, nullptr, true, -1);
            }
            char text[1036];
            const auto type = Read<std::uint32_t>(message, 8);
            if (type == 19 || type == 21)
            {
                const auto* item = type == 19 ? NC2Logic::StringAccess_GetGoodNamePtr(Read<std::uint32_t>(message, 0x38), 1) : NC2Logic::StringAccess_GetHouseNamePtr(Read<std::uint32_t>(message, 0x208), 1);
                NXBasics::Runtime_PrintStringN(text, 1000, "%s %s - %s", name, NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr(type == 19 ? 38 : 40), item);
                NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 4, 20, 436, resources.font12, color, text);
                return;
            }
            if (type != 18)
            {
                Tool_CreateMessageString(static_cast<const std::byte*>(message) + 8, text, 1000);
                NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 4, 20, 436, resources.font12, color, text);
                return;
            }
            NXBasics::Runtime_PrintStringN(text, 1000, "%s %s", name, NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr(33));
            y = NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 4, 20, 436, resources.font12, color, text) + 20;
            const std::uint32_t offsets[4]{0x140, 0x34, 0x204, 0x2E4};
            for (std::uint32_t category = 0; category < 4; ++category)
            {
                const auto offset = offsets[category], count = Read<std::uint32_t>(message, offset);
                if (count == 0) continue;
                y += 12;
                NXBasics::Runtime_PrintStringN(text, 1000, "%s:", NC2GuiToolsBase::StringTool_GetGameGuiMessagesStringPtr(34u + category));
                NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 4, y, 436, resources.font08, color, text);
                for (std::uint32_t i = 0; i < count; ++i)
                {
                    const auto id = Read<std::uint32_t>(message, offset + 4u + i * 4u);
                    if (category == 1)
                    {
                        Buttons(window).UpdateGfxButton(10200, id, 160, y, 143, nullptr);
                        Buttons(window).SetButtonMiscCommandData(10200, id, id, 0);
                        NXBasics::Runtime_PrintStringN(text, 1000, "{gfxgood:%d,1}", id);
                        NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, 180, y, resources.font08, color, text);
                    }
                    if (category == 2)
                    {
                        Buttons(window).UpdateGfxButton(10201, id, 160, y, 143, nullptr);
                        std::uint32_t stage = 0, tribe;
                        if (humanId != -1) tribe = Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars + static_cast<std::intptr_t>(humanId) * NC2Logic::CHumanArray::ElementSize, 0x14);
                        else { const auto player = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId(); tribe = player >= 20 ? 1u : Read<std::uint32_t>(NC2Logic::gGameControl, static_cast<std::uint32_t>(player) * 0x4A80u + 0xC); }
                        const auto engineType = NC2E2::Engine2d_EntryPoint_GetHouseId(tribe, id, &stage);
                        Buttons(window).SetButtonMiscCommandData(10201, id, engineType, stage);
                    }
                    const auto* item = category == 0 ? NC2Logic::StringAccess_GetJobNamePtr(id, 1) : category == 1 ? NC2Logic::StringAccess_GetGoodNamePtr(id, 1) : category == 2 ? NC2Logic::StringAccess_GetHouseNamePtr(id, 1) : NC2Logic::StringAccess_GetVehicleTypeNamePtr(id);
                    y += NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, category == 1 ? 210 : category == 2 ? 180 : 160, y, 436, resources.font08, color, item);
                    if (category == 1 || category == 2) y += 4;
                }
            }
        }

        // Complete 19-slot Windows virtual table with native content and context branches.
        const std::uintptr_t gMessagePresentationWindowVirtualTable[19]{
            reinterpret_cast<std::uintptr_t>(&MessagePresentationWindow_Delete),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
            reinterpret_cast<std::uintptr_t>(&MessagePresentationWindow_MessageHandle),
            reinterpret_cast<std::uintptr_t>(&MessagePresentationWindow_Draw),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_Activate),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_DeActivate),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_ButtonPressed),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_ButtonReleased),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_IsHit),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_GetRealActiveElementPtr),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_FindElementOnPosition),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ParentToLocalPosition),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_Available),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Hide_CanBeHidden),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_UnresolvedVirtual40),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_Notify),
            reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseWindow_DrawOverlays)
        };
        static_assert(sizeof(gMessagePresentationWindowVirtualTable) == 0x4C);

        // Original function: FUN_004d08eb.
        void* MessagePresentationWindow_Construct(void* window, std::uint32_t uniqueId)
        {
            const auto& vars = NInGameGuiManager::CInGameGuiManager::mStaticVars;
            const NXBasics::SRectangle rectangle((vars.displayWidth - 440) / 2, (vars.displayHeight - 240) / 2, 440, 240);
            NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
            new (static_cast<std::byte*>(window) + 0x74) NC2InGameGuiBase::CDynamicSubButtonsHandler;
            Write(window, 0x1C, Read<std::uint32_t>(window, 0x1C) | 0x20u);
            Write(window, 0, gMessagePresentationWindowVirtualTable + 0);
            std::memset(static_cast<std::byte*>(window) + 0x70, 0, 0xAF8);
            Write(window, 0x70, uniqueId); Buttons(window).SetWindow(window);
            MessagePresentationWindow_l_GuiElements_Add(window);
            MessagePresentationWindow_l_Misc_Select(window);
            return window;
        }
    }

    // Original function: FUN_004b428b.
    void CMessageManager::PresentationWindow_Closed() { Write<void*>(this, 0xC, nullptr); }

    // Original function: FUN_004b4247; one presentation window is reused.
    void CMessageManager::PresentationWindow_Open(std::uint32_t uniqueId)
    {
        auto* window = Read<void*>(this, 0xC);
        if (window != nullptr)
        {
            MessagePresentationWindow_ChangeToMessage(window, uniqueId);
            return;
        }
        auto* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xB68, 1);
        window = allocation == nullptr ? nullptr : MessagePresentationWindow_Construct(allocation, uniqueId);
        Write(this, 0xC, window);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }
}
