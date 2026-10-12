#include "InGameGuiOptionsWindowAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiBaseAccess.h"
#include "C2GameEngineStartup.h"
#include "C2GameState.h"
#include "C2GameTime.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2UserNetworkCommandAccess.h"
#include "C2SavegamesManager.h"
#include "C2SavegameIO.h"
#include "C2GameControlNetwork.h"
#include "C2CleanMapsManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2E2LevelOfDetailManager.h"
#include "C2EngineLifetimeAccess.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "ApplicationGlobals.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "DSManager.h"
#include "DMManager.h"
#include "CDAudioManager.h"
#include "GuiListBoxElementAccess.h"
#include "X2DApplication.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiClockToolElementAccess.h"
#include "GuiTextToolElementAccess.h"
#include "GuiToolsBase.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"
#include "XGuiDesktop.h"
#include "XGuiWindowAccess.h"

#include <cstring>

namespace NC2InGameGuiMiscWindows
{
    using namespace NC2Lifetime;
    static void* __fastcall OptionsWindow_Delete(void* window, void* unusedEdx, std::uint32_t flags);
    static void OptionsWindow_Destruct(void* window);

    // Complete Windows table: 004F73FC..004F7447.
    static const std::uintptr_t gOptionsWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&OptionsWindow_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&OptionsWindow_XGui_BE_Message_Handle),
        reinterpret_cast<std::uintptr_t>(&NC2InGameGuiBase::BaseToolWindow_Draw),
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
    static_assert(sizeof(gOptionsWindowVirtualTable) == 0x4C);

    // Original function: FUN_004bf96f; the original object allocation is 0x78 bytes.
    void* OptionsWindow_Construct(void* window, bool loadPage, bool savePage)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayWidth) - 500u) / 2, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayHeight) - 420u) / 2, 500, 420);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        Write(window, 0, gOptionsWindowVirtualTable + 0);
        std::memset(static_cast<std::byte*>(window) + 0x70, 0, 8);
        Write<std::uint32_t>(window, 0x70, 0);
        if (loadPage) Write<std::uint32_t>(window, 0x70, 4);
        if (savePage) Write<std::uint32_t>(window, 0x70, 5);
        OptionsWindow_l_GuiButtons_Add(window);
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        manager->FrameCall_Register(window);
        manager->Misc_GameTime_Stop_LargeWindowOpened();
        return window;
    }

    // Original function: FUN_004bfa0a; preserve the separate destruction boundary.
    static void* __fastcall OptionsWindow_Delete(void* window, void*, std::uint32_t flags)
    {
        OptionsWindow_Destruct(window);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(window);
        return window;
    }

    // Original function: FUN_004bfa26.
    static void OptionsWindow_Destruct(void* window)
    {
        Write(window, 0, gOptionsWindowVirtualTable + 0);
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        manager->Misc_GameTime_Continue_LargeWindowClosed();
        manager->FrameCall_UnRegister(window);
        Write(window, 0, NC2InGameGuiBase::gBaseToolWindowVirtualTable + 0);
        NXBaseGui::BaseWindow_Destruct(window);
    }

    // Original function: FUN_004c00e1; all six page bodies and the common window controls.
    void __fastcall OptionsWindow_l_GuiButtons_Add(void* window)
    {
        using namespace NC2InGameGuiBase;
        const auto addButton = [window](const NXBasics::SRectangle& rectangle, std::uint32_t stringId, std::uint32_t command, bool toggle, bool selected)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
            void* button = nullptr;
            if (allocation != nullptr)
            {
                const char* text = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(stringId);
                button = toggle ? BaseToolTextButton_ConstructToggle(allocation, nullptr, &rectangle, text, command, window, selected) : BaseToolTextButton_Construct(allocation, nullptr, &rectangle, text, command, window);
            }
            NXBaseGui::BaseWindow_Element_Add(window, button);
        };

        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xF8, 1);
        void* element = nullptr;
        if (allocation != nullptr)
        {
            const char* title = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x1E);
            const NXBasics::SRectangle rectangle(4, 2, NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, 0xC) - 0x1Cu), 14);
            element = WindowMoverButton_Construct(allocation, &rectangle, title);
        }
        NXBaseGui::BaseWindow_Element_Add(window, element);
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        element = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(window, 0xC) - 0x14u), 2, 16, 14);
            element = WindowCloserButton_Construct(allocation, nullptr, &rectangle);
        }
        NXBaseGui::BaseWindow_Element_Add(window, element);
        Write<std::uint32_t>(window, 0x74, 0xFFFFFFFFu);
        NXBasics::SRectangle tabRectangle(4, 22, 492 / 6 - 2, 22);
        for (std::uint32_t tab = 0; tab < 6; ++tab)
        {
            addButton(tabRectangle, 0x1Fu + tab, 0x2904u + tab, true, Read<std::uint32_t>(window, 0x70) == tab);
            tabRectangle.x += tabRectangle.width + 2;
        }

        const auto makeButton = [window](const NXBasics::SRectangle& rectangle, std::uint32_t stringId, std::uint32_t command, bool toggle, bool selected, const char* caption = nullptr, const char* tooltip = nullptr, std::uint32_t tooltipId = 0) -> void*
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
            if (allocation == nullptr) return nullptr;
            if (tooltipId != 0) tooltip = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(tooltipId);
            if (caption == nullptr) caption = NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(stringId);
            if (toggle && tooltip != nullptr) return BaseToolTextButton_ConstructToggleWithToolTip(allocation, nullptr, &rectangle, caption, tooltip, command, window, selected);
            if (toggle) return BaseToolTextButton_ConstructToggle(allocation, nullptr, &rectangle, caption, command, window, selected);
            if (tooltip != nullptr) return BaseToolTextButton_ConstructWithToolTip(allocation, nullptr, &rectangle, caption, tooltip, command, window);
            return BaseToolTextButton_Construct(allocation, nullptr, &rectangle, caption, command, window);
        };
        const auto text = [window](const NXBasics::SRectangle& rectangle, std::uint32_t stringId, const char* caption = nullptr) -> void*
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x100, 1);
            void* element = allocation == nullptr ? nullptr : BaseToolTextElement_Construct(allocation, nullptr, &rectangle, caption == nullptr ? NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(stringId) : caption, false, false);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            return element;
        };
        const auto button = [&](const NXBasics::SRectangle& rectangle, std::uint32_t stringId, std::uint32_t command, bool toggle, bool selected, const char* caption = nullptr, const char* tooltip = nullptr, std::uint32_t tooltipId = 0)
        {
            NXBaseGui::BaseWindow_Element_Add(window, makeButton(rectangle, stringId, command, toggle, selected, caption, tooltip, tooltipId));
        };
        const auto listbox = [](const NXBasics::SRectangle& rectangle) -> void*
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x74, 1);
            return allocation == nullptr ? nullptr : ToolListBoxElement_Construct(allocation, nullptr, &rectangle);
        };
        const auto addList = [window](const NXBasics::SRectangle& rectangle, void* list)
        {
            NXBaseGui::BaseWindow_Element_Add(window, list);
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x3C, 1);
            const NXBasics::SRectangle controller(476, rectangle.y, 20, rectangle.height);
            void* element = allocation == nullptr ? nullptr : ToolListBoxControllerElement_Construct(allocation, nullptr, &controller, list, false, true);
            NXBaseGui::BaseWindow_Element_Add(window, element);
        };
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        auto* properties = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;
        switch (Read<std::uint32_t>(window, 0x70))
        {
        case 0:
        {
            NXBasics::SRectangle rectangle(104, 150, 292, 60);
            addButton(rectangle, 0x25, 0x32C8, false, false);
            rectangle.y += rectangle.height + 36;
            if (!NNetwork::Network_IsInSession()) addButton(rectangle, 0x26, 0x32C9, false, false);
            const auto difficulty = Read<std::uint32_t>(NC2Logic::gGameControl, 0x5D200);
            if (difficulty < 3)
            {
                const char* description = NC2GuiToolsBase::StringTool_GetOdin001StringPtr(0x1Bu + difficulty * 2u);
                if (description != nullptr)
                {
                    char caption[0x200];
                    NXBasics::Runtime_PrintString(caption, "%s: %s", NC2GuiToolsBase::StringTool_GetOdin001StringPtr(0x1A), description);
                    allocation = NXBasics::Runtime_AllocateWithNewHandler(0x100, 1);
                    element = nullptr;
                    if (allocation != nullptr)
                    {
                        const NXBasics::SRectangle rectangle(8, 400, 484, 16);
                        element = BaseToolTextElement_Construct(allocation, nullptr, &rectangle, caption, false, false);
                    }
                    Write<std::uint8_t>(element, 0xF8, 0);
                    Write<std::uint8_t>(element, 0xFB, 1);
                    NXBaseGui::BaseWindow_Element_Add(window, element);
                }
            }
            break;
        }
        case 1:
        {
            text({4, 70, 492, 14}, 0x28);
            const auto scroll = manager->Options_ScrollSpeed_Get();
            for (std::uint32_t index = 0; index < 3; ++index) button({4 + static_cast<std::int32_t>(index) * 164, 86, 162, 18}, 0x29 + index, 0x27D8 + index, true, scroll == index);
            text({4, 140, 492, 14}, 0x2C);
            button({4, 156, 244, 18}, 0x2D, 0x27EC, true, Read<std::uint8_t>(manager, 0xE2) != 0);
            button({250, 156, 244, 18}, 0x2E, 0x27ED, true, Read<std::uint8_t>(manager, 0xE3) != 0);
            text({4, 210, 492, 14}, 0x35);
            const bool tooltipsOff = Read<std::uint8_t>(manager, 0xE1) != 0;
            button({4, 226, 244, 18}, 0x36, 0x32D4, true, !tooltipsOff);
            button({250, 226, 244, 18}, 0x37, 0x32D5, true, tooltipsOff);
            break;
        }
        case 2:
        {
            text({4, 70, 492, 14}, 0x38);
            const NXBasics::SRectangle rectangle(4, 86, 468, 160);
            void* list = listbox(rectangle);
            NXBasics::SRectangle entry(0, 0, Read<std::int32_t>(list, 0xC) / 2 - 1, 22);
            auto* app = NXSys2DApp::CApplication::sTheObjectPtr;
            for (std::uint32_t index = 0; index < app->GetDisplayModeData().fullscreenDisplayModeCount; ++index)
            {
                const auto& mode = app->GetDisplayModeData().fullscreenDisplayModes[index];
                entry.x = static_cast<std::int32_t>(index & 1u) * (entry.width + 2);
                entry.y = static_cast<std::int32_t>(index >> 1) * (entry.height + 2);
                const bool selected = mode.width == static_cast<std::uint32_t>(app->GetDisplayWidth()) && mode.height == static_cast<std::uint32_t>(app->GetDisplayHeight()) && mode.depth == static_cast<std::uint32_t>(app->GetDisplayDepth());
                char caption[100];
                NXBasics::Runtime_PrintString(caption, "%d*%d/%d", mode.width, mode.height, mode.depth);
                BaseListBoxElement_Element_Add(list, nullptr, makeButton(entry, 0, 0x2710 + index, true, selected, caption));
            }
            addList(rectangle, list);
            text({4, 272, 492, 14}, 0x39);
            for (std::int32_t index = 0; index < 3; ++index) button({4 + index * 164, 288, 162, 18}, 0x3A + index, 0x2774 + index, true, NC2E2::CLevelOfDetailManager::sLevelOfDetail == index);
            text({4, 332, 492, 14}, 0x3D);
            const bool softwareMouse = Read<std::uint8_t>(manager, 0xE4) != 0;
            button({4, 348, 244, 18}, 0x3E, 0x277E, true, softwareMouse);
            button({250, 348, 244, 18}, 0x3F, 0x277F, true, !softwareMouse);
            break;
        }
        case 3:
        {
            std::int32_t y = 70;
            auto* sound = NDSManager::CManager::sTheObjectPtr;
            char caption[4];
            if (sound != nullptr)
            {
                text({4, y, 492, 14}, 0x41); y += 16;
                const auto quality = Read<std::uint32_t>(sound, 0xC);
                for (std::uint32_t index = 0; index < 3; ++index) button({4 + static_cast<std::int32_t>(index) * 70, y, 68, 18}, 0x42 + index, 0x2968 + index, true, quality == index);
                button({284, y, 68, 18}, 0x50, 0x296B, false, false, "-", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x50));
                NXBasics::Runtime_PrintString(caption, "%d", Read<std::uint32_t>(sound, 0x10));
                text({354, y, 68, 18}, 0, caption);
                button({424, y, 68, 18}, 0x51, 0x296C, false, false, "+", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x51));
                y += 34; text({4, y, 492, 14}, 0x45); y += 16;
                const bool jinglesOff = properties->Property_GetIntegerValue("fx_jingles_off") != 0;
                button({4, y, 244, 18}, 0x46, 0x296D, true, !jinglesOff);
                button({250, y, 244, 18}, 0x47, 0x296E, true, jinglesOff);
                y += 34;
            }
            text({4, y, 492, 14}, 0x48); y += 16;
            const auto musicMode = properties->Property_GetIntegerValue("music_mode");
            button({4, y, 162, 18}, 0x49, 0x2972, true, musicMode == 1);
            element = makeButton({168, y, 162, 18}, 0x4A, 0x2973, true, musicMode == 2);
            if (gDirectMusicManager == nullptr || !NC2E2::Tool_Sound_IsDMUsable()) Write<std::uint8_t>(element, 0x56, 1);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            element = makeButton({332, y, 162, 18}, 0x4B, 0x2974, true, musicMode == 3);
            if (NCDAudioManager::CCDAudioManager::sTheObjectPtr == nullptr || !NC2E2::Tool_Sound_IsCDAUsable()) Write<std::uint8_t>(element, 0x56, 1);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            y += 34;
            if (gDirectMusicManager != nullptr && musicMode == 2)
            {
                text({4, y, 492, 14}, 0x4C); y += 16;
                button({4, y, 162, 18}, 0x50, 0x29E0, false, false, "-", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x50));
                NXBasics::Runtime_PrintString(caption, "%d", NDMManager::MainVolume_Get(gDirectMusicManager));
                text({168, y, 162, 18}, 0, caption);
                button({332, y, 162, 18}, 0x51, 0x29E1, false, false, "+", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x51));
                y += 34;
            }
            auto* cd = NCDAudioManager::CCDAudioManager::sTheObjectPtr;
            if (cd != nullptr && musicMode == 3)
            {
                if (NMasterControlProgram::FindGameCDDrive() == 0 || !cd->Init(NMasterControlProgram::FindGameCDDrive()) || cd->NumberOfTracks() == 0)
                {
                    text({4, y, 492, 14}, 0x4F);
                    break;
                }
                text({4, y, 492, 14}, 0x4D); y += 16;
                const NXBasics::SRectangle rectangle(4, y, 468, 120);
                void* list = listbox(rectangle);
                NXBasics::SRectangle entry(0, 0, Read<std::int32_t>(list, 0xC), 22);
                for (std::uint32_t track = 2; track - 1u < cd->NumberOfTracks(); ++track)
                {
                    char name[100];
                    NXBasics::Runtime_PrintString(name, "#%d %s", track, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(track + 0x59));
                    element = makeButton(entry, 0, 0x2A30, false, false, name);
                    Write<std::uint32_t>(element, 0x50, 0); Write(element, 0x4C, track);
                    BaseListBoxElement_Element_Add(list, nullptr, element);
                    entry.y += entry.height + 2;
                }
                addList(rectangle, list); y += 126;
                button({4, y, 96, 18}, 0x4E, 0x2A31, false, false);
                button({200, y, 96, 18}, 0x50, 0x2A32, false, false, "-", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x50));
                NXBasics::Runtime_PrintString(caption, "%d", cd->GetVolume() * 100u / 0xFFFFu);
                text({298, y, 96, 18}, 0, caption);
                button({396, y, 96, 18}, 0x51, 0x2A33, false, false, "+", NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x51));
            }
            break;
        }
        case 4:
        {
            if (NC2Logic::gGameStaticVars.networkGame != 0) break;
            text({4, 70, 492, 14}, 0x6E);
            const std::uint32_t sortTypes[3] = {2, 1, 3};
            for (std::uint32_t index = 0; index < 3; ++index) button({4 + static_cast<std::int32_t>(index) * 164, 86, 162, 18}, 0x6F + index * 2, 0x2AF8 + index, true, NC2Logic::gSavegamesManager->mSortType == sortTypes[index], nullptr, nullptr, 0x70 + index * 2);
            const NXBasics::SRectangle rectangle(4, 120, 468, 260);
            void* list = listbox(rectangle);
            Tool_Options_FillSavegameListboxForLoading(list, window, 0x2B02, false, 0, false);
            addList(rectangle, list);
            break;
        }
        case 5:
        {
            text({4, 70, 492, 14}, 0x75);
            const NXBasics::SRectangle rectangle(4, 86, 468, 240);
            void* list = listbox(rectangle);
            NXBasics::SRectangle entry(0, 0, Read<std::int32_t>(list, 0xC), 22);
            NC2Logic::gSavegamesManager->SavegameList_Update(0, true);
            for (std::uint32_t slot = 1; slot <= 100; ++slot)
            {
                char filename[100], caption[344]{}, name[512];
                NXBasics::Runtime_PrintString(filename, "user_%3.3d", slot);
                auto* info = NC2Logic::gSavegamesManager->GetSavegameInfoPtrByFilename(filename);
                if (info != nullptr) { info->Tool_CreateSavegameButtonName(name); std::strcat(caption, name); }
                else { std::strcat(caption, " ("); std::strcat(caption, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x77)); std::strcat(caption, ")"); }
                element = makeButton(entry, 0, 0x2B03, false, false, caption);
                Write<std::uint32_t>(element, 0x50, 0); Write(element, 0x4C, slot); Write<std::uint8_t>(element, 0x57, 1);
                BaseListBoxElement_Element_Add(list, nullptr, element);
                entry.y += entry.height + 2;
            }
            addList(rectangle, list);
            text({4, 342, 492, 14}, 0x78);
            for (std::uint32_t index = 0; index < 4; ++index)
            {
                const auto interval = NC2Logic::cAutosaveOptionArray[index];
                char caption[100];
                if (interval == 0) std::strcpy(caption, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x79));
                else if (interval % 720u == 0) NXBasics::Runtime_PrintString(caption, "%d %s", interval / 720u, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(interval / 720u == 1 ? 0x7A : 0x7B));
                else NXBasics::Runtime_PrintString(caption, "%d %s", interval / 12u, NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x7C));
                button({4 + static_cast<std::int32_t>(index) * 123, 358, 121, 18}, 0, 0x28A0 + index, true, NC2Logic::gGameStaticVars.autosave.intervalTicks == interval, caption);
            }
            break;
        }
        }
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x50, 1);
        element = nullptr;
        if (allocation != nullptr)
        {
            const NXBasics::SRectangle rectangle(466, 386, 30, 30);
            element = ToolClockElement_Construct(allocation, &rectangle);
        }
        Write<std::uint8_t>(element, 0x30, 1);
        NXBaseGui::BaseWindow_Element_Add(window, element);
    }

    // Original function: FUN_004bfa55; the Windows entry consumes five stack DWORDs.
    bool __fastcall OptionsWindow_XGui_BE_Message_Handle(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t /*param3*/)
    {
        const auto rebuild = [window]() { NXBaseGui::BaseWindow_Element_DeleteAll(window); OptionsWindow_l_GuiButtons_Add(window); };
        if (message == 0x1000 && command == 0x7D8)
        {
            if (param1 != 0) { Write<std::uint32_t>(window, 0x70, 4); rebuild(); }
            if (param2 != 0) { Write<std::uint32_t>(window, 0x70, 5); rebuild(); }
            return true;
        }
        if (message != 0x800) return false;
        if (command >= 0x2710 && command < 0x2774)
        {
            const auto& mode = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayModeData().fullscreenDisplayModes[command - 0x2710];
            NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->DisplayResolution_SetDuringNextMainloop(static_cast<std::int32_t>(mode.width), static_cast<std::int32_t>(mode.height), mode.depth);
            return true;
        }
        if (command >= 0x283C && command < 0x28A0)
        {
            const auto speed = NC2Logic::cSpeedOptionArray[command - 0x283C];
            std::byte networkCommand[0x14]{};
            networkCommand[0] = std::byte{0x87};
            Write(networkCommand, 4, speed);
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, networkCommand, false, true);
            rebuild();
            return true;
        }
        if (command >= 0x28A0 && command < 0x2904)
        {
            NC2Logic::Autosave_Start(NC2Logic::cAutosaveOptionArray[command - 0x28A0]);
            Write<std::uint32_t>(window, 0x70, 5);
            rebuild();
            return true;
        }
        if (command >= 0x2904 && command < 0x2968)
        {
            Write(window, 0x70, command - 0x2904u);
            rebuild();
            return true;
        }
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        auto* properties = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;
        auto* sound = NDSManager::CManager::sTheObjectPtr;
        auto* cd = NCDAudioManager::CCDAudioManager::sTheObjectPtr;
        bool update = true;
        switch (command)
        {
        case 0x7EE:
            update = Read<std::uint32_t>(window, 0x74) != 0xFFFFFFFFu && Read<std::uint32_t>(window, 0x74) != static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(NC2Logic::gGameTime.speed));
            break;
        case 0x2774: NC2E2::CLevelOfDetailManager::SetLevelOfDetail(0); break;
        case 0x2775: NC2E2::CLevelOfDetailManager::SetLevelOfDetail(1); break;
        case 0x2776: NC2E2::CLevelOfDetailManager::SetLevelOfDetail(2); break;
        case 0x277E: manager->Options_Scrolling_SetSoftwareMouseFlag(true, true); break;
        case 0x277F: manager->Options_Scrolling_SetSoftwareMouseFlag(false, true); break;
        case 0x27D8: manager->Options_ScrollSpeed_Set(0, true); break;
        case 0x27D9: manager->Options_ScrollSpeed_Set(1, true); break;
        case 0x27DA: manager->Options_ScrollSpeed_Set(2, true); break;
        case 0x27EC: manager->Options_Scrolling_SetThirdMouseButtonScrollFlag(Read<std::uint8_t>(manager, 0xE2) == 0, true); break;
        case 0x27ED: manager->Options_Scrolling_SetBorderScrollFlag(Read<std::uint8_t>(manager, 0xE3) == 0, true); break;
        case 0x2968:
        case 0x2969:
        case 0x296A:
            if (sound != nullptr) { sound->Quality_Set(static_cast<std::int32_t>(command - 0x2968)); properties->Options_Save_InGame(); }
            break;
        case 0x296B:
        case 0x296C:
            if (sound != nullptr)
            {
                const auto volume = NXBasics::Runtime_TimeSigned32(Read<std::uint32_t>(sound, 0x10) + (command == 0x296B ? static_cast<std::uint32_t>(-10) : 10u));
                sound->MainVolume_Set(volume);
                properties->Options_Save_InGame();
                Write<std::uint32_t>(NC2E2::gE2AmbientSoundManager, 0xC, 0);
                NC2E2::AmbientSoundManager_UpdateAmbientSounds(NC2E2::gE2AmbientSoundManager, Read<void*>(Read<void*>(manager, 4), 0x30));
            }
            break;
        case 0x296D:
        case 0x296E:
            properties->Property_Add("fx_jingles_off", true, command == 0x296D ? 0 : 1, nullptr);
            properties->Options_Save_InGame();
            break;
        case 0x2972:
        case 0x2973:
        case 0x2974:
            properties->Property_Add("music_mode", true, static_cast<std::int32_t>(command - 0x2971), nullptr);
            properties->Options_Save_InGame();
            NC2E2::WorldMusic_Update(NC2Logic::gGameTime.tick);
            NC2E2::WorldMusic_UpdateCdTrack();
            break;
        case 0x29CC:
        case 0x29CD:
        case 0x29CE:
            if (gDirectMusicManager != nullptr) NDMManager::ProcessEvent(gDirectMusicManager, 10, command == 0x29CC ? 4 : command == 0x29CD ? 5 : 6, 2);
            break;
        case 0x29D6: NDSManager::CManager::Jingle_Play(2); break;
        case 0x29D7: NDSManager::CManager::Jingle_Play(1); break;
        case 0x29E0:
        case 0x29E1:
            if (gDirectMusicManager != nullptr)
            {
                NDMManager::MainVolume_Set(gDirectMusicManager, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(NDMManager::MainVolume_Get(gDirectMusicManager)) + (command == 0x29E0 ? static_cast<std::uint32_t>(-10) : 10u)));
                properties->Options_Save_InGame();
            }
            break;
        case 0x2A30:
            if (cd == nullptr) update = false;
            else cd->StartTrack(param2);
            break;
        case 0x2A31:
            if (cd == nullptr) update = false;
            else cd->StopTrack();
            break;
        case 0x2A32:
        case 0x2A33:
            if (cd == nullptr) update = false;
            else
            {
                auto percent = NXBasics::Runtime_TimeSigned32(cd->GetVolume() * 100u / 0xFFFFu + (command == 0x2A32 ? static_cast<std::uint32_t>(-5) : 5u));
                if (command == 0x2A32 && percent < 0) percent = 0;
                if (command == 0x2A33 && percent > 100) percent = 100;
                cd->SetVolume(static_cast<std::uint32_t>(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(percent) * 0xFFFFu) / 100));
            }
            break;
        case 0x2AF8: NC2Logic::gSavegamesManager->SortType_Set(2); break;
        case 0x2AF9: NC2Logic::gSavegamesManager->SortType_Set(1); break;
        case 0x2AFA: NC2Logic::gSavegamesManager->SortType_Set(3); break;
        case 0x2B02:
        {
            auto* info = NC2Logic::gSavegamesManager->GetSavegameInfoPtrByUniqueId(param2);
            if (info != nullptr)
            {
                NC2Logic::GameIO_Savegame_Load(info->filename, false);
                NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
                manager->MiscWindows_StringOutputWindow_PrintString(NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x82));
            }
            update = false;
            break;
        }
        case 0x2B03:
        {
            char filename[100];
            NXBasics::Runtime_PrintString(filename, "user_%3.3d", param2);
            NC2Logic::GameIO_Savegame_Save(filename, nullptr, NXBasics::Runtime_TimeSigned32(param2 + 1000u));
            NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
            manager->MiscWindows_StringOutputWindow_PrintString(NC2GuiToolsBase::StringTool_GetGameGuiMainStringPtr(0x83));
            update = false;
            break;
        }
        case 0x32C8: manager->MiscWindows_GameSafetyBoxWindow_Open(1); update = false; break;
        case 0x32C9:
            manager->MiscWindows_GameSafetyBoxWindow_Open(3);
            NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
            update = false;
            break;
        case 0x32D2: manager->Options_GuiMisc_SetExpertFlag(false, true); break;
        case 0x32D3: manager->Options_GuiMisc_SetExpertFlag(true, true); break;
        case 0x32D4: manager->Options_GuiMisc_SetTooltipsOffFlag(false, true); break;
        case 0x32D5: manager->Options_GuiMisc_SetTooltipsOffFlag(true, true); break;
        case 0x32DC: manager->Options_GuiMisc_SetMainMode(0, true); break;
        case 0x32DD: manager->Options_GuiMisc_SetMainMode(1, true); break;
        default: update = false; break;
        }
        if (update) rebuild();
        return true;
    }

    static bool __fastcall GameSafetyBoxWindow_Message(void* window, void* unusedEdx, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3);
    static void __fastcall GameSafetyBoxWindow_Draw(void* window, void* unusedEdx, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle);
    // Complete Windows table: 004F78B0..004F78FB.
    static const std::uintptr_t gGameSafetyBoxWindowVirtualTable[19] = {
        reinterpret_cast<std::uintptr_t>(&NC2InGameGuiBase::BaseToolWindow_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&GameSafetyBoxWindow_Message),
        reinterpret_cast<std::uintptr_t>(&GameSafetyBoxWindow_Draw),
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
    static_assert(sizeof(gGameSafetyBoxWindowVirtualTable) == 0x4C);

    // Original function: FUN_004c94c4.
    static void GameSafetyBoxWindow_l_GuiButtons_Add(void* window)
    {
        using namespace NC2InGameGuiBase;
        const auto mode = Read<std::uint32_t>(window, 0x70);
        char title[220]{};
        if (mode == 1 || mode == 2) std::strcpy(title, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x50));
        else if (mode == 3) std::strcpy(title, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x53));
        else return;
        if (mode == 2)
        {
            if (Read<std::uint8_t>(NC2Logic::gMissionManager, 4) != 0) { std::strcat(title, " "); std::strcat(title, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x51)); }
            if (Read<std::uint8_t>(NC2Logic::gMissionManager, 5) != 0) { std::strcat(title, " "); std::strcat(title, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x52)); }
        }
        if (*title == 0) return;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0xF8, 1);
        const NXBasics::SRectangle moverRectangle(2, 2, Read<std::int32_t>(window, 0xC) - 22, 14);
        void* element = allocation == nullptr ? nullptr : WindowMoverButton_Construct(allocation, &moverRectangle, title);
        NXBaseGui::BaseWindow_Element_Add(window, element);
        allocation = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        const NXBasics::SRectangle closeRectangle(Read<std::int32_t>(window, 0xC) - 18, 2, 16, 14);
        element = allocation == nullptr ? nullptr : WindowCloserButton_Construct(allocation, nullptr, &closeRectangle);
        NXBaseGui::BaseWindow_Element_Add(window, element);
        const bool restart = mode == 2 && Read<std::uint8_t>(NC2Logic::gMissionManager, 5) != 0;
        const auto count = restart ? 3 : 2;
        NXBasics::SRectangle rectangle(4, 20, 396 / count - 4, 116);
        for (std::int32_t index = 0; index < count; ++index)
        {
            allocation = NXBasics::Runtime_AllocateWithNewHandler(0x60, 1);
            element = allocation == nullptr ? nullptr : BaseToolTextButton_ConstructToggle(allocation, nullptr, &rectangle, NC2GuiToolsBase::StringTool_GetGameGuiMiscWindowStringPtr(0x54 + index), 0x2710 + index, window, false);
            NXBaseGui::BaseWindow_Element_Add(window, element);
            rectangle.x += rectangle.width + 4;
        }
    }

    // Original function: FUN_004c92a5; CGameSafetyBoxWindow has 0x74 bytes.
    static void* GameSafetyBoxWindow_Construct(void* window, std::uint32_t mode)
    {
        const auto& state = NInGameGuiManager::CInGameGuiManager::mStaticVars;
        const NXBasics::SRectangle rectangle(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayWidth) - 400u) / 2, NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(state.displayHeight) - 140u) / 2, 400, 140);
        NC2InGameGuiBase::BaseToolWindow_Construct(window, &rectangle);
        Write(window, 0, gGameSafetyBoxWindowVirtualTable + 0);
        Write<std::uint32_t>(window, 0x70, 0);
        Write(window, 0x70, mode);
        GameSafetyBoxWindow_l_GuiButtons_Add(window);
        return window;
    }

    // Original function: FUN_004c859d; preserve the separate inherited draw adapter.
    static void __fastcall GameSafetyBoxWindow_Draw(void* window, void*, NXBasics::CBitmap* bitmap, const NXBasics::SRectangle* rectangle)
    {
        NC2InGameGuiBase::BaseToolWindow_Draw(window, nullptr, bitmap, rectangle);
    }

    // Original function: FUN_004c9335; Windows additionally supports the fourth campaign.
    static bool __fastcall GameSafetyBoxWindow_Message(void* window, void*, std::uint32_t message, std::uint32_t command, std::uint32_t param1, std::uint32_t, std::uint32_t)
    {
        if (message == 0x1000 && command == 0x7EB)
        {
            if (Read<std::uint32_t>(window, 0x70) != param1)
            {
                Write(window, 0x70, param1);
                NXBaseGui::BaseWindow_Element_DeleteAll(window);
                GameSafetyBoxWindow_l_GuiButtons_Add(window);
            }
            return true;
        }
        if (message != 0x800) return false;
        if (command == 0x2712) NC2Logic::System_RestartCurrentMap();
        if (command == 0x2711 || command == 0x2712)
        {
            NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
            return true;
        }
        if (command != 0x2710) return true;
        const auto mode = Read<std::int32_t>(window, 0x70);
        if (mode == 3)
        {
            NC2Logic::System_RestartCurrentMap();
            NXBaseGui::CDesktop::sTheObjectPtr->Window_AddToCloseList(static_cast<NXBaseGui::CBaseWindow*>(window));
        }
        else if (mode > 0 && mode <= 2)
        {
            std::byte networkCommand[0x14]{};
            networkCommand[0] = std::byte{0x88};
            const auto player = Read<std::int32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0);
            Write(networkCommand, 4, player);
            Write<std::uint32_t>(networkCommand, 8, 0);
            Write<std::uint32_t>(networkCommand, 12, NC2Logic::GameControl_Player_IsNetworkPlayer(NC2Logic::gGameControl, player) ? 1 : 0);
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, networkCommand, true, false);
            if (NC2Logic::gGameStaticVars.mapType == 1)
            {
                auto* info = NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(NC2Logic::gGameStaticVars.mapGuid);
                if (info != nullptr)
                {
                    const char* properties[4] = {"start_campaign_1_screen", "start_campaign_2_screen", "start_campaign_3_screen", "start_campaign_4_screen"};
                    for (std::uint32_t index = 1; index <= 4; ++index)
                    {
                        if ((info->mapTypes & 2u) == 0) break;
                        if (info->campaignId == index) NC2LogicExternals::External_Misc_PropertyString_AddString(properties[index - 1], "");
                    }
                }
            }
            NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_ProgramState_ChangeNextTimePossible(NMasterControlProgram::TProgramState::MainMenu, false);
        }
        return true;
    }

}

