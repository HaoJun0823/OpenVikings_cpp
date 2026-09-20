#include "OriginalStrings.h"
#include <new>
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"

#include <cstring>

// Pending dependencies.
// These declarations describe reconstruction interfaces, not absolute-address bindings.

extern char* FUN_004e3252(std::int32_t result);

// Original data: DAT_0050f6a8.
NNetwork::SDynamicNetworkData DAT_0050f6a8{};


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
		// Shared expansion of the original four OutputDebugStringA calls.
		void ReportDirectPlayError(const char* operation, HRESULT result)
		{
			OutputDebugStringA(operation);
			OutputDebugStringA(OriginalStrings::DirectPlayErrorSeparator);
			OutputDebugStringA(FUN_004e3252(result));
			OutputDebugStringA(OriginalStrings::NewLine);
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

	// Original function: FUN_00405949
	void Network_ShutDown()
	{
		ShutDownDynamicNetworkData(&DAT_0050f6a8);

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
}

namespace NNetwork
{
	// Original function: FUN_004058dd
	void InitializeGlobalDynamicNetworkData()
	{
		ConstructDynamicNetworkData(&DAT_0050f6a8);

		// Original function: FUN_004058f3
		// The existing reconstructed onexit interface expects an int return value.
		NXBasics::Runtime_AtExit(+[] () -> int
		{
			DestroyDynamicNetworkData(&DAT_0050f6a8);
			return 0;
		});
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
			array.unknown0C = 0;
			array.count = 0;
			array.slots = nullptr;
		}

		data->active = 0;
		data->unknown04 = 0;
		data->allocation10 = nullptr;
		data->allocation28 = nullptr;
		data->receivedRecordCount = 0;
		data->unknown2C = 0;
		data->allocation30 = nullptr;
		data->unknown08 = 0;
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