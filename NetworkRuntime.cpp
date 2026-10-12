#include "SwitchIdentifiers.h"
#include <winsock2.h>
#include <Windows.h>

#include "DirectXErrorText.h"
#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"
#include "MasterPropertyManager.h"
#include "FileTransferManager.h"
#include "CRC16.h"
#include "CRC32.h"

#include "dplay8.h"

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <cwchar>
#include <new>

namespace NNetwork
{
    // Original function: FUN_004DD864; padding is deliberately left untouched.
    void* CommandPacket_Construct(void* packet)
    {
        auto* bytes = static_cast<std::uint8_t*>(packet);
        const std::uint32_t zero = 0;
        std::memcpy(bytes + 4, &zero, 4);
        bytes[8] = 0;
        bytes[0] = 5;
        std::memset(bytes + 9, 0, 0xC8);
        return packet;
    }

    // Original function: FUN_004DEE2D.
    void* ChatPacket_Construct(void* packet)
    {
        auto* bytes = static_cast<std::uint8_t*>(packet);
        bytes[1] = 0;
        bytes[0] = 3;
        std::memset(bytes + 2, 0, 0x100);
        return packet;
    }

    // Original function: FUN_004DEE4E.
    void* PayloadPacket_Construct(void* packet)
    {
        auto* bytes = static_cast<std::uint8_t*>(packet);
        const std::uint32_t zero = 0;
        std::memcpy(bytes + 4, &zero, 4);
        std::memcpy(bytes + 8, &zero, 4);
        bytes[0] = 4;
        std::memset(bytes + 0x0C, 0, 0x400);
        return packet;
    }

    // Original function: FUN_004DFFDF; folded with the sound-device pointer ctor.
    SFileTransferList::SFileTransferList() { first = nullptr; }

    // Original function: FUN_004E0A2C; the original GUID at 004F7FB0 is zero.
    void* FileTransferAnnouncementPacket_Construct(void* packet)
    {
        auto* bytes = static_cast<std::uint8_t*>(packet);
        const std::uint32_t zero = 0;
        std::memcpy(bytes + 0x14, &zero, 4);
        bytes[0] = 11;
        const GUID transferId{};
        std::memcpy(bytes + 4, &transferId, sizeof(transferId));
        std::memset(bytes + 0x18, 0, 0x100);
        return packet;
    }
}

// Original data: DAT_00569650.
// Enabled at 004db9c7 after successfully opening the network log.
std::uint8_t gNetworkLogEnabled = 0;

// Original data: DAT_00569658
NNetwork::CDPNet* gNetworkInstance = nullptr;

// Original data: DAT_00569654
NNetwork::SNetLog* gNetworkLog = nullptr;

// Original data: DAT_005697b0.
std::uint8_t gDynamicNetworkUpdateStopped = 0;

// Original data: DAT_005697b4.
DPNID gPendingMissingPlayerId = 0;

// Original data: DAT_005697b8.
DPNID gLastRecoveryPlayerId = 0;

// Original data: DAT_005697bc.
std::uint32_t gProcessedPlayerChangeCount = 0;

// Original data: DAT_0050aa2c.
// Windows PE bytes are 01 00 00 00; all three exported accesses use the first byte.
std::uint8_t gDynamicNetworkInitialUpdatePending = 1;

namespace
{

    // Original storage: 0056965c and 0056976c.
    char gNetworkWindowsVersion[0x110] = {};
    char gNetworkDirectXVersion[0x40] = {};

    void ReportNetworkSystemError(const char* sourceFile, std::int32_t line, const char* additionalInformation);
    NNetwork::SNetLog* InitializeNetworkLog(NNetwork::SNetLog* object);
    void ReportDirectPlayError(const char* operation, HRESULT result);
    const char* GetNetworkWindowsVersion();
    void ReportNetworkConditionFailure(const char* sourceFile, std::int32_t line, std::int32_t condition, const char* message);
    const char* GetNetworkDirectXVersion();

    void ReportDirectPlayError(const char* operation, HRESULT result)
    {
        OutputDebugStringA(operation);
        OutputDebugStringA(OriginalStrings::DirectPlayErrorSeparator);
        OutputDebugStringA(NXBasics::GetDirectXErrorText(result));
        OutputDebugStringA(OriginalStrings::NewLine);
    }

    // Original function: FUN_004db8a5
    NNetwork::SNetLog* InitializeNetworkLog(NNetwork::SNetLog* object)
    {
        object->unknown00C = 0;
        object->unknown004 = 0;
        object->sequenceNumber = 0;
        object->writtenEntries = 0;
        object->entryLimit = 0;
        object->level = 3;
        object->error = 0;
        object->stream = nullptr;

        // formatFlags and all other bytes remain untouched.
        return object;
    }

    // Original function: FUN_004de237
    void ReportNetworkSystemError(const char* sourceFile, std::int32_t line, const char* additionalInformation)
    {
        char message[0x200] = {};
        char shortenedFile[0x100] = "...";
        char* systemMessage = nullptr;

        if (std::strlen(sourceFile) > 0x100u)
        {
            // Preserve the original offset calculation and unbounded append.
            // This branch can read before sourceFile or overflow shortenedFile.
            const std::ptrdiff_t prefixLength = static_cast<std::ptrdiff_t>(std::strlen(shortenedFile));
            const std::ptrdiff_t sourceLength = static_cast<std::ptrdiff_t>(std::strlen(sourceFile));
            const char* suffix = sourceFile + (sourceLength - prefixLength - 0x100);

            std::strcat(shortenedFile + std::strlen(shortenedFile), suffix);
        }
        else
        {
            shortenedFile[0] = '\0';
        }

        // The original selects argument 3, not sourceFile, for short filenames.
        const char* displayedFile = shortenedFile[0] != '\0' ? shortenedFile : additionalInformation;

        NXBasics::Runtime_PrintString(message, "* System error in %s (%i)\n* Error message: ", displayedFile, line);
        ::OutputDebugStringA(message);

        // Preserve the original timing: GetLastError follows the first output.
        const DWORD errorCode = ::GetLastError();
        ::FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorCode, 0x0400u, reinterpret_cast<LPSTR>(&systemMessage), 0, nullptr);

        // The original does not test whether FormatMessageA succeeded.
        ::OutputDebugStringA(systemMessage);
        ::OutputDebugStringA(OriginalStrings::NewLine);
        ::LocalFree(systemMessage);

        if (additionalInformation != nullptr)
        {
            ::OutputDebugStringA("* Additional information: ");
            ::OutputDebugStringA(additionalInformation);
            ::OutputDebugStringA(OriginalStrings::NewLine);
        }
    }

    // Original function: FUN_004de352
    const char* GetNetworkWindowsVersion()
    {
        OSVERSIONINFOEXA version = {};
        version.dwOSVersionInfoSize = 0x9C;

        if (::GetVersionExA(reinterpret_cast<OSVERSIONINFOA*>(&version)) == FALSE)
        {
            version.dwOSVersionInfoSize = 0x94;

            if (::GetVersionExA(reinterpret_cast<OSVERSIONINFOA*>(&version)) == FALSE)
                return nullptr;
        }

        switch (version.dwPlatformId)
        {
            case VER_PLATFORM_WIN32s:
                std::strcpy(gNetworkWindowsVersion, "Microsoft Win32s ");
                break;

            case VER_PLATFORM_WIN32_WINDOWS:
                if (version.dwMajorVersion > 4 || (version.dwMajorVersion == 4 && version.dwMinorVersion > 0))
                    std::strcpy(gNetworkWindowsVersion, "Microsoft Windows 98 ");
                else
                    std::strcpy(gNetworkWindowsVersion, "Microsoft Windows 95 ");

                break;

            case VER_PLATFORM_WIN32_NT:
                {
                    if (version.dwMajorVersion <= 4)
                    {
                        std::strcpy(gNetworkWindowsVersion, "Microsoft Windows NT ");
                    }
                    else if (version.dwMajorVersion == 5)
                    {
                        std::strcpy(gNetworkWindowsVersion, "Microsoft Windows 2000 ");
                    }
                    else
                    {
                        char majorVersion[10] = {};
                        std::int32_t majorVersionBits;
                        std::memcpy(&majorVersionBits, &version.dwMajorVersion, sizeof(majorVersionBits));

                        NXBasics::Runtime_LongToString(majorVersionBits, majorVersion, 10);

                        std::strcpy(gNetworkWindowsVersion, "Microsoft Windows NT ");
                        std::strcat(gNetworkWindowsVersion, majorVersion);
                    }

                    char details[0x40] = {};

                    NXBasics::Runtime_PrintString(details, "Version %d.%d %s (Build %d)", static_cast<int>(version.dwMajorVersion), static_cast<int>(version.dwMinorVersion), version.szCSDVersion, static_cast<int>(version.dwBuildNumber & 0xFFFFu));
                    std::strcat(gNetworkWindowsVersion, details);
                    break;
                }

            default:
                std::strcpy(gNetworkWindowsVersion, "Unknown");
                break;
        }

        return gNetworkWindowsVersion;
    }

}

namespace NNetwork
{
    // Original function: FUN_004dc8e3; caller owns even a failed second-pass allocation.
    bool CDPNet::GetServiceProviders(DPN_SERVICE_PROVIDER_INFO** providers, DWORD* count, bool allProviders)
    {
        if (peer == nullptr) return false;
        *providers = nullptr; *count = 0;
        DWORD bytes = 0;
        const DWORD options = allProviders ? 1u : 0u;
        HRESULT result = peer->EnumServiceProviders(nullptr, nullptr, *providers, &bytes, count, options);
        if (result != static_cast<HRESULT>(0x80158100u))
        {
            ReportDirectPlayError("CDPNet::GetServiceProviders(): IDirectPlay8Peer::EnumServiceProviders 1", result);
            return false;
        }
        *providers = static_cast<DPN_SERVICE_PROVIDER_INFO*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x31D, *providers != nullptr, "Memory allocation failed.");
        // The original clears the provider count in bytes, not the buffer size.
        std::memset(*providers, 0, *count);
        result = peer->EnumServiceProviders(nullptr, nullptr, *providers, &bytes, count, options);
        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::GetServiceProviders(): IDirectPlay8Peer::EnumServiceProviders 2", result);
            return false;
        }
        return true;
    }

    // Original function: FUN_004dcaba.
    bool CDPNet::GetConnectionData(DPNID playerId, DPN_CONNECTION_INFO* connection)
    {
        if ((flags & 2u) != 2u && (flags & 8u) != 8u) return false;
        std::memset(connection, 0, sizeof(*connection)); connection->dwSize = sizeof(*connection);
        const HRESULT result = peer->GetConnectionInfo(playerId, connection, 0);
        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::GetConnectionData(): IDirectPlay8Peer::GetConnectionInfo", result);
            return false;
        }
        return true;
    }

    // Original function: FUN_004dded2; circular list, original count cookie and holes.
    void GetSessionSummaries(SSessionList* list, SSessionSummary** summaries, DWORD* count)
    {
        if (list->first == nullptr) { *summaries = nullptr; *count = 0; return; }
        *count = list->count;
        const auto records = list->count;
        auto* allocation = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(records * 0x14u + 4u, 1));
        SSessionSummary* array = nullptr;
        if (allocation != nullptr)
        {
            std::memcpy(allocation, &records, 4); array = reinterpret_cast<SSessionSummary*>(allocation + 4);
            if (static_cast<std::int32_t>(records - 1u) >= 0)
                for (std::uint32_t index = 0; index < records; ++index) { array[index].name = nullptr; std::memset(&array[index].instance, 0, sizeof(GUID)); }
        }
        *summaries = array;
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0x109, array != nullptr, "Memory allocation failed.");
        std::memset(array, 0, list->count * 0x14u);
        std::uint32_t index = 0;
        SSessionNode* node = list->first;
        do
        {
            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0x114, list->count >= index, "Array index overflow.");
            if (node->invalid == 0)
            {
                char suffix[128]{}; wchar_t wideSuffix[128]{};
                wchar_t* name = NXBasics::Runtime_DuplicateWideString(node->description->pwszSessionName);
                if (node->description->dwMaxPlayers > 0) ::wsprintfA(suffix, " (%d/%d) (%d ms)", node->description->dwCurrentPlayers, node->description->dwMaxPlayers, node->responseTime);
                else ::wsprintfA(suffix, " (%d) (%d ms)", node->description->dwCurrentPlayers, node->responseTime);
                CopyNetworkAnsiToWide(wideSuffix, suffix, -1);
                const auto length = static_cast<std::uint32_t>(std::wcslen(name)) + static_cast<std::uint32_t>(std::wcslen(wideSuffix));
                auto* combined = static_cast<wchar_t*>(NXBasics::Runtime_AllocateWithNewHandler(length * 2u + 2u, 1));
                combined[0] = 0; std::wcscat(combined, name); std::wcscat(combined, wideSuffix);
                NXBasics::Runtime_Free(name);
                array[index].name = NXBasics::Runtime_DuplicateWideString(combined);
                array[index].instance = node->description->guidInstance;
                NXBasics::Runtime_Free(combined); ++index;
            }
            node = node->next;
        } while (node != list->first);
    }

    // Original function: FUN_004dea48; reset is followed by the original first update.
    void ResetDynamicNetworkData(SDynamicNetworkData* data)
    {
        WriteNetworkLog(GetNetworkLog(), "Reset logic module", 0);
        ReleaseDynamicNetworkMemory(data);
        data->networkTick = 0; data->allocation10 = nullptr; data->allocation28 = nullptr;
        data->receivedRecordCount = 0; data->pendingDataMessageCount = 0; data->allocation30 = nullptr;
        data->waitStartTick = 0; data->unknown7C = 0;
        data->allocation30 = NXBasics::Runtime_AllocateWithNewHandler(0x10300, 1);
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x8D, data->allocation30 != nullptr, "Memory allocation failed.");
        std::memset(data->allocation30, 0, 0x10300);
        UpdateDynamicNetwork(data, true);
    }

    // Original function: FUN_004deb32.
    bool AttachDynamicNetwork(SDynamicNetworkData* data, CDPNet* network)
    {
        if ((network->flags & 8u) != 8u && (network->flags & 2u) != 2u)
        {
            ReleaseDynamicNetworkMemory(data); data->active = 0;
            return false;
        }
        if (data->active != 0) ShutDownDynamicNetworkData(data);
        ResetDynamicNetworkData(data);
        data->playerCount = 0;
        if (data->allocation10 != nullptr) { NXBasics::Runtime_Free(data->allocation10); data->allocation10 = nullptr; }
        data->active = network->GetPlayerInfo(reinterpret_cast<SPlayerInfo**>(&data->allocation10), &data->playerCount);
        if (data->active != 0)
        {
            char message[100]{};
            const auto playerId = network->GetLocalPlayerId();
            NXBasics::Runtime_PrintString(message, "Logic module is attached to the network. Local DPNID is 0x%x", playerId);
            WriteNetworkLog(GetNetworkLog(), message, 0);
        }
        else WriteNetworkLog(GetNetworkLog(), "Cannot attach logic module to network because player data is unavailable.", 0);
        if (data->active == 0) ReleaseDynamicNetworkMemory(data);
        return data->active != 0;
    }
}

namespace NNetwork
{
    // Original function: FUN_004de4c4
    bool GetNetworkIPAddress(std::uint32_t index, char* destination, std::uint32_t capacity)
    {
        WSADATA socketData;

        if (::WSAStartup(0x0101, &socketData) != 0)
            return false;

        bool success = false;

        if (destination != nullptr)
        {
            std::memset(destination, 0, capacity);

            char hostName[0x10] = {};

            if (::gethostname(hostName, sizeof(hostName)) != SOCKET_ERROR)
            {
                hostent* host = ::gethostbyname(hostName);

                if (host != nullptr)
                {
                    // The original indexes directly; callers enumerate from zero
                    // and stop at the first null entry.
                    const char* addressBytes = host->h_addr_list[index];

                    if (addressBytes != nullptr)
                    {
                        in_addr address;
                        std::memcpy(&address, addressBytes, sizeof(address));

                        std::strncpy(destination, ::inet_ntoa(address), capacity);
                        success = true;
                    }
                }
            }
        }

        ::WSACleanup();
        return success;
    }

}

