#include "DirectXErrorText.h"
#include "OriginalStrings.h"
#include <new>
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"
#include "FileTransferManager.h"
#include "MasterPropertyManager.h"

#include "dplay8.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <Windows.h>

// Original data: DAT_00509910.
// The diagnostic string contents are not present in the supplied ASM export.
extern const char DAT_00509910[];

// Original data: DAT_0050f6a8.
NNetwork::SDynamicNetworkData gDynamicNetworkData{};

// Original data: 0050f728..0050f74f.
char gNetworkCallsign[0x28]{};

// Original data: 0050f750.
std::uint8_t gNetworkCallsignInitialized = 0;

namespace NNetwork
{
    // Original function: FUN_004db528
    SEventQueue* ClearEventQueue(SEventQueue* queue)
    {
        while (queue->first != nullptr)
        {
            SEventNode* node = queue->first;
            queue->first = node->next;

            NXBasics::Runtime_Free(node);
        }

        queue->last = nullptr;
        queue->first = nullptr;
        queue->count = 0;
        queue->changed = 0;

        return queue;
    }

    namespace
    {

        void CloseNetworkLog(SNetLog* log);
        void ReleaseDynamicSlots(SDynamicSlotArray* array);

        // Shared expansion of the original four OutputDebugStringA calls.
        void ReportDirectPlayError(const char* operation, HRESULT result)
        {
            OutputDebugStringA(operation);
            OutputDebugStringA(OriginalStrings::DirectPlayErrorSeparator);
            OutputDebugStringA(NXBasics::GetDirectXErrorText(result));
            OutputDebugStringA(OriginalStrings::NewLine);
        }

        // Original function: FUN_004db87e
        void DestroyLogSingleton()
        {
            SNetLog* log = gNetworkLog;

            if (log != nullptr)
            {
                CloseNetworkLog(log);
                ::operator delete(log);

                gNetworkLog = nullptr;
            }
        }

        // Original function: FUN_004db8c8
        void CloseNetworkLog(SNetLog* log)
        {
            if (log->stream != nullptr)
            {
                auto* stream = static_cast<NXBasics::RuntimeFormatStream*>(log->stream);
                NXBasics::Runtime_FlushStream(stream);
                NXBasics::Runtime_CloseStream(stream);
            }
        }

        // Original function: FUN_004ddc86
        void DestroySessionListContents(SSessionList* list)
        {
            SSessionNode* node = list->first;

            if (node == nullptr)
            {
                return;
            }

            SSessionNode* last = node->previous;

            if (last == node)
            {
                ReleaseSessionNodeResources(node);
                NXBasics::Runtime_Free(node);
                return;
            }

            bool finished;

            do
            {
                node = list->first;
                list->first = node->next;

                // The original stops after destroying the captured last node.
                finished = node == last;

                ReleaseSessionNodeResources(node);
                NXBasics::Runtime_Free(node);
            } while (!finished);
        }

        // Original function: FUN_004de8b9
        void ShutDownTimer(STimerPrefix* timer)
        {
            if (timer->thread == nullptr)
            {
                return;
            }

            const HANDLE thread = timer->thread;
            timer->stopRequested = 1;

            const DWORD result = WaitForSingleObject(thread, INFINITE);

            if (result == WAIT_TIMEOUT)
            {
                char buffer[20] = {};

                OutputDebugStringA(OriginalStrings::TimerNotRespondingWarning);
                OutputDebugStringA(OriginalStrings::TimerThreadLabel);

                NXBasics::Runtime_PrintString(buffer, OriginalStrings::TimerThreadFormat, timer->thread);
                OutputDebugStringA(buffer);

                OutputDebugStringA(OriginalStrings::TimerTimeoutLabel);

                NXBasics::Runtime_LongToString(-1, buffer, 10);
                OutputDebugStringA(buffer);

                OutputDebugStringA(OriginalStrings::NewLine);
            }

            CloseHandle(timer->thread);
            timer->thread = nullptr;
        }

