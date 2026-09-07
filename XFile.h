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

		// Original function: FUN_004061bc
		void Write(const void* data, std::uint32_t byteCount);

		// Original function: FUN_00406126
		void Close();

	private:
		std::int32_t mDescriptor;    // +0x004
		char mFileName[0x104];       // +0x008
		std::int32_t mArchiveIndex;  // +0x10C
	};

	static_assert(sizeof(void*) == 4, "CFile requires an x86 build");
	static_assert(sizeof(CFile) == 0x110, "CFile must be 272 bytes");
}