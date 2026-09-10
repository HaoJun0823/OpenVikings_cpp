#pragma once

#include "XFile.h"
#include "XCrtFileAccess.h"
#include "XMemory.h"
#include "XMKeyManager.h"
#include "XStorable.h"
#include "XStringArray.h"

#include <new>

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NXBasics
{
	namespace IniFileReaderDetail
	{
		static_assert(sizeof(void*) == 4, "Original reader access requires an x86 build");

		template<typename T>
		inline T ReadField(const void* object, std::size_t offset)
		{
			T value;
			std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
			return value;
		}

		template<typename T>
		inline void WriteField(void* object, std::size_t offset, const T& value)
		{
			std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
		}
	}

	// Original function: FUN_004258eb
	inline void IniFileReader_CopyCurrentLineIntoBuffer(void* reader)
	{
		using namespace IniFileReaderDetail;

		CStringArray* lines = ReadField<CStringArray*>(reader, 0x108);
		const std::uint32_t lineIndex = ReadField<std::uint32_t>(reader, 0x10C);
		const char* storedLine = lines->GetStringPtr(lineIndex);

		const std::uint8_t typeByte = static_cast<std::uint8_t>(storedLine[0]);
		const std::int32_t lineType = typeByte < 0x80 ? static_cast<std::int32_t>(typeByte) : static_cast<std::int32_t>(typeByte) - 0x100;

		WriteField(reader, 0x110, lineType);

		char* lineBuffer = reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x11C);
		char* nameBuffer = reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x8EC);

		std::strcpy(lineBuffer, storedLine + 1);

		char* separator = std::strchr(lineBuffer, ' ');

		if (separator == nullptr)
		{
			std::strcpy(nameBuffer, lineBuffer);

			char* parameterPosition = lineBuffer + std::strlen(lineBuffer);
			WriteField(reader, 0x118, parameterPosition);
			return;
		}

		const std::size_t nameLength = static_cast<std::size_t>(separator - lineBuffer);

		std::strncpy(nameBuffer, lineBuffer, nameLength);
		nameBuffer[nameLength] = '\0';

		char* parameterPosition = separator + 1;
		WriteField(reader, 0x118, parameterPosition);
	}

	// Original function: FUN_00424f7e
	inline char* IniFileReader_GetFirstLine(void* reader)
	{
		using namespace IniFileReaderDetail;

		const void* lines = ReadField<const void*>(reader, 0x108);

		if (ReadField<std::uint32_t>(lines, 0x08) == 0)
		{
			return nullptr;
		}

		WriteField(reader, 0x10C, std::uint32_t{ 0 });
		IniFileReader_CopyCurrentLineIntoBuffer(reader);

		return reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x11C);
	}

	// Original function: FUN_00424fa7
	inline char* IniFileReader_GetNextLine(void* reader)
	{
		using namespace IniFileReaderDetail;

		const void* lines = ReadField<const void*>(reader, 0x108);
		const std::uint32_t lineCount = ReadField<std::uint32_t>(lines, 0x08);

		if (lineCount == 0)
		{
			return nullptr;
		}

		const std::uint32_t nextIndex = ReadField<std::uint32_t>(reader, 0x10C) + std::uint32_t{ 1 };

		if (nextIndex == lineCount)
		{
			return nullptr;
		}

		if (static_cast<const CStringArray*>(lines)->GetStringPtr(nextIndex) == nullptr)
		{
			return nullptr;
		}

		WriteField(reader, 0x10C, nextIndex);
		IniFileReader_CopyCurrentLineIntoBuffer(reader);

		return reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x11C);
	}

	// Original function: FUN_0042538d
	inline std::int32_t IniFileReader_GetTypeOfNextParameter(void* reader)
	{
		using namespace IniFileReaderDetail;

		for (;;)
		{
			char* position = ReadField<char*>(reader, 0x118);
			const char character = *position;

			if (character == '\0')
			{
				return 0;
			}

			if (character == '"')
			{
				return 2;
			}

			if (character == '-' || character == '.')
			{
				return 3;
			}

			if (Runtime_IsDigit(character))
			{
				return 3;
			}

			if (character != ' ')
			{
				return 1;
			}

			++position;
			WriteField(reader, 0x118, position);
		}
	}

	// Original function: FUN_00425291
	inline std::int32_t IniFileReader_GetInt(void* reader)
	{
		using namespace IniFileReaderDetail;

		char* position = ReadField<char*>(reader, 0x118);

		while (*position != '\0')
		{
			if (*position == '-' || Runtime_IsDigit(*position))
			{
				break;
			}

			++position;
		}

		if (*position == '\0')
		{
			return 0;
		}

		const std::int32_t value = Runtime_StringToInteger(position);
		char* separator = std::strchr(position, ' ');
		char* nextPosition;

		if (separator != nullptr)
		{
			nextPosition = separator + 1;
		}
		else
		{
			char* lineBuffer = reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x11C);
			nextPosition = lineBuffer + std::strlen(lineBuffer);
		}

		WriteField(reader, 0x118, nextPosition);

		return value;
	}

	// Original function: FUN_0042522e
	inline char* IniFileReader_GetString(void* reader)
	{
		using namespace IniFileReaderDetail;

		char* position = ReadField<char*>(reader, 0x118);
		char* openingQuote = std::strchr(position, '"');

		if (openingQuote == nullptr)
		{
			return nullptr;
		}

		char* stringStart = openingQuote + 1;
		char* closingQuote = std::strchr(stringStart, '"');

		if (closingQuote == nullptr)
		{
			return nullptr;
		}

		const std::size_t stringLength = static_cast<std::size_t>(closingQuote - stringStart);
		char* stringBuffer = reinterpret_cast<char*>(static_cast<std::byte*>(reader) + 0x10BC);

		std::strncpy(stringBuffer, stringStart, stringLength);
		stringBuffer[stringLength] = '\0';

		char* nextPosition = closingQuote + 1;
		WriteField(reader, 0x118, nextPosition);

		return stringBuffer;
	}

	struct SIniFileReaderDefine
	{
		char name[100];
		char replacement[200];
	};

	static_assert(sizeof(SIniFileReaderDefine) == 0x12C);
	static_assert(offsetof(SIniFileReaderDefine, replacement) == 0x64);

	// Original function: FUN_00425429
	inline void IniFileReader_DoLoadIniFile(void* reader, const char* fileName, NXSysKeyManager::CKeyEntryListBase& definitions)
	{
		using namespace IniFileReaderDetail;

		CMemory* memory;

		{
			CFile file(fileName, true);
			const std::uint32_t fileSize = file.GetSize();

			memory = new (std::nothrow) CMemory(fileSize + std::uint32_t{ 1 }, std::uint8_t{ 0 });

			char* destination = ReadField<char*>(memory, 0x08);
			file.Read(destination, fileSize);
		}

		char* fileData = ReadField<char*>(memory, 0x08);

		char rawLine[2000];
		char normalizedLine[2000];
		char storedLine[2000];
		char defineName[2000];
		char includedFileName[260];

		char* cursor = fileData;

		while (*cursor != '\0')
		{
			while (*cursor == '\n' || *cursor == '\r' || *cursor == ' ' || *cursor == '\t')
			{
				++cursor;
			}

			if (*cursor == '\0')
			{
				break;
			}

			char* lineStart = cursor;

			while (*cursor != '\0' && *cursor != '\n' && *cursor != '\r')
			{
				++cursor;
			}

			std::size_t lineLength = static_cast<std::size_t>(cursor - lineStart);

			if (lineLength >= 0x7CE)
			{
				lineLength = 0x7CE;
			}

			std::memcpy(rawLine, lineStart, lineLength);
			rawLine[lineLength] = '\0';

			bool insideQuotes = false;
			bool pendingSpace = false;
			char* output = normalizedLine;

			for (const char* input = rawLine; *input != '\0'; ++input)
			{
				char character = *input;

				if (character == '"')
				{
					insideQuotes = !insideQuotes;
				}

				if (!insideQuotes && (character == '=' || character == ','))
				{
					character = ' ';
				}

				if (!insideQuotes && (character == '\t' || character == ' '))
				{
					pendingSpace = true;
					continue;
				}

				if (pendingSpace)
				{
					*output++ = ' ';
				}

				*output++ = character;
				pendingSpace = false;
			}

			*output = '\0';

			char* comment = std::strstr(normalizedLine, "//");

			if (comment != nullptr)
			{
				*comment = '\0';
			}

			if (normalizedLine[0] == '\0')
			{
				continue;
			}

			if (normalizedLine[0] == '#')
			{
				if (std::strncmp(normalizedLine + 1, "include", 7) == 0)
				{
					char* openingQuote = std::strchr(normalizedLine, '"');

					if (openingQuote == nullptr)
					{
						continue;
					}

					char* includedName = openingQuote + 1;
					char* closingQuote = std::strchr(includedName, '"');

					if (closingQuote == nullptr)
					{
						continue;
					}

					const std::size_t nameLength = static_cast<std::size_t>(closingQuote - includedName);

					std::memcpy(includedFileName, includedName, nameLength);
					includedFileName[nameLength] = '\0';

					IniFileReader_DoLoadIniFile(reader, includedFileName, definitions);
					continue;
				}

				if (std::strncmp(normalizedLine + 1, "define", 6) != 0)
				{
					continue;
				}

				char* input = normalizedLine + 8;

				if (!Runtime_IsPropertyNameCharacter(*input))
				{
					continue;
				}

				auto* definition = new (std::nothrow) SIniFileReaderDefine;
				std::memset(definition, 0, sizeof(*definition));

				char* nameOutput = definition->name;
				char character;

				for (;;)
				{
					character = *input++;

					if (character == ' ' || character == '\0')
					{
						break;
					}

					*nameOutput++ = character;
				}

				*nameOutput = '\0';

				if (character != '\0')
				{
					bool replacementInsideQuotes = false;
					char* replacementOutput = definition->replacement;

					for (;;)
					{
						character = *input++;

						if (character == '\0')
						{
							break;
						}

						if (character == '"')
						{
							replacementInsideQuotes = !replacementInsideQuotes;
						}

						if (character == ' ' && !replacementInsideQuotes)
						{
							break;
						}

						*replacementOutput++ = character;
					}

					// The original ultimately writes a terminator here,
					// even when the replacement contains an unmatched quote.
					*replacementOutput = '\0';
				}

				definitions.PushBack(definition);
				continue;
			}

			if (normalizedLine[0] == '[')
			{
				normalizedLine[0] = '\x01';

				char* closingBracket = std::strchr(normalizedLine + 1, ']');

				if (closingBracket == nullptr)
				{
					continue;
				}

				*closingBracket = '\0';

				CStringArray* lines = ReadField<CStringArray*>(reader, 0x108);
				lines->InsertString(normalizedLine);
				continue;
			}

			if (!Runtime_IsLetter(normalizedLine[0]))
			{
				continue;
			}

			storedLine[0] = '\x02';

			char* input = normalizedLine;
			char* storedOutput = storedLine + 1;
			bool hasName = false;

			while (Runtime_IsPropertyNameCharacter(*input))
			{
				*storedOutput++ = *input++;
				hasName = true;
			}

			if (!hasName)
			{
				continue;
			}

			for (;;)
			{
				char character = *input++;

				if (character == '\0')
				{
					break;
				}

				if (character == ' ')
				{
					continue;
				}

				if (character == '"')
				{
					*storedOutput++ = ' ';
					*storedOutput++ = '"';

					for (;;)
					{
						character = *input++;

						if (character == '\0' || character == '"')
						{
							break;
						}

						*storedOutput++ = character;
					}

					*storedOutput++ = '"';
				}
				else if (character == '-' || character == '+' || character == '.' || Runtime_IsDigit(character))
				{
					*storedOutput++ = ' ';

					for (;;)
					{
						*storedOutput++ = character;
						character = *input++;

						if (!Runtime_IsDigit(character) && character != '.')
						{
							break;
						}

						if (character == '\0')
						{
							break;
						}
					}

					// The first character after the number has already
					// been consumed and is not processed again.
				}
				else if (character == '#')
				{
					char* nameOutput = defineName;

					for (;;)
					{
						character = *input++;

						if (character == ' ' || character == '\0')
						{
							break;
						}

						*nameOutput++ = character;
					}

					*nameOutput = '\0';

					SIniFileReaderDefine* matchingDefinition = nullptr;
					NXSysKeyManager::CKeyEntryListBase::Iterator iterator(definitions);

					for (auto* definition = static_cast<SIniFileReaderDefine*>(iterator.First()); definition != nullptr; definition = static_cast<SIniFileReaderDefine*>(iterator.Next()))
					{
						if (Runtime_CompareStringsIgnoreCase(definition->name, defineName) == 0 && definition->replacement[0] != '\0')
						{
							matchingDefinition = definition;
							break;
						}
					}

					if (matchingDefinition != nullptr)
					{
						*storedOutput++ = ' ';
						std::strcpy(storedOutput, matchingDefinition->replacement);
						storedOutput += std::strlen(storedOutput);
					}
					else
					{
						FUN_00406995("NXBasics::CIniFileReader::l_DoLoadIniFile  #define could not be solved - <%s> in file <%s>", defineName, fileName);
					}
				}
				else
				{
					using ErrorCallback = void(__cdecl*)(std::int32_t, const char*, const char*);

					ErrorCallback errorCallback = ReadField<ErrorCallback>(reader, 0x188C);

					if (errorCallback == nullptr)
					{
						errorCallback = ReadField<ErrorCallback>(&DAT_00512580, 0);
					}

					if (errorCallback != nullptr)
					{
						errorCallback(1, normalizedLine, input - 1);
					}
				}

				if (character == '\0')
				{
					break;
				}
			}

			*storedOutput = '\0';

			if (storedOutput != storedLine + 1)
			{
				CStringArray* lines = ReadField<CStringArray*>(reader, 0x108);
				lines->InsertString(storedLine);
			}
		}

		delete memory;
	}

	// Original function: FUN_004253e0
	inline void IniFileReader_LoadIniFile(void* reader, const char* fileName, const char* preliminaryFileName)
	{
		// Original list destructor: FUN_004259d6
		// Original value destruction: LAB_00479e17
		NXSysKeyManager::CKeyEntryList definitions(true);

		if (preliminaryFileName != nullptr)
		{
			IniFileReader_DoLoadIniFile(reader, preliminaryFileName, definitions);
		}

		IniFileReader_DoLoadIniFile(reader, fileName, definitions);
	}

	// Original function: FUN_00425980
	inline void IniFileReader_CreateCompiledFilename(char* fileName, const char* compiledExtension)
	{
		char drive[4];
		char directory[256];
		char baseName[256];

		if (compiledExtension == nullptr)
		{
			compiledExtension = ".cif";
		}

		FUN_004e7aa8(fileName, drive, directory, baseName, nullptr);
		FUN_004e8215(fileName, drive, directory, baseName, compiledExtension);
	}
}