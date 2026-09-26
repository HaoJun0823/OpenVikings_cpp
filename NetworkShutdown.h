#pragma once

#include <Windows.h>
#include "dplay8.h"
#include "dpaddr.h"

#include <cstddef>
#include <cstdint>

namespace NNetwork
{
    struct SEventNode
    {
        DPNID playerId;                           // +0x000
        wchar_t playerName[0x100];                 // +0x004
        std::uint32_t timestamp;                  // +0x204
        std::uint32_t eventType;                  // +0x208
        SEventNode* next;                         // +0x20C
    };

    static_assert(sizeof(SEventNode) == 0x210);
    static_assert(offsetof(SEventNode, playerName) == 0x004);
    static_assert(offsetof(SEventNode, timestamp) == 0x204);
    static_assert(offsetof(SEventNode, eventType) == 0x208);

    struct SEventQueue
    {
        std::uint8_t unknown00;                    // +0x00
        std::uint8_t changed;                      // +0x01
        std::byte unknown02[2];
        std::uint32_t count;                       // +0x04
        SEventNode* first;                         // +0x08
        SEventNode* last;                          // +0x0C
    };

    // Original function: FUN_004db528
    SEventQueue* ClearEventQueue(SEventQueue* queue);

    struct SReceivedMessage
    {
        DPNID sender;                             // +0x00
        std::uint32_t size;                        // +0x04
        void* data;                               // +0x08
        DPNHANDLE bufferHandle;                    // +0x0C
    };

    struct SSessionNode
    {
        std::uint8_t invalid;                       // +0x00
        std::byte unknown01[3];
        std::uint32_t responseTime;                 // +0x04
        std::uint32_t lastResponseTime;             // +0x08
        IDirectPlay8Address* address0C;              // +0x0C
        IDirectPlay8Address* address10;              // +0x10
        DPN_APPLICATION_DESC* description;          // +0x14
        SSessionNode* next;                         // +0x18
        SSessionNode* previous;                     // +0x1C
    };

    struct SSessionList
    {
        std::uint32_t count;                       // +0x00
        std::uint32_t flags;                       // +0x04
        SSessionNode* first;                       // +0x08
    };

    // Original function: FUN_004ddd3c
    void UpdateSessionList(SSessionList* list, const DPNMSG_ENUM_HOSTS_RESPONSE* response);

    static_assert(sizeof(DPN_APPLICATION_DESC) == 0x48);
    static_assert(offsetof(DPN_APPLICATION_DESC, guidInstance) == 0x08);
    static_assert(offsetof(DPN_APPLICATION_DESC, pwszSessionName) == 0x30);

    struct SNetLog
    {
        std::uint8_t error;                         // +0x000
        std::byte unknown001[3];
        std::uint32_t unknown004;                   // +0x004
        std::uint32_t formatFlags;                  // +0x008
        std::uint32_t unknown00C;                   // +0x00C
        std::uint32_t entryLimit;                   // +0x010
        std::uint32_t writtenEntries;               // +0x014
        std::uint32_t sequenceNumber;               // +0x018
        std::int32_t level;                         // +0x01C
        char fileName[0x100];                       // +0x020
        void* stream;                               // +0x120
    };

    // Only the prefix accessed during destruction is described here.
    struct STimerPrefix
    {
        std::uint8_t stopRequested;                 // +0x00
        std::byte unknown01[3];
        HANDLE thread;                              // +0x04
    };

    struct SDynamicSlot
    {
        std::uint32_t unknown00;                   // +0x00
        std::uint8_t used;                         // +0x04: message length; zero means missing
        std::byte data[200];                       // +0x05
        std::byte unknownCD[3];
    };

    struct SDynamicSlotArray
    {
        std::uint32_t unknown00;                   // +0x00
        std::uint32_t revision;                    // +0x04
        std::uint32_t count;                       // +0x08
        std::uint32_t unknown0C;                   // +0x0C
        SDynamicSlot* slots;                       // +0x10
    };

    struct SDynamicDataMessage
    {
        DPNID sender;                       // +0x000
        std::uint32_t payloadSize;          // +0x004
        std::uint8_t payload[0x400];         // +0x008
        std::uint8_t checksumMismatch;      // +0x408
        std::byte padding409[3];            // +0x409
    };

    static_assert(sizeof(SDynamicDataMessage) == 0x40C);
    static_assert(offsetof(SDynamicDataMessage, payload) == 0x008);
    static_assert(offsetof(SDynamicDataMessage, checksumMismatch) == 0x408);
    static_assert(sizeof(SDynamicDataMessage) * 64 == 0x10300);

