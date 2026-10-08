#include "XGuiElementAccess.h"
#include "C2GameState.h"
#include "C2LifetimeGlobals.h"
#include "WorldDisplayElementAccess.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2AmbientSoundManagerAccess.h"
#include "C2EngineDisplayAccess.h"
#include "GuiToolsBase.h"
#include "GuiBitmapToolElementAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiInputAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2DefaultInteraction.h"
#include "C2HouseAccess.h"
#include "C2E2HumanSoundAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2UserNetworkCommandAccess.h"
#include "XMKeyManager.h"
#include "XMMouseManager.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"
#include "XPaletteAccess.h"
#include "XPictureAccess.h"
#include "XGuiDesktop.h"

#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <initializer_list>
#include <new>

extern void __fastcall Display_DE_MapMoveableToGlobalWorldPixelCoordinates(void* display, void* unusedEdx, std::uint32_t moveableId, std::int32_t* point);

namespace NC2InGameGuiWorldElement
{
    template<class T> static T WorldDisplayElement_ReadField(const void* object, std::size_t offset)
    {
        T value;
        std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
        return value;
    }

    template<typename T> static void WorldDisplayElement_WriteField(void* object, std::size_t offset, T value)
    {
        std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
    }

    // Original function: FUN_004bab5e.
    static void WorldDisplayElement_MouseMoved(void* element, std::int32_t x, std::int32_t y)
    {
        using namespace NC2Lifetime;
        std::int32_t point[2]{x, y};
        NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(element, point);
        auto* display = Read<void*>(element, 0x30);
        NC2E2::Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(display, point, true);
        if (Read<std::uint8_t>(element, 0x1F4) != 0) { Write(element, 0x200, point[0]); Write(element, 0x204, point[1]); return; }
        NC2E2::DisplayDescription_MakeValidWorldPixelPosition(display, point);
        std::uint32_t mapPoint = 0;
        NC2E2::DisplayDescription_WorldPixelToMapMigCoordinates(display, point, &mapPoint);
        NC2InGameGuiInputManager::Cursor_SetPosition(NC2InGameGuiInputManager::gInputManager, &mapPoint);
    }

