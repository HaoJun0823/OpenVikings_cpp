#include "InGameGuiSupportAccess.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiBaseAccess.h"
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

        // Partial graphics-button path through the native dynamic-subbutton layout.
        // The complete shared handler methods remain unreconstructed.
        void MessageWindow_UpdateGfxButton(void* window, std::uint32_t command, std::uint32_t id, std::int32_t x, std::int32_t y, std::uint32_t parameter1, std::uint32_t parameter2)
        {
            auto* handler = static_cast<std::byte*>(window) + 0x74;
            std::byte* record = nullptr;
            for (std::uint32_t i = 0; i < 100; ++i)
            {
                auto* candidate = handler + 4u + i * 0x1Cu;
                if (Read<std::uint8_t>(candidate, 0) != 0 && Read<std::uint32_t>(candidate, 0x14) == command && Read<std::uint32_t>(candidate, 0x18) == id) { record = candidate; break; }
            }
            if (record == nullptr)
                for (std::uint32_t i = 0; i < 100; ++i) { auto* candidate = handler + 4u + i * 0x1Cu; if (Read<std::uint8_t>(candidate, 0) == 0) { record = candidate; break; } }
            if (record == nullptr) return;
            if (Read<std::uint8_t>(record, 0) == 0 || Read<std::int32_t>(record, 0xC) != x || Read<std::int32_t>(record, 0x10) != y)
            {
                if (Read<std::uint8_t>(record, 0) != 0) NXBaseGui::BaseWindow_Element_Delete(window, Read<void*>(record, 4));
                auto* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
                const NXBasics::SRectangle rectangle(x, y, 18, 16);
                auto* button = allocation == nullptr ? nullptr : NC2InGameGuiBase::BaseToolGfxButton_ConstructWithToolTip(allocation, nullptr, &rectangle, 143, nullptr, command, window);
                Write(record, 4, button); NXBaseGui::BaseWindow_Element_Add(window, button);
                Write<std::uint8_t>(record, 0, 1); Write(record, 0xC, x); Write(record, 0x10, y); Write(record, 0x14, command); Write(record, 0x18, id);
            }
            auto* button = Read<void*>(record, 4);
            if (button != nullptr) { Write(button, 0x48, parameter1); Write(button, 0x4C, parameter2); }
            auto* textButton = Read<void*>(record, 8);
            if (textButton != nullptr) { Write(textButton, 0x4C, parameter1); Write(textButton, 0x50, parameter2); }
        }

        // Original function: FUN_004d0975; queued closure clears the owner's pointer.
        void* __fastcall MessagePresentationWindow_Delete(void* window, void*, std::uint32_t flags)
        {
            Write(window, 0, gMessagePresentationWindowVirtualTable + 0);
            Write<void*>(gMessageManager, 0xC, nullptr);
            Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
            NXBaseGui::BaseWindow_Destruct(window);
            if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
            return window;
        }

        // Original function: FUN_004d0acf; informational context-window commands
        // 10200/10201 depend on the unfinished good/house help window.
        bool __fastcall MessagePresentationWindow_MessageHandle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t, std::uint32_t)
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
                    const auto* item = category == 0 ? NC2Logic::StringAccess_GetJobNamePtr(id, 1) : category == 1 ? NC2Logic::StringAccess_GetGoodNamePtr(id, 1) : category == 2 ? NC2Logic::StringAccess_GetHouseNamePtr(id, 1) : NC2Logic::StringAccess_GetVehicleTypeNamePtr(id);
                    if (category == 1)
                    {
                        MessageWindow_UpdateGfxButton(window, 10200, id, 160, y, id, 0);
                        NXBasics::Runtime_PrintStringN(text, 1000, "{gfxgood:%d,1}", id);
                        NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, 180, y, resources.font08, color, text);
                    }
                    // House-type context buttons need the engine-house/tribe
                    // correspondence and remain with the information-window block.
                    y += NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, category == 1 ? 210 : category == 2 ? 180 : 160, y, 436, resources.font08, color, item);
                    if (category == 1 || category == 2) y += 4;
                }
            }
        }

        // Complete 19-slot Windows virtual table; message content/context branches
        // listed above remain excluded from reconstruction completion counts.
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
            Write(window, 0x1C, Read<std::uint32_t>(window, 0x1C) | 0x20u);
            Write(window, 0, gMessagePresentationWindowVirtualTable + 0);
            std::memset(static_cast<std::byte*>(window) + 0x70, 0, 0xAF8);
            Write(window, 0x70, uniqueId); Write(window, 0x74, window);
            MessagePresentationWindow_l_GuiElements_Add(window);
            MessagePresentationWindow_l_Misc_Select(window);
            return window;
        }
    }

    // Original function: FUN_004b4247; one presentation window is reused.
    void CMessageManager::PresentationWindow_Open(std::uint32_t uniqueId)
    {
        auto* window = Read<void*>(this, 0xC);
        if (window != nullptr)
        {
            // Original functions: FUN_004d0ab0/004d09aa.
            if (Read<std::uint32_t>(window, 0x70) != uniqueId)
            {
                Write(window, 0x70, uniqueId); NXBaseGui::BaseWindow_Element_DeleteAll(window);
                std::memset(static_cast<std::byte*>(window) + 0x78, 0, 0xAF0);
                MessagePresentationWindow_l_GuiElements_Add(window);
            }
            MessagePresentationWindow_l_Misc_Select(window);
            return;
        }
        auto* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xB68, 1);
        window = allocation == nullptr ? nullptr : MessagePresentationWindow_Construct(allocation, uniqueId);
        Write(this, 0xC, window);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }
}