namespace
{
    // Original function: FUN_004de569
    void ReportNetworkConditionFailure(const char* sourceFile, std::int32_t line, std::int32_t condition, const char* message)
    {
        if (condition != 0)
            return;

        char output[0x200] = {};
        char sourcePath[0x104] = {};
        char drive[3] = {};
        char directory[0x100] = {};
        char baseName[0x100] = {};
        char extension[0x100] = {};

        if (sourceFile != nullptr)
        {
            char lineText[0x10] = {};

            // The original does not force termination after this bounded copy.
            std::strncpy(sourcePath, sourceFile, 0x104);
            NXBasics::Runtime_SplitPath(sourcePath, drive, directory, baseName, extension);

            std::strcpy(output, baseName);
            std::strcat(output, extension);
            std::strcat(output, ": Line ");

            // FUN_004e8ff2 uses the same signed decimal conversion as FUN_004e907b.
            NXBasics::Runtime_LongToString(line, lineText, 10);
            std::strcat(output, lineText);
            std::strcat(output, "\n\n");
        }

        std::strcat(output, message);

        ::MessageBoxA(nullptr, output, "Network Error", MB_OK | MB_ICONWARNING);
    }

    // Original function: FUN_004de6d8
    const char* GetNetworkDirectXVersion()
    {
        HKEY key = nullptr;

        if (::RegOpenKeyExA(HKEY_LOCAL_MACHINE, "Software\\Microsoft\\DirectX", 0, KEY_QUERY_VALUE, &key) != ERROR_SUCCESS)
            return nullptr;

        if (key == nullptr)
            return nullptr;

        const char* result = nullptr;
        DWORD valueType = 0;
        DWORD valueBytes = 0;

        if (::RegQueryValueExA(key, "Version", nullptr, nullptr, nullptr, &valueBytes) == ERROR_SUCCESS)
        {
            // Original allocation wrapper FUN_004e5ddd requests new-handler use.
            char* value = static_cast<char*>(NXBasics::Runtime_AllocateWithNewHandler(valueBytes, 1));

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dputil.cpp", 0x76, value != nullptr, "Memory allocation failed.");

            // The diagnostic helper returns; the original continues afterward.
            std::memset(value, 0, valueBytes);

            const LSTATUS queryResult = ::RegQueryValueExA(key, "Version", nullptr, &valueType, reinterpret_cast<BYTE*>(value), &valueBytes);

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dputil.cpp", 0x7C, valueType == REG_SZ, "RegQueryValueEx() failed.");

            if (queryResult == ERROR_SUCCESS)
            {
                std::strncpy(gNetworkDirectXVersion, value, 0x40);
                result = gNetworkDirectXVersion;

                // Original delete wrapper FUN_004e5e48 forwards to Runtime_Free.
                NXBasics::Runtime_Free(value);
            }

            // The original does not free the allocation when the second query fails.
        }

        ::RegCloseKey(key);
        return result;
    }
}

namespace NNetwork
{

    // Original data: 005097a8; initial bytes verified in the earlier PE review.
    GUID gNetworkApplicationGuid = { 0xAC671273u, 0x85B1u, 0x4C11u, { 0xAC, 0x54, 0x16, 0x6C, 0x39, 0x9C, 0x80, 0x78 } };

    namespace
    {
        // Original data: DAT_005697c0, DAT_005697dc and DAT_005697e4.
        SFileTransferInfo gFileTransferInfo = {};
        std::uint8_t gFileTransferInfoInitializationFlags = 0;
        CFileTransferManager* gFileTransferManager = nullptr;

        struct SFileTransferMetadataPacket
        {
            std::uint8_t type;
            std::byte reserved01[3];
            std::byte transferId[0x10];
            std::uint32_t checksum;
            std::uint32_t fileSize;
        };

        static_assert(sizeof(SFileTransferMetadataPacket) == 0x1C);
        static_assert(offsetof(SFileTransferMetadataPacket, checksum) == 0x14);
        static_assert(offsetof(SFileTransferMetadataPacket, fileSize) == 0x18);

        void ReleaseFileTransferInfoStrings(SFileTransferInfo* info);
        int __cdecl DestroyFileTransferInfoAtExit();
        void InitializeEventQueue(NNetwork::SEventQueue* queue);
        SEventQueue* AddEventQueueEntry(SEventQueue* queue, DPNID playerId, std::uint32_t timestamp, const wchar_t* playerName, std::uint32_t eventType);
        void InsertSessionNode(SSessionList* list, SSessionNode* node);
        SSessionNode* ConstructSessionNode(SSessionNode* node);
        char* TransformNetworkEasterEggText(const char* source, char* destination);

        // Original function: FUN_004db429
        void InitializeEventQueue(NNetwork::SEventQueue* queue)
        {
            queue->count = 0;
            queue->last = nullptr;
            queue->first = nullptr;
            queue->changed = 0;
        }

        // Original function: FUN_004db558
        SEventQueue* AddEventQueueEntry(SEventQueue* queue, DPNID playerId, std::uint32_t timestamp, const wchar_t* playerName, std::uint32_t eventType)
        {
            if (queue->count == 0x100u)
            {
                SEventNode* oldest = queue->first;
                queue->first = oldest->next;

                NXBasics::Runtime_Free(oldest);
                queue->changed = 1;

                // Preserve the original behavior: count is not decremented.
            }

            auto* node = static_cast<SEventNode*>(NXBasics::Runtime_AllocateWithNewHandler(sizeof(SEventNode), 1));

            if (node != nullptr)
            {
                std::memset(node, 0, sizeof(SEventNode));
            }

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netevent.cpp", 0xAF, node != nullptr, "Memory allocation failed.");

            node->playerId = playerId;
            node->timestamp = timestamp;

            const wchar_t* source = playerName != nullptr ? playerName : L"Unknown player";
            wchar_t* destination = node->playerName;

            // Original call: FUN_004e8a3c, an unbounded wide-string copy.
            do
            {
                *destination++ = *source;
            } while (*source++ != L'\0');

            node->eventType = eventType;

            if (queue->first == nullptr)
            {
                queue->first = node;
                queue->last = node;
                node->next = nullptr;
            }
            else
            {
                queue->last->next = node;
                queue->last = node;
            }

            ++queue->count;
            return queue;
        }

        // Original function: FUN_004ddcd0
        void InsertSessionNode(SSessionList* list, SSessionNode* node)
        {
            if (list->first == nullptr)
            {
                list->first = node;
                node->previous = node;
                node->next = node;
            }
            else
            {
                SSessionNode* first = list->first;

                node->next = first->next;
                node->previous = first;
                first->next->previous = node;
                first->next = node;
            }

            ++list->count;
            list->flags |= 0x01u;
        }

        // Original function: FUN_004de0e4
        SSessionNode* ConstructSessionNode(SSessionNode* node)
        {
            node->invalid = 0;
            node->lastResponseTime = 0;
            node->responseTime = 0;
            node->address10 = nullptr;
            node->address0C = nullptr;
            node->description = nullptr;
            node->previous = nullptr;
            node->next = nullptr;

            // The original constructor does not initialize padding bytes.
            return node;
        }

        // Original function: FUN_004dff82
        char* TransformNetworkEasterEggText(const char* source, char* destination)
        {
            while (*source != '\0')
            {
                const auto value = static_cast<unsigned char>(*source++);
                *destination++ = static_cast<char>(value ^ 0xEFu);
            }

            *destination = '\0';
            return destination;
        }

        // Original function: FUN_004dffb8
        void ReleaseFileTransferInfoStrings(SFileTransferInfo* info)
        {
            if (info->fileName != nullptr)
            {
                NXBasics::Runtime_Free(info->fileName);
                info->fileName = nullptr;
            }

            if (info->path24 != nullptr)
            {
                NXBasics::Runtime_Free(info->path24);
                info->path24 = nullptr;
            }
        }

        // Original function: FUN_004e0397
        int __cdecl DestroyFileTransferInfoAtExit()
        {
            ReleaseFileTransferInfoStrings(&gFileTransferInfo);

            // The reconstructed onexit interface expects an int return value.
            return 0;
        }
    }

    // Original function: FUN_004058fd
    bool Network_StartUp()
    {
        // Original data: 004f3378.
        static constexpr GUID applicationGuid = { 0xA7F93099u, 0x9BCFu, 0x4145u, { 0xAB, 0x6A, 0x57, 0xCD, 0xFC, 0x0A, 0x84, 0x19 } };

        CDPNet* network = GetNetworkInstance();
        network->SetApplicationGuid(&applicationGuid);

        const bool initialized = network->Initialize(nullptr, false);

        const char* logFileName = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetStringValuePtr("multiplayer_log");

        // Logging is configured even when DirectPlay initialization failed.
        if (logFileName != nullptr)
            OpenNetworkLog(GetNetworkLog(), logFileName, 3, 0);

        return initialized;
    }

    // Original function: FUN_004db4a0
    SEventQueue* AppendEventQueueCopy(SEventQueue* destination, const SEventQueue* source)
    {
        destination->changed = source->changed;

        for (const SEventNode* node = source->first; node != nullptr; node = node->next)
        {
            const auto* bytes = reinterpret_cast<const std::byte*>(node);

            DPNID playerId;
            std::uint32_t timestamp;
            std::int32_t eventType;

            std::memcpy(&playerId, bytes, sizeof(playerId));
            std::memcpy(&timestamp, bytes + 0x204, sizeof(timestamp));
            std::memcpy(&eventType, bytes + 0x208, sizeof(eventType));

            const auto* playerName = reinterpret_cast<const wchar_t*>(bytes + 4);

            AddEventQueueEntry(destination, playerId, timestamp, playerName, eventType);
        }

        return destination;
    }

    // Original function: FUN_004db43a; distinct from replacement/assignment.
    SEventQueue* ConstructEventQueueAndTakeSource(SEventQueue* destination, SEventQueue* source)
    {
        destination->count = 0;
        destination->last = nullptr;
        destination->first = nullptr;
        destination->changed = 0;
        ClearEventQueue(destination);
        destination->count = source->count;
        destination->changed = source->changed;
        for (auto* event = source->first; event != nullptr; event = event->next)
            AddEventQueueEntry(destination, event->playerId, event->timestamp, event->playerName, event->eventType);
        ClearEventQueue(source);
        return destination;
    }

    // Original function: FUN_004db635.
    // Complete thirteen-entry jump table at 004DB754.
    void FormatNetworkEvent(const SEventQueue* queue, const SEventNode* event, char* destination, std::int32_t appendLimit)
    {
        (void)queue;
        if (event == nullptr || destination == nullptr || appendLimit <= 0)
        {
            // Original invalid-input branch still dereferences destination.
            destination[0] = '\0';
            return;
        }
        if (event->playerName[0] != L'\0')
        {
            char playerName[64]{};
            CopyNetworkWideToAnsi(playerName, event->playerName, -1);
            std::strcat(destination, playerName);
        }
        char playerIdText[16] = " (0x";
        NXBasics::Runtime_LongToString(static_cast<std::int32_t>(event->playerId), playerIdText + 4, 16);
        std::strncat(destination, playerIdText, static_cast<std::uint32_t>(appendLimit));
        static constexpr const char* eventText[] = {
            ") got an error.",
            ") joined the session.",
            ") disconnected normally.",
            ") was kicked by the host.",
            ") lost the connection.",
            ") disconnected because the session was terminated.",
            ") disconnected but I don't know why...",
            ") is new host.",
            ") stopped responding.",
            ") stopped responding. Connection to server lost.",
            ") did either not sent a message or the message could not be received.",
            ") has reached a timeout.",
            ") lost connection. Waiting for DirectPlay service to confirm."
        };
        const std::uint32_t index = event->eventType - 1u;
        const char* text = index < 13 ? eventText[index] : ") generated an unknown event.";
        // The original limit applies separately to each append, not to total size.
        std::strncat(destination, text, static_cast<std::uint32_t>(appendLimit));
    }

    // Original function: FUN_004db4dd
    SEventQueue* ReplaceEventQueueAndClearSource(SEventQueue* destination, SEventQueue* source)
    {
        ClearEventQueue(destination);
        AppendEventQueueCopy(destination, source);
        ClearEventQueue(source);

        return destination;
    }

    // Original function: FUN_004db788
    void ConvertNetworkAnsiToWide(wchar_t* destination, const char* source, std::int32_t length)
    {
        if (destination == nullptr || source == nullptr)
            return;

        if (length == -1)
            length = static_cast<std::int32_t>(std::strlen(source)) + 1;

        // Preserve the original capacity and explicit terminator placement.
        ::MultiByteToWideChar(CP_ACP, 0, source, -1, destination, length - 1);
        destination[length - 1] = L'\0';
    }

    // Original function: FUN_004db7cc
    void ConvertNetworkWideToAnsi(char* destination, const wchar_t* source, std::int32_t length)
    {
        if (destination == nullptr || source == nullptr)
            return;

        if (length == -1)
            length = static_cast<std::int32_t>(std::wcslen(source)) + 1;

        // Preserve the original capacity and explicit terminator placement.
        ::WideCharToMultiByte(CP_ACP, 0, source, -1, destination, length - 1, nullptr, nullptr);
        destination[length - 1] = '\0';
    }

    // Original function: FUN_004db811
    void CopyNetworkAnsiToWide(wchar_t* destination, const char* source, std::int32_t length)
    {
        if (destination == nullptr || source == nullptr)
            return;

        ConvertNetworkAnsiToWide(destination, source, length);
    }

    // Original function: FUN_004db833
    void CopyNetworkWideToAnsi(char* destination, const wchar_t* source, std::int32_t length)
    {
        if (destination == nullptr || source == nullptr)
            return;

        ConvertNetworkWideToAnsi(destination, source, length);
    }

    // Original function: FUN_004dc31e
    CDPNet::CDPNet()
    {
        InitializeEventQueue(&events18);
        InitializeEventQueue(&events28);

        std::memset(data48, 0, sizeof(data48));

        // Original stores at +0x64, +0x60, +0x5C and +0x58.
        std::memset(data58, 0, sizeof(data58));

        // Copy the complete 0x124-byte log object, including untouched bytes.
        std::memcpy(&log, GetNetworkLog(), sizeof(log));

        peer = nullptr;
        interface70 = nullptr;
        interface6C = nullptr;

        callbackContext = nullptr;
        flags = 0;
        sessions = nullptr;
        searchTimer = nullptr;
        searchOperation = 0;

        playerInfoChanged = 0;
        allowConnections = 0;

        receivedMessageCount = 0;
        receivedMessages = nullptr;
        playerChangeCount = 0;

        // Critical sections and the remaining fields are not initialized here.
    }

    // Original function: FUN_004dc3f3
    void CDPNet::SetApplicationGuid(const GUID* applicationGuid)
    {
        // The original writes shared storage, not a member of this object.
        std::memcpy(&gNetworkApplicationGuid, applicationGuid, 0x10);
    }

    // Original function: FUN_004dc45f
    bool CDPNet::Initialize(void* context, bool option)
    {
        // The original reads callbackContext instead of the supplied argument.
        (void) context;

        // Original data: 004fb8c8, 004fb8b8, 004fb8e8 and 004fb8d8.
        static constexpr GUID peerClassId = { 0x286F484Du, 0x375Eu, 0x4458u, { 0xA2, 0x72, 0xB1, 0x38, 0xE2, 0xF8, 0x0A, 0x6A } };
        static constexpr GUID peerInterfaceId = { 0x5102DACFu, 0x241Bu, 0x11D3u, { 0xAE, 0xA7, 0x00, 0x60, 0x97, 0xB0, 0x14, 0x11 } };
        static constexpr GUID addressClassId = { 0x934A9523u, 0xA3CAu, 0x4BC5u, { 0xAD, 0xA0, 0xD6, 0xD9, 0x5D, 0x97, 0x94, 0x21 } };
        static constexpr GUID addressInterfaceId = { 0x83783300u, 0x4063u, 0x4C8Au, { 0x9D, 0xB3, 0x82, 0x83, 0x0A, 0x7F, 0xEB, 0x31 } };

        HRESULT result = ::CoInitialize(nullptr);

        if (FAILED(result))
        {
            // The original text says CoInitializeEx, but the import is CoInitialize.
            ReportDirectPlayError("CDPNet::Initialize(): CoInitializeEx", result);
            WriteNetworkLogLevel(3, "COM initialization failed", 0);

            // The original continues even after COM initialization failed.
        }

        result = ::CoCreateInstance(peerClassId, nullptr, 7u, peerInterfaceId, reinterpret_cast<void**>(&peer));

        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::Initialize(): CoCreateInstance", result);
            WriteNetworkLogLevel(3, "CoCreateInstance failed", 0);
            return false;
        }

        result = peer->Initialize(callbackContext, &CDPNet::MessageProc, option ? 0u : 1u);

        if (FAILED(result))
        {
            if (peer != nullptr)
            {
                peer->Release();
                peer = nullptr;
            }

            ::CoUninitialize();

            ReportDirectPlayError("CDPNet::Initialize(): IDirectPlay8Peer::Initialize", result);
            WriteNetworkLogLevel(3, "IDirectPlay8Peer initialization failed", 0);
            return false;
        }

