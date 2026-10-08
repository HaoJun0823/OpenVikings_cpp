#include "InGameGuiSupportAccess.h"
#include "ApplicationGlobals.h"
#include "C2CallbackManager.h"
#include "C2LifetimeAccess.h"
#include "XCrtFileAccess.h"
#include "C2LogicObjectArrays.h"
#include "InGameGuiManager.h"
#include "C2EngineObjectData.h"
#include "C2GameState.h"
#include "C2VehicleAccess.h"
#include "C2UserMessageAccess.h"
#include "InGameGuiInputAccess.h"
#include "C2PlayerCenterPosition.h"

#include <cstdint>
#include <cstring>
#include <initializer_list>

// Original logic callbacks remain separate reconstruction targets.
extern bool __cdecl InputManager_ls_Callback_Logic(std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

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
    // Original function: FUN_004a32c2.
    void CObjectGroupManager::Group_VerifyAllObjects()
    {
        for (std::uint32_t group = 0; group < 10; ++group) l_Group_VerifyAllObjects(mStorage + 4u + group * 0x4B4u);
    }

    // Original function: FUN_004a32e2.
    void CObjectGroupManager::Group_CreateNewGroup(std::uint32_t index)
    {
        if (index >= 10) return;
        Write<std::uint32_t>(mStorage, 4u + index * 0x4B4u, 0);
        Group_AddSelectionToGroup(index);
    }

    // Original function: FUN_004a3301.
    void CObjectGroupManager::Group_AddSelectionToGroup(std::uint32_t index)
    {
        using namespace NC2InGameGuiInputManager;
        if (index >= 10) return;
        Group_VerifyAllObjects();
        void* group = mStorage + 4u + index * 0x4B4u;
        auto* input = gInputManager;
        switch (Read<std::uint32_t>(input, 4))
        {
        case 1: l_Group_AddHuman(group, Selection_GetSelectedSingleHumanId(input)); break;
        case 2: for (std::uint32_t i = 0; i < Selection_GetSelectedMultipleHumanNumber(input); ++i) l_Group_AddHuman(group, Selection_GetSelectedMultipleHumanId(input, i)); break;
        case 4: l_Group_AddVehicle(group, Selection_GetSelectedSingleVehicleId(input)); break;
        case 5: for (std::uint32_t i = 0; i < Selection_GetSelectedMultipleVehiclesNumber(input); ++i) l_Group_AddVehicle(group, Selection_GetSelectedMultipleVehicleId(input, i)); break;
        case 6: l_Group_AddHouse(group, Selection_GetSelectedHouseId(input)); break;
        default: break;
        }
    }

    // Original function: FUN_004a340e.
    void CObjectGroupManager::Group_Choose(std::uint32_t index)
    {
        using namespace NC2InGameGuiInputManager;
        if (index >= 10) return;
        Group_VerifyAllObjects();
        const void* group = mStorage + 4u + index * 0x4B4u;
        if (l_Group_IsSelected(group))
        {
            std::uint32_t point = 0;
            if (l_Group_GetPosition(group, &point)) NC2InGameGuiEntryPoints::C2IGG_EntryPoint_MainDisplay_SetToMapPosition(&point);
            return;
        }
        Selection_UnselectAll(gInputManager);
        InputMode_Cancel(gInputManager);
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(group, 0); ++i)
        {
            const auto id = Read<std::int32_t>(group, 8u + i * 12u);
            switch (Read<std::uint32_t>(group, 4u + i * 12u))
            {
            case 1: Selection_SelectAddSingleHuman(gInputManager, id); break;
            case 2: Selection_SelectAddSingleVehicle(gInputManager, id); break;
            case 3: Selection_SelectHouse(gInputManager, id); break;
            default: break;
            }
        }
    }

    // Original function: FUN_004a358d; an object belongs to at most one group.
    void CObjectGroupManager::l_Group_AddHuman(void* group, std::uint32_t id)
    {
        for (std::uint32_t i = 0; i < 10; ++i) l_Group_RemoveHuman(mStorage + 4u + i * 0x4B4u, id);
        if (Read<std::uint32_t>(group, 0) == 0 || Read<std::uint32_t>(group, 4) != 1) Write<std::uint32_t>(group, 0, 0);
        const auto count = Read<std::uint32_t>(group, 0);
        if (count == 100) return;
        Write<std::uint32_t>(group, 4u + count * 12u, 1); Write(group, 8u + count * 12u, id);
        Write(group, 12u + count * 12u, Read<std::uint32_t>(NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize, 8));
        Write(group, 0, count + 1);
    }

    // Original function: FUN_004a363a.
    void CObjectGroupManager::l_Group_AddVehicle(void* group, std::uint32_t id)
    {
        for (std::uint32_t i = 0; i < 10; ++i) l_Group_RemoveVehicle(mStorage + 4u + i * 0x4B4u, id);
        if (Read<std::uint32_t>(group, 0) == 0 || Read<std::uint32_t>(group, 4) != 2) Write<std::uint32_t>(group, 0, 0);
        const auto count = Read<std::uint32_t>(group, 0);
        if (count == 100) return;
        Write<std::uint32_t>(group, 4u + count * 12u, 2); Write(group, 8u + count * 12u, id);
        Write(group, 12u + count * 12u, Read<std::uint32_t>(NC2Logic::CVehicleArray::mStaticVars + id * NC2Logic::CVehicleArray::ElementSize, 8));
        Write(group, 0, count + 1);
    }

    // Original function: FUN_004a36e7; houses replace the complete group.
    void CObjectGroupManager::l_Group_AddHouse(void* group, std::uint32_t id)
    {
        for (std::uint32_t i = 0; i < 10; ++i) l_Group_RemoveHouse(mStorage + 4u + i * 0x4B4u, id);
        Write<std::uint32_t>(group, 0, 0); Write(group, 8, id); Write<std::uint32_t>(group, 4, 3);
        Write(group, 12, Read<std::uint32_t>(NC2Logic::CHouseArray::mStaticVars + id * NC2Logic::CHouseArray::ElementSize, 8));
        Write<std::uint32_t>(group, 0, 1);
    }

    // Original function: FUN_004a3947; Windows tests membership, not equal counts.
    bool CObjectGroupManager::l_Group_IsSelected(const void* group) const
    {
        using namespace NC2InGameGuiInputManager;
        bool selected = false;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(group, 0); ++i)
        {
            const auto id = Read<std::int32_t>(group, 8u + i * 12u);
            switch (Read<std::uint32_t>(group, 4u + i * 12u))
            {
            case 1: selected = Selection_IsHumanSelected(gInputManager, id); break;
            case 2: selected = Selection_IsVehicleSelected(gInputManager, id); break;
            case 3: selected = Selection_IsHouseSelected(gInputManager, id); break;
            default: continue;
            }
            if (!selected) return false;
        }
        return selected;
    }

    // Original function: FUN_004a39a8; signed sums and division precede WORD narrowing.
    bool CObjectGroupManager::l_Group_GetPosition(const void* group, std::uint32_t* point) const
    {
        std::int32_t x = 0, y = 0, count = 0;
        for (std::uint32_t i = 0; i < Read<std::uint32_t>(group, 0); ++i)
        {
            const auto id = Read<std::uint32_t>(group, 8u + i * 12u);
            const void* position;
            switch (Read<std::uint32_t>(group, 4u + i * 12u))
            {
            case 1: position = NC2Logic::Human_GetMapPositionRef(NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize); break;
            case 2: position = NC2Logic::Vehicle_GetMapPositionRef(NC2Logic::CVehicleArray::mStaticVars + id * NC2Logic::CVehicleArray::ElementSize); break;
            case 3: position = NC2Logic::CHouseArray::mStaticVars + id * NC2Logic::CHouseArray::ElementSize + 0x1C; break;
            default: continue;
            }
            x += Read<std::int16_t>(position, 0); y += Read<std::int16_t>(position, 2); ++count;
        }
        if (count == 0) { *point = 0; return false; }
        *point = static_cast<std::uint16_t>(x / count) | (static_cast<std::uint32_t>(static_cast<std::uint16_t>(y / count)) << 16);
        return true;
    }

    // Original function: FUN_004a34c8; removal compacts and revisits the slot.
    void CObjectGroupManager::l_Group_VerifyAllObjects(void* group)
    {
        std::uint32_t index = 0;
        while (index < NC2Lifetime::Read<std::uint32_t>(group, 0))
        {
            if (l_Group_IsObjectValid(static_cast<std::byte*>(group) + 4u + index * 12u)) ++index;
            else l_Group_RemoveObject(group, index);
        }
    }

    // Original function: FUN_004a3508.
    bool CObjectGroupManager::l_Group_IsObjectValid(const void* object) const
    {
        using NC2Lifetime::Read;
        const auto id = Read<std::uint32_t>(object, 4);
        const void* entity;
        std::uint32_t playerOffset;
        switch (Read<std::uint32_t>(object, 0))
        {
        case 1: entity = NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize; playerOffset = 0x10; break;
        case 2: entity = NC2Logic::CVehicleArray::mStaticVars + id * NC2Logic::CVehicleArray::ElementSize; playerOffset = 0x1E0; break;
        case 3: entity = NC2Logic::CHouseArray::mStaticVars + id * NC2Logic::CHouseArray::ElementSize; playerOffset = 0x0C; break;
        default: return false;
        }
        return Read<std::uint8_t>(entity, 0) != 0 && Read<std::uint32_t>(entity, 8) == Read<std::uint32_t>(object, 8) && Read<std::int32_t>(entity, playerOffset) == NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();
    }

    // Original function: FUN_004a3603; remove only the first matching object.
    void CObjectGroupManager::l_Group_RemoveHuman(void* group, std::uint32_t id)
    {
        const auto count = NC2Lifetime::Read<std::uint32_t>(group, 0);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            if (NC2Lifetime::Read<std::uint32_t>(group, 4u + index * 12u) == 1 && NC2Lifetime::Read<std::uint32_t>(group, 8u + index * 12u) == id) { l_Group_RemoveObject(group, index); return; }
        }
    }

    // Original function: FUN_004a36b0.
    void CObjectGroupManager::l_Group_RemoveVehicle(void* group, std::uint32_t id)
    {
        const auto count = NC2Lifetime::Read<std::uint32_t>(group, 0);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            if (NC2Lifetime::Read<std::uint32_t>(group, 4u + index * 12u) == 2 && NC2Lifetime::Read<std::uint32_t>(group, 8u + index * 12u) == id) { l_Group_RemoveObject(group, index); return; }
        }
    }

    // Original function: FUN_004a3741.
    void CObjectGroupManager::l_Group_RemoveHouse(void* group, std::uint32_t id)
    {
        const auto count = NC2Lifetime::Read<std::uint32_t>(group, 0);
        for (std::uint32_t index = 0; index < count; ++index)
        {
            if (NC2Lifetime::Read<std::uint32_t>(group, 4u + index * 12u) == 3 && NC2Lifetime::Read<std::uint32_t>(group, 8u + index * 12u) == id) { l_Group_RemoveObject(group, index); return; }
        }
    }

    // Original function: FUN_004a3778; unchecked slot and wrapped byte count.
    void CObjectGroupManager::l_Group_RemoveObject(void* group, std::uint32_t index)
    {
        auto* bytes = static_cast<std::byte*>(group);
        const auto count = NC2Lifetime::Read<std::uint32_t>(group, 0);
        std::memmove(bytes + 4u + index * 12u, bytes + 16u + index * 12u, (count - index - 1u) * 12u);
        NC2Lifetime::Write(group, 0, NC2Lifetime::Read<std::uint32_t>(group, 0) - 1u);
    }

    // Original function: FUN_004a37b1.
    bool __cdecl CObjectGroupManager::ls_Callback_Logic(std::uint32_t type, std::uint32_t, std::uint32_t param1, std::uint32_t param2, std::uint32_t)
    {
        if (type == 1) static_cast<CObjectGroupManager*>(gObjectGroupManager)->Group_VerifyAllObjects();
        else if (type == 0x16 && param1 != 0) std::memset(static_cast<CObjectGroupManager*>(gObjectGroupManager)->mStorage + 4, 0, 0x2F08);
        else if (param2 == 0)
        {
            auto* manager = static_cast<CObjectGroupManager*>(gObjectGroupManager);
            if (type == 4) for (std::uint32_t group = 0; group < 10; ++group) manager->l_Group_RemoveHuman(manager->mStorage + 4u + group * 0x4B4u, param1);
            else if (type == 8) for (std::uint32_t group = 0; group < 10; ++group) manager->l_Group_RemoveVehicle(manager->mStorage + 4u + group * 0x4B4u, param1);
            else if (type == 10) for (std::uint32_t group = 0; group < 10; ++group) manager->l_Group_RemoveHouse(manager->mStorage + 4u + group * 0x4B4u, param1);
        }
        return true;
    }

    // Original function: FUN_004a388d.
    std::int32_t CObjectGroupManager::Misc_GetGroupIdOfHuman(std::uint32_t humanId) const
    {
        for (std::uint32_t group = 0; group < 10; ++group)
        {
            const auto* data = mStorage + 4u + group * 0x4B4u;
            for (std::uint32_t index = 0; index < NC2Lifetime::Read<std::uint32_t>(data, 0); ++index)
                if (NC2Lifetime::Read<std::uint32_t>(data, 4u + index * 12u) == 1 && NC2Lifetime::Read<std::uint32_t>(data, 8u + index * 12u) == humanId) return static_cast<std::int32_t>(group);
        }
        return -1;
    }

    // Original function: FUN_004a38cb.
    std::int32_t CObjectGroupManager::Misc_GetGroupIdOfVehicle(std::uint32_t vehicleId) const
    {
        for (std::uint32_t group = 0; group < 10; ++group)
        {
            const auto* data = mStorage + 4u + group * 0x4B4u;
            for (std::uint32_t index = 0; index < NC2Lifetime::Read<std::uint32_t>(data, 0); ++index)
                if (NC2Lifetime::Read<std::uint32_t>(data, 4u + index * 12u) == 2 && NC2Lifetime::Read<std::uint32_t>(data, 8u + index * 12u) == vehicleId) return static_cast<std::int32_t>(group);
        }
        return -1;
    }

    // Original function: FUN_004a3909.
    std::int32_t CObjectGroupManager::Misc_GetGroupIdOfHouse(std::uint32_t houseId) const
    {
        for (std::uint32_t group = 0; group < 10; ++group)
        {
            const auto* data = mStorage + 4u + group * 0x4B4u;
            for (std::uint32_t index = 0; index < NC2Lifetime::Read<std::uint32_t>(data, 0); ++index)
                if (NC2Lifetime::Read<std::uint32_t>(data, 4u + index * 12u) == 3 && NC2Lifetime::Read<std::uint32_t>(data, 8u + index * 12u) == houseId) return static_cast<std::int32_t>(group);
        }
        return -1;
    }

    // Original function: FUN_004a3179.
    CObjectGroupManager::CObjectGroupManager()
    {
        gObjectGroupManager = this;
        std::memset(this, 0, 0x2F0C);
        RegisterCallbacks(ls_Callback_Logic, {1, 0x16, 4, 8, 0xA}, 10);
    }

    // Original function: FUN_004a31f4.
    CObjectGroupManager::~CObjectGroupManager()
    {
        UnregisterCallbacks(ls_Callback_Logic, {0xA, 8, 4, 0x16, 1});
        gObjectGroupManager = nullptr;
    }
}