    // Storage required by the original constructor and existing shutdown paths.
    struct SDynamicNetworkData
    {
        std::uint8_t active;                       // +0x00
        std::byte unknown01[3];
        std::uint32_t unknown04;                   // +0x04
        std::uint32_t unknown08;                   // +0x08
        DWORD unknown0C;                   // +0x0C
        void* allocation10;                       // +0x10
        SEventQueue events;                        // +0x14
        std::uint32_t receivedRecordCount;         // +0x24
        void* allocation28;                       // +0x28
        std::uint32_t unknown2C;                   // +0x2C
        void* allocation30;                       // +0x30
        SDynamicSlotArray arrays[3];               // +0x34, +0x48, +0x5C
        std::byte unknown70[0x0C];
        std::uint32_t unknown7C;                   // +0x7C
    };

    SEventQueue* AppendEventQueueCopy(SEventQueue* destination, const SEventQueue* source);

    SEventQueue* ReplaceEventQueueAndClearSource(SEventQueue* destination, SEventQueue* source);

    DPNID GetDynamicNetworkHostPlayerId(const SDynamicNetworkData* data);

    DPNID GetDynamicNetworkLocalPlayerId(const SDynamicNetworkData* data);

    bool SendDynamicNetworkChatMessage(SDynamicNetworkData* data, DPNID recipient, const wchar_t* text);

    bool TakeDynamicNetworkTextRecords(SDynamicNetworkData* data, void** records, std::uint32_t* recordCount);

    bool SendDynamicNetworkMessage(SDynamicNetworkData* data, DPNID recipient, const void* payload, std::uint32_t byteCount);

    void PrepareDynamicCommandBuffers(SDynamicNetworkData* data);

    void ClearDynamicNetworkEvents(SDynamicNetworkData* data);

    // Extracted initialization and event processing from FUN_004df240.
    // Returns true only when the remaining update stages should execute.
    bool PrepareDynamicNetworkUpdate(SDynamicNetworkData* data, bool reset);

    bool BroadcastControlMessage8(SDynamicNetworkData* data);

    // Extracted type-4 receive branch from FUN_004df240.
    void StoreDynamicDataMessage(SDynamicNetworkData* data, const SReceivedMessage& received);

    void GetDynamicNetworkMessages(SDynamicNetworkData* data, SDynamicDataMessage** messages, std::uint32_t* count);

    inline std::uint32_t gDynamicSlotArrayConstructionCount = 0;

    void InitializeGlobalDynamicNetworkData();

    void ConvertNetworkAnsiToWide(wchar_t* destination, const char* source, std::int32_t length);

    void ConvertNetworkWideToAnsi(char* destination, const wchar_t* source, std::int32_t length);

    void CopyNetworkAnsiToWide(wchar_t* destination, const char* source, std::int32_t length);

    void CopyNetworkWideToAnsi(char* destination, const wchar_t* source, std::int32_t length);

    SDynamicNetworkData* ConstructDynamicNetworkData(SDynamicNetworkData* data);

    void DestroyDynamicNetworkData(SDynamicNetworkData* data);

    void UpdateDynamicCommandBuffers(SDynamicNetworkData* data);

    void UpdateDynamicNetwork(SDynamicNetworkData* data, bool reset);

    void HandleSpecialPlayerDepartureEvent(const SEventNode* event);

    void UpdateCommandBufferPlayers(SDynamicSlotArray* buffer, const DPNID* playerIds, std::uint32_t playerCount);

    bool IsCommandBufferComplete(const SDynamicSlotArray* buffer);

    std::uint32_t FindCommandBufferPlayerIndex(const SDynamicSlotArray* buffer, DPNID playerId);

    DPNID GetFirstMissingCommandPlayerId(const SDynamicSlotArray* buffer);

    bool SetCommandMessageLength(SDynamicSlotArray* buffer, DPNID playerId, std::uint8_t length);

    bool StoreCommandMessageData(SDynamicSlotArray* buffer, DPNID playerId, const void* source, std::uint32_t length);

    std::uint8_t GetCommandMessageLength(const SDynamicSlotArray* buffer, DPNID playerId);

    bool CopyCommandMessageData(const SDynamicSlotArray* buffer, DPNID playerId, void* destination, std::uint32_t destinationSize);

    static_assert(sizeof(SDynamicNetworkData) == 0x80);
    static_assert(offsetof(SDynamicNetworkData, receivedRecordCount) == 0x24);
    static_assert(offsetof(SDynamicNetworkData, unknown7C) == 0x7C);

    // Reconstructed player record populated by FUN_004dd0de.
    struct SPlayerInfo
    {
        DPNID playerId;                           // +0x00
        wchar_t playerName[0x40];                 // +0x04
        std::uint8_t isHost;                      // +0x84
        std::uint8_t isLocal;                     // +0x85
        std::byte reserved86[2];                  // +0x86
        std::uint32_t bytesSent;                  // +0x88
        std::uint32_t bytesReceived;              // +0x8C
        std::uint32_t roundTripLatencyMS;         // +0x90
        std::uint32_t throughputBPS;              // +0x94
    };