        result = ::CoCreateInstance(addressClassId, nullptr, 7u, addressInterfaceId, reinterpret_cast<void**>(&interface6C));

        if (FAILED(result))
        {
            if (peer != nullptr)
            {
                peer->Release();
                peer = nullptr;
            }

            ::CoUninitialize();

            ReportDirectPlayError("CDPNet::Initialize(): CoCreateInstance 1", result);
            WriteNetworkLogLevel(3, "CoCreateInstance failed", 0);
            return false;
        }

        result = ::CoCreateInstance(addressClassId, nullptr, 7u, addressInterfaceId, reinterpret_cast<void**>(&interface70));

        if (FAILED(result))
        {
            if (interface6C != nullptr)
            {
                interface6C->Release();
                interface6C = nullptr;
            }

            if (peer != nullptr)
            {
                peer->Release();
                peer = nullptr;
            }

            ::CoUninitialize();

            ReportDirectPlayError("CDPNet::Initialize(): CoCreateInstance 2", result);
            WriteNetworkLogLevel(3, "CoCreateInstance failed", 0);
            return false;
        }

        auto* messages = static_cast<SReceivedMessage*>(NXBasics::Runtime_AllocateWithNewHandler(0x400u, 1));

        if (messages != nullptr)
        {
            // Original construction of 64 entries, each occupying 0x10 bytes.
            for (std::uint32_t index = 0; index < 0x40u; ++index)
                std::memset(&messages[index], 0, 0x10);
        }

