#include "XNumericConstants.h"
#include "XMKeyManager.h"
#include "XCrtFileAccess.h"

#include <Windows.h>
#include <mmsystem.h>

#include <cstring>
#include <cstdint>
#include <new>

namespace NXSysKeyManager
{
    // Original function: FUN_00439da8
    void* CKeyEntryListBase::Remove(void* value)
    {
        if (mCount == 0)
        {
            return value;
        }

        if (mFirst->value == value)
        {
            return PopFront();
        }

        if (mLast->value == value)
        {
            return PopBack();
        }

        SKeyEntryNode* previous = mFirst;

        while (previous->next != nullptr)
        {
            SKeyEntryNode* node = previous->next;

            if (node->value == value)
            {
                previous->next = node->next;

                NXBasics::Runtime_Free(node);
                --mCount;
                return value;
            }

            previous = node;
        }

        return value;
    }

    // Original function: FUN_00439e41
    void* CKeyEntryListBase::GetStartElement() const
    {
        if (mCount == 0)
        {
            return nullptr;
        }

        return mFirst->value;
    }

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
        auto* message = ::new(NXBasics::Runtime_AllocateWithNewHandler(sizeof(SKeyMessage))) SKeyMessage;

        message->keyCode = keyCode;
        message->auxiliaryCode = auxiliaryCode;
        message->state = static_cast<std::uint8_t>(state);

        mModifierFlags = 0;

        if ((GetAsyncKeyState(VK_SHIFT) & NXBasics::BitMask::Bit15Signed) != 0)
        {
            mModifierFlags |= 1;
        }

        if ((GetAsyncKeyState(VK_CONTROL) & NXBasics::BitMask::Bit15Signed) != 0)
        {
            mModifierFlags |= NXBasics::BitMask::Bit1Signed;
        }

        message->modifierFlags = mModifierFlags;
        mEntries.PushBack(message);
        mLastKeyTime = timeGetTime();
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
        NXBasics::Runtime_CopyMemory(&message, value, sizeof(message));

        NXBasics::Runtime_Free(value);

        return message.state != 0 ? 1 : 2;
    }

    // Original function: FUN_00479dea
    void CKeyManager::Init()
    {
        mEntries.ClearOwnedEntries();
        mModifierFlags = 0;
    }

    // Original function: FUN_00439bed
    CKeyEntryListBase::CKeyEntryListBase(std::uint8_t destroyValues) : mCount(0), mFirst(nullptr), mLast(nullptr), mDestroyValues(destroyValues), mPadding11{} {}

    CKeyEntryList::CKeyEntryList(std::uint8_t destroyValues) : CKeyEntryListBase(destroyValues)
    {}

    // Original function: LAB_00479e17; the stored value has no destructor call.
    void CKeyEntryList::DestroyValue(void* value)
    {
        NXBasics::Runtime_Free(value);
    }

    CKeyEntryListBase::Iterator::Iterator(CKeyEntryListBase& list) : mList(&list), mPosition(nullptr), mStarted(0)
    {}

    // Original function: FUN_004259f3
    void* CKeyEntryListBase::Iterator::Next()
    {
        if (mStarted == 0)
        {
            return First();
        }

        if (mPosition != nullptr)
        {
            mPosition = mPosition->next;

            if (mPosition != nullptr)
            {
                return mPosition->value;
            }
        }

        mStarted = 0;
        return nullptr;
    }

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
        void* allocation = NXBasics::Runtime_AllocateWithNewHandler(sizeof(SKeyEntryNode));
        auto* node = allocation == nullptr ? nullptr : ::new(allocation) SKeyEntryNode;

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

        NXBasics::Runtime_Free(removedNode);

        --mCount;

        return value;
    }

    // Original function: FUN_00439d54
    void* CKeyEntryListBase::PopBack()
    {
        if (mCount == 0)
        {
            return nullptr;
        }

        void* value = mLast->value;
        SKeyEntryNode* current = mFirst;
        SKeyEntryNode* previous = nullptr;

        // The original compares stored values, not node addresses.
        while (current->next != nullptr)
        {
            if (current->next->value == value)
            {
                previous = current;
                break;
            }

            current = current->next;
        }

        if (previous != nullptr)
        {
            previous->next = nullptr;
        }

        NXBasics::Runtime_Free(mLast);

        const std::uint32_t count = mCount;
        mLast = previous;

        // Preserve the original check before decrementing the count.
        if (count == 0)
        {
            mFirst = nullptr;
        }

        mCount = count - 1u;
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

    // Original function: FUN_00479d27
    CKeyManager::~CKeyManager()
    {
        mEntries.ClearOwnedEntries();
        sTheObjectPtr = nullptr;

        // mEntries is destroyed automatically afterward.
    }

    // Original function: FUN_00479dfa
    CKeyEntryList::~CKeyEntryList()
    {
        if (mDestroyValues != 0)
        {
            ClearOwnedEntries();
        }

        // The base destructor runs automatically afterward.
    }

    // Original function: FUN_00479e24; member-list destruction normally keeps its storage.
    void* CKeyEntryList::Delete(std::uint32_t flags)
    {
        auto* result = this;
        this->~CKeyEntryList();
        if ((flags & 1u) != 0) NXBasics::Runtime_Free(result);
        return result;
    }

    // Original function: FUN_0048d143
    void* CKeyEntryListBase::Iterator::First()
    {
        if (mList->mCount == 0)
        {
            // The original leaves the iterator state unchanged here.
            return nullptr;
        }

        mStarted = 1;
        mPosition = mList->mFirst;
        return mPosition->value;
    }
}
