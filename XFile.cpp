#include "XFile.h"
#include "XFileAccess.h"

#include <new>

namespace NXBasics
{
	// Original function: FUN_00405d8b
	CFile::CFile(const char* fileName, bool openForReading)
	{
		File_Initialize(this, fileName, openForReading);
	}

	// Original function: FUN_00405eda; deleting wrapper: FUN_00405eb8.
	CFile::~CFile()
	{
		Close();
	}

	// Original function: FUN_00405ee5
	bool CFile::OpenForReading(bool reportErrors, bool searchPaths)
	{
		return File_OpenForReading(this, reportErrors, searchPaths);
	}

	// Original function: FUN_004060ca
	bool CFile::OpenForWriting(bool reportError)
	{
		Close();

		mDescriptor = FUN_004e679e(mFileName, 0x8301, 0x180);

		if (mDescriptor != -1)
		{
			return true;
		}

		if (reportError)
		{
			const auto errorForText = *Runtime_GetErrnoPointer();
			const auto errorText = FUN_004e7525(errorForText);

			// Read errno again after obtaining its description.
			const auto currentError = *Runtime_GetErrnoPointer();

			FUN_00406935(reinterpret_cast<const char*>(&DAT_004ff090), mFileName, currentError, errorText);
		}

		return false;
	}

	// Original function: FUN_00406126
	void CFile::Close()
	{
		File_Close(this);
	}

	// Original function: FUN_00406170
	void CFile::Read(void* destination, std::uint32_t byteCount)
	{
		File_Read(this, destination, byteCount);
	}

	// Original function: FUN_004062ee
	std::uint32_t CFile::GetPosition() const
	{
		return File_GetPosition(this);
	}

	// Original function: FUN_00406286
	void CFile::SeekToPosition(std::uint32_t position)
	{
		File_SeekToPosition(this, position);
	}

	// Original function: FUN_0040635a
	std::uint32_t CFile::GetSize() const
	{
		return File_GetSize(this);
	}

	// Original function: FUN_004063f9
	std::int32_t CFile::FileSystem_SystemPathName_Set(const char* systemPathName, const char* replacementPath)
	{
		auto* records = reinterpret_cast<std::uint8_t*>(&DAT_0050f758);
		const std::int32_t existingIndex = FileSystem_SystemPathName_GetId(systemPathName);

		if (existingIndex != -1)
		{
			auto* record = records + static_cast<std::uint32_t>(existingIndex) * 0x210u;
			std::strcpy(reinterpret_cast<char*>(record + 0x10C), replacementPath);

			return existingIndex;
		}

		for (std::uint32_t index = 0; index < 5; ++index)
		{
			auto* record = records + index * 0x210u;

			if (record[0] != 0)
			{
				continue;
			}

			std::strcpy(reinterpret_cast<char*>(record + 0x01), systemPathName);
			std::strcpy(reinterpret_cast<char*>(record + 0x10C), replacementPath);

			const std::uint32_t nameLength = static_cast<std::uint32_t>(std::strlen(systemPathName));
			std::memcpy(record + 0x108, &nameLength, sizeof(nameLength));

			record[0] = 1;

			return static_cast<std::int32_t>(index);
		}

		return -1;
	}

	// Original function: FUN_0040648a
	bool CFile::FileSystem_SystemPathName_Clear(const char* systemPathName)
	{
		const std::int32_t index = FileSystem_SystemPathName_GetId(systemPathName);

		if (index == -1)
		{
			return false;
		}

		auto* records = reinterpret_cast<std::uint8_t*>(&DAT_0050f758);
		records[static_cast<std::uint32_t>(index) * 0x210u] = 0;

		return true;
	}

	// Original function: FUN_004064ac
	std::int32_t CFile::FileSystem_SystemPathName_GetId(const char* systemPathName)
	{
		const auto* records = reinterpret_cast<const std::uint8_t*>(&DAT_0050f758);

		for (std::int32_t index = 0; index < 5; ++index)
		{
			const auto* record = records + static_cast<std::uint32_t>(index) * 0x210u;

			if (record[0] == 0)
			{
				continue;
			}

			const auto* storedName = reinterpret_cast<const char*>(record + 0x01);

			if (Runtime_CompareStringsIgnoreCase(storedName, systemPathName) == 0)
			{
				return index;
			}
		}

		return -1;
	}

	// Original function: FUN_0040659c
	bool CFile::FileSystem_Tool_FileExists(const char* fileName, bool searchPaths)
	{
		CFile file(fileName, false);

		return File_OpenForReading(&file, false, searchPaths);
	}

	// Original function: FUN_00406542
	bool CFile::FileSystem_Tool_GetRealFilename(const char* fileName, char* destination)
	{
		CFile file(fileName, false);

		if (!File_OpenForReading(&file, false, true))
		{
			return false;
		}

		const auto* resolvedName = reinterpret_cast<const char*>(static_cast<const void*>(&file)) + 0x08;
		std::strcpy(destination, resolvedName);

		return true;
	}

	// Original function: FUN_004061bc
	void CFile::Write(const void* data, std::uint32_t byteCount)
	{
		File_Write(this, data, byteCount);
	}

	// Original function: FUN_004e5e48
	void CFile::operator delete(void* allocation) noexcept
	{
		::operator delete(allocation);
	}
}