        receivedMessages = messages;

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x272, receivedMessages != nullptr, "Memory allocation failed.");

        // The original clears the complete allocation again after the diagnostic.
        std::memset(receivedMessages, 0, 0x400);

        ::InitializeCriticalSection(&criticalEC);

        ::InitializeCriticalSection(&critical74);
        ::EnterCriticalSection(&critical74);
        std::memset(data48, 0, 0x10);
        ::LeaveCriticalSection(&critical74);

        ::InitializeCriticalSection(&critical8C);
        ::EnterCriticalSection(&critical8C);
        std::memset(data58, 0, 0x10);
        ::LeaveCriticalSection(&critical8C);

        ::InitializeCriticalSection(&criticalA4);
        ::InitializeCriticalSection(&criticalBC);

        initializationTick = ::timeGetTime();

        ::EnterCriticalSection(&criticalA4);
        ClearEventQueue(&events18);
        ::LeaveCriticalSection(&criticalA4);

        ::EnterCriticalSection(&criticalBC);
        ClearEventQueue(&events28);
        ::LeaveCriticalSection(&criticalBC);

        flags |= 0x01u;
        return true;
    }

    // Original function: FUN_004dc700
    bool CDPNet::InitializePeer(void* context, bool option)
    {
        // The original ignores the supplied context and reads this->callbackContext.
        (void)context;

        if (peer != nullptr)
        {
            const DWORD initializeFlags = option ? 0u : 1u;
            const HRESULT result = peer->Initialize(callbackContext, &CDPNet::MessageProc, initializeFlags);

            if (SUCCEEDED(result) || result == static_cast<HRESULT>(0x80158080u))
            {
                flags |= 0x01u;
                return true;
            }

            ReportDirectPlayError(OriginalStrings::NetworkInitializeOperation, result);
            WriteNetworkLogLevel(3, OriginalStrings::NetworkReinitializationFailed, 0);

            return false;
        }

        ReportDirectPlayError(OriginalStrings::NetworkPeerDestroyedError, 0);
        WriteNetworkLogLevel(3, OriginalStrings::NetworkPeerDestroyedLog, 0);

        return false;
    }

    // Original function: FUN_004dc9cc
    DPNID CDPNet::GetLocalPlayerId()
    {
        if ((flags & 0x02u) == 0 && (flags & 0x08u) == 0)
        {
            return 0;
        }

        SPlayerInfo* players = nullptr;
        DWORD playerCount = 0;
        DPNID playerId = 0;

        // The original enumerates players through the singleton.
        if (GetNetworkInstance()->GetPlayerInfo(&players, &playerCount))
        {
            for (DWORD index = 0; index < playerCount; ++index)
            {
                if (players[index].isLocal != 0)
                {
                    playerId = players[index].playerId;
                    break;
                }
            }

            NXBasics::Runtime_Free(players);
        }

        return playerId;
    }

    // Original function: FUN_004dca43
    DPNID CDPNet::GetHostPlayerId()
    {
        if ((flags & 0x02u) == 0 && (flags & 0x08u) == 0)
        {
            return 0;
        }

        SPlayerInfo* players = nullptr;
        DWORD playerCount = 0;
        DPNID playerId = 0;

        // The original enumerates players through the singleton.
        if (GetNetworkInstance()->GetPlayerInfo(&players, &playerCount))
        {
            for (DWORD index = 0; index < playerCount; ++index)
            {
                if (players[index].isHost != 0)
                {
                    playerId = players[index].playerId;
                    break;
                }
            }

            NXBasics::Runtime_Free(players);
        }

        return playerId;
    }

    // Original function: FUN_004dcf42
    bool CDPNet::GetPlayerName(DPNID playerId, wchar_t** playerName)
    {
        DWORD bufferSize = 0;
        HRESULT result = peer->GetPeerInfo(playerId, nullptr, &bufferSize, 0);

        if (result != DPNERR_BUFFERTOOSMALL)
        {
            return false;
        }

        auto* information = static_cast<DPN_PLAYER_INFO*>(NXBasics::Runtime_AllocateWithNewHandler(bufferSize, 1));

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x573, information != nullptr, "Memory allocation failed.");

        std::memset(information, 0, sizeof(DPN_PLAYER_INFO));
        information->dwSize = sizeof(DPN_PLAYER_INFO);

        result = peer->GetPeerInfo(playerId, information, &bufferSize, 0);

        if (FAILED(result))
        {
            // The original leaves the output untouched and does not free this buffer.
            return false;
        }

        *playerName = NXBasics::Runtime_DuplicateWideString(information->pwszName);

        NXBasics::Runtime_Free(information);
        return true;
    }

    // Original function: FUN_004dcfe9
    bool CDPNet::GetPlayerIds(DPNID** playerIds, DWORD* playerCount)
    {
        *playerCount = 0;

        HRESULT result = peer->EnumPlayersAndGroups(nullptr, playerCount, DPNENUM_PLAYERS);

        if (result != DPNERR_BUFFERTOOSMALL)
        {
            *playerIds = nullptr;
            *playerCount = 0;

            ReportDirectPlayError("CDPNet::GetPlayerIds(): IDirectPlay8Peer::EnumPlayersAndGroups 1", result);
            return false;
        }

        const std::uint32_t allocationBytes = *playerCount * 4u;
        auto* ids = static_cast<DPNID*>(NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1));

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x5B3, ids != nullptr, "Memory allocation failed.");

        std::memset(ids, 0, allocationBytes);

        result = peer->EnumPlayersAndGroups(ids, playerCount, DPNENUM_PLAYERS);

        if (SUCCEEDED(result))
        {
            *playerIds = ids;
            return true;
        }

        ReportDirectPlayError("CDPNet::GetPlayerIds(): IDirectPlay8Peer::EnumPlayersAndGroups 2", result);

        *playerIds = nullptr;
        *playerCount = 0;

        if (ids != nullptr)
            NXBasics::Runtime_Free(ids);

        return false;
    }

    // Original function: FUN_004dd0de
    bool CDPNet::GetPlayerInfo(SPlayerInfo** players, DWORD* playerCount)
    {
        DPNID* playerIds = nullptr;
        DPN_PLAYER_INFO* peerInformation = nullptr;
        bool success = true;

        *playerCount = 0;

        do
        {
            HRESULT result = peer->EnumPlayersAndGroups(nullptr, playerCount, DPNENUM_PLAYERS);

            if (result != DPNERR_BUFFERTOOSMALL)
            {
                ReportDirectPlayError("CDPNet::GetPlayerInfo(): IDirectPlay8Peer::EnumPlayersAndGroups 1", result);
                success = false;
                break;
            }

            const std::uint32_t playerIdBytes = *playerCount * 4u;
            playerIds = static_cast<DPNID*>(NXBasics::Runtime_AllocateWithNewHandler(playerIdBytes, 1));

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x5E7, playerIds != nullptr, "Memory allocation failed.");

            std::memset(playerIds, 0, playerIdBytes);

            result = peer->EnumPlayersAndGroups(playerIds, playerCount, DPNENUM_PLAYERS);

            // The original requires exactly zero here, not merely SUCCEEDED().
            if (result != S_OK)
            {
                ReportDirectPlayError("CDPNet::GetPlayerInfo(): IDirectPlay8Peer::EnumPlayersAndGroups 2", result);
                success = false;
                break;
            }

            const std::uint32_t playerInfoBytes = *playerCount * 0x98u;
            auto* allocatedPlayers = static_cast<SPlayerInfo*>(NXBasics::Runtime_AllocateWithNewHandler(playerInfoBytes, 1));

            if (allocatedPlayers != nullptr)
            {
                // Preserve the original per-record initialization pass.
                for (DWORD index = 0; index < *playerCount; ++index)
                {
                    std::memset(&allocatedPlayers[index], 0, sizeof(SPlayerInfo));
                }
            }

            *players = allocatedPlayers;

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x5F4, *players != nullptr, "Memory allocation failed.");

            std::memset(*players, 0, playerInfoBytes);

            for (DWORD index = 0; index < *playerCount; ++index)
            {
                DWORD peerInformationBytes = 0;

                result = peer->GetPeerInfo(playerIds[index], nullptr, &peerInformationBytes, 0);

                if (result != DPNERR_BUFFERTOOSMALL)
                {
                    ReportDirectPlayError("CDPNet::GetPlayerInfo(): IDirectPlay8Peer::GetPeerInfo 1", result);
                    success = false;
                    break;
                }

                peerInformation = static_cast<DPN_PLAYER_INFO*>(NXBasics::Runtime_AllocateWithNewHandler(peerInformationBytes, 1));

                ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x603, peerInformation != nullptr, "Memory allocation failed.");

                std::memset(peerInformation, 0, peerInformationBytes);
                peerInformation->dwSize = sizeof(DPN_PLAYER_INFO);

                result = peer->GetPeerInfo(playerIds[index], peerInformation, &peerInformationBytes, 0);

                // The original also requires exactly zero for this call.
                if (result != S_OK)
                {
                    ReportDirectPlayError("CDPNet::GetPlayerInfo(): IDirectPlay8Peer::GetPeerInfo 2", result);
                    success = false;
                    break;
                }

                SPlayerInfo& player = (*players)[index];
                const wchar_t* source = peerInformation->pwszName;
                wchar_t* destination = player.playerName;

                // Original call: FUN_004e8a3c, an unbounded wide-string copy.
                do
                {
                    *destination++ = *source;
                } while (*source++ != L'\0');

                player.playerId = playerIds[index];
                player.isHost = (peerInformation->dwPlayerFlags & DPNPLAYER_HOST) != 0;
                player.isLocal = (peerInformation->dwPlayerFlags & DPNPLAYER_LOCAL) != 0;

                NXBasics::Runtime_Free(peerInformation);
                peerInformation = nullptr;

                if (player.isLocal != 0)
                {
                    player.bytesReceived = 0;
                    player.bytesSent = 0;
                    player.roundTripLatencyMS = 0;
                    player.throughputBPS = 0;
                }
                else
                {
                    DPN_CONNECTION_INFO connectionInformation{};
                    connectionInformation.dwSize = sizeof(DPN_CONNECTION_INFO);

                    result = peer->GetConnectionInfo(playerIds[index], &connectionInformation, 0);

                    if (FAILED(result))
                    {
                        ReportDirectPlayError("CDPNet::GetPlayerInfo(): IDirectPlay8Peer::GetConnectionInfo", result);
                        success = false;
                        break;
                    }

                    player.throughputBPS = connectionInformation.dwThroughputBPS;
                    player.roundTripLatencyMS = connectionInformation.dwRoundTripLatencyMS;
                    player.bytesSent = connectionInformation.dwBytesSentGuaranteed + connectionInformation.dwBytesSentNonGuaranteed;
                    player.bytesReceived = connectionInformation.dwBytesReceivedGuaranteed + connectionInformation.dwBytesReceivedNonGuaranteed;
                }
            }
        } while (false);

        NXBasics::Runtime_Free(playerIds);

        if (!success)
        {
            NXBasics::Runtime_Free(peerInformation);

            if (*players != nullptr)
            {
                NXBasics::Runtime_Free(*players);
                *players = nullptr;
            }

            *playerCount = 0;
        }

        // The original clears this flag on both success and failure.
        playerInfoChanged = 0;

        return success;
    }

    // Original function: FUN_004dd5bd; connection policy is broadcast only in host mode.
    bool CDPNet::SetAllowConnections(bool allow)
    {
        if (allowConnections == static_cast<std::uint8_t>(allow)) return true;
        if ((flags & 2u) != 2u) return false;
        allowConnections = static_cast<std::uint8_t>(allow);
        const std::uint8_t packet[2] = {2, static_cast<std::uint8_t>(allow)};
        return SendPacket(0, packet);
    }

    // Original function: FUN_004dd5f2
    bool CDPNet::KickPlayer(DPNID playerId)
    {
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x6C1, peer != nullptr, "DirectPlay peer object is invalid (0).");

        const HRESULT result = peer->DestroyPeer(playerId, nullptr, 0, 0);

        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::KickPlayer(): IDirectPlay8Peer::DestroyPeer", result);
            return false;
        }

        return true;
    }

    // Original function: FUN_004dd65e
    bool CDPNet::SendPacket(DPNID recipient, const void* packet)
    {
        const auto* bytes = static_cast<const std::uint8_t*>(packet);

        DPN_BUFFER_DESC buffer = {};
        buffer.pBufferData = const_cast<std::uint8_t*>(bytes);

        DWORD sendFlags = 0;
        std::uint8_t convertedPacket[0xD4];

        switch (bytes[0])
        {
            case SwitchIds::NetworkPacket::AllowConnections:
                buffer.dwBufferSize = 2;
                sendFlags = 0x22u;
                break;

            case SwitchIds::NetworkPacket::Chat:
                {
                    std::uint32_t length = 0;

                    while (bytes[2u + length * 2u] != 0 || bytes[3u + length * 2u] != 0)
                        ++length;

                    buffer.dwBufferSize = length * 2u + 4u;
                    sendFlags = 0x42u;
                    break;
                }

            case SwitchIds::NetworkPacket::DynamicData:
                {
                    std::uint32_t payloadSize;
                    std::memcpy(&payloadSize, bytes + 4, sizeof(payloadSize));

                    buffer.dwBufferSize = payloadSize + 0x0Cu;
                    sendFlags = 0x02u;
                    break;
                }

            case SwitchIds::NetworkPacket::Commands:
                buffer.dwBufferSize = static_cast<std::uint32_t>(bytes[8]) + 0x0Cu;
                sendFlags = 0x28u;
                break;

            case SwitchIds::NetworkPacket::PlayerInfo:
                buffer.dwBufferSize = 0x0Cu;
                sendFlags = 0x2Cu;
                break;

            case SwitchIds::NetworkPacket::ConnectionProbe:
                buffer.dwBufferSize = 2;
                sendFlags = 0x24u;
                break;

            case SwitchIds::NetworkPacket::PrepareCommandBuffers:
                buffer.dwBufferSize = 2;
                sendFlags = 0x88u;
                break;

            case SwitchIds::NetworkPacket::RequestCommands:
                buffer.dwBufferSize = 0x10u;
                sendFlags = 0x88u;
                break;

            case SwitchIds::NetworkPacket::ConvertCommands:
                CommandPacket_Construct(convertedPacket);
                std::memcpy(convertedPacket + 4, bytes + 4, 4);
                convertedPacket[8] = bytes[8];
                std::memcpy(convertedPacket + 9, bytes + 9, bytes[8]);

                buffer.dwBufferSize = sizeof(convertedPacket);
                buffer.pBufferData = convertedPacket;
                sendFlags = 0x88u;
                break;

            case SwitchIds::NetworkPacket::FileTransferAnnouncement:
                buffer.dwBufferSize = 0x118u;
                sendFlags = 0x20u;
                break;

            case SwitchIds::NetworkPacket::FileTransferAccept:
                buffer.dwBufferSize = 0x18u;
                break;

            case SwitchIds::NetworkPacket::FileTransferRequestHeader:
                buffer.dwBufferSize = 0x14u;
                break;

            case SwitchIds::NetworkPacket::FileTransferHeader:
                buffer.dwBufferSize = 0x1Cu;
                break;

            case SwitchIds::NetworkPacket::FileTransferData:
                buffer.dwBufferSize = 0x818u;
                sendFlags = DPNSEND_SYNC;
                break;

            default:
                ReportDirectPlayError("CDPNet::SendMessage(): Unknown message type ignored.", 0);
                return false;
        }

        // The original condition checks +0x3C despite referring to the peer.
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x7A8, receivedMessages != nullptr, "DirectPlay peer object is invalid (0).");

        const HRESULT result = peer->SendTo(recipient, &buffer, 1, 3000, nullptr, nullptr, sendFlags);

        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::SendMessage(): SendTo", result);
            return false;
        }

        return true;
    }

    // Original function: FUN_004dd948
    void CDPNet::GetReceivedMessages(SReceivedMessage** messages, std::uint32_t* count)
    {
        static_assert(sizeof(SReceivedMessage) == 0x10);

        // Preserve the original check outside the critical section.
        if (receivedMessageCount == 0)
        {
            *messages = nullptr;
            *count = 0;
            return;
        }

        EnterCriticalSection(&criticalEC);

        *messages = receivedMessages;
        *count = receivedMessageCount;

        receivedMessageCount = 0;

        auto* replacement = static_cast<SReceivedMessage*>(NXBasics::Runtime_AllocateWithNewHandler(0x400u, 1));

        if (replacement != nullptr)
            std::memset(replacement, 0, 0x400u);

        receivedMessages = replacement;

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x7F8, receivedMessages != nullptr, "Memory allocation failed.");

        std::memset(receivedMessages, 0, 0x400u);

        LeaveCriticalSection(&criticalEC);
    }

    // Original function: FUN_004ddadb
    void CDPNet::AddPlayerEvent(DPNID playerId, std::uint32_t eventType)
    {
        wchar_t* playerName = nullptr;

        // The original obtains the name through the singleton.
        GetNetworkInstance()->GetPlayerName(playerId, &playerName);

        EnterCriticalSection(&criticalA4);

        const std::uint32_t timestamp = timeGetTime() - initializationTick;
        AddEventQueueEntry(&events18, playerId, timestamp, playerName, eventType);

        LeaveCriticalSection(&criticalA4);

        NXBasics::Runtime_Free(playerName);
    }

    // Original function: FUN_004ddb3e
    void CDPNet::GetEvents(SEventQueue* destination)
    {
        if (destination == nullptr)
            return;

        EnterCriticalSection(&criticalA4);

        ReplaceEventQueueAndClearSource(destination, &events18);

        LeaveCriticalSection(&criticalA4);

        // Preserve the additional clear after releasing the first lock.
        ClearEvents();
    }

    // Original function: FUN_004ddb76
    void CDPNet::ClearEvents()
    {
        EnterCriticalSection(&criticalA4);

        if (events18.count != 0)
            ClearEventQueue(&events18);

        LeaveCriticalSection(&criticalA4);
    }

    // Original function: FUN_004ddb9f
    bool CDPNet::HasPlayerConnectionFailed(DPNID playerId)
    {
        // The original checks member +0x3C, not the peer pointer at +0x68.
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x8EE, receivedMessages != nullptr, "DirectPlay peer object is invalid.");

        std::uint8_t packet[2] = { 7, 0 };

        DPN_BUFFER_DESC buffer = {};
        buffer.dwBufferSize = sizeof(packet);
        buffer.pBufferData = packet;

        // Preserve the original send flags and 500 ms timeout.
        const HRESULT result = peer->SendTo(playerId, &buffer, 1, 500, nullptr, nullptr, 0x80000088u);

        if (FAILED(result))
            ReportDirectPlayError("Guaranteed send operation failed", result);

        return FAILED(result);
    }

    // Original function: FUN_004ddd3c
    void UpdateSessionList(SSessionList* list, const DPNMSG_ENUM_HOSTS_RESPONSE* response)
    {
        SSessionNode* current = list->first;
        bool found = false;

        if (current != nullptr)
        {
            do
            {
                if (current->invalid == 0 && std::memcmp(&current->description->guidInstance, &response->pApplicationDescription->guidInstance, sizeof(GUID)) == 0)
                {
                    if (current->responseTime != response->dwRoundTripLatencyMS)
                    {
                        current->responseTime = response->dwRoundTripLatencyMS;
                        list->flags |= 0x02u;
                    }

                    current->lastResponseTime = timeGetTime();
                    found = true;
                }

                current = current->next;
            } while (current != list->first && list->first != nullptr);
        }

        if (found)
        {
            return;
        }

        auto* node = static_cast<SSessionNode*>(NXBasics::Runtime_AllocateWithNewHandler(sizeof(SSessionNode), 1));

        if (node != nullptr)
        {
            ConstructSessionNode(node);
        }

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0x9D, node != nullptr, "Memory allocation failed.");

        node->invalid = 0;
        node->lastResponseTime = timeGetTime();
        node->responseTime = response->dwRoundTripLatencyMS;

        HRESULT result = response->pAddressSender->Duplicate(&node->address0C);

        if (FAILED(result))
        {
            OutputDebugStringA("CDPSessionList::Update(): Duplicate (1)\n");

            ReleaseSessionNodeResources(node);
            NXBasics::Runtime_Free(node);
            return;
        }

        result = response->pAddressDevice->Duplicate(&node->address10);

        if (FAILED(result))
        {
            OutputDebugStringA("CDPSessionList::Update(): Duplicate (2)\n");

            ReleaseSessionNodeResources(node);
            NXBasics::Runtime_Free(node);
            return;
        }

        const DPN_APPLICATION_DESC* description = response->pApplicationDescription;

        node->description = static_cast<DPN_APPLICATION_DESC*>(NXBasics::Runtime_AllocateWithNewHandler(sizeof(DPN_APPLICATION_DESC), 1));

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0xBE, node->description != nullptr, "Memory allocation failed.");

        std::memset(node->description, 0, sizeof(DPN_APPLICATION_DESC));
        std::memcpy(node->description, description, sizeof(DPN_APPLICATION_DESC));

        // Only the session name receives an independent allocation.
        node->description->pwszSessionName = NXBasics::Runtime_DuplicateWideString(description->pwszSessionName);

        InsertSessionNode(list, node);
    }

    // Original function: FUN_004deac6
    DPNID GetDynamicNetworkHostPlayerId(const SDynamicNetworkData* data)
    {
        const auto* players = static_cast<const std::byte*>(data->allocation10);

        if (players == nullptr)
            return 0;

        for (std::uint32_t index = 0; index < data->playerCount; ++index)
        {
            const std::byte* player = players + index * 0x98u;

            if (player[0x84] != std::byte{ 0 })
            {
                DPNID playerId;
                std::memcpy(&playerId, player, sizeof(playerId));
                return playerId;
            }
        }

        return 0;
    }

    // Original function: FUN_004deafc
    DPNID GetDynamicNetworkLocalPlayerId(const SDynamicNetworkData* data)
    {
        const auto* players = static_cast<const std::byte*>(data->allocation10);

        if (players == nullptr)
            return 0;

        for (std::uint32_t index = 0; index < data->playerCount; ++index)
        {
            const std::byte* player = players + index * 0x98u;

            if (player[0x85] != std::byte{ 0 })
            {
                DPNID playerId = 0;
                std::memcpy(&playerId, player, sizeof(playerId));
                return playerId;
            }
        }

        return 0;
    }

    // Original function: FUN_004dec0a
    bool SendDynamicNetworkChatMessage(SDynamicNetworkData* /*data*/, DPNID recipient, const wchar_t* text)
    {
        CDPNet* network = GetNetworkInstance();

        struct SChatPacket
        {
            std::uint8_t type;
            std::uint8_t characterCount;
            wchar_t text[128];
            std::byte trailingPadding[2];
        };

        static_assert(sizeof(wchar_t) == 2);
        static_assert(offsetof(SChatPacket, text) == 0x02);
        static_assert(sizeof(SChatPacket) == 0x104);

        SChatPacket packet;

        ChatPacket_Construct(&packet);

        // Preserve the original length byte before truncating the copied text.
        packet.characterCount = static_cast<std::uint8_t>(std::wcslen(text) + 1u);
        std::wcsncpy(packet.text, text, 127);

        return network->SendPacket(recipient, &packet);
    }

    // Original function: FUN_004dec60
    bool TakeDynamicNetworkTextRecords(SDynamicNetworkData* data, void** records, std::uint32_t* recordCount)
    {
        if (data->receivedRecordCount == 0)
        {
            return false;
        }

        *records = data->allocation28;
        *recordCount = data->receivedRecordCount;

        data->allocation28 = nullptr;
        data->receivedRecordCount = 0;

        return true;
    }

    // Original function: FUN_004dec8a
    bool SendDynamicNetworkMessage(SDynamicNetworkData* /*data*/, DPNID recipient, const void* payload, std::uint32_t byteCount)
    {
        CDPNet* network = GetNetworkInstance();

        if (byteCount > 0x400u)
            return false;

        struct SPayloadPacket
        {
            std::uint8_t type;             // +0x00
            std::byte padding01[3];        // +0x01
            std::uint32_t payloadSize;     // +0x04
            std::uint32_t checksum;        // +0x08
            std::uint8_t payload[0x400];   // +0x0C
        };

        static_assert(sizeof(SPayloadPacket) == 0x40C);
        static_assert(offsetof(SPayloadPacket, payloadSize) == 0x04);
        static_assert(offsetof(SPayloadPacket, checksum) == 0x08);
        static_assert(offsetof(SPayloadPacket, payload) == 0x0C);

        SPayloadPacket packet;

        PayloadPacket_Construct(&packet);

        packet.checksum = NCRC32::Calculate(payload, static_cast<std::int32_t>(byteCount));
        packet.payloadSize = byteCount;

        std::memcpy(packet.payload, payload, byteCount);

        return network->SendPacket(recipient, &packet);
    }

    // Original function: FUN_004ded59; CDPLogic command-packet send.
    bool SendDynamicNetworkCommands(SDynamicNetworkData* data, const void* payload, std::uint32_t byteCount)
    {
        CDPNet* network = GetNetworkInstance();
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x16C, byteCount != 0, "Invalid message size (<= 0).");
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x16D, byteCount <= 200u, "Maximum message size exceeded.");
        if (byteCount > 200u) return false;
        std::byte packet[0xD4];
        CommandPacket_Construct(packet);
        std::memcpy(packet + 4, &data->networkTick, 4);
        packet[8] = static_cast<std::byte>(byteCount);
        std::memcpy(packet + 9, payload, byteCount);
        const auto player = GetDynamicNetworkLocalPlayerId(data);
        SetCommandMessageLength(&data->arrays[data->commandBufferIndices[0]], player, static_cast<std::uint8_t>(byteCount));
        StoreCommandMessageData(&data->arrays[data->commandBufferIndices[0]], player, payload, byteCount);
        return network->SendPacket(0, packet);
    }

    // Original function: FUN_004df16f; copies the complete current command buffer.
    bool TakeDynamicNetworkCommands(SDynamicNetworkData* data, SDynamicSlot** slots, std::uint32_t* count)
    {
        auto& buffer = data->arrays[data->commandBufferIndices[0]];
        if (buffer.slots == nullptr || !IsCommandBufferComplete(&buffer))
        {
            *slots = nullptr;
            *count = 0;
            return false;
        }
        *count = buffer.count;
        *slots = static_cast<SDynamicSlot*>(NXBasics::Runtime_Allocate(*count * 0xD0u));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x248, *slots != nullptr, "Memory allocation failed.");
        std::memcpy(*slots, data->arrays[data->commandBufferIndices[0]].slots, *count * 0xD0u);
        return true;
    }

    // Original function: FUN_004decf5
    void GetDynamicNetworkMessages(SDynamicNetworkData* data, SDynamicDataMessage** messages, std::uint32_t* count)
    {
        *messages = nullptr;
        *count = 0;

        if (data->pendingDataMessageCount == 0)
            return;

        *messages = static_cast<SDynamicDataMessage*>(data->allocation30);
        *count = data->pendingDataMessageCount;

        data->allocation30 = NXBasics::Runtime_AllocateWithNewHandler(0x10300u, 1);

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x153, data->allocation30 != nullptr, "Memory allocation failed.");

        std::memset(data->allocation30, 0, 0x10300u);

        data->pendingDataMessageCount = 0;
    }

    // Original function: FUN_004dee72
    void PrepareDynamicCommandBuffers(SDynamicNetworkData* data)
    {
        std::uint32_t bufferIndex = 0;
        bufferIndex = data->commandBufferIndices[0];

        if (data->arrays[bufferIndex].slots != nullptr)
            return;

        DPNID* playerIds = nullptr;
        DWORD playerCount = 0;

        if (!GetNetworkInstance()->GetPlayerIds(&playerIds, &playerCount))
        {
            ReportDirectPlayError("CDPLogic::PrepareCmdBuffers(): CDPNet::GetPlayerIds()", 0);
            return;
        }

        const std::uint32_t initialIndices[3] = { 0, 1, 2 };
        std::memcpy(data->commandBufferIndices, initialIndices, sizeof(initialIndices));

        UpdateCommandBufferPlayers(&data->arrays[0], playerIds, playerCount);

        for (std::uint32_t selector = 1; selector < 3; ++selector)
        {
            bufferIndex = data->commandBufferIndices[selector];
            UpdateCommandBufferPlayers(&data->arrays[bufferIndex], playerIds, playerCount);
        }

        if (playerIds != nullptr)
            NXBasics::Runtime_Free(playerIds);

        data->playerCount = 0;

        if (data->allocation10 != nullptr)
        {
            NXBasics::Runtime_Free(data->allocation10);
            data->allocation10 = nullptr;
        }

        SPlayerInfo* playerInfo = nullptr;
        const bool success = GetNetworkInstance()->GetPlayerInfo(&playerInfo, &data->playerCount);

        data->allocation10 = playerInfo;

        if (!success)
            ReportDirectPlayError("CDPLogic::PrepareCmdBuffers(): CDPNet::GetPlayerInfo()\n", 0);
    }

    // Original function: FUN_004e105e.
    static void CopyDynamicCommandBuffer(SDynamicSlotArray* destination, const SDynamicSlotArray* source)
    {
        constexpr const char* sourceFile = "D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\cmdbuffer.cpp";
        ReportNetworkConditionFailure(sourceFile, 0x73, source != nullptr, "Command buffer destroyed.");
        ReportNetworkConditionFailure(sourceFile, 0x74, source->count == destination->count, "Command buffer destroyed (invalid size).");
        destination->count = source->count;
        destination->networkTick = source->networkTick;
        for (std::uint32_t index = 0; index < destination->count; ++index)
        {
            destination->slots[index].playerId = source->slots[index].playerId;
            destination->slots[index].used = source->slots[index].used;
            // Windows passes player count, not message length, to memmove (OB089).
            std::memmove(destination->slots[index].data, source->slots[index].data, destination->count);
        }
    }

    // Original function: FUN_004df092.
    void AdvanceDynamicNetworkTick(SDynamicNetworkData* data)
    {
        constexpr const char* sourceFile = "D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp";
        ReportNetworkConditionFailure(sourceFile, 0x211, data->arrays[data->commandBufferIndices[0]].slots != nullptr, "Message buffer is invalid.");
        ReportNetworkConditionFailure(sourceFile, 0x212, data->arrays[data->commandBufferIndices[1]].slots != nullptr, "Message buffer is invalid.");
        ReportNetworkConditionFailure(sourceFile, 0x213, data->arrays[data->commandBufferIndices[2]].slots != nullptr, "Message buffer is invalid.");
        if (++data->networkTick > 1)
        {
            CopyDynamicCommandBuffer(&data->arrays[data->commandBufferIndices[2]], &data->arrays[data->commandBufferIndices[0]]);
            const std::uint32_t current = data->commandBufferIndices[0];
            const std::uint32_t next = data->commandBufferIndices[1];
            data->commandBufferIndices[1] = current;
            data->commandBufferIndices[0] = next;
            ResetDynamicSlots(&data->arrays[current]);
        }
        data->arrays[data->commandBufferIndices[0]].networkTick = data->networkTick;
        data->arrays[data->commandBufferIndices[1]].networkTick = data->networkTick + 1u;
        data->arrays[data->commandBufferIndices[2]].networkTick = data->networkTick - 1u;
        data->waitStartTick = timeGetTime();
    }

    // Original function: FUN_004def63
    void UpdateDynamicCommandBuffers(SDynamicNetworkData* data)
    {
        std::uint32_t bufferIndex = 0;
        bufferIndex = data->commandBufferIndices[0];

        if (data->arrays[bufferIndex].slots == nullptr)
            return;

        WriteNetworkLog(GetNetworkLog(), "Updating command message buffers...", 0);

        DPNID* playerIds = nullptr;
        DWORD playerCount = 0;

        if (!GetNetworkInstance()->GetPlayerIds(&playerIds, &playerCount))
        {
            ReportDirectPlayError("CDPLogic::UpdateCmdBuffers(): CDPNet::GetPlayerIds()", 0);
            return;
        }

        char text[100] = {};

        NXBasics::Runtime_PrintString(text, "%u player(s) left in session", static_cast<unsigned int>(playerCount));
        WriteNetworkLog(GetNetworkLog(), text, 0);

        for (std::uint32_t index = 0; index < 3; ++index)
        {
            bufferIndex = data->commandBufferIndices[index];
            UpdateCommandBufferPlayers(&data->arrays[bufferIndex], playerIds, playerCount);
        }

        if (playerIds != nullptr)
            NXBasics::Runtime_Free(playerIds);

        data->playerCount = 0;

        if (data->allocation10 != nullptr)
        {
            NXBasics::Runtime_Free(data->allocation10);
            data->allocation10 = nullptr;
        }

        SPlayerInfo* playerInfo = nullptr;
        const bool success = GetNetworkInstance()->GetPlayerInfo(&playerInfo, &data->playerCount);

        data->allocation10 = playerInfo;

        if (!success)
            ReportDirectPlayError("CDPLogic::PrepareCmdBuffers(): CDPNet::GetPlayerInfo()\n", 0);
    }

    // Original function: FUN_004df207
    bool BroadcastControlMessage8(SDynamicNetworkData* /*data*/)
    {
        CDPNet* network = GetNetworkInstance();

        const std::uint8_t packet[2] = { 8, 0 };

        return network->SendPacket(0, packet);
    }

    // Original function: FUN_004df227
    void ClearDynamicNetworkEvents(SDynamicNetworkData* data)
    {
        GetNetworkInstance()->ClearEvents();
        ClearEventQueue(&data->events);
    }

    // Original function: FUN_004df240
    // Extracted prefix: 004df240 through 004df383.
    bool PrepareDynamicNetworkUpdate(SDynamicNetworkData* data, bool reset)
    {
        CDPNet* network = GetNetworkInstance();

        if (reset)
        {
            gDynamicNetworkUpdateStopped = 0;
            gLastRecoveryPlayerId = 0;
            gPendingMissingPlayerId = 0;
            gProcessedPlayerChangeCount = 0;
            gDynamicNetworkInitialUpdatePending = 1;
            return false;
        }

        if (gDynamicNetworkInitialUpdatePending != 0)
        {
            WriteNetworkLog(GetNetworkLog(), "Update(): Initial call", 0);
            ClearEventQueue(&data->events);
            gDynamicNetworkInitialUpdatePending = 0;
        }

        if (gDynamicNetworkUpdateStopped != 0)
            return false;

        const std::uint32_t playerChangeCount = network->playerChangeCount;

        if (playerChangeCount > gProcessedPlayerChangeCount)
        {
            SEventQueue events;

            // Original function: FUN_004db429.
            // The remaining bytes are not initialized by the original constructor.
            events.count = 0;
            events.last = nullptr;
            events.first = nullptr;
            events.changed = 0;

            network->GetEvents(&events);
            AppendEventQueueCopy(&data->events, &events);

            if (events.first == nullptr)
            {
                WriteNetworkLog(GetNetworkLog(), "Update(): Session has changed but event list is not yet updated.", 0);
                ClearEventQueue(&events);
                return false;
            }

            for (SEventNode* event = events.first; event != nullptr; event = event->next)
            {
                const auto* eventBytes = reinterpret_cast<const std::byte*>(event);

                DPNID playerId;
                std::int32_t eventType;

                std::memcpy(&playerId, eventBytes, sizeof(playerId));
                std::memcpy(&eventType, eventBytes + 0x208, sizeof(eventType));

                if (eventType < 3 || eventType > 7)
                    continue;

                if (gPendingMissingPlayerId == playerId)
                    gPendingMissingPlayerId = 0;

                const auto* players = static_cast<const std::byte*>(data->allocation10);

                for (std::uint32_t index = 0; index < data->playerCount; ++index)
                {
                    DPNID existingPlayerId;
                    std::memcpy(&existingPlayerId, players + index * 0x98u, sizeof(existingPlayerId));

                    if (existingPlayerId == playerId)
                    {
                        UpdateDynamicCommandBuffers(data);
                        break;
                    }
                }

                HandleSpecialPlayerDepartureEvent(event);
            }

            ClearEventQueue(&events);
        }

        gProcessedPlayerChangeCount = playerChangeCount;
        return true;
    }

    // Original function: FUN_004df240
    // Extracted type-4 receive branch: 004df77a through 004df80a.
    void StoreDynamicDataMessage(SDynamicNetworkData* data, const SReceivedMessage& received)
    {
        if (data->pendingDataMessageCount >= 64u)
        {
            WriteNetworkLog(GetNetworkLog(), "Update(): Data message buffer overflow - message ignored", 0);
            ReportDirectPlayError("CDPLogic::Update(): Data message buffer overflow. Message ignored.\n", 0);
            return;
        }

        const auto* packet = static_cast<const std::uint8_t*>(received.data);

        std::uint32_t payloadSize;
        std::uint32_t expectedChecksum;

        std::memcpy(&payloadSize, packet + 4, sizeof(payloadSize));
        std::memcpy(&expectedChecksum, packet + 8, sizeof(expectedChecksum));

        auto* messages = static_cast<SDynamicDataMessage*>(data->allocation30);
        SDynamicDataMessage& message = messages[data->pendingDataMessageCount];

        message.sender = received.sender;
        message.payloadSize = payloadSize;

        // The original copies the payload before checking its checksum.
        std::memcpy(message.payload, packet + 0x0C, payloadSize);

        const std::uint32_t checksum = NCRC32::Calculate(packet + 0x0C, NXBasics::Runtime_TimeSigned32(payloadSize));

        message.checksumMismatch = checksum != expectedChecksum ? 1u : 0u;

        ++data->pendingDataMessageCount;
    }

    // Original function: FUN_004df240
    void UpdateDynamicNetwork(SDynamicNetworkData* data, bool reset)
    {
        CDPNet* network = GetNetworkInstance();

        if (!PrepareDynamicNetworkUpdate(data, reset))
            return;

        const auto readDword = [] (const void* source) -> std::uint32_t
        {
            std::uint32_t value = 0;
            std::memcpy(&value, source, sizeof(value));
            return value;
        };

        const auto getBuffer = [&] (std::uint32_t selector) -> SDynamicSlotArray*
        {
            const std::uint32_t index = data->commandBufferIndices[selector];
            return &data->arrays[index];
        };

        const auto log = [] (const char* text)
        {
            WriteNetworkLog(GetNetworkLog(), text, 0);
        };

        SReceivedMessage* messages = nullptr;
        std::uint32_t messageCount = 0;

        GetNetworkInstance()->GetReceivedMessages(&messages, &messageCount);

        if (messageCount != 0)
        {
            std::uint32_t incomingChatCount = 0;

            // Process game-start packets before dispatching command packets.
            for (std::uint32_t index = 0; index < messageCount; ++index)
            {
                const auto* packet = static_cast<const std::uint8_t*>(messages[index].data);

                if (packet[0] == 3)
                    ++incomingChatCount;

                if (packet[0] == 8)
                {
                    if (getBuffer(0)->slots == nullptr)
                    {
                        log("Update(): Game start message received");
                        PrepareDynamicCommandBuffers(data);
                    }
                    else
                    {
                        log("Update(): Game start message received but buffer is already created (warning)");
                    }
                }
            }

            if (data->allocation28 != nullptr && data->receivedRecordCount != 0)
            {
                log("Update(): Chat buffer contains messages that were not yet received");

                const std::uint32_t allocationBytes = (data->receivedRecordCount + incomingChatCount) * 0x104u;
                void* replacement = NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1);

                ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x340, replacement != nullptr, "Memory allocation failed.");

                std::memset(replacement, 0, allocationBytes);
                std::memcpy(replacement, data->allocation28, data->receivedRecordCount * 0x104u);

                NXBasics::Runtime_Free(data->allocation28);
                data->allocation28 = replacement;
            }
            else
            {
                if (data->allocation28 != nullptr)
                {
                    NXBasics::Runtime_Free(data->allocation28);
                    data->allocation28 = nullptr;
                }

                const std::uint32_t allocationBytes = incomingChatCount * 0x104u;

                data->receivedRecordCount = 0;
                data->allocation28 = NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1);

                ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\dplogic.cpp", 0x351, data->allocation28 != nullptr, "Memory allocation failed.");

                std::memset(data->allocation28, 0, allocationBytes);
            }

            for (std::uint32_t index = 0; index < messageCount; ++index)
            {
                const SReceivedMessage& message = messages[index];
                const auto* packet = static_cast<const std::uint8_t*>(message.data);

                switch (packet[0])
                {
                    case SwitchIds::NetworkPacket::Chat:
                        {
                            log("Update(): Incoming chat message");

                            auto* destination = static_cast<std::byte*>(data->allocation28) + data->receivedRecordCount * 0x104u;

                            std::memcpy(destination, &message.sender, sizeof(message.sender));
                            std::wcscpy(reinterpret_cast<wchar_t*>(destination + 4), reinterpret_cast<const wchar_t*>(packet + 2));

                            ++data->receivedRecordCount;
                            break;
                        }

                    case SwitchIds::NetworkPacket::DynamicData:
                        StoreDynamicDataMessage(data, message);
                        break;

                    case SwitchIds::NetworkPacket::Commands:
                        {
                            const std::uint32_t tick = readDword(packet + 4);
                            SDynamicSlotArray* buffer = nullptr;

                            // Command packets search the current, next, then previous buffer.
                            for (std::uint32_t selector = 0; selector < 3; ++selector)
                            {
                                SDynamicSlotArray* candidate = getBuffer(selector);

                                if (candidate->networkTick == tick)
                                {
                                    buffer = candidate;
                                    break;
                                }
                            }

                            if (buffer == nullptr)
                            {
                                log("Update(): Cannot assign incoming command message to the correct buffer because of an invalid network tick number. Message ignored.");
                                break;
                            }

                            const std::uint32_t length = packet[8];

                            if (!SetCommandMessageLength(buffer, message.sender, static_cast<std::uint8_t>(length)))
                            {
                                log("Update(): Cannot set message length");
                                break;
                            }

                            if (!StoreCommandMessageData(buffer, message.sender, packet + 9, length))
                                log("Update(): Cannot save message data");

                            break;
                        }

                    case SwitchIds::NetworkPacket::RequestCommands:
                        {
                            log("Update(): Message request detected");

                            const std::uint32_t tick = readDword(packet + 4);
                            const DPNID requestedPlayer = readDword(packet + 8);
                            const DPNID recipient = readDword(packet + 12);

                            SDynamicSlotArray* buffer = nullptr;

                            // Requests use a different search order: current, previous, next.
                            const std::uint32_t selectors[3] = { 0, 2, 1 };

                            for (std::uint32_t selector : selectors)
                            {
                                SDynamicSlotArray* candidate = getBuffer(selector);

                                if (candidate->networkTick == tick)
                                {
                                    buffer = candidate;
                                    break;
                                }
                            }

                            if (buffer == nullptr)
                            {
                                log("Update(): Could not find requested message");
                                break;
                            }

                            struct SCommandReply
                            {
                                std::uint8_t type;
                                std::byte padding01[3];
                                std::uint32_t tick;
                                std::uint8_t length;
                                std::uint8_t payload[200];
                                std::byte paddingD1[3];
                            };

                            static_assert(offsetof(SCommandReply, payload) == 9);
                            static_assert(sizeof(SCommandReply) == 0xD4);

                            SCommandReply reply;

                            CommandPacket_Construct(&reply);
                            reply.type = 10;

                            if (buffer->slots == nullptr || GetCommandMessageLength(buffer, requestedPlayer) == 0)
                            {
                                log("Update(): Unable to re-send requested message - don't have it... ");
                                ReportDirectPlayError("CDLogic::Update(): Unable to re-send requested message\n", 0);
                                break;
                            }

                            reply.tick = buffer->networkTick;
                            reply.length = GetCommandMessageLength(buffer, requestedPlayer);

                            CopyCommandMessageData(buffer, requestedPlayer, reply.payload, reply.length);

                            if (!GetNetworkInstance()->SendPacket(recipient, &reply))
                            {
                                log("Update(): Unable to re-send requested message");
                                ReportDirectPlayError("CDLogic::Update(): Unable to re-send requested message\n", 0);
                                break;
                            }

                            char text[256] = {};
                            const std::uint32_t replyTick = buffer->networkTick;
                            const auto checksum = NCRC16::Calculate(reply.payload, reply.length);
                            const auto length = GetCommandMessageLength(buffer, requestedPlayer);

                            NXBasics::Runtime_PrintString(text, "Update(): Network tick #%u, peer: 0x%x length %i byte(s), crc-16 0x%x (info)", replyTick, requestedPlayer, static_cast<int>(length), static_cast<unsigned int>(checksum));

                            log("Update(): Re-sent requested message");
                            log(text);
                            break;
                        }

                    default:
                        break;
                }
            }
        }

        if (gPendingMissingPlayerId != 0)
        {
            if (messageCount != 0)
                log("Update(): Message buffer lost (warning)");

            // Original branch bypasses ReleaseReceivedMessages.
            return;
        }

        const auto copyPlayerName = [&] (wchar_t* destination, DPNID playerId, bool localPlayer)
        {
            const auto* players = static_cast<const std::byte*>(data->allocation10);

            if (players == nullptr)
                return;

            for (std::uint32_t index = 0; index < data->playerCount; ++index)
            {
                const std::byte* player = players + index * 0x98u;
                const bool matches = localPlayer ? player[0x85] != std::byte{ 0 } : readDword(player) == playerId;

                if (matches)
                    std::wcscpy(destination, reinterpret_cast<const wchar_t*>(player + 4));
            }
        };

        // False preserves an original early return without receive-buffer release.
        const auto updateTimeout = [&] () -> bool
        {
            SDynamicSlotArray* currentBuffer = getBuffer(0);

            if (currentBuffer->slots == nullptr)
                return true;

            if (IsCommandBufferComplete(currentBuffer))
            {
                gLastRecoveryPlayerId = 0;
                return true;
            }

            if (data->waitStartTick == 0)
                return true;

            const std::uint32_t elapsed = ::timeGetTime() - data->waitStartTick;
            const std::uint32_t timeout = data->networkTick > 1u ? 9000u : 90000u;

            if (elapsed >= timeout)
            {
                log("Update(): Timeout!");

                const DPNID localPlayerId = GetNetworkInstance()->GetLocalPlayerId();

                if (localPlayerId == 0)
                {
                    wchar_t name[64] = L"Local peer";
                    copyPlayerName(name, 0, true);

                    log("Update(): Network is down");

                    const DWORD initializationTick = network->initializationTick;
                    AddEventQueueEntry(&data->events, 0, ::timeGetTime() - initializationTick, name, 5);

                    gDynamicNetworkUpdateStopped = 1;
                    return true;
                }

                wchar_t missingName[64] = L"n/a";
                const DPNID missingPlayerId = GetFirstMissingCommandPlayerId(getBuffer(0));

                if (missingPlayerId == 0)
                {
                    log("Update(): dpnidMissing not found");

                    const DWORD initializationTick = network->initializationTick;
                    AddEventQueueEntry(&data->events, 0, ::timeGetTime() - initializationTick, L"Local peer", 5);

                    gDynamicNetworkUpdateStopped = 1;
                    return true;
                }

                copyPlayerName(missingName, missingPlayerId, false);

                const DPNID hostPlayerId = GetNetworkInstance()->GetHostPlayerId();

                if (hostPlayerId == 0)
                {
                    wchar_t name[64] = L"Local peer";
                    copyPlayerName(name, 0, true);

                    const DWORD initializationTick = network->initializationTick;
                    AddEventQueueEntry(&data->events, 0, ::timeGetTime() - initializationTick, name, 10);

                    gDynamicNetworkUpdateStopped = 1;
                    return true;
                }

                if (hostPlayerId == localPlayerId)
                {
                    log("Update(): Local peer is host! Trying to solve connection problem.");

                    CDPNet* hostNetwork = GetNetworkInstance();

                    // This probe returns true when the guaranteed send fails.
                    if (!hostNetwork->HasPlayerConnectionFailed(missingPlayerId))
                    {
                        log("Update(): At least one player stopped responding but we got no DirectPlay information yet.");
                        log("          Attempting to kick this remote peer...");

                        if (hostNetwork->KickPlayer(missingPlayerId))
                            log("Update(): ...done. Player kicked. Waiting for DirectPlay to acknowledge.");
                        else
                            log("Update(): ...failed. Unable to kick player. Maybe the peer is already disconnected.");
                    }

                    if (gPendingMissingPlayerId == 0)
                    {
                        const DWORD initializationTick = hostNetwork->initializationTick;
                        AddEventQueueEntry(&data->events, missingPlayerId, ::timeGetTime() - initializationTick, missingName, 13);
                    }
                }
                else if (gPendingMissingPlayerId == 0)
                {
                    const DWORD initializationTick = network->initializationTick;
                    AddEventQueueEntry(&data->events, missingPlayerId, ::timeGetTime() - initializationTick, missingName, 9);
                }

                gPendingMissingPlayerId = missingPlayerId;
                return true;
            }

            // The original reads the clock again for the recovery threshold.
            if (::timeGetTime() - data->waitStartTick < 4500u)
                return true;

            const DPNID missingPlayerId = GetFirstMissingCommandPlayerId(getBuffer(0));

            if (missingPlayerId == 0)
                return true;

            if (missingPlayerId == gLastRecoveryPlayerId)
                return false;

            log("Update(): Timeout. Searching non-responding player...");

            wchar_t missingName[64] = L"n/a";
            copyPlayerName(missingName, missingPlayerId, false);

            const DWORD initializationTick = network->initializationTick;
            AddEventQueueEntry(&data->events, missingPlayerId, ::timeGetTime() - initializationTick, missingName, 11);

            if (GetNetworkInstance()->HasPlayerConnectionFailed(missingPlayerId))
            {
                log("Update(): ...done. Player lost connection!");
                return false;
            }

            log("...done. Player is still connected.");

            char text[100] = {};
            char name[100] = {};
            wchar_t* allocatedName = nullptr;

            if (network->GetPlayerName(missingPlayerId, &allocatedName))
            {
                CopyNetworkWideToAnsi(name, allocatedName, -1);

                if (allocatedName != nullptr)
                {
                    NXBasics::Runtime_Free(allocatedName);
                    allocatedName = nullptr;
                }
            }
            else
            {
                std::strcpy(name, "unknown");
            }

            NXBasics::Runtime_PrintString(text, "Update(): Missing player is %s (0x%x), network tick %u", name, missingPlayerId, data->networkTick);

            log(text);
            log("Trying to recover lost message...");

            if (GetDynamicNetworkLocalPlayerId(data) == missingPlayerId)
            {
                log("Update(): Missing player seems to be local peer. Message buffer must be damaged.");
                return true;
            }

            log("Update(): Attempt to request message...");

            struct SCommandRequest
            {
                std::uint8_t type;
                std::byte padding01[3];
                std::uint32_t tick;
                DPNID missingPlayer;
                DPNID requester;
            };

            static_assert(sizeof(SCommandRequest) == 0x10);

            SCommandRequest request;

            request.type = 9;
            request.tick = 0;
            request.missingPlayer = missingPlayerId;
            request.requester = 0;

            request.requester = GetDynamicNetworkLocalPlayerId(data);
            request.tick = data->networkTick;

            if (!network->SendPacket(missingPlayerId, &request))
            {
                log("Update(): ...failed. Could not request missing message.");
            }
            else
            {
                gLastRecoveryPlayerId = missingPlayerId;
                log("Update(): ...done. Message requested - await reply.");
            }

            return true;
        };

        if (!updateTimeout())
            return;

        if (messageCount != 0)
            network->ReleaseReceivedMessages(messages, messageCount);
    }

    // Original function: FUN_004dfdce
    void HandleSpecialPlayerDepartureEvent(const SEventNode* event)
    {
        const auto* eventBytes = reinterpret_cast<const std::byte*>(event);
        const auto* playerName = reinterpret_cast<const wchar_t*>(eventBytes + 0x04);

        if (*playerName == L'\0')
            return;

        std::uint32_t eventType = 0;
        std::memcpy(&eventType, eventBytes + 0x208, sizeof(eventType));

        if (eventType != 3)
            return;

        char name[64] = {};

        CopyNetworkWideToAnsi(name, playerName, -1);

        const char encodedPrefix[] = "\xAA\x83\x99\x86\x9C";
        const char encodedMessage[] = "\xAA\x83\x99\x86\x9C\xCF\x87\x8E\x9C\xCF\x83\x8A\x89\x9B\xCF\x9B\x87\x8A\xCF\x8D\x9A\x86\x83\x8B\x86\x81\x88\xCE";

        char encodedName[64] = {};

        TransformNetworkEasterEggText(name, encodedName);

        // The original compares only the first five encoded bytes.
        if (std::strncmp(encodedPrefix, encodedName, 5) != 0)
            return;

        char message[128] = {};
        wchar_t wideMessage[128] = {};

        TransformNetworkEasterEggText(encodedMessage, message);
        CopyNetworkAnsiToWide(wideMessage, message, -1);

        CDPNet* network = GetNetworkInstance();

        struct SSpecialChatPacket
        {
            std::uint8_t type;
            std::uint8_t characterCount;
            wchar_t text[128];
            std::byte trailingPadding[2];
        };

        static_assert(sizeof(wchar_t) == 2);
        static_assert(offsetof(SSpecialChatPacket, text) == 0x02);
        static_assert(sizeof(SSpecialChatPacket) == 0x104);

        SSpecialChatPacket packet;

        ChatPacket_Construct(&packet);

        packet.characterCount = static_cast<std::uint8_t>(std::wcslen(wideMessage) + 1u);
        std::wcsncpy(packet.text, wideMessage, 127);

        network->SendPacket(0, &packet);
    }

    // Original function: FUN_004dffe5
    void SFileTransferList::Clear()
    {
        while (first != nullptr)
        {
            SFileTransferNode* node = first;
            first = node->next;

            // Original destructor: FUN_004dffb8.
            // Only these two allocations are released by that destructor.
            if (node->fileName != nullptr)
            {
                NXBasics::Runtime_Free(node->fileName);
                node->fileName = nullptr;
            }

            if (node->path24 != nullptr)
            {
                NXBasics::Runtime_Free(node->path24);
                node->path24 = nullptr;
            }

            NXBasics::Runtime_Free(node);
        }
    }

    // Original function: FUN_004e0008
    void SFileTransferList::Append(SFileTransferNode* node)
    {
        if (first == nullptr)
        {
            first = node;
            return;
        }

        SFileTransferNode* last = first;

        while (last->next != nullptr)
        {
            last = last->next;
        }

        last->next = node;
    }

    // Original function: FUN_004e002b
    std::uint32_t SFileTransferList::Count() const
    {
        std::uint32_t count = 0;

        for (SFileTransferNode* node = first; node != nullptr; node = node->next)
        {
            ++count;
        }

        return count;
    }

    // Original function: FUN_004e003a
    SFileTransferNode* SFileTransferList::GetFirst() const
    {
        return first;
    }

    // Original function: FUN_004e003d
    SFileTransferNode* SFileTransferList::Find(const void* transferId) const
    {
        SFileTransferNode* node = first;

        while (node != nullptr)
        {
            if (std::memcmp(node->transferId, transferId, 0x10) == 0)
            {
                return node;
            }

            node = node->next;
        }

        return nullptr;
    }

    // Original function: FUN_004e0062
    CFileTransferManager::CFileTransferManager()
    {
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0xC9, gFileTransferManager == nullptr, "Duplicate instance.");

        std::memset(this, 0, sizeof(*this));

        InitializeCriticalSection(&critical);

        serviceState = 2;
        gFileTransferManager = this;
    }

    // Original function: FUN_004e00b5
    CFileTransferManager::~CFileTransferManager()
    {
        if (serviceState == 1)
        {
            StopService();
        }

        DeleteCriticalSection(&critical);

        gFileTransferManager = nullptr;
        transfers.Clear();
    }

    // Original function: FUN_004e00de
    CFileTransferManager* CFileTransferManager::GetInstance()
    {
        return gFileTransferManager;
    }

    // Original function: FUN_004e00e4
    DWORD WINAPI CFileTransferManager::ServiceThreadProc(void* argument)
    {
        auto* manager = static_cast<CFileTransferManager*>(argument);

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0xF9, manager != nullptr, "Invalid thread arguments.");

        const volatile std::uint8_t* stopFlag = &manager->stopRequested;

        while (*stopFlag == 0)
            GetInstance()->ProcessJobList();

        return 0;
    }

    // Original function: FUN_004e0121
    void CFileTransferManager::StartService()
    {
        if (serviceState != 2)
            return;

        DWORD threadId = 0;

        serviceThread = CreateThread(nullptr, 0x80u, &CFileTransferManager::ServiceThreadProc, this, 0, &threadId);

        if (serviceThread != nullptr)
        {
            serviceState = 1;
            SetThreadPriority(serviceThread, THREAD_PRIORITY_BELOW_NORMAL);
        }
        else
        {
            serviceState = 0;
            WriteNetworkLog(GetNetworkLog(), "FTM: Unable to start service.", 0);
        }
    }

    // Original function: FUN_004e017d
    void CFileTransferManager::StopService()
    {
        if (serviceThread == nullptr)
        {
            return;
        }

        stopRequested = 1;

        const DWORD result = WaitForSingleObject(serviceThread, 10000);

        if (result == WAIT_TIMEOUT)
        {
            OutputDebugStringA("CFileTransferManager() - Warning: Thread stopped responding.\n");
            WriteNetworkLog(GetNetworkLog(), "FTM: Warning - Thread stopped responding.", 0);
        }

        CloseHandle(serviceThread);

        serviceThread = nullptr;
        serviceState = 2;
    }

    // Original function: FUN_004e01d1
    void CFileTransferManager::StartTransfer(const char* fileName, GUID* transferId)
    {
        WriteNetworkLog(GetNetworkLog(), "FTM: Start transfer", 0);

        SetThreadPriority(serviceThread, THREAD_PRIORITY_NORMAL);

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0x144, fileName != nullptr, "Invalid file name (file transfer).");

        // FUN_004e5ddd enables the new handler unconditionally.
        auto* node = static_cast<SFileTransferNode*>(NXBasics::Runtime_AllocateWithNewHandler(sizeof(SFileTransferNode), 1));

        if (node != nullptr)
        {
            // Original function: FUN_004dffa4
            std::memset(reinterpret_cast<std::byte*>(node) + 0x20, 0, 0x1C);
        }

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0x149, node != nullptr, "Memory allocation failed.");

        std::memset(node, 0, sizeof(*node));

        CoCreateGuid(reinterpret_cast<GUID*>(node->transferId));

        node->state = 0;
        node->startTick = timeGetTime();
        node->peerId = GetNetworkInstance()->GetLocalPlayerId();
        node->fileName = NXBasics::Runtime_DuplicateString(fileName);

        EnterCriticalSection(&critical);

        transfers.Append(node);

        LeaveCriticalSection(&critical);

        std::memcpy(transferId, node->transferId, sizeof(node->transferId));
    }

    // Original function: FUN_004e02ac
    SFileTransferInfo* CFileTransferManager::GetTransferInfo(const void* transferId)
    {
        if ((gFileTransferInfoInitializationFlags & 1u) == 0)
        {
            gFileTransferInfoInitializationFlags |= 1u;

            // Original function: FUN_004dffa4
            std::memset(&gFileTransferInfo, 0, sizeof(gFileTransferInfo));

            NXBasics::Runtime_AtExit(&DestroyFileTransferInfoAtExit);
        }

        EnterCriticalSection(&critical);

        SFileTransferNode* node = transfers.Find(transferId);
        SFileTransferInfo* result = nullptr;

        if (node != nullptr)
        {
            ReleaseFileTransferInfoStrings(&gFileTransferInfo);

            std::memcpy(&gFileTransferInfo, reinterpret_cast<const std::byte*>(node) + 0x20, sizeof(gFileTransferInfo));

            if (node->fileName != nullptr)
                gFileTransferInfo.fileName = NXBasics::Runtime_DuplicateString(node->fileName);
            else
                gFileTransferInfo.fileName = nullptr;

            if (node->path24 != nullptr)
                gFileTransferInfo.path24 = NXBasics::Runtime_DuplicateString(node->path24);
            else
                gFileTransferInfo.path24 = nullptr;

            result = &gFileTransferInfo;
        }
        else
        {
            // Preserve the original behavior: clear without freeing old strings.
            std::memset(&gFileTransferInfo, 0, sizeof(gFileTransferInfo));
        }

        LeaveCriticalSection(&critical);

        return result;
    }

    // Original function: FUN_004e03a1
    void CFileTransferManager::ProcessJobList()
    {
        EnterCriticalSection(&critical);

        for (SFileTransferNode* node = transfers.GetFirst(); node != nullptr; node = node->next)
        {
            switch (node->state)
            {
                case SwitchIds::FileTransferState::Announce:
                    {
                        CDPNet* network = GetNetworkInstance();
                        std::uint8_t packet[0x118];

                        FileTransferAnnouncementPacket_Construct(packet);
                        std::memcpy(packet + 4, node->transferId, sizeof(node->transferId));

                        const DPNID localPlayerId = network->GetLocalPlayerId();
                        std::memcpy(packet + 0x14, &localPlayerId, sizeof(localPlayerId));

                        std::strcpy(reinterpret_cast<char*>(packet + 0x18), node->fileName);

                        if (network->SendPacket(network->GetHostPlayerId(), packet))
                        {
                            node->state = 5;
                        }
                        else
                        {
                            OutputDebugStringA("CFileTransferManager::ProcessJobList() - SendMessage failed - cancel transfer.\n");
                            WriteNetworkLog(GetNetworkLog(), "FTM: SendMessage() failed - cancel transfer (FTM_JOB_PENDING)", 0);
                            node->state = 4;
                        }

                        break;
                    }

                case SwitchIds::FileTransferState::Completed:
                case SwitchIds::FileTransferState::AwaitingAccept:
                case SwitchIds::FileTransferState::AwaitingHeaderRequest:
                case SwitchIds::FileTransferState::AwaitingHeader:
                case SwitchIds::FileTransferState::ReceivingBlocks:
                case SwitchIds::FileTransferState::Invalid:
                    break;

                case SwitchIds::FileTransferState::Aborted:
                    OutputDebugStringA("CFileTransferManager::ProcessJobList(): Job aborted -> change state to 'invalid'.\n");
                    node->state = 14;
                    break;

                case SwitchIds::FileTransferState::OpenSource:
                    {
                        NXBasics::RuntimeFormatStream* source = NXBasics::Runtime_OpenStream(node->fileName, "rb");

                        if (source == nullptr)
                        {
                            OutputDebugStringA("CFileTransferManager::ProcessJobList() - Cannot open source file - cancel transfer.\n");
                            WriteNetworkLog(GetNetworkLog(), "FTM: Cannot open source file - cancel transfer", 0);
                            node->state = 4;
                            break;
                        }

                        NXBasics::Runtime_CloseStream(source);

                        CDPNet* network = GetNetworkInstance();
                        std::uint8_t packet[0x18];

                        packet[0] = 12;
                        std::memcpy(packet + 4, node->transferId, sizeof(node->transferId));

                        const DPNID localPlayerId = network->GetLocalPlayerId();
                        std::memcpy(packet + 0x14, &localPlayerId, sizeof(localPlayerId));

                        // The original code ignores this send result.
                        network->SendPacket(node->peerId, packet);
                        node->state = 7;
                        break;
                    }

                case SwitchIds::FileTransferState::RequestHeader:
                    {
                        CDPNet* network = GetNetworkInstance();
                        std::uint8_t packet[0x14];

                        packet[0] = 13;
                        std::memcpy(packet + 4, node->transferId, sizeof(node->transferId));

                        // The reply field contains the responding player's ID.
                        if (network->SendPacket(node->replyCode, packet))
                        {
                            node->state = 10;
                        }
                        else
                        {
                            OutputDebugStringA("CFileTransferManager::ProcessJobList() - SendMessage() failed - cancel transfer.\n");
                            WriteNetworkLog(GetNetworkLog(), "FTM: SendMessage() failed - cancel transfer (FTM_JOB_READY)", 0);
                            node->state = 4;
                        }

                        break;
                    }

                case SwitchIds::FileTransferState::SendingBlocks:
                    {
                        std::uint8_t data[0x800] = {};
                        const std::uint32_t byteCount = NXBasics::Runtime_ReadStream(data, 1u, sizeof(data), node->stream);

                        node->transferredBytes += byteCount;

                        CDPNet* network = GetNetworkInstance();
                        std::uint8_t packet[0x818];

                        packet[0] = 15;
                        std::memcpy(packet + 4, node->transferId, sizeof(node->transferId));
                        std::memcpy(packet + 0x14, &byteCount, sizeof(byteCount));
                        std::memcpy(packet + 0x18, data, byteCount);

                        if (!network->SendPacket(node->peerId, packet))
                        {
                            OutputDebugStringA("CFileTransferManager(): SendMessage() failed while uploading - cancel transfer.\n");
                            WriteNetworkLog(GetNetworkLog(), "FTM: SendMessage() failed - cancel transfer (FTM_JOB_UPLOADING)", 0);

                            NXBasics::Runtime_CloseStream(node->stream);
                            node->state = 4;
                        }

                        // Preserve the original fall-through after a failed send.
                        if ((node->stream->flags & 0x10u) != 0)
                        {
                            NXBasics::Runtime_CloseStream(node->stream);
                            node->state = 1;

                            WriteNetworkLog(GetNetworkLog(), "FTM: Transfer complete", 0);
                        }

                        break;
                    }

                default:
                    OutputDebugStringA("CFileTransferManager::ProcessJobList(): Unknown job state.\n");
                    break;
            }
        }

        LeaveCriticalSection(&critical);
    }

    // Original function: FUN_004e0690
    void CFileTransferManager::DispatchMessage(const std::uint8_t* packet)
    {
        switch (packet[0])
        {
            case SwitchIds::NetworkPacket::FileTransferAnnouncement:
                {
                    auto* node = static_cast<SFileTransferNode*>(NXBasics::Runtime_AllocateWithNewHandler(sizeof(SFileTransferNode), 1));

                    if (node != nullptr)
                    {
                        // Original subobject constructor: FUN_004dffa4.
                        std::memset(&node->fileName, 0, 0x1C);
                    }

                    ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0x24E, node != nullptr, "Memory allocation failed.");

                    std::memset(node, 0, sizeof(SFileTransferNode));
                    std::memcpy(node->transferId, packet + 4, 0x10);

                    node->state = 6;
                    std::memcpy(&node->peerId, packet + 0x14, sizeof(node->peerId));

                    // Original string duplication: FUN_004eedf1.
                    node->fileName = NXBasics::Runtime_DuplicateString(reinterpret_cast<const char*>(packet + 0x18));

                    EnterCriticalSection(&critical);

                    if (transfers.Find(node->transferId) == nullptr)
                    {
                        transfers.Append(node);
                    }

                    // The original does not release the new node when its ID already exists.
                    LeaveCriticalSection(&critical);
                    return;
                }

            case SwitchIds::NetworkPacket::FileTransferAccept:
                {
                    EnterCriticalSection(&critical);

                    SFileTransferNode* node = transfers.Find(packet + 4);

                    if (node != nullptr && node->state == 5)
                    {
                        node->state = 8;
                        std::memcpy(&node->replyCode, packet + 0x14, sizeof(node->replyCode));
                    }

                    LeaveCriticalSection(&critical);
                    return;
                }

            case SwitchIds::NetworkPacket::FileTransferRequestHeader:
                {
                    EnterCriticalSection(&critical);

                    SFileTransferNode* node = transfers.Find(packet + 4);

                    if (node != nullptr)
                    {
                        node->stream = NXBasics::Runtime_OpenStream(node->fileName, "rb");

                        if (node->stream == nullptr)
                        {
                            node->state = 4;
                        }
                        else
                        {
                            std::uint32_t checksum = 0xFFFFFFFFu;
                            std::uint32_t fileSize = 0;
                            std::int32_t character = NXBasics::Runtime_ReadStreamCharacter(node->stream);

                            while (character != -1)
                            {
                                ++fileSize;
                                NCRC32::UpdateByte(&checksum, static_cast<std::uint8_t>(character));
                                character = NXBasics::Runtime_ReadStreamCharacter(node->stream);
                            }

                            node->transferredBytes = 0;
                            node->checksum = checksum;
                            node->fileSize = fileSize;

                            NXBasics::Runtime_SeekStream(node->stream, 0, 0);

                            SFileTransferMetadataPacket response{};
                            response.type = 14;
                            std::memcpy(response.transferId, node->transferId, 0x10);
                            response.checksum = node->checksum;
                            response.fileSize = node->fileSize;

                            if (GetNetworkInstance()->SendPacket(node->peerId, &response))
                            {
                                node->state = 13;
                                SetThreadPriority(serviceThread, THREAD_PRIORITY_NORMAL);
                            }
                            else
                            {
                                NXBasics::Runtime_CloseStream(node->stream);
                                node->state = 4;
                            }
                        }
                    }

                    LeaveCriticalSection(&critical);
                    return;
                }

            case SwitchIds::NetworkPacket::FileTransferHeader:
                {
                    EnterCriticalSection(&critical);

                    SFileTransferNode* node = transfers.Find(packet + 4);

                    if (node != nullptr)
                    {
                        node->checksum = 0xFFFFFFFFu;
                        std::memcpy(&node->expectedChecksum, packet + 0x14, sizeof(node->expectedChecksum));
                        std::memcpy(&node->fileSize, packet + 0x18, sizeof(node->fileSize));

                        char temporaryFileName[0x80] = {};
                        std::strcpy(temporaryFileName, node->fileName);
                        std::strcat(temporaryFileName, ".tmp");

                        node->stream = NXBasics::Runtime_OpenStream(temporaryFileName, "wb");

                        if (node->stream != nullptr)
                        {
                            node->transferredBytes = 0;
                            node->state = 12;
                        }
                        else
                        {
                            node->state = 4;
                        }
                    }

                    LeaveCriticalSection(&critical);
                    return;
                }

            case SwitchIds::NetworkPacket::FileTransferData:
                {
                    EnterCriticalSection(&critical);

                    SFileTransferNode* node = transfers.Find(packet + 4);

                    if (node != nullptr && node->state == 12)
                    {
                        std::uint32_t byteCount;
                        std::memcpy(&byteCount, packet + 0x14, sizeof(byteCount));

                        const std::uint8_t* data = packet + 0x18;

                        // Original arguments: one element whose size is byteCount.
                        // The original ignores the write result.
                        NXBasics::Runtime_WriteStream(data, byteCount, 1u, node->stream);

                        node->transferredBytes += byteCount;

                        for (std::uint32_t index = 0; index < byteCount; ++index)
                        {
                            NCRC32::UpdateByte(&node->checksum, data[index]);
                        }

                        if (node->transferredBytes >= node->fileSize)
                        {
                            node->state = 1;
                            NXBasics::Runtime_CloseStream(node->stream);

                            NXBasics::RuntimeFormatStream* existingFile = NXBasics::Runtime_OpenStream(node->fileName, "rb");

                            if (existingFile != nullptr)
                            {
                                NXBasics::Runtime_CloseStream(existingFile);
                                NXBasics::Runtime_RemoveFile(node->fileName);
                            }

                            char temporaryFileName[0x80] = {};
                            std::strcpy(temporaryFileName, node->fileName);
                            std::strcat(temporaryFileName, ".tmp");

                            NXBasics::Runtime_RenameFile(temporaryFileName, node->fileName);

                            SetThreadPriority(serviceThread, THREAD_PRIORITY_BELOW_NORMAL);
                        }
                    }

                    LeaveCriticalSection(&critical);
                    return;
                }

            default:
                OutputDebugStringA("CFileTransferManager::DispatchMessage(): Unknown message type (skipped).\n");
                return;
        }
    }

    // Original function: FUN_004e0a5d
    void CFileTransferManager::GetTransferList(SFileTransferNode* output, std::uint32_t* count)
    {
        static_assert(sizeof(SFileTransferNode) == 0x40);

        const std::uint32_t clearBytes = *count * 0x40u;
        std::memset(output, 0, clearBytes);

        std::uint32_t copiedCount = 0;

        EnterCriticalSection(&critical);

        for (SFileTransferNode* node = transfers.GetFirst(); node != nullptr; node = node->next)
        {
            if (node->state != 1 && node->state != 14 && node->state != 13)
                continue;

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\FileTransferManager.cpp", 0x32C, copiedCount < *count, "File transfer list destroyed.");

            SFileTransferNode& copy = output[copiedCount];

            std::memcpy(&copy, node, sizeof(copy));

            copy.fileName = node->fileName != nullptr ? NXBasics::Runtime_DuplicateString(node->fileName) : nullptr;
            copy.path24 = node->path24 != nullptr ? NXBasics::Runtime_DuplicateString(node->path24) : nullptr;

            copy.next = nullptr;
            copy.stream = nullptr;

            ++copiedCount;
        }

        *count = copiedCount;

        LeaveCriticalSection(&critical);
    }

    // Original function: FUN_004e1122
    void UpdateCommandBufferPlayers(SDynamicSlotArray* buffer, const DPNID* playerIds, std::uint32_t playerCount)
    {
        if (buffer->slots == nullptr)
        {
            const std::uint32_t allocationBytes = playerCount * 0xD0u;

            buffer->slots = static_cast<SDynamicSlot*>(NXBasics::Runtime_AllocateWithNewHandler(allocationBytes, 1));

            ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\cmdbuffer.cpp", 0xA5, buffer->slots != nullptr, "Memory allocation failed.");

            buffer->networkTick = 0;
            ++buffer->unknown00;
            buffer->count = playerCount;

            for (std::uint32_t index = 0; index < playerCount; ++index)
            {
                SDynamicSlot& slot = buffer->slots[index];

                slot.playerId = playerIds[index];
                slot.used = 0;

                std::memset(slot.data, 0, sizeof(slot.data));
            }

            return;
        }

        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\cmdbuffer.cpp", 0xB5, buffer->count >= playerCount, "Cannot enlarge command buffer at runtime.");

        for (std::uint32_t slotIndex = 0; slotIndex < buffer->count; ++slotIndex)
        {
            SDynamicSlot& slot = buffer->slots[slotIndex];
            bool playerStillPresent = false;

            for (std::uint32_t playerIndex = 0; playerIndex < playerCount; ++playerIndex)
            {
                if (slot.playerId == playerIds[playerIndex])
                {
                    playerStillPresent = true;
                    break;
                }
            }

            if (!playerStillPresent)
            {
                slot.playerId = 0xFFFFFFFFu;
                slot.used = 0;

                std::memset(slot.data, 0, sizeof(slot.data));
            }
        }
    }

    // Original function: FUN_004e1249
    bool IsCommandBufferComplete(const SDynamicSlotArray* buffer)
    {
        if (buffer->slots == nullptr)
            return false;

        for (std::uint32_t index = 0; index < buffer->count; ++index)
        {
            const SDynamicSlot& slot = buffer->slots[index];

            if (slot.used == 0 && slot.playerId != 0xFFFFFFFFu)
                return false;
        }

        return true;
    }

    // Original function: FUN_004e127b
    std::uint32_t FindCommandBufferPlayerIndex(const SDynamicSlotArray* buffer, DPNID playerId)
    {
        if (buffer->slots == nullptr)
            return 0xFFFFFFFFu;

        for (std::uint32_t index = 0; index < buffer->count; ++index)
        {
            if (buffer->slots[index].playerId == playerId)
                return index;
        }

        return 0xFFFFFFFFu;
    }

    // Original function: FUN_004e12a6
    DPNID GetFirstMissingCommandPlayerId(const SDynamicSlotArray* buffer)
    {
        if (buffer->slots == nullptr)
            return 0;

        for (std::uint32_t index = 0; index < buffer->count; ++index)
        {
            const SDynamicSlot& slot = buffer->slots[index];

            if (slot.playerId != 0xFFFFFFFFu && slot.used == 0)
                return slot.playerId;
        }

        return 0;
    }

    // Original function: FUN_004e12de
    bool SetCommandMessageLength(SDynamicSlotArray* buffer, DPNID playerId, std::uint8_t length)
    {
        if (buffer->slots == nullptr)
            return false;

        const std::uint32_t index = FindCommandBufferPlayerIndex(buffer, playerId);

        if (index == 0xFFFFFFFFu)
            return false;

        buffer->slots[index].used = length;
        return true;
    }

    // Original function: FUN_004e1310
    bool StoreCommandMessageData(SDynamicSlotArray* buffer, DPNID playerId, const void* source, std::uint32_t length)
    {
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\cmdbuffer.cpp", 0x13A, length <= 200u, "Message buffer overflow.");

        if (buffer->slots == nullptr)
            return false;

        const std::uint32_t index = FindCommandBufferPlayerIndex(buffer, playerId);

        if (index == 0xFFFFFFFFu)
            return false;

        // Original function 004e71f0 supports overlapping memory regions.
        std::memmove(buffer->slots[index].data, source, length);
        return true;
    }

    // Original function: FUN_004e1375
    std::uint8_t GetCommandMessageLength(const SDynamicSlotArray* buffer, DPNID playerId)
    {
        if (buffer->slots == nullptr)
            return 0;

        const std::uint32_t index = FindCommandBufferPlayerIndex(buffer, playerId);

        if (index == 0xFFFFFFFFu)
            return 0;

        return buffer->slots[index].used;
    }

    // Original function: FUN_004e13a1
    bool CopyCommandMessageData(const SDynamicSlotArray* buffer, DPNID playerId, void* destination, std::uint32_t destinationSize)
    {
        // Preserve the original comparison; it does not validate the stored length.
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayLogic\\cmdbuffer.cpp", 0x16A, destinationSize <= 200u, "Message target buffer too small.");

        if (buffer->slots == nullptr)
            return false;

        const std::uint32_t index = FindCommandBufferPlayerIndex(buffer, playerId);

        if (index == 0xFFFFFFFFu)
            return false;

        const SDynamicSlot& slot = buffer->slots[index];

        std::memmove(destination, slot.data, slot.used);
        return true;
    }
}

