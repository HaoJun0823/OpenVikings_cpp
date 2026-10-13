#include "ReadableValues.h"
#include "XResultIdentifiers.h"
#include "MainMenuGuiManager.h"
#include "C2LifetimeAccess.h"
#include "C2MapGuid.h"
#include "GuiToolsBase.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"

#include <cstring>
#include <mmsystem.h>

namespace NC2MainMenuGuiManager
{
    using namespace NC2Lifetime;
    static constexpr GUID cTcpIpProvider = { 0xEBFE7BA0u, 0x628Du, 0x11D2u, { 0xAE, 0x0F, 0, 0x60, 0x97, 0xB0, 0x14, 0x11 } };

    // Original function: FUN_004d757e; 0x14-byte summary records have a count cookie.
    static void* DestroySessionSummaries(void* records, std::uint32_t flags)
    {
        auto* bytes = static_cast<std::byte*>(records);
        void* allocation = records;
        if ((flags & 2u) != 0)
        {
            allocation = bytes - 4;
            const auto count = Read<std::uint32_t>(allocation, 0);
            auto index = count;
            while (index != 0)
            {
                void* record = bytes + --index * 0x14u;
                if (auto* name = Read<char*>(record, 0))
                {
                    NXBasics::Runtime_Free(name);
                    Write<char*>(record, 0, nullptr);
                }
            }
        }
        else if (auto* name = Read<char*>(records, 0))
        {
            NXBasics::Runtime_Free(name);
            Write<char*>(records, 0, nullptr);
        }
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(allocation);
        return allocation;
    }

