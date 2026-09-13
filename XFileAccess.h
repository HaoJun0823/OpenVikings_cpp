#pragma once

#include "OriginalStrings.h"
#include "XCrtFileAccess.h"
#include "XDiagnostics.h"

#include <cstdint>
#include <cstring>

namespace NXBasics
{
	// Original function: FUN_00406286
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

		if (static_cast<std::uint32_t>(result) == 0xFFFFFFFFu)
		{
			const auto firstError = *NXBasics::Runtime_GetErrnoPointer();
			const auto errorText = FUN_004e7525(firstError);

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
		const auto errorText = FUN_004e7525(firstError);
		const auto currentError = *Runtime_GetErrnoPointer();
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		NXBasics::ReportFatalError("NXBasics::CFile::Read\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
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

		if (position == UINT32_C(0xFFFFFFFF))
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = FUN_004e7525(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::Position\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}

		std::uint32_t archiveIndex;
		std::memcpy(&archiveIndex, fileBytes + 0x10C, sizeof(archiveIndex));

		if (archiveIndex != UINT32_C(0xFFFFFFFF))
		{
			const auto* archiveSlots = reinterpret_cast<const std::uint8_t*>(&DAT_005106c4);

			void* archive;
			std::memcpy(&archive, archiveSlots + archiveIndex * 4u, sizeof(archive));

			const std::uint32_t entryOffset = Archive_GetFileOffset(archive, fileName);
			position -= entryOffset;
		}

		return position;
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

		if (archiveIndex != UINT32_C(0xFFFFFFFF))
		{
			const auto* archiveSlots = reinterpret_cast<const std::uint8_t*>(&DAT_005106c4);

			void* archive;
			std::memcpy(&archive, archiveSlots + archiveIndex * 4u, sizeof(archive));

			const std::uint32_t entryOffset = Archive_GetFileOffset(archive, fileName);
			position += entryOffset;
		}

		// Reload the descriptor after resolving the archive offset.
		std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));

		std::int32_t seekOffset;
		std::memcpy(&seekOffset, &position, sizeof(seekOffset));

		const std::uint32_t result = Runtime_Seek(static_cast<std::uint32_t>(descriptor), seekOffset, 0);

