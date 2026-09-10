#include "MasterPropertyManager.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XIniFileReaderAccess.h"
#include "XStringMemory.h"

#include <cstring>
#include <new>

namespace NMasterPropertyManager
{
	// Original global: DAT_005697ec
	CPropertyManager* CPropertyManager::sTheObjectPtr = nullptr;

	// Original function: FUN_004e1408
	CPropertyManager::CPropertyManager() : mProperties(false)
	{
		sTheObjectPtr = this;
		mState = 0;
	}

	// Original function: FUN_004e1436
	CPropertyManager::~CPropertyManager()
	{
		while (true)
		{
			auto* property = static_cast<SProperty*>(mProperties.GetStartElement());

			if (property == nullptr)
			{
				break;
			}

			l_Property_RemoveAndDelete(property);
		}

		sTheObjectPtr = nullptr;

		// mProperties is destroyed automatically after the destructor body.
	}

	// Original function: FUN_004e1463
	void CPropertyManager::Property_WorkOnCommandLine(const char* commandLine)
	{
		if (commandLine == nullptr)
		{
			return;
		}

		const char* cursor = commandLine;
		char propertyName[200];
		char stringValue[200];

		const auto isNameCharacter = [](char character) -> bool
			{
				return NXBasics::Runtime_IsPropertyNameCharacter(character);
			};

		for (;;)
		{
			while (*cursor != '\0' && !isNameCharacter(*cursor))
			{
				++cursor;
			}

			if (*cursor == '\0')
			{
				return;
			}

			char* nameOutput = propertyName;

			do
			{
				if (!isNameCharacter(*cursor))
				{
					break;
				}

				*nameOutput++ = *cursor++;
			} while (*cursor != '\0');

			*nameOutput = '\0';

			bool isInteger = false;
			std::int32_t integerValue = 0;
			const char* textValue = nullptr;

			if (*cursor == ':')
			{
				++cursor;

				if (*cursor != '\0')
				{
					if (*cursor == '"')
					{
						++cursor;
						char* textOutput = stringValue;

						while (*cursor != '\0' && *cursor != '"')
						{
							*textOutput++ = *cursor++;
						}

						*textOutput = '\0';
						textValue = stringValue;

						if (*cursor == '"')
						{
							++cursor;
						}
					}
					else
					{
						isInteger = true;
						integerValue = NXBasics::Runtime_StringToInteger(cursor);

						// Only a literal space terminates this input segment.
						const char* separator = std::strchr(cursor, ' ');

						if (separator != nullptr)
						{
							cursor = separator;
						}
						else
						{
							cursor += std::strlen(cursor);
						}
					}
				}
			}

			Property_Add(propertyName, isInteger, integerValue, textValue);
		}
	}

	// Original function: FUN_004e1551
	void CPropertyManager::Property_WorkOnIniFile(const char* fileName)
	{
		if (static_cast<std::uint8_t>(NXBasics::CFile::FileSystem_Tool_FileExists(fileName, false)) == 0)
		{
			return;
		}

		// Temporary storage matching the original stack region.
		// Replace with CIniFileReader once its native layout is reconstructed.
		alignas(4) std::byte readerStorage[0x1890];
		void* reader = readerStorage;

		FUN_00424df8(reader, fileName, nullptr, false, nullptr, nullptr);

		if (readerStorage[0] != std::byte{ 0 })
		{
			if (NXBasics::IniFileReader_GetFirstLine(reader) != nullptr)
			{
				do
				{
					std::int32_t lineType;
					std::memcpy(&lineType, readerStorage + 0x110, sizeof(lineType));

					if (lineType != 1)
					{
						bool isInteger = false;
						std::int32_t integerValue = 0;
						const char* stringValue = nullptr;

						if (NXBasics::IniFileReader_GetTypeOfNextParameter(reader) == 3)
						{
							isInteger = true;
							integerValue = NXBasics::IniFileReader_GetInt(reader);
						}
						else if (NXBasics::IniFileReader_GetTypeOfNextParameter(reader) == 2)
						{
							stringValue = NXBasics::IniFileReader_GetString(reader);
						}

						// Read the name after the parameter-access calls.
						const auto* propertyName = reinterpret_cast<const char*>(readerStorage + 0x8EC);
						Property_Add(propertyName, isInteger, integerValue, stringValue);
					}
				} while (NXBasics::IniFileReader_GetNextLine(reader) != nullptr);
			}
		}

		FUN_00424f6c(reader);
	}

