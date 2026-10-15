#include "DirectXErrorText.h"
#include "FileTransferManager.h"
#include "MasterPropertyManager.h"
#include "NetworkShutdown.h"
#include "OriginalStrings.h"
#include "SwitchIdentifiers.h"
#include "XCrtFileAccess.h"
#include "XNumericConstants.h"
#include <new>

#include "dplay8.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <Windows.h>

// Original data: DAT_0050f6a8.
NNetwork::SDynamicNetworkData gDynamicNetworkData{ };

// Original data: 0050f728..0050f74f.
char gNetworkCallsign[40]{ };

// Original data: 0050f750.
std::uint8_t gNetworkCallsignInitialized = 0;

namespace NNetwork
{
	// Original functions: FUN_004db528 and its tail-jump alias FUN_004db49b.
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

	// Original function: FUN_004e101b; shared by tick rotation and destruction.
	void ResetDynamicSlots(SDynamicSlotArray* array)
	{
		if (array->slots == nullptr)
		{
			return;
		}

		for (std::uint32_t index = 0; index < array->count; ++index)
		{
			array->slots[index].payloadByteCount = 0;
			std::memset(array->slots[index].data, 0, 200);
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

		if ((network->flags & NXBasics::BitMask::Bit2) != 0)
		{
			network->StopSearchingSessions();
		}

		if ((network->flags & NXBasics::BitMask::Bit3) != 0)
		{
			network->LeaveSession();
		}

		// Read the flags again after LeaveSession().
		if ((network->flags & NXBasics::BitMask::Bit1) != 0)
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
		return (GetNetworkInstance()->flags & NXBasics::BitMask::Bit1) != 0;
	}

	// Original function: FUN_004059b3
	bool Network_IsInSession()
	{
		if (GetExistingNetworkInstance() == nullptr)
		{
			return false;
		}

		return (GetNetworkInstance()->flags & NXBasics::BitMask::Bit1AndBit3) != 0;
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

					EnterCriticalSection(&GetNetworkInstance()->sessionSearchCriticalSection);

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

					LeaveCriticalSection(&GetNetworkInstance()->sessionSearchCriticalSection);
					DeleteCriticalSection(&GetNetworkInstance()->sessionSearchCriticalSection);

					GetNetworkInstance()->searchOperation = 0;
					GetNetworkInstance()->flags &= ~0x04u;

					return S_OK;
				}

			case DPN_MSGID_ENUM_HOSTS_RESPONSE:
				{
					WriteNetworkLogLevel(3, "Remote host found", 0);
					OutputDebugStringA("# DPN_MSGID_ENUM_HOSTS_RESPONSE - Remote host found\n");

					EnterCriticalSection(&GetNetworkInstance()->sessionSearchCriticalSection);

					if (GetNetworkInstance()->sessions != nullptr)
					{
						UpdateSessionList(GetNetworkInstance()->sessions, static_cast<const DPNMSG_ENUM_HOSTS_RESPONSE*>(message));
					}

					LeaveCriticalSection(&GetNetworkInstance()->sessionSearchCriticalSection);

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
						case SwitchIds::NetworkPacket::AllowConnections:
							GetNetworkInstance()->allowConnections = packet[1];
							return S_OK;

						case SwitchIds::NetworkPacket::PlayerInfo:
							{
								if ((GetNetworkInstance()->flags & NXBasics::BitMask::Bit3) == 0)
								{
									return S_OK;
								}

								EnterCriticalSection(&GetNetworkInstance()->controlPacketCriticalSection);

								if (GetNetworkInstance()->controlPacketReceiveState.awaitingPacket != 0)
								{
									GetNetworkInstance()->controlPacketReceiveState.awaitingPacket = 0;
									std::memcpy(&GetNetworkInstance()->controlPacketReceiveState.receivedValue, packet + 4, sizeof(std::uint32_t));
								}

								LeaveCriticalSection(&GetNetworkInstance()->controlPacketCriticalSection);

								return S_OK;
							}

						case SwitchIds::NetworkPacket::ConnectionProbe:
							return S_OK;

						case SwitchIds::NetworkPacket::FileTransferAnnouncement:
						case SwitchIds::NetworkPacket::FileTransferAccept:
						case SwitchIds::NetworkPacket::FileTransferRequestHeader:
						case SwitchIds::NetworkPacket::FileTransferHeader:
						case SwitchIds::NetworkPacket::FileTransferData:
							{
								CFileTransferManager* fileTransferManager = CFileTransferManager::GetInstance();

								if (fileTransferManager != nullptr)
								{
									fileTransferManager->DispatchMessage(received->pReceiveData);
								}
								else
								{
									OutputDebugStringA(OriginalStrings::FileTransferServiceUnavailable);
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
					if ((GetNetworkInstance()->flags & NXBasics::BitMask::Bit6) == 0)
					{
						return S_OK;
					}

					std::uint32_t statistics[4] = { };
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

							GetNetworkInstance()->flags |= NXBasics::BitMask::Bit1;
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
					char diagnostic[64] = { };
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
		ClearEventQueue(&playerEvents);
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

		EnterCriticalSection(&receivedMessagesCriticalSection);
		ReleaseReceivedMessages(receivedMessages, receivedMessageCount);
		LeaveCriticalSection(&receivedMessagesCriticalSection);
		DeleteCriticalSection(&receivedMessagesCriticalSection);

		EnterCriticalSection(&controlPacketCriticalSection);
		std::memset(&controlPacketReceiveState, 0, sizeof(controlPacketReceiveState));
		LeaveCriticalSection(&controlPacketCriticalSection);
		DeleteCriticalSection(&controlPacketCriticalSection);

		EnterCriticalSection(&sendStatisticsCriticalSection);
		std::memset(sendStatisticsStorage, 0, sizeof(sendStatisticsStorage));
		LeaveCriticalSection(&sendStatisticsCriticalSection);
		DeleteCriticalSection(&sendStatisticsCriticalSection);

		EnterCriticalSection(&playerEventsCriticalSection);
		ClearEventQueue(&playerEvents);
		LeaveCriticalSection(&playerEventsCriticalSection);
		DeleteCriticalSection(&playerEventsCriticalSection);

		EnterCriticalSection(&criticalBC);
		ClearEventQueue(&events28);
		LeaveCriticalSection(&criticalBC);
		DeleteCriticalSection(&criticalBC);

		if (hostAddress != nullptr)
		{
			hostAddress->Release();
			hostAddress = nullptr;
		}

		if (deviceAddress != nullptr)
		{
			deviceAddress->Release();
			deviceAddress = nullptr;
		}

		if (peer != nullptr)
		{
			peer->Release();
			peer = nullptr;
		}

		CoUninitialize();

		flags &= NXBasics::BitMask::DwordExceptBit0;
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

		flags &= NXBasics::BitMask::Bit2AndBits4Through31;

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
		EnterCriticalSection(&receivedMessagesCriticalSection);

		if (receivedMessageCount == 0x40u)
		{
			const HRESULT result = peer->ReturnBuffer(receivedMessages[0].bufferHandle, 0);

			if (FAILED(result))
			{
				ReportDirectPlayError("CDPNet::SaveReceivedMessage(): IDirectPlay8Peer::ReturnBuffer", result);
			}

			std::memmove(receivedMessages, receivedMessages + 1, 1008);
			--receivedMessageCount;
		}

		receivedMessages[receivedMessageCount].sender = message->dpnidSender;
		receivedMessages[receivedMessageCount].size = message->dwReceiveDataSize;
		receivedMessages[receivedMessageCount].data = message->pReceiveData;
		receivedMessages[receivedMessageCount].bufferHandle = message->hBufferHandle;
		++receivedMessageCount;

		LeaveCriticalSection(&receivedMessagesCriticalSection);
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
		EnterCriticalSection(&sendStatisticsCriticalSection);

		if (statistics != nullptr)
		{
			std::memcpy(statistics, sendStatisticsStorage, 16);
		}

		LeaveCriticalSection(&sendStatisticsCriticalSection);
	}

	// Original function: FUN_004ddaa5
	void CDPNet::SetSendStatistics(const std::uint32_t* statistics)
	{
		EnterCriticalSection(&sendStatisticsCriticalSection);

		if (statistics != nullptr)
		{
			std::memcpy(sendStatisticsStorage, statistics, 16);
		}

		LeaveCriticalSection(&sendStatisticsCriticalSection);
	}

	// Original function: FUN_004E0FFD.
	SDynamicSlotArray* ConstructDynamicSlotArray(SDynamicSlotArray* array)
	{
		++gDynamicSlotArrayConstructionCount;
		array->revision = 0;
		array->slotAllocationCount = 0;
		array->networkTick = 0;
		array->count = 0;
		array->slots = nullptr;
		return array;
	}

	// Original function: FUN_004E1016; distinct tail-jump destructor entry.
	void DestroyDynamicSlotArray(SDynamicSlotArray* array)
	{
		ReleaseDynamicSlots(array);
	}

	// Original function: FUN_004de956.
	SDynamicNetworkData* ConstructDynamicNetworkData(SDynamicNetworkData* data)
	{
		// Original function: FUN_004db429
		data->events.count = 0;
		data->events.last = nullptr;
		data->events.first = nullptr;
		data->events.changed = 0;

		for (std::uint32_t index = 0; index < 3; ++index)
		{
			ConstructDynamicSlotArray(&data->commandBuffers[index]);
		}

		data->active = 0;
		data->networkTick = 0;
		data->playerInfoRecords = nullptr;
		data->receivedChatRecords = nullptr;
		data->receivedChatRecordCount = 0;
		data->pendingDataMessageCount = 0;
		data->pendingDataMessages = nullptr;
		data->waitStartTick = 0;
		data->unknown7C = 0;

		// Original allocation: FUN_004e5ddd -> FUN_004e860e(size, 1).
		data->pendingDataMessages = NXBasics::Runtime_AllocateWithNewHandler(66304u, 1);

		// Preserve the original unconditional initialization after allocation.
		std::memset(data->pendingDataMessages, 0, 66304u);

		return data;
	}

	// Original function: FUN_004de9b1
	void DestroyDynamicNetworkData(SDynamicNetworkData* data)
	{
		ReleaseDynamicNetworkMemory(data);

		// Original destructors: FUN_004e1016 -> FUN_004e10fb.
		for (std::uint32_t index = 3; index != 0; --index)
		{
			DestroyDynamicSlotArray(&data->commandBuffers[index - 1]);
		}

		// Original destructor: FUN_004db49b -> FUN_004db528.
		ClearEventQueue(&data->events);
	}
}

namespace NNetwork
{
	// Original function: FUN_004de9da
	void ReleaseDynamicNetworkMemory(SDynamicNetworkData* data)
	{
		WriteNetworkLog(GetNetworkLog(), "Deleting dynamic memory", 0);

		ClearEventQueue(&data->events);

		if (data->playerInfoRecords != nullptr)
		{
			NXBasics::Runtime_Free(data->playerInfoRecords);
			data->playerInfoRecords = nullptr;
		}

		if (data->receivedChatRecords != nullptr)
		{
			NXBasics::Runtime_Free(data->receivedChatRecords);
			data->receivedChatRecords = nullptr;
		}

		if (data->pendingDataMessages != nullptr)
		{
			NXBasics::Runtime_Free(data->pendingDataMessages);
			data->pendingDataMessages = nullptr;
		}

		ReleaseDynamicSlots(&data->commandBuffers[0]);
		ReleaseDynamicSlots(&data->commandBuffers[1]);
		ReleaseDynamicSlots(&data->commandBuffers[2]);
	}

}

namespace NNetwork
{
	// Original function: FUN_004debfd
	void ShutDownDynamicNetworkData(SDynamicNetworkData* data)
	{
		ReleaseDynamicNetworkMemory(data);
		data->active = 0;
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

}

namespace NNetwork
{
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
			char buffer[20] = { };

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

}
