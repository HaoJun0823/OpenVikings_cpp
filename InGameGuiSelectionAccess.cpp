#include "InGameGuiInputAccess.h"
#include "InGameGuiManager.h"
#include "InGameGuiSupportAccess.h"
#include "C2CallbackManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2EngineMoveableDrawing.h"
#include "C2GameMapAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MissionStartObjects.h"
#include "C2OverviewObjectIterators.h"
#include "C2VehicleAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2HumanMerchantAccess.h"
#include "C2E2HumanSoundAccess.h"
#include "InGameGuiBaseAccess.h"
#include "C2EngineDisplayManager.h"
#include "XGuiDesktop.h"
#include "XBasicTypes.h"
#include "XCrtFileAccess.h"
#include "XBobRendering.h"

#include <cstring>
#include <initializer_list>

extern void __fastcall InGameGuiManager_Selection_UpdateWindows(void* manager);
extern void __fastcall InGameGuiManager_InputMode_Changed(void* manager);
extern bool __fastcall InputManager_InputMode_Set(void* manager, void* unusedEdx, int mode);

namespace NC2InGameGuiInputManager
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Human(std::uint32_t id) { return NC2Logic::CHumanArray::mStaticVars + id * NC2Logic::CHumanArray::ElementSize; }
        void* Animal(std::uint32_t id) { return NC2Logic::CAnimalArray::mStaticVars + id * NC2Logic::CAnimalArray::ElementSize; }
        void* Vehicle(std::uint32_t id) { return NC2Logic::CVehicleArray::mStaticVars + id * NC2Logic::CVehicleArray::ElementSize; }
        void* House(std::uint32_t id) { return NC2Logic::CHouseArray::mStaticVars + id * NC2Logic::CHouseArray::ElementSize; }
        void* Guide(std::uint32_t id) { return NC2Logic::CGuideArray::mStaticVars + id * NC2Logic::CGuideArray::ElementSize; }
        std::uint32_t Player() { return static_cast<std::uint32_t>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId()); }

        // l_Selection_Changed is inlined in Windows callers before 004a0e9d.
        void Changed(void* manager)
        {
            Write(manager, 0xAB0, Read<std::uint32_t>(manager, 0xAB0) + 1u);
            InGameGuiManager_Selection_UpdateWindows(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
        }

        // Original function: FUN_004b7303.
        void l_Selection_Inform_Human(std::uint32_t id, std::uint32_t revision, bool selected)
        {
            if (id == 0xFFFFFFFFu) return;
            const auto* object = Human(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return;
            NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0xE, true, id, selected ? 3u : 2u, 0);
            if (!selected) static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->Message_RemoveAllForHuman(id);
        }

        // Original function: FUN_004b7361.
        void l_Selection_Inform_Animal(std::uint32_t id, std::uint32_t revision, bool selected)
        {
            if (id == 0xFFFFFFFFu) return;
            const auto* object = Animal(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return;
            NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0xF, true, id, selected ? 3u : 2u, 0);
        }

        // Original function: FUN_004b73ac.
        void l_Selection_Inform_Vehicle(std::uint32_t id, std::uint32_t revision, bool selected)
        {
            if (id == 0xFFFFFFFFu) return;
            const auto* object = Vehicle(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return;
            NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0x10, true, id, selected ? 3u : 2u, 0);
        }

        // Original function: FUN_004b73f7.
        void l_Selection_Inform_House(std::uint32_t id, std::uint32_t revision, bool selected)
        {
            if (id == 0xFFFFFFFFu) return;
            const auto* object = House(id);
            if (Read<std::uint8_t>(object, 0) == 0 || Read<std::uint32_t>(object, 8) != revision) return;
            NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0x11, true, id, selected ? 3u : 2u, 0);
        }

        // Integration dispatch retains the four distinct original helpers.
        void Inform(std::uint32_t type, std::uint32_t id, std::uint32_t revision, bool selected)
        {
            switch (type)
            {
            case 0xE: l_Selection_Inform_Human(id, revision, selected); break;
            case 0xF: l_Selection_Inform_Animal(id, revision, selected); break;
            case 0x10: l_Selection_Inform_Vehicle(id, revision, selected); break;
            case 0x11: l_Selection_Inform_House(id, revision, selected); break;
            default: break;
            }
        }

        bool SelectSingle(void* manager, std::uint32_t mode, std::uint32_t offset, std::uint32_t id, const void* object, std::uint32_t revisionOffset, std::uint32_t callback)
        {
            auto revision = Read<std::uint32_t>(object, revisionOffset);
            if (Read<std::uint32_t>(manager, 4) == mode && Read<std::uint32_t>(manager, offset) == id && Read<std::uint32_t>(manager, offset + 4) == revision) return false;
            Selection_UnselectAll(manager);
            InputMode_Set(manager, 0);
            revision = Read<std::uint32_t>(object, revisionOffset);
            Write(manager, 4, mode); Write(manager, offset, id); Write(manager, offset + 4, revision);
            Changed(manager);
            if (callback != 0) Inform(callback, id, revision, true);
            return true;
        }

        void ResetInvalid(void* manager) { Selection_UnselectAll(manager); InputMode_Set(manager, 0); }

        void ClearObjectHighlights()
        {
            NC2Logic::CHumanIterator humans;
            for (auto id = humans.GetFirstHumanId(); id != -1; id = humans.GetNextHumanId()) NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0xE, true, id, 8, 0);
            for (const auto type : {0xFu, 0x10u})
            {
                const auto* array = type == 0xF ? static_cast<const NC2Logic::CArrayHandler*>(NC2Logic::CAnimalArray::sTheObjectPtr) : static_cast<const NC2Logic::CArrayHandler*>(NC2Logic::CVehicleArray::sTheObjectPtr);
                for (auto id = array->mFirstScanIndex; id <= array->mLastScanIndex; ++id)
                {
                    if (!array->mIsAllocated(array->mEntries + static_cast<std::uint32_t>(id) * array->mElementSize)) continue;
                    NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(type, true, id, 8, 0);
                }
            }
        }

        bool IsLive(const void* object, std::uint32_t revision, std::uint32_t offset = 8) { return Read<std::uint8_t>(object, 0) != 0 && Read<std::uint32_t>(object, offset) == revision; }

        void ValidateGroup(void* manager, bool vehicles)
        {
            const auto countOffset = vehicles ? 0x984u : 0x10u, entriesOffset = vehicles ? 0x988u : 0x14u;
            bool changed = false;
            for (std::uint32_t index = 0; index < Read<std::uint32_t>(manager, countOffset);)
            {
                auto* record = static_cast<std::byte*>(manager) + entriesOffset + index * 8u;
                const auto id = Read<std::uint32_t>(record, 0), revision = Read<std::uint32_t>(record, 4);
                const auto* object = vehicles ? Vehicle(id) : Human(id);
                if (IsLive(object, revision) && Read<std::uint32_t>(object, vehicles ? 0x1E0 : 0x10) == Player()) { ++index; continue; }
                Inform(vehicles ? 0x10 : 0xE, id, revision, false);
                const auto count = Read<std::uint32_t>(manager, countOffset) - 1u;
                Write(manager, countOffset, count);
                if (index != count) std::memmove(record, record + 8, (count - index) * 8u);
                changed = true;
            }
            const auto count = Read<std::uint32_t>(manager, countOffset);
            if (count == 0) ResetInvalid(manager);
            else if (count == 1)
            {
                const auto id = Read<std::int32_t>(manager, entriesOffset);
                if (vehicles) Selection_SelectSingleVehicle(manager, id); else Selection_SelectSingleHuman(manager, id);
            }
            else if (changed) Changed(manager);
        }

        void RemoveFromSelection(void* manager, std::uint32_t id, std::uint32_t singleMode, std::uint32_t singleOffset, std::uint32_t groupMode, std::uint32_t countOffset, std::uint32_t recordsOffset, std::uint32_t callback)
        {
            Selection_Validate(manager);
            const auto mode = Read<std::uint32_t>(manager, 4);
            if (mode == singleMode)
            {
                if (Read<std::uint32_t>(manager, singleOffset) == id) ResetInvalid(manager);
                return;
            }
            if (groupMode == 0 || mode != groupMode) return;
            bool changed = false;
            for (std::uint32_t index = 0; index < Read<std::uint32_t>(manager, countOffset);)
            {
                auto* record = static_cast<std::byte*>(manager) + recordsOffset + index * 8u;
                if (Read<std::uint32_t>(record, 0) != id) { ++index; continue; }
                Inform(callback, id, Read<std::uint32_t>(record, 4), false);
                const auto count = Read<std::uint32_t>(manager, countOffset) - 1u;
                Write(manager, countOffset, count);
                if (index != count) std::memmove(record, record + 8, (count - index) * 8u);
                changed = true;
            }
            const auto count = Read<std::uint32_t>(manager, countOffset);
            if (count == 0) ResetInvalid(manager);
            else if (count == 1)
            {
                const auto remaining = Read<std::int32_t>(manager, recordsOffset);
                if (groupMode == 5) Selection_SelectSingleVehicle(manager, remaining); else Selection_SelectSingleHuman(manager, remaining);
            }
            else if (changed) Changed(manager);
        }
    }

    // Partial original function: FUN_004b4854; normal, home/work/build/route and vehicle-entry modes.
    bool InputMode_Set(void* manager, std::int32_t mode)
    {
        if (Read<std::int32_t>(manager, 0) == mode) return true;
        const auto selection = Read<std::uint32_t>(manager, 4);
        if (mode != 0 && mode != 4 && mode != 5 && mode != 7 && mode != 8 && mode != 9 && mode != 27) return false;
        if (mode != 0 && mode != 27 && selection != 1) return false;
        if (mode == 27 && selection != 1 && selection != 2 && selection != 4) return false;
        if (Read<std::uint8_t>(manager, 0xAB4) != static_cast<std::uint8_t>(mode != 0))
        {
            Write<std::uint8_t>(manager, 0xAB4, mode != 0);
            // Original function: FUN_004832dc; clear only the two highlight bits.
            for (std::int32_t y = NC2Logic::GameMap_GetHeight() - 1; y >= 0; --y)
                for (std::int32_t x = NC2Logic::GameMap_GetWidth() - 1; x >= 0; --x)
                {
                    const std::int16_t point[2]{static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)};
                    auto* micro = const_cast<void*>(NC2E2::EngineMap_GetConstMapMicroPointPtr(point));
                    Write<std::uint8_t>(micro, 2, Read<std::uint8_t>(micro, 2) & 0xFCu);
                }
            auto* world = Read<void*>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 4);
            auto* display = Read<void*>(world, 0x30);
            if (Read<std::uint8_t>(display, 0x74) != static_cast<std::uint8_t>(mode != 0)) { Write<std::uint8_t>(display, 0x74, mode != 0); Write<std::uint8_t>(display, 0x978, 1); }
        }
        if (mode == 0) ClearObjectHighlights();
        else if (mode == 27) { Write(manager, 0, mode); Highlighting_Vehicles_Update(manager, true, false); }
        NXBaseGui::CDesktop::sTheObjectPtr->Element_Activate(static_cast<NXBaseGui::CBaseElement*>(Read<void*>(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, 4)));
        if (mode == 0) NC2E2::C2DEngineDisplayManager::sTheObjectPtr->InformSetExplorationState(true);
        Write<std::uint8_t>(manager, 0xABA, 0); Write<std::int32_t>(manager, 0, mode);
        Cursor_SetPosition(manager, static_cast<std::byte*>(manager) + 0xAB6);
        InGameGuiManager_InputMode_Changed(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
        NC2InGameGuiBase::BaseToolDesktop_SelectionActiveChanged();
        return true;
    }

    // Original function: FUN_004b4a5f.
    void InputMode_Cancel(void* manager)
    {
        const auto mode = Read<std::int32_t>(manager, 0);
        if (mode == 12) { InputMode_Set(manager, 0); Selection_UnselectAll(manager); }
        else if (mode == 0) Selection_UnselectAll(manager);
        else if (mode > 0 && mode <= 28) InputMode_Set(manager, 0);
    }

    // Original function: FUN_004b5eb1; road/wall queries remain unfinished.
    std::uint32_t Action_NormalSelectPossible(void* manager, const void* object, std::uint32_t* first, std::uint32_t* second, std::uint32_t* third, std::uint32_t* fourth)
    {
        *first = *second = *third = *fourth = 0xFFFFFFFFu;
        std::int32_t action = 0, id = -1;
        if (Read<std::uint32_t>(manager, 0) == 0 && object != nullptr)
        {
            switch (Read<std::uint32_t>(object, 0))
            {
            case 1: id = Read<std::int32_t>(object, 8); if (Read<std::uint32_t>(Human(id), 0x10) == Player()) action = 1; break;
            case 2: id = Read<std::int32_t>(object, 0x10); if (Read<std::uint32_t>(Animal(id), 0x10) == Player()) action = 3; break;
            case 3: id = Read<std::int32_t>(object, 0x18); if (Read<std::uint32_t>(Vehicle(id), 0x1E0) == Player()) action = 4; break;
            case 4: id = Read<std::int32_t>(object, 0x28); action = 6; break;
            case 5: id = Read<std::int32_t>(object, 0x30); if (Read<std::uint32_t>(Guide(id), 4) == Player()) action = 8; break;
            case 6: id = Read<std::int32_t>(object, 0x38); *second = Read<std::uint32_t>(object, 0x3C); *third = Read<std::uint32_t>(object, 4); *fourth = Read<std::uint32_t>(object, 0x40); action = 9; break;
            default: break;
            }
        }
        else if (Read<std::uint32_t>(manager, 0) == 0 && Read<std::uint8_t>(manager, 0xABA) != 0)
        {
            const auto* point = static_cast<const std::byte*>(manager) + 0xABC;
            NC2Logic::CMapMoveableIterator iterator;
            const auto moveableId = iterator.GetFirstMapMoveableId(point);
            if (moveableId != -1)
            {
                const auto* moveable = NC2Logic::gMapMoveableArrayStorage.entries + moveableId * 0x58u;
                id = Read<std::int32_t>(moveable, 0x54);
                switch (Read<std::uint32_t>(moveable, 8))
                {
                case 1: if (Read<std::uint32_t>(Human(id), 0x10) == Player()) action = 1; break;
                case 2: if (Read<std::uint32_t>(Animal(id), 0x10) == Player() || Read<std::uint32_t>(Animal(id), 0x10) == 20) action = 3; break;
                case 3: if (Read<std::uint32_t>(Vehicle(id), 0x1E0) == Player() || Read<std::int32_t>(Vehicle(id), 0x1E0) == -1) action = 4; break;
                default: break;
                }
            }
            if (action == 0) { id = NC2Logic::Logic_EntryPoint_House_GetOnPosition(point); if (id != -1 && Read<std::uint32_t>(House(id), 0xC) == Player()) action = 6; }
        }
        if (action != 0) *first = static_cast<std::uint32_t>(id);
        return static_cast<std::uint32_t>(action);
    }

    // Original function: FUN_004b4a9f; road/wall selection remains unfinished.
    bool Action_DoNormalSelect(void* manager, const void* object, bool keepSelection, bool add, bool remove)
    {
        std::uint32_t first, second, third, fourth;
        const auto action = Action_NormalSelectPossible(manager, object, &first, &second, &third, &fourth);
        const auto id = static_cast<std::int32_t>(first);
        switch (action)
        {
        case 1: if (add) Selection_SelectAddSingleHuman(manager, id); else if (remove) UnSelect_Human(manager, id); else Selection_SelectSingleHuman(manager, id); break;
        case 3: Selection_SelectSingleAnimal(manager, id); break;
        case 4: if (add) Selection_SelectAddSingleVehicle(manager, id); else if (remove) UnSelect_Vehicle(manager, id); else Selection_SelectSingleVehicle(manager, id); break;
        case 6: Selection_SelectHouse(manager, id); break;
        case 8: Selection_SelectGuide(manager, id); break;
        case 9:
            if (Read<std::uint32_t>(manager, 4) != 9 || Read<std::uint32_t>(manager, 0xA98) != first || Read<std::uint32_t>(manager, 0xA9C) != second || Read<std::uint32_t>(manager, 0xAA0) != third || Read<std::int32_t>(manager, 0xAA4) != static_cast<std::int32_t>(fourth))
            {
                Selection_UnselectAll(manager); InputMode_Set(manager, 0); Write<std::uint32_t>(manager, 4, 9);
                Write(manager, 0xA98, first); Write(manager, 0xA9C, second); Write(manager, 0xAA0, third); Write(manager, 0xAA4, static_cast<std::int32_t>(fourth)); Changed(manager);
            }
            break;
        default: if (!keepSelection) Selection_UnselectAll(manager); return false;
        }
        return true;
    }

    // Original function: FUN_004b6bb0; record payloads remain untouched.
    void Selection_UnselectAll(void* manager)
    {
        switch (Read<std::uint32_t>(manager, 4))
        {
        case 1: Inform(0xE, Read<std::uint32_t>(manager, 8), Read<std::uint32_t>(manager, 0xC), false); break;
        case 2:
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(manager, 0x10); ++i) Inform(0xE, Read<std::uint32_t>(manager, 0x14u + i * 8u), Read<std::uint32_t>(manager, 0x18u + i * 8u), false);
            break;
        case 3: Inform(0xF, Read<std::uint32_t>(manager, 0x974), Read<std::uint32_t>(manager, 0x978), false); break;
        case 4: Inform(0x10, Read<std::uint32_t>(manager, 0x97C), Read<std::uint32_t>(manager, 0x980), false); break;
        case 5:
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(manager, 0x984); ++i) Inform(0x10, Read<std::uint32_t>(manager, 0x988u + i * 8u), Read<std::uint32_t>(manager, 0x98Cu + i * 8u), false);
            break;
        case 6: Inform(0x11, Read<std::uint32_t>(manager, 0xA78), Read<std::uint32_t>(manager, 0xA7C), false); break;
        default: break;
        }
        Write<std::uint32_t>(manager, 4, 0);
        for (auto offset : {8u, 0x974u, 0x97Cu, 0xA78u, 0xA80u, 0xA84u, 0xA90u, 0xA98u, 0xA9Cu}) Write<std::uint32_t>(manager, offset, 0xFFFFFFFFu);
        for (auto offset : {0xCu, 0x10u, 0x978u, 0x980u, 0xA7Cu, 0xA94u}) Write<std::uint32_t>(manager, offset, 0);
        Changed(manager);
    }

    // Original functions: FUN_004b6115/004b62a9/004b632f/004b655f/004b6672.
    bool Selection_SelectSingleHuman(void* manager, std::int32_t id) { return SelectSingle(manager, 1, 8, id, Human(id), 8, 0xE); }
    bool Selection_SelectSingleAnimal(void* manager, std::int32_t id) { return SelectSingle(manager, 3, 0x974, id, Animal(id), 8, 0xF); }
    bool Selection_SelectSingleVehicle(void* manager, std::int32_t id) { return SelectSingle(manager, 4, 0x97C, id, Vehicle(id), 8, 0x10); }
    bool Selection_SelectHouse(void* manager, std::int32_t id) { return SelectSingle(manager, 6, 0xA78, id, House(id), 8, 0x11); }
    bool Selection_SelectGuide(void* manager, std::int32_t id) { return SelectSingle(manager, 8, 0xA90, id, Guide(id), 0x10, 0); }

    // Original function: FUN_004b618b; at most 300 humans, preserving the old single first.
    bool Selection_SelectAddSingleHuman(void* manager, std::int32_t id)
    {
        const auto mode = Read<std::uint32_t>(manager, 4);
        if (mode != 1 && mode != 2) return Selection_SelectSingleHuman(manager, id);
        if (mode == 1)
        {
            const auto old = Read<std::uint32_t>(manager, 8);
            if (old == static_cast<std::uint32_t>(id)) return false;
            Selection_UnselectAll(manager); InputMode_Set(manager, 0);
            Write<std::uint32_t>(manager, 4, 2); Write<std::uint32_t>(manager, 0x10, 2);
            Write(manager, 0x14, old); Write(manager, 0x18, Read<std::uint32_t>(Human(old), 8));
            Write(manager, 0x1C, id); Write(manager, 0x20, Read<std::uint32_t>(Human(id), 8));
            Changed(manager);
            Inform(0xE, old, Read<std::uint32_t>(Human(old), 8), true);
        }
        else
        {
            const auto count = Read<std::uint32_t>(manager, 0x10);
            for (std::uint32_t i = 0; i < count; ++i) if (Read<std::int32_t>(manager, 0x14u + i * 8u) == id) return false;
            if (count == 300) return false;
            Write(manager, 0x14u + count * 8u, id); Write(manager, 0x18u + count * 8u, Read<std::uint32_t>(Human(id), 8)); Write(manager, 0x10, count + 1u);
            Changed(manager);
        }
        Inform(0xE, id, Read<std::uint32_t>(Human(id), 8), true);
        return true;
    }

    // Original function: FUN_004b63b5; only type-five vehicles form a group, maximum 30.
    bool Selection_SelectAddSingleVehicle(void* manager, std::int32_t id)
    {
        const auto mode = Read<std::uint32_t>(manager, 4);
        if (mode != 4 && mode != 5) return Selection_SelectSingleVehicle(manager, id);
        if (mode == 5)
        {
            const auto count = Read<std::uint32_t>(manager, 0x984);
            for (std::uint32_t i = 0; i < count; ++i) if (Read<std::int32_t>(manager, 0x988u + i * 8u) == id) return false;
            if (count == 30) return false;
            if (Read<std::uint32_t>(Vehicle(Read<std::uint32_t>(manager, 0x988)), 0x1E8) != 5 || Read<std::uint32_t>(Vehicle(id), 0x1E8) != 5) return Selection_SelectSingleVehicle(manager, id);
            Write(manager, 0x988u + count * 8u, id); Write(manager, 0x98Cu + count * 8u, Read<std::uint32_t>(Vehicle(id), 8)); Write(manager, 0x984, count + 1u);
            Changed(manager);
        }
        else
        {
            const auto old = Read<std::uint32_t>(manager, 0x97C);
            if (Read<std::uint32_t>(Vehicle(old), 0x1E8) != 5 || Read<std::uint32_t>(Vehicle(id), 0x1E8) != 5) return Selection_SelectSingleVehicle(manager, id);
            if (old == static_cast<std::uint32_t>(id)) return false;
            Selection_UnselectAll(manager); InputMode_Set(manager, 0);
            Write<std::uint32_t>(manager, 4, 5); Write<std::uint32_t>(manager, 0x984, 2);
            Write(manager, 0x988, old); Write(manager, 0x98C, Read<std::uint32_t>(Vehicle(old), 8));
            Write(manager, 0x990, id); Write(manager, 0x994, Read<std::uint32_t>(Vehicle(id), 8));
            Changed(manager);
            Inform(0x10, old, Read<std::uint32_t>(Vehicle(old), 8), true);
        }
        Inform(0x10, id, Read<std::uint32_t>(Vehicle(id), 8), true);
        return true;
    }

    // Original function: FUN_004b6ce1; construction-type cases remain an explicit boundary.
    void Selection_Validate(void* manager)
    {
        const auto mode = Read<std::uint32_t>(manager, 4);
        bool valid = true;
        switch (mode)
        {
        case 1: valid = IsLive(Human(Read<std::uint32_t>(manager, 8)), Read<std::uint32_t>(manager, 0xC)) && Read<std::uint32_t>(Human(Read<std::uint32_t>(manager, 8)), 0x10) == Player(); break;
        case 2: ValidateGroup(manager, false); return;
        case 3: valid = IsLive(Animal(Read<std::uint32_t>(manager, 0x974)), Read<std::uint32_t>(manager, 0x978)) && Read<std::uint32_t>(Animal(Read<std::uint32_t>(manager, 0x974)), 0x10) == Player(); break;
        case 4: valid = IsLive(Vehicle(Read<std::uint32_t>(manager, 0x97C)), Read<std::uint32_t>(manager, 0x980)) && Read<std::uint32_t>(Vehicle(Read<std::uint32_t>(manager, 0x97C)), 0x1E0) == Player(); break;
        case 5: ValidateGroup(manager, true); return;
        case 6: valid = IsLive(House(Read<std::uint32_t>(manager, 0xA78)), Read<std::uint32_t>(manager, 0xA7C)); break;
        case 8: valid = IsLive(Guide(Read<std::uint32_t>(manager, 0xA90)), Read<std::uint32_t>(manager, 0xA94), 0x10) && Read<std::uint32_t>(Guide(Read<std::uint32_t>(manager, 0xA90)), 4) == Player(); break;
        default: break;
        }
        if (!valid) ResetInvalid(manager);
    }

    // Original functions: FUN_004b7059/004b7159/004b7187/004b72a7/004b72d5.
    void UnSelect_Human(void* manager, std::int32_t id) { RemoveFromSelection(manager, id, 1, 8, 2, 0x10, 0x14, 0xE); }
    void UnSelect_Animal(void* manager, std::int32_t id) { RemoveFromSelection(manager, id, 3, 0x974, 0, 0, 0, 0xF); }
    void UnSelect_Vehicle(void* manager, std::int32_t id) { RemoveFromSelection(manager, id, 4, 0x97C, 5, 0x984, 0x988, 0x10); }
    void UnSelect_House(void* manager, std::int32_t id) { RemoveFromSelection(manager, id, 6, 0xA78, 0, 0, 0, 0x11); }
    void UnSelect_Guide(void* manager, std::int32_t id) { RemoveFromSelection(manager, id, 8, 0xA90, 0, 0, 0, 0); }

    // Original functions: FUN_004b7442/004b7450/004b745d/004b7478.
    std::int32_t Selection_GetSelectedSingleHumanId(const void* manager) { return Read<std::uint32_t>(manager, 4) == 1 ? Read<std::int32_t>(manager, 8) : -1; }
    std::uint32_t Selection_GetSelectedMultipleHumanNumber(const void* manager) { return Read<std::uint32_t>(manager, 4) == 2 ? Read<std::uint32_t>(manager, 0x10) : 0; }
    std::int32_t Selection_GetSelectedMultipleHumanId(const void* manager, std::uint32_t index) { return Read<std::uint32_t>(manager, 4) == 2 && index < Read<std::uint32_t>(manager, 0x10) ? Read<std::int32_t>(manager, 0x14u + index * 8u) : -1; }
    bool Selection_IsHumanSelected(const void* manager, std::int32_t id)
    {
        if (Read<std::uint32_t>(manager, 4) == 1) return Read<std::int32_t>(manager, 8) == id;
        for (std::uint32_t i = 0; i < Selection_GetSelectedMultipleHumanNumber(manager); ++i) if (Selection_GetSelectedMultipleHumanId(manager, i) == id) return true;
        return false;
    }

    // Partial original function: FUN_004c9e2a; complete outlined house/route helper 004ca427 for modes 4..9.
    bool Highlighting_Map_Update(void* manager, const std::int16_t* rectangle)
    {
        const auto mode = Read<std::uint32_t>(manager, 0);
        if (mode < 4 || mode > 9) return false;
        const auto id = Selection_GetSelectedSingleHumanId(manager);
        const void* human = Human(static_cast<std::uint32_t>(id));
        std::uint32_t position;
        const bool positioned = NC2Logic::Human_GetMapPosition(human, &position);
        void* merchant = positioned && NC2Logic::Human_IsJobTrader(human) ? NC2Logic::HumanMerchantArray_GetMerchantDataPtr(NC2Logic::gHumanMerchantArray, Read<std::int32_t>(human, 0xB44)) : nullptr;
        bool changed = false;
        const auto right = static_cast<std::int16_t>(rectangle[0] + rectangle[2] - 1), bottom = static_cast<std::int16_t>(rectangle[1] + rectangle[3] - 1);
        for (std::int32_t y = rectangle[1]; y <= bottom; ++y)
            for (std::int32_t x = rectangle[0]; x <= right; ++x)
            {
                const std::int16_t point[2]{static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)};
                auto* engine = const_cast<void*>(NC2E2::EngineMap_GetConstMapMicroPointPtr(point));
                auto flags = Read<std::uint8_t>(engine, 2);
                std::uint8_t mark = positioned ? 1 : 0;
                if (positioned)
                {
                    const void* micro = NC2Logic::GameMap_GetConstMapMicroPointPtr(point);
                    if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 1u) != 0)
                    {
                        const auto houseId = Read<std::uint16_t>(micro, 4);
                        bool valid = false;
                        switch (mode)
                        {
                        case 4: valid = NC2Logic::House_Inhabitants_CanHumanBeAttachedAsHomie(House(houseId), id); break;
                        case 5: valid = NC2Logic::House_Inhabitants_CanHumanBeAttachedAsWorker(House(houseId), id); break;
                        case 6: valid = NC2Logic::House_Inhabitants_CanHumanBeAttachedAsStudent(House(houseId), id); break;
                        case 7: valid = NC2Logic::House_Inhabitants_CanHumanBeAttachedAsBuilder(House(houseId), id); break;
                        case 8: valid = merchant != nullptr && NC2Logic::MerchantData_Tool_CanHouseBeAdded(merchant, id, houseId); break;
                        case 9: valid = merchant != nullptr && NC2Logic::MerchantData_IsHouseInArray(merchant, houseId, Read<std::uint32_t>(House(houseId), 8)); break;
                        }
                        if (valid) mark = 3;
                    }
                }
                if ((flags & 3u) != mark) { Write<std::uint8_t>(engine, 2, (flags & 0xFCu) | mark); changed = true; }
            }
        return changed;
    }

    // Partial original function: FUN_004b767e; normal mode and native house/route target rings.
    void Cursor_SetPosition(void* manager, const void* point)
    {
        Write(manager, 0xAB6, Read<std::uint32_t>(point, 0));
        auto* cursor = static_cast<std::byte*>(manager) + 0xAB6;
        NC2Logic::MapMigPoint_ValidatePosition(cursor);
        if (Read<std::uint32_t>(manager, 0) >= 4 && Read<std::uint32_t>(manager, 0) <= 9)
        {
            std::int16_t rectangle[4]{static_cast<std::int16_t>(Read<std::int16_t>(cursor, 0) - 7), static_cast<std::int16_t>(Read<std::int16_t>(cursor, 2) - 7), 14, 14};
            // Native SMapMigRectangle construction clips before highlighting.
            NC2Logic::MapMigRectangle_CutInsideMap(rectangle);
            Highlighting_Map_Update(manager, rectangle);
            const auto accept = [&](const void* candidate)
            {
                if ((Read<std::uint8_t>(NC2E2::EngineMap_GetConstMapMicroPointPtr(candidate), 2) & 3u) != 3) return false;
                Write<std::uint8_t>(manager, 0xABA, 1); Write(manager, 0xABC, Read<std::uint32_t>(candidate, 0)); return true;
            };
            if (accept(cursor)) return;
            Write<std::uint8_t>(manager, 0xABA, 0);
            auto candidate = Read<std::uint32_t>(cursor, 0);
            for (std::uint32_t radius = 1; radius <= 5; ++radius)
            {
                NC2Logic::MapMigPoint_MoveCursorWrapped(&candidate, 4);
                for (std::uint32_t direction = 0; direction < 6; ++direction)
                    for (std::uint32_t step = 0; step < radius; ++step)
                    {
                        if (NC2Logic::MapMigPoint_IsInsideMap(&candidate) && accept(&candidate)) return;
                        NC2Logic::MapMigPoint_MoveCursorWrapped(&candidate, direction);
                    }
            }
            return;
        }
        if (Read<std::uint32_t>(manager, 0) != 0) return;
        const bool valid = NC2Logic::MapMigPoint_IsInsideMap(cursor) && !NC2Logic::MapMigPoint_IsBorderPoint(cursor);
        Write<std::uint8_t>(manager, 0xABA, valid);
        if (valid) Write(manager, 0xABC, Read<std::uint32_t>(cursor, 0));
    }

    // Original function: FUN_004b7b08.
    bool Misc_Vehicle_IsToEnter(const void* manager, std::int32_t id)
    {
        const auto selection = Read<std::uint32_t>(manager, 4);
        if (selection == 1)
        {
            const auto human = Read<std::int32_t>(manager, 8);
            return human != -1 && NC2Logic::Vehicle_Passengers_CanHumanBeAttached(Vehicle(id), human);
        }
        if (selection == 4)
        {
            const auto vehicle = Read<std::int32_t>(manager, 0x97C);
            return vehicle != -1 && NC2Logic::Vehicle_Passengers_CanVehicleBeAttached(Vehicle(id), vehicle);
        }
        return false;
    }
    // Partial original function: FUN_004cb137; complete mode-27 and forced-clear branches.
    void Highlighting_Vehicles_Update(void* manager, bool force, bool clear)
    {
        if (!force && Read<std::uint32_t>(manager, 0) != 27) return;
        // The unfiltered Windows iterator scans allocation bytes in ascending array order.
        const auto* array = NC2Logic::CVehicleArray::sTheObjectPtr;
        for (auto id = array->mFirstScanIndex; id <= array->mLastScanIndex; ++id)
        {
            if (Read<std::uint8_t>(Vehicle(id), 0) == 0) continue;
            const bool allowed = !clear && Read<std::uint32_t>(manager, 0) == 27 && Misc_Vehicle_IsToEnter(manager, id);
            NC2Logic::CCallbackManager::sTheObjectPtr->ExecuteCallback(0x10, true, id, allowed ? 12 : 8, 0);
        }
    }
    // Partial original function: FUN_004b4bd6; native home/work/build/route and vehicle-entry branches.
    bool Action_DoInputSelect(void* manager, const void* object)
    {
        const auto mode = Read<std::uint32_t>(manager, 0);
        if (mode == 27)
        {
            if (object == nullptr || Read<std::uint32_t>(object, 0) != 3) return false;
            const auto id = Read<std::int32_t>(object, 0x18);
            if (!Misc_Vehicle_IsToEnter(manager, id)) return false;
            const auto selection = Read<std::uint32_t>(manager, 4);
            if (selection == 1 || selection == 4)
            {
                std::byte command[20]{};
                Write<std::uint8_t>(command, 0, selection == 1 ? 38 : 113); Write(command, 4, Read<std::uint32_t>(manager, selection == 1 ? 8 : 0x97C));
                Write(command, 0x0C, id); Write(command, 0x10, Read<std::uint32_t>(Vehicle(id), 8));
                if (selection == 1)
                {
                    NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
                    NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Read<std::int32_t>(manager, 8));
                }
                else NC2Logic::UserNetworkCommand_AddVehicleCommand(NC2Logic::gUserNetworkCommandManager, command);
            }
            InputMode_Cancel(manager);
            return true;
        }
        if ((mode != 4 && mode != 5 && mode != 7 && mode != 8 && mode != 9) || Read<std::uint8_t>(manager, 0xABA) == 0) return false;
        const auto* micro = NC2Logic::GameMap_GetConstMapMicroPointPtr(static_cast<std::byte*>(manager) + 0xABC);
        if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 1u) != 0)
        {
            const auto id = Read<std::uint16_t>(micro, 4);
            std::byte command[20]{};
            const auto opcode = mode == 4 ? 26 : mode == 5 ? 28 : mode == 7 ? 30 : mode == 8 ? 36 : 37;
            Write<std::uint8_t>(command, 0, static_cast<std::uint8_t>(opcode)); Write(command, 4, Read<std::uint32_t>(manager, 8));
            Write<std::uint32_t>(command, 0x0C, id); Write(command, 0x10, Read<std::uint32_t>(House(id), 8));
            NC2Logic::UserNetworkCommand_AddHumanCommand(NC2Logic::gUserNetworkCommandManager, command);
            NC2E2::E2HumanSoundManager_PlayRespondingSound(NC2E2::gE2HumanSoundManager, 0, Read<std::int32_t>(manager, 8));
        }
        InputMode_Cancel(manager);
        return true;
    }

    // Original function: FUN_004b5cff; object is deliberately unused by Windows.
    bool Action_DoDefault(void* manager, const void*)
    {
        if (Read<std::uint32_t>(manager, 0) != 0) return false;
        const auto mode = Read<std::uint32_t>(manager, 4);
        if (mode != 1 && mode != 2) return false; // Vehicle defaults remain a separate reconstruction boundary.
        auto* cursor = static_cast<std::byte*>(manager) + 0xAB6;
        std::uint32_t position = 0;
        const void* human = Human(Read<std::uint32_t>(manager, mode == 1 ? 8 : 0x14));
        if (mode == 1)
        {
            const auto player = Player();
            if (player >= 16 || (Read<std::uint16_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(cursor), 8) & (1u << player)) != 0) return false;
        }
        if (!NC2Logic::Human_GetMapPosition(human, &position)) return false;
        if (Read<std::uint8_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(cursor), 6) != Read<std::uint8_t>(NC2Logic::GameMap_GetConstMapMicroPointPtr(&position), 6)) return false;
        Misc_Tool_SendSelectedHuman(manager, cursor);
        return true;
    }

    // Original function: FUN_004b5e91.
    bool Action_DoCancel(void* manager, const void*)
    {
        const auto mode = Read<std::int32_t>(manager, 0);
        if (mode == 0) { Selection_UnselectAll(manager); return false; }
        if (mode > 0 && mode <= 28) InputMode_Cancel(manager);
        return true;
    }

    // Original function: FUN_004b6852; area hit testing uses every current animation bob.
    bool Selection_SelectHumansInArea(void* manager, const void* worldElement, NXBasics::SRectangle rectangle, std::uint32_t job, bool add, bool remove)
    {
        rectangle.Validate();
        const auto* display = Read<void*>(worldElement, 0x30);
        std::int16_t area[4]{};
        NC2E2::DisplayDescription_GetMapRectangle(display, rectangle.x, rectangle.y, rectangle.width, rectangle.height, area);
        area[0] = static_cast<std::int16_t>(area[0] - 5); area[2] = static_cast<std::int16_t>(area[2] + 10);
        area[1] = static_cast<std::int16_t>(area[1] - 4); area[3] = static_cast<std::int16_t>(area[3] + 24);
        NC2Logic::MapMigRectangle_CutInsideMap(area);
        const bool military = job >= 42 && job <= 47;
        if (military) job = 0;
        std::uint32_t ids[300], count = 0;
        NC2Logic::CMapMoveableIterator iterator;
        for (std::int32_t y = area[1]; y <= static_cast<std::int16_t>(area[1] + area[3] - 1) && count != 300; ++y)
        {
            for (std::int32_t x = area[0]; x <= static_cast<std::int16_t>(area[0] + area[2] - 1) && count != 300; ++x)
            {
                const std::int16_t point[2]{static_cast<std::int16_t>(x), static_cast<std::int16_t>(y)};
                std::int32_t pixel[2];
                NC2E2::DisplayDescription_MapMigToWorldPixelCoordinates(display, Read<std::uint32_t>(point, 0), pixel, true);
                for (auto id = iterator.GetFirstMapMoveableId(point); id != -1 && count != 300; id = iterator.GetNextMapMoveableId())
                {
                    const auto* moveable = NC2Logic::gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                    if (Read<std::uint32_t>(moveable, 8) != 1) continue;
                    const auto humanId = Read<std::uint32_t>(moveable, 0x54);
                    const auto* human = Human(humanId);
                    const auto humanJob = Read<std::uint32_t>(human, 0x18);
                    if (Read<std::uint32_t>(human, 0x10) != Player() || NC2Logic::Human_Misc_IsHidden(human) || (job != 0 && humanJob != job) || (military && humanJob < 42) || humanJob > 47) continue;
                    NC2E2::S2DEngineDisplaySearchObjectBobData bobs;
                    NC2E2::DrawAtom_MapMoveable_GetBobData(static_cast<std::uint32_t>(id), display, &bobs, Player());
                    std::uint32_t bob = 0;
                    while (bob < bobs.count)
                    {
                        const auto& data = bobs.bobs[bob];
                        const auto* bobRectangle = NXBasics::BobManager_GetBobAreaRectanglePtr(data.manager, data.id);
                        if (bobRectangle != nullptr)
                        {
                            auto bobArea = *static_cast<const NXBasics::SRectangle*>(bobRectangle);
                            bobArea.x += data.offsetX + pixel[0]; bobArea.y += data.offsetY + pixel[1];
                            if (bobArea.IsTouching(rectangle)) break;
                        }
                        ++bob;
                    }
                    if (bob != bobs.count) ids[count++] = humanId;
                }
            }
        }
        if (add) for (std::uint32_t i = 0; i < count; ++i) Selection_SelectAddSingleHuman(manager, ids[i]);
        else if (remove) for (std::uint32_t i = 0; i < count; ++i) UnSelect_Human(manager, ids[i]);
        else if (count == 1) return Selection_SelectSingleHuman(manager, ids[0]);
        else if (count > 1)
        {
            Selection_UnselectAll(manager); InputMode_Set(manager, 0);
            Write<std::uint32_t>(manager, 4, 2); Write(manager, 0x10, count);
            for (std::uint32_t i = 0; i < count; ++i) { Write(manager, 0x14u + i * 8u, ids[i]); Write(manager, 0x18u + i * 8u, Read<std::uint32_t>(Human(ids[i]), 8)); }
            Changed(manager);
            for (std::uint32_t i = 0; i < count; ++i) Inform(0xE, ids[i], Read<std::uint32_t>(Human(ids[i]), 8), true);
        }
        return true;
    }
}

