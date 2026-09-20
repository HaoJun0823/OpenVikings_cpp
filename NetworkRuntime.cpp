#include <winsock2.h>
#include <mmsystem.h>

#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"
#include "MasterPropertyManager.h"

#include <cstring>
#include <new>

// Pending dependencies.
extern char* FUN_004e3252(std::int32_t result);

// DirectPlay callback. Its implementation remains outside this block.
extern HRESULT WINAPI FUN_004dbd68(void* context, DWORD messageId, void* message);

// Original data: DAT_00569650.
// Enabled at 004db9c7 after successfully opening the network log.
std::uint8_t DAT_00569650 = 0;

// Original data: DAT_00569658
NNetwork::CDPNet* gNetworkInstance = nullptr;

// Original data: DAT_00569654
NNetwork::SNetLog* gNetworkLog = nullptr;

namespace NNetwork
{
	// Original data: 005097a8; initial bytes verified against Game.exe.
	GUID gNetworkApplicationGuid = { 0xAC671273u, 0x85B1u, 0x4C11u, { 0xAC, 0x54, 0x16, 0x6C, 0x39, 0x9C, 0x80, 0x78 } };

	// Original function: FUN_004dc3f3
	void CDPNet::SetApplicationGuid(const GUID* applicationGuid)
	{
		// The original writes shared storage, not a member of this object.
		std::memcpy(&gNetworkApplicationGuid, applicationGuid, 0x10);
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
}

namespace NC2MainMenuGuiManager
{
	// Original function: FUN_004d7092
	bool StartNetworkForMenu(void* guiManager)
	{
		auto* objectBytes = static_cast<std::byte*>(guiManager);

		// Original helper FUN_00410b46 forwards to memset with a zero value.
		std::memset(objectBytes + 0x68, 0, 0x4B8);

		if (!NNetwork::Network_StartUp())
		{
			NNetwork::Network_ShutDown();
			return false;
		}

		return true;
	}
}

namespace
{
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

	// Original storage starts at 0056965c.
// Reserve the address span up to the next referenced buffer at 0056976c;
// the original declared array size is not recoverable from these accesses.
	char gNetworkWindowsVersion[0x110] = {};

	// Original storage: 0056976c; copied with a limit of 0x40 bytes.
	char gNetworkDirectXVersion[0x40] = {};

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

// Original function: FUN_004db9f6
NNetwork::SNetLog* WriteNetworkLogHeader(NNetwork::SNetLog* object)
{
	if (DAT_00569650 == 0)
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

	while (GetNetworkIPAddress(addressIndex, address, 0x40u))
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
	DAT_00569650 = 1;

	WriteNetworkLogHeader(object);

	// The original does not clear a previous error or reset the counters.
	return object;
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

		result = peer->Initialize(callbackContext, FUN_004dbd68, option ? 0u : 1u);

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