        // Original function: FUN_004de9da
        void ReleaseDynamicNetworkMemory(SDynamicNetworkData* data)
        {
            WriteNetworkLog(GetNetworkLog(), "Deleting dynamic memory", 0);

            ClearEventQueue(&data->events);

            if (data->allocation10 != nullptr)
            {
                NXBasics::Runtime_Free(data->allocation10);
                data->allocation10 = nullptr;
            }

            if (data->allocation28 != nullptr)
            {
                NXBasics::Runtime_Free(data->allocation28);
                data->allocation28 = nullptr;
            }

            if (data->allocation30 != nullptr)
            {
                NXBasics::Runtime_Free(data->allocation30);
                data->allocation30 = nullptr;
            }

            ReleaseDynamicSlots(&data->arrays[0]);
            ReleaseDynamicSlots(&data->arrays[1]);
            ReleaseDynamicSlots(&data->arrays[2]);
        }

        // Original function: FUN_004debfd
        void ShutDownDynamicNetworkData(SDynamicNetworkData* data)
        {
            ReleaseDynamicNetworkMemory(data);
            data->active = 0;
        }

        // Original function: FUN_004e101b
        void ResetDynamicSlots(SDynamicSlotArray* array)
        {
            if (array->slots == nullptr)
            {
                return;
            }

            for (std::uint32_t index = 0; index < array->count; ++index)
            {
                array->slots[index].used = 0;
                std::memset(array->slots[index].data, 0, 200);
            }
        }

        // Original function: FUN_004e10fb
        void ReleaseDynamicSlots(SDynamicSlotArray* array)
        {
            if (array->slots != nullptr)
            {
                ++array->revision;
            }

            ResetDynamicSlots(array);

            if (array->slots != nullptr)
            {
                NXBasics::Runtime_Free(array->slots);
                array->slots = nullptr;
            }
        }
    }

    // Original function: FUN_004058dd
    void InitializeGlobalDynamicNetworkData()
    {
        ConstructDynamicNetworkData(&gDynamicNetworkData);

        // Original function: FUN_004058f3
        // The existing reconstructed onexit interface expects an int return value.
        NXBasics::Runtime_AtExit(+[] () -> int
        {
            DestroyDynamicNetworkData(&gDynamicNetworkData);
            return 0;
        });
    }

    // Original function: FUN_00405949
    void Network_ShutDown()
    {
        ShutDownDynamicNetworkData(&gDynamicNetworkData);

        CDPNet* network = GetNetworkInstance();

        if ((network->flags & 0x04u) != 0)
        {
            network->StopSearchingSessions();
        }

        if ((network->flags & 0x08u) != 0)
        {
            network->LeaveSession();
        }

        // Read the flags again after LeaveSession().
        if ((network->flags & 0x02u) != 0)
        {
            network->LeaveSession();
        }

        network->Uninitialize();

        DestroyNetworkSingleton();
    }

    // Original function: FUN_00405999
    std::uint32_t Network_GetFlags()
    {
        return GetNetworkInstance()->flags;
    }

    // Original function: FUN_004059a1
    bool Network_IsHost()
    {
        return (GetNetworkInstance()->flags & 0x02u) != 0;
    }

    // Original function: FUN_004059b3
    bool Network_IsInSession()
    {
        // Original helper 004dc432 returns the existing singleton without creating it.
        if (gNetworkInstance == nullptr)
        {
            return false;
        }

        return (GetNetworkInstance()->flags & 0x0Au) != 0;
    }

    // Original function: FUN_004059cb
    DPNID Network_GetLocalPlayerId()
    {
        return GetNetworkInstance()->GetLocalPlayerId();
    }

    // Original function: FUN_004059d7
    DPNID Network_GetHostPlayerId()
    {
        return GetNetworkInstance()->GetHostPlayerId();
    }

    // Original function: FUN_004059e3
    char* Network_GetCallsign()
    {
        return gNetworkCallsign;
    }

