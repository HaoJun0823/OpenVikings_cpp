#include "InGameGuiHumanActionButtons.h"
#include "InGameGuiManager.h"
#include "InGameGuiInputAccess.h"
#include "InGameGuiBaseAccess.h"
#include "C2E2HumanSoundAccess.h"
#include "C2HumanUserControl.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "GuiBitmapToolElementAccess.h"
#include "GuiToolsBase.h"
#include "XGuiDesktop.h"
#include "XGuiElementAccess.h"
#include "XList.h"
#include "XMemoryChecksum.h"
#include "XMMouseManager.h"
#include "XCrtFileAccess.h"

using namespace NC2Lifetime;
namespace
{
    void* Input() { return NC2InGameGuiInputManager::gInputManager; }
}
// Partial original function: FUN_004a1bf0; human branches only, house/vehicle command lists remain open.
std::uint32_t __fastcall InGameGuiManager_l_Selection_ActionButtons_GetChecksum(void*)
{
    using namespace NC2InGameGuiInputManager;
    const auto type = Read<std::uint32_t>(Input(), 4);
    NC2Logic::SUserCommandsForHumanHandler commands;
    if (type == 1)
    {
        if (!NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedSingleHumanId(Input()), &commands, false, false)) return 0;
        return NXBasics::XB_GetMemoryChecksum(&commands, sizeof(commands));
    }
    if (type != 2 || !NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedMultipleHumanId(Input(), 0), &commands, true, false)) return 0;
    auto checksum = NXBasics::XB_GetMemoryChecksum(&commands, sizeof(commands));
    const auto count = Selection_GetSelectedMultipleHumanNumber(Input());
    for (std::uint32_t index = 1; index < count; ++index)
    {
        NC2Logic::SUserCommandsForHumanHandler other;
        if (!NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedMultipleHumanId(Input(), index), &other, true, false)) break;
        // Windows hashes the first command list again, rather than the newly filled list.
        checksum ^= NXBasics::XB_GetMemoryChecksum(&commands, sizeof(commands)) + 10u;
    }
    return checksum;
}
// Partial original function: FUN_004a10cb; complete character layout, house/vehicle branches remain open.
void __fastcall InGameGuiManager_l_Selection_ActionButtons_BringUp(void* manager)
{
    using namespace NC2InGameGuiInputManager;
    const auto selection = Read<std::uint32_t>(Input(), 4);
    if (selection != 1 && selection != 2) { Write(manager, 0xCC, InGameGuiManager_l_Selection_ActionButtons_GetChecksum(manager)); return; }
    NC2Logic::SUserCommandsForHumanHandler commands;
    if (selection == 1)
    {
        if (!NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedSingleHumanId(Input()), &commands, false, false)) return;
    }
    else
    {
        if (!NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedMultipleHumanId(Input(), 0), &commands, true, false)) return;
        const auto count = Selection_GetSelectedMultipleHumanNumber(Input());
        if (count > 1)
        {
            for (std::uint32_t index = 1; index < count; ++index)
            {
                NC2Logic::SUserCommandsForHumanHandler other;
                if (!NC2Logic::Logic_UserControl_GetHumanCommands(Selection_GetSelectedMultipleHumanId(Input(), index), &other, true, false)) return;
                for (std::int32_t command = 0; command < commands.count; ++command)
                {
                    if (other.IsCommandAllowed(commands.commands[command])) continue;
                    for (auto next = command; next < commands.count - 1; ++next) commands.commands[next] = commands.commands[next + 1];
                    --commands.count; --command;
                }
            }
            for (std::int32_t command = 0; command < commands.count; ++command)
            {
                if (commands.commands[command] != 12) continue;
                for (auto next = command; next < commands.count - 1; ++next) commands.commands[next] = commands.commands[next + 1];
                --commands.count; --command;
            }
        }
    }
    if (commands.count == 0) return;
    commands.SortCommandsByPriority();
    std::uint32_t groups[5]{};
    for (std::int32_t index = 0; index < commands.count; ++index) ++groups[NC2Logic::Tool_UserHumanCommandTypes_GetGroupType(commands.commands[index])];
    NXBasics::SRectangle area(Read<std::int32_t>(manager, 0xD0) - 116, Read<std::int32_t>(manager, 0xD4) - 116, 232, 232);
    NXBasics::SRectangle desktopBounds(0, 0, 0, 0);
    // CDesktop stores its rectangle at +0, unlike CBaseElement's rectangle at +4.
    std::memcpy(&desktopBounds, NXBaseGui::CDesktop::sTheObjectPtr, sizeof(desktopBounds)); area.PlaceInside(desktopBounds);
    // Complete graphic switch 004A18E6..004A1995, covering native actions 2..45.
    constexpr std::uint32_t graphics[44]{107,109,108,125,126,104,105,106,101,102,127,128,107,107,96,97,129,130,103,133,131,132,112,135,134,107,100,99,107,111,136,107,107,114,115,116,117,123,124,118,119,120,121,122};
    for (std::uint32_t group = 0; group < 5; ++group)
    {
        auto x = area.x + area.width / 2, y = area.y + area.height / 2;
        std::int32_t dx = 0, dy = 0, bendX = 0, bendY = 0;
        const auto span = static_cast<std::int32_t>(groups[group] * 16);
        switch (group)
        {
        case 0: x += 16 - span; y += 100; dx = 32; bendY = -5; break;
        case 1: x += span - 16; y -= 100; dx = -32; bendY = 5; break;
        case 2: x += 100; y += 16 - span; dy = 32; bendX = -5; break;
        case 3: x -= 100; y += span - 16; dy = -32; bendX = 5; break;
        case 4: x -= 68; y += span - 16; dy = -32; bendX = 5; break;
        }
        std::uint32_t ordinal = 0;
        for (std::int32_t index = 0; index < commands.count; ++index)
        {
            const auto action = commands.commands[index];
            if (NC2Logic::Tool_UserHumanCommandTypes_GetGroupType(action) != group) continue;
            const char* tooltip = NC2GuiToolsBase::StringTool_GetLogicUserCommandsForHumanTypeNamePtr(action);
            const auto graphic = action >= 2 && action <= 45 ? graphics[action - 2] : 107u;
            NXBasics::SRectangle rectangle(x - 16, y - 16, 32, 32);
            if (ordinal == 0 || ordinal == groups[group] - 1) { rectangle.x += bendX; rectangle.y += bendY; }
            void* memory = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            void* element = memory == nullptr ? nullptr : NC2InGameGuiBase::BaseToolGfxButton_ConstructWithToolTip(memory, nullptr, &rectangle, graphic, tooltip, 3000 + action, static_cast<std::byte*>(manager) + 0x1A8);
            Write<std::uint8_t>(element, 0x34, 1); Write<std::uint8_t>(element, 0x44, 1);
            NC2InGameGuiBase::ToolGraphics_SetPalettePtr(element, NC2GuiToolsBase::CGuiBaseDataManager::mStaticVars.resources.contextPalette);
            NC2InGameGuiBase::BaseToolDesktop_AddOverlayElement(static_cast<NXBaseGui::CBaseElement*>(element));
            reinterpret_cast<NXBasics::CListBase*>(static_cast<std::byte*>(manager) + 0xB8)->InsertAtEnd(element);
            x += dx; y += dy; ++ordinal;
        }
    }
    Write(manager, 0xCC, InGameGuiManager_l_Selection_ActionButtons_GetChecksum(manager));
}
namespace NInGameGuiManager
{
    // Original function: FUN_004a10a4.
    void CInGameGuiManager::Selection_ActionButtons_BringUp()
    {
        l_Selection_RemoveAllGuiElements();
        auto* mouse = NXSysMouseManager::CMouseManager::sTheObjectPtr;
        Write(this, 0xD0, Read<std::int32_t>(mouse, 0)); Write(this, 0xD4, Read<std::int32_t>(mouse, 4));
        InGameGuiManager_l_Selection_ActionButtons_BringUp(this);
    }
}
namespace NC2InGameGuiManager
{
    // Original function: FUN_004a480e; action IDs are distinct from network opcodes.
    bool ls_Tool_SendSimpleHumanNetworkCommand(std::uint32_t action, std::int32_t id, std::uint32_t first, std::uint32_t second)
    {
        std::uint8_t opcode;
        switch (action)
        {
        case 3: opcode = 7; break; case 4: opcode = 8; break; case 5: opcode = 9; break; case 6: opcode = 10; break;
        case 7: opcode = 13; break; case 8: opcode = 15; break; case 9: opcode = 16; break; case 11: opcode = 27; break;
        case 13: opcode = 39; break; case 17: opcode = 29; break; case 19: opcode = 33; break; case 21: opcode = 31; break;
        case 32: opcode = 20; break; case 35: case 36: case 37: case 38: opcode = 41; break;
        case 39: case 40: opcode = 42; break; case 48: opcode = 5; break; case 49: opcode = 6; break;
        case 54: opcode = 12; break; case 55: opcode = 11; break; default: return false;
        }
        std::byte command[20]{}; Write(command, 0, opcode); Write(command, 4, id); Write(command, 0x0C, first); Write(command, 0x10, second);
        NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
        NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, id); return true;
    }
    // Human branch of FUN_004a3edb; original unsupported input modes remain explicit reconstruction boundaries.
    bool HumanActionButtons_HandleCommand(std::uint32_t action)
    {
        using namespace NC2InGameGuiInputManager;
        auto* manager = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;
        std::uint32_t first = action >= 35 && action <= 38 ? action - 34 : action == 39 ? 1 : 0;
        std::int32_t mode = -1;
        switch (action) { case 41: mode = 23; break; case 42: mode = 10; break; case 43: mode = 25; break; case 44: mode = 26; break; case 45: mode = 22; break; }
        bool remove = true;
        if (mode != -1) InputMode_Set(Input(), mode);
        else if (Read<std::uint32_t>(Input(), 4) == 1)
        {
            const auto id = Selection_GetSelectedSingleHumanId(Input());
            switch (action)
            {
            case 2: mode = 1; break; case 10: mode = 4; break; case 12: mode = 27; break; case 16: mode = 5; break;
            case 18: mode = 6; break; case 20: mode = 7; break; case 22: mode = 8; break; case 23: mode = 9; break;
            case 24: manager->MiscWindows_HumanChangeJobWindow_Open(); break;
            case 25: manager->MiscWindows_HumanChangeProducedGoodWindow_Open(); break;
            case 26: manager->MiscWindows_HumanChangeEquippedGoodsWindow_Open(6, -1); break;
            case 27: manager->MiscWindows_HumanLearnJobWindow_Open(); break;
            case 28: mode = 2; break; case 29: mode = 3; break; case 31: mode = 13; break;
            case 32: remove = ls_Tool_SendSimpleHumanNetworkCommand(32, id, 0xFFFFFFFFu, 1); break;
            default: remove = ls_Tool_SendSimpleHumanNetworkCommand(action, id, first, 0); break;
            }
            if (mode != -1) InputMode_Set(Input(), mode);
        }
        else if (Read<std::uint32_t>(Input(), 4) == 2)
        {
            if (action == 24) manager->MiscWindows_HumanChangeJobWindow_Open();
            else if (action == 26) manager->MiscWindows_HumanChangeEquippedGoodsWindow_Open(6, -1);
            else for (std::uint32_t index = 0; index < Selection_GetSelectedMultipleHumanNumber(Input()); ++index) remove = ls_Tool_SendSimpleHumanNetworkCommand(action, Selection_GetSelectedMultipleHumanId(Input(), index), first, 0);
        }
        if (remove) manager->l_Selection_RemoveAllGuiElements();
        return true;
    }
}
