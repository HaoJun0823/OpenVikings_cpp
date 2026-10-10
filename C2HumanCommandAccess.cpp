#include "C2HumanCommandAccess.h"
#include "C2HumanMerchantAccess.h"
#include "C2HumanAtomicEvents.h"
#include "C2HumanJobAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2HouseAccess.h"
#include "C2VehicleAccess.h"
#include "C2InteractionTargetAccess.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2GameMapAccess.h"
#include "C2OverviewObjectIterators.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2HumanTaskAccess.h"
#include "C2HumanNeeds.h"
#include "C2HumanInitialization.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanWalking.h"
#include "C2HumanGoods.h"
#include "C2MissionStartObjects.h"
#include "C2UserMessageAccess.h"
#include "C2HumanEquipment.h"
#include "C2HumanFamily.h"
#include "C2HumanScout.h"
#include "C2HumanWorkCommands.h"
#include "C2HumanChest.h"
#include <cstring>
namespace NC2Logic
{
    // Original function: FUN_0043623a; both trailing parameters are unused.
    std::uint32_t Human_UserControl_GetDefaultInteractionCommand_TrySendTo(const void* human, void* command, const void* point, bool, bool)
    {
        using namespace NC2Lifetime;
        if ((Read<std::uint32_t>(human, 0xB84) & 0x20u) != 0) return 0;
        const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0xC) * 0x58u;
        if (Read<std::uint8_t>(moveable, 0x10) == 0) return 0;
        const auto* target = GameMap_GetConstMapMicroPointPtr(point);
        const auto* source = GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(moveable) + 0x12);
        if (Read<std::uint8_t>(target, 6) != Read<std::uint8_t>(source, 6)) return 0;
        const auto player = Read<std::uint32_t>(human, 0x10);
        if (player < 16 && (Read<std::uint16_t>(target, 8) & (1u << player)) == 0) return 0;
        std::memset(command, 0, 0x14);
        Write<std::uint8_t>(command, 0, 4); Write(command, 4, Read<std::uint32_t>(human, 4)); Write(command, 8, Read<std::uint32_t>(human, 8)); Write(command, 0xC, MapMigPoint_GetLong(point));
        return 2;
    }

    // Original function: FUN_0042cc19.
    bool Human_IsAtWorkCenter(const void* human) { return NC2Lifetime::Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == NC2Lifetime::Read<std::uint32_t>(human, 0xB30); }

    // Original function: FUN_0042cecb.
    bool Human_Target_HasTarget(const void* human) { return NC2Lifetime::Read<std::uint32_t>(human, 0xA68) != 0; }

    // Original function: FUN_0042cc40; complete PE selector 0042CD3D..0042CD5C.
    bool Human_IsAtTargetPoint(const void* human)
    {
        using namespace NC2Lifetime;
        if (!Human_Target_IsValid(human)) return false;
        switch (Read<std::uint32_t>(human, 0xA68))
        {
        case 1: case 3: case 4: case 5:
            return Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(human, 0xA70);
        case 2:
            return Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(human, 0xA70) && Human_GetLandscapeTypeAtTargetPosition(human) == Read<std::uint32_t>(human, 0xA74);
        case 6:
        {
            const void* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
            std::uint32_t entry = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &entry, 0);
            const auto kind = Read<std::uint32_t>(vehicle, 0x1E8);
            if ((kind == 3 || kind == 4) && Read<std::uint8_t>(vehicle, 0x1D0) == 0) return false;
            return Vehicle_Door_IsEntryPointOnLand(vehicle, 0) && Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == entry;
        }
        case 7: case 8: return true;
        default: return false;
        }
    }

    // Original function: FUN_0042ced7; a passenger may target its own vehicle
    // even when the general interaction-target validation fails.
    bool Human_Target_IsValid(const void* human)
    {
        using namespace NC2Lifetime;
        if (InteractionTargetData_IsValid(static_cast<const std::byte*>(human) + 0xA68)) return true;
        return Read<std::uint32_t>(human, 0xA68) == 6 && Read<std::uint32_t>(human, 0xA7C) == Read<std::uint32_t>(human, 0xB48) && Read<std::uint8_t>(human, 0xB51) != 0;
    }

    // Partial original function: FUN_0044314a; Mac CurrentTask_DoPerform.
    // Established events run before incrementing time; unsupported event trees are held.
    void Human_CurrentTask_DoPerform(void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0xAB8) == 0)
        {
            Write<std::uint32_t>(human, 0x370, 0);
            Write<std::uint32_t>(human, 0x368, 1);
            Write<std::uint32_t>(human, 0x36C, 0);
            return;
        }
        if (Read<std::uint8_t>(human, 0xABC) == 0) return;
        const auto& animation = gStaticAtomicAnimationData[Read<std::uint32_t>(human, 0xAC0)];
        if (!Human_AreAtomicEventsReconstructed(human, animation)) return;
        for (std::uint32_t index = 0; index < animation.eventCount; ++index)
            if (static_cast<std::uint32_t>(animation.events[index].time) == Read<std::uint32_t>(human, 0xAC4)) Human_l_PerformTaskEvent(human, animation.events[index]);
        const auto done = Read<std::uint32_t>(human, 0xAC4) + 1u;
        Write(human, 0xAC4, done);
        if (done != static_cast<std::uint32_t>(animation.length)) return;
        if (Read<std::uint32_t>(human, 0x36C) == 1u) Write<std::uint32_t>(human, 0x36C, 2);
        if (animation.endDirection != -1)
        {
            void* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            Write(moveable, 0x20, animation.endDirection);
        }
        Human_l_ResetAtomic(human);
    }

    // Original function: FUN_0042c5aa.
    static bool Human_IsJobWoman(const void* human) { return NC2Lifetime::Read<std::uint32_t>(human, 0x18) == 5; }

    // Original function: FUN_00441cb4; all five Windows circulation modes release their reservations.
    void Human_StopCurrentGoodCirculationMode(void* human)
    {
        using namespace NC2Lifetime;
        const auto houseById = [](std::uint32_t id) { return CHouseArray::mStaticVars + id * CHouseArray::ElementSize; };
        switch (Read<std::uint32_t>(human, 0xACC))
        {
        case 1:
            if (auto* house = Human_GetTargetHousePtr(human)) House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD0), 1, false);
            break;
        case 2:
        {
            if (Read<std::uint32_t>(human, 0xA68) == 8) break;
            auto* house = Human_GetTargetHousePtr(human);
            if (house != nullptr && (House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0xAD0), true) || House_Stock_CanStoreGoodType(house, Read<std::uint32_t>(human, 0xAD0), false))) House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD0), 1, false);
            if (Read<std::uint32_t>(human, 0xA68) == 6 && InteractionTargetData_IsValid(static_cast<std::byte*>(human) + 0xA68))
            {
                auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
                if (Vehicle_Stock_CanStoreGoodType(vehicle, Read<std::uint32_t>(human, 0xAD0))) Vehicle_Stock_ModifyFutureAmount(vehicle, Read<std::uint32_t>(human, 0xAD0), 1, Read<std::uint32_t>(human, 8));
            }
            if (Human_IsAttachedToWorkHouse(human)) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 0xAD0), -1, true);
            break;
        }
        case 3:
            if (auto* house = Human_GetTargetHousePtr(human)) House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD0), 1, false);
            if (Human_IsAttachedToHomeHouse(human)) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB28)), Read<std::uint32_t>(human, 0xAD0), -1, true);
            break;
        case 4:
        {
            if (Read<std::uint32_t>(human, 0xA68) == 8) break;
            if (auto* house = Human_GetTargetHousePtr(human)) House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD0), -1, true);
            else if (Read<std::uint32_t>(human, 0xA68) == 6)
            {
                if (!InteractionTargetData_IsValid(static_cast<std::byte*>(human) + 0xA68)) break;
                auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize;
                if (Vehicle_Stock_CanStoreGoodType(vehicle, Read<std::uint32_t>(human, 0xAD0))) Vehicle_Stock_ModifyFutureAmount(vehicle, Read<std::uint32_t>(human, 0xAD0), -1, Read<std::uint32_t>(human, 8));
            }
            else if (Human_IsAttachedToWorkHouse(human)) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 0xAD0), -1, true);
            else if (Human_IsJobWoman(human) && Human_IsAttachedToHomeHouse(human)) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB28)), Read<std::uint32_t>(human, 0xAD0), -1, true);
            break;
        }
        case 5:
            if (Read<std::uint32_t>(human, 0xAD4) != 0)
            {
                if (auto* house = Human_GetTargetHousePtr(human)) House_Stock_ModifyFutureAmount(house, Read<std::uint32_t>(human, 0xAD4), -1, false);
            }
            else if (Human_IsJobWoman(human))
            {
                if (Human_IsAttachedToHomeHouse(human) && Read<std::uint32_t>(human, 0xA68) == 5) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB28)), Read<std::uint32_t>(human, 0xAD0), 1, false);
            }
            else
            {
                if (Human_IsAttachedToWorkHouse(human) && Read<std::uint32_t>(human, 0xA68) == 5) House_Stock_ModifyFutureAmount(houseById(Read<std::uint32_t>(human, 0xB34)), Read<std::uint32_t>(human, 0xAD0), 1, false);
                if (Read<std::uint32_t>(human, 0xA68) != 6) break;
                auto* vehicle = CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xA7C) * CVehicleArray::ElementSize;
                if (Read<std::uint8_t>(vehicle, 0) == 0 || Read<std::uint32_t>(human, 0xA80) != Read<std::uint32_t>(vehicle, 8)) break;
                if (Vehicle_Stock_CanStoreGoodType(vehicle, Read<std::uint32_t>(human, 0xAD0))) Vehicle_Stock_ModifyFutureAmount(vehicle, Read<std::uint32_t>(human, 0xAD0), 1, Read<std::uint32_t>(human, 8));
            }
            break;
        }
        Write<std::uint32_t>(human, 0xACC, 0);
    }

    // Original function: FUN_00443dab.
    bool Human_StopWaitingForOtherHuman(void* human)
    {
        using namespace NC2Lifetime;
        const auto waiting = Read<std::uint32_t>(human, 0x378);
        if (waiting == 0)
        {
            Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
            return false;
        }
        Write(human, 0x378, waiting - 1u);
        if (waiting == 1)
        {
            Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
            if (Read<std::uint32_t>(human, 0x368) == 9) Write<std::uint32_t>(human, 0x368, 1);
        }
        return true;
    }

    // Original function: FUN_00438d0a; the other human's occupied flag is checked.
    void Human_StopWaitingForOtherHuman_Detach(void* human)
    {
        using namespace NC2Lifetime;
        const auto otherId = Read<std::uint32_t>(human, 0xB1C);
        if (otherId == 0xFFFFFFFFu) return;
        auto* other = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(other, 0) != 0) Human_StopWaitingForOtherHuman(other);
        Write<std::uint32_t>(human, 0xB1C, 0xFFFFFFFFu);
    }

    // Original function: FUN_0042cea7.
    void Human_Target_Reset(void* human) { NC2Lifetime::Write<std::uint32_t>(human, 0xA68, 0); }

    // Original function: FUN_0042ceb7.
    void Human_Target_SetType(void* human, std::uint32_t type) { NC2Lifetime::Write(human, 0xA68, type); }

    // Original function: FUN_0042d18b; a non-landscape target has type zero.
    std::uint32_t Human_GetLandscapeTypeAtTargetPosition(const void* human)
    {
        using namespace NC2Lifetime;
        const void* point = GameMap_GetConstMapMicroPointPtr(static_cast<const std::byte*>(human) + 0xA6C);
        return Read<std::uint8_t>(point, 0) == 1 ? Read<std::uint8_t>(point, 2) : 0;
    }

    // Original function: FUN_0042d45a.
    std::uint32_t Human_Equipment_GetWeaponType(const void* human) { return NC2Lifetime::Read<std::uint32_t>(human, 0xAE4); }

    // Original function: FUN_00462ec6; arriving updates the current task before AI dispatch.
    void Human_AI_DoNewMapPositionReached_DestinationReached(void* human)
    {
        using namespace NC2Lifetime;
        const auto task = Read<std::uint32_t>(human, 0x368);
        switch (task)
        {
        case 2: case 4: case 23: case 24: case 25: case 26: case 27: case 28: case 29:
            Write<std::uint32_t>(human, 0x36C, 2);
            return;
        case 5: case 13:
            if (Read<std::uint32_t>(human, 0x36C) == 1) Write<std::uint32_t>(human, 0x36C, 2);
            return;
        case 7:
            if (Read<std::uint32_t>(human, 0x36C) != 1) return;
            switch (Read<std::uint32_t>(human, 0x370))
            {
            case 1: Write<std::uint32_t>(human, 0x36C, 0); return;
            case 2:
                if (Human_GetLandscapeTypeAtTargetPosition(human) == 4) Write<std::uint32_t>(human, 0x36C, 2);
                else { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 3); }
                return;
            case 3:
            {
                CMapMoveableIterator iterator;
                for (auto id = iterator.GetFirstMapMoveableId(Human_GetMapPositionRef(human)); id != -1; id = iterator.GetNextMapMoveableId())
                {
                    const void* moveable = gMapMoveableArrayStorage.entries + static_cast<std::uint32_t>(id) * 0x58u;
                    if (Read<std::uint32_t>(moveable, 8) == 1 && Read<std::uint32_t>(moveable, 0x54) != Read<std::uint32_t>(human, 4))
                    {
                        Write<std::uint32_t>(human, 0x36C, 2);
                        return;
                    }
                }
                Write<std::uint32_t>(human, 0x36C, 3);
                Write<std::uint32_t>(human, 0x374, 6);
                return;
            }
            case 4: Write<std::uint32_t>(human, 0x36C, 2); return;
            default: return;
            }
        case 14: case 15:
        {
            Human_Target_Reset(human);
            const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)]);
            if (static_cast<std::uint32_t>(weapons[Human_Equipment_GetWeaponType(human)].minimumRange) > 1u)
            {
                Write<std::uint32_t>(human, 0x37C, 0);
                Write<std::uint32_t>(human, 0x36C, 0);
                Write<std::uint32_t>(human, 0x368, 1);
            }
            else Write<std::uint32_t>(human, 0x36C, 2);
            return;
        }
        default: return;
        }
    }

    // Original function: FUN_004632c5; Windows clears carrier circulation state.
    void Human_AI_DoPathfinderFailed(void* human)
    {
        using namespace NC2Lifetime;
        const auto task = Read<std::uint32_t>(human, 0x368);
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 1);
        if (task == 5 || task == 13) Human_StopWaitingForOtherHuman_Detach(human);
        if (Human_IsJobCarrier(human))
        {
            Write<std::uint32_t>(human, 0x218, 0);
            Write<std::uint32_t>(human, 0x1C, 0);
            Write<std::uint32_t>(human, 0x38C, 0);
        }
    }

    // Original function: FUN_00463311; interrupt only at an interruptable atomic.
    void Human_AI_DoPathfinderStopped(void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0x368) == 2) Write<std::uint32_t>(human, 0x36C, 3);
        if (Human_l_IsAtomicInterruptable(human) && Human_UserControl_IsPriorityCommandInQueue(human)) Human_CurrentTask_DoInterrupt(human);
    }

    // Original function: FUN_00434deb; complete PE table 00434E0E..00434E6A.
    bool Human_UserControl_IsAPriorityCommand(std::uint32_t opcode)
    {
        switch (opcode)
        {
        case 3: case 4: case 13: case 17: case 21: case 22: case 23: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 41: case 42: case 43: case 44: case 45: case 46: case 47: case 48: case 49: case 50: case 51: return true;
        default: return false;
        }
    }

    // Original function: FUN_00434e6b; separate predicate from priority commands.
    bool Human_UserControl_IsAInterruptingCommand(std::uint32_t opcode)
    {
        switch (opcode)
        {
        case 3: case 4: case 13: case 21: case 22: case 25: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 38: case 39: case 43: case 44: case 45: case 46: case 47: case 48: return true;
        default: return false;
        }
    }

    // Original function: FUN_00434ed0; all 87 command slots are checked in order.
    bool Human_UserControl_IsPriorityCommandInQueue(const void* human)
    {
        for (std::uint32_t index = 0; index < 87; ++index)
        {
            const auto opcode = NC2Lifetime::Read<std::uint8_t>(human, 0x390u + index * 20u);
            if (opcode != 0 && Human_UserControl_IsAPriorityCommand(opcode)) return true;
        }
        return false;
    }

    // Original function: FUN_00434f03; return the first matching slot or -1.
    std::int32_t Human_UserControl_GetPriorityCommandIndexInQueue(const void* human)
    {
        for (std::int32_t index = 0; index < 87; ++index)
        {
            const auto opcode = NC2Lifetime::Read<std::uint8_t>(human, 0x390u + static_cast<std::uint32_t>(index) * 20u);
            if (opcode != 0 && Human_UserControl_IsAPriorityCommand(opcode)) return index;
        }
        return -1;
    }

    // Original function: FUN_004355fc.
    std::uint32_t Human_UserControl_GetLastCommandType(const void* human)
    {
        return NC2Lifetime::Read<std::uint32_t>(human, 0xA5C) + 2u;
    }

    // Original function: FUN_004355da; zero is also a searchable command type.
    bool Human_UserControl_IsCommandTypeInQueue(const void* human, std::uint8_t opcode)
    {
        for (std::uint32_t slot = 0; slot < 87; ++slot) if (NC2Lifetime::Read<std::uint8_t>(human, 0x390u + slot * 20u) == opcode) return true;
        return false;
    }

    // Original function: FUN_0044320e; Windows tests atomic interruptibility once.
    void Human_CurrentTask_DoInterrupt(void* human)
    {
        using namespace NC2Lifetime;
        if (!Human_l_IsAtomicInterruptable(human)) return;
        Write<std::uint32_t>(human, 0x37C, 0);
        Write<std::uint32_t>(human, 0x378, 0);
        Write<std::uint32_t>(human, 0x380, 0);
        switch (Read<std::uint32_t>(human, 0x368))
        {
        case 2:
        case 8:
            std::memcpy(static_cast<std::byte*>(human) + 0xA84, static_cast<const std::byte*>(human) + 0xA68, 28);
            Write(human, 0xA94, Read<std::uint32_t>(human, 0xAD0));
            Write(human, 0xAD8, Read<std::uint32_t>(human, 0xACC));
            break;
        case 3:
            Human_DoAddNewUserCommand(human, 6, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 4:
            Write<std::uint32_t>(human, 0xA84, 1);
            Write(human, 0xA88, Read<std::uint32_t>(human, 0xA6C));
            break;
        case 5:
        {
            const auto otherId = Read<std::uint32_t>(human, 0xB1C);
            if (otherId != 0xFFFFFFFFu)
            {
                auto* other = CHumanArray::mStaticVars + otherId * CHumanArray::ElementSize;
                Human_StopWaitingForOtherHuman(other);
                Human_StopWaitingForOtherHuman_Detach(other);
            }
            Human_DoAddNewUserCommand(human, 14, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        }
        case 11:
            Human_DoAddNewUserCommand(human, 15, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 12:
            Human_DoAddNewUserCommand(human, 16, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            break;
        case 16:
        case 17:
            Write(human, 0x390u + Read<std::uint32_t>(human, 0xA5C) * 20u, static_cast<std::uint8_t>(Human_UserControl_GetLastCommandType(human)));
            break;
        case 18:
            Human_DoAddNewUserCommand(human, 18, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(static_cast<const std::byte*>(human) + 0xA70), 0);
            break;
        case 20:
            Human_DoAddNewUserCommand(human, 20, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(static_cast<const std::byte*>(human) + 0xB30), 0);
            break;
        default: break;
        }
            Human_StopCurrentGoodCirculationMode(human);
        Write<std::uint32_t>(human, 0xAD0, 0);
        Write<std::uint32_t>(human, 0x370, 0);
        Human_StopWaitingForOtherHuman_Detach(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 1);
    }

    // Original function: FUN_0045cc64; an inactive non-moveable atomic is interruptable.
    bool Human_l_IsAtomicInterruptable(const void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0xAB8) == 1)
        {
            const auto* moveable = gMapMoveableArrayStorage.entries + Read<std::uint32_t>(human, 0x0C) * 0x58u;
            return Read<std::uint8_t>(moveable, 0x2C) == 0;
        }
        if (Read<std::uint8_t>(human, 0xABC) == 0) return true;
        return Read<std::uint8_t>(gStaticAtomicAnimationData, Read<std::uint32_t>(human, 0xAC0) * 0x138u + 0x38u) != 0;
    }

    // Original function: FUN_0043bb2d; the structured-command overload delegates.
    bool Human_DoAddNewUserCommand(void* human, const void* command)
    {
        using namespace NC2Lifetime;
        return Human_DoAddNewUserCommand(human, Read<std::uint8_t>(command, 0), Read<std::uint8_t>(command, 1), Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8), Read<std::uint32_t>(command, 12), Read<std::uint32_t>(command, 16));
    }

    // Original function: FUN_0043bb4e; the opcode selects an unchecked 20-byte slot.
    bool Human_DoAddNewUserCommand(void* human, std::uint8_t opcode, std::uint8_t player, std::uint32_t id, std::uint32_t revision, std::uint32_t parameter1, std::uint32_t parameter2)
    {
        using namespace NC2Lifetime;
        if (opcode == 0) return true;
        if (opcode == 4)
        {
            if (Read<std::uint32_t>(human, 0xAB8) == 1 && Read<std::uint32_t>(human, 0x368) != 15)
            {
                std::uint32_t target;
                MapMigPoint_AssignLong(&target, parameter1);
                if (target == Read<std::uint32_t>(human, 0xA6C)) return true;
            }
            Write<std::uint32_t>(human, 0x37C, 0);
            Write<std::uint8_t>(human, 0x4F8, 0);
            Write<std::uint8_t>(human, 0x4E4, 0);
        }
        if (opcode >= 0x2B && opcode <= 0x2F)
        {
            Write<std::uint32_t>(human, 0x37C, 0);
            for (const auto offset : {0x9D0u, 0x9E4u, 0x9F8u, 0xA0Cu, 0xA20u, 0xA34u, 0x994u, 0x9A8u, 0x9BCu}) Write<std::uint8_t>(human, offset, 0);
        }
        const auto offset = 0x390u + (static_cast<std::uint32_t>(opcode) - 2u) * 20u;
        Write(human, offset, opcode);
        Write(human, offset + 1, player);
        Write(human, offset + 4, id);
        Write(human, offset + 8, revision);
        Write(human, offset + 12, parameter1);
        Write(human, offset + 16, parameter2);
        if (Read<std::uint32_t>(human, 0x368) == 1 && Human_l_IsAtomicInterruptable(human)) Human_CurrentTask_DoInterrupt(human);
        return true;
    }
    // Original function: FUN_0043bc7d.
    void Human_DoCheckAllUserCommands(void* human)
    {
        using namespace NC2Lifetime;
        for (std::uint32_t slot = 0; slot < 87; ++slot)
        {
            auto* command = static_cast<std::byte*>(human) + 0x390u + slot * 0x14u;
            if (Read<std::uint8_t>(command, 0) != 0 && Human_DoCheckUserCommand(human, command)) Write<std::uint8_t>(command, 0, 0);
        }
    }
    // Original function: FUN_0043bca9.
    // Complete Windows selector table 0043BD42..0043BD96.
    bool Human_DoCheckUserCommand(const void* human, const void* command)
    {
        switch (NC2Lifetime::Read<std::uint8_t>(command, 0))
        {
        case 6: case 13: case 14: return Human_IsJobSoldier(human) || Human_IsJobScout(human) || Human_IsJobTrader(human);
        case 28: case 29: return Human_IsJobBuilder(human);
        case 30: case 31: return !Human_IsJobBuilder(human);
        case 36: case 37: return !Human_IsJobTrader(human);
        case 19: case 20: return !Human_IsJobScout(human);
        default: { const auto opcode = NC2Lifetime::Read<std::uint8_t>(command, 0); return opcode < 4 || opcode > 87; }
        }
    }
    // Original function: FUN_0043e49d.
    void Human_ResetUserCommandArray(void* human)
    {
        for (std::uint32_t slot = 0; slot < 87; ++slot) NC2Lifetime::Write<std::uint8_t>(human, 0x390u + slot * 0x14u, 0);
    }

    // Original function: FUN_0042d348.
    bool Human_HasEquippedItemType(const void* human, std::uint32_t good, std::int32_t slot)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0xAEC) == good || Read<std::uint32_t>(human, 0xAF0) == good) return true;
        const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)]);
        const auto* armor = static_cast<const SStaticArmorTypeData*>(gStaticArmorTypeData);
        if (static_cast<std::uint32_t>(weapons[Read<std::uint32_t>(human, 0xAE4)].goodType) == good || static_cast<std::uint32_t>(armor[Read<std::uint32_t>(human, 0xAE8)].goodType) == good) return true;
        if (slot == -1) return Human_l_Item_IsGoodTypeEquipped(human, good);
        const auto offset = 0xAF4u + static_cast<std::uint32_t>(slot) * 8u;
        return Read<std::uint32_t>(human, offset) == good && Read<std::int32_t>(human, offset + 4) > 0;
    }

    // Translation boundary only; unsupported commands must not be consumed.
    static bool IsUserCommandReconstructed(const void* human, const void* command)
    {
        using namespace NC2Lifetime;
        if (Human_IsDefendingHouse(human, -1)) return false;
        const auto opcode = Read<std::uint8_t>(command, 0);
        if (Read<std::uint8_t>(human, 0xB51) != 0 && opcode != 17 && (opcode < 21 || opcode > 25) && opcode != 28 && opcode != 30 && opcode != 31 && opcode != 32 && opcode != 34 && opcode != 35 && opcode != 36 && opcode != 37 && opcode != 39 && (opcode < 49 || opcode > 51)) return false;
        if (Read<std::uint8_t>(command, 0) >= 52 && Read<std::uint8_t>(command, 0) <= 87) return true;
        switch (Read<std::uint8_t>(command, 0))
        {
        case 4: case 5: case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 16: case 17: case 18: case 19: case 20: case 21: case 22: case 23: case 24: case 25: case 26: case 27: case 28: case 29: case 30: case 31: case 32: case 33: case 34: case 35: case 36: case 37: case 39: case 41: case 49: case 50: case 51: return true;
        default: return false;
        }
    }

    // Original function: FUN_0045c293; the attachment-mask argument is unused in Windows.
    void Human_UserMessage_SendToUserMessage_CannotDetachHouse(void* human, std::uint32_t attachmentMask)
    {
        using namespace NC2Lifetime;
        (void)attachmentMask;
        std::byte message[0x304];
        ToUserMessage_Construct(message);
        Write<std::uint32_t>(message, 0, 42);
        Write(message, 0x10, Read<std::uint32_t>(human, 4));
        Write(message, 8, Read<std::uint32_t>(human, 0x10));
        ToUserMessage_FinishMessage(message);
        NC2LogicExternals::External_Misc_SendToUserMessage(message);
    }

    // Original function: FUN_0043cc88.
    std::uint32_t Human_DoExecuteUserCommand_DetachWorkHouse(void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint32_t>(human, 0x18) == 7) return 1;
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        if (!Human_IsAttachedToWorkHouse(human) || Human_IsAttachedToLearnHouse(human)) return 1;
        auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
        if (House_Inhabitants_DetachHuman(house, Read<std::uint32_t>(human, 4), 2)) return 2;
        Human_UserMessage_SendToUserMessage_CannotDetachHouse(human, 2);
        return 1;
    }

    // Original function: FUN_0043ce31.
    std::uint32_t Human_DoExecuteUserCommand_DetachLearnHouse(void* human)
    {
        using namespace NC2Lifetime;
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        if (!Human_IsAttachedToWorkHouse(human)) return 1;
        auto* house = CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize;
        if (House_Inhabitants_DetachHuman(house, Read<std::uint32_t>(human, 4), 8)) return 2;
        Human_UserMessage_SendToUserMessage_CannotDetachHouse(human, 8);
        return 1;
    }

    // Original function: FUN_0043c486; fallback walking retains the original work-center point.
    std::uint32_t Human_DoExecuteUserCommand_GotoAndWait(void* human, const void* point)
    {
        using namespace NC2Lifetime;
        Write<std::uint32_t>(human, 0xB64, 0);
        Write<std::uint32_t>(human, 0xB70, 0);
        Human_Target_Reset(human);
        Human_Target_ResetSecondaryTarget(human);
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        const bool direct = Human_l_StartAtomicWalkAndWait(human, point);
        if (!direct)
        {
            auto fallback = Read<std::uint32_t>(point, 0);
            if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6) != Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human)), 6)) return 1;
            if (!Tool_Map_GetNotBlockedPosition(&fallback, false) || !Human_l_StartAtomicWalkAndWait(human, &fallback)) return 1;
        }
        if (Human_IsAttachedToLearnHouse(human)) Human_DoExecuteUserCommand_DetachLearnHouse(human);
        const auto job = Read<std::uint32_t>(human, 0x18);
        bool recenter = Human_IsJobSoldier(human) || Human_IsJobHero(human) || Human_IsJobScout(human);
        if (direct)
        {
            recenter = recenter || job == 6 || (job == 5 && !Human_IsAttachedToHomeHouse(human)) || (!Human_IsAgeAdult(human) && !Human_IsAttachedToHomeHouse(human));
            if (Human_IsJobCarrier(human) && !Human_IsAttachedToWorkHouse(human) && !Human_IsAttachedToVehicle(human)) recenter = true;
            if (Read<std::uint8_t>(gStaticJobTypeData, job * 0x88u + 0x83u) != 0 && !Human_IsAttachedToWorkHouse(human)) recenter = true;
        }
        if (recenter)
        {
            Write(human, 0xB30, Read<std::uint32_t>(point, 0));
            Write<std::uint32_t>(human, 0x38C, 0);
            Human_WorkCenterMarker_Update(human);
            if (Human_IsAttachedToWorkHouse(human)) Human_DoExecuteUserCommand_DetachWorkHouse(human);
        }
        return 2;
    }

    // Original function: FUN_0043c7cf; speaker identity comes from command id/revision.
    std::uint32_t Human_DoExecuteUserCommand_Cheer(void* human, std::uint32_t speakerId, std::uint32_t revision)
    {
        using namespace NC2Lifetime;
        if (!Human_IsAbleToListen(human)) return 1;
        if (!Human_l_IsAtomicInterruptable(human)) return 0;
        const auto* speaker = CHumanArray::mStaticVars + speakerId * CHumanArray::ElementSize;
        if (Read<std::uint8_t>(speaker, 0) == 0 || Read<std::uint32_t>(speaker, 8) != revision) return 1;
        Human_Target_SetTargetPositions(human, Human_GetMapPositionRef(speaker), Human_GetMapPositionRef(human));
        Human_l_StartTask_Listen(human, Human_Target_GetDirectionTo(human));
        return 2;
    }

    bool Human_CanExecuteQueuedCommands(const void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(human, 0xA60) != 0 || (Read<std::uint32_t>(human, 0xACC) != 0 && Read<std::uint32_t>(human, 0xACC) != 6) || Read<std::uint32_t>(human, 0x378) != 0) return true;
        for (std::uint32_t slot = 0; slot < 87; ++slot)
        {
            const void* command = static_cast<const std::byte*>(human) + 0x390u + slot * 0x14u;
            if (Read<std::uint8_t>(command, 0) != 0 && !IsUserCommandReconstructed(human, command)) return false;
        }
        return true;
    }

    // Partial original function: FUN_0043bed3; needs, goodies, house assignments and military-mode commands.
    std::uint32_t Human_DoExecuteUserCommand(void* human, const void* command)
    {
        using namespace NC2Lifetime;
        const auto opcode = Read<std::uint8_t>(command, 0);
        // Windows 0043BE95/0043BF0B: these assignment commands request vehicle
        // detachment before their own handler, regardless of the detach result.
        if (Human_IsAttachedToVehicle(human) && (opcode == 28 || opcode == 30 || opcode == 32)) Human_DoExecuteUserCommand_DetachVehicle(human);
        if ((opcode >= 52 && opcode <= 60) || (opcode >= 70 && opcode <= 78)) return Human_DoExecuteUserCommand_Equipment_Drop(human, Read<std::uint32_t>(command, 12), Read<std::int32_t>(command, 16));
        if ((opcode >= 61 && opcode <= 69) || (opcode >= 79 && opcode <= 87)) return Human_DoExecuteUserCommand_Equipment_Change(human, Read<std::uint32_t>(command, 12), Read<std::int32_t>(command, 16));
        switch (opcode)
        {
        case 4:
        {
            Write<std::uint8_t>(human, 0x728, 0);
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 0x0C));
            return Human_DoExecuteUserCommand_GotoAndWait(human, &point);
        }
        case 5:
        {
            if (!Human_l_IsAtomicInterruptable(human)) return 0;
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 0x0C));
            Human_l_StartTask_GoAway(human, point, Read<std::uint32_t>(command, 0x10));
            return 2;
        }
        case 6: // DoExecuteUserCommand_GoHome, inlined at 0043BF80.
            if (!Human_l_IsAtomicInterruptable(human)) return 0;
            Write<std::uint32_t>(human, 0x368, 3);
            Write<std::uint32_t>(human, 0x36C, 0);
            return 2;
        case 7: // DoExecuteUserCommand_Eat, 0043C674.
        case 8: // DoExecuteUserCommand_Sleep, 0043C6BE.
            if (!Human_l_IsAtomicInterruptable(human)) return 0;
            if (Human_HasEquippedItemType(human, opcode == 7 ? 50u : 51u, -1)) return 1;
            if (opcode == 7) Human_CurrentTask_DoInterrupt(human);
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 7);
            Write<std::uint32_t>(human, 0x370, opcode == 7 ? 1 : 2);
            return 2;
        case 9: // DoExecuteUserCommand_Talk, 0043C6FE.
        case 10: // DoExecuteUserCommand_Pray, 0043C73D.
            if (opcode == 9 ? !Human_IsAbleToTalk(human) : !Human_IsAbleToPray(human)) return 1;
            if (!Human_l_IsAtomicInterruptable(human)) return 0;
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0x368, 7);
            Write<std::uint32_t>(human, 0x370, opcode == 9 ? 3 : 4);
            return 2;
        case 11: // DoExecuteUserCommand_Goodie, 0043C77C.
        {
            auto task = Read<std::int32_t>(command, 0x0C);
            if (task < 24 || task > 29) task = 24;
            const auto job = Read<std::uint32_t>(human, 0x18);
            if ((job >= 31 && job <= 41) || (job >= 42 && job <= 47)) return 1;
            if (!Human_IsAbleToDoGoodieNow(human) || !Human_l_IsAtomicInterruptable(human)) return 0;
            Write<std::uint32_t>(human, 0x36C, 0);
            Write(human, 0x368, task);
            return 2;
        }
        case 12: return Human_DoExecuteUserCommand_Cheer(human, Read<std::uint32_t>(command, 4), Read<std::uint32_t>(command, 8));
        case 13: case 14: return Human_DoExecuteUserCommand_Marry(human);
        case 15: return Human_DoExecuteUserCommand_GetBoy(human);
        case 16: return Human_DoExecuteUserCommand_GetGirl(human);
        case 17:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 12));
            return Human_DoExecuteUserCommand_SetWorkCenter(human, &point);
        }
        case 18:
        {
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 12));
            return Human_DoExecuteUserCommand_OpenChest(human, &point);
        }
        case 19: case 20:
        {
            if (opcode == 20 && Read<std::uint32_t>(command, 16) != 0)
            {
                Write(human, 0xB30, Read<std::uint32_t>(Human_GetMapPositionRef(human), 0));
                Write<std::uint32_t>(human, 0x38C, 0);
                Human_WorkCenterMarker_Update(human);
            }
            std::uint32_t point;
            MapMigPoint_AssignLong(&point, Read<std::uint32_t>(command, 12));
            return opcode == 19 ? Human_DoExecuteUserCommand_BuildGuide(human, &point) : Human_DoExecuteUserCommand_ExploreArea(human, &point);
        }
        case 26: return Human_DoExecuteUserCommand_AttachHomeHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 21: return Human_DoExecuteUserCommand_ChangeJob(human, Read<std::int32_t>(command, 12));
        case 22: return Human_DoExecuteUserCommand_ChangeProducedGood(human, Read<std::uint32_t>(command, 12));
        case 23: return Human_DoExecuteUserCommand_ChangeProduction(human, Read<std::uint32_t>(command, 12), Read<std::int32_t>(command, 16));
        case 24: return Human_DoExecuteUserCommand_SetJob(human, Read<std::uint32_t>(command, 12));
        case 25: return Human_DoExecuteUserCommand_RestartJobExecution(human);
        case 27: return Human_DoExecuteUserCommand_DetachHomeHouse(human);
        case 28: return Human_DoExecuteUserCommand_AttachWorkHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 29: return Human_DoExecuteUserCommand_DetachWorkHouse(human);
        case 30: return Human_DoExecuteUserCommand_AttachBuildHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 31: return Human_DoExecuteUserCommand_DetachBuildHouse(human);
        case 32: return Human_DoExecuteUserCommand_AttachLearnHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 33: return Human_DoExecuteUserCommand_DetachLearnHouse(human);
        case 34: return Human_DoExecuteUserCommand_LearnJob(human, Read<std::uint32_t>(command, 12));
        case 35: return Human_DoExecuteUserCommand_LearnGood(human, Read<std::uint32_t>(command, 12));
        case 36: return Human_DoExecuteUserCommand_AttachTradeHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 37: return Human_DoExecuteUserCommand_DetachTradeHouse(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 38: return Human_DoExecuteUserCommand_AttachVehicle(human, Read<std::int32_t>(command, 12), Read<std::uint32_t>(command, 16));
        case 39: return Human_DoExecuteUserCommand_DetachVehicle(human);
        case 41:
            if (!Human_l_IsAtomicInterruptable(human)) return 0;
            Write(human, 0xB5C, Read<std::uint32_t>(command, 0x0C));
            if (Read<std::uint32_t>(command, 0x0C) == 3) Write<std::uint32_t>(human, 0xB70, 0);
            return 2;
        case 49: // Inlined at 0043C2B2: enqueue restart before resetting flags.
            Human_DoAddNewUserCommand(human, 25, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            Human_l_Trader_ResetImportFlags(human);
            return 1;
        case 50: // Inlined at 0043C2D9: house id, good and flag share parameter 1.
        case 51: // Inlined at 0043C340: select a trade agreement without validation.
        {
            if (!Human_IsJobTrader(human)) return 1;
            void* merchant = HumanMerchantArray_GetMerchantDataPtr(gHumanMerchantArray, Read<std::int32_t>(human, 0xB44));
            const auto packed = Read<std::uint32_t>(command, 12);
            void* data = nullptr;
            if (opcode == 50)
            {
                data = MerchantData_GetHouseDataPtr(merchant, static_cast<std::int32_t>(packed & 0xFFFFu), Read<std::uint32_t>(command, 16));
                if (data == nullptr) return 1;
            }
            Human_DoAddNewUserCommand(human, 25, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), 0, 0);
            if (opcode == 50) MerchantHouseData_SetGoodTypeImportFlag(data, (packed >> 16u) & 0xFFu, (packed & 0xFF000000u) != 0);
            else Write(merchant, 4, packed);
            return 2;
        }
        default: return 0;
        }
    }

    // Original function: FUN_0043bd97. Call only after the reconstruction-boundary check.
    bool Human_DoExecuteNextUserCommand(void* human)
    {
        using namespace NC2Lifetime;
        if (Read<std::uint8_t>(human, 0xA60) != 0 || (Read<std::uint32_t>(human, 0xACC) != 0 && Read<std::uint32_t>(human, 0xACC) != 6) || Read<std::uint32_t>(human, 0x378) != 0) return false;
        const auto priority = Human_UserControl_GetPriorityCommandIndexInQueue(human);
        if (priority != -1)
        {
            void* command = static_cast<std::byte*>(human) + 0x390u + static_cast<std::uint32_t>(priority) * 0x14u;
            const auto result = Human_DoExecuteUserCommand(human, command);
            if (result == 1) Write<std::uint8_t>(command, 0, 0);
            else if (result != 0)
            {
                Write<std::uint8_t>(command, 0, 0);
                if (result == 2) Write(human, 0xA5C, priority);
                Write<std::uint8_t>(human, 0xA60, 1);
                return true;
            }
        }
        for (std::uint32_t slot = 0; slot < 87; ++slot)
        {
            void* command = static_cast<std::byte*>(human) + 0x390u + slot * 0x14u;
            if (Read<std::uint8_t>(command, 0) == 0) continue;
            const auto result = Human_DoExecuteUserCommand(human, command);
            if (result == 1) Write<std::uint8_t>(command, 0, 0);
            else if (result == 2)
            {
                Write<std::uint8_t>(command, 0, 0);
                Write(human, 0xA5C, slot);
                Write<std::uint8_t>(human, 0xA60, 1);
                return true;
            }
        }
        return false;
    }

    // Partial original function: FUN_0043be40; unsupported command trees retain their queue slots.
    void Human_DoExecuteAllPriorityUserCommandsWhichDoNotInterrupt(void* human)
    {
        using namespace NC2Lifetime;
        for (;;)
        {
            const auto index = Human_UserControl_GetPriorityCommandIndexInQueue(human);
            if (index == -1) return;
            void* command = static_cast<std::byte*>(human) + 0x390u + static_cast<std::uint32_t>(index) * 0x14u;
            if (Human_UserControl_IsAInterruptingCommand(Read<std::uint8_t>(command, 0)) || !IsUserCommandReconstructed(human, command)) return;
            const auto result = Human_DoExecuteUserCommand(human, command);
            if (result == 0) return; // Pending command startup must yield without consuming the slot.
            if (result == 1 || result == 2) Write<std::uint8_t>(command, 0, 0);
            if (result == 2)
            {
                Write(human, 0xA5C, index);
                Write<std::uint8_t>(human, 0xA60, 1);
            }
        }
    }

    // Original function: FUN_00445300; cargo mode selects pickup or equipment-drop startup.
    void Human_l_FinishTask_Goto(void* human)
    {
        using namespace NC2Lifetime;
        const void* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
        if (Read<std::uint8_t>(micro, 0) == 2 && (Read<std::uint8_t>(micro, 2) & 2u) != 0 && Read<std::uint32_t>(CHouseArray::mStaticVars + Read<std::uint16_t>(micro, 4) * CHouseArray::ElementSize, 0x2C) == 3) Human_HouseMisc_Enter(human);
        if (Read<std::uint32_t>(human, 0x370) != 0)
        {
            Write<std::uint32_t>(human, 0x368, 7);
            Write<std::uint32_t>(human, 0x36C, 0);
            return;
        }
        if ((!Human_IsAttachedToVehicle(human) && Read<std::uint32_t>(human, 0xA68) == 6) || !Human_IsAtTargetPoint(human))
        {
            if (!Human_IsAttachedToVehicle(human) && Read<std::uint32_t>(human, 0xA68) == 6) Human_Target_Reset(human);
            Write<std::uint32_t>(human, 0x368, 8);
            Write<std::uint32_t>(human, 0x36C, 0);
            return;
        }
        const auto target = Read<std::uint32_t>(human, 0xA68);
        // Native task 0x15 enters a vehicle; task 0x12 opens a chest.
        if (target == 6) Write<std::uint32_t>(human, 0x368, 21);
        else if (target == 2 && Read<std::uint32_t>(human, 0xAD0) == 64) Write<std::uint32_t>(human, 0x368, 18);
        else if (Read<std::uint32_t>(human, 0xACC) == 1)
        {
            Write<std::uint32_t>(human, 0x368, 16);
            if (!Human_l_StartTask_Tool_PickupWantedGood_FromTargetPosition(human)) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 3); }
            return;
        }
        else if (Read<std::uint32_t>(human, 0xACC) == 6) { Write<std::uint32_t>(human, 0x368, 17); Human_l_StartTask_Equip_DropItem(human, Read<std::uint32_t>(human, 0xAD0)); return; }
        else Write<std::uint32_t>(human, 0x368, 8);
        Write<std::uint32_t>(human, 0x36C, 0);
    }
}
