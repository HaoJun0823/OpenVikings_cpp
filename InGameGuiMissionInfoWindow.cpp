#include "C2ObjectLayout.h"
#include "C2RuntimeConstants.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "InGameGuiMissionInfoWindow.h"
#include "InGameGuiManager.h"
#include "InGameGuiWindowState.h"
#include "InGameGuiBaseAccess.h"
#include "C2CleanMapsManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineDrawingAccess.h"
#include "C2GameState.h"
#include "C2IoHelper.h"
#include "C2LanguageTools.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MissionStartObjects.h"
#include "C2PlayerCenterPosition.h"
#include "C2StringIO.h"
#include "C2VehicleAccess.h"
#include "DSManager.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "HyperLinkTextSystem.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XBitmap.h"
#include "XPalette.h"
#include "XStringArray.h"
#include "XGuiWindowAccess.h"

#include <cstring>
#include <new>

extern void __fastcall Display_DE_MapMoveableToGlobalWorldPixelCoordinates(void* display, void* unusedEdx, std::uint32_t moveableId, std::int32_t* point);

namespace NC2InGameGuiMiscWindows
{
    using namespace NC2Lifetime;

    // Complete shared preview region 005691AC..005691B7.
    static NXBasics::CBitmap* lsUserCallbackHumanDisplayBitmapPtr = nullptr;
    static NXBasics::CBitmap* lsUserCallbackMapDisplayBitmapPtr = nullptr;
    static void* lsUserCallbackMapDisplayEnginePtr = nullptr;

    static NXBasics::SRectangle ReadRectangle(const void* object, std::uint32_t offset)
    {
        NXBasics::SRectangle rectangle(0, 0, 0, 0);
        std::memcpy(&rectangle, static_cast<const std::byte*>(object) + offset, sizeof(rectangle));
        return rectangle;
    }

