#include "XMKeyManager.h"

#include <Windows.h>
#include <mmsystem.h>

#include <cstring>
#include <cstdint>
#include <new>

namespace NXSysKeyManager
{
	CKeyManager* CKeyManager::sTheObjectPtr = nullptr;

	// Original function: FUN_00479cf9
	CKeyManager::CKeyManager() : mEntries(1)
	{
		sTheObjectPtr = this;
		mModifierFlags = 0;
		mLastKeyTime = 0;
	}

	// Original function: FUN_00479d42
	void CKeyManager::PushKey(std::uint8_t keyCode, std::uint8_t auxiliaryCode, bool state)
	{
		SKeyMessage* message = new SKeyMessage;

		message->keyCode = keyCode;
		message->auxiliaryCode = auxiliaryCode;
		message->state = static_cast<std::uint8_t>(state);

		mModifierFlags = 0;

		if ((GetAsyncKeyState(VK_SHIFT) & 0x8000) != 0)
		{
			mModifierFlags |= 0x01;
		}

		if ((GetAsyncKeyState(VK_CONTROL) & 0x8000) != 0)
		{
			mModifierFlags |= 0x02;
		}

		message->modifierFlags = mModifierFlags;
		mEntries.PushBack(message);
		mLastKeyTime = timeGetTime();
	}

	// Original function: FUN_00479dea
	void CKeyManager::Init()
	{
		mEntries.ClearOwnedEntries();
		mModifierFlags = 0;
	}

	// Original function: FUN_00479da2
	bool CKeyManager::GetKey(SKeyMessage& message)
	{
		return GetKeyboardMessage(message) == 1;
	}

	// Original function: FUN_00479db4
	char CKeyManager::GetKeyboardMessage(SKeyMessage& message)
	{
		if (mEntries.IsEmpty())
		{
			return 0;
		}

		void* value = mEntries.PopFront();

		// Copy all 12 bytes, including padding, as in the original.
		std::memcpy(&message, value, sizeof(message));

		// Original: FUN_004e5e48(value).
		::operator delete(value);

		return message.state != 0 ? 1 : 2;
	}

	// Original function: FUN_00439bed
	CKeyEntryListBase::CKeyEntryListBase(std::uint8_t destroyValues) : mCount(0), mFirst(nullptr), mLast(nullptr), mDestroyValues(destroyValues), mPadding11{} {}

	// Original function: FUN_00439c3f
	CKeyEntryListBase::~CKeyEntryListBase()
	{
		while (mFirst != nullptr)
		{
			PopFront();
		}
	}

	// Original function: FUN_00439c8b
	void CKeyEntryListBase::PushBack(void* value)
	{
		SKeyEntryNode* node = new (std::nothrow) SKeyEntryNode;

		if (node != nullptr)
		{
			node->next = nullptr;
			node->value = value;
		}

		if (mCount == 0)
		{
			mFirst = node;
		}
		else
		{
			mLast->next = node;
		}

		++mCount;
		mLast = node;
	}

	// Original function: FUN_00439d1e
	void* CKeyEntryListBase::PopFront()
	{
		if (mCount == 0)
		{
			return nullptr;
		}

		SKeyEntryNode* removedNode = mFirst;
		SKeyEntryNode* nextNode = removedNode->next;
		void* value = removedNode->value;

		if (removedNode == mLast)
		{
			mLast = nullptr;
			mFirst = nullptr;
		}
		else
		{
			mFirst = nextNode;
		}

		// Original: FUN_004e5e48(removedNode).
		::operator delete(static_cast<void*>(removedNode));

		--mCount;

		return value;
	}

	// Original function: FUN_00439e0e
	void CKeyEntryListBase::DeleteFront()
	{
		void* value = PopFront();

		if (value != nullptr)
		{
			DestroyValue(value);
		}
	}

	// Original function: FUN_00439e24
	void CKeyEntryListBase::ClearOwnedEntries()
	{
		while (mCount != 0)
		{
			// The original stops when the first entry contains a null value.
			if (mFirst->value == nullptr)
			{
				break;
			}

			DeleteFront();
		}
	}

	CKeyEntryList::CKeyEntryList(std::uint8_t destroyValues) : CKeyEntryListBase(destroyValues)
	{}

	// Original function: FUN_00479dfa
	CKeyEntryList::~CKeyEntryList()
	{
		if (mDestroyValues != 0)
		{
			ClearOwnedEntries();
		}

		// The base destructor runs automatically afterward.
	}

	// Original: FUN_004e5e48(value).
	void CKeyEntryList::DestroyValue(void* value)
	{
		// Original code: LAB_00479e17
		// No destructor is called for the stored object.
		::operator delete(value);
	}

	// Original function: FUN_00479d27
	CKeyManager::~CKeyManager()
	{
		mEntries.ClearOwnedEntries();
		sTheObjectPtr = nullptr;

		// mEntries is destroyed automatically afterward.
	}
}