#include "InGameGuiManager_MessageHandlers.h"
#include "C2LifetimeAccess.h"
#include "InGameGuiHumanActionButtons.h"
#include "InGameGuiManager.h"
#include "C2GameTime.h"
#include "C2UserNetworkCommandAccess.h"
#include "InGameGuiWindowState.h"
#include "InGameGuiSupportAccess.h"
#include "InGameGuiInputAccess.h"
#include <array>
#include <cstring>
#include "XGuiElementAccess.h"
#include "XCrtFileAccess.h"

namespace
{
    // Complete original table: 004F4DE0..004F4E27. Slot two remains partially reconstructed.
    const std::uintptr_t gInGameDefaultMessageHandlerVirtualTable[18] =
    {
        reinterpret_cast<std::uintptr_t>(&InGameGuiManagerDefaultMessageHandlerElement_Delete),
        reinterpret_cast<std::uintptr_t>(&NXBaseGui::BaseElement_HasSubElements),
        reinterpret_cast<std::uintptr_t>(&InGameGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle),
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
    static_assert(sizeof(gInGameDefaultMessageHandlerVirtualTable) == 0x48);
}

// Original function: FUN_004a0145; flags zero destroy the embedded handler without freeing it.
void* __fastcall InGameGuiManagerDefaultMessageHandlerElement_Delete(void* element, void* /*unusedEdx*/, std::uint32_t flags)
{
    NC2Lifetime::Write(element, 0, gInGameDefaultMessageHandlerVirtualTable + 0);
    NXBaseGui::BaseElement_Destruct(element);
    if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
    return element;
}

// Original function: FUN_004a3eab; CGuiManagerDefaultMessageHandlerElement constructor.
void* __fastcall InGameGuiManagerDefaultMessageHandlerElement_Construct(void* element)
{
    const NXBasics::SRectangle rectangle(0, 0, 0, 0);
    NXBaseGui::BaseElement_Construct(element, &rectangle, 0, 0x800);
    NC2Lifetime::Write(element, 0, gInGameDefaultMessageHandlerVirtualTable + 0);
    return element;
}

namespace
{
    // Partial original function: FUN_004bc36f; native name-input forwarding, other keyboard branches remain open.
    bool __fastcall InGameGuiManagerPrimaryMessageHandlerElement_MessageHandle(void*, void*, std::uint32_t message, std::uint32_t key, std::uint32_t character, std::uint32_t, std::uint32_t)
    {
        if (message != 0x200) return false;
        return NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->MiscWindows_HumanChangeNameWindow_SendKey(key, static_cast<char>(character));
    }
    void* __fastcall InGameGuiManagerPrimaryMessageHandlerElement_Delete(void* element, void*, std::uint32_t flags);
    // Complete Windows table region 004F7038..004F707F; inherited folded entries retain their base semantics.
    const std::array<std::uintptr_t, 18> gInGamePrimaryMessageHandlerVirtualTable = []
    {
        std::array<std::uintptr_t, 18> table{};
        std::memcpy(table.data(), gInGameDefaultMessageHandlerVirtualTable, sizeof(table));
        table[0] = reinterpret_cast<std::uintptr_t>(&InGameGuiManagerPrimaryMessageHandlerElement_Delete);
        table[2] = reinterpret_cast<std::uintptr_t>(&InGameGuiManagerPrimaryMessageHandlerElement_MessageHandle);
        return table;
    }();
    // Original function: FUN_004bc34d.
    void* __fastcall InGameGuiManagerPrimaryMessageHandlerElement_Delete(void* element, void*, std::uint32_t flags)
    {
        NC2Lifetime::Write(element, 0, gInGamePrimaryMessageHandlerVirtualTable.data()); NXBaseGui::BaseElement_Destruct(element);
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(element);
        return element;
    }
}
// Original function: FUN_004bc31c.
void* __fastcall InGameGuiManagerPrimaryMessageHandlerElement_Construct(void* element)
{
    const NXBasics::SRectangle rectangle(0, 0, 0, 0);
    NXBaseGui::BaseElement_Construct(element, &rectangle, 4, 0xA01);
    NC2Lifetime::Write(element, 0, gInGamePrimaryMessageHandlerVirtualTable.data()); return element;
}

// Partial original function: FUN_004a3edb; character and miscellaneous branches are connected.
// Naming reference: NC2InGameGuiManager::CGuiManagerDefaultMessageHandlerElement::XGui_BE_Message_Handle.
// Remaining window, house, vehicle and unsupported character-input branches stay excluded from completion counts.
bool __fastcall InGameGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle(void*, void*, std::uint32_t message, std::uint32_t command, std::uint32_t, std::uint32_t, std::uint32_t)
{
    if (message == 0x800)
    {
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        if (command >= 3000 && command < 3299) return NC2InGameGuiManager::HumanActionButtons_HandleCommand(command - 3000);
        if (command == 0xF3C) manager->MiscWindows_OptionsWindow_Toggle(false, false);
        else if (command == 0xF42) manager->MiscWindows_MissionInfoWindow_Toggle(0xFFFFFFFFu, true);
        else if (command == 0xF45) manager->MiscWindows_OverviewWindow_Open();
        else if (command == 0xF46)
        {
            const double speed = NC2Logic::Logic_EntryPoint_GameTime_GetSpeed();
            std::uint32_t nextSpeed = 12;
            if (speed >= 12.0 && speed < 24.0) nextSpeed = 24;
            else if (speed >= 24.0 && speed < 36.0) nextSpeed = 36;
            std::byte networkCommand[0x14]{};
            networkCommand[0] = std::byte{0x87};
            std::memcpy(networkCommand + 4, &nextSpeed, sizeof(nextSpeed));
            NC2Logic::UserNetworkCommandManager_AddCommand(NC2Logic::gUserNetworkCommandManager, nullptr, networkCommand, false, true);
        }
        else if (command == 0xF47)
        {
            static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->MessagePriority_Toggle();
            manager->MiscButtons_MessagePriorityButton_Update(true);
        }
    }
    return true;
}