    // Original function: FUN_004d70b6; Windows-only provider list, ten 0x78-byte rows.
    bool CGuiManager::Network_EnumerateServiceProviders()
    {
        auto* network = GetNetworkInstance();
        DPN_SERVICE_PROVIDER_INFO* providers = nullptr;
        DWORD count = 0;
        if (!network->GetServiceProviders(&providers, &count, false))
        {
            Debug_ErrorString_Add("Cannot get service providers");
            if (providers != nullptr) NXBasics::Runtime_Free(providers);
            return false;
        }
        Write<std::uint32_t>(this, 0x68, 0);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const auto& provider = providers[index];
            if (std::memcmp(&provider.guid, &cTcpIpProvider, sizeof(GUID)) != 0) continue;
            char name[200];
            NNetwork::ConvertNetworkWideToAnsi(name, provider.pwszName, -1);
            const auto row = Read<std::uint32_t>(this, 0x68);
            auto* destination = reinterpret_cast<std::byte*>(this) + 0x6C + row * 0x78u;
            std::strncpy(reinterpret_cast<char*>(destination), name, 100);
            std::memcpy(destination + 0x68, &provider.guid, sizeof(GUID));
            Write(this, 0x68, row + 1u);
            if (Read<std::uint32_t>(this, 0x68) == 10) break;
        }
        if (providers != nullptr) NXBasics::Runtime_Free(providers);
        return true;
    }

    // Original function: FUN_004d71f5.
    bool CGuiManager::Network_HostDefaultSession()
    {
        char name[400];
        std::strcpy(name, NC2GuiToolsBase::StringTool_GetMainMenuStringPtr(0xB4));
        std::strcat(name, " - ");
        std::strcat(name, NNetwork::Network_GetCallsign());
        std::strcat(name, " - ");
        char version[100]; NXBasics::Runtime_PrintString(version, "V%d.%2.2d", 1, 0);
        std::strcat(name, version);
        return Network_HostSession(name, 10);
    }

    // Original function: FUN_004d7286; Windows CDPNet host data has a 0x20-byte layout.
    bool CGuiManager::Network_HostSession(const char* sessionName, std::uint32_t maximumPlayers)
    {
        const char* error;
        if (Read<std::uint32_t>(this, 0x51C) == ReadableValues::IntegerRange::UInt32Maximum) error = "Host start session - no service provider selected";
        else
        {
            auto* network = GetNetworkInstance();
            wchar_t callsign[200]; NNetwork::ConvertNetworkAnsiToWide(callsign, NNetwork::Network_GetCallsign(), -1);
            wchar_t name[200]; NNetwork::ConvertNetworkAnsiToWide(name, sessionName, -1);
            NNetwork::SHostSessionData hostData; std::memset(&hostData, 0, sizeof(hostData)); std::memset(&hostData, 0, sizeof(hostData));
            hostData.maximumPlayers = maximumPlayers; hostData.sessionName = name; hostData.password = nullptr;
            hostData.clientServer = 1; hostData.application = NNetwork::gNetworkApplicationGuid;
            const auto* provider = reinterpret_cast<const GUID*>(reinterpret_cast<const std::byte*>(this) + 0xD4 + Read<std::uint32_t>(this, 0x51C) * 0x78u);
            if (!network->HostSession(provider, callsign, &hostData, false)) error = "Host start session - could not host session";
            else if (!NNetwork::AttachDynamicNetwork(&gDynamicNetworkData, network)) error = "Host start session - could not attach to network";
            else
            {
                auto* destination = reinterpret_cast<char*>(this) + 0xA3C;
                std::strncpy(destination, sessionName, 99); destination[99] = 0;
                return true;
            }
        }
        Debug_ErrorString_Add(error);
        return false;
    }

    // Original function: FUN_004d73e5; the empty-address branch tests != 0, not SUCCEEDED.
    bool CGuiManager::Network_StartSessionSearch(const char* address)
    {
        const char* error;
        if (Read<std::uint32_t>(this, 0x51C) == ReadableValues::IntegerRange::UInt32Maximum) error = "Join session - no service provider selected";
        else
        {
            auto* network = GetNetworkInstance();
            if (address != nullptr && std::strlen(address) != 0)
            {
                wchar_t wideAddress[102]; NNetwork::ConvertNetworkAnsiToWide(wideAddress, address, -1);
                const auto* provider = reinterpret_cast<const GUID*>(reinterpret_cast<const std::byte*>(this) + 0xD4 + Read<std::uint32_t>(this, 0x51C) * 0x78u);
                const auto result = network->StartSearchingSessions(provider, &NNetwork::gNetworkApplicationGuid, wideAddress);
                if (result >= 0) return true;
                error = result == static_cast<HRESULT>(SwitchIds::DirectXResult::Code_DPNERR_ADDRESSING) ? "Join session - passed invalid host name" : "Join session - could not start searching for sessions";
            }
            else
            {
                const auto* provider = reinterpret_cast<const GUID*>(reinterpret_cast<const std::byte*>(this) + 0xD4 + Read<std::uint32_t>(this, 0x51C) * 0x78u);
                if (network->StartSearchingSessions(provider, &NNetwork::gNetworkApplicationGuid, nullptr) != 0) return true;
                error = "Join session - could not start searching for sessions";
            }
        }
        Debug_ErrorString_Add(error);
        return false;
    }

    // Original function: FUN_004d75eb; the copied session is released on both join paths.
    bool CGuiManager::Network_JoinSession(std::uint32_t index)
    {
        auto* network = GetNetworkInstance();
        wchar_t callsign[200]; NNetwork::ConvertNetworkAnsiToWide(callsign, NNetwork::Network_GetCallsign(), -1);
        const auto offset = index * 0x78u;
        const auto* guid = reinterpret_cast<const GUID*>(reinterpret_cast<const std::byte*>(this) + offset + 0x5F4);
        auto* session = NNetwork::CopySessionNode(network->sessions, guid);
        const bool joined = network->JoinSession(callsign, session);
        if (!joined) Debug_ErrorString_Add("Join select session - could not join");
        if (session != nullptr) { ReleaseSessionNodeResources(session); NXBasics::Runtime_Free(session); }
        if (!joined) return false;
        if (!NNetwork::AttachDynamicNetwork(&gDynamicNetworkData, network))
        {
            Debug_ErrorString_Add("Join select session - could not attach to network");
            return false;
        }
        auto* destination = reinterpret_cast<char*>(sTheObjectPtr) + 0xA3C;
        const auto* name = reinterpret_cast<const char*>(this) + offset + 0x58C;
        std::strncpy(destination, name, 99); destination[99] = 0;
        return true;
    }

    // Original function: FUN_004d74a4; false means no changed session list.
    bool CGuiManager::Network_UpdateSessionList()
    {
        auto* network = GetNetworkInstance();
        auto* sessions = network->sessions;
        const auto changed = sessions->flags;
        sessions->flags = 0;
        if (changed == 0) return false;
        auto* storage = reinterpret_cast<std::byte*>(this) + 0x588;
        std::memset(storage, 0, 1204);
        NNetwork::SSessionSummary* records = nullptr;
        DWORD count = 0;
        NNetwork::GetSessionSummaries(sessions, &records, &count);
        const auto shown = (count < 10u ? count : 10u);
        Write(this, 0x588, shown);
        for (DWORD index = 0; index < shown; ++index)
        {
            auto* record = reinterpret_cast<std::byte*>(records) + index * 0x14u;
            char name[200];
            NNetwork::ConvertNetworkWideToAnsi(name, Read<wchar_t*>(record, 0), -1);
            auto* destination = storage + 4 + index * 0x78u;
            std::strncpy(reinterpret_cast<char*>(destination), name, 100);
            std::memcpy(destination + 0x68, record + 4, sizeof(GUID));
        }
        if (records != nullptr) DestroySessionSummaries(records, 3);
        return true;
    }

    // Original function: FUN_004d76cc.
    bool CGuiManager::Network_UpdatePlayerList()
    {
        auto* network = GetNetworkInstance();
        std::memset(reinterpret_cast<std::byte*>(this) + 0xAA4, 0, 1092);
        NNetwork::SPlayerInfo* players = nullptr;
        DWORD count = 0;
        if (!network->GetPlayerInfo(&players, &count))
        {
            Debug_ErrorString_Add("Update player list - could not get player info");
            return false;
        }
        const auto shown = (count < 10u ? count : 10u);
        Write(this, 0xAA4, shown);
        for (DWORD index = 0; index < shown; ++index)
        {
            char name[200];
            NNetwork::ConvertNetworkWideToAnsi(name, players[index].playerName, -1);
            auto* destination = reinterpret_cast<std::byte*>(this) + 0xAA8 + index * 0x6Cu;
            std::strcpy(reinterpret_cast<char*>(destination), name);
            Write(destination, 0x66, players[index].isHost);
            Write(destination, 0x68, players[index].playerId);
        }
        NXBasics::Runtime_Free(players);
        Write(this, 0xEE0, network->playerChangeCount);
        const auto selected = Read<DPNID>(this, 0xEE4);
        if (selected != 0)
        {
            DWORD index = 0;
            for (; index < Read<std::uint32_t>(this, 0xAA4); ++index)
                if (Read<DPNID>(this, 0xB10 + index * 0x6Cu) == selected) break;
            if (index == Read<std::uint32_t>(this, 0xAA4)) Write<DPNID>(this, 0xEE4, 0);
        }
        return true;
    }

    // Original function: FUN_004d782f.
    bool CGuiManager::Network_PlayerListHasChanged() const
    {
        auto* network = GetNetworkInstance();
        return Read<std::uint32_t>(this, 0xEE0) != network->playerChangeCount;
    }

    // Original function: FUN_004d7849; no bounds check on the selected index.
    bool CGuiManager::Network_SelectedProviderIsTcpIp() const
    {
        const auto selected = Read<std::uint32_t>(this, 0x51C);
        if (selected == ReadableValues::IntegerRange::UInt32Maximum) return false;
        const auto address = reinterpret_cast<std::uintptr_t>(this) + selected * 0x78u + 0xD4u;
        if (address == 0) return false;
        return std::memcmp(reinterpret_cast<const void*>(address), &cTcpIpProvider, sizeof(GUID)) == 0;
    }

    // Original function: FUN_004d7aa7.
    void CGuiManager::Network_SendLobbyData()
    {
        if (!NNetwork::Network_IsHost()) return;
        std::uint32_t message[360 / sizeof(std::uint32_t)]{};
        message[0] = 1;
        std::memcpy(reinterpret_cast<std::byte*>(message) + 8, lobbyData, 352);
        NNetwork::SendDynamicNetworkMessage(&gDynamicNetworkData, 0, message, sizeof(message));
    }

    // Original function: FUN_004d7b06; distinct tail-forwarding entry.
    void CGuiManager::Network_InitializeCallsign() { NNetwork::Network_InitializeCallsign(); }

    // Original function: FUN_004d7b0b.
    bool CGuiManager::Network_AutoHost()
    {
        const char* error = nullptr;
        if (!StartNetworkForMenu(this)) error = "Could not init multi player module";
        else
        {
            NNetwork::Network_InitializeCallsign();
            Chat_ClearAll();
            if (!Network_EnumerateServiceProviders()) error = "Could not enumerate service provider";
            else
            {
                std::uint32_t index = 0;
                for (; index < Read<std::uint32_t>(this, 0x68); ++index)
                    if (std::memcmp(reinterpret_cast<std::byte*>(this) + 0xD4 + index * 0x78u, &cTcpIpProvider, sizeof(GUID)) == 0) break;
                if (index == Read<std::uint32_t>(this, 0x68)) error = "No TCPIP service provider found";
                else
                {
                    Write(this, 0x51C, index);
                    if (!Network_HostDefaultSession()) error = "Could not host a session";
                }
            }
        }
        if (error != nullptr) { Debug_ErrorString_Add(error); return false; }
        return true;
    }

    // Original function: FUN_004d7ba7; retains the original unslept timeout loop.
    bool CGuiManager::Network_AutoJoin(const char* address)
    {
        const char* error = nullptr;
        if (!StartNetworkForMenu(this)) error = "Could not init multi player module";
        else
        {
            NNetwork::Network_InitializeCallsign();
            Chat_ClearAll();
            if (!Network_EnumerateServiceProviders()) error = "Could not enumerate service providers";
            else
            {
                std::uint32_t index = 0;
                for (; index < Read<std::uint32_t>(this, 0x68); ++index)
                    if (std::memcmp(reinterpret_cast<std::byte*>(this) + 0xD4 + index * 0x78u, &cTcpIpProvider, sizeof(GUID)) == 0) break;
                if (index == Read<std::uint32_t>(this, 0x68)) error = "No TCPIP service provider found";
                else
                {
                    Write(this, 0x51C, index);
                    if (!Network_StartSessionSearch(address)) error = "Could not join host";
                    else
                    {
                        const DWORD start = timeGetTime();
                        for (;;)
                        {
                            if (Network_UpdateSessionList() && Read<std::uint32_t>(this, 0x588) > 0)
                            {
                                if (!Network_JoinSession(0)) error = "Could not join session on host - failed";
                                break;
                            }
                            if (timeGetTime() - start > 10000u) { error = "Could not join session on host - timeout"; break; }
                        }
                    }
                }
            }
        }
        if (error != nullptr) { Debug_ErrorString_Add(error); return false; }
        return true;
    }

    // Original function: FUN_004d7c95.
    void CGuiManager::Chat_ClearAll() { std::memset(chatHistoryStorage, 0, 2324); }

    // Original function: FUN_004d7d56.
    const char* CGuiManager::Network_GetPlayerName(std::uint32_t playerId) const
    {
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(this, 0xAA4); ++index)
            if (Read<std::uint32_t>(this, 0xB10 + index * 0x6Cu) == playerId) return reinterpret_cast<const char*>(this) + 0xAA8 + index * 0x6Cu;
        return nullptr;
    }

    // Original function: FUN_004d7db7.
    void CGuiManager::Network_FormatPlayerDescription(std::uint32_t index, char* destination) const
    {
        auto* network = GetNetworkInstance();
        const auto offset = index * 0x6Cu;
        const auto id = Read<DPNID>(this, 0xB10 + offset);
        std::strcpy(destination, reinterpret_cast<const char*>(this) + 0xAA8 + offset);
        if (Read<std::uint8_t>(this, 0xB0E + offset) != 0)
        {
            std::strcat(destination, " (");
            std::strcat(destination, NC2GuiToolsBase::StringTool_GetMainMenuStringPtr(0xBA));
            std::strcat(destination, ")");
        }
        if (id != NNetwork::Network_GetLocalPlayerId())
        {
            DPN_CONNECTION_INFO connection;
            if (network->GetConnectionData(id, &connection))
            {
                char latency[20];
                NXBasics::Runtime_PrintString(latency, " (%dms)", connection.dwRoundTripLatencyMS);
                std::strcat(destination, latency);
            }
        }
    }

    // Original function: FUN_004d8169; mode is read after the initial clear.
    void CGuiManager::Network_ClearLobbyData()
    {
        std::memset(lobbyData, 0, 352);
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            auto* row = reinterpret_cast<std::byte*>(this) + 0xF28 + index * 0x10u;
            Write<std::uint32_t>(row, 0, 0);
            Write<std::uint32_t>(row, 4, 0);
            Write<std::uint32_t>(row, 8, 0);
            Write(row, 0x0C, Read<std::uint8_t>(this, 0x1068) != 0 ? (index < 9u ? index : 9u) : 0u);
        }
        Screen_UpdateGui();
        Network_SendLobbyData();
    }

    // Original function: FUN_004d7899.
    bool CGuiManager::Network_AllowsConnections() const { return GetNetworkInstance()->allowConnections != 0; }

    // Original function: FUN_004d79a7; validates the twenty lobby player rows.
    bool CGuiManager::Network_CanStartGame(char* errorText) const
    {
        if (errorText != nullptr) errorText[0] = 0;
        auto reject = [&](std::uint32_t message)
        {
            if (errorText != nullptr) std::strcpy(errorText, NC2GuiToolsBase::StringTool_GetMainMenuStringPtr(message));
            return false;
        };
        const auto mode = Read<std::uint32_t>(this, 0xF10);
        if (mode == 0) return reject(0xB5);
        if (mode == 1 && !reinterpret_cast<const NC2Logic::SCleanMapGuid*>(lobbyData + 4)->IsValid()) return reject(0xB6);
        std::uint32_t index = 0;
        for (; index < 20; ++index) if (Read<std::uint32_t>(this, 0xF28 + index * 0x10u) != 0) break;
        if (index == 20) return reject(0xB7);
        for (index = 0; index < 20; ++index)
        {
            const auto flags = Read<std::uint32_t>(this, 0xF30 + index * 0x10u);
            if (flags != 0 && (flags & 1) == 0 && Read<std::uint32_t>(this, 0xF2C + index * 0x10u) == 0)
            {
                if (errorText != nullptr) NXBasics::Runtime_PrintString(errorText, NC2GuiToolsBase::StringTool_GetMainMenuStringPtr(0xB8), index);
                return false;
            }
        }
        for (index = 0; index < 20; ++index) if (Read<std::uint32_t>(this, 0xF28 + index * 0x10u) == NNetwork::Network_GetLocalPlayerId()) return true;
        return reject(0xB9);
    }

    // Original function: FUN_004d7d8d.
    std::uint32_t CGuiManager::Network_GetPlayerListIndex(std::uint32_t playerId) const
    {
        const auto count = Read<std::uint32_t>(this, 0xAA4);
        for (std::uint32_t index = 0; index < count; ++index) if (Read<std::uint32_t>(this, 0xB10 + index * 0x6Cu) == playerId) return index;
        return ReadableValues::IntegerRange::UInt32Maximum;
    }
}
