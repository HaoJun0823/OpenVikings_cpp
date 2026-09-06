#pragma once

#include <cstddef>
#include <cstdint>

namespace NXSysKeyManager
{
	struct SKeyEntryNode
	{
		SKeyEntryNode* next;  // +0x00
		void* value;         // +0x04
	};

	struct SKeyMessage
	{
		std::uint8_t keyCode;         // +0x00
		std::uint8_t auxiliaryCode;   // +0x01
		std::byte padding02[2];       // +0x02
		std::uint32_t modifierFlags;  // +0x04
		std::uint8_t state;           // +0x08
		std::byte padding09[3];       // +0x09
	};

	static_assert(sizeof(SKeyMessage) == 0x0C);
	static_assert(offsetof(SKeyMessage, modifierFlags) == 0x04);
	static_assert(offsetof(SKeyMessage, state) == 0x08);

	static_assert(sizeof(SKeyEntryNode) == 0x08);

	class CKeyEntryListBase
	{
	public:
		// Original function: FUN_00439bed
		explicit CKeyEntryListBase(std::uint8_t destroyValues);

		// Original function: FUN_00439c3f
		virtual ~CKeyEntryListBase();

		CKeyEntryListBase(const CKeyEntryListBase&) = delete;
		CKeyEntryListBase& operator=(const CKeyEntryListBase&) = delete;

		// Original function: FUN_00439d1e
		void* PopFront();

		// Original function: FUN_00439e0e
		void DeleteFront();

		// Original function: FUN_00439e24
		void ClearOwnedEntries();

		// Original function: FUN_00439c8b
		void PushBack(void* value);

		bool IsEmpty() const
		{
			return mCount == 0;
		}

	protected:
		// Container vtable slot at +0x04 in the x86 build.
		// Implemented by CKeyEntryList.
		virtual void DestroyValue(void* value) = 0;

		// Implicit vtable pointer at +0x00.
		std::uint32_t mCount;          // +0x04
		SKeyEntryNode* mFirst;         // +0x08
		SKeyEntryNode* mLast;          // +0x0C
		std::uint8_t mDestroyValues;   // +0x10
		std::byte mPadding11[3];       // +0x11
	};

	static_assert(sizeof(CKeyEntryListBase) == 0x14);

	class CKeyEntryList final : public CKeyEntryListBase
	{
	public:
		explicit CKeyEntryList(std::uint8_t destroyValues);

		// Original function: FUN_00479dfa
		// FUN_00479e24 is the corresponding scalar deleting destructor.
		~CKeyEntryList() override;

	protected:
		// Original code: LAB_00479e17
		void DestroyValue(void* value) override;
	};

	static_assert(sizeof(CKeyEntryList) == 0x14);

	class CKeyManager
	{
	public:
		// Original function: FUN_00479cf9
		CKeyManager();

		// Original function: FUN_00479d27
		~CKeyManager();

		CKeyManager(const CKeyManager&) = delete;
		CKeyManager& operator=(const CKeyManager&) = delete;

		// Original function: FUN_00479dea
		void Init();

		// Original function: FUN_00479da2
		bool GetKey(SKeyMessage& message);

		// Original function: FUN_00479db4
		char GetKeyboardMessage(SKeyMessage& message);

		// Original function: FUN_00479d42
		void PushKey(std::uint8_t keyCode, std::uint8_t auxiliaryCode, bool state);

		static CKeyManager* sTheObjectPtr;

	private:
		std::uint32_t mModifierFlags;  // +0x00
		std::uint32_t mLastKeyTime;    // +0x04
		CKeyEntryList mEntries;        // +0x08
	};

	static_assert(sizeof(CKeyManager) == 0x1C);
}