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