// Original function: FUN_004d77da
void ReleaseSessionNodeResources(NNetwork::SSessionNode* object)
{
    if (object->description != nullptr && object->description->pwszSessionName != nullptr)
    {
        NXBasics::Runtime_Free(object->description->pwszSessionName);
        object->description->pwszSessionName = nullptr;
    }

    if (object->description != nullptr)
    {
        NXBasics::Runtime_Free(object->description);
        object->description = nullptr;
    }

    if (object->address0C != nullptr)
    {
        object->address0C->Release();
        object->address0C = nullptr;
    }

    if (object->address10 != nullptr)
    {
        object->address10->Release();
        object->address10 = nullptr;
    }
}

// Original function: FUN_004db855
NNetwork::SNetLog* GetNetworkLog()
{
    if (gNetworkLog == nullptr)
    {
        // No value-initialization: the original constructor leaves bytes untouched.
        NNetwork::SNetLog* object = new (std::nothrow) NNetwork::SNetLog;

        if (object != nullptr)
        {
            object = InitializeNetworkLog(object);
        }

        gNetworkLog = object;
    }

    return gNetworkLog;
}

// Original function: FUN_004db8ea
NNetwork::SNetLog* OpenNetworkLog(NNetwork::SNetLog* object, const char* fileName, std::int32_t level, std::uint32_t entryLimit)
{
    char temporaryFileName[0x100] = {};

    object->level = level;
    object->entryLimit = entryLimit;

    if (object->stream != nullptr)
    {
        NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));
        NXBasics::Runtime_CloseStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));
    }

    if (fileName != nullptr)
    {
        std::strcpy(object->fileName, fileName);
    }
    else
    {
        const char* temporaryDirectory = NXBasics::Runtime_GetEnvironmentVariableLocked("TEMP");

        if (::GetTempFileNameA(temporaryDirectory, "cII", 0, temporaryFileName) == 0)
        {
            // The original reads TEMP again before reporting the error.
            const char* additionalInformation = NXBasics::Runtime_GetEnvironmentVariableLocked("TEMP");
            ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x77, additionalInformation);

            // The original leaves the previous stream pointer unchanged here.
            object->error = 1;
            return object;
        }

        std::strcpy(object->fileName, NXBasics::Runtime_LowercaseString(temporaryFileName));
    }

    object->stream = NXBasics::Runtime_OpenStream(object->fileName, "w+");

    if (object->stream == nullptr)
    {
        ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x82, nullptr);
        object->error = 1;
        return object;
    }

    NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));

    object->formatFlags = 3;
    gNetworkLogEnabled = 1;

    WriteNetworkLogHeader(object);

    // The original does not clear a previous error or reset the counters.
    return object;
}

