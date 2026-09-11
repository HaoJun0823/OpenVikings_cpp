#pragma once

#include <cstddef>
#include <cstdint>

namespace NXSysKeyManager
{
	class CKeyEntryListBase;
}

namespace NXBasics
{
	class CStringArray;

	class CIniFileReader
	{
	public:
		using ErrorCallback = void(__cdecl*)(std::int32_t, const char*, const char*);

		// Original function: FUN_00424df8
		CIniFileReader(const char* fileName, const char* preliminaryFileName, bool createCompiledFile, const char* compiledExtension, ErrorCallback errorCallback);

		// Original function: FUN_00424f6c
		~CIniFileReader();

		CIniFileReader(const CIniFileReader&) = delete;
		CIniFileReader& operator=(const CIniFileReader&) = delete;

		// Original function: FUN_00424f7e
		char* GetFirstLine();

		// Original function: FUN_00424fa7
		char* GetNextLine();

		// Original function: FUN_0042538d
		std::int32_t GetTypeOfNextParameter();

		// Original function: FUN_00425291
		std::int32_t GetInt();

		// Original function: FUN_0042522e
		char* GetString();

		// Field accessors for the property-manager path.
		bool IsLoaded() const;
		std::int32_t GetCurrentLineType() const;
		const char* GetCurrentName() const;

	private:
		// Original function: FUN_004258eb
		void l_CopyCurrentLineIntoBuffer();

		// Original function: FUN_004253e0
		void l_LoadIniFile(const char* fileName, const char* preliminaryFileName);

		// Original function: FUN_00425429
		void l_DoLoadIniFile(const char* fileName, NXSysKeyManager::CKeyEntryListBase& definitions);

		// Original function: FUN_00425980
		void l_CreateCompiledFilename(char* fileName, const char* compiledExtension);

		std::uint8_t mLoaded;           // +0x000
		char mFileName[0x104];          // +0x001
		std::byte mPadding105[3];       // +0x105
		CStringArray* mLines;           // +0x108
		std::uint32_t mCurrentLine;     // +0x10C
		std::int32_t mCurrentLineType;  // +0x110
		std::uint32_t mUnknown114;      // +0x114
		char* mParameterPosition;      // +0x118
		char mLineBuffer[2000];         // +0x11C
		char mNameBuffer[2000];         // +0x8EC
		char mStringBuffer[2000];       // +0x10BC
		ErrorCallback mErrorCallback;  // +0x188C
	};

	static_assert(sizeof(void*) == 4, "CIniFileReader requires an x86 build");
	static_assert(sizeof(CIniFileReader) == 0x1890, "CIniFileReader must be 0x1890 bytes");
	static_assert(alignof(CIniFileReader) == 4, "CIniFileReader requires four-byte alignment");
}