    // Original function: FUN_004059e9
    void Network_SetCallsign(const char* callsign)
    {
        std::strncpy(gNetworkCallsign, callsign, 0x27);
        gNetworkCallsign[0x27] = '\0';
    }

    // Original function: FUN_00405a04
    void Network_InitializeCallsign()
    {
        if (gNetworkCallsignInitialized == 0)
        {
            gNetworkCallsign[0] = '\0';
            gNetworkCallsignInitialized = 1;
        }

        if (std::strlen(gNetworkCallsign) != 0)
        {
            return;
        }

        auto* properties = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;

        if (properties->Property_DoesExists("multiplayer_callsign"))
        {
            // Reload the singleton as in the original.
            properties = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;

            std::strncpy(gNetworkCallsign, properties->Property_GetStringValuePtr("multiplayer_callsign"), 0x27);
        }
        else
        {
            // Preserve the original unchecked API result and uninitialized buffer.
            char userName[100];
            DWORD userNameSize = 100;

            ::GetUserNameA(userName, &userNameSize);
            std::strncpy(gNetworkCallsign, userName, 0x27);
        }

        gNetworkCallsign[0x27] = '\0';
    }

    // Original function: FUN_00405a87
    std::int32_t Network_GetCallsignBufferSize()
    {
        return 0x28;
    }

    // Original function: FUN_00405a8b
    bool Network_GetPlayerName(DPNID playerId, char* destination)
    {
        wchar_t* playerName = nullptr;

        if (!GetNetworkInstance()->GetPlayerName(playerId, &playerName))
        {
            return false;
        }

        ConvertNetworkWideToAnsi(destination, playerName, -1);
        NXBasics::Runtime_Free(playerName);

        return true;
    }