namespace NInGameGuiManager
{
    using namespace NC2Lifetime;

    // Original function: FUN_004a1dbd.
    void CInGameGuiManager::MiscWindows_OptionsWindow_Toggle(bool loadPage, bool savePage)
    {
        auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
        auto* window = desktop->Message_SendToAllWindows(0x7D8, loadPage, savePage, 0);
        if (window != nullptr) desktop->Window_AddToCloseList(reinterpret_cast<NXBaseGui::CBaseWindow*>(window));
        else MiscWindows_OptionsWindow_Open(loadPage, savePage);
    }

    // Original function: FUN_004a1e03.
    void CInGameGuiManager::MiscWindows_OptionsWindow_Open(bool loadPage, bool savePage)
    {
        l_Selection_RemoveAllGuiElements();
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7D8, loadPage, savePage, 0) != nullptr) return;
        MiscWindows_CloseAllLargeOnes(true);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x78, 1);
        void* window = allocation == nullptr ? nullptr : NC2InGameGuiMiscWindows::OptionsWindow_Construct(allocation, loadPage, savePage);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }

    // Original function: FUN_004a2dc2; single-player pause is queued through the original command manager.
    void CInGameGuiManager::Misc_GameTime_Stop_LargeWindowOpened()
    {
        if (NNetwork::Network_IsInSession() || Read<std::uint8_t>(this, 0x198) != 0 || !(NC2Logic::gGameTime.speed >= 0.01)) return;
        Write<double>(this, 0x1A0, NC2Logic::gGameTime.maximizeSpeed != 0 ? 12.0 : NC2Logic::gGameTime.speed);
        NC2Logic::GameTime_MaximizeGameSpeed_SetFlag(false);
        Write<std::uint8_t>(this, 0x198, 1);
        std::byte command[0x14]{};
        command[0] = std::byte{0x87};
        NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, command, false, true);
    }

    // Original function: FUN_004a2e45; an unordered current speed follows the original restore branch.
    void CInGameGuiManager::Misc_GameTime_Continue_LargeWindowClosed()
    {
        if (Read<std::uint8_t>(this, 0x198) != 0 && !(NC2Logic::gGameTime.speed >= 0.01))
        {
            std::byte command[0x14]{};
            command[0] = std::byte{0x87};
            const auto speed = static_cast<std::uint32_t>(NXBasics::Runtime_TruncateDoubleToInt64(Read<double>(this, 0x1A0)));
            std::memcpy(command + 4, &speed, sizeof(speed));
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, command, false, true);
        }
        Write<std::uint8_t>(this, 0x198, 0);
    }
}

namespace NInGameGuiManager
{
    // Original function: FUN_004a2862; reuse an existing safety box through its query message.
    void CInGameGuiManager::MiscWindows_GameSafetyBoxWindow_Open(std::uint32_t mode)
    {
        if (NXBaseGui::CDesktop::sTheObjectPtr->Message_SendToAllWindows(0x7EB, mode, 0, 0) != nullptr) return;
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x74, 1);
        void* window = allocation == nullptr ? nullptr : NC2InGameGuiMiscWindows::GameSafetyBoxWindow_Construct(allocation, mode);
        NC2InGameGuiBase::BaseToolDesktop_AddWindow(static_cast<NXBaseGui::CBaseWindow*>(window), false);
    }
}