// ABI adapter used by the existing GUI constructors and command handlers.
// Partial original function: FUN_004a2f54; mode 8 is connected.
bool __cdecl C2IGG_EntryPoint_InputManager_Highlighting_Update(const std::int16_t* rectangle) { return NC2InGameGuiInputManager::Highlighting_Map_Update(NC2InGameGuiInputManager::gInputManager, rectangle); }
bool __fastcall InputManager_InputMode_Set(void* manager, void*, int mode) { return NC2InGameGuiInputManager::InputMode_Set(manager, mode); }

// Original function: FUN_004b77e8; normal-mode validation/destruction callbacks.
// Positive command-mode highlighting remains excluded from completion counts.
bool __cdecl InputManager_ls_Callback_Logic(unsigned int type, unsigned int, unsigned int id, unsigned int created, unsigned int)
{
    using namespace NC2InGameGuiInputManager;
    auto* manager = gInputManager;
    if (type == 1) Selection_Validate(manager);
    else if (type == 22) { Selection_Validate(manager); Selection_UnselectAll(manager); const std::uint32_t point = 0; Cursor_SetPosition(manager, &point); }
    else if (created == 0)
    {
        switch (type)
        {
        case 4: UnSelect_Human(manager, id); break;
        case 6: UnSelect_Animal(manager, id); break;
        case 8: UnSelect_Vehicle(manager, id); break;
        case 10: UnSelect_House(manager, id); break;
        case 12: UnSelect_Guide(manager, id); break;
        default: break;
        }
    }
    return true;
}

// Original function: FUN_004a0d01; preserve the fourteen desktop element notifications.
void __fastcall InGameGuiManager_InputMode_Changed(void* manager)
{
    const bool normal = NC2Lifetime::Read<std::uint32_t>(NC2InGameGuiInputManager::gInputManager, 0) == 0;
    auto* desktop = NXBaseGui::CDesktop::sTheObjectPtr;
    for (std::uint32_t offset = 0xC; offset <= 0x40; offset += 4)
    {
        auto* element = NC2Lifetime::Read<NXBaseGui::CBaseElement*>(manager, offset);
        if (normal) desktop->Element_UnHide(element); else desktop->Element_Hide(element);
    }
    NC2InGameGuiBase::BaseToolDesktop_SelectionActiveChanged();
}