    // Original function: FUN_004dbd68
    HRESULT WINAPI CDPNet::MessageProc(void* context, DWORD messageId, void* message)
    {
        switch (messageId)
        {
            case DPN_MSGID_INDICATE_CONNECT:
                {
                    WriteNetworkLogLevel(3, "Remote peer attempts to connect", 0);
                    OutputDebugStringA("# DPN_MSGID_INDICATE_CONNECT - Remote peer attempts to connect\n");

                    return GetNetworkInstance()->allowConnections != 0 ? S_FALSE : S_OK;
                }

            case DPN_MSGID_CREATE_PLAYER:
                {
                    WriteNetworkLogLevel(3, "A new player joined the session", 0);
                    OutputDebugStringA("# DPN_MSGID_CREATE_PLAYER - New player joined the session\n");

                    GetNetworkInstance()->playerInfoChanged = 1;
                    ++GetNetworkInstance()->playerChangeCount;

                    const auto* created = static_cast<const DPNMSG_CREATE_PLAYER*>(message);

                    // Pending player-event implementation.
                    GetNetworkInstance()->AddPlayerEvent(created->dpnidPlayer, 2u);

                    return S_OK;
                }

            case DPN_MSGID_DESTROY_PLAYER:
                {
                    OutputDebugStringA("# DPN_MSGID_DESTROY_PLAYER - A player has left the session\n");

                    GetNetworkInstance()->playerInfoChanged = 1;
                    ++GetNetworkInstance()->playerChangeCount;

                    const auto* destroyed = static_cast<const DPNMSG_DESTROY_PLAYER*>(message);
                    std::uint32_t eventType;

                    switch (destroyed->dwReason)
                    {
                        case DPNDESTROYPLAYERREASON_NORMAL:
                            WriteNetworkLogLevel(3, "A player has left the session normally", 0);
                            eventType = 3;
                            break;

                        case DPNDESTROYPLAYERREASON_CONNECTIONLOST:
                            WriteNetworkLogLevel(3, "A player has lost the connection", 0);
                            eventType = 5;
                            break;

                        case DPNDESTROYPLAYERREASON_SESSIONTERMINATED:
                            WriteNetworkLogLevel(3, "The session was terminated", 0);
                            eventType = 6;
                            break;

                        case DPNDESTROYPLAYERREASON_HOSTDESTROYEDPLAYER:
                            WriteNetworkLogLevel(3, "A player was kicked by the host", 0);
                            eventType = 4;
                            break;

                        default:
                            WriteNetworkLogLevel(3, "A player lost the connection", 0);
                            eventType = 7;
                            break;
                    }

                    GetNetworkInstance()->AddPlayerEvent(destroyed->dpnidPlayer, eventType);

                    return S_OK;
                }

            case DPN_MSGID_ASYNC_OP_COMPLETE:
                {
                    WriteNetworkLogLevel(3, "Cancel search for sessions complete", 0);
                    OutputDebugStringA("# DPN_MSGID_ASYNC_OP_COMPLETE - Cancel search for sessions completely\n");

                    const auto* completed = static_cast<const DPNMSG_ASYNC_OP_COMPLETE*>(message);

                    if (completed->hAsyncOp != GetNetworkInstance()->searchOperation)
                    {
                        return S_OK;
                    }

                    EnterCriticalSection(&GetNetworkInstance()->criticalD4);

                    if (GetNetworkInstance()->sessions != nullptr)
                    {
                        SSessionList* sessionList = GetNetworkInstance()->sessions;

                        if (sessionList != nullptr)
                        {
                            DestroySessionListContents(sessionList);
                            NXBasics::Runtime_Free(sessionList);
                        }

                        GetNetworkInstance()->sessions = nullptr;
                    }

                    LeaveCriticalSection(&GetNetworkInstance()->criticalD4);
                    DeleteCriticalSection(&GetNetworkInstance()->criticalD4);

                    GetNetworkInstance()->searchOperation = 0;
                    GetNetworkInstance()->flags &= ~0x04u;

                    return S_OK;
                }

            case DPN_MSGID_ENUM_HOSTS_RESPONSE:
                {
                    WriteNetworkLogLevel(3, "Remote host found", 0);
                    OutputDebugStringA("# DPN_MSGID_ENUM_HOSTS_RESPONSE - Remote host found\n");

                    EnterCriticalSection(&GetNetworkInstance()->criticalD4);

                    if (GetNetworkInstance()->sessions != nullptr)
                    {
                        UpdateSessionList(GetNetworkInstance()->sessions, static_cast<const DPNMSG_ENUM_HOSTS_RESPONSE*>(message));
                    }

                    LeaveCriticalSection(&GetNetworkInstance()->criticalD4);

                    return S_OK;
                }

            case DPN_MSGID_ENUM_HOSTS_QUERY:
                {
                    WriteNetworkLogLevel(3, "Remote peer is searching for sessions on the local system", 0);
                    OutputDebugStringA("# DPN_MSGID_ENUM_HOSTS_QUERY - Remote peer is searching for sessions on the local system\n");

                    if (context != nullptr)
                    {
                        std::uint32_t contextValue;
                        std::memcpy(&contextValue, context, sizeof(contextValue));

                        if (contextValue == 0)
                        {
                            return S_OK;
                        }
                    }

                    return GetNetworkInstance()->allowConnections != 0 ? S_FALSE : S_OK;
                }

            case DPN_MSGID_RECEIVE:
                {
                    const auto* received = static_cast<const DPNMSG_RECEIVE*>(message);
                    const std::uint8_t* packet = received->pReceiveData;

                    switch (packet[0])
                    {
                        case 2:
                            GetNetworkInstance()->allowConnections = packet[1];
                            return S_OK;

                        case 6:
                            {
                                if ((GetNetworkInstance()->flags & 0x08u) == 0)
                                {
                                    return S_OK;
                                }

                                EnterCriticalSection(&GetNetworkInstance()->critical74);

                                if (GetNetworkInstance()->data48[1] != std::byte{ 0 })
                                {
                                    GetNetworkInstance()->data48[1] = std::byte{ 0 };
                                    std::memcpy(GetNetworkInstance()->data48 + 8, packet + 4, sizeof(std::uint32_t));
                                }

                                LeaveCriticalSection(&GetNetworkInstance()->critical74);

                                return S_OK;
                            }

                        case 7:
                            return S_OK;

                        case 11:
                        case 12:
                        case 13:
                        case 14:
                        case 15:
                            {
                                CFileTransferManager* fileTransferManager = CFileTransferManager::GetInstance();

                                if (fileTransferManager != nullptr)
                                {
                                    fileTransferManager->DispatchMessage(received->pReceiveData);
                                }
                                else
                                {
                                    OutputDebugStringA(DAT_00509910);
                                }

                                return S_OK;
                            }

                        default:
                            GetNetworkInstance()->SaveReceivedMessage(received);
                            return DPNSUCCESS_PENDING;
                    }
                }

            case DPN_MSGID_SEND_COMPLETE:
                {
                    if ((GetNetworkInstance()->flags & 0x40u) == 0)
                    {
                        return S_OK;
                    }

                    std::uint32_t statistics[4] = {};
                    GetNetworkInstance()->GetSendStatistics(statistics);

                    const auto* completed = static_cast<const DPNMSG_SEND_COMPLETE*>(message);

                    if (completed->dwSendTime < statistics[0])
                    {
                        statistics[0] = completed->dwSendTime;
                    }

                    if (completed->dwSendTime > statistics[2])
                    {
                        statistics[2] = completed->dwSendTime;
                    }

                    statistics[1] = 0;

                    GetNetworkInstance()->SetSendStatistics(statistics);
                    GetNetworkInstance()->flags &= ~0x40u;

                    return S_OK;
                }

            case DPN_MSGID_HOST_MIGRATE:
                {
                    WriteNetworkLogLevel(3, "Host migration detected", 0);
                    OutputDebugStringA("# DPN_MSGID_HOST_MIGRATE - Host migration detected\n");

                    const auto* migrated = static_cast<const DPNMSG_HOST_MIGRATE*>(message);

                    GetNetworkInstance()->AddPlayerEvent(migrated->dpnidNewHost, 8u);

                    if (GetNetworkInstance()->GetLocalPlayerId() != 0)
                    {
                        const DPNID hostPlayer = GetNetworkInstance()->GetHostPlayerId();

                        if (GetNetworkInstance()->GetLocalPlayerId() == hostPlayer)
                        {
                            WriteNetworkLogLevel(3, "Local peer is new host", 0);

                            GetNetworkInstance()->flags |= 0x02u;
                            GetNetworkInstance()->flags &= ~0x08u;
                        }
                    }

                    return S_OK;
                }

            case DPN_MSGID_CONNECT_COMPLETE:
                WriteNetworkLogLevel(3, "Connect complete", 0);
                OutputDebugStringA("# DPN_MSGID_CONNECT_COMPLETE - Connect complete");
                return S_OK;

            case DPN_MSGID_TERMINATE_SESSION:
                WriteNetworkLogLevel(3, "Terminate session", 0);
                return S_OK;

            default:
                {
                    WriteNetworkLogLevel(3, "Unknown DirectPlay message ignored", 0);

                    // The original stack buffer is too small for this diagnostic.
                    char diagnostic[64] = {};
                    NXBasics::Runtime_PrintString(diagnostic, "DirectPlay message type is 0x%x", static_cast<unsigned int>(messageId));

                    WriteNetworkLogLevel(3, diagnostic, 0);

                    OutputDebugStringA("CDPNet::MessageProc(): Unknown network message type ignored.");
                    OutputDebugStringA(OriginalStrings::DirectPlayErrorSeparator);
                    OutputDebugStringA(NXBasics::GetDirectXErrorText(0));
                    OutputDebugStringA(OriginalStrings::NewLine);

                    return S_FALSE;
                }
        }
    }