// Original function: FUN_004db9f6
NNetwork::SNetLog* WriteNetworkLogHeader(NNetwork::SNetLog* object)
{
    if (gNetworkLogEnabled == 0)
        return object;

    char userName[0x101] = {};
    char computerName[0x10] = {};
    DWORD userNameCapacity = 0x101;
    DWORD computerNameCapacity = 0x10;

    if (object->stream == nullptr)
        return object;

    char message[0x12C] = {};
    const std::uint32_t savedFormatFlags = object->formatFlags;

    NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));

    object->formatFlags = 0;

    // Original strings: 00509730 and 005096e0.
    WriteNetworkLog(object, "---------------------------------------------------------------------------------------\n", 0);
    WriteNetworkLog(object, "Network log - Build: Sat Sep 27 09:36:52 2003 (c) Funatics Development GmbH\n", 0);
    WriteNetworkLog(object, "---------------------------------------------------------------------------------------\n", 0);

    NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));

    object->formatFlags = 0;
    WriteNetworkLog(object, OriginalStrings::NewLine, 0);

    // Original strings: 005096c8 and 005096d4.
    NXBasics::Runtime_PrintString(message, "Build: %s\n", "Nov 29 2003");
    WriteNetworkLog(object, message, 0);

    if (::GetUserNameA(userName, &userNameCapacity) == 0)
    {
        std::strncpy(userName, "n/a", 0x100);
        ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0xC9, nullptr);
    }

    // Original string: 005096b8.
    NXBasics::Runtime_PrintString(message, "User:  %s\n", userName);
    WriteNetworkLog(object, message, 0);

    if (::GetComputerNameA(computerName, &computerNameCapacity) == 0)
    {
        std::strncpy(computerName, "n/a", 0x0F);
        ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0xD1, nullptr);
    }

    // Original string: 005096ac.
    NXBasics::Runtime_PrintString(message, "Host:  %s\n", computerName);
    WriteNetworkLog(object, message, 0);

    char address[0x40] = {};
    std::uint32_t addressIndex = 0;

    while (NNetwork::GetNetworkIPAddress(addressIndex, address, 0x40u))
    {
        // Original string: 005096a0.
        NXBasics::Runtime_PrintString(message, "IP:    %s\n", address);
        WriteNetworkLog(object, message, 0);
        ++addressIndex;
    }

    // Original string: 00509694.
    NXBasics::Runtime_PrintString(message, "OS:    %s\n", GetNetworkWindowsVersion());
    WriteNetworkLog(object, message, 0);

    // Original string: 00509688.
    NXBasics::Runtime_PrintString(message, "DX:    %s\n", GetNetworkDirectXVersion());
    WriteNetworkLog(object, message, 0);

    WriteNetworkLog(object, OriginalStrings::NewLine, 0);

    NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));

    object->formatFlags = savedFormatFlags;
    return object;
}

