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