    // Original function: FUN_004dc391
    CDPNet::~CDPNet()
    {
        if (gNetworkLog != nullptr)
        {
            DestroyLogSingleton();
        }

        STimerPrefix* timer = searchTimer;

        if (timer != nullptr)
        {
            ShutDownTimer(timer);
            NXBasics::Runtime_Free(timer);

            searchTimer = nullptr;
        }

        SSessionList* sessionList = sessions;

        if (sessionList != nullptr)
        {
            DestroySessionListContents(sessionList);
            NXBasics::Runtime_Free(sessionList);

            sessions = nullptr;
        }

        CloseNetworkLog(&log);

        ClearEventQueue(&events28);
        ClearEventQueue(&events18);
    }

    // Original function: FUN_004dc438
    void DestroyNetworkSingleton()
    {
        CDPNet* object = gNetworkInstance;

        if (object != nullptr)
        {
            object->~CDPNet();
            ::operator delete(object);

            gNetworkInstance = nullptr;
        }
    }

    // Original function: FUN_004dc7a3
    bool CDPNet::Uninitialize()
    {
        if (peer == nullptr)
        {
            return false;
        }

        const HRESULT result = peer->Close(0);

        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::Uninitialize(): IDirectPlay8Peer::Close", result);
        }

        EnterCriticalSection(&criticalEC);
        ReleaseReceivedMessages(receivedMessages, receivedMessageCount);
        LeaveCriticalSection(&criticalEC);
        DeleteCriticalSection(&criticalEC);

