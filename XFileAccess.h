#pragma once

#include "XCrtFileAccess.h"

#include <cstdint>
#include <cstring>

namespace NXBasics
{
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
			const auto firstError = *FUN_004e75e8();
			const auto errorText = FUN_004e7525(firstError);

			// Read errno again after obtaining its description.
			const auto currentError = *FUN_004e75e8();
			const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

			FUN_00406935(DAT_004ff0c8, fileName, currentError, errorText);
		}

		const std::int32_t invalidIndex = -1;
		std::memcpy(fileBytes + 0x04, &invalidIndex, sizeof(invalidIndex));
		std::memcpy(fileBytes + 0x10C, &invalidIndex, sizeof(invalidIndex));
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

			FUN_004e5c20(fileName, replacement);
			FUN_004e5c30(fileName, inputFileName + prefixLength);
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
					const auto firstError = *FUN_004e75e8();
					const auto errorText = FUN_004e7525(firstError);
					const auto currentError = *FUN_004e75e8();

					FUN_00406935(DAT_004ff058, fileName, currentError, errorText);
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
			FUN_004e5c20(temporaryName, fileName);

			for (std::uint32_t index = 0; index < 5; ++index)
			{
				const auto* pathRecord = pathRecords + index * 0x105u;

				if (pathRecord[0] == 0)
				{
					continue;
				}

				const auto* prefix = reinterpret_cast<const char*>(pathRecord + 1);

				FUN_004e5c20(fileName, prefix);
				FUN_004e5c30(fileName, temporaryName);

				if (tryOpen(fileName))
				{
					// Keep the successfully expanded name.
					return true;
				}
			}

			FUN_004e5c20(fileName, temporaryName);
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

				FUN_004e5c20(temporaryName, prefix);
				FUN_004e5c30(temporaryName, getArchiveFileName(getArchive(index)));

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
		const auto entryOffset = FUN_0040688a(getArchive(selectedArchiveIndex), fileName);
		FUN_00406286(file, entryOffset);

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

		const auto firstError = *FUN_004e75e8();
		const auto errorText = FUN_004e7525(firstError);

		// Read the error value again after obtaining its description.
		const auto currentError = *FUN_004e75e8();
		const auto* fileName = reinterpret_cast<const char*>(fileBytes + 0x08);

		FUN_00406935("NXBasics::CFile::Write\n\nfile %s - errno %d: %s", fileName, currentError, errorText);
	}

	static_assert(sizeof(void*) == 4, "These file accesses require an x86 build");
}