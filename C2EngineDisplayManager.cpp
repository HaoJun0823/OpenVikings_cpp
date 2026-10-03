#include "C2EngineDisplayAccess.h"
#include "C2EngineDisplayManager.h"
#include "C2GameMapAccess.h"
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

    // Original function: FUN_004812b4.
    void C2DEngineDisplayManager::InformThatMapXtructed()
    {
        const std::uint32_t center = static_cast<std::uint16_t>(NC2Logic::GameMap_GetWidth() / 2) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(NC2Logic::GameMap_GetHeight() / 2)) << 16);
        Iterator iterator(*this);
        for (void* display = iterator.First(); display != nullptr; display = iterator.Next())
        {
            Display_ReInit(display);
            DE_SetWantedPositionToMapPoint(display, &center);
        }
    }

    // Original function: FUN_00481319.
    void C2DEngineDisplayManager::InformThatControllingPlayerChanged(std::int32_t playerId)
    {
        Iterator iterator(*this);
        for (void* display = iterator.First(); display != nullptr; display = iterator.Next())
            DE_ControllingPlayer_SetId(display, playerId);
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

    // Original function: FUN_0048134e.
    void C2DEngineDisplayManager::InformSetExplorationState(bool enabled)
    {
        Iterator iterator(*this);
        for (void* display = iterator.First(); display != nullptr; display = iterator.Next())
            DE_ToDraw_SetState(display, 4, enabled);
    }
}

// Original function: FUN_0047e669; CE2Manager::Inform_SetExplorationState.
void __cdecl E2Manager_Inform_SetExplorationState(bool enabled)
{
    NC2E2::C2DEngineDisplayManager::sTheObjectPtr->InformSetExplorationState(enabled);
}
