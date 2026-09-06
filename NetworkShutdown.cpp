#include "NetworkShutdown.h"

#include <cstring>

// Pending dependencies.
// These declarations describe reconstruction interfaces, not absolute-address bindings.

extern void FUN_004e5e48(void* allocation);
extern char* FUN_004e3252(std::int32_t result);

extern NNetwork::CDPNet* FUN_004dc409();
extern NNetwork::SNetLog* FUN_004db855();
extern void* FUN_004dbc6d(NNetwork::SNetLog* object, const char* text, std::int32_t indentation);
extern void FUN_004d77da(NNetwork::SSessionNode* object);

extern int FUN_004e8cc7(void* stream);
extern int FUN_004e8c4a(void* stream);
extern int FUN_004e5d8b(char* destination, const char* format, ...);
extern char* FUN_004e907b(std::uint32_t value, char* destination, int radix);

// Original globals. Storage remains with the corresponding owning module.
extern NNetwork::CDPNet* DAT_00569658;
extern NNetwork::SNetLog* DAT_00569654;
extern NNetwork::SDynamicNetworkData DAT_0050f6a8;

// Original strings whose complete contents have not been supplied.
extern const char lpOutputString_0050071c[];
extern const char lpOutputString_00509630[];
extern const char DAT_0050a698[];
extern const char DAT_0050a9ec[];
extern const char DAT_0050a9dc[];
extern const char DAT_0050a9d8[];
extern const char DAT_0050a9c8[];

namespace NNetwork
{
	namespace
	{
		// Shared expansion of the original four OutputDebugStringA calls.
		void ReportDirectPlayError(const char* operation, HRESULT result)
		{
			OutputDebugStringA(operation);
			OutputDebugStringA(lpOutputString_0050071c);
			OutputDebugStringA(FUN_004e3252(result));
			OutputDebugStringA(lpOutputString_00509630);
		}

		// Original function: FUN_004db528
		SEventQueue* ClearEventQueue(SEventQueue* queue)
		{
			while (queue->first != nullptr)
			{
				SEventNode* node = queue->first;
				queue->first = node->next;

				FUN_004e5e48(node);
			}

			queue->last = nullptr;
			queue->first = nullptr;
			queue->count = 0;
			queue->changed = 0;

			return queue;
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
				FUN_004e5e48(array->slots);
				array->slots = nullptr;
			}
		}

		// Original function: FUN_004de9da
		void ReleaseDynamicNetworkMemory(SDynamicNetworkData* data)
		{
			FUN_004dbc6d(FUN_004db855(), "Deleting dynamic memory", 0);

			ClearEventQueue(&data->events);

			if (data->allocation10 != nullptr)
			{
				FUN_004e5e48(data->allocation10);
				data->allocation10 = nullptr;
			}

			if (data->allocation28 != nullptr)
			{
				FUN_004e5e48(data->allocation28);
				data->allocation28 = nullptr;
			}

			if (data->allocation30 != nullptr)
			{
				FUN_004e5e48(data->allocation30);
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

				OutputDebugStringA(DAT_0050a9ec);
				OutputDebugStringA(DAT_0050a9dc);

				FUN_004e5d8b(buffer, DAT_0050a9d8, timer->thread);
				OutputDebugStringA(buffer);

				OutputDebugStringA(DAT_0050a9c8);

				FUN_004e907b(0xFFFFFFFFu, buffer, 10);
				OutputDebugStringA(buffer);

				OutputDebugStringA(lpOutputString_00509630);
			}

			CloseHandle(timer->thread);
			timer->thread = nullptr;
		}

		// Original function: FUN_004db8c8
		void CloseNetworkLog(SNetLog* log)
		{
			if (log->stream != nullptr)
			{
				FUN_004e8cc7(log->stream);
				FUN_004e8c4a(log->stream);
			}
		}

		// Original function: FUN_004db87e
		void DestroyLogSingleton()
		{
			SNetLog* log = DAT_00569654;

			if (log != nullptr)
			{
				CloseNetworkLog(log);
				FUN_004e5e48(log);

				DAT_00569654 = nullptr;
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
				FUN_004d77da(node);
				FUN_004e5e48(node);
				return;
			}

			bool finished;

			do
			{
				node = list->first;
				list->first = node->next;

				// The original stops after destroying the captured last node.
				finished = node == last;

				FUN_004d77da(node);
				FUN_004e5e48(node);
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
			FUN_004e5e48(timer);

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

		return FUN_004dc700(callbackContext, false);
	}

	// Original function: FUN_004dd9fe
	void CDPNet::ReleaseReceivedMessages(SReceivedMessage* messages, std::uint32_t count)
	{
		for (std::uint32_t index = 0; index < count; ++index)
		{
			const HRESULT result = peer->ReturnBuffer(messages[index].bufferHandle, 0);

			if (FAILED(result))
			{
				ReportDirectPlayError(DAT_0050a698, result);
			}
		}

		if (messages != nullptr)
		{
			FUN_004e5e48(messages);
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
		if (DAT_00569654 != nullptr)
		{
			DestroyLogSingleton();
		}

		STimerPrefix* timer = searchTimer;

		if (timer != nullptr)
		{
			ShutDownTimer(timer);
			FUN_004e5e48(timer);

			searchTimer = nullptr;
		}

		SSessionList* sessionList = sessions;

		if (sessionList != nullptr)
		{
			DestroySessionListContents(sessionList);
			FUN_004e5e48(sessionList);

			sessions = nullptr;
		}

		CloseNetworkLog(&log);

		ClearEventQueue(&events28);
		ClearEventQueue(&events18);
	}

	// Original function: FUN_004dc438
	void DestroyNetworkSingleton()
	{
		CDPNet* object = DAT_00569658;

		if (object != nullptr)
		{
			object->~CDPNet();
			FUN_004e5e48(object);

			DAT_00569658 = nullptr;
		}
	}

	// Original function: FUN_00405949
	void Network_ShutDown()
	{
		ShutDownDynamicNetworkData(&DAT_0050f6a8);

		CDPNet* network = FUN_004dc409();

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