		if (result == UINT32_C(0xFFFFFFFF))
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = FUN_004e7525(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::SeekToPosition\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}
	}

	// Original function: FUN_00406861
	inline std::uint32_t Archive_GetFileSize(void* archive, const char* fileName)
	{
		const auto* archiveBytes = static_cast<const std::uint8_t*>(archive);

		const std::uint8_t* entries;
		std::memcpy(&entries, archiveBytes + 0x04, sizeof(entries));

		if (entries == nullptr)
		{
			return 0;
		}

		const std::uint32_t entryIndex = Archive_FindFileIndex(archive, fileName);

		if (entryIndex == UINT32_C(0xFFFFFFFF))
		{
			return 0;
		}

		// Reload the entry table after the lookup.
		std::memcpy(&entries, archiveBytes + 0x04, sizeof(entries));

		std::uint32_t size;
		std::memcpy(&size, entries + entryIndex * 0x10u + 0x0C, sizeof(size));

		return size;
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

		if (archiveIndex != UINT32_C(0xFFFFFFFF))
		{
			const auto* archiveSlots = reinterpret_cast<const std::uint8_t*>(&DAT_005106c4);

			void* archive;
			std::memcpy(&archive, archiveSlots + archiveIndex * std::uint32_t{ 4 }, sizeof(archive));

			return Archive_GetFileSize(archive, fileName);
		}

		const std::uint32_t size = Runtime_GetFileSize(static_cast<std::uint32_t>(descriptor));

		if (size == UINT32_C(0xFFFFFFFF))
		{
			const auto firstError = *Runtime_GetErrnoPointer();
			const auto errorText = FUN_004e7525(firstError);
			const auto currentError = *Runtime_GetErrnoPointer();

			NXBasics::ReportFatalError("NXBasics::CFile::Size\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
		}

		return size;
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

	// Original function: FUN_004068b3
	inline std::uint32_t Archive_FindFileIndex(void* archive, const char* fileName)
	{
		const std::uint8_t nameCheck = Archive_CalculateNameCheck(fileName);
		const auto* archiveBytes = static_cast<const std::uint8_t*>(archive);

		for (std::uint32_t index = 0;; ++index)
		{
			std::uint32_t entryCount;
			std::memcpy(&entryCount, archiveBytes, sizeof(entryCount));

			if (index >= entryCount)
			{
				return UINT32_C(0xFFFFFFFF);
			}

			const std::uint8_t* entries;
			std::memcpy(&entries, archiveBytes + 0x04, sizeof(entries));

			const std::uint8_t* entry = entries + index * 0x10u;

			if (entry[0x04] != nameCheck)
			{
				continue;
			}

			const char* storedName;
			std::memcpy(&storedName, entry, sizeof(storedName));

			if (Runtime_CompareStringsIgnoreCase(fileName, storedName) == 0)
			{
				return index;
			}
		}
	}

	// Original function: FUN_0040688a
	inline std::uint32_t Archive_GetFileOffset(void* archive, const char* fileName)
	{
		const auto* archiveBytes = static_cast<const std::uint8_t*>(archive);

		const std::uint8_t* entries;
		std::memcpy(&entries, archiveBytes + 0x04, sizeof(entries));

		if (entries == nullptr)
		{
			return 0;
		}

		const std::uint32_t entryIndex = Archive_FindFileIndex(archive, fileName);

		if (entryIndex == UINT32_C(0xFFFFFFFF))
		{
			return 0;
		}

		std::memcpy(&entries, archiveBytes + 0x04, sizeof(entries));

		std::uint32_t offset;
		std::memcpy(&offset, entries + entryIndex * 0x10u + 0x08, sizeof(offset));

		return offset;
	}

	inline bool File_OpenForReading(void* file, bool reportErrors, bool searchPaths);

	// Original function: FUN_004063c0; NXBasics::CFile::l_InitObject().
	inline void File_InitObject(void* file)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);

		std::memset(fileBytes + 0x04, 0, 0x10C);

		const std::int32_t invalidIndex = -1;
		std::memcpy(fileBytes + 0x04, &invalidIndex, sizeof(invalidIndex));
		std::memcpy(fileBytes + 0x10C, &invalidIndex, sizeof(invalidIndex));
	}

	// Initialization body of FUN_00405d8b; the native constructor supplies the vtable.
	inline void File_Initialize(void* file, const char* inputFileName, bool openForReading)
	{
		auto* fileBytes = static_cast<std::uint8_t*>(file);
		auto* fileName = reinterpret_cast<char*>(fileBytes + 0x08);

		File_InitObject(file);

		// The original does not explicitly append a terminator here.
		std::strncpy(fileName, inputFileName, 0x103);

		const auto* replacementRecords = reinterpret_cast<const std::uint8_t*>(&DAT_0050f758);

		for (std::uint32_t index = 0; index < 5; ++index)
		{
			const auto* record = replacementRecords + index * 0x210u;

			if (record[0] == 0)
			{
				continue;
			}

			std::uint32_t prefixLength;
			std::memcpy(&prefixLength, record + 0x108, sizeof(prefixLength));

			const auto* prefix = reinterpret_cast<const char*>(record + 0x01);

			if (FUN_004e6f80(inputFileName, prefix, prefixLength) != 0)
			{
				continue;
			}

			const auto* replacement = reinterpret_cast<const char*>(record + 0x10C);

			std::strcpy(fileName, replacement);
			std::strcat(fileName, inputFileName + prefixLength);
			break;
		}

		std::int32_t length = static_cast<std::int32_t>(std::strlen(fileName));

		if (length > 2)
		{
			// Remove one leading dot and backslash, including moving the terminator.
			if (fileName[0] == '.' && fileName[1] == '\\')
			{
				length -= 2;
				std::memmove(fileName, fileName + 2, static_cast<std::size_t>(length) + 1);
			}

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

		const auto getDescriptor = [fileBytes]() -> std::int32_t
			{
				std::int32_t descriptor;
				std::memcpy(&descriptor, fileBytes + 0x04, sizeof(descriptor));
				return descriptor;
			};

		const auto storeDescriptor = [fileBytes](std::int32_t descriptor)
			{
				std::memcpy(fileBytes + 0x04, &descriptor, sizeof(descriptor));
			};

		const auto tryOpen = [&storeDescriptor](const char* name) -> bool
			{
				const std::int32_t descriptor = Runtime_Open(name, 0x8000u, 0u);
				storeDescriptor(descriptor);
				return descriptor != -1;
			};

		const auto reportFailure = [reportErrors, fileName]() -> bool
			{
				if (reportErrors)
				{
					const auto firstError = *NXBasics::Runtime_GetErrnoPointer();
					const auto errorText = FUN_004e7525(firstError);
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

		// Each search-path record contains an enabled byte and a path string.
		const auto* pathRecords = reinterpret_cast<const std::uint8_t*>(&DAT_005101a8);
		const auto* archiveSlots = reinterpret_cast<const std::uint8_t*>(&DAT_005106c4);

		const auto getArchive = [archiveSlots](std::uint32_t index) -> void*
			{
				void* archive;
				std::memcpy(&archive, archiveSlots + index * 4u, sizeof(archive));
				return archive;
			};

		const auto getArchiveFileName = [](void* archive) -> const char*
			{
				const auto* archiveBytes = static_cast<const std::uint8_t*>(archive);

				void* archiveFile;
				std::memcpy(&archiveFile, archiveBytes + 0x10, sizeof(archiveFile));

				return reinterpret_cast<const char*>(static_cast<const std::uint8_t*>(archiveFile) + 0x08);
			};

		char temporaryName[0x104];

		if (searchPaths)
		{
			std::strcpy(temporaryName, fileName);

			for (std::uint32_t index = 0; index < 5; ++index)
			{
				const auto* pathRecord = pathRecords + index * 0x105u;

				if (pathRecord[0] == 0)
				{
					continue;
				}

				const auto* prefix = reinterpret_cast<const char*>(pathRecord + 1);

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
			void* archive = getArchive(selectedArchiveIndex);

			if (archive != nullptr && FUN_00406841(archive, fileName))
			{
				break;
			}
		}

		if (selectedArchiveIndex == 5)
		{
			return reportFailure();
		}

		if (!tryOpen(getArchiveFileName(getArchive(selectedArchiveIndex))))
		{
			// This fallback runs even when searchPaths is false.
			// The original advances the path and archive indices together.
			for (std::uint32_t index = 0; index < 5; ++index)
			{
				const auto* pathRecord = pathRecords + index * 0x105u;

				if (pathRecord[0] == 0)
				{
					continue;
				}

				const auto* prefix = reinterpret_cast<const char*>(pathRecord + 1);

				std::strcpy(temporaryName, prefix);
				std::strcat(temporaryName, getArchiveFileName(getArchive(index)));

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
		const auto entryOffset = Archive_GetFileOffset(getArchive(selectedArchiveIndex), fileName);
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
		const auto errorText = FUN_004e7525(firstError);

		// Read the error value again after obtaining its description.
		const auto currentError = *NXBasics::Runtime_GetErrnoPointer();
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		NXBasics::ReportFatalError("NXBasics::CFile::Write\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
	}

	static_assert(sizeof(void*) == 4, "These file accesses require an x86 build");
}