	// Original function: FUN_004e171d
	SProperty* CPropertyManager::l_Property_GetPtr(const char* propertyName)
	{
		NXSysKeyManager::CKeyEntryListBase::Iterator iterator(mProperties);
		auto* property = static_cast<SProperty*>(iterator.First());

		while (property != nullptr)
		{
			if (NXBasics::Runtime_CompareStringsIgnoreCase(propertyName, property->name) == 0)
			{
				return property;
			}

			property = static_cast<SProperty*>(iterator.Next());
		}

		return nullptr;
	}

	// Original function: FUN_004e1765
	void CPropertyManager::l_Property_RemoveAndDelete(SProperty* property)
	{
		mProperties.Remove(property);

		::operator delete(static_cast<void*>(property->name));
		::operator delete(static_cast<void*>(property->stringValue));

		delete property;
	}

	// List construction within FUN_004e1408.
	CPropertyManager::SObjectVars::SObjectVars(std::uint8_t destroyValues) : CKeyEntryListBase(destroyValues)
	{}

	// Original function: FUN_004e163b
	std::int32_t CPropertyManager::Property_GetIntegerValue(const char* propertyName)
	{
		const SProperty* property = l_Property_GetPtr(propertyName);

		if (property == nullptr || property->isInteger == 0)
		{
			return 0;
		}

		return property->integerValue;
	}

	// Original function: FUN_004e1658
	const char* CPropertyManager::Property_GetStringValuePtr(const char* propertyName)
	{
		const SProperty* property = l_Property_GetPtr(propertyName);
		return property != nullptr ? property->stringValue : nullptr;
	}

	// Original function: FUN_004e166b
	bool CPropertyManager::Property_DoesExists(const char* propertyName)
	{
		return l_Property_GetPtr(propertyName) != nullptr;
	}

	// Original function: FUN_004e167d
	void CPropertyManager::Property_Add(const char* propertyName, bool isInteger, std::int32_t integerValue, const char* stringValue)
	{
		SProperty* property = l_Property_GetPtr(propertyName);

		if (property != nullptr)
		{
			// Preserve the original order: release before duplicating.
			::operator delete(static_cast<void*>(property->stringValue));

			property->isInteger = static_cast<std::uint8_t>(isInteger);
			property->integerValue = integerValue;
			property->stringValue = NXBasics::StringDuplicate(stringValue);
			return;
		}

		property = new (std::nothrow) SProperty;
		std::memset(property, 0, sizeof(*property));

		property->name = NXBasics::StringDuplicate(propertyName);
		property->isInteger = static_cast<std::uint8_t>(isInteger);
		property->integerValue = integerValue;
		property->stringValue = NXBasics::StringDuplicate(stringValue);

		mProperties.PushBack(property);
	}

	// Original function: FUN_004e16fb
	bool CPropertyManager::Property_Remove(const char* propertyName)
	{
		SProperty* property = l_Property_GetPtr(propertyName);

		if (property == nullptr)
		{
			return false;
		}

		l_Property_RemoveAndDelete(property);
		return true;
	}

	// Original function: FUN_004e1d00
	CPropertyManager::SObjectVars::~SObjectVars()
	{
		if (mDestroyValues != 0)
		{
			ClearOwnedEntries();
		}

		// The base destructor runs automatically afterward.
	}

	// Original function: LAB_00479e17
	void CPropertyManager::SObjectVars::DestroyValue(void* value)
	{
		// Release storage only; nested string allocations are not released here.
		::operator delete(value);
	}
}