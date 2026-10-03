#include "MainMenuGuiManager.h"
#include "C2CleanMapsManager.h"
#include "C2CampaignStaticDataManager.h"
#include "C2ControlStartup.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2SavegamesManager.h"
#include "GuiToolsBase.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "NetworkShutdown.h"
#include "XBTokenList.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "XIniFileReader.h"
#include "XMKeyManager.h"

#include <cstring>

namespace
{
    using namespace NC2Lifetime;
    // Complete Windows token table 004F7E58..004F7E77 and state 005694E8..005694EF.
    const NXBasics::SToken gMultiplayerMapTokens[] = {
        { "playeroption", 0x706C6F70u }, { "playerhideinmenu", 0x7068696Du },
        { "playerfixcolors", 0x7066636Fu }, { nullptr, 0 }
    };
    struct SMultiplayerMapTokenState { const NXBasics::SToken* table; std::uint32_t flags; };
    SMultiplayerMapTokenState gMultiplayerMapTokenState{};
    // Complete Windows static region 005694F0..00569504; Mac names it lastKeysBuffer.
    char lastKeysBuffer[21]{};
    static_assert(sizeof(gMultiplayerMapTokens) == 0x20);
    static_assert(sizeof(SMultiplayerMapTokenState) == 8);

    // Original function: FUN_004d32f4; Windows CD-identification predicate.
    bool MainMenu_HasGameCD() { return NMasterControlProgram::FindGameCDDrive() != 0; }
    // Original function: FUN_004d8454; empty token-list exit callback.
    int __cdecl DestroyMultiplayerMapTokenList() { return 0; }

    // Source helpers for Windows inline row stores and 0x168-byte kick messages.
    std::uint32_t FixedPlayerColor(std::uint32_t slot) { return slot < 10 ? slot : 9; }
    void SendKickToPlayer(std::uint32_t player)
    {
        std::byte message[0x168]{};
        Write<std::uint32_t>(message, 0, 3);
        NNetwork::SendDynamicNetworkMessage(&gDynamicNetworkData, player, message, sizeof(message));
    }
}

namespace
{
    // Original function: FUN_004db3a5; Windows-only ANSI clipboard helper.
    bool CopyClipboardText(char* destination, std::int32_t capacity)
    {
        if (destination == nullptr || capacity <= 1 || !::OpenClipboard(nullptr)) return false;
        const auto text = static_cast<const char*>(::GetClipboardData(CF_TEXT));
        bool copied = false;
        // The original treats the clipboard handle as a pointer without GlobalLock.
        if (text != nullptr && std::strlen(text) != 0)
        {
            std::strncpy(destination, text, capacity - 1);
            destination[capacity - 1] = '\0';
            copied = true;
        }
        ::CloseClipboard();
        return copied;
    }
}

// Original function: FUN_004db0d1.
bool __fastcall MainMenuGuiManagerPrimaryMessageHandlerElement_XGui_BE_Message_Handle(void* element, void* unusedEdx, std::uint32_t message, std::uint32_t key, std::uint32_t character, std::uint32_t param3, std::uint32_t param4)
{
    using NC2MainMenuGuiManager::CGuiManager;
    if (message != 0x200) return true;
    if ((NC2Lifetime::Read<std::uint32_t>(NXSysKeyManager::CKeyManager::sTheObjectPtr, 0) & 2) != 0 && key == 0x56)
    {
        char buffer[100];
        if (CopyClipboardText(buffer, sizeof(buffer)))
            for (std::uint32_t index = 0; index < std::strlen(buffer); ++index)
                CGuiManager::sTheObjectPtr->StringInput_HandleKeys(0, buffer[index]);
        return true;
    }
    const char value = static_cast<char>(character);
    if (CGuiManager::sTheObjectPtr->StringInput_HandleKeys(key, value) || value == '\0') return true;
    std::memmove(lastKeysBuffer + 1, lastKeysBuffer, 19);
    lastKeysBuffer[20] = '\0';
    lastKeysBuffer[0] = value;
    if (NC2Logic::gCampaignStaticDataManager == nullptr || CGuiManager::sTheObjectPtr->currentScreen != 5) return true;
    // Windows compares case-sensitive prefixes of the newest-first history.
    const char* codes[] = { "esao", "lekaro", "lesniztahcs", "lepmet", "aipmylo", "oarahp", "sitnalta" };
    bool matched = false;
    for (std::uint32_t index = 0; index < 7; ++index)
        if (std::strncmp(lastKeysBuffer, codes[index], std::strlen(codes[index])) == 0)
        {
            NC2Logic::gCampaignStaticDataManager->CampaignSubMission_DoAllow(3, 20 + index * 10, true);
            matched = true;
        }
    if (std::strncmp(lastKeysBuffer, "sursemag", 8) == 0)
    {
        for (std::uint32_t mission = 20; mission <= 80; mission += 10)
            NC2Logic::gCampaignStaticDataManager->CampaignSubMission_DoAllow(3, mission, true);
        matched = true;
    }
    if (matched)
    {
        NC2GuiToolsBase::FXTool_Chat();
        CGuiManager::sTheObjectPtr->Screen_UpdateGui();
        lastKeysBuffer[0] = '\0';
    }
    return true;
}