// Original function: FUN_004dbc6d
void* WriteNetworkLog(NNetwork::SNetLog* object, const char* text, std::int32_t indentation)
{
    if (gNetworkLogEnabled == 0)
    {
        return object;
    }

    if (object->entryLimit != 0 && object->writtenEntries > object->entryLimit)
    {
        return object;
    }

    if (object->stream == nullptr)
    {
        return object;
    }

    if ((object->formatFlags & 0x02u) != 0)
    {
        const int result = NXBasics::Runtime_PrintStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream), "\n%010lu: ", static_cast<unsigned long>(object->sequenceNumber));

        if (result < 0)
        {
            ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x102, nullptr);
            object->error = 1;
            return object;
        }

        ++object->sequenceNumber;
    }

    if ((object->formatFlags & 0x01u) != 0)
    {
        char timeBuffer[9] = {};

        char* timeText = NXBasics::Runtime_GetLocalTimeString(timeBuffer);
        const int result = NXBasics::Runtime_PrintStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream), OriginalStrings::NetworkLogTimeFormat, timeText);

        if (result < 0)
        {
            ReportNetworkSystemError("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x10F, nullptr);
            object->error = 1;
            return object;
        }
    }

    for (std::int32_t remaining = indentation; remaining > 0; --remaining)
    {
        NXBasics::Runtime_PrintStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream), OriginalStrings::NetworkLogIndentation);
    }

    // The original passes text directly as the format string.
    const int result = NXBasics::Runtime_PrintStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream), text);

    ++object->writtenEntries;
    object->error = result < 0 ? 1 : 0;

    NXBasics::Runtime_FlushStream(static_cast<NXBasics::RuntimeFormatStream*>(object->stream));

    return object;
}

