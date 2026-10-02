#include "InGameGuiSupportAccess.h"
#include "ApplicationGlobals.h"
#include "C2CallbackManager.h"
#include "C2LifetimeAccess.h"
#include "XCrtFileAccess.h"

#include <cstdint>
#include <cstring>
#include <initializer_list>

// Original logic callbacks remain separate reconstruction targets.
extern bool __cdecl FUN_004a37b1(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern bool __cdecl FUN_004b3957(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern bool __cdecl FUN_004b450d(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
extern bool __cdecl FUN_004b77e8(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

namespace
{
    using namespace NC2Lifetime;
    // Complete one-byte InputManager::mStaticVars at 0056940C. Only its
    // constructor write is present in the Windows export; its field is unnamed.
    std::uint8_t gInputManagerStaticVars = 0;

    // Original function: FUN_004b476c; embedded input selection construction.
    void ConstructInputSelection(void* selection)
    {
        Write<std::uint16_t>(selection, 0xA9E, 0);
        Write<std::uint16_t>(selection, 0xA9C, 0);
        Write<std::uint16_t>(selection, 0xAA6, 0);
        Write<std::uint16_t>(selection, 0xAA4, 0);
        Write<std::uint16_t>(selection, 0xAAA, 0);
        Write<std::uint16_t>(selection, 0xAA8, 0);
    }

    // Original function: FUN_004b47bf; complete 300-point array.
    void ConstructInputPoints(void* storage)
    {
        for (std::uint32_t index = 0; index < 300; ++index)
        {
            Write<std::uint16_t>(storage, 4u + index * 4u + 2u, 0);
            Write<std::uint16_t>(storage, 4u + index * 4u, 0);
        }
    }

    // Original function: FUN_004b479f.
    void ConstructInputPointState(void* storage)
    {
        Write<std::uint16_t>(storage, 2, 0);
        Write<std::uint16_t>(storage, 0, 0);
        Write<std::uint16_t>(storage, 6, 0);
        Write<std::uint16_t>(storage, 4, 0);
        ConstructInputPoints(static_cast<std::byte*>(storage) + 8);
    }

    // Original function: FUN_004b473b.
    void ConstructInputMembers(void* storage)
    {
        auto* bytes = static_cast<std::byte*>(storage);
        ConstructInputSelection(bytes + 4);
        Write<std::uint16_t>(storage, 0xAB8, 0);
        Write<std::uint16_t>(storage, 0xAB6, 0);
        Write<std::uint16_t>(storage, 0xABE, 0);
        Write<std::uint16_t>(storage, 0xABC, 0);
        ConstructInputPointState(bytes + 0xAC0);
    }

    // Original function: FUN_0042bf7e; SToUserMessage construction.
    void ConstructToUserMessage(void* message)
    {
        Write<std::uint16_t>(message, 0x2A, 0);
        Write<std::uint16_t>(message, 0x28, 0);
        std::memset(message, 0, 0x304);
        for (std::size_t offset : {0x30u, 0x13Cu, 0x200u, 0x2E0u}) Write<std::uint32_t>(message, offset, 0);
        for (std::size_t offset : {8u, 0x10u, 0x18u, 0x20u}) Write<std::uint32_t>(message, offset, 0xFFFFFFFFu);
        Write<std::uint16_t>(message, 0x28, 0xFFFF);
        Write<std::uint16_t>(message, 0x2A, 0xFFFF);
    }

    void RegisterCallbacks(NC2Logic::TCallbackFunction callback, std::initializer_list<std::uint32_t> types, std::int32_t priority)
    {
        for (auto type : types) NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(type, callback, 0, priority);
    }

    void UnregisterCallbacks(NC2Logic::TCallbackFunction callback, std::initializer_list<std::uint32_t> types)
    {
        for (auto type : types) NC2Logic::CCallbackManager::sTheObjectPtr->UnRegisterCallback(type, callback, 0);
    }
}

namespace NC2InGameGuiGroups
{
    // Original function: FUN_004a3179.
    CObjectGroupManager::CObjectGroupManager()
    {
        gObjectGroupManager = this;
        std::memset(this, 0, 0x2F0C);
        RegisterCallbacks(FUN_004a37b1, {1, 0x16, 4, 8, 0xA}, 10);
    }

    // Original function: FUN_004a31f4.
    CObjectGroupManager::~CObjectGroupManager()
    {
        UnregisterCallbacks(FUN_004a37b1, {0xA, 8, 4, 0x16, 1});
        gObjectGroupManager = nullptr;
    }
}

namespace NC2InGameGuiDeadHumanDataManager
{
    // Original function: FUN_004b3759.
    CDeadHumanDataManager::CDeadHumanDataManager()
    {
        gDeadHumanDataManager = this;
        std::memset(this, 0, 8);
        std::memset(mStorage + 8, 0, 4);
        RegisterCallbacks(FUN_004b3957, {4, 3}, 10);
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0xBB8, 1));
        if (records != nullptr)
        {
            for (std::uint32_t index = 0; index < 50; ++index)
            {
                Write<std::uint16_t>(records, index * 0x3Cu + 0x3A, 0);
                Write<std::uint16_t>(records, index * 0x3Cu + 0x38, 0);
            }
        }
        Write(this, 8, records);
        std::memset(records, 0, 0xBB8);
    }

    // Original function: FUN_004b37e0.
    CDeadHumanDataManager::~CDeadHumanDataManager()
    {
        NXBasics::Runtime_Free(Read<void*>(this, 8));
        UnregisterCallbacks(FUN_004b3957, {3, 4});
        gDeadHumanDataManager = nullptr;
    }
}

namespace NC2InGameGuiMessages
{
    // Original function: FUN_004b3997.
    CMessageManager::CMessageManager()
    {
        gMessageManager = this;
        std::memset(this, 0, 0xC);
        std::memset(mStorage + 0xC, 0, 0xC);
        RegisterCallbacks(FUN_004b450d, {1, 0x16, 4, 3, 0x2E}, 10);
        Write<std::uint32_t>(this, 4, 1);
        Write<std::uint32_t>(this, 8, 0);
        auto* active = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x26480, 1));
        if (active != nullptr)
            for (std::uint32_t index = 0; index < 200; ++index) ConstructToUserMessage(active + 8u + index * 0x310u);
        Write(this, 0x10, active);
        std::memset(active, 0, 0x26480);
        auto* pending = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x26160, 1));
        if (pending != nullptr)
            for (std::uint32_t index = 0; index < 200; ++index) ConstructToUserMessage(pending + 8u + index * 0x30Cu);
        Write(this, 0x14, pending);
        std::memset(pending, 0, 0x26160);
    }

    // Original function: FUN_004b3abd.
    CMessageManager::~CMessageManager()
    {
        NXBasics::Runtime_Free(Read<void*>(this, 0x14));
        NXBasics::Runtime_Free(Read<void*>(this, 0x10));
        UnregisterCallbacks(FUN_004b450d, {0x2E, 3, 4, 0x16, 1});
        gMessageManager = nullptr;
    }
}

namespace NC2InGameGuiInputManager
{
    // Original function: FUN_004b4683.
    CInputManager::CInputManager()
    {
        ConstructInputMembers(this);
        gInputManager = this;
        std::memset(this, 0, 0xF7C);
        gInputManagerStaticVars = 0;
        RegisterCallbacks(FUN_004b77e8, {1, 4, 6, 8, 0xA, 0xC, 0x16}, 20);
        Write<std::uint32_t>(this, 0xAB0, 1);
    }

    // Original function: FUN_004b47d9.
    CInputManager::~CInputManager()
    {
        UnregisterCallbacks(FUN_004b77e8, {0x16, 0xC, 0xA, 8, 6, 4, 1});
        gInputManager = nullptr;
    }
}