namespace NC2MainMenuGuiManager
{
    using namespace NC2Lifetime;

    // These network-only method names describe Windows behavior; no matching Mac symbol exists.
    // Original function: FUN_004d71ba.
    bool CGuiManager::Network_SelectServiceProvider(std::uint32_t index)
    {
        if (index >= Read<std::uint32_t>(this, 0x68))
        {
            Debug_ErrorString_Add("Select service provider - id out of range");
            return false;
        }
        Write(this, 0x51C, index);
        return true;
    }

    // Original function: FUN_004d71dc; only the -1 sentinel is checked.
    const GUID* CGuiManager::Network_GetSelectedServiceProvider() const
    {
        const auto index = Read<std::uint32_t>(this, 0x51C);
        return index == 0xFFFFFFFFu ? nullptr : reinterpret_cast<const GUID*>(reinterpret_cast<const std::byte*>(this) + 0xD4 + index * 0x78u);
    }

    // Original function: FUN_004d787b.
    void CGuiManager::Network_SetAllowConnections(bool allow)
    {
        GetNetworkInstance()->SetAllowConnections(allow);
        Screen_UpdateGui();
    }

    // Original function: FUN_004d78a2; send the original kick message to a selected remote player.
    void CGuiManager::Network_KickSelectedPlayer()
    {
        const auto player = Read<std::uint32_t>(this, 0xEE4);
        if (player == 0) return;
        const auto index = Network_GetPlayerListIndex(player);
        if (index == 0xFFFFFFFFu) return;
        if (Read<std::uint8_t>(this, 0xB0E + index * 0x6Cu) != 0) return;
        SendKickToPlayer(player);
    }