    static_assert(sizeof(SPlayerInfo) == 0x98);
    static_assert(offsetof(SPlayerInfo, playerName) == 0x04);
    static_assert(offsetof(SPlayerInfo, isHost) == 0x84);
    static_assert(offsetof(SPlayerInfo, isLocal) == 0x85);
    static_assert(offsetof(SPlayerInfo, bytesSent) == 0x88);
    static_assert(offsetof(SPlayerInfo, bytesReceived) == 0x8C);
    static_assert(offsetof(SPlayerInfo, roundTripLatencyMS) == 0x90);
    static_assert(offsetof(SPlayerInfo, throughputBPS) == 0x94);

    static_assert(sizeof(DPN_PLAYER_INFO) == 0x18);
    static_assert(sizeof(DPN_CONNECTION_INFO) == 0x5C);
    static_assert(offsetof(DPN_CONNECTION_INFO, dwBytesSentGuaranteed) == 0x10);
    static_assert(offsetof(DPN_CONNECTION_INFO, dwBytesSentNonGuaranteed) == 0x18);
    static_assert(offsetof(DPN_CONNECTION_INFO, dwBytesReceivedGuaranteed) == 0x48);
    static_assert(offsetof(DPN_CONNECTION_INFO, dwBytesReceivedNonGuaranteed) == 0x50);

    class CDPNet
    {
    public:
        // Original function: FUN_004dbd68
        static HRESULT WINAPI MessageProc(void* context, DWORD messageId, void* message);

        // Original function: FUN_004dc31e.
        CDPNet();

        // Original function: FUN_004dc391
        ~CDPNet();

        // Original function: FUN_004dc3f3
        void SetApplicationGuid(const GUID* applicationGuid);

        // Original function: FUN_004dc45f
        bool Initialize(void* context, bool option);

        // Original function: FUN_004dc700.
        bool InitializePeer(void* context, bool option);

        // Original function: FUN_004dc7a3
        bool Uninitialize();

        // Original function: FUN_004dc9cc
        DPNID GetLocalPlayerId();

        // Original function: FUN_004dca43
        DPNID GetHostPlayerId();

        // Original function: FUN_004dcc9b
        bool LeaveSession();

        // Original function: FUN_004dcedd
        bool StopSearchingSessions();

        // Original function: FUN_004dcf42
        bool GetPlayerName(DPNID playerId, wchar_t** playerName);

        // Original function: FUN_004dcfe9
        bool GetPlayerIds(DPNID** playerIds, DWORD* playerCount);

        // Original function: FUN_004dd0de
        // Initialize *players to nullptr before calling.
        // Release a successful result with NXBasics::Runtime_Free.
        bool GetPlayerInfo(SPlayerInfo** players, DWORD* playerCount);

        // Original function: FUN_004dd5f2
        bool KickPlayer(DPNID playerId);

        // Original function: FUN_004dd65e
        bool SendPacket(DPNID recipient, const void* packet);

        // Original function: FUN_004dd888
        void SaveReceivedMessage(const DPNMSG_RECEIVE* message);

        // Original function: FUN_004dd948
        void GetReceivedMessages(SReceivedMessage** messages, std::uint32_t* count);

        // Original function: FUN_004dd9fe
        void ReleaseReceivedMessages(SReceivedMessage* messages, std::uint32_t count);

        // Original function: FUN_004dda6f
        void GetSendStatistics(std::uint32_t* statistics);

        // Original function: FUN_004ddaa5
        void SetSendStatistics(const std::uint32_t* statistics);

        // Original function: FUN_004ddadb
        void AddPlayerEvent(DPNID playerId, std::uint32_t eventType);

        // Original function: FUN_004ddb3e
        void GetEvents(SEventQueue* destination);

        // Original function: FUN_004ddb76
        void ClearEvents();

        // Original function: FUN_004ddb9f
        bool HasPlayerConnectionFailed(DPNID playerId);

        std::uint32_t flags;                       // +0x000
        std::uint32_t playerChangeCount;           // +0x004
        std::uint8_t allowConnections;             // +0x008
        std::uint8_t playerInfoChanged;            // +0x009
        std::byte unknown00A[2];
        STimerPrefix* searchTimer;                 // +0x00C
        SSessionList* sessions;                    // +0x010
        DWORD initializationTick;                  // +0x014
        SEventQueue events18;                      // +0x018
        SEventQueue events28;                      // +0x028
        std::uint32_t receivedMessageCount;        // +0x038
        SReceivedMessage* receivedMessages;        // +0x03C
        DPNHANDLE searchOperation;                 // +0x040
        void* callbackContext;                     // +0x044
        std::byte data48[0x10];                    // +0x048
        std::byte data58[0x10];                    // +0x058
        IDirectPlay8Peer* peer;                    // +0x068
        IUnknown* interface6C;                     // +0x06C
        IUnknown* interface70;                     // +0x070
        CRITICAL_SECTION critical74;               // +0x074
        CRITICAL_SECTION critical8C;               // +0x08C
        CRITICAL_SECTION criticalA4;               // +0x0A4
        CRITICAL_SECTION criticalBC;               // +0x0BC
        CRITICAL_SECTION criticalD4;               // +0x0D4
        CRITICAL_SECTION criticalEC;               // +0x0EC
        SNetLog log;                               // +0x104
    };