    static void* __fastcall MissionInfoWindow_Delete(void* window, void* unusedEdx, std::uint32_t flags);
    static bool __fastcall MissionInfoWindow_XGui_BE_Message_Handle(void* window, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    static void __fastcall MissionInfoWindow_XGui_BE_Element_Draw(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static void __fastcall MissionInfoWindow_XGui_BW_DrawOverlays(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    static NXBasics::CBitmap* __cdecl ls_GetUserBitmapCallback(std::uint32_t type, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);

    // Complete Windows table 004F5258..004F52A3.
    static const std::uintptr_t gMissionInfoWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&MissionInfoWindow_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&MissionInfoWindow_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&MissionInfoWindow_XGui_BE_Element_Draw),
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
        reinterpret_cast<std::uintptr_t>(&MissionInfoWindow_XGui_BW_DrawOverlays)
    };
    static_assert(sizeof(gMissionInfoWindowVirtualTable) == 76);

    // Original function: FUN_004a491d; window allocation is 0x3B8 bytes.
    void* MissionInfoWindow_Construct(void* window, std::uint32_t briefingId, bool goalsPage)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayWidth) - 500u) / 2, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayHeight) - 420u) / 2, 500, 420);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        Write(window, NC2Layout::GuiBaseElementOffset::VirtualTable, gMissionInfoWindowVirtualTable + 0);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 840);
        Write<std::uint32_t>(window, 0x6C, 1);
        Write<std::uint32_t>(window, 0x70, goalsPage ? 1u : 0u);
        Write<std::int32_t>(window, 0x180, -1);
        MissionInfoWindow_l_Briefing_SetId(window, NXBasics::Runtime_TimeSigned32(briefingId), true);
        const auto depth = NXBasics::ConvertBitsToTDepth(state.displayDepth);
        if (lsUserCallbackHumanDisplayBitmapPtr == nullptr)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            lsUserCallbackHumanDisplayBitmapPtr = allocation == nullptr ? nullptr : ::new(allocation) NXBasics::CBitmap(50, 80, depth);
        }
        if (lsUserCallbackMapDisplayBitmapPtr == nullptr)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            lsUserCallbackMapDisplayBitmapPtr = allocation == nullptr ? nullptr : ::new(allocation) NXBasics::CBitmap(280, 220, depth);
        }
        if (lsUserCallbackMapDisplayEnginePtr == nullptr)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(2452, 1);
            lsUserCallbackMapDisplayEnginePtr = allocation == nullptr ? nullptr : NC2E2::Display_Construct(allocation, lsUserCallbackMapDisplayBitmapPtr, 0, nullptr);
            NC2E2::DE_ToDraw_SetState(lsUserCallbackMapDisplayEnginePtr, 4, false);
        }
        MissionInfoWindow_l_GuiButtons_Add(window, true);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Misc_GameTime_Stop_LargeWindowOpened();
        return window;
    }

    // Original function: FUN_004a5275; strings are independently owned by this window.
    void MissionInfoWindow_l_Briefing_FreeData(void* window)
    {
        if (Read<std::uint8_t>(window, 0x39C) != 0 && Read<void*>(window, 0x3A0) != nullptr)
        {
            DeleteStorable(Read<void*>(window, 0x3A0));
            Write<void*>(window, 0x3A0, nullptr);
        }
        Write<std::uint8_t>(window, 0x74, 0);
        Write<std::uint8_t>(window, 0x17C, 0);
        Write<std::uint8_t>(window, 0x39C, 0);
    }

    // Original function: FUN_004a5164.
    void MissionInfoWindow_l_Briefing_SetId(void* window, std::int32_t briefingId, bool recordHistory)
    {
        if (Read<std::uint8_t>(window, 0x17C) != 0 && Read<std::int32_t>(window, 0x180) == briefingId) return;
        MissionInfoWindow_l_Briefing_FreeData(window);
        const auto* info = NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(NC2Logic::gGameStaticVars.mapGuid);
        if (info == nullptr) return;
        auto* storage = static_cast<char*>(window);
        std::strcpy(storage + 0x75, info->mapRoot);
        Write<std::uint8_t>(window, 0x74, 1);
        Write(window, 0x180, briefingId);
        char pattern[260], fullFilename[260];
        NXBasics::Runtime_PrintString(pattern, "%s\\Text\\%%s\\Briefings\\", storage + 0x75);
        NC2Logic::LanguageTool_GetDirectoryName(pattern, storage + 0x184, "briefings.txt");
        NXBasics::Runtime_PrintString(storage + 0x288, "%4.4d.hlt", briefingId);
        NXBasics::Runtime_PrintString(fullFilename, "%s%s", storage + 0x184, storage + 0x288);
        if (NXBasics::CFile::FileSystem_Tool_FileExists(fullFilename, false)) Write<std::uint8_t>(window, 0x17C, 1);
        auto* strings = NC2Logic::StringIO_GetMissionStringArrayPtr(storage + 0x75);
        Write(window, 0x3A0, strings);
        if (strings != nullptr) Write<std::uint8_t>(window, 0x39C, 1);
        if (recordHistory && Read<std::uint8_t>(window, 0x17C) != 0) MissionInfoWindow_MissionBriefingHistory_Add(briefingId);
    }

    // Original function: FUN_004a4a8d; shared previews are released before base-window teardown.
    static void MissionInfoWindow_Destruct(void* window)
    {
        Write(window, NC2Layout::GuiBaseElementOffset::VirtualTable, gMissionInfoWindowVirtualTable + 0);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Misc_GameTime_Continue_LargeWindowClosed();
        MissionInfoWindow_l_Briefing_FreeData(window);
        if (NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_Stop(NC2Runtime::SoundTag::MissionBriefingVoice);
        if (lsUserCallbackMapDisplayEnginePtr != nullptr)
        {
            NC2E2::Display_Destruct(lsUserCallbackMapDisplayEnginePtr);
            NXBasics::Runtime_Free(lsUserCallbackMapDisplayEnginePtr);
            lsUserCallbackMapDisplayEnginePtr = nullptr;
        }
        DeleteStorable(lsUserCallbackMapDisplayBitmapPtr);
        lsUserCallbackMapDisplayBitmapPtr = nullptr;
        DeleteStorable(lsUserCallbackHumanDisplayBitmapPtr);
        lsUserCallbackHumanDisplayBitmapPtr = nullptr;
        Write(window, NC2Layout::GuiBaseElementOffset::VirtualTable, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
        NXBaseGui::BaseWindow_Destruct(window);
    }

    // Original function: FUN_004a4a71.
    static void* __fastcall MissionInfoWindow_Delete(void* window, void*, std::uint32_t flags)
    {
        MissionInfoWindow_Destruct(window);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
        return window;
    }

    // Original function: FUN_004a4b1c; every command in message 0x800 is acknowledged.
    static bool __fastcall MissionInfoWindow_XGui_BE_Message_Handle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        if (message == 0x1000) return command == 0x7E6;
        if (message != 0x800) return false;
        const auto stopSample = []() { if (NDSManager::CManager::sTheObjectPtr != nullptr) NDSManager::CManager::sTheObjectPtr->FireAndForget_Stop(NC2Runtime::SoundTag::MissionBriefingVoice); };
        if (command >= 10000u && command < 10100u)
        {
            Write(window, 0x70, command - 10000u);
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            MissionInfoWindow_l_GuiButtons_Add(window, false);
            stopSample();
        }
        else if (command == 50000u)
        {
            Write<std::uint32_t>(window, 0x70, param2 != 0 ? 1u : 0u);
            stopSample();
            MissionInfoWindow_l_Briefing_SetId(window, NXBasics::Runtime_TimeSigned32(param1), true);
            NXBaseGui::BaseWindow_Element_DeleteAll(window);
            MissionInfoWindow_l_GuiButtons_Add(window, false);
        }
        else if (command == 50001u) MissionInfoWindow_l_Briefing_FreeData(window);
        else if (command == 0x277E || command == 0x277F)
        {
            if (void* hlt = Read<void*>(window, 0x3B4))
            {
                HyperLinkTextGuiElementBase_SetAutoScrollSpeed(hlt, nullptr, 0.0f, 0);
                Read<NHyperLinkTextSystem::CDisplayManager*>(hlt, 0x34)->ScrollY(command == 0x277E ? -60 : 60);
            }
        }
        else if ((command == 0x2774 || command == 0x2775) && Read<std::uint8_t>(window, 0x17C) != 0)
        {
            for (std::uint32_t index = 0; index < gMissionBriefingHistory.count; ++index)
            {
                if (gMissionBriefingHistory.entries[index] != Read<std::int32_t>(window, 0x180)) continue;
                const auto next = command == 0x2774 ? index - 1u : index + 1u;
                if ((command == 0x2774 && index == 0) || next >= gMissionBriefingHistory.count) break;
                const auto briefingId = gMissionBriefingHistory.entries[next];
                if (briefingId == -1) break;
                stopSample();
                MissionInfoWindow_l_Briefing_SetId(window, briefingId, false);
                NXBaseGui::BaseWindow_Element_DeleteAll(window);
                MissionInfoWindow_l_GuiButtons_Add(window, false);
                break;
            }
        }
        return true;
    }

    // Original function: FUN_004a52b0.
    static const char* MissionInfoWindow_l_Goals_GetStringPtr(const void* window, std::uint32_t stringId)
    {
        if (Read<std::uint8_t>(window, 0x39C) == 0 || Read<void*>(window, 0x3A0) == nullptr) return "<GOAL STRINGS NOT LOADED>";
        const char* text = Read<NXBasics::CStringArray*>(window, 0x3A0)->GetStringPtr(stringId);
        return text == nullptr ? "<GOAL STRING DOES NOT EXIST>" : text;
    }

    // Original function: FUN_004a4ced.
    static void __fastcall MissionInfoWindow_XGui_BE_Element_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
        const auto page = Read<std::uint32_t>(window, 0x70);
        if (page == 2) { NC2GuiToolsBase::DrawTool_DrawFrame(bitmap, ReadRectangle(window, 0x3A4), 2); return; }
        if (page != 1) return;
        const auto& resources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;
        const auto* dark = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontDarkPalette) + 0x400);
        if (Read<std::uint8_t>(window, 0x39C) == 0)
        {
            const auto* red = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontRedPalette) + 0x400);
            bitmap->Text_PrintV(resources.font08, red, 10, 100, "NO GOAL STRING FILE FOUND!");
            return;
        }
        bitmap->Text_PrintV(resources.font08, dark, 20, 55, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x42));
        std::int32_t y = 80;
        const auto count = Read<std::int32_t>(NC2Logic::gMissionManager, NC2Layout::MissionManagerOffset::MissionCount);
        for (std::int32_t index = 0; index < count; ++index)
        {
            const auto* mission = static_cast<const std::byte*>(NC2Logic::gMissionManager) + NC2Layout::MissionManagerOffset::Missions + static_cast<std::uint32_t>(index) * NC2Layout::RecordSize::MissionDataUnsigned;
            const auto description = Read<std::uint32_t>(mission, NC2Layout::MissionDataOffset::DescriptionId);
            if (description == NXBasics::IntegerRange::UInt32Maximum || Read<std::uint8_t>(mission, NC2Layout::MissionDataOffset::VisibleFlag) == 0) continue;
            const char* text = MissionInfoWindow_l_Goals_GetStringPtr(window, description);
            NXBasics::SColorRGB color = *dark;
            if (*text == '@') { ++text; color = NXBasics::SColorRGB(0, 0, 0); }
            if (Read<std::uint8_t>(mission, NC2Layout::MissionDataOffset::FulfilledFlag) != 0 || Read<std::uint8_t>(mission, NC2Layout::MissionDataOffset::ActiveFlag) != 0)
            {
                bitmap->Text_PrintV(resources.font08, &color, 20, y, Read<std::uint8_t>(mission, NC2Layout::MissionDataOffset::FulfilledFlag) != 0 ? "X" : "o");
                y += NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 40, y, 500, resources.font08, &color, text) + 4;
            }
            else
            {
                const auto* dimmed = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(resources.fontDimmedPalette) + 0x2E8);
                y += NC2GuiToolsBase::DrawTool_PrintStringWrapping(bitmap, 40, y, 500, resources.font08, dimmed, text) + 4;
            }
        }
    }

    // Original function: FUN_004a52df; all three pages and common controls.
    void MissionInfoWindow_l_GuiButtons_Add(void* window, bool autoScroll)
    {
        using namespace NC2InGameGuiBase;
        const auto add = [window](void* element) { NXBaseGui::BaseWindow_Element_Add(window, element); };
        Write<void*>(window, 0x3B4, nullptr);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(248, 1);
        const NXBasics::SRectangle mover(2, 2, NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, NC2Layout::GuiBaseElementOffset::Width) - 22u), 14);
        add(allocation == nullptr ? nullptr : WindowMoverButton_Construct(allocation, &mover, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x3C)));
        allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
        const NXBasics::SRectangle closer(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, NC2Layout::GuiBaseElementOffset::Width) - 18u), 2, 16, 14);
        add(allocation == nullptr ? nullptr : WindowCloserButton_Construct(allocation, nullptr, &closer));
        NXBasics::SRectangle tab(4, 22, 164, 22);
        for (std::uint32_t index = 0; index < 3; ++index)
        {
            allocation = NXBasics::Runtime_AllocateWithNewHandler(96, 1);
            add(allocation == nullptr ? nullptr : BaseToolTextButton_ConstructToggle(allocation, nullptr, &tab, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x3D + index), 10000u + index, window, Read<std::uint32_t>(window, 0x70) == index));
            tab.x += tab.width + 2;
        }
        const auto gfx = [&](std::int32_t x, std::uint32_t graphic, std::uint32_t caption, std::uint32_t command)
        {
            allocation = NXBasics::Runtime_AllocateWithNewHandler(84, 1);
            const NXBasics::SRectangle rectangle(x, 358, 42, 58);
            void* button = allocation == nullptr ? nullptr : BaseToolGfxButton_ConstructWithToolTip(allocation, nullptr, &rectangle, graphic, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(caption), command, window);
            ToolGraphics_SetPalettePtr(button, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.iconsLeftPalette);
            add(button);
        };
        NXBasics::SRectangle content(4, 52, 492, 302);
        const auto page = Read<std::uint32_t>(window, 0x70);
        if (page == 0)
        {
            const bool historyButtons = gMissionBriefingHistory.count > 1;
            const auto currentId = Read<std::int32_t>(NC2Logic::gMissionManager, NC2Layout::MissionManagerOffset::CurrentBriefingId);
            if (currentId != -1 && Read<std::int32_t>(window, 0x180) == -1) MissionInfoWindow_l_Briefing_SetId(window, currentId, true);
            if (Read<std::uint8_t>(window, 0x17C) != 0)
            {
                allocation = NXBasics::Runtime_AllocateWithNewHandler(72, 1);
                auto* storage = static_cast<char*>(window);
                void* element = allocation == nullptr ? nullptr : HyperLinkTextGuiElementBase_Construct(allocation, nullptr, &content, 3, 0x100, storage + 0x184, storage + 0x288, storage + 0x38C, reinterpret_cast<void*>(&ls_GetUserBitmapCallback));
                Write<std::uint8_t>(window, 0x38C, 0);
                Write(window, 0x3B4, element);
                Write<std::uint8_t>(element, 0x3A, 0);
                if (autoScroll) HyperLinkTextGuiElementBase_SetAutoScrollSpeed(element, nullptr, 7.2f, 1000);
                Write<std::uint8_t>(element, 0x38, 1);
                add(element);
                gfx(212, 0x8A, 0x43, 0x277E);
                gfx(246, 0x8B, 0x44, 0x277F);
            }
            else
            {
                content.height = 22;
                allocation = NXBasics::Runtime_AllocateWithNewHandler(256, 1);
                add(allocation == nullptr ? nullptr : BaseToolTextElement_Construct(allocation, nullptr, &content, "NO BRIEFING DEFINED OR BRIEFING PATH/FILE NOT FOUND!", false, false));
            }
            if (historyButtons) { gfx(8, 0x8C, 0x40, 0x2774); gfx(450, 0x8D, 0x41, 0x2775); }
        }
        else if (page == 2)
        {
            Write(window, 0x3A4, content);
            content.Inflate(-5, -5);
            char directory[260];
            NC2Logic::LanguageTool_GetDirectoryName("data\\text\\%s\\hypertext\\history", directory, "index.hlt");
            allocation = NXBasics::Runtime_AllocateWithNewHandler(72, 1);
            void* element = allocation == nullptr ? nullptr : HyperLinkTextGuiElementBase_Construct(allocation, nullptr, &content, 3, 0x100, directory, "index.hlt", nullptr, nullptr);
            Write(window, 0x3B4, element);
            Write<std::uint8_t>(element, 0x3A, 0);
            add(element);
            gfx(212, 0x8A, 0x43, 0x277E);
            gfx(246, 0x8B, 0x44, 0x277F);
        }
    }

    // Original function: FUN_004a5844; userpicture/usericon callback.
    static NXBasics::CBitmap* __cdecl ls_GetUserBitmapCallback(std::uint32_t type, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        if (type == 0)
        {
            if (lsUserCallbackHumanDisplayBitmapPtr == nullptr) return nullptr;
            auto* bitmap = lsUserCallbackHumanDisplayBitmapPtr;
            bitmap->Fill(NXBasics::SColorRGB(196, 192, 159));
            const auto id = NC2Logic::MissionCheck_GetFirstHumanIdWithMissionCheckId(param1);
            if (id != -1) NC2E2::DrawAtom_MapMoveable_DoDraw(Read<std::uint32_t>(NC2Logic::GetHumanPtr(id), 0xC), bitmap, Read<std::int32_t>(bitmap, 0x10) / 2, Read<std::int32_t>(bitmap, 0x14) - 20, nullptr, true, -1);
            const auto rectangle = ReadRectangle(bitmap, 8);
            NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, bitmap, &rectangle, 2);
            return bitmap;
        }
        if ((type != 1 && type != 2) || lsUserCallbackMapDisplayBitmapPtr == nullptr || lsUserCallbackMapDisplayEnginePtr == nullptr) return nullptr;
        if (type == 1)
        {
            const std::uint32_t point = (param1 & NXBasics::IntegerRange::UInt16Maximum) | (param2 << 16);
            NC2E2::DE_SetWantedPositionToMapPoint(lsUserCallbackMapDisplayEnginePtr, &point);
        }
        else
        {
            const auto id = NC2Logic::MissionCheck_GetFirstHumanIdWithMissionCheckId(param1);
            if (id == -1) return nullptr;
            std::int32_t point[2];
            Display_DE_MapMoveableToGlobalWorldPixelCoordinates(lsUserCallbackMapDisplayEnginePtr, nullptr, Read<std::uint32_t>(NC2Logic::GetHumanPtr(id), 0xC), point);
            point[1] -= 25;
            NC2E2::DE_SetWantedPositionToPixelPoint(lsUserCallbackMapDisplayEnginePtr, point);
        }
        NC2E2::Display_UpdateDisplay(lsUserCallbackMapDisplayEnginePtr);
        const auto rectangle = ReadRectangle(lsUserCallbackMapDisplayBitmapPtr, 8);
        NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, lsUserCallbackMapDisplayBitmapPtr, &rectangle, 2);
        return lsUserCallbackMapDisplayBitmapPtr;
    }

    // Original function: FUN_004a4e87; onscreen callbacks are consumed only on the briefing page.
    static void __fastcall MissionInfoWindow_XGui_BW_DrawOverlays(void* window, void*, NXBasics::CBitmap*, const NXBasics::SRectangle*)
    {
        if (Read<std::uint32_t>(window, 0x70) != 0 || Read<std::uint8_t>(window, 0x38C) == 0) return;
        const auto command = Read<std::uint32_t>(window, 0x390);
        const auto argument = Read<std::uint32_t>(window, 0x394);
        if (command == 2000 && NDSManager::CManager::sTheObjectPtr != nullptr)
        {
            NDSManager::CManager::sTheObjectPtr->FireAndForget_Stop(NC2Runtime::SoundTag::MissionBriefingVoice);
            if (NDSManager::CManager::sTheObjectPtr != nullptr)
            {
                char pattern[272], filename[260];
                NXBasics::Runtime_PrintString(pattern, "%s\\SFX\\%%s\\%4.4d.wav", static_cast<char*>(window) + 0x75, argument);
                NC2Logic::LanguageTool_GetFilename(pattern, filename, false);
                NDSManager::CManager::sTheObjectPtr->FireAndForget_LoadAndPlay(filename, 100, NC2Runtime::SoundTag::MissionBriefingVoice, 1, false, false, 100);
            }
        }
        else if (command >= 1000 && command <= 1003 && Read<std::uint8_t>(window, 0x179) == 0)
        {
            std::uint32_t point = 0;
            if (command == 1000) point = (argument & NXBasics::IntegerRange::UInt16Maximum) | (Read<std::uint32_t>(window, 0x398) << 16);
            else
            {
                std::uint16_t sums[2]{};
                std::uint16_t count = 0;
                const auto first = command == 1001 ? NC2Logic::MissionCheck_GetFirstHumanIdWithMissionCheckId : command == 1002 ? NC2Logic::MissionCheck_GetFirstVehicleIdWithMissionCheckId : NC2Logic::MissionCheck_GetFirstHouseIdWithMissionCheckId;
                const auto next = command == 1001 ? NC2Logic::MissionCheck_GetNextHumanIdWithMissionCheckId : command == 1002 ? NC2Logic::MissionCheck_GetNextVehicleIdWithMissionCheckId : NC2Logic::MissionCheck_GetNextHouseIdWithMissionCheckId;
                for (auto id = first(argument); id != -1; id = next(argument, id))
                {
                    const void* position = command == 1001 ? NC2Logic::Human_GetMapPositionRef(NC2Logic::GetHumanPtr(id)) : command == 1002 ? NC2Logic::Vehicle_GetMapPositionRef(NC2Logic::CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CVehicleArray::ElementSize) : NC2Logic::CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHouseArray::ElementSize + 0x1C;
                    sums[0] = static_cast<std::uint16_t>(sums[0] + Read<std::uint16_t>(position, 0));
                    sums[1] = static_cast<std::uint16_t>(sums[1] + Read<std::uint16_t>(position, 2));
                    ++count;
                }
                const auto signedCount = static_cast<std::int16_t>(count);
                if (signedCount > 0)
                {
                    const auto x = static_cast<std::uint16_t>(static_cast<std::int16_t>(sums[0]) / signedCount);
                    const auto y = static_cast<std::uint16_t>(static_cast<std::int16_t>(sums[1]) / signedCount);
                    point = x | (static_cast<std::uint32_t>(y) << 16);
                    NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetToMapPosition(&point);
                }
                Write<std::uint8_t>(window, 0x179, 1);
                Write<std::uint8_t>(window, 0x38C, 0);
                return;
            }
            NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetToMapPosition(&point);
            Write<std::uint8_t>(window, 0x179, 1);
        }
        Write<std::uint8_t>(window, 0x38C, 0);
    }

    // Original function: FUN_004a59c2; unique chronological IDs, retaining the last fifty.
    void MissionInfoWindow_MissionBriefingHistory_Add(std::int32_t briefingId)
    {
        for (std::uint32_t index = 0; index < gMissionBriefingHistory.count; ++index) if (gMissionBriefingHistory.entries[index] == briefingId) return;
        if (gMissionBriefingHistory.count == 50)
        {
            std::memmove(gMissionBriefingHistory.entries, gMissionBriefingHistory.entries + 1, 49u * sizeof(std::int32_t));
            gMissionBriefingHistory.entries[49] = briefingId;
        }
        else gMissionBriefingHistory.entries[gMissionBriefingHistory.count++] = briefingId;
    }

    // Original function: FUN_004a5a16.
    void MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(SwitchIds::GuiSavegameChunk::MissionBriefingHistory, 1);
        io->IO_Memory_WriteBinary(&gMissionBriefingHistory, 0xCC);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004a5a43.
    void MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == SwitchIds::GuiSavegameChunk::MissionBriefingHistory && io->mReadHeader.version == 1) io->IO_File_ReadBinary(&gMissionBriefingHistory, 0xCC);
        else { io->IO_File_Chunk_Skip(); MissionInfoWindow_MissionBriefingHistory_Init(); }
    }
}