    // Original function: FUN_004babe0; complete single/group human and vehicle preparation.
    static void WorldDisplayElement_CheckObjectUnderMouse(void* element)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(element, 0x34) == 0) return;
        auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr;
        std::int32_t point[2]{Read<std::int32_t>(mouse, 0), Read<std::int32_t>(mouse, 4)};
        NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(element, point);
        auto* display = Read<void*>(element, 0x30);
        NC2E2::Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(display, point, true);
        auto* target = static_cast<std::byte*>(element) + 0x38;
        NC2E2::DE_SearchObject_FindOnWorldPosition(display, point, target);
        Write<std::uint32_t>(display, 0xAC, 0);
        const auto localPlayer = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();
        if (Read<std::uint32_t>(target, 0) == 1)
        {
            const auto* hoveredHuman = NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(target, 8) * NC2Logic::CHumanArray::ElementSize;
            if (Read<std::int32_t>(hoveredHuman, 0x10) == localPlayer)
            {
                for (const auto offset : {0xB20u, 0xB18u, 0xB14u, 0xB24u})
                {
                    const auto id = Read<std::uint32_t>(hoveredHuman, offset);
                    if (id != 0xFFFFFFFFu) NC2E2::Display_DE_ObjectsToMark_AddSecondaryHuman(display, 2, id);
                }
                for (const auto offset : {0xB28u, 0xB34u})
                {
                    const auto id = Read<std::uint32_t>(hoveredHuman, offset);
                    if (id != 0xFFFFFFFFu) NC2E2::Display_DE_ObjectsToMark_AddSecondaryHouse(display, 2, id);
                }
            }
        }
        else if (Read<std::uint32_t>(target, 0) == 4)
        {
            const auto* house = NC2Logic::CHouseArray::mStaticVars + Read<std::uint32_t>(target, 0x28) * NC2Logic::CHouseArray::ElementSize;
            if (Read<std::int32_t>(house, 0xC) == localPlayer && NC2Logic::House_Inhabitants_GetNumberOfAttachedHumans(house) != 0)
                for (std::uint32_t i = 0; i < 20; ++i)
                {
                    const auto* slot = house + 0x640u + i * 0x10u;
                    if (Read<std::uint8_t>(slot, 0) != 0) NC2E2::Display_DE_ObjectsToMark_AddSecondaryHuman(display, 2, Read<std::uint32_t>(slot, 4));
                }
        }
        Write<std::uint32_t>(element, 0x1C4, 0); Write<std::uint32_t>(element, 0x1C8, 0);
        std::memset(static_cast<std::byte*>(element) + 0x1CC, 0, 0x14); std::memset(static_cast<std::byte*>(element) + 0x1E0, 0, 0x14);
        const auto* input = NC2InGameGuiInputManager::gInputManager;
        auto* command = static_cast<std::byte*>(element) + 0x1CC;
        auto* secondary = static_cast<std::byte*>(element) + 0x1E0;
        const auto modifier = []() { return (Read<std::uint32_t>(NXSysKeyManager::CKeyManager::sTheObjectPtr, 0) & 2u) != 0; };
        const auto clearGroup = [&](std::size_t offset) { Write<std::uint32_t>(element, offset, 0); std::memset(command, 0, 0x14); std::memset(secondary, 0, 0x14); };
        using namespace NC2InGameGuiInputManager;
        switch (Read<std::uint32_t>(input, 4))
        {
        case 1:
        {
            const auto id = Selection_GetSelectedSingleHumanId(input);
            if (Read<std::uint32_t>(target, 0) == 1 && Read<std::int32_t>(target, 8) == id) return;
            const auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize;
            Write(element, 0x1C4, NC2Logic::Human_UserControl_GetDefaultInteractionCommand_SingleHuman(human, target, command, secondary, modifier()));
            return;
        }
        case 2:
        {
            const auto count = Selection_GetSelectedMultipleHumanNumber(input);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = Selection_GetSelectedMultipleHumanId(input, i);
                if (Read<std::uint32_t>(target, 0) == 1 && id == Read<std::int32_t>(target, 8)) return;
            }
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = Selection_GetSelectedMultipleHumanId(input, i);
                const auto* human = NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize;
                if (!NC2Logic::Human_UserControl_GetDefaultInteractionCommand_MultipleHuman(human, reinterpret_cast<std::uint32_t*>(static_cast<std::byte*>(element) + 0x1C4), target, command, i, count, modifier())) { clearGroup(0x1C4); return; }
            }
            return;
        }
        case 4:
        {
            const auto id = Selection_GetSelectedSingleVehicleId(input);
            if (Read<std::uint32_t>(target, 0) == 3 && Read<std::int32_t>(target, 0x18) == id) return;
            const auto* vehicle = NC2Logic::CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CVehicleArray::ElementSize;
            Write(element, 0x1C8, NC2Logic::Vehicle_UserControl_GetDefaultInteractionCommand_SingleVehicle(vehicle, target, command, secondary, modifier()));
            return;
        }
        case 5:
        {
            const auto count = Selection_GetSelectedMultipleVehiclesNumber(input);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = Selection_GetSelectedMultipleVehicleId(input, i);
                if (Read<std::uint32_t>(target, 0) == 3 && id == Read<std::int32_t>(target, 0x18)) return;
            }
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = Selection_GetSelectedMultipleVehicleId(input, i);
                const auto* vehicle = NC2Logic::CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CVehicleArray::ElementSize;
                if (!NC2Logic::Vehicle_UserControl_GetDefaultInteractionCommand_MultipleVehicles(vehicle, reinterpret_cast<std::uint32_t*>(static_cast<std::byte*>(element) + 0x1C8), target, command, i, count, modifier())) { clearGroup(0x1C8); return; }
            }
            return;
        }
        default: return;
        }
    }

    // Original function: FUN_004bb0c1; complete prepared single/group human and vehicle dispatch.
    static bool WorldDisplayElement_DoDefaultAction(void* element)
    {
        using namespace NC2Lifetime;
        auto* command = static_cast<std::byte*>(element) + 0x1CC;
        if (Read<std::uint8_t>(command, 0) == 0) return false;
        if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 4) == 1)
        {
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, command, false, false);
            if (Read<std::uint8_t>(element, 0x1E0) != 0) NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, static_cast<std::byte*>(element) + 0x1E0, false, false);
            NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Read<std::int32_t>(command, 4));
            if (Read<std::uint8_t>(command, 0) == 32)
            {
                const auto address = reinterpret_cast<std::uintptr_t>(NC2Logic::CHouseArray::mStaticVars) + Read<std::uint32_t>(command, 0xC) * NC2Logic::CHouseArray::ElementSize;
                const auto* house = reinterpret_cast<const void*>(address);
                if (address != 0 && Read<std::uint32_t>(house, 8) == Read<std::uint32_t>(command, 0x10) && Read<std::uint32_t>(house, 0x14) == 38) NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_HumanLearnJobWindow_Open();
            }
        }
        else if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 4) == 2)
        {
            auto* input = NC2InGameGuiInputManager::gInputManager;
            const auto count = NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanNumber(input);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = NC2InGameGuiInputManager::Selection_GetSelectedMultipleHumanId(input, i);
                std::byte selectedCommand[0x14]; std::memcpy(selectedCommand, command, sizeof(selectedCommand));
                Write(selectedCommand, 4, id); Write(selectedCommand, 8, Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CHumanArray::ElementSize, 8));
                NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, selectedCommand, false, false);
                NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, id);
            }
        }
        else if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 4) == 4)
        {
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, command, false, false);
            if (Read<std::uint8_t>(element, 0x1E0) != 0) NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, static_cast<std::byte*>(element) + 0x1E0, false, false);
        }
        else if (Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 4) == 5)
        {
            auto* input = NC2InGameGuiInputManager::gInputManager;
            const auto count = NC2InGameGuiInputManager::Selection_GetSelectedMultipleVehiclesNumber(input);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                const auto id = NC2InGameGuiInputManager::Selection_GetSelectedMultipleVehicleId(input, i);
                std::byte selectedCommand[0x14]; std::memcpy(selectedCommand, command, sizeof(selectedCommand));
                Write(selectedCommand, 4, id); Write(selectedCommand, 8, Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars + static_cast<std::uint32_t>(id) * NC2Logic::CVehicleArray::ElementSize, 8));
                NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, selectedCommand, false, false);
            }
        }
        return true;
    }

    // Original function: FUN_004bb288.
    bool WorldDisplayElement_l_TryToBringUpActionButtonsForObjectUnderMouse(void* element)
    {
        using namespace NC2Lifetime;
        auto* input = NC2InGameGuiInputManager::gInputManager;
        const auto type = Read<std::uint32_t>(element, 0x38), selection = Read<std::uint32_t>(input, 4);
        const bool matches = (type == 1 && selection == 1 && NC2InGameGuiInputManager::Selection_GetSelectedSingleHumanId(input) == Read<std::int32_t>(element, 0x40)) || (type == 3 && selection == 4 && NC2InGameGuiInputManager::Selection_GetSelectedSingleVehicleId(input) == Read<std::int32_t>(element, 0x50)) || (type == 4 && selection == 6 && Read<std::int32_t>(input, 0xA78) == Read<std::int32_t>(element, 0x60));
        if (!matches) return false;
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Selection_ActionButtons_BringUp(); return true;
    }
    // Original function: FUN_004bb2f2.
    bool WorldDisplayElement_l_TryToSelectAndBringUpActionButtonsForObjectUnderMouse(void* element)
    {
        using namespace NC2Lifetime;
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        const auto type = Read<std::uint32_t>(element, 0x38);
        std::uint32_t player;
        if (type == 1) player = Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars, Read<std::uint32_t>(element, 0x40) * NC2Logic::CHumanArray::ElementSize + 0x10);
        else if (type == 3) player = Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars, Read<std::uint32_t>(element, 0x50) * NC2Logic::CVehicleArray::ElementSize + 0x1E0);
        else if (type == 4) player = Read<std::uint32_t>(NC2Logic::CHouseArray::mStaticVars, Read<std::uint32_t>(element, 0x60) * NC2Logic::CHouseArray::ElementSize + 0x0C);
        else return false;
        if (player != static_cast<std::uint32_t>(manager->GetLocalPlayerId()) || !NC2InGameGuiInputManager::Action_DoNormalSelect(NC2InGameGuiInputManager::gInputManager, static_cast<std::byte*>(element) + 0x38, true, false, false)) return false;
        manager->Selection_ActionButtons_BringUp(); NC2GuiToolsBase::FXTool_ClickConfirm(); return true;
    }
    // Original function: FUN_004ba3be; normal selection, double click, area drag,
    // human point orders, overview camera and wheel zoom. Specialized input
    // modes, selection windows and remaining camera commands stay unfinished.
    static bool __fastcall WorldDisplayElement_MessageHandle(void* element, void*, std::int32_t message, std::int32_t command, std::uint32_t param1, std::uint32_t param2, std::uint32_t param3)
    {
        using namespace NC2Lifetime;
        using namespace NC2InGameGuiInputManager;
        auto* input = gInputManager;
        auto* display = Read<void*>(element, 0x30);
        const auto keys = Read<std::uint32_t>(NXSysKeyManager::CKeyManager::sTheObjectPtr, 0);
        auto* target = static_cast<std::byte*>(element) + 0x38;
        const auto mainMode = Read<std::uint32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0xDC);
        if (message == 1 || message == 2)
        {
            // Use the modern adapter's event time for the native 250 ms gate.
            const auto doubleTime = Read<std::uint32_t>(element, 0x21C);
            if (doubleTime != 0 && NXSysMouseManager::CMouseManager::sTheObjectPtr->GetInputMessageTime() - doubleTime < 250u) return true;
        }
        if (message == 1 || message == 0x20)
        {
            const auto offset = message == 1 ? 0x20Cu : 0x214u;
            auto* overrideHandler = Read<void*>(element, offset);
            if (overrideHandler != nullptr)
            {
                using THandle = bool(__fastcall*)(void*, void*, std::int32_t, std::int32_t, std::uint32_t, std::uint32_t, std::uint32_t);
                const auto handle = Read<THandle>(Read<void*>(overrideHandler, 0), 8);
                if (handle(overrideHandler, nullptr, 0x800, Read<std::int32_t>(element, offset + 4), 0, 0, 0)) return true;
            }
        }
        if (message == 1)
        {
            if (Read<std::uint8_t>(element, 0x34) == 0 || Read<std::uint8_t>(display, 0x6C) == 0 || Read<std::int32_t>(input, 0) != 0) return true;
            Write<std::uint8_t>(element, 0x1F4, 1);
            std::int32_t point[2]{command, static_cast<std::int32_t>(param1)};
            NC2E2::Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(display, point, true);
            Write(element, 0x1F8, point[0]); Write(element, 0x1FC, point[1]); Write(element, 0x200, point[0]); Write(element, 0x204, point[1]);
            return true;
        }
        if (message == 2)
        {
            if (Read<std::uint8_t>(element, 0x34) == 0) return true;
            bool selectedArea = false;
            if (WorldDisplayElement_MouseSelectingArea_IsAreaValid(element))
            {
                NXBasics::SRectangle rectangle(Read<std::int32_t>(element, 0x1F8), Read<std::int32_t>(element, 0x1FC), Read<std::int32_t>(element, 0x200) - Read<std::int32_t>(element, 0x1F8), Read<std::int32_t>(element, 0x204) - Read<std::int32_t>(element, 0x1FC));
                selectedArea = Selection_SelectHumansInArea(input, element, rectangle, 0, (keys & 1u) != 0, (keys & 2u) != 0);
            }
            Write<std::uint8_t>(element, 0x1F4, 0);
            WorldDisplayElement_MouseMoved(element, command, static_cast<std::int32_t>(param1));
            if (selectedArea) return true;
            if (mainMode == 1)
            {
                if (Read<std::int32_t>(input, 0) != 0 && Action_DoCancel(input, target)) return true;
                if (Action_DoNormalSelect(input, target, false, (keys & 1u) != 0, (keys & 2u) != 0)) NC2GuiToolsBase::FXTool_ClickConfirm();
            }
            else if (mainMode == 0)
            {
                if (Action_DoInputSelect(input, target)) return true;
                if (Read<std::int32_t>(input, 0) != 0) return true;
                if (WorldDisplayElement_DoDefaultAction(element) || Action_DoDefault(input, target)) return true;
                if (Action_DoNormalSelect(input, target, true, (keys & 1u) != 0, (keys & 2u) != 0)) NC2GuiToolsBase::FXTool_ClickConfirm();
            }
            return true;
        }
        if (message == 0x20)
        {
            if (Read<std::uint8_t>(element, 0x34) == 0) return true;
            WorldDisplayElement_MouseMoved(element, command, static_cast<std::int32_t>(param1));
            if (mainMode == 1)
            {
                if (Action_DoInputSelect(input, target)) { NC2GuiToolsBase::FXTool_ClickConfirm(); return true; }
                if (Read<std::int32_t>(input, 0) != 0) return true;
                if (WorldDisplayElement_DoDefaultAction(element) || Action_DoDefault(input, target)) { NC2GuiToolsBase::FXTool_ClickConfirm(); return true; }
            }
            else if (mainMode == 0)
            {
                if (WorldDisplayElement_l_TryToBringUpActionButtonsForObjectUnderMouse(element)) return true;
                if (Action_DoCancel(input, target)) return true;
            }
            else return true;
            WorldDisplayElement_l_TryToSelectAndBringUpActionButtonsForObjectUnderMouse(element);
            return true;
        }
        if (message == 4)
        {
            Write(element, 0x21C, NXSysMouseManager::CMouseManager::sTheObjectPtr->GetInputMessageTime());
            if (Read<std::uint8_t>(element, 0x34) != 0 && Read<std::uint8_t>(display, 0x6C) != 0 && Read<std::uint32_t>(target, 0) == 1 && Read<std::uint32_t>(target, 0x40) == Read<std::uint32_t>(display, 0x7C) && Read<std::uint32_t>(input, 0) == 0 && (Selection_IsHumanSelected(input, Read<std::int32_t>(target, 8)) || Read<std::uint32_t>(input, 4) == 2))
            {
                const auto* human = NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(target, 8) * NC2Logic::CHumanArray::ElementSize;
                const auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr;
                std::int32_t point[2]{Read<std::int32_t>(mouse, 0), Read<std::int32_t>(mouse, 4)};
                NC2InGameGuiBase::BaseElement_GlobalToLocalCoordinates(element, point);
                NC2E2::Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(display, point, true);
                Selection_SelectHumansInArea(input, element, NXBasics::SRectangle(point[0] - 400, point[1] - 300, 800, 600), Read<std::uint32_t>(human, 0x18), (keys & 1u) != 0, (keys & 2u) != 0);
            }
            Write<std::uint8_t>(element, 0x1F4, 0);
            return true;
        }
        if (message == 0x80)
        {
            if (Read<std::uint8_t>(element, 0x34) == 0) return true;
            const auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr;
            const auto callback = Read<bool(__cdecl*)()>(mouse, 0x4C);
            if (callback != nullptr && callback())
            {
                const auto speed = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Options_ScrollSpeed_Get();
                NC2E2::DE_SetWantedPositionAddDeltaPixelPoint(display, static_cast<std::int32_t>(speed * param2) / 300, static_cast<std::int32_t>(speed * param3) / 300);
                WorldDisplayElement_FollowMode_End(element);
            }
            WorldDisplayElement_MouseMoved(element, command, static_cast<std::int32_t>(param1));
            return true;
        }
        if (message == 0x800)
        {
            if (command == 2030) { WorldDisplayElement_l_FollowMode_Update(element); WorldDisplayElement_CheckObjectUnderMouse(element); }
            else if (command == 2031)
            {
                NC2E2::DisplayObjectReference_Reset(target); Write<std::uint32_t>(element, 0x1C4, 0); Write<std::uint32_t>(element, 0x1C8, 0);
                std::memset(static_cast<std::byte*>(element) + 0x1CC, 0, 0x14); std::memset(static_cast<std::byte*>(element) + 0x1E0, 0, 0x14); Write<std::uint8_t>(element, 0x1F4, 0);
            }
            return true;
        }
        if (message == 0x100)
        {
            void* display = WorldDisplayElement_ReadField<void*>(element, 0x30);
            if (WorldDisplayElement_ReadField<std::uint8_t>(element, 0x34) != 0 && WorldDisplayElement_ReadField<std::uint8_t>(display, 0x6C) != 0)
            {
                const auto wheelDelta = NXBasics::Runtime_TimeSigned32(param2);
                if (wheelDelta > 0) NC2E2::DE_Zoom_In(display, false);
                else if (wheelDelta < 0) NC2E2::DE_Zoom_Out(display);
            }
            return true;
        }
        if (message != 0x1000 || command != 2004) return false;
        if (WorldDisplayElement_ReadField<std::uint8_t>(display, 0x6C) == 0 || (WorldDisplayElement_ReadField<std::uint32_t>(element, 0x18) & 0x200u) != 0) return false;
        std::int32_t point[2];
        NC2E2::Display_MapMigToLocalDisplayPixelCoordinates(display, &param1, point, false);
        NC2E2::Display_LocalDisplayPixelToGlobalWorldPixelCoordinates(display, point, false);
        NC2E2::DE_SetWantedPositionToPixelPoint(display, point);
        WorldDisplayElement_FollowMode_End(element);
        // The original command branch changes the camera and still returns false.
        return false;
    }

    // Original virtual entry: FUN_004ba124.
    static void* __fastcall WorldDisplayElement_DeletingDestructor(void* element, void* unusedEdx, std::uint32_t flags)
    {
        (void)unusedEdx;
        WorldDisplayElement_Destruct(element);
        if ((flags & 1u) != 0)
            NXBasics::Runtime_Free(element);
        return element;
    }

    // ABI adapter for the reconstructed virtual draw method, FUN_004ba1ba.
    static void __fastcall WorldDisplayElement_VirtualDraw(void* element, void* unusedEdx, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        (void)unusedEdx;
        WorldDisplayElement_Draw(element, bitmap, rectangle);
    }

    // Complete 15-slot table recovered from Game.exe at 004F6FAC..004F6FE7.
    // Destruction, drawing, wheel zoom and the overview camera command use reconstructed
    // bodies. Other world-input dispatcher branches remain unfinished.
    static const std::uintptr_t gWorldDisplayElementVirtualTable[] =
    {
        reinterpret_cast<std::uintptr_t>(&WorldDisplayElement_DeletingDestructor),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&WorldDisplayElement_MessageHandle),
        reinterpret_cast<std::uintptr_t>(&WorldDisplayElement_VirtualDraw),
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
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_ToolTip_AtOnce)
    };
    static_assert(sizeof(gWorldDisplayElementVirtualTable) == 0x3C);

    // Original function: FUN_004ba140; initializes the state subobjects at +0x30.
    static void* WorldDisplayElement_ConstructState(void* state)
    {
        WorldDisplayElement_WriteField<std::uint16_t>(state, 0x0E, 0);
        WorldDisplayElement_WriteField<std::uint16_t>(state, 0x0C, 0);
        std::memset(static_cast<std::byte*>(state) + 0x19C, 0, 0x14);
        std::memset(static_cast<std::byte*>(state) + 0x1B0, 0, 0x14);
        return state;
    }

    // Original functions: FUN_004baffa/FUN_004bb007.
    void WorldDisplayElement_DrawFrame_SetMode(void* element, std::uint32_t mode) { NC2Lifetime::Write(element, 0x208, mode); }
    void WorldDisplayElement_Misc_LeftMouseClick_HandlerSet(void* element, void* handler, std::uint32_t command) { NC2Lifetime::Write(element, 0x20C, handler); NC2Lifetime::Write(element, 0x210, command); }
    // Original function: FUN_004bb01e.
    void WorldDisplayElement_Misc_LeftMouseDoubleClick_HandlerSet(void* element, void* handler, std::uint32_t command) { NC2Lifetime::Write(element, 0x214, handler); NC2Lifetime::Write(element, 0x218, command); }

    // Original function: FUN_004ba042.
    void* WorldDisplayElement_Construct(void* element, const NXBasics::SRectangle* rectangle, bool active, bool inputEnabled)
    {
        auto* desktopBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();
        const std::int32_t depth = WorldDisplayElement_ReadField<std::int32_t>(desktopBitmap, 0x04);
        NC2InGameGuiBase::BaseBitmapToolElement_Construct(element, rectangle, 1, 0x1A7, depth);
        auto* state = static_cast<std::byte*>(element) + 0x30;
        WorldDisplayElement_ConstructState(state);
        WorldDisplayElement_WriteField(element, 0x00, gWorldDisplayElementVirtualTable + 0);
        std::memset(state, 0, 0x214);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x994, 1);
        void* display = nullptr;
        if (allocation != nullptr)
        {
            auto* font = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.font08;
            auto* bitmap = WorldDisplayElement_ReadField<NXBasics::CBitmap*>(element, 0x2C);
            display = NC2E2::Display_Construct(allocation, bitmap, static_cast<std::uint8_t>(active), font);
        }
        WorldDisplayElement_WriteField(element, 0x30, display);
        WorldDisplayElement_WriteField<std::uint8_t>(element, 0x34, static_cast<std::uint8_t>(inputEnabled));
        auto* interactionTarget = static_cast<std::byte*>(element) + 0x38;
        std::memset(interactionTarget, 0, 0x18C);
        NC2E2::DisplayObjectReference_Reset(interactionTarget);
        if (active)
        {
            WorldDisplayElement_WriteField<std::uint8_t>(display, 0xA4, interactionTarget != nullptr ? 1 : 0);
            WorldDisplayElement_WriteField(display, 0xA8, interactionTarget);
        }
        else
        {
            const bool explored = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->Exploration_GetFlag();
            NC2E2::DE_ToDraw_SetState(display, 4, explored);
        }
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->EngineWorldDisplayGuiElement_Register(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_Register(element);
        WorldDisplayElement_WriteField<std::uint32_t>(element, 0x208, 0);
        return element;
    }

    // Original function: FUN_004ba174.
    void WorldDisplayElement_Destruct(void* element)
    {
        WorldDisplayElement_WriteField(element, 0x00, gWorldDisplayElementVirtualTable + 0);
        WorldDisplayElement_DarkenBitmap_Exit(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->FrameCall_UnRegister(element);
        NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->EngineWorldDisplayGuiElement_UnRegister(element);
        void* display = WorldDisplayElement_ReadField<void*>(element, 0x30);
        if (display != nullptr)
        {
            NC2E2::Display_Destruct(display);
            NXBasics::Runtime_Free(display);
        }
        NC2InGameGuiBase::BaseBitmapToolElement_Destruct(element);
    }

    // Original function: FUN_004bb388.
    void WorldDisplayElement_FollowMode_FollowHuman(void* element, std::int32_t id)
    {
        using namespace NC2Lifetime;
        std::memset(static_cast<std::byte*>(element) + 0x220, 0, 0x18);
        Write(element, 0x224, id); Write<std::uint8_t>(element, 0x220, 1); Write<std::uint8_t>(element, 0x221, 1);
        Write(element, 0x228, Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CHumanArray::ElementSize, 8));
        WorldDisplayElement_l_FollowMode_Update(element);
    }
    // Original function: FUN_004bb3d5; validate the revision before reporting the id.
    std::int32_t WorldDisplayElement_FollowMode_GetHumanId(void* element)
    {
        WorldDisplayElement_l_FollowMode_Validate(element);
        return NC2Lifetime::Read<std::uint8_t>(element, 0x220) != 0 && NC2Lifetime::Read<std::uint8_t>(element, 0x221) != 0 ? NC2Lifetime::Read<std::int32_t>(element, 0x224) : -1;
    }
    // Original function: FUN_004bb3fc.
    void WorldDisplayElement_FollowMode_FollowVehicle(void* element, std::int32_t id)
    {
        using namespace NC2Lifetime;
        std::memset(static_cast<std::byte*>(element) + 0x220, 0, 0x18);
        Write(element, 0x230, id); Write<std::uint8_t>(element, 0x220, 1); Write<std::uint8_t>(element, 0x22C, 1);
        Write(element, 0x234, Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars + static_cast<std::intptr_t>(id) * NC2Logic::CVehicleArray::ElementSize, 8));
        WorldDisplayElement_l_FollowMode_Update(element);
    }
    // Original function: FUN_004bb449.
    std::int32_t WorldDisplayElement_FollowMode_GetVehicleId(void* element)
    {
        WorldDisplayElement_l_FollowMode_Validate(element);
        return NC2Lifetime::Read<std::uint8_t>(element, 0x220) != 0 && NC2Lifetime::Read<std::uint8_t>(element, 0x22C) != 0 ? NC2Lifetime::Read<std::int32_t>(element, 0x230) : -1;
    }
    // Original function: FUN_004bb470; CWorldDisplayElement::FollowMode_End.
    void WorldDisplayElement_FollowMode_End(void* element)
    {
        std::memset(static_cast<std::byte*>(element) + 0x220, 0, 0x18);
    }

    // Original function: FUN_004bb484; both modes share the 0x18-byte state.
    void WorldDisplayElement_l_FollowMode_Validate(void* element)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(element, 0x220) == 0) return;
        const void* object = nullptr;
        std::uint32_t revision = 0;
        if (Read<std::uint8_t>(element, 0x221) != 0)
        {
            object = NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(element, 0x224) * NC2Logic::CHumanArray::ElementSize; revision = Read<std::uint32_t>(element, 0x228);
        }
        else if (Read<std::uint8_t>(element, 0x22C) != 0)
        {
            object = NC2Logic::CVehicleArray::mStaticVars + Read<std::uint32_t>(element, 0x230) * NC2Logic::CVehicleArray::ElementSize; revision = Read<std::uint32_t>(element, 0x234);
        }
        if (object == nullptr || Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) WorldDisplayElement_FollowMode_End(element);
    }
    // Original function: FUN_004bb4f1; use the map-moveable id, including vehicle passengers.
    void WorldDisplayElement_l_FollowMode_Update(void* element)
    {
        using namespace NC2Lifetime;
        WorldDisplayElement_l_FollowMode_Validate(element);
        if (Read<std::uint8_t>(element, 0x220) == 0) return;
        std::uint32_t moveable;
        if (Read<std::uint8_t>(element, 0x221) != 0) moveable = Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars + Read<std::uint32_t>(element, 0x224) * NC2Logic::CHumanArray::ElementSize, 0xC);
        else if (Read<std::uint8_t>(element, 0x22C) != 0) moveable = Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars + Read<std::uint32_t>(element, 0x230) * NC2Logic::CVehicleArray::ElementSize, 0x1DC);
        else return;
        std::int32_t point[2];
        void* display = Read<void*>(element, 0x30);
        Display_DE_MapMoveableToGlobalWorldPixelCoordinates(display, nullptr, moveable, point);
        point[1] = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(point[1]) - 25u);
        NC2E2::DE_SetWantedPositionToPixelPoint(display, point);
    }

    // Original function: FUN_004bb57b; Windows maps the brightest channel through a PCX palette.
    void WorldDisplayElement_DarkenBitmap_Init(void* element, std::uint32_t variant)
    {
        using namespace NC2Lifetime;
        WorldDisplayElement_DarkenBitmap_Exit(element);
        Write(element, 0x238, variant);
        if (variant == 0) return;
        char path[200];
        NXBasics::Runtime_PrintString(path, "data\\gui\\palettes\\ingame_remap_%2.2d.pcx", variant);
        auto* palette = NXBasics::XB_PictureTool_LoadPaletteOutOfPicture(path);
        Write(element, 0x23C, palette);
        if (Read<std::uint32_t>(Read<void*>(element, 0x2C), 4) != 1) return;
        auto* table = static_cast<std::uint16_t*>(NXBasics::Runtime_Allocate(0x20000));
        Write(element, 0x240, table);
        const auto* colours = NXBasics::Palette_GetHighColorTablePtr(palette);
        const auto* converter = NXBasics::g16BitPixelConverter;
        for (std::uint32_t value = 0; value < 0x10000; ++value)
        {
            const auto red = ((value & Read<std::uint16_t>(converter, 0x0C)) >> (Read<std::uint8_t>(converter, 0x2C) & 31u)) << 3;
            const auto green = ((value & Read<std::uint16_t>(converter, 0x0E)) >> (Read<std::uint8_t>(converter, 0x30) & 31u)) << 3;
            const auto blue = ((value & Read<std::uint16_t>(converter, 0x10)) >> (Read<std::uint8_t>(converter, 0x34) & 31u)) << 3;
            const auto brightness = red > green ? (red > blue ? red : blue) : (green > blue ? green : blue);
            table[value] = colours[brightness];
        }
    }

    // Original function: FUN_004bb669.
    void WorldDisplayElement_DarkenBitmap_Exit(void* element)
    {
        void* palette = WorldDisplayElement_ReadField<void*>(element, 0x23C);
        WorldDisplayElement_WriteField<std::uint32_t>(element, 0x238, 0);
        if (palette != nullptr)
        {
            using TDeletingDestructor = void*(__thiscall*)(void*, std::uint32_t);
            auto* methods = *static_cast<TDeletingDestructor**>(palette);
            methods[3](palette, 1);
        }
        void* lookupTable = WorldDisplayElement_ReadField<void*>(element, 0x240);
        WorldDisplayElement_WriteField<void*>(element, 0x23C, nullptr);
        NXBasics::Runtime_Free(lookupTable);
        WorldDisplayElement_WriteField<void*>(element, 0x240, nullptr);
    }

    // Original function: FUN_004bb6a6; CWorldDisplayElement::l_DarkenBitmap_Do.
    void WorldDisplayElement_DarkenBitmap_Do(const void* element, NXBasics::CBitmap* bitmap)
    {
        // +0x238 is the palette variant selected by DarkenBitmap_Init (004bb57b).
        if (WorldDisplayElement_ReadField<std::uint32_t>(element, 0x238) == 0)
            return;

        const std::uint32_t depth = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 4);
        if (depth != 1u && depth != 2u)
            return;

        const std::uint32_t width = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x10);
        const std::uint32_t height = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x14);
        const std::uint32_t rowPixels = WorldDisplayElement_ReadField<std::uint32_t>(bitmap, 0x30);

        if (depth == 2u)
        {
            auto* pixels = WorldDisplayElement_ReadField<std::uint32_t*>(bitmap, 0x2C);
            void* palette = WorldDisplayElement_ReadField<void*>(element, 0x23C);
            const std::uint32_t* table = NXBasics::Palette_GetTrueColorTablePtr(palette);

            for (std::uint32_t y = 0; y < height; ++y)
            {
                for (std::uint32_t x = 0; x < width; ++x)
                {
                    const std::uint32_t pixel = *pixels;
                    std::uint32_t brightness = (pixel >> 16) & 255u;
                    const std::uint32_t green = (pixel >> 8) & 255u;
                    const std::uint32_t blue = pixel & 255u;
                    if (brightness < green)
                        brightness = green;
                    if (brightness < blue)
                        brightness = blue;
                    *pixels++ = table[brightness];
                }
                const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pixels)) + (rowPixels - width) * 4u;
                pixels = reinterpret_cast<std::uint32_t*>(static_cast<std::uintptr_t>(address));
            }
        }
        else
        {
            auto* pixels = WorldDisplayElement_ReadField<std::uint16_t*>(bitmap, 0x28);
            const auto* table = WorldDisplayElement_ReadField<const std::uint16_t*>(element, 0x240);

            for (std::uint32_t y = 0; y < height; ++y)
            {
                for (std::uint32_t x = 0; x < width; ++x)
                {
                    *pixels = table[*pixels];
                    ++pixels;
                }
                const std::uint32_t address = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(pixels)) + (rowPixels - width) * 2u;
                pixels = reinterpret_cast<std::uint16_t*>(static_cast<std::uintptr_t>(address));
            }
        }
    }

    // Original function: FUN_004ba1ba
    // Original method: CWorldDisplayElement::XGui_BE_Element_Draw.
    void WorldDisplayElement_Draw(void* element, NXBasics::CBitmap* bitmap, NXBasics::SRectangle* rectangle)
    {
        static_assert(sizeof(void*) == 4);

        const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const auto readByte = [] (const void* object, std::size_t offset) -> std::uint8_t
        {
            std::uint8_t value;
            std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
            return value;
        };

        const auto readPointer = [&readWord] (const void* object, std::size_t offset) -> void*
        {
            return reinterpret_cast<void*>(static_cast<std::uintptr_t>(readWord(object, offset)));
        };

        WorldDisplayElement_ValidateObjectUnderMouse(element);
        NC2E2::Display_UpdateDisplay(readPointer(element, 0x30));
        NC2InGameGuiBase::BaseBitmapToolElement_Draw(element, bitmap, rectangle);
        WorldDisplayElement_DarkenBitmap_Do(element, bitmap);

        if (readWord(element, 0x38) == 4u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0xE1) == 0;
                NC2E2::Display_DrawTooltipOverlay_External_House(display, readWord(element, 0x60), bitmap, enabled);
            }
        }

        if (readWord(element, 0x38) == 3u)
        {
            void* display = readPointer(element, 0x30);

            if (readWord(element, 0x78) == readWord(display, 0x7C))
            {
                const bool enabled = readByte(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 0xE1) == 0;
                NC2E2::Display_DrawTooltipOverlay_External_Vehicle(display, readWord(element, 0x50), bitmap, enabled);
            }
        }

        // The original branch also accepts an unordered comparison.
        if (!(DAT_00510848 >= 0.01) || NC2Logic::gGameStaticVars.networkWait != 0)
            bitmap->Tool_Darken();

        if (WorldDisplayElement_MouseSelectingArea_IsAreaValid(element))
        {
            // Inline the two SPoint copies performed by 00439842.
            std::int32_t firstPoint[2];
            std::int32_t lastPoint[2];

            std::memcpy(firstPoint, static_cast<const std::byte*>(element) + 0x1F8, sizeof(firstPoint));
            std::memcpy(lastPoint, static_cast<const std::byte*>(element) + 0x200, sizeof(lastPoint));

            NC2E2::Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(readPointer(element, 0x30), firstPoint, true);
            NC2E2::Display_GlobalWorldPixelToLocalDisplayPixelCoordinates(readPointer(element, 0x30), lastPoint, true);

            const std::uint32_t widthBits = static_cast<std::uint32_t>(lastPoint[0]) - static_cast<std::uint32_t>(firstPoint[0]);
            const std::uint32_t heightBits = static_cast<std::uint32_t>(lastPoint[1]) - static_cast<std::uint32_t>(firstPoint[1]);

            NXBasics::SRectangle selection(firstPoint[0], firstPoint[1], NXBasics::Runtime_TimeSigned32(widthBits), NXBasics::Runtime_TimeSigned32(heightBits));
            NXBasics::SColorRGB white(255, 255, 255);

            bitmap->Draw_Rectangle(selection, white);
        }

        void* display = readPointer(element, 0x30);

        // Original double at 004f6ff8: 120.0.
        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= 120.0) && DAT_0051089d == 0)
        {
            NC2E2::AmbientSoundManager_UpdateAmbientSounds(NC2E2::gE2AmbientSoundManager, display);
            NC2E2::CE2AnimalSoundManager::sTheObjectPtr->PlayAnimalSounds(readPointer(element, 0x30));
            NC2E2::WorldMusic_UpdateVisiblePlayerSelection();
        }

        display = readPointer(element, 0x30);

        if (readByte(display, 0x6C) != 0 && !(DAT_00510848 >= 0.01))
        {
            char text[200];
            NXBasics::Runtime_PrintString(text, "* %s *", NC2GuiToolsBase::StringTool_GetUpdate103StringPtr(20));

            const auto& guiResources = NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources;

            auto* font = guiResources.font10;
            const auto* colour = reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(guiResources.fontWhitePalette) + 0x400);

            const std::int32_t halfHeight = bitmap->GetRectangle().height / 2;
            const std::int32_t y = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(halfHeight) + 180u);
            const std::int32_t textWidth = NXBasics::Font_GetPixelWidth(font, text);
            const std::uint32_t remainingWidth = static_cast<std::uint32_t>(bitmap->GetRectangle().width) - static_cast<std::uint32_t>(textWidth);
            const std::int32_t x = NXBasics::Runtime_TimeSigned32(remainingWidth) / 2;

            NC2GuiToolsBase::DrawTool_PrintStringX(bitmap, x, y, font, colour, text);
        }

        const std::uint32_t frameMode = readWord(element, 0x208);

        if (frameMode != 0)
            NC2GuiToolsBase::DrawTool_DrawFrame(nullptr, bitmap, rectangle, NXBasics::Runtime_TimeSigned32(frameMode));
    }

    // Original function: FUN_004baf71
    void WorldDisplayElement_ValidateObjectUnderMouse(void* element)
    {
        auto* data = static_cast<std::byte*>(element);

        if (!NC2E2::DisplaySearchObject_Validate(data + 0x38))
            return;

        std::uint32_t displayAddress;
        std::memcpy(&displayAddress, data + 0x30, sizeof(displayAddress));

        auto* display = reinterpret_cast<std::byte*>(static_cast<std::uintptr_t>(displayAddress));
        const std::uint32_t zero = 0;

        std::memcpy(display + 0xAC, &zero, sizeof(zero));
        std::memcpy(data + 0x1C4, &zero, sizeof(zero));
        std::memcpy(data + 0x1C8, &zero, sizeof(zero));

        std::memset(data + 0x1CC, 0, 0x14);
        std::memset(data + 0x1E0, 0, 0x14);
    }

    // Original function: FUN_004bafc2
    bool WorldDisplayElement_MouseSelectingArea_IsAreaValid(const void* element)
    {
        const auto* data = static_cast<const std::byte*>(element);

        std::uint8_t active;
        std::memcpy(&active, data + 0x1F4, sizeof(active));

        if (active == 0)
            return false;

        const auto hasMinimumDistance = [data] (std::size_t firstOffset, std::size_t lastOffset) -> bool
        {
            std::uint32_t first;
            std::uint32_t last;

            std::memcpy(&first, data + firstOffset, sizeof(first));
            std::memcpy(&last, data + lastOffset, sizeof(last));

            const std::uint32_t difference = first - last;
            const std::uint32_t magnitude = (difference & 0x80000000u) != 0 ? 0u - difference : difference;

            // Preserve wrapped subtraction and the signed comparison after NEG.
            return magnitude >= 5u && magnitude <= 0x7FFFFFFFu;
        };

        if (!hasMinimumDistance(0x1F8, 0x200))
            return false;

        return hasMinimumDistance(0x1FC, 0x204);
    }
}