    extern GUID gNetworkApplicationGuid;

    bool Network_StartUp();
    void Network_ShutDown();
    std::uint32_t Network_GetFlags();
    bool Network_IsHost();
    bool Network_IsInSession();
    DPNID Network_GetLocalPlayerId();
    DPNID Network_GetHostPlayerId();

    char* Network_GetCallsign();
    void Network_SetCallsign(const char* callsign);
    void Network_InitializeCallsign();
    std::int32_t Network_GetCallsignBufferSize();
    bool Network_GetPlayerName(DPNID playerId, char* destination);
    void DestroyNetworkSingleton();

    static_assert(sizeof(void*) == 4, "This reconstruction requires an x86 build");
    static_assert(sizeof(SEventQueue) == 0x10);
    static_assert(offsetof(SEventNode, next) == 0x20C);
    static_assert(sizeof(SReceivedMessage) == 0x10);
    static_assert(offsetof(SReceivedMessage, bufferHandle) == 0x0C);
    static_assert(offsetof(SSessionNode, description) == 0x14);
    static_assert(offsetof(SSessionNode, next) == 0x18);
    static_assert(sizeof(SSessionNode) == 0x20);
    static_assert(sizeof(SNetLog) == 0x124);
    static_assert(offsetof(SNetLog, formatFlags) == 0x008);
    static_assert(offsetof(SNetLog, stream) == 0x120);
    static_assert(offsetof(STimerPrefix, thread) == 0x04);
    static_assert(sizeof(SDynamicSlot) == 0xD0);
    static_assert(sizeof(SDynamicSlotArray) == 0x14);
    static_assert(offsetof(SDynamicNetworkData, events) == 0x14);
    static_assert(offsetof(SDynamicNetworkData, allocation28) == 0x28);
    static_assert(offsetof(SDynamicNetworkData, arrays) == 0x34);

    static_assert(offsetof(CDPNet, allowConnections) == 0x008);
    static_assert(offsetof(CDPNet, playerInfoChanged) == 0x009);
    static_assert(offsetof(CDPNet, events18) == 0x18);
    static_assert(offsetof(CDPNet, receivedMessages) == 0x3C);
    static_assert(offsetof(CDPNet, peer) == 0x68);
    static_assert(offsetof(CDPNet, critical74) == 0x74);
    static_assert(offsetof(CDPNet, critical8C) == 0x8C);
    static_assert(offsetof(CDPNet, criticalA4) == 0xA4);
    static_assert(offsetof(CDPNet, criticalBC) == 0xBC);
    static_assert(offsetof(CDPNet, criticalD4) == 0xD4);
    static_assert(offsetof(CDPNet, criticalEC) == 0xEC);
    static_assert(offsetof(CDPNet, log) == 0x104);
    static_assert(sizeof(CDPNet) == 0x228);
}

// Existing reconstruction interfaces, with their original addresses retained.
// Original function: FUN_004d77da
void ReleaseSessionNodeResources(NNetwork::SSessionNode* object);

// Original function: FUN_004db855
NNetwork::SNetLog* GetNetworkLog();

// Original function: FUN_004db8ea
NNetwork::SNetLog* OpenNetworkLog(NNetwork::SNetLog* object, const char* fileName, std::int32_t level, std::uint32_t entryLimit);

// Original function: FUN_004db9f6
NNetwork::SNetLog* WriteNetworkLogHeader(NNetwork::SNetLog* object);

// Original function: FUN_004dbc6d
void* WriteNetworkLog(NNetwork::SNetLog* object, const char* text, std::int32_t indentation);

// Original function: FUN_004dc409
NNetwork::CDPNet* GetNetworkInstance();

// Original function: FUN_004ddc42
void WriteNetworkLogLevel(std::int32_t level, const char* text, std::int32_t indentation);

// Original data: 00569654
extern NNetwork::SNetLog* gNetworkLog;

// Original data: 00569658
extern NNetwork::CDPNet* gNetworkInstance;

// Original data: 0050f6a8.
extern NNetwork::SDynamicNetworkData DAT_0050f6a8;
