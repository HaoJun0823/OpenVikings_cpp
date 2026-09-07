#pragma once

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
	struct RuntimeErrorMapping
	{
		std::uint32_t osError;
		std::uint32_t crtError;
	};

	// Original data: 0050c250 through 0050c3b7.
	inline constexpr RuntimeErrorMapping gRuntimeErrorMappings[] =
	{
		{ 0x0001u, 0x16u },
		{ 0x0002u, 0x02u },
		{ 0x0003u, 0x02u },
		{ 0x0004u, 0x18u },
		{ 0x0005u, 0x0Du },
		{ 0x0006u, 0x09u },
		{ 0x0007u, 0x0Cu },
		{ 0x0008u, 0x0Cu },
		{ 0x0009u, 0x0Cu },
		{ 0x000Au, 0x07u },
		{ 0x000Bu, 0x08u },
		{ 0x000Cu, 0x16u },
		{ 0x000Du, 0x16u },
		{ 0x000Fu, 0x02u },
		{ 0x0010u, 0x0Du },
		{ 0x0011u, 0x12u },
		{ 0x0012u, 0x02u },
		{ 0x0021u, 0x0Du },
		{ 0x0035u, 0x02u },
		{ 0x0041u, 0x0Du },
		{ 0x0043u, 0x02u },
		{ 0x0050u, 0x11u },
		{ 0x0052u, 0x0Du },
		{ 0x0053u, 0x0Du },
		{ 0x0057u, 0x16u },
		{ 0x0059u, 0x0Bu },
		{ 0x006Cu, 0x0Du },
		{ 0x006Du, 0x20u },
		{ 0x0070u, 0x1Cu },
		{ 0x0072u, 0x09u },
		{ 0x0006u, 0x16u }, // Preserve the duplicate and original order.
		{ 0x0080u, 0x0Au },
		{ 0x0081u, 0x0Au },
		{ 0x0082u, 0x09u },
		{ 0x0083u, 0x16u },
		{ 0x0084u, 0x0Du },
		{ 0x0091u, 0x29u },
		{ 0x009Eu, 0x0Du },
		{ 0x00A1u, 0x02u },
		{ 0x00A4u, 0x0Bu },
		{ 0x00A7u, 0x0Du },
		{ 0x00B7u, 0x11u },
		{ 0x00CEu, 0x02u },
		{ 0x00D7u, 0x0Bu },
		{ 0x0718u, 0x0Cu }
	};

	static_assert(sizeof(gRuntimeErrorMappings) / sizeof(gRuntimeErrorMappings[0]) == 45, "The original error table contains 45 entries");

	// Original function: FUN_004e7575
	inline void Runtime_MapOsError(std::uint32_t osError)
	{
		*Runtime_GetDosErrnoPointer() = osError;

		for (const auto& entry : gRuntimeErrorMappings)
		{
			if (entry.osError == osError)
			{
				DWORD* errorPointer = Runtime_GetErrnoPointer();
				*errorPointer = entry.crtError;
				return;
			}
		}

		if (osError >= 0x13u && osError <= 0x24u)
		{
			*Runtime_GetErrnoPointer() = 0x0D;
			return;
		}

		if (osError >= 0xBCu && osError <= 0xCAu)
		{
			*Runtime_GetErrnoPointer() = 0x08;
			return;
		}

		*Runtime_GetErrnoPointer() = 0x16;
	}

	// Original function: FUN_004eb1bb
	inline void Runtime_InitializeThreadData(void* threadData)
	{
		auto* threadBytes = static_cast<std::uint8_t*>(threadData);

		const void* initialTable = &DAT_0050c968;
		std::memcpy(threadBytes + 0x50, &initialTable, sizeof(initialTable));

		const std::uint32_t initialValue = 1;
		std::memcpy(threadBytes + 0x14, &initialValue, sizeof(initialValue));
	}

	// Original function: FUN_004eb1ce
	inline DWORD* Runtime_GetThreadData()
	{
		const DWORD savedLastError = ::GetLastError();
		void* threadData = ::TlsGetValue(dwTlsIndex_0050c6e4);

		if (threadData == nullptr)
		{
			threadData = FUN_004ef3ac(1, 0x74);

			if (threadData == nullptr || !::TlsSetValue(dwTlsIndex_0050c6e4, threadData))
			{
				// Fatal CRT error; the original performs no local cleanup.
				__amsg_exit(0x10);
			}
			else
			{
				Runtime_InitializeThreadData(threadData);

				const DWORD threadId = ::GetCurrentThreadId();
				auto* threadBytes = static_cast<std::uint8_t*>(threadData);

				const std::uint32_t invalidHandle = 0xFFFFFFFFu;
				std::memcpy(threadBytes + 0x04, &invalidHandle, sizeof(invalidHandle));
				std::memcpy(threadBytes, &threadId, sizeof(threadId));
			}
		}

		::SetLastError(savedLastError);
		return static_cast<DWORD*>(threadData);
	}

	// Original function: FUN_004e75e8
	inline DWORD* Runtime_GetErrnoPointer()
	{
		auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
		return reinterpret_cast<DWORD*>(threadBytes + 0x08);
	}

	// Original function: FUN_004e75f1
	inline DWORD* Runtime_GetDosErrnoPointer()
	{
		auto* threadBytes = reinterpret_cast<std::uint8_t*>(Runtime_GetThreadData());
		return reinterpret_cast<DWORD*>(threadBytes + 0x0C);
	}

	// Original function: FUN_004e7575
	inline void Runtime_MapOsError(std::uint32_t osError)
	{
		*Runtime_GetDosErrnoPointer() = osError;

		const auto* errorTable = reinterpret_cast<const std::uint8_t*>(&DAT_0050c250);

		for (std::uint32_t index = 0; index < 45u; ++index)
		{
			const auto* entry = errorTable + index * 8u;

			std::uint32_t tableOsError;
			std::memcpy(&tableOsError, entry, sizeof(tableOsError));

			if (tableOsError == osError)
			{
				// Obtain the errno pointer before reading the mapped value.
				DWORD* errorPointer = Runtime_GetErrnoPointer();

				std::uint32_t mappedError;
				std::memcpy(&mappedError, entry + 4u, sizeof(mappedError));

				*errorPointer = mappedError;
				return;
			}
		}

		if (osError >= 0x13u && osError <= 0x24u)
		{
			*Runtime_GetErrnoPointer() = 0x0D;
			return;
		}

		if (osError >= 0xBCu && osError <= 0xCAu)
		{
			*Runtime_GetErrnoPointer() = 0x08;
			return;
		}

		*Runtime_GetErrnoPointer() = 0x16;
	}

	// Original function: FUN_004eb167
	inline std::int32_t Runtime_InitializeThreading()
	{
		Runtime_InitializeLocks();

		const DWORD tlsIndex = ::TlsAlloc();
		dwTlsIndex_0050c6e4 = tlsIndex;

		if (tlsIndex == TLS_OUT_OF_INDEXES)
		{
			return 0;
		}

		auto* threadData = FUN_004ef3ac(1, 0x74);

		if (threadData == nullptr)
		{
			return 0;
		}

		if (!::TlsSetValue(dwTlsIndex_0050c6e4, threadData))
		{
			return 0;
		}

		Runtime_InitializeThreadData(threadData);

		const DWORD threadId = ::GetCurrentThreadId();
		auto* threadBytes = reinterpret_cast<std::uint8_t*>(threadData);

		const std::uint32_t invalidHandle = 0xFFFFFFFFu;
		std::memcpy(threadBytes + 0x04, &invalidHandle, sizeof(invalidHandle));
		std::memcpy(threadBytes, &threadId, sizeof(threadId));

		return 1;
	}

	// Original function: FUN_004ebdc2
	inline void Runtime_InitializeLocks()
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);

		::InitializeCriticalSection(slots[0x11]);
		::InitializeCriticalSection(slots[0x0D]);
		::InitializeCriticalSection(slots[0x09]);
		::InitializeCriticalSection(slots[0x01]);
	}

	// Original function: FUN_004ebe4c
	inline void Runtime_Unlock(std::int32_t lockIndex)
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);

		::LeaveCriticalSection(slots[lockIndex]);
	}

	// Original function: FUN_004ebdeb
	inline void Runtime_Lock(std::int32_t lockIndex)
	{
		auto* slots = reinterpret_cast<CRITICAL_SECTION* volatile*>(&lpCriticalSection_0050c798);
		auto* slot = slots + lockIndex;

		if (*slot == nullptr)
		{
			// Keep the original CRT allocation and deallocation pair.
			auto* candidate = static_cast<CRITICAL_SECTION*>(_malloc(0x18));

			if (candidate == nullptr)
			{
				__amsg_exit(0x11);
			}

			Runtime_Lock(0x11);

			// Another thread may have initialized this slot in the meantime.
			if (*slot == nullptr)
			{
				::InitializeCriticalSection(candidate);
				*slot = candidate;
			}
			else
			{
				FUN_004e8513(reinterpret_cast<std::uint8_t*>(candidate));
			}

			Runtime_Unlock(0x11);
		}

		::EnterCriticalSection(*slot);
	}

	// Original function: FUN_004ea8a6
	inline void Runtime_LockDescriptor(std::uint32_t descriptor)
	{
		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
		auto* entry = block + entryOffset;
		auto* initialized = reinterpret_cast<volatile std::uint32_t*>(entry + 0x08);

		if (*initialized == 0)
		{
			Runtime_Lock(0x11);

			// Recheck the original entry after acquiring the global CRT lock.
			if (*initialized == 0)
			{
				auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(entry + 0x0C);
				::InitializeCriticalSection(criticalSection);

				*initialized = *initialized + 1u;
			}

			Runtime_Unlock(0x11);
		}

		// The original reloads the block pointer before entering the section.
		block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
		auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

		::EnterCriticalSection(criticalSection);
	}

	// Original function: FUN_004ea905
	inline void Runtime_UnlockDescriptor(std::uint32_t descriptor)
	{
		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);
		auto* criticalSection = reinterpret_cast<CRITICAL_SECTION*>(block + entryOffset + 0x0C);

		::LeaveCriticalSection(criticalSection);
	}

	static_assert(sizeof(CRITICAL_SECTION) == 0x18, "Descriptor locks require the Win32 critical-section layout");

	// Original function: FUN_004ea864
	inline std::uint32_t Runtime_GetOsHandle(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				std::uint32_t handleValue;
				std::memcpy(&handleValue, block + entryOffset, sizeof(handleValue));
				return handleValue;
			}
		}

		*Runtime_GetErrnoPointer() = 9;
		*Runtime_GetDosErrnoPointer() = 0;
		return 0xFFFFFFFFu;
	}

	// Original function: FUN_004ea7e5
	inline std::int32_t Runtime_ClearOsHandle(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const std::uint32_t blockIndex = descriptor >> 5;
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;
			auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				std::uint32_t handleValue;
				std::memcpy(&handleValue, block + entryOffset, sizeof(handleValue));

				if (handleValue != 0xFFFFFFFFu)
				{
					if (DAT_0050c244 == 1)
					{
						// The original ignores the return value of SetStdHandle.
						switch (descriptor)
						{
						case 0u:
							::SetStdHandle(STD_INPUT_HANDLE, nullptr);
							break;

						case 1u:
							::SetStdHandle(STD_OUTPUT_HANDLE, nullptr);
							break;

						case 2u:
							::SetStdHandle(STD_ERROR_HANDLE, nullptr);
							break;
						}
					}

					// Reload the block pointer after the possible Windows call.
					block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]);

					const std::uint32_t invalidHandle = 0xFFFFFFFFu;
					std::memcpy(block + entryOffset, &invalidHandle, sizeof(invalidHandle));
					return 0;
				}
			}
		}

		*Runtime_GetErrnoPointer() = 9;
		*Runtime_GetDosErrnoPointer() = 0;
		return -1;
	}

	// Original function: FUN_004e60de
	inline std::int32_t Runtime_CloseNoLock(std::uint32_t descriptor)
	{
		DWORD closeError = 0;

		const auto initialHandle = Runtime_GetOsHandle(descriptor);

		if (static_cast<std::uint32_t>(initialHandle) != 0xFFFFFFFFu)
		{
			bool closeHandle = true;

			if (descriptor == 1u || descriptor == 2u)
			{
				// Preserve the original lookup order.
				const auto stderrHandle = Runtime_GetOsHandle(2u);
				const auto stdoutHandle = Runtime_GetOsHandle(1u);

				closeHandle = stdoutHandle != stderrHandle;
			}

			if (closeHandle)
			{
				// The original retrieves the handle again before closing it.
				const auto handleValue = Runtime_GetOsHandle(descriptor);
				const HANDLE handle = reinterpret_cast<HANDLE>(static_cast<std::uintptr_t>(handleValue));

				if (!::CloseHandle(handle))
				{
					closeError = ::GetLastError();
				}
			}
		}

		// Perform descriptor cleanup even when CloseHandle failed.
		Runtime_ClearOsHandle(descriptor);

		auto* block = reinterpret_cast<std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		// Clear the entire flags byte, not only the open bit.
		block[entryOffset + 4u] = 0;

		if (closeError != 0)
		{
			Runtime_MapOsError(closeError);
			return -1;
		}

		return 0;
	}

	// Original function: FUN_004e6081
	inline std::int32_t Runtime_Close(std::uint32_t descriptor)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				Runtime_LockDescriptor(descriptor);

				const std::int32_t result = Runtime_CloseNoLock(descriptor);

				Runtime_UnlockDescriptor(descriptor);
				return result;
			}
		}

		// Invalid or closed descriptor.
		*Runtime_GetErrnoPointer() = 9;
		*Runtime_GetDosErrnoPointer() = 0;
		return -1;
	}

	// Original function: FUN_004e650a
	inline std::int32_t Runtime_WriteNoLock(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
	{
		if (byteCount == 0)
		{
			return 0;
		}

		const std::uint32_t blockIndex = descriptor >> 5;
		const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

		// DAT_0056b600 remains the original, unresolved descriptor-block table.
		const auto getEntry = [blockIndex, entryOffset]() -> const std::uint8_t*
			{
				return reinterpret_cast<const std::uint8_t*>(DAT_0056b600[blockIndex]) + entryOffset;
			};

		const auto getHandle = [&getEntry]() -> HANDLE
			{
				HANDLE handle;
				std::memcpy(&handle, getEntry(), sizeof(handle));
				return handle;
			};

		if ((getEntry()[4] & 0x20u) != 0)
		{
			// The original ignores the result of seeking to the end.
			FUN_004e6d27(descriptor, 0, 2);
		}

		const auto* input = static_cast<const std::uint8_t*>(data);
		std::uint32_t writtenTotal = 0;
		std::uint32_t insertedCarriageReturns = 0;
		DWORD writeError = 0;

		if ((getEntry()[4] & 0x80u) != 0)
		{
			std::uint8_t buffer[0x401];
			std::uint32_t consumed = 0;

			while (consumed < byteCount)
			{
				std::uint32_t buffered = 0;

				while (consumed < byteCount)
				{
					const std::uint8_t value = input[consumed];
					++consumed;

					if (value == 0x0A)
					{
						++insertedCarriageReturns;
						buffer[buffered++] = 0x0D;
					}

					buffer[buffered++] = value;

					if (buffered >= 0x400u)
					{
						break;
					}
				}

				DWORD written;

				if (!::WriteFile(getHandle(), buffer, buffered, &written, nullptr))
				{
					writeError = ::GetLastError();
					break;
				}

				writtenTotal += written;

				// Preserve the signed comparison performed by the original.
				std::int32_t signedWritten;
				std::memcpy(&signedWritten, &written, sizeof(signedWritten));

				if (signedWritten < static_cast<std::int32_t>(buffered))
				{
					break;
				}
			}
		}
		else
		{
			DWORD written;

			if (::WriteFile(getHandle(), data, byteCount, &written, nullptr))
			{
				writtenTotal = written;
			}
			else
			{
				writeError = ::GetLastError();
			}
		}

		if (writtenTotal != 0)
		{
			// Count all inserted CR bytes, including those in a partially written
			// or failed final text chunk, as the original does.
			const std::uint32_t resultBits = writtenTotal - insertedCarriageReturns;
			std::int32_t result;
			std::memcpy(&result, &resultBits, sizeof(result));
			return result;
		}

		if (writeError != 0)
		{
			if (writeError == 5)
			{
				*Runtime_GetErrnoPointer() = 9;
				*Runtime_GetDosErrnoPointer() = 5;
			}
			else
			{
				Runtime_MapOsError(writeError);
			}

			return -1;
		}

		if ((getEntry()[4] & 0x40u) != 0 && input[0] == 0x1A)
		{
			return 0;
		}

		*Runtime_GetErrnoPointer() = 28;
		*Runtime_GetDosErrnoPointer() = 0;
		return -1;
	}

	// Original function: FUN_004e64a5
	inline std::int32_t Runtime_Write(std::uint32_t descriptor, const void* data, std::uint32_t byteCount)
	{
		if (descriptor < static_cast<std::uint32_t>(uNumber_0056b700))
		{
			const auto* block = reinterpret_cast<const std::uint8_t*>(DAT_0056b600[descriptor >> 5]);
			const std::uint32_t entryOffset = (descriptor & 31u) * 36u;

			if ((block[entryOffset + 4u] & 0x01u) != 0)
			{
				FUN_004ea8a6(descriptor);

				const std::int32_t result = Runtime_WriteNoLock(descriptor, data, byteCount);

				FUN_004ea905(descriptor);
				return result;
			}
		}

		*Runtime_GetErrnoPointer() = 9;
		*Runtime_GetDosErrnoPointer() = 0;
		return -1;
	}

	// Original function: FUN_004e67b5
	inline std::int32_t Runtime_OpenShared(const char* fileName, std::uint32_t flags, std::uint32_t shareMode, std::uint32_t permissions)
	{
		SECURITY_ATTRIBUTES security{};
		security.nLength = sizeof(security);
		security.lpSecurityDescriptor = nullptr;
		security.bInheritHandle = (flags & 0x80u) == 0;

		std::uint8_t descriptorFlags = (flags & 0x80u) != 0 ? 0x10u : 0u;

		if ((flags & 0x8000u) == 0)
		{
			if ((flags & 0x4000u) != 0 || DAT_00569b2c != 0x8000)
			{
				descriptorFlags |= 0x80u;
			}
		}

		const auto failWithError = [](std::int32_t error) -> std::int32_t
			{
				*Runtime_GetErrnoPointer() = error;
				*Runtime_GetDosErrnoPointer() = 0;
				return -1;
			};

		DWORD desiredAccess;

		switch (flags & 3u)
		{
		case 0:
			desiredAccess = GENERIC_READ;
			break;

		case 1:
			desiredAccess = GENERIC_WRITE;
			break;

		case 2:
			desiredAccess = GENERIC_READ | GENERIC_WRITE;
			break;

		default:
			return failWithError(22);
		}

		DWORD sharing;

		switch (shareMode)
		{
		case 0x10:
			sharing = 0;
			break;

		case 0x20:
			sharing = FILE_SHARE_READ;
			break;

		case 0x30:
			sharing = FILE_SHARE_WRITE;
			break;

		case 0x40:
			sharing = FILE_SHARE_READ | FILE_SHARE_WRITE;
			break;

		default:
			return failWithError(22);
		}

		DWORD creationDisposition;

		switch (flags & 0x700u)
		{
		case 0x000:
		case 0x400:
			creationDisposition = OPEN_EXISTING;
			break;

		case 0x100:
			creationDisposition = OPEN_ALWAYS;
			break;

		case 0x200:
		case 0x600:
			creationDisposition = TRUNCATE_EXISTING;
			break;

		case 0x300:
			creationDisposition = CREATE_ALWAYS;
			break;

		case 0x500:
		case 0x700:
			creationDisposition = CREATE_NEW;
			break;

		default:
			return failWithError(22);
		}

		DWORD attributes = FILE_ATTRIBUTE_NORMAL;

		if ((flags & 0x100u) != 0)
		{
			const std::uint32_t effectivePermissions = permissions & ~static_cast<std::uint32_t>(DAT_00569a00);

			if ((effectivePermissions & 0x80u) == 0)
			{
				attributes = FILE_ATTRIBUTE_READONLY;
			}
		}

		if ((flags & 0x40u) != 0)
		{
			attributes |= FILE_FLAG_DELETE_ON_CLOSE;
			desiredAccess |= DELETE;
		}

		if ((flags & 0x1000u) != 0)
		{
			attributes |= FILE_ATTRIBUTE_TEMPORARY;
		}

		if ((flags & 0x20u) != 0)
		{
			attributes |= FILE_FLAG_SEQUENTIAL_SCAN;
		}
		else if ((flags & 0x10u) != 0)
		{
			attributes |= FILE_FLAG_RANDOM_ACCESS;
		}

		const std::uint32_t descriptorBits = FUN_004ea646();

		if (descriptorBits == 0xFFFFFFFFu)
		{
			return failWithError(24);
		}

		std::int32_t descriptor;
		std::memcpy(&descriptor, &descriptorBits, sizeof(descriptor));

		const HANDLE handle = ::CreateFileA(fileName, desiredAccess, sharing, &security, creationDisposition, attributes, nullptr);

		const auto failWithLastError = [descriptor]() -> std::int32_t
			{
				const DWORD error = ::GetLastError();
				Runtime_MapOsError(error);
				FUN_004ea905(descriptor);
				return -1;
			};

		if (handle == INVALID_HANDLE_VALUE)
		{
			return failWithLastError();
		}

		const DWORD fileType = ::GetFileType(handle);

		if (fileType == FILE_TYPE_UNKNOWN)
		{
			// Preserve the original order: close first, then obtain the error.
			::CloseHandle(handle);
			return failWithLastError();
		}

		if (fileType == FILE_TYPE_CHAR)
		{
			descriptorFlags |= 0x40u;
		}
		else if (fileType == FILE_TYPE_PIPE)
		{
			descriptorFlags |= 0x08u;
		}

		FUN_004ea769(descriptor, handle);

		const std::uint32_t blockIndex = descriptorBits >> 5;
		const std::uint32_t entryOffset = (descriptorBits & 31u) * 36u;

		const auto getEntry = [blockIndex, entryOffset]() -> std::uint8_t*
			{
				return reinterpret_cast<std::uint8_t*>(DAT_0056b600[blockIndex]) + entryOffset;
			};

		descriptorFlags |= 0x01u;
		getEntry()[4] = descriptorFlags;

		const bool isDeviceOrPipe = (descriptorFlags & 0x48u) != 0;

		const auto closeAndFail = [descriptor]() -> std::int32_t
			{
				Runtime_Close(descriptor);
				FUN_004ea905(descriptor);
				return -1;
			};

		if (!isDeviceOrPipe && (descriptorFlags & 0x80u) != 0 && (flags & 2u) != 0)
		{
			const std::uint32_t endPosition = FUN_004e6d27(descriptor, -1, 2);

			if (endPosition == 0xFFFFFFFFu)
			{
				if (*Runtime_GetDosErrnoPointer() != 0x83u)
				{
					return closeAndFail();
				}
			}
			else
			{
				std::uint8_t finalByte = 0;
				const std::int32_t readResult = FUN_004e6ae9(descriptor, &finalByte, 1);

				// Preserve the original zero-result test.
				if (readResult == 0 && finalByte == 0x1A)
				{
					const std::uint32_t resizeResult = FUN_004eb235(descriptor, endPosition);

					if (resizeResult == 0xFFFFFFFFu)
					{
						return closeAndFail();
					}
				}

				const std::uint32_t resetPosition = FUN_004e6d27(descriptor, 0, 0);

				if (resetPosition == 0xFFFFFFFFu)
				{
					return closeAndFail();
				}
			}
		}

		if (!isDeviceOrPipe && (flags & 0x08u) != 0)
		{
			getEntry()[4] |= 0x20u;
		}

		FUN_004ea905(descriptor);
		return descriptor;
	}

	// Original function: FUN_004e679e
	inline std::int32_t Runtime_Open(const char* fileName, std::uint32_t flags, std::uint32_t permissions)
	{
		return Runtime_OpenShared(fileName, flags, 0x40u, permissions);
	}

	static_assert(sizeof(void*) == 4, "These CRT descriptor accesses require an x86 build");
}