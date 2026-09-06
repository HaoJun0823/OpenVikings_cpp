#include "NetworkShutdown.h"

#include <cstring>
#include <new>

// Pending dependencies.
extern void FUN_004e5e48(void* allocation);
extern char* FUN_004e3252(std::int32_t result);

extern int FUN_004e8f88(void* stream, const char* format, ...);
extern char* FUN_004e8f0f(char* destination);
extern int FUN_004e8cc7(void* stream);
extern void FUN_004de237(const char* sourceFile, std::int32_t line, std::int32_t value);

// DirectPlay callback. Its implementation remains outside this block.
extern HRESULT WINAPI FUN_004dbd68(void* context, DWORD messageId, void* message);

// The initial value of this global has not been established here.
extern std::uint8_t DAT_00569650;

// Original strings whose complete contents remain unresolved.
extern const char lpOutputString_0050071c[];
extern const char lpOutputString_00509630[];
extern const char DAT_005087fc[];
extern const char DAT_0050978c[];

extern const char DAT_00509eb8[];
extern const char DAT_00509e30[];
extern const char DAT_00509e8c[];
extern const char DAT_00509dec[];

// Original data: DAT_00569658
NNetwork::CDPNet* DAT_00569658 = nullptr;

// Original data: DAT_00569654
NNetwork::SNetLog* DAT_00569654 = nullptr;

NNetwork::SNetLog* FUN_004db855();
void* FUN_004dbc6d(NNetwork::SNetLog* object, const char* text, std::int32_t indentation);
void FUN_004ddc42(std::int32_t level, const char* text, std::int32_t indentation);

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
		OutputDebugStringA(lpOutputString_0050071c);
		OutputDebugStringA(FUN_004e3252(result));
		OutputDebugStringA(lpOutputString_00509630);
	}
}

// Original function: FUN_004db855
NNetwork::SNetLog* FUN_004db855()
{
	if (DAT_00569654 == nullptr)
	{
		// No value-initialization: the original constructor leaves bytes untouched.
		NNetwork::SNetLog* object = new (std::nothrow) NNetwork::SNetLog;

		if (object != nullptr)
		{
			object = InitializeNetworkLog(object);
		}

		DAT_00569654 = object;
	}

	return DAT_00569654;
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
		std::memcpy(&log, FUN_004db855(), sizeof(log));

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
NNetwork::CDPNet* FUN_004dc409()
{
	if (DAT_00569658 == nullptr)
	{
		DAT_00569658 = new (std::nothrow) NNetwork::CDPNet;
	}

	return DAT_00569658;
}

// Original function: FUN_004dbc6d
void* FUN_004dbc6d(NNetwork::SNetLog* object, const char* text, std::int32_t indentation)
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
		const int result = FUN_004e8f88(object->stream, DAT_005087fc, timeText);

		if (result < 0)
		{
			FUN_004de237("D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\ToolLibs\\DPlayBase\\netlog.cpp", 0x10F, 0);
			object->error = 1;
			return object;
		}
	}

	for (std::int32_t remaining = indentation; remaining > 0; --remaining)
	{
		FUN_004e8f88(object->stream, DAT_0050978c);
	}

	// The original passes text directly as the format string.
	const int result = FUN_004e8f88(object->stream, text);

	++object->writtenEntries;
	object->error = result < 0 ? 1 : 0;

	FUN_004e8cc7(object->stream);

	return object;
}

// Original function: FUN_004ddc42
void FUN_004ddc42(std::int32_t level, const char* text, std::int32_t indentation)
{
	// Preserve the signed comparison: negative levels also pass this check.
	if (level == 0 || level > 3)
	{
		return;
	}

	if (FUN_004db855()->stream == nullptr)
	{
		return;
	}

	FUN_004dbc6d(FUN_004db855(), text, indentation);
}

namespace NNetwork
{
	// Original function: FUN_004dc700
	bool CDPNet::FUN_004dc700(void* context, bool option)
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

			ReportDirectPlayError(DAT_00509eb8, result);
			FUN_004ddc42(3, DAT_00509e8c, 0);

			return false;
		}

		ReportDirectPlayError(DAT_00509e30, 0);
		FUN_004ddc42(3, DAT_00509dec, 0);

		return false;
	}
}

// Original function: FUN_004d77da
void FUN_004d77da(NNetwork::SSessionNode* object)
{
	if (object->description != nullptr && object->description->pwszSessionName != nullptr)
	{
		FUN_004e5e48(object->description->pwszSessionName);
		object->description->pwszSessionName = nullptr;
	}

	if (object->description != nullptr)
	{
		FUN_004e5e48(object->description);
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