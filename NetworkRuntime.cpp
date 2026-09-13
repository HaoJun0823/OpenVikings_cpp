#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"

#include <cstring>
#include <new>

// Pending dependencies.
extern char* FUN_004e3252(std::int32_t result);

extern int FUN_004e8f88(void* stream, const char* format, ...);
extern char* FUN_004e8f0f(char* destination);
extern int FUN_004e8cc7(void* stream);
extern void FUN_004de237(const char* sourceFile, std::int32_t line, std::int32_t value);

// DirectPlay callback. Its implementation remains outside this block.
extern HRESULT WINAPI FUN_004dbd68(void* context, DWORD messageId, void* message);

// The initial value of this global has not been established here.
extern std::uint8_t DAT_00569650;

// Original data: DAT_00569658
NNetwork::CDPNet* gNetworkInstance = nullptr;

// Original data: DAT_00569654
NNetwork::SNetLog* gNetworkLog = nullptr;


namespace
{
	// Original function: FUN_004db429
	void InitializeEventQueue(NNetwork::SEventQueue* queue)
	{
		queue->count = 0;
		queue->last = nullptr;
		queue->first = nullptr;
		queue->changed = 0;
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

	void ReportDirectPlayError(const char* operation, HRESULT result)
	{
		OutputDebugStringA(operation);
		OutputDebugStringA(OriginalStrings::DirectPlayErrorSeparator);
		OutputDebugStringA(FUN_004e3252(result));
		OutputDebugStringA(OriginalStrings::NewLine);
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

namespace NNetwork
{
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

// Original function: FUN_004dbc6d
void* WriteNetworkLog(NNetwork::SNetLog* object, const char* text, std::int32_t indentation)
{
	if (DAT_00569650 == 0)
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
		const int result = FUN_004e8f88(object->stream, "\n%010lu: ", static_cast<unsigned long>(object->sequenceNumber));

		if (result < 0)
		{
			FUN_004de237("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x102, 0);
			object->error = 1;
			return object;
		}

		++object->sequenceNumber;
	}

	if ((object->formatFlags & 0x01u) != 0)
	{
		char timeBuffer[9] = {};

		char* timeText = FUN_004e8f0f(timeBuffer);
		const int result = FUN_004e8f88(object->stream, OriginalStrings::NetworkLogTimeFormat, timeText);

		if (result < 0)
		{
			FUN_004de237("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x10F, 0);
			object->error = 1;
			return object;
		}
	}

	for (std::int32_t remaining = indentation; remaining > 0; --remaining)
	{
		FUN_004e8f88(object->stream, OriginalStrings::NetworkLogIndentation);
	}

	// The original passes text directly as the format string.
	const int result = FUN_004e8f88(object->stream, text);

	++object->writtenEntries;
	object->error = result < 0 ? 1 : 0;

	FUN_004e8cc7(object->stream);

	return object;
}

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
	// Original function: FUN_004dc700
	bool CDPNet::InitializePeer(void* context, bool option)
	{
		// The original ignores the supplied context and reads this->callbackContext.
		(void)context;

		if (peer != nullptr)
		{
			const DWORD initializeFlags = option ? 0u : 1u;
			const HRESULT result = peer->Initialize(callbackContext, FUN_004dbd68, initializeFlags);

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