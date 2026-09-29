#include "C2EngineDisplayManager.h"
#include "XCrtFileAccess.h"
#include "XMKeyManager.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

// Original data: DAT_00555110; meaning not yet established.
std::uint8_t DAT_00555110 = 0;

namespace NC2E2
{
    // Original function: FUN_004811ae
    C2DEngineDisplayManager::C2DEngineDisplayManager() : CKeyEntryListBase(0), mActiveDisplay(nullptr)
    {
        sTheObjectPtr = this;

        // Preserve the initialized data fields without overwriting the C++ vtable pointer.
        mCount = 0;
        mFirst = nullptr;
        mLast = nullptr;
        mDestroyValues = 0;
        std::memset(mPadding11, 0, sizeof(mPadding11));
        mActiveDisplay = nullptr;

        DAT_00555110 = 0;
    }

    // Original function: FUN_004811e3; list teardown: FUN_004813e8.
    C2DEngineDisplayManager::~C2DEngineDisplayManager()
    {
        sTheObjectPtr = nullptr;

        if (mDestroyValues != 0)
        {
            ClearOwnedEntries();
        }
    }

    // Original function: FUN_004811ef
    void C2DEngineDisplayManager::RegisterDisplayObject(void* display)
    {
        PushBack(display);

        std::uint8_t active;
        std::memcpy(&active, static_cast<const std::byte*>(display) + 0x6c, sizeof(active));

        if (active != 0)
        {
            mActiveDisplay = display;
        }
    }

    // Original function: FUN_0048120b
    void C2DEngineDisplayManager::UnregisterDisplayObject(void* display)
    {
        Remove(display);

        std::uint8_t active;
        std::memcpy(&active, static_cast<const std::byte*>(display) + 0x6c, sizeof(active));

        if (active != 0)
        {
            mActiveDisplay = nullptr;
        }
    }

    // Original function: FUN_00481284
    void C2DEngineDisplayManager::InformThatStaticMapChanged()
    {
        Iterator iterator(*this);

        for (void* display = iterator.First(); display != nullptr; display = iterator.Next())
        {
            static_cast<std::byte*>(display)[0x978] = std::byte{ 1 };
        }
    }

    // Original function: FUN_00481405
    void C2DEngineDisplayManager::DestroyValue(void* value)
    {
        if (value == nullptr)
        {
            return;
        }

        Display_Destruct(value);
        NXBasics::Runtime_Free(value);
    }
}