        EnterCriticalSection(&critical74);
        std::memset(data48, 0, sizeof(data48));
        LeaveCriticalSection(&critical74);
        DeleteCriticalSection(&critical74);

        EnterCriticalSection(&critical8C);
        std::memset(data58, 0, sizeof(data58));
        LeaveCriticalSection(&critical8C);
        DeleteCriticalSection(&critical8C);

        EnterCriticalSection(&criticalA4);
        ClearEventQueue(&events18);
        LeaveCriticalSection(&criticalA4);
        DeleteCriticalSection(&criticalA4);

        EnterCriticalSection(&criticalBC);
        ClearEventQueue(&events28);
        LeaveCriticalSection(&criticalBC);
        DeleteCriticalSection(&criticalBC);

        if (interface6C != nullptr)
        {
            interface6C->Release();
            interface6C = nullptr;
        }

        if (interface70 != nullptr)
        {
            interface70->Release();
            interface70 = nullptr;
        }

        if (peer != nullptr)
        {
            peer->Release();
            peer = nullptr;
        }

        CoUninitialize();

        flags &= 0xFFFFFFFEu;
        return true;
    }

    // Original function: FUN_004dcc9b
    bool CDPNet::LeaveSession()
    {
        if (peer == nullptr)
        {
            return false;
        }

        const HRESULT result = peer->Close(0);

        if (FAILED(result))
        {
            ReportDirectPlayError("CDPNet::LeaveSession(): IDirectPlay8Peer::Close", result);
            return false;
        }

        flags &= 0xFFFFFFF4u;

        return InitializePeer(callbackContext, false);
    }

    // Original function: FUN_004dcedd
    bool CDPNet::StopSearchingSessions()
    {
        STimerPrefix* timer = searchTimer;

        if (timer != nullptr)
        {
            ShutDownTimer(timer);
            NXBasics::Runtime_Free(timer);

            searchTimer = nullptr;
        }

        if (searchOperation != 0)
        {
            const HRESULT result = peer->CancelAsyncOperation(searchOperation, 0);

            if (FAILED(result))
            {
                ReportDirectPlayError("CDPNet::StopSearchingSessions(): IDirectPlay8Peer::CancelAsyncOperation", result);
            }
            else
            {
                searchOperation = 0;
            }
        }

        return true;
    }

    // Original function: FUN_004dd888
    void CDPNet::SaveReceivedMessage(const DPNMSG_RECEIVE* message)
    {
        EnterCriticalSection(&criticalEC);

        if (receivedMessageCount == 0x40u)
        {
            const HRESULT result = peer->ReturnBuffer(receivedMessages[0].bufferHandle, 0);

            if (FAILED(result))
            {
                ReportDirectPlayError("CDPNet::SaveReceivedMessage(): IDirectPlay8Peer::ReturnBuffer", result);
            }

            std::memmove(receivedMessages, receivedMessages + 1, 0x3F0);
            --receivedMessageCount;
        }

        receivedMessages[receivedMessageCount].sender = message->dpnidSender;
        receivedMessages[receivedMessageCount].size = message->dwReceiveDataSize;
        receivedMessages[receivedMessageCount].data = message->pReceiveData;
        receivedMessages[receivedMessageCount].bufferHandle = message->hBufferHandle;
        ++receivedMessageCount;

        LeaveCriticalSection(&criticalEC);
    }

    // Original function: FUN_004dd9fe
    void CDPNet::ReleaseReceivedMessages(SReceivedMessage* messages, std::uint32_t count)
    {
        for (std::uint32_t index = 0; index < count; ++index)
        {
            const HRESULT result = peer->ReturnBuffer(messages[index].bufferHandle, 0);

            if (FAILED(result))
            {
                ReportDirectPlayError(OriginalStrings::NetworkReturnBufferOperation, result);
            }
        }

        if (messages != nullptr)
        {
            NXBasics::Runtime_Free(messages);
        }
    }

    // Original function: FUN_004dda6f
    void CDPNet::GetSendStatistics(std::uint32_t* statistics)
    {
        EnterCriticalSection(&critical8C);

        if (statistics != nullptr)
        {
            std::memcpy(statistics, data58, 0x10);
        }

        LeaveCriticalSection(&critical8C);
    }

    // Original function: FUN_004ddaa5
    void CDPNet::SetSendStatistics(const std::uint32_t* statistics)
    {
        EnterCriticalSection(&critical8C);

        if (statistics != nullptr)
        {
            std::memcpy(data58, statistics, 0x10);
        }

        LeaveCriticalSection(&critical8C);
    }

    // Original function: FUN_004de956
    SDynamicNetworkData* ConstructDynamicNetworkData(SDynamicNetworkData* data)
    {
        // Original function: FUN_004db429
        data->events.count = 0;
        data->events.last = nullptr;
        data->events.first = nullptr;
        data->events.changed = 0;

        for (std::uint32_t index = 0; index < 3; ++index)
        {
            // Original function: FUN_004e0ffd
            ++gDynamicSlotArrayConstructionCount;

            SDynamicSlotArray& array = data->arrays[index];

            array.revision = 0;
            array.unknown00 = 0;
            array.networkTick = 0;
            array.count = 0;
            array.slots = nullptr;
        }

        data->active = 0;
        data->networkTick = 0;
        data->allocation10 = nullptr;
        data->allocation28 = nullptr;
        data->receivedRecordCount = 0;
        data->pendingDataMessageCount = 0;
        data->allocation30 = nullptr;
        data->waitStartTick = 0;
        data->unknown7C = 0;

        // Original allocation: FUN_004e5ddd -> FUN_004e860e(size, 1).
        data->allocation30 = NXBasics::Runtime_AllocateWithNewHandler(0x10300u, 1);

        // Preserve the original unconditional initialization after allocation.
        std::memset(data->allocation30, 0, 0x10300u);

        return data;
    }

    // Original function: FUN_004de9b1
    void DestroyDynamicNetworkData(SDynamicNetworkData* data)
    {
        ReleaseDynamicNetworkMemory(data);

        // Original destructors: FUN_004e1016 -> FUN_004e10fb.
        for (std::uint32_t index = 3; index != 0; --index)
        {
            ReleaseDynamicSlots(&data->arrays[index - 1]);
        }

        // Original destructor: FUN_004db49b -> FUN_004db528.
        ClearEventQueue(&data->events);
    }
}
