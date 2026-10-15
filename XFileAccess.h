#pragma once

#include "XNumericConstants.h"

#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "XDiagnostics.h"
#include "XFile.h"

#include <cstdint>
#include <cstring>
#include <new>

namespace NXBasics
{
	inline std::uint8_t Archive_CalculateNameCheck(const char* fileName);
	inline std::uint32_t Archive_FindFileIndex(void* archive, const char* fileName);
	inline bool Archive_IsFileInLibrary(void* archive, const char* fileName);
	inline std::uint32_t Archive_GetFileOffset(void* archive, const char* fileName);
	inline std::uint32_t Archive_GetFileSize(void* archive, const char* fileName);
	inline void File_SeekToPosition(void* file, std::uint32_t position);

	// Original function: FUN_00406126; NXBasics::CFile::Close().
	inline void File_Close(void* file)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return;
		}

		const std::int32_t result = Runtime_Close(static_cast<std::uint32_t>(descriptor));

		if (static_cast<std::uint32_t>(result) == NXBasics::IntegerRange::UInt32Maximum)
		{
			const auto firstError = *NXBasics::Runtime_GetErrnoPointer();
			const auto errorText = NXBasics::Runtime_GetErrorText(firstError);

			// Read errno again after obtaining its description.
			const auto currentError = *NXBasics::Runtime_GetErrnoPointer();
			const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

			NXBasics::ReportFatalError(OriginalStrings::FileCloseError, fileName, currentError, errorText);
		}

		const std::int32_t invalidIndex = -1;
		std::memcpy(fileBytes + 0x04, &invalidIndex, sizeof(invalidIndex));
		std::memcpy(fileBytes + 0x10C, &invalidIndex, sizeof(invalidIndex));
	}

	// Original function: FUN_00406170
	inline void File_Read(void* file, void* destination, std::uint32_t byteCount)
	{
		const auto* fileBytes = static_cast<const std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return;
		}

		const std::int32_t bytesRead = Runtime_Read(static_cast<std::uint32_t>(descriptor), destination, byteCount);

		if (static_cast<std::uint32_t>(bytesRead) == byteCount)
		{
			return;
		}

		const auto firstError = *Runtime_GetErrnoPointer();
		const auto errorText = NXBasics::Runtime_GetErrorText(firstError);
		const auto currentError = *Runtime_GetErrnoPointer();
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		NXBasics::ReportFatalError("NXBasics::CFile::Read\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
	}

	// Original function: FUN_00406286
	inline void File_SeekToPosition(void* file, std::uint32_t position)
	{
		const auto* fileBytes = static_cast<const std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return;
		}

		std::uint32_t archiveIndex;
		std::memcpy(&archiveIndex, fileBytes + 0x10C, sizeof(archiveIndex));

		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		if (archiveIndex != NXBasics::IntegerRange::UInt32Maximum)
		{
			void* archive = CFile::mStaticVars.fileLibraries[archiveIndex];

			const std::uint32_t entryOffset = Archive_GetFileOffset(archive, fileName);
			position += entryOffset;
		}

		// Reload the descriptor after resolving the archive offset.
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		std::int32_t seekOffset;
		std::memcpy(&seekOffset, &position, sizeof(seekOffset));

		const std::uint32_t result = Runtime_Seek(static_cast<std::uint32_t>(descriptor), seekOffset, 0);

		if (result == NXBasics::IntegerRange::UInt32Maximum)
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = NXBasics::Runtime_GetErrorText(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::SeekToPosition\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}
	}

	// Original function: FUN_004062ee
	inline std::uint32_t File_GetPosition(const void* file)
	{
		const auto* fileBytes = static_cast<const std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return 0;
		}

		std::uint32_t position = Runtime_Seek(static_cast<std::uint32_t>(descriptor), 0, 1);
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		if (position == NXBasics::IntegerRange::UInt32Maximum)
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = NXBasics::Runtime_GetErrorText(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::Position\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}

		std::uint32_t archiveIndex;
		std::memcpy(&archiveIndex, fileBytes + 0x10C, sizeof(archiveIndex));

		if (archiveIndex != NXBasics::IntegerRange::UInt32Maximum)
		{
			void* archive = CFile::mStaticVars.fileLibraries[archiveIndex];

			const std::uint32_t entryOffset = Archive_GetFileOffset(archive, fileName);
			position -= entryOffset;
		}

		return position;
	}

	// Original function: FUN_0040635a
	inline std::uint32_t File_GetSize(const void* file)
	{
		const auto* fileBytes = static_cast<const std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return 0;
		}

		std::uint32_t archiveIndex;
		std::memcpy(&archiveIndex, fileBytes + 0x10C, sizeof(archiveIndex));

		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		if (archiveIndex != NXBasics::IntegerRange::UInt32Maximum)
		{
			void* archive = CFile::mStaticVars.fileLibraries[archiveIndex];

			return Archive_GetFileSize(archive, fileName);
		}

		const std::uint32_t size = Runtime_GetFileSize(static_cast<std::uint32_t>(descriptor));

		if (size == NXBasics::IntegerRange::UInt32Maximum)
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = NXBasics::Runtime_GetErrorText(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::Size\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}

		return size;
	}

	// Original function: FUN_0040665d
	// Mac symbol: NXBasics::CSimpleFileLibrary::CSimpleFileLibrary(char const*).
	inline void* Archive_Initialize(void* archive, const char* fileName)
	{
		auto* state = ::new (archive) SArchiveState;
		std::memset(state, 0, sizeof(*state));

		void* fileStorage = Runtime_AllocateWithNewHandler(272u, 1);
		CFile* archiveFile = nullptr;

		if (fileStorage != nullptr)
		{
			archiveFile = ::new (fileStorage) CFile(fileName, true);
		}

		state->archiveFile = archiveFile;

		// The original reads but does not validate this value.
		std::uint32_t ignoredHeader;
		File_Read(state->archiveFile, &ignoredHeader, 4u);
		File_Read(state->archiveFile, &state->directoryEntryCount, 4u);
		File_Read(state->archiveFile, &state->entryCount, 4u);

		// Preserve the original reuse of the filename argument slot.
		std::uint32_t nameLength = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(fileName));

		if (state->directoryEntryCount != 0)
		{
			const std::uint32_t allocationBytes = state->directoryEntryCount << 3;
			auto* tableStorage = static_cast<std::uint8_t*>(Runtime_AllocateWithNewHandler(allocationBytes, 1));
			state->directoryEntries = reinterpret_cast<SArchiveDirectoryEntry*>(tableStorage);

			const std::uint32_t clearBytes = state->directoryEntryCount << 3;
			std::memset(tableStorage, 0, clearBytes);

			for (std::uint32_t index = 0; index < state->directoryEntryCount; ++index)
			{
				auto* entry = ::new (tableStorage + index * 0x08u) SArchiveDirectoryEntry;

				File_Read(state->archiveFile, &nameLength, 4u);

				entry->name = static_cast<char*>(Runtime_AllocateWithNewHandler(nameLength + 1u, 1));
				entry->name[nameLength] = '\0';

				File_Read(state->archiveFile, entry->name, nameLength);
				File_Read(state->archiveFile, &entry->value, 4u);
			}
		}

		// Preserve allocation even when the file entry count is zero.
		const std::uint32_t allocationBytes = state->entryCount << 4;
		auto* tableStorage = static_cast<std::uint8_t*>(Runtime_AllocateWithNewHandler(allocationBytes, 1));
		state->entries = reinterpret_cast<SArchiveFileEntry*>(tableStorage);

		const std::uint32_t clearBytes = state->entryCount << 4;
		std::memset(tableStorage, 0, clearBytes);

		for (std::uint32_t index = 0; index < state->entryCount; ++index)
		{
			auto* entry = ::new (tableStorage + index * 0x10u) SArchiveFileEntry;

			File_Read(state->archiveFile, &nameLength, 4u);

			entry->name = static_cast<char*>(Runtime_AllocateWithNewHandler(nameLength + 1u, 1));
			entry->name[nameLength] = '\0';

			File_Read(state->archiveFile, entry->name, nameLength);
			entry->nameCheck = Archive_CalculateNameCheck(entry->name);

			File_Read(state->archiveFile, &entry->fileOffset, 4u);
			File_Read(state->archiveFile, &entry->fileSize, 4u);
		}

		return state;
	}

	// Original function: FUN_004067d7
	// Mac symbol: NXBasics::CSimpleFileLibrary::~CSimpleFileLibrary().
	inline void Archive_Destroy(void* archive)
	{
		auto* state = static_cast<SArchiveState*>(archive);

		for (std::uint32_t index = 0; index < state->entryCount; ++index)
		{
			Runtime_Free(state->entries[index].name);
		}

		Runtime_Free(state->entries);

		for (std::uint32_t index = 0; index < state->directoryEntryCount; ++index)
		{
			Runtime_Free(state->directoryEntries[index].name);
		}

		Runtime_Free(state->directoryEntries);

		CFile* archiveFile = state->archiveFile;

		if (archiveFile != nullptr)
		{
			archiveFile->~CFile();
			Runtime_Free(archiveFile);
		}

		// The caller frees the archive allocation after this function returns.
		std::memset(state, 0, sizeof(*state));
	}

	// Original function: FUN_00406841
	// Mac symbol: NXBasics::CSimpleFileLibrary::IsFileInLibrary(char const*) const.
	inline bool Archive_IsFileInLibrary(void* archive, const char* fileName)
	{
		const auto* state = static_cast<const SArchiveState*>(archive);

		if (state->entries == nullptr)
		{
			return false;
		}

		return Archive_FindFileIndex(archive, fileName) != NXBasics::IntegerRange::UInt32Maximum;
	}

	// Original function: FUN_00406861
	inline std::uint32_t Archive_GetFileSize(void* archive, const char* fileName)
	{
		const auto* state = static_cast<const SArchiveState*>(archive);

		if (state->entries == nullptr)
		{
			return 0;
		}

		const std::uint32_t entryIndex = Archive_FindFileIndex(archive, fileName);

		if (entryIndex == NXBasics::IntegerRange::UInt32Maximum)
		{
			return 0;
		}

		return state->entries[entryIndex].fileSize;
	}

	// Original function: FUN_0040688a
	inline std::uint32_t Archive_GetFileOffset(void* archive, const char* fileName)
	{
		const auto* state = static_cast<const SArchiveState*>(archive);

		if (state->entries == nullptr)
		{
			return 0;
		}

		const std::uint32_t entryIndex = Archive_FindFileIndex(archive, fileName);

		if (entryIndex == NXBasics::IntegerRange::UInt32Maximum)
		{
			return 0;
		}

		return state->entries[entryIndex].fileOffset;
	}

	// Original function: FUN_004068b3
	inline std::uint32_t Archive_FindFileIndex(void* archive, const char* fileName)
	{
		const std::uint8_t nameCheck = Archive_CalculateNameCheck(fileName);
		const auto* state = static_cast<const SArchiveState*>(archive);

		for (std::uint32_t index = 0; index < state->entryCount; ++index)
		{
			const auto& entry = state->entries[index];

			if (entry.nameCheck != nameCheck)
			{
				continue;
			}

			if (Runtime_CompareStringsIgnoreCase(fileName, entry.name) == 0)
			{
				return index;
			}
		}

		return NXBasics::IntegerRange::UInt32Maximum;
	}

	// Original function: FUN_00406903
	inline std::uint8_t Archive_CalculateNameCheck(const char* fileName)
	{
		const std::uint32_t lengthBits = static_cast<std::uint32_t>(std::strlen(fileName));

		std::int32_t length;
		std::memcpy(&length, &lengthBits, sizeof(length));

		std::uint8_t result = 0;

		for (std::int32_t index = 0; index < length; ++index)
		{
			const std::uint8_t character = static_cast<std::uint8_t>(fileName[index]);
			const std::int32_t signedCharacter = character < 0x80 ? static_cast<std::int32_t>(character) : static_cast<std::int32_t>(character) - 0x100;
			const std::uint8_t convertedCharacter = static_cast<std::uint8_t>(Runtime_ToLower(signedCharacter));

			result = static_cast<std::uint8_t>(result + convertedCharacter);
		}

		return result;
	}

	inline bool File_OpenForReading(void* file, bool reportErrors, bool searchPaths);

	inline void File_InitObject(void* file);

	// Initialization body of FUN_00405d8b; the native constructor supplies the vtable.
	inline void File_Initialize(void* file, const char* inputFileName, bool openForReading)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);
		auto& fileName = *reinterpret_cast<char (*)[260]>(fileBytes + 0x08);

		File_InitObject(file);

		// The native reset already leaves this last byte zero after the 259-byte copy.
		std::strncpy(fileName, inputFileName, sizeof(fileName) - 1);
		fileName[sizeof(fileName) - 1] = '\0';

		for (std::uint32_t index = 0; index < 5; ++index)
		{
			const auto& record = CFile::mStaticVars.systemPathNames[index];

			if (record.enabled == 0)
			{
				continue;
			}

			if (Runtime_CompareStringsIgnoreCaseN(inputFileName, record.systemPathName, record.nameLength) != 0)
			{
				continue;
			}

			// Preserve first-match selection and the original unchecked concatenation.
			std::strcpy(fileName, record.replacementPath);
			std::strcat(fileName, inputFileName + record.nameLength);
			break;
		}

		std::int32_t length = static_cast<std::int32_t>(std::strlen(fileName));

		// Remove one leading dot and backslash, including moving the terminator.
		if (length > 2 && fileName[0] == '.' && fileName[1] == '\\')
		{
			length -= 2;
			// Windows 00405E3F calls the reconstructed overlap-safe move at 004E71F0.
			Runtime_MoveMemory(fileName, fileName + 2, static_cast<std::uint32_t>(length) + 1u);
		}

		// Prefix removal can reduce the length to one or two; no slash pair is scanned then.
		if (length > 2)
		{
			// Start at index one to preserve an initial UNC backslash pair.
			for (std::int32_t index = 1; index < length - 1; ++index)
			{
				if (fileName[index] != '\\' || fileName[index + 1] != '\\')
				{
					continue;
				}

				for (std::int32_t position = index; position < length; ++position)
				{
					fileName[position] = fileName[position + 1];
				}

				--length;
				--index;
			}
		}

		if (openForReading)
		{
			File_OpenForReading(file, true, true);
		}
	}

	// Original function: FUN_00405ee5; NXBasics::CFile::OpenForReading().
	inline bool File_OpenForReading(void* file, bool reportErrors, bool searchPaths)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);
		auto* fileName = reinterpret_cast<char*>(fileBytes + 0x08);

		const auto getDescriptor = [fileBytes] () -> std::int32_t
		{
			std::int32_t descriptor;
			std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));
			return descriptor;
		};

		const auto storeDescriptor = [fileBytes] (std::int32_t descriptor)
		{
			std::memcpy(fileBytes + 0x04, &descriptor, sizeof(descriptor));
		};

		const auto tryOpen = [&storeDescriptor] (const char* name) -> bool
		{
			const std::int32_t descriptor = Runtime_Open(name, 0x8000u, 0u);
			storeDescriptor(descriptor);
			return descriptor != -1;
		};

		const auto reportFailure = [reportErrors, fileName] () -> bool
		{
			if (reportErrors)
			{
				const auto firstError = *NXBasics::Runtime_GetErrnoPointer();
				const auto errorText = NXBasics::Runtime_GetErrorText(firstError);
				const auto currentError = *NXBasics::Runtime_GetErrnoPointer();

				NXBasics::ReportFatalError(OriginalStrings::FileOpenForReadingError, fileName, currentError, errorText);
			}

			return false;
		};

		if (getDescriptor() != -1)
		{
			File_Close(file);
		}

		if (tryOpen(fileName))
		{
			return true;
		}

		// The original checks only for a colon in the second character.
		if (std::strlen(fileName) >= 2 && fileName[1] == ':')
		{
			return reportFailure();
		}

		const auto getArchiveFileName = [] (void* archive) -> const char*
		{
			const auto* state = static_cast<const SArchiveState*>(archive);
			const auto* fileBytes = reinterpret_cast<const std::uint8_t*>(state->archiveFile);

			return reinterpret_cast<const char*>(fileBytes + 0x08);
		};

		char temporaryName[260];

		if (searchPaths)
		{
			std::strcpy(temporaryName, fileName);

			for (std::uint32_t index = 0; index < 5; ++index)
			{
				const auto& pathRecord = CFile::mStaticVars.additionalLoadPaths[index];

				if (pathRecord.enabled == 0)
				{
					continue;
				}

				const char* prefix = pathRecord.path;

				std::strcpy(fileName, prefix);
				std::strcat(fileName, temporaryName);

				if (tryOpen(fileName))
				{
					// Keep the successfully expanded name.
					return true;
				}
			}

			std::strcpy(fileName, temporaryName);
		}

		std::uint32_t selectedArchiveIndex = 0;

		for (; selectedArchiveIndex < 5; ++selectedArchiveIndex)
		{
			void* archive = CFile::mStaticVars.fileLibraries[selectedArchiveIndex];

			if (archive != nullptr && Archive_IsFileInLibrary(archive, fileName))
			{
				break;
			}
		}

		if (selectedArchiveIndex == 5)
		{
			return reportFailure();
		}

		if (!tryOpen(getArchiveFileName(CFile::mStaticVars.fileLibraries[selectedArchiveIndex])))
		{
			// This fallback runs even when searchPaths is false.
			// The original advances the path and archive indices together.
			for (std::uint32_t index = 0; index < 5; ++index)
			{
				const auto& pathRecord = CFile::mStaticVars.additionalLoadPaths[index];

				if (pathRecord.enabled == 0)
				{
					continue;
				}

				const char* prefix = pathRecord.path;

				std::strcpy(temporaryName, prefix);
				std::strcat(temporaryName, getArchiveFileName(CFile::mStaticVars.fileLibraries[index]));

				if (tryOpen(temporaryName))
				{
					break;
				}
			}

			if (getDescriptor() == -1)
			{
				return reportFailure();
			}
		}

		// Use the originally selected archive for the entry offset.
		const auto entryOffset = Archive_GetFileOffset(CFile::mStaticVars.fileLibraries[selectedArchiveIndex], fileName);
		File_SeekToPosition(file, entryOffset);

		std::memcpy(fileBytes + 0x10C, &selectedArchiveIndex, sizeof(selectedArchiveIndex));
		return true;
	}

	// Original function: FUN_004061bc; NXBasics::CFile::Write().
	inline void File_Write(void* file, const void* data, std::uint32_t byteCount)
	{
		const auto* fileBytes = static_cast<const std::uint8_t*>(file);

		std::int32_t descriptor;
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		if (descriptor == -1)
		{
			return;
		}

		const std::int32_t written = Runtime_Write(static_cast<std::uint32_t>(descriptor), data, byteCount);

		// The original compares all 32 bits of the result.
		if (static_cast<std::uint32_t>(written) == byteCount)
		{
			return;
		}

		const auto firstError = *NXBasics::Runtime_GetErrnoPointer();
		const auto errorText = NXBasics::Runtime_GetErrorText(firstError);

		// Read the error value again after obtaining its description.
		const auto currentError = *NXBasics::Runtime_GetErrnoPointer();
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		NXBasics::ReportFatalError("NXBasics::CFile::Write\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
	}

	// Original function: FUN_004063c0; NXBasics::CFile::l_InitObject().
	inline void File_InitObject(void* file)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);

		std::memset(fileBytes + 0x04, 0, 268);

		const std::int32_t invalidIndex = -1;
		std::memcpy(fileBytes + 0x04, &invalidIndex, sizeof(invalidIndex));
		std::memcpy(fileBytes + 0x10C, &invalidIndex, sizeof(invalidIndex));
	}

	static_assert(sizeof(void*) == 4, "These file accesses require an x86 build");
}