    // Original function: FUN_004d7922.
    void CGuiManager::Network_KickUnassignedPlayers()
    {
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(this, 0xAA4); ++index)
        {
            const auto player = Read<std::uint32_t>(this, 0xB10 + index * 0x6Cu);
            std::uint32_t slot = 0;
            while (slot < 20 && Read<std::uint32_t>(this, 0xF28 + slot * 0x10u) != player) ++slot;
            if (slot == 20) SendKickToPlayer(player);
        }
    }

    // Original function: FUN_004d7d19.
    void CGuiManager::Network_SelectPlayer(std::uint32_t playerId)
    {
        const auto index = Network_GetPlayerListIndex(playerId);
        if (index == 0xFFFFFFFFu) Write<std::uint32_t>(this, 0xEE4, 0);
        else
        {
            Write(this, 0xEE4, playerId);
            Screen_UpdateGui();
        }
    }

    // Original function: FUN_004d7e5d; slot bounds remain unchecked.
    void CGuiManager::Network_AssignSelectedPlayer(std::uint32_t slot)
    {
        const auto player = Read<std::uint32_t>(this, 0xEE4);
        if (player == 0 || (Read<std::uint8_t>(this, 0xF30 + slot * 0x10u) & 2) == 0) return;
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            const auto offset = 0xF28 + index * 0x10u;
            if (Read<std::uint32_t>(this, offset) != Read<std::uint32_t>(this, 0xEE4)) continue;
            Write<std::uint32_t>(this, offset, 0);
            Write<std::uint32_t>(this, offset + 4, 0);
            if (Read<std::uint32_t>(this, 0xF24) == 0) Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(index) : 0u);
            if (index != slot && (Read<std::uint8_t>(this, offset + 8) & 4) != 0)
            {
                if (Read<std::uint32_t>(this, 0xF24) == 0) Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(index) : Network_FindNextPlayerColor(0xFFFFFFFFu));
                Write<std::uint32_t>(this, offset + 4, 2);
            }
        }
        const auto offset = 0xF28 + slot * 0x10u;
        Write<std::uint32_t>(this, offset + 4, 0);
        if (Read<std::uint32_t>(this, 0xF24) == 0) Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : Network_FindNextPlayerColor(0xFFFFFFFFu));
        Write(this, offset, Read<std::uint32_t>(this, 0xEE4));
        Write<std::uint32_t>(this, offset + 4, 1);
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d7f7e.
    void CGuiManager::Network_ChangePlayerType(std::uint32_t slot)
    {
        const auto offset = 0xF28 + slot * 0x10u;
        const auto type = Read<std::uint32_t>(this, offset + 4);
        const auto flags = Read<std::uint8_t>(this, offset + 8);
        if (type == 1)
        {
            bool changed = false;
            if ((flags & 4) != 0) { Write<std::uint32_t>(this, offset + 4, 2); Write<std::uint32_t>(this, offset, 0); changed = true; }
            if ((flags & 1) != 0) { Write<std::uint32_t>(this, offset + 4, 0); Write<std::uint32_t>(this, offset, 0); changed = true; }
            if (!changed) return;
        }
        else if (type == 2)
        {
            if ((flags & 1) == 0) return;
            Write<std::uint32_t>(this, offset + 4, 0);
            Write<std::uint32_t>(this, offset, 0);
        }
        else
        {
            if ((flags & 4) == 0) return;
            if (Read<std::uint32_t>(this, 0xF24) == 0) Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : Network_FindNextPlayerColor(0xFFFFFFFFu));
            Write<std::uint32_t>(this, offset + 4, 2);
            Write<std::uint32_t>(this, offset, 0);
        }
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d804f.
    void CGuiManager::Network_ChangePlayerColor(std::uint32_t slot)
    {
        if (Read<std::uint32_t>(this, 0xF24) != 0) return;
        const auto color = Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : Network_FindNextPlayerColor(Read<std::uint32_t>(this, 0xF34 + slot * 0x10u));
        Write(this, 0xF34 + slot * 0x10u, color);
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d80a7; a full cycle returns color zero.
    std::uint32_t CGuiManager::Network_FindNextPlayerColor(std::uint32_t color) const
    {
        const auto stop = color == 0xFFFFFFFFu ? 9u : color;
        for (;;)
        {
            ++color;
            if (color == 10) color = 0;
            if (color == stop) return 0;
            bool used = false;
            for (std::uint32_t slot = 0; slot < 20; ++slot)
            {
                const auto offset = 0xF2C + slot * 0x10u;
                const auto type = Read<std::uint32_t>(this, offset);
                if ((type == 1 || type == 2) && (Read<std::uint8_t>(this, offset + 7) & 0x80) == 0 && Read<std::uint32_t>(this, offset + 8) == color) { used = true; break; }
            }
            if (!used) return color;
        }
    }

    // Original function: FUN_004d80f6.
    void CGuiManager::Network_SetLobbySelection(std::uint32_t selection, bool keepData)
    {
        if (selection == lobbyValue) return;
        lobbyValue = selection;
        if (selection == 2) NC2Logic::gSavegamesManager->SavegameList_Update(2, true);
        if (!keepData) std::memset(lobbyData, 0, sizeof(lobbyData));
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d814c.
    void CGuiManager::Network_SetLobbyDifficulty(std::uint32_t difficulty)
    {
        Write(this, 0x106C, difficulty);
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d81c7.
    void CGuiManager::Network_SelectMap(const NC2Logic::SCleanMapGuid& guid)
    {
        const auto difficulty = Read<std::uint32_t>(this, 0x106C);
        std::memset(lobbyData, 0, sizeof(lobbyData));
        Write<std::uint32_t>(lobbyData, 0, 1);
        std::memcpy(lobbyData + 4, &guid, sizeof(guid));
        for (std::uint32_t slot = 0; slot < 20; ++slot)
            Write(this, 0xF34 + slot * 0x10u, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : 0u);
        auto* map = NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(guid);
        void* archive = nullptr;
        if (map->userMap != 0)
        {
            void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x14, 1);
            archive = allocation == nullptr ? nullptr : NXBasics::Archive_Initialize(allocation, map->archiveFilename);
            NXBasics::CFile::FileSystem_FileLibrary_Add(archive);
        }
        NXBasics::CFile::FileSystem_SystemPathName_Set("$maproot$", map->mapRoot);
        {
            NXBasics::CIniFileReader reader("$maproot$\\map.ini", nullptr, false, nullptr, nullptr);
            NXBasics::CFile::FileSystem_SystemPathName_Clear("$maproot$");
            if (reader.IsLoaded() && reader.SearchGroup("multiplayer"))
            {
                while (reader.GetNextCommandLineInGroup() != nullptr)
                {
                    if ((gMultiplayerMapTokenState.flags & 1u) == 0)
                    {
                        gMultiplayerMapTokenState.flags |= 1u;
                        gMultiplayerMapTokenState.table = gMultiplayerMapTokens;
                        NXBasics::Runtime_AtExit(DestroyMultiplayerMapTokenList);
                    }
                    NXBasics::CTokenList tokens(gMultiplayerMapTokenState.table);
                    switch (tokens.FindToken(reader.GetCurrentName()))
                    {
                        case 0x706C6F70u:
                        {
                            const auto slot = reader.GetInt();
                            if (slot >= 20) break;
                            std::uint32_t flags = 0;
                            while (reader.GetTypeOfNextParameter() == 3)
                            {
                                const auto option = static_cast<std::uint32_t>(reader.GetInt());
                                if (option <= 2) flags |= 1u << option;
                            }
                            Write(this, (static_cast<std::uint32_t>(slot) + 0xF3u) * 0x10u, flags);
                            break;
                        }
                        case 0x7068696Du:
                        {
                            const auto slot = reader.GetInt();
                            if (slot < 20)
                            {
                                const auto offset = (static_cast<std::uint32_t>(slot) + 0xF3u) * 0x10u + 3u;
                                Write<std::uint8_t>(this, offset, Read<std::uint8_t>(this, offset) | 0x80);
                            }
                            break;
                        }
                        case 0x7066636Fu:
                            Write<std::uint8_t>(this, 0x1068, reader.GetInt() != 0);
                            break;
                    }
                }
            }
            if (map->userMap != 0) NXBasics::CFile::FileSystem_FileLibrary_Remove(archive);
        }
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            const auto offset = 0xF28 + slot * 0x10u;
            if ((Read<std::uint8_t>(this, offset + 8) & 4) == 0) continue;
            Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : Network_FindNextPlayerColor(0xFFFFFFFFu));
            Write<std::uint32_t>(this, offset + 4, 2);
            Write<std::uint32_t>(this, offset, 0);
        }
        Write(this, 0x106C, difficulty);
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d8455.
    void CGuiManager::Network_SelectSavegame(std::uint32_t id)
    {
        std::memset(lobbyData, 0, sizeof(lobbyData));
        Write<std::uint32_t>(lobbyData, 0, 2);
        Write(this, 0xF24, id);
        auto* saved = NC2Logic::gSavegamesManager->GetSavegameInfoPtrByUniqueId(id);
        NC2Logic::gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(saved->header.mapGuid);
        for (std::uint32_t slot = 0; slot < 20; ++slot)
        {
            const auto offset = 0xF28 + slot * 0x10u;
            Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : 0u);
            if (saved->header.multiplayer == 0) continue;
            const auto type = saved->header.players[slot].value;
            if (type == 1 || type == 2)
            {
                Write<std::uint32_t>(this, offset + 4, 2);
                Write<std::uint32_t>(this, offset + 8, type == 1 ? 6 : 4);
            }
            Write(this, offset + 12, Read<std::uint8_t>(this, 0x1068) != 0 ? FixedPlayerColor(slot) : saved->header.players[slot].colorId);
        }
        if (saved->header.multiplayer != 0)
        {
            for (std::uint32_t slot = 0; slot < 20; ++slot)
            {
                const auto& player = saved->header.players[slot];
                if (player.value != 1 || std::strlen(player.name) == 0) continue;
                for (std::uint32_t index = 0; index < Read<std::uint32_t>(this, 0xAA4); ++index)
                {
                    if (NXBasics::Runtime_CompareStringsIgnoreCase(reinterpret_cast<const char*>(this) + 0xAA8 + index * 0x6Cu, player.name) != 0) continue;
                    Write(this, 0xF28 + slot * 0x10u, Read<std::uint32_t>(this, 0xB10 + index * 0x6Cu));
                    Write<std::uint32_t>(this, 0xF2C + slot * 0x10u, 1);
                    break;
                }
            }
        }
        Screen_UpdateGui();
        Network_SendLobbyData();
    }
}

// Original function: FUN_004d6bbb; CGuiManagerDefaultMessageHandlerElement::XGui_BE_Message_Handle.
bool __fastcall MainMenuGuiManagerDefaultMessageHandlerElement_XGui_BE_Message_Handle(void* /*element*/, void* /*unusedEdx*/, std::uint32_t message, std::uint32_t command, std::uint32_t /*sender*/, std::uint32_t data, std::uint32_t /*extra*/)
{
    using namespace NC2Lifetime;
    using namespace NC2MainMenuGuiManager;
    using NMasterControlProgram::CMasterControlProgram;
    using NMasterControlProgram::TProgramState;
    if (message != 0x800) return true;
    auto* gui = CGuiManager::sTheObjectPtr;
    auto* master = CMasterControlProgram::sTheObjectPtr;
    auto changeScreen = [gui](std::int32_t screen, bool shutdown = false) { gui->Screen_ChangeTo(screen, shutdown); };
    auto startup = [master]() { return reinterpret_cast<NC2Logic::SGameControlStartupData*>(reinterpret_cast<std::byte*>(master) + 0x2C); };
    auto failNetwork = [gui, &changeScreen](const char* error) { changeScreen(18, true); gui->Debug_ErrorString_Add(error); };
    switch (command)
    {
        case 0x1388: changeScreen(2); break;
        case 0x1389:
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("game_start_post_picture_sequence", false, 0, nullptr);
            master->System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
            break;
        case 0x138A: changeScreen(1); break;
        case 0x138B: changeScreen(3); break;
        case 0x138C: changeScreen(9); break;
        case 0x138D:
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove("intro_skip");
            master->System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
            break;
        case 0x138E: break;
        case 0x138F: changeScreen(22); break;
        case 0x1390: if (MainMenu_HasGameCD()) changeScreen(1); break;
        case 0x1391: CGuiManager::mStaticVars.unknown005694d8[0] = std::byte{1}; changeScreen(1); break;
        case 0x1392: changeScreen(4); break;
        case 0x1393: changeScreen(5); break;
        case 0x1394: changeScreen(8); break;
        case 0x1395: changeScreen(6); break;
        case 0x1396:
        case 0x1397:
            startup()->Reset();
            Write<std::uint32_t>(startup(), 0, 1);
            std::memcpy(reinterpret_cast<std::byte*>(startup()) + 4, reinterpret_cast<const void*>(static_cast<std::uintptr_t>(data)), 16);
            if (command == 0x1397) changeScreen(7);
            else master->System_ProgramState_ChangeNextTimePossible(TProgramState::Game, false);
            break;
        case 0x1398:
            startup()->Reset();
            Write<std::uint32_t>(startup(), 0, 2);
            Write(startup(), 0x14, data);
            master->System_ProgramState_ChangeNextTimePossible(TProgramState::Game, false);
            break;
        case 0x1399: NC2Logic::gSavegamesManager->SortType_Set(2); gui->Screen_UpdateGui(); break;
        case 0x139A: NC2Logic::gSavegamesManager->SortType_Set(1); gui->Screen_UpdateGui(); break;
        case 0x139B: NC2Logic::gSavegamesManager->SortType_Set(3); gui->Screen_UpdateGui(); break;
        case 0x139C: Write<std::uint32_t>(master, 0x148, 0); gui->Screen_UpdateGui(); break;
        case 0x139D: Write<std::uint32_t>(master, 0x148, 1); gui->Screen_UpdateGui(); break;
        case 0x139E: Write<std::uint32_t>(master, 0x148, 2); gui->Screen_UpdateGui(); break;
        case 0x139F: master->System_ProgramState_ChangeNextTimePossible(TProgramState::PreIngameFmv, false); break;
        case 0x13A0: changeScreen(1, true); break;
        case 0x13A1:
            if (std::strlen(NNetwork::Network_GetCallsign()) == 0) gui->textInput.StartEditing();
            else changeScreen(10);
            break;
        case 0x13A2:
            if (gui->Network_SelectServiceProvider(data)) changeScreen(11);
            else failNetwork("Could not switch into select host or client mode");
            break;
        case 0x13A3:
            if (gui->Network_HostDefaultSession()) changeScreen(14);
            else failNetwork("Could not switch into select host or client mode");
            break;
        case 0x13A4:
            if (gui->Network_GetSelectedServiceProvider() == nullptr) failNetwork("No service provider selected");
            else if (gui->Network_SelectedProviderIsTcpIp()) changeScreen(12);
            else if (gui->Network_StartSessionSearch(gui->Network_GetSessionName())) changeScreen(13);
            else failNetwork("Could not switch to enum sessions mode");
            break;
        case 0x13A5:
            if (gui->Network_StartSessionSearch(gui->Network_GetSessionName())) changeScreen(13);
            else failNetwork("Could not switch to enum sessions mode");
            break;
        case 0x13A6:
            if (gui->Network_JoinSession(data)) changeScreen(14);
            else failNetwork("Could not switch into wait for game start mode");
            break;
        case 0x13A7: gui->Network_SetLobbySelection(1, true); break;
        case 0x13A8: gui->Network_SetLobbySelection(2, true); break;
        case 0x13A9: gui->Network_SetLobbySelection(0, true); break;
        case 0x13AA: gui->Network_SelectMap(*reinterpret_cast<const NC2Logic::SCleanMapGuid*>(static_cast<std::uintptr_t>(data))); gui->Network_SetLobbySelection(0, true); break;
        case 0x13AB: gui->Network_SetLobbyDifficulty(0); break;
        case 0x13AC: gui->Network_SetLobbyDifficulty(1); break;
        case 0x13AD: gui->Network_SetLobbyDifficulty(2); break;
        case 0x13AE: gui->Network_SelectSavegame(data); gui->Network_SetLobbySelection(0, true); break;
        case 0x13AF: gui->Network_SetAllowConnections(!gui->Network_AllowsConnections()); break;
        case 0x13B0:
        {
            char error[200];
            if (!gui->Network_CanStartGame(error)) gui->Chat_AddText(2, error);
            else
            {
                GetNetworkInstance()->SetAllowConnections(true);
                gui->Network_KickUnassignedPlayers();
                changeScreen(15);
            }
            break;
        }
        case 0x13B1: gui->Network_SelectPlayer(data); break;
        case 0x13B2: if (NNetwork::Network_IsHost()) gui->Network_KickSelectedPlayer(); break;
        case 0x13B3: if (NNetwork::Network_IsHost()) gui->Network_AssignSelectedPlayer(data); break;
        case 0x13B4: if (NNetwork::Network_IsHost()) gui->Network_ChangePlayerType(data); break;
        case 0x13B5: if (NNetwork::Network_IsHost()) gui->Network_ChangePlayerColor(data); break;
    }
    return true;
}