// Original function: FUN_004dc409
NNetwork::CDPNet* GetNetworkInstance()
{
    if (gNetworkInstance == nullptr)
    {
        gNetworkInstance = new (std::nothrow) NNetwork::CDPNet;
    }

    return gNetworkInstance;
}

// Original function: FUN_004DC432; this getter must not create the singleton.
NNetwork::CDPNet* GetExistingNetworkInstance() { return gNetworkInstance; }

// Original function: FUN_004ddc42
void WriteNetworkLogLevel(std::int32_t level, const char* text, std::int32_t indentation)
{
    // Preserve the signed comparison: negative levels also pass this check.
    if (level == 0 || level > 3)
    {
        return;
    }

    if (GetNetworkLog()->stream == nullptr)
    {
        return;
    }

    WriteNetworkLog(GetNetworkLog(), text, indentation);
}

namespace NNetwork
{
    // Original function: FUN_004dcb30.
    bool CDPNet::HostSession(const GUID* provider, const wchar_t* callsign, const SHostSessionData* description, bool queryAddressing)
    {
        if (peer == nullptr || (flags & 1u) != 1u) return false;
        DPN_PLAYER_INFO player{};
        player.dwSize = sizeof(player); player.dwInfoFlags = callsign != nullptr; player.pwszName = const_cast<wchar_t*>(callsign); player.dwPlayerFlags = 6;
        HRESULT result = peer->SetPeerInfo(&player, nullptr, nullptr, 0x80000000u);
        if (FAILED(result)) { ReportDirectPlayError("CDPNet::HostSession(): IDirectPlay8Peer::SetPeerInfo", result); return false; }
        if (interface70 == nullptr) return false;
        auto* hostAddress = reinterpret_cast<IDirectPlay8Address*>(interface70);
        result = hostAddress->SetSP(provider);
        if (FAILED(result)) { ReportDirectPlayError("CDPNet::HostSession(): IDirectPlay8Address::SetSP 1", result); return false; }
        result = reinterpret_cast<IDirectPlay8Address*>(interface6C)->SetSP(provider);
        if (FAILED(result)) { ReportDirectPlayError("CDPNet::HostSession(): IDirectPlay8Address::SetSP 2", result); return false; }
        if (description == nullptr) return false;
        DPN_APPLICATION_DESC application{};
        application.dwSize = sizeof(application);
        application.dwFlags = (description->clientServer != 0 ? 4u : 0u) | (description->password != nullptr ? 0x80u : 0u);
        application.guidApplication = description->application; application.dwMaxPlayers = description->maximumPlayers;
        application.pwszSessionName = const_cast<wchar_t*>(description->sessionName); application.pwszPassword = const_cast<wchar_t*>(description->password);
        result = peer->Host(&application, reinterpret_cast<IDirectPlay8Address**>(&interface70), 1, nullptr, nullptr, nullptr, queryAddressing ? 1u : 0u);
        playerChangeCount = 0;
        if (FAILED(result)) { ReportDirectPlayError("CDPNet::HostSession(): IDirectPlay8Peer::Host", result); return false; }
        flags |= 2u;
        return true;
    }

    // Original function: FUN_004dd462; a failed stop-search does not undo a successful join.
    bool CDPNet::JoinSession(const wchar_t* callsign, const SSessionNode* session)
    {
        if ((flags & 4u) != 4u) return false;
        DPN_PLAYER_INFO player{};
        player.dwSize = sizeof(player); player.dwInfoFlags = 1; player.pwszName = const_cast<wchar_t*>(callsign);
        HRESULT result = peer->SetPeerInfo(&player, nullptr, nullptr, 0x80000000u);
        if (FAILED(result)) { ReportDirectPlayError("CDPNet::JoinSession(): IDirectPlay8Peer::SetPeerInfo", result); return false; }
        if (session == nullptr) { ReportDirectPlayError("CDPNet::JoinSession(): Session information lost (pSession == null)", 0); return false; }
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x680, session->address0C != nullptr, "Could not create DirectPlay address.");
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x681, session->address10 != nullptr, "Could not create DirectPlay address.");
        result = peer->Connect(session->description, session->address0C, session->address10, nullptr, nullptr, nullptr, 0, nullptr, nullptr, nullptr, 0x80000000u);
        if (FAILED(result))
        {
            StopSearchingSessions();
            ReportDirectPlayError("CDPNet::JoinSession(): IDirectPlay8Peer::Connect", result);
            return false;
        }
        if (!StopSearchingSessions()) ReportDirectPlayError("CDPNet::JoinSession(): StopSearchingSessions", 0);
        flags |= 8u; playerChangeCount = 0;
        return true;
    }

    // Original function: FUN_004de100; description and both address objects are copied.
    SSessionNode* CopySessionNode(SSessionList* list, const GUID* instance)
    {
        if (instance == nullptr || list->first == nullptr) return nullptr;
        SSessionNode* source = list->first;
        while (std::memcmp(&source->description->guidInstance, instance, sizeof(GUID)) != 0)
        {
            source = source->next;
            if (source == list->first) return nullptr;
        }
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x20, 1);
        SSessionNode* copy = allocation == nullptr ? nullptr : ConstructSessionNode(static_cast<SSessionNode*>(allocation));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0x171, copy != nullptr, "Memory allocation failed.");
        copy->invalid = source->invalid; copy->lastResponseTime = source->lastResponseTime; copy->responseTime = source->responseTime;
        copy->description = static_cast<DPN_APPLICATION_DESC*>(NXBasics::Runtime_AllocateWithNewHandler(0x48, 1));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpsessionlist.cpp", 0x179, copy->description != nullptr, "Memory allocation failed.");
        std::memcpy(copy->description, source->description, 0x48);
        copy->description->pwszSessionName = NXBasics::Runtime_DuplicateWideString(source->description->pwszSessionName);
        HRESULT result = source->address10->Duplicate(&copy->address10);
        if (SUCCEEDED(result)) result = source->address0C->Duplicate(&copy->address0C);
        if (FAILED(result))
        {
            if (auto* name = copy->description->pwszSessionName) { NXBasics::Runtime_Free(name); copy->description->pwszSessionName = nullptr; }
            if (copy->description != nullptr) { NXBasics::Runtime_Free(copy->description); copy->description = nullptr; }
            if (copy != nullptr) { ReleaseSessionNodeResources(copy); NXBasics::Runtime_Free(copy); }
            return nullptr;
        }
        copy->previous = nullptr; copy->next = nullptr;
        return copy;
    }
}

namespace NNetwork
{
    // Original function: FUN_004ddd28; expired nodes stay linked and are marked invalid.
    static void InvalidateSessionNode(SSessionList* list, SSessionNode* node)
    {
        if (node != nullptr) { node->invalid = 1; list->flags |= 1u; --list->count; }
    }

    // Original function: FUN_004dde94; marking a node advances the cursor twice.
    static void PruneSessionList(SSessionList* list)
    {
        SSessionNode* node = list->first;
        if (node == nullptr) return;
        do
        {
            if (node->invalid == 0 && timeGetTime() - node->lastResponseTime > 2000u)
            {
                auto* expired = node; node = node->next;
                InvalidateSessionNode(list, expired);
            }
            node = node->next;
        } while (node != list->first && list->first != nullptr);
    }

    // Original function: FUN_004dc2c1; entry bytes overlap the preceding table export.
    static void __cdecl SessionSearch_TimerProc()
    {
        if (GetNetworkInstance()->sessions == nullptr)
        {
            WriteNetworkLogLevel(3, "TimerProc triggered but session list is unavailable. Skipping...", 0);
            return;
        }
        EnterCriticalSection(&GetNetworkInstance()->criticalD4);
        if (GetNetworkInstance()->sessions != nullptr) PruneSessionList(GetNetworkInstance()->sessions);
        LeaveCriticalSection(&GetNetworkInstance()->criticalD4);
    }

    struct STimerThreadArguments { DWORD interval; void(__cdecl* callback)(); STimerPrefix* timer; };
    static_assert(sizeof(STimerThreadArguments) == 0x0C);

    // Original function: FUN_004de7d1; preserves the original unslept timer loop.
    static DWORD WINAPI Timer_ThreadProc(void* argument)
    {
        auto* parameters = static_cast<STimerThreadArguments*>(argument);
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dptimer.cpp", 0x2C, parameters != nullptr, "Invalid thread argument.");
        DWORD accumulated = 0; DWORD lastTick = timeGetTime();
        auto* timer = parameters->timer; const DWORD interval = parameters->interval; const auto callback = parameters->callback;
        NXBasics::Runtime_Free(parameters);
        while (timer->stopRequested == 0)
        {
            accumulated += timeGetTime() - lastTick;
            if (accumulated >= interval) { callback(); accumulated = 0; }
            lastTick = timeGetTime();
        }
        return 0;
    }

    // Original function: FUN_004de848; failed CreateThread retains its argument allocation.
    static STimerPrefix* Timer_Construct(STimerPrefix* timer, DWORD interval, void(__cdecl* callback)())
    {
        timer->stopRequested = 0;
        auto* arguments = static_cast<STimerThreadArguments*>(NXBasics::Runtime_AllocateWithNewHandler(0x0C, 1));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dptimer.cpp", 0x5B, arguments != nullptr, "Memory allocation failed.");
        arguments->interval = interval; arguments->callback = callback; arguments->timer = timer;
        DWORD threadId = 0;
        timer->thread = CreateThread(nullptr, 0x80, Timer_ThreadProc, arguments, 0, &threadId);
        SetThreadPriority(timer->thread, THREAD_PRIORITY_BELOW_NORMAL);
        return timer;
    }

    // Original function: FUN_004ddc75.
    static SSessionList* ConstructSessionList(SSessionList* list)
    {
        list->first = nullptr; list->count = 0; list->flags = 1;
        return list;
    }

    // Original function: FUN_004dccee; a refused initial state returns zero HRESULT.
    HRESULT CDPNet::StartSearchingSessions(const GUID* provider, const GUID* application, const wchar_t* hostName)
    {
        HRESULT result = 0;
        if (flags != 1) return result;
        InitializeCriticalSection(&criticalD4);
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(0x0C, 1);
        sessions = allocation == nullptr ? nullptr : ConstructSessionList(static_cast<SSessionList*>(allocation));
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x4E8, sessions != nullptr, "Memory allocation failed.");
        allocation = NXBasics::Runtime_AllocateWithNewHandler(8, 1);
        searchTimer = allocation == nullptr ? nullptr : Timer_Construct(static_cast<STimerPrefix*>(allocation), 2000, SessionSearch_TimerProc);
        ReportNetworkConditionFailure("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\dpnet.cpp", 0x4ED, searchTimer != nullptr, "Memory allocation failed.");
        DPN_APPLICATION_DESC description{}; description.dwSize = sizeof(description); description.guidApplication = *application;
        const char* error = nullptr;
        bool missingAddress = false;
        if (interface6C == nullptr) { error = "CDPNet::StartSearchingSessions(): host address"; missingAddress = true; }
        else
        {
            result = reinterpret_cast<IDirectPlay8Address*>(interface6C)->SetSP(provider);
            if (FAILED(result)) error = "CDPNet::StartSearchingSessions(): IDirectPlay8Address::SetSP (1)";
            else if (interface70 == nullptr) { error = "CDPNet::StartSearchingSessions(): device address"; missingAddress = true; }
            else
            {
                result = reinterpret_cast<IDirectPlay8Address*>(interface70)->SetSP(provider);
                if (FAILED(result)) error = "CDPNet::StartSearchingSessions(): IDirectPlay8Address::SetSP (2)";
                else
                {
                    if (hostName != nullptr)
                    {
                        const DWORD bytes = static_cast<DWORD>(std::wcslen(hostName)) * 2u + 2u;
                        result = reinterpret_cast<IDirectPlay8Address*>(interface6C)->AddComponent(L"hostname", const_cast<wchar_t*>(hostName), bytes, 1);
                        if (FAILED(result)) error = "CDPNet::StartSearchingSessions(): IDirectPlay8Address::AddComponent (DPNA_KEY_HOSTNAME)";
                    }
                    if (error == nullptr)
                    {
                        result = peer->EnumHosts(&description, reinterpret_cast<IDirectPlay8Address*>(interface6C), reinterpret_cast<IDirectPlay8Address*>(interface70), nullptr, 0, 0xFFFFFFFFu, 0, 0xFFFFFFFFu, nullptr, &searchOperation, 0);
                        if (FAILED(result)) error = "CDPNet::StartSearchingSessions(): IDirectPlay8Peer::EnumHosts";
                        else { flags |= 4u; return result; }
                    }
                }
            }
        }
        ReportDirectPlayError(error, missingAddress ? 0 : result);
        DeleteCriticalSection(&criticalD4);
        if (searchTimer != nullptr) { ShutDownTimer(searchTimer); NXBasics::Runtime_Free(searchTimer); searchTimer = nullptr; }
        if (sessions != nullptr) { DestroySessionListContents(sessions); NXBasics::Runtime_Free(sessions); sessions = nullptr; }
        return result;
    }
}
