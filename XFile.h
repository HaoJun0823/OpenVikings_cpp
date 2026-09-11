#pragma once

#include <cstdint>

namespace NXBasics
{
	class CFile
	{
	public:
		// Original function: FUN_00405d8b
		CFile(const char* fileName, bool openForReading);

		// Original function: FUN_00405eda; deleting wrapper: FUN_00405eb8.
		virtual ~CFile();

		// Original function: FUN_004e5e48
		static void operator delete(void* allocation) noexcept;

		// Original function: FUN_00405ee5
		bool OpenForReading(bool reportErrors, bool searchPaths);

		// Original function: FUN_004060ca
		bool OpenForWriting(bool reportError);

		// Original function: FUN_004061bc
		void Write(const void* data, std::uint32_t byteCount);

		// Original function: FUN_00406126
		void Close();

		// Original function: FUN_00406170
		void Read(void* destination, std::uint32_t byteCount);

		// Original function: FUN_004062ee
		std::uint32_t GetPosition() const;

		// Original function: FUN_00406286
		void SeekToPosition(std::uint32_t position);

		// Original function: FUN_0040635a
		std::uint32_t GetSize() const;

		// Original function: FUN_004063f9
		static std::int32_t FileSystem_SystemPathName_Set(const char* systemPathName, const char* replacementPath);

		// Original function: FUN_0040648a
		static bool FileSystem_SystemPathName_Clear(const char* systemPathName);

		// Original function: FUN_004064ac
		static std::int32_t FileSystem_SystemPathName_GetId(const char* systemPathName);

		// Original function: FUN_0040659c
		static bool FileSystem_Tool_FileExists(const char* fileName, bool searchPaths);

		// Original function: FUN_00406542
		static bool FileSystem_Tool_GetRealFilename(const char* fileName, char* destination);

	private:
		std::int32_t mDescriptor;    // +0x004
		char mFileName[0x104];       // +0x008
		std::int32_t mArchiveIndex;  // +0x10C
	};

	static_assert(sizeof(void*) == 4, "CFile requires an x86 build");
	static_assert(sizeof(CFile) == 0x110, "CFile must be 272 bytes");
}