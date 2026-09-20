#include <winsock2.h>
#include <Windows.h>

#include "DirectXErrorText.h"
#include "dplay8.h"
#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "NetworkShutdown.h"
#include "MasterPropertyManager.h"
#include "FileTransferManager.h"
#include "CRC32.h"

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <new>

// Original data: DAT_00569650.
// Enabled at 004db9c7 after successfully opening the network log.
std::uint8_t DAT_00569650 = 0;

// Original data: DAT_00569658
NNetwork::CDPNet* gNetworkInstance = nullptr;

// Original data: DAT_00569654
NNetwork::SNetLog* gNetworkLog = nullptr;

namespace NNetwork
{
	// Original function: FUN_004deac6
	DPNID GetDynamicNetworkHostPlayerId(const SDynamicNetworkData* data)
	{
		const auto* players = static_cast<const std::byte*>(data->allocation10);

		if (players == nullptr)
			return 0;

		for (std::uint32_t index = 0; index < data->unknown0C; ++index)
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

		// Original function: FUN_004dee4e.
		// The original constructor leaves padding01 untouched.
		packet.type = 4;
		packet.payloadSize = 0;
		packet.checksum = 0;
		std::memset(packet.payload, 0, sizeof(packet.payload));

		packet.checksum = NCRC32::Calculate(payload, static_cast<std::int32_t>(byteCount));
		packet.payloadSize = byteCount;

		std::memcpy(packet.payload, payload, byteCount);

		return network->SendPacket(recipient, &packet);
	}

	// Original function: FUN_004df207
	bool BroadcastControlMessage8(SDynamicNetworkData* /*data*/)
	{
		CDPNet* network = GetNetworkInstance();

		const std::uint8_t packet[2] = { 8, 0 };

		return network->SendPacket(0, packet);
	}

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

	// Original data: DAT_005697c0
	SFileTransferInfo gFileTransferInfo = {};

	// Original data: DAT_005697dc
	std::uint8_t gFileTransferInfoInitializationFlags = 0;

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
		node->unknown1C = timeGetTime();
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
		OutputDebugStringA(NXBasics::GetDirectXErrorText(result));
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

namespace NNetwork
{
	namespace
	{
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
}

namespace NNetwork
{
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
}

namespace NNetwork
{
	namespace
	{
		// Original data: DAT_005697e4.
		CFileTransferManager* gFileTransferManager = nullptr;

		struct SFileTransferMetadataPacket
		{
			std::uint8_t type;                    // +0x00
			std::byte reserved01[3];              // +0x01
			std::byte transferId[0x10];           // +0x04
			std::uint32_t checksum;               // +0x14
			std::uint32_t fileSize;               // +0x18
		};

		static_assert(sizeof(SFileTransferMetadataPacket) == 0x1C);
		static_assert(offsetof(SFileTransferMetadataPacket, checksum) == 0x14);
		static_assert(offsetof(SFileTransferMetadataPacket, fileSize) == 0x18);
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
		// Original list initialization: FUN_004dffdf.
		transfers.first = nullptr;

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

	// Original function: FUN_004e0690
	void CFileTransferManager::DispatchMessage(const std::uint8_t* packet)
	{
		switch (packet[0])
		{
			case 11:
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

			case 12:
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

			case 13:
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

			case 14:
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

			case 15:
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
			case 2:
				buffer.dwBufferSize = 2;
				sendFlags = 0x22u;
				break;

			case 3:
				{
					std::uint32_t length = 0;

					while (bytes[2u + length * 2u] != 0 || bytes[3u + length * 2u] != 0)
						++length;

					buffer.dwBufferSize = length * 2u + 4u;
					sendFlags = 0x42u;
					break;
				}

			case 4:
				{
					std::uint32_t payloadSize;
					std::memcpy(&payloadSize, bytes + 4, sizeof(payloadSize));

					buffer.dwBufferSize = payloadSize + 0x0Cu;
					sendFlags = 0x02u;
					break;
				}

			case 5:
				buffer.dwBufferSize = static_cast<std::uint32_t>(bytes[8]) + 0x0Cu;
				sendFlags = 0x28u;
				break;

			case 6:
				buffer.dwBufferSize = 0x0Cu;
				sendFlags = 0x2Cu;
				break;

			case 7:
				buffer.dwBufferSize = 2;
				sendFlags = 0x24u;
				break;

			case 8:
				buffer.dwBufferSize = 2;
				sendFlags = 0x88u;
				break;

			case 9:
				buffer.dwBufferSize = 0x10u;
				sendFlags = 0x88u;
				break;

			case 10:
				// Inline the packet initialization from FUN_004dd864.
				convertedPacket[0] = 5;
				std::memcpy(convertedPacket + 4, bytes + 4, 4);
				convertedPacket[8] = bytes[8];
				std::memset(convertedPacket + 9, 0, 0xC8);
				std::memcpy(convertedPacket + 9, bytes + 9, bytes[8]);

				buffer.dwBufferSize = sizeof(convertedPacket);
				buffer.pBufferData = convertedPacket;
				sendFlags = 0x88u;
				break;

			case 11:
				buffer.dwBufferSize = 0x118u;
				sendFlags = 0x20u;
				break;

			case 12:
				buffer.dwBufferSize = 0x18u;
				break;

			case 13:
				buffer.dwBufferSize = 0x14u;
				break;

			case 14:
				buffer.dwBufferSize = 0x1Cu;
				break;

			case 15:
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

	// Original function: FUN_004e03a1
	void CFileTransferManager::ProcessJobList()
	{
		EnterCriticalSection(&critical);

		for (SFileTransferNode* node = transfers.GetFirst(); node != nullptr; node = node->next)
		{
			switch (node->state)
			{
				case 0:
					{
						CDPNet* network = GetNetworkInstance();
						std::uint8_t packet[0x118];

						// Inline the relevant initialization from FUN_004e0a2c.
						packet[0] = 11;
						std::memcpy(packet + 4, node->transferId, sizeof(node->transferId));

						const DPNID localPlayerId = network->GetLocalPlayerId();
						std::memcpy(packet + 0x14, &localPlayerId, sizeof(localPlayerId));

						std::memset(packet + 0x18, 0, 0x100);
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

				case 1:
				case 5:
				case 7:
				case 10:
				case 12:
				case 14:
					break;

				case 4:
					OutputDebugStringA("CFileTransferManager::ProcessJobList(): Job aborted -> change state to 'invalid'.\n");
					node->state = 14;
					break;

				case 6:
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

				case 8:
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

				case 13:
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
}