namespace NC2InGameGuiDeadHumanDataManager
{
    // Original function: FUN_004b3935; returns the first matching retained record.
    void* CDeadHumanDataManager::DeadHuman_GetDataPtrByHumanUniqueId(std::uint32_t uniqueId) const
    {
        const auto count = Read<std::uint32_t>(this, 0);
        auto* record = Read<std::byte*>(this, 8);
        for (std::uint32_t index = 0; index < count; ++index, record += 0x3C)
            if (Read<std::uint32_t>(record, 0) == uniqueId) return record;
        return nullptr;
    }
    // Original function: FUN_004b3759.
    CDeadHumanDataManager::CDeadHumanDataManager()
    {
        gDeadHumanDataManager = this;
        std::memset(this, 0, 8);
        std::memset(mStorage + 8, 0, 4);
        RegisterCallbacks(ls_Callback_Logic, {4, 3}, 10);
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
        UnregisterCallbacks(ls_Callback_Logic, {3, 4});
        gDeadHumanDataManager = nullptr;
    }

    // Original function: FUN_004b389e.
    void CDeadHumanDataManager::DeadHuman_Add(std::uint32_t humanId)
    {
        void* human = NC2Logic::CHumanArray::mStaticVars + humanId * 0xC58u;
        if (Read<std::uint32_t>(human, 0x10) != NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId()) return;
        void* entry = static_cast<std::byte*>(Read<void*>(this, 8)) + Read<std::uint32_t>(this, 4) * 0x3Cu;
        std::memset(entry, 0, 0x3C);
        Write<std::uint32_t>(entry, 0, Read<std::uint32_t>(human, 8));
        std::strncpy(static_cast<char*>(entry) + 4, reinterpret_cast<const char*>(NC2E2::gDynamicDataHumans.entries + humanId * 0x8C8u + 0x85Cu), 40);
        Write<std::uint32_t>(entry, 0x30, NC2Logic::gGameTime.tick);
        Write<std::uint32_t>(entry, 0x34, Read<std::uint32_t>(human, 0x18));
        Write<std::uint32_t>(entry, 0x38, Read<std::uint32_t>(NC2Logic::Human_GetMapPositionRef(human), 0));
        const auto count = Read<std::uint32_t>(this, 0);
        if (count != 50) Write<std::uint32_t>(this, 0, count + 1u);
        const auto next = Read<std::uint32_t>(this, 4) + 1u;
        Write<std::uint32_t>(this, 4, next == 50 ? 0 : next);
    }
    // Original callback range: 004b397b..004b3991; Mac System_HumanArrayDestructed.
    void CDeadHumanDataManager::System_HumanArrayDestructed() { std::memset(this, 0, 8); }
    // Original function: FUN_004b3957.
    bool __cdecl CDeadHumanDataManager::ls_Callback_Logic(std::uint32_t type, std::uint32_t /*userData*/, std::uint32_t param1, std::uint32_t param2, std::uint32_t /*param3*/)
    {
        auto* manager = static_cast<CDeadHumanDataManager*>(gDeadHumanDataManager);
        if (type == 3 && param1 == 0) manager->System_HumanArrayDestructed();
        else if (type == 4 && param2 == 0) manager->DeadHuman_Add(param1);
        return true;
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
        RegisterCallbacks(ls_Callback_Logic, {1, 0x16, 4, 3, 0x2E}, 10);
        Write<std::uint32_t>(this, 4, 1);
        Write<std::uint32_t>(this, 8, 0);
        auto* active = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x26480, 1));
        if (active != nullptr)
            for (std::uint32_t index = 0; index < 200; ++index) NC2Logic::ToUserMessage_Construct(active + 8u + index * 0x310u);
        Write(this, 0x10, active);
        std::memset(active, 0, 0x26480);
        auto* pending = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x26160, 1));
        if (pending != nullptr)
            for (std::uint32_t index = 0; index < 200; ++index) NC2Logic::ToUserMessage_Construct(pending + 8u + index * 0x30Cu);
        Write(this, 0x14, pending);
        std::memset(pending, 0, 0x26160);
    }

    // Original function: FUN_004b3abd.
    CMessageManager::~CMessageManager()
    {
        NXBasics::Runtime_Free(Read<void*>(this, 0x14));
        NXBasics::Runtime_Free(Read<void*>(this, 0x10));
        UnregisterCallbacks(ls_Callback_Logic, {0x2E, 3, 4, 0x16, 1});
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
        RegisterCallbacks(InputManager_ls_Callback_Logic, {1, 4, 6, 8, 0xA, 0xC, 0x16}, 20);
        Write<std::uint32_t>(this, 0xAB0, 1);
    }

    // Original function: FUN_004b47d9.
    CInputManager::~CInputManager()
    {
        UnregisterCallbacks(InputManager_ls_Callback_Logic, {0x16, 0xC, 0xA, 8, 6, 4, 1});
        gInputManager = nullptr;
    }
}
