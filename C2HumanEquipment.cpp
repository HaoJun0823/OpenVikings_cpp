#include "C2HumanEquipment.h"
#include "C2CallbackManager.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2HumanTaskAccess.h"
#include "C2HumanWalking.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2StaticGoodTypeData.h"
#include "C2StringAccess.h"
#include "C2UserMessageAccess.h"
#include "C2VehicleAccess.h"
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_00438C96; Windows does not validate the slot.
    std::uint32_t Human_Inventory_GetGoodInSlot(const void* human, std::uint32_t slot) { return NC2Lifetime::Read<std::uint32_t>(human, 0xAF4 + slot * 8u); }
    // Original function: FUN_00438CA4.
    std::uint32_t Human_GetCarriedGood(const void* human) { return NC2Lifetime::Read<std::uint32_t>(human, 0xAD4); }
    // Original function: FUN_00438CAB; both native tool divisors are 10000.
    std::int32_t Human_GetPercentageConditionOfTool(const void* human)
    {
        if (Human_GetEquipmentType_Tool(human) == 31) return NXBasics::Runtime_TimeSigned32(NC2Lifetime::Read<std::uint32_t>(human, 0xAE0) * 100u) / 10000;
        if (Human_GetEquipmentType_Tool(human) == 32) return NXBasics::Runtime_TimeSigned32(NC2Lifetime::Read<std::uint32_t>(human, 0xAE0) * 100u) / 10000;
        return 0;
    }

    using namespace NC2Lifetime;
    // Original function: FUN_00438cec; shared carried-good callback leaf.
    void Human_ClearCarriedGood(void* human) { Write<std::uint32_t>(human, 0xAD4, 0); CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, 0); }
    namespace
    {
        void Fail(void* human, std::uint32_t code) { Write<std::uint32_t>(human, 0x36C, 3); Write(human, 0x374, code); }
        void Done(void* human) { Write<std::uint32_t>(human, 0x36C, 2); }
        void Queue(void* human, std::uint8_t opcode, std::uint32_t good, std::int32_t slot) { Human_DoAddNewUserCommand(human, opcode, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), good, static_cast<std::uint32_t>(slot)); }
        void* LastCommand(void* human) { return static_cast<std::byte*>(human) + 0x390u + Read<std::uint32_t>(human, 0xA5C) * 20u; }
        // Original function: FUN_0042db6c.
        void Human_SetCarriedGood(void* human, std::uint32_t good) { Write(human, 0xAD4, good); CCallbackManager::sTheObjectPtr->ExecuteCallback(0x21, true, Read<std::uint32_t>(human, 4), 1, good); }
        // Original function: FUN_0045c075.
        void Human_UserMessage_SendToUserMessage_CannotFindEquipment(void* human)
        {
            if (GameAssistantSingleHumanData_CommandHistory_IsEquipCommandInArray(static_cast<std::byte*>(human) + 0xB88)) return;
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 44);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            Write(message, 0x30, Read<std::uint32_t>(human, 0xAD0));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
        // Original function: FUN_0045c0de.
        void Human_UserMessage_SendToUserMessage_BackpackIsFull(void* human)
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write<std::uint32_t>(message, 0, 45);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            Write(message, 0x30, Read<std::uint32_t>(human, 0xAD0));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
        }
    }

    // Original function: FUN_0042db17; empty good succeeds before the enable/search checks.
    bool Human_IsAbleToEquipGoodNow(const void* human, std::uint32_t good, bool anyPlayer) { return good == 0 || (Read<std::uint8_t>(human, 0x1E0u + good) != 0 && Human_FindEquipment_Complex_Nearby(human, good, anyPlayer)); }

    // Original function: FUN_0042d3c2.
    std::uint32_t Human_GetEquipmentType_Shoes(const void* human) { return Read<std::uint32_t>(human, 0xAEC); }
    // Original function: FUN_0042d3c9.
    std::uint32_t Human_GetEquipmentType_Tool(const void* human) { return Read<std::uint32_t>(human, 0xAF0); }
    // Original function: FUN_0042d3d0.
    std::uint32_t Human_GetEquipmentType_Weapon(const void* human)
    {
        const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)]);
        return static_cast<std::uint32_t>(weapons[Read<std::uint32_t>(human, 0xAE4)].goodType);
    }
    // Original function: FUN_0042d3eb.
    std::uint32_t Human_GetEquipmentType_Armor(const void* human) { return static_cast<std::uint32_t>(static_cast<const SStaticArmorTypeData*>(gStaticArmorTypeData)[Read<std::uint32_t>(human, 0xAE8)].goodType); }
    // Original function: FUN_0042d40d; an exhausted pair uses the empty-good name.
    const char* Human_GetEquipmentType_Shoes_GetGuiNamePtr(const void* human) { const auto good = Human_GetEquipmentType_Shoes(human); return StringAccess_GetGoodNamePtr(good != 0 && Read<std::uint32_t>(human, 0xADC) != 0 ? good : 0, good != 0 && Read<std::uint32_t>(human, 0xADC) != 0 ? 1u : 0u); }
    // Original function: FUN_0042d430.
    const char* Human_GetEquipmentType_Tool_GetGuiNamePtr(const void* human) { const auto good = Human_GetEquipmentType_Tool(human); return StringAccess_GetGoodNamePtr(good != 0 && Read<std::uint32_t>(human, 0xAE0) != 0 ? good : 0, good != 0 && Read<std::uint32_t>(human, 0xAE0) != 0 ? 1u : 0u); }
    // Original function: FUN_0045d0d3; values are stored as percentages, without clipping.
    std::uint32_t Human_Inventory_GetPercentageConditionOfItemInSlot(const void* human, std::int32_t slot) { return slot == -1 || Read<std::uint32_t>(human, 0xAF4u + static_cast<std::uint32_t>(slot) * 8u) == 0 ? 0 : Read<std::uint32_t>(human, 0xAF8u + static_cast<std::uint32_t>(slot) * 8u); }
    // Original function: FUN_0042d47e.
    void Human_Equipment_SetToolType(void* human, std::uint32_t good) { Write(human, 0xAF0, good); }
    // Original function: FUN_0042d4a8.
    void Human_Equipment_SetArmorType(void* human, std::uint32_t armor) { Write(human, 0xAE8, armor); CCallbackManager::sTheObjectPtr->ExecuteCallback(0x20, true, Read<std::uint32_t>(human, 4), armor, 0); }
    // Original function: FUN_0042d4ca.
    void Human_Equipment_SetWeaponType(void* human, std::uint32_t weapon)
    {
        Write(human, 0xAE4, weapon);
        if (Human_IsJobSoldier(human) || Human_IsJobHero(human))
        {
            const auto* weapons = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)]);
            Human_SetJobType(human, Read<std::uint32_t>(weapons + weapon, 0x34));
        }
    }

    // Original function: FUN_0045ce94; native replacement favors the lowest condition of the same good.
    bool Human_l_Item_Set(void* human, std::uint32_t good)
    {
        std::int32_t selected = -1, otherMinimum = 100, sameMinimum = 100;
        for (std::uint32_t slot = 0; slot < 4; ++slot)
        {
            const auto offset = 0xAF4u + slot * 8u;
            const auto item = Read<std::uint32_t>(human, offset);
            if (item == 0) { Write(human, offset, good); Write<std::uint32_t>(human, offset + 4, 100); return true; }
            const auto condition = Read<std::int32_t>(human, offset + 4);
            if (item == good && condition < sameMinimum) { otherMinimum = -1; sameMinimum = condition; selected = static_cast<std::int32_t>(slot); }
            else if (condition < otherMinimum) { otherMinimum = condition; selected = static_cast<std::int32_t>(slot); }
        }
        if (selected == -1) return false;
        Write(human, 0xAF4u + static_cast<std::uint32_t>(selected) * 8u, good);
        Write<std::uint32_t>(human, 0xAF8u + static_cast<std::uint32_t>(selected) * 8u, 100);
        return true;
    }
    // Original function: FUN_0045cf25.
    void Human_l_Item_Clear(void* human, std::int32_t slot)
    {
        if (slot == -1) return;
        Write<std::uint32_t>(human, 0xAF4u + static_cast<std::uint32_t>(slot) * 8u, 0);
        Write<std::uint32_t>(human, 0xAF8u + static_cast<std::uint32_t>(slot) * 8u, 0);
    }
    // Original function: FUN_0045cf41.
    std::int32_t Human_l_Item_GetItemSlotIndex(const void* human, std::uint32_t good)
    {
        for (std::uint32_t slot = 0; slot < 4; ++slot) if (Read<std::uint32_t>(human, 0xAF4u + slot * 8u) == good && Read<std::int32_t>(human, 0xAF8u + slot * 8u) > 0) return static_cast<std::int32_t>(slot);
        return -1;
    }
    // Original function: FUN_0045d156; negative conditions still occupy a slot.
    std::int32_t Human_l_Item_GetFreeSlotIndex(const void* human)
    {
        for (std::uint32_t slot = 0; slot < 4; ++slot) if (Read<std::uint32_t>(human, 0xAF4u + slot * 8u) == 0 || Read<std::int32_t>(human, 0xAF8u + slot * 8u) == 0) return static_cast<std::int32_t>(slot);
        return -1;
    }
    // Original function: FUN_0045d1c2; the final native OR admits every remaining value.
    std::uint32_t Tool_GoodEquipmentType_Convert(std::uint32_t good)
    {
        const auto value = static_cast<std::int32_t>(good);
        if (value >= 37 && value <= 42) return 1;
        if (value >= 33 && value <= 36) return 2;
        if (value == 31 || value == 32) return 3;
        if (value == 30) return 4;
        return 5;
    }

    // Original function: FUN_0042d507; partially used shoes/tools are discarded, not recovered as cargo.
    void Human_Equipment_UnEquipItem(void* human, std::uint32_t good)
    {
        if (good == 30 || good == 31 || good == 32)
        {
            const auto condition = good == 30 ? 0xADC : 0xAE0;
            if (Read<std::int32_t>(human, condition) < 10000) Done(human);
            else Human_SetCarriedGood(human, good);
            if (good == 30) Human_Equipment_ResetShoeType(human); else Human_Equipment_ResetToolType(human);
            Write<std::uint32_t>(human, condition, 0);
            return;
        }
        switch (Tool_GoodEquipmentType_Convert(Read<std::uint32_t>(human, 0xAD0)))
        {
        case 1: Human_Equipment_SetWeaponType(human, 1); Human_SetCarriedGood(human, good); return;
        case 2: Human_Equipment_ResetArmorType(human); Human_SetCarriedGood(human, good); return;
        case 5:
        {
            const auto last = Human_UserControl_GetLastCommandType(human);
            auto slot = last >= 74 && last <= 78 ? Read<std::int32_t>(LastCommand(human), 0x10) : -1;
            if (slot == -1) slot = Human_l_Item_GetItemSlotIndex(human, Read<std::uint32_t>(human, 0xAD0));
            if (slot == -1) return;
            if (Human_l_Item_IsItemUnused(human, slot)) Human_SetCarriedGood(human, Read<std::uint32_t>(human, 0xAF4u + static_cast<std::uint32_t>(slot) * 8u));
            Human_l_Item_Clear(human, slot);
            return;
        }
        default: return;
        }
    }

    // Original function: FUN_0043df4a; the second parameter is unused.
    std::uint32_t Human_DoExecuteUserCommand_Equipment_Change(void* human, std::uint32_t good, std::int32_t slot)
    {
        (void)slot;
        if (Read<std::uint32_t>(human, 0xAD0) == good)
        {
            if (Read<std::uint32_t>(human, 0x368) == 16) return 1;
            if (Read<std::uint32_t>(human, 0x368) == 17) return 0;
        }
        if (Read<std::uint8_t>(human, 0xB51) != 0 || !Human_l_IsAtomicInterruptable(human)) return 0;
        Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write(human, 0xAD0, good);
        Write<std::uint32_t>(human, 0x368, 16);
        return 2;
    }
    // Original function: FUN_0043dfac.
    std::uint32_t Human_DoExecuteUserCommand_Equipment_Drop(void* human, std::uint32_t good, std::int32_t slot)
    {
        const auto cargo = Read<std::uint32_t>(human, 0xAD4);
        if ((cargo != 0 && cargo != good) || Read<std::uint32_t>(human, 0x368) == 16 || !Human_l_IsAtomicInterruptable(human)) return 0;
        Human_Target_Reset(human);
        if (Read<std::uint32_t>(human, 0xAD4) != good && !Human_HasEquippedItemType(human, good, slot)) return 1;
        Human_CurrentTask_DoInterrupt(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write(human, 0xAD0, good);
        Write<std::uint32_t>(human, 0x368, 17);
        return 2;
    }

    // Original function: FUN_0046ba57.
    void Human_l_StartTask_Equip(void* human)
    {
        const auto cargo = Read<std::uint32_t>(human, 0xAD4);
        if (cargo != 0 && cargo != Read<std::uint32_t>(human, 0xAD0))
        {
            Human_l_StartTask_Tool_DropCarriedGoodNearby_Remember(human);
            if (Read<std::uint32_t>(human, 0x36C) != 3) Write<std::uint8_t>(LastCommand(human), 0, static_cast<std::uint8_t>(Human_UserControl_GetLastCommandType(human)));
            return;
        }
        const auto good = Read<std::uint32_t>(human, 0xAD0);
        if (good == 30) Human_l_StartTask_Equip_Shoes(human);
        else if (good == 31 || good == 32) Human_l_StartTask_Equip_Tool(human);
        else if (static_cast<std::int32_t>(good) >= 43 && static_cast<std::int32_t>(good) <= 55) Human_l_StartTask_Equip_Item(human);
        else if (gStaticGoodTypeStorage.records[good].armorType != 0) Human_l_StartTask_Equip_Armor(human, static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[good].armorType));
        else if (gStaticGoodTypeStorage.records[good].weaponType != 0) Human_l_StartTask_Equip_Weapon(human, static_cast<std::uint32_t>(gStaticGoodTypeStorage.records[good].weaponType));
        else { Write<std::uint32_t>(human, 0xAD0, 0); Done(human); }
    }
    // Original function: FUN_0046bb11; maximum shoe condition is PE 004F3B78 = 10000.
    void Human_l_StartTask_Equip_Shoes(void* human)
    {
        if (Read<std::uint32_t>(human, 0xAEC) != 0 && Read<std::uint32_t>(human, 0xADC) != 0) { Done(human); return; }
        if (Read<std::uint32_t>(human, 0xAD4) != 30) { Human_l_StartTask_Equip_FetchWantedItem(human); return; }
        Human_ClearCarriedGood(human);
        Human_Equipment_SetShoeType(human, 30);
        Write<std::uint32_t>(human, 0xADC, 10000);
        Done(human);
    }
    // Original function: FUN_0046bb54; both Windows tool maxima are 10000.
    void Human_l_StartTask_Equip_Tool(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0xAD0);
        if (Read<std::uint32_t>(human, 0xAF0) != 0 && Read<std::uint32_t>(human, 0xAE0) != 0)
        {
            if (Read<std::uint32_t>(human, 0xAF0) != good) { Queue(human, 83, good, 0); Queue(human, 53, Read<std::uint32_t>(human, 0xAF0), 0); }
            Done(human);
            return;
        }
        if (Read<std::uint32_t>(human, 0xAD4) != good) { Human_l_StartTask_Equip_FetchWantedItem(human); return; }
        Human_ClearCarriedGood(human);
        if (good == 31 || good == 32) { Human_Equipment_SetToolType(human, good); Write<std::uint32_t>(human, 0xAE0, 10000); }
        Done(human);
    }
    // Original function: FUN_0046bc12; a specified occupied slot schedules its own drop first.
    void Human_l_StartTask_Equip_Item(void* human)
    {
        const auto last = Human_UserControl_GetLastCommandType(human);
        const auto slot = ((last >= 83 && last <= 87) || (last >= 65 && last <= 69)) ? Read<std::int32_t>(LastCommand(human), 0x10) : -1;
        if (slot != -1)
        {
            const auto offset = 0xAF4u + static_cast<std::uint32_t>(slot) * 8u;
            if (Read<std::uint32_t>(human, offset) != 0 && Read<std::int32_t>(human, offset + 4) > 0)
            {
                Queue(human, static_cast<std::uint8_t>(slot + 75), Read<std::uint32_t>(human, offset), slot);
                Fail(human, 10);
                return;
            }
        }
        else if (Human_l_Item_GetFreeSlotIndex(human) == -1) { Fail(human, 12); return; }
        const auto good = Read<std::uint32_t>(human, 0xAD0);
        if (Read<std::uint32_t>(human, 0xAD4) != good) { Human_l_StartTask_Equip_FetchWantedItem(human); return; }
        const auto added = Human_l_Item_Set(human, good);
        Human_ClearCarriedGood(human);
        if (added) Done(human); else Fail(human, 11);
    }
    // Original function: FUN_0046bcf9.
    void Human_l_StartTask_Equip_Weapon(void* human, std::uint32_t weapon)
    {
        if (Human_GetEquipmentType_Weapon(human) != 0)
        {
            if (Read<std::uint32_t>(human, 0xAE4) != weapon) { Queue(human, 81, Read<std::uint32_t>(human, 0xAD0), 0); Queue(human, 72, Human_GetEquipmentType_Weapon(human), 0); }
            Done(human);
            return;
        }
        if (Read<std::uint32_t>(human, 0xAD4) != Read<std::uint32_t>(human, 0xAD0)) { Human_l_StartTask_Equip_FetchWantedItem(human); return; }
        Human_ClearCarriedGood(human);
        Human_Equipment_SetWeaponType(human, weapon);
        Done(human);
    }
    // Original function: FUN_0046bd8d; the equipped predicate checks armor type, not its good id.
    void Human_l_StartTask_Equip_Armor(void* human, std::uint32_t armor)
    {
        if (Human_Equipment_GetArmorType(human) != 0)
        {
            if (Human_Equipment_GetArmorType(human) != armor) { Queue(human, 82, Read<std::uint32_t>(human, 0xAD0), 0); Queue(human, 73, Human_GetEquipmentType_Armor(human), 0); }
            Done(human);
            return;
        }
        if (Read<std::uint32_t>(human, 0xAD4) != Read<std::uint32_t>(human, 0xAD0)) { Human_l_StartTask_Equip_FetchWantedItem(human); return; }
        Human_ClearCarriedGood(human);
        Human_Equipment_SetArmorType(human, armor);
        Done(human);
    }
    // Original function: FUN_0046be21; restoring a command retains its arguments and original queue slot.
    void Human_l_StartTask_Equip_FetchWantedItem(void* human)
    {
        std::uint32_t real = 0, work = 0;
        if (!Human_FindGood_Complex(human, Read<std::uint32_t>(human, 0xAD0), &real, &work, Human_GetMapPositionRef(human))) { Fail(human, 3); return; }
        if (!Human_l_StartAtomicWalkAndFetch(human, &real, &work, true)) { Human_Target_Reset(human); Fail(human, 7); return; }
        const auto last = Human_UserControl_GetLastCommandType(human);
        if (last >= 61 && last <= 87) { Write<std::uint8_t>(LastCommand(human), 0, static_cast<std::uint8_t>(last)); Write<std::uint8_t>(human, 0xA60, 0); }
        else Human_StopCurrentGoodCirculationMode(human);
    }

    // Original function: FUN_0046bed3; validate the drop target before removing equipment.
    void Human_l_StartTask_Equip_DropItem(void* human, std::uint32_t good)
    {
        if (Read<std::uint32_t>(human, 0xA68) != 0)
        {
            const auto target = Read<std::uint32_t>(human, 0xA68);
            bool usable = true;
            if (target == 5) usable = House_Stock_HasDemandForGood(Human_GetTargetHousePtr(human), good);
            else if (target == 1) usable = Map_GoodTool_CanBePutOnMap(good, static_cast<std::byte*>(human) + 0xA6C);
            if (!usable) { Human_StopCurrentGoodCirculationMode(human); Human_Target_Reset(human); }
        }
        if (Human_IsAtTargetPoint(human))
        {
            if (Read<std::uint32_t>(human, 0xAD4) == 0) Human_Equipment_UnEquipItem(human, good);
            if (Read<std::uint32_t>(human, 0xAD4) == 0) { Done(human); return; }
            Human_l_StartAtomicAnimation(human, 23, -1);
            Write<std::uint32_t>(human, 0x36C, 1);
            return;
        }
        const auto kind = Tool_GoodEquipmentType_Convert(Read<std::uint32_t>(human, 0xAD0));
        if (kind == 5)
        {
            const auto last = Human_UserControl_GetLastCommandType(human);
            auto slot = ((last >= 74 && last <= 78) || (last >= 56 && last <= 60)) ? Read<std::int32_t>(LastCommand(human), 0x10) : -1;
            if (slot == -1) slot = Human_l_Item_GetItemSlotIndex(human, Read<std::uint32_t>(human, 0xAD0));
            if (slot == -1) { Done(human); return; }
            if (!Human_l_Item_IsItemUnused(human, slot)) { Human_l_Item_Clear(human, slot); Done(human); return; }
        }
        else if (kind == 4 && Read<std::int32_t>(human, 0xADC) < 10000) { Human_Equipment_ResetShoeType(human); Write<std::uint32_t>(human, 0xADC, 0); Done(human); return; }
        else if (kind == 3 && (Read<std::uint32_t>(human, 0xAD0) == 31 || Read<std::uint32_t>(human, 0xAD0) == 32) && Read<std::int32_t>(human, 0xAE0) < 10000) { Human_Equipment_ResetToolType(human); Write<std::uint32_t>(human, 0xAE0, 0); Done(human); return; }
        std::uint32_t real = 0, work = 0;
        if (!Human_FindConsumerNearWorkCenter(human, good, &real, &work) && !Human_FindPilePointNearby(human, good, &real, &work)) { Human_Target_Reset(human); Fail(human, 7); return; }
        Human_Target_SetTargetPositions(human, &real, &work);
        Write<std::uint32_t>(human, 0xACC, 6);
        if (!Human_l_StartAtomicWalkAndDrop(human, &real, &work, good)) { Human_Target_Reset(human); Fail(human, 7); return; }
        const auto last = Human_UserControl_GetLastCommandType(human);
        if ((last >= 70 && last <= 78) || (last >= 52 && last <= 60)) { Write<std::uint8_t>(LastCommand(human), 0, static_cast<std::uint8_t>(last)); Write<std::uint8_t>(human, 0xA60, 0); }
    }

    // Original function: FUN_0046c130; a matching carried item restarts equipment startup.
    void Human_l_FinishTask_Equip(void* human)
    {
        if (Read<std::uint32_t>(human, 0xAD4) != 0 && Read<std::uint32_t>(human, 0xAD4) == Read<std::uint32_t>(human, 0xAD0)) { Write<std::uint32_t>(human, 0x36C, 0); return; }
        const auto last = Human_UserControl_GetLastCommandType(human);
        if ((last >= 83 && last <= 87) || (last >= 65 && last <= 69)) Write<std::uint8_t>(LastCommand(human), 0, 0);
        Write<std::uint32_t>(human, 0xAD0, 0);
        Human_l_FinishTask_Idle(human);
        if (!Human_IsJobSoldier(human) && !Human_IsJobHero(human))
        {
            const auto weapon = Human_GetEquipmentType_Weapon(human);
            if (weapon != 0) { Queue(human, 72, weapon, -1); return; }
            const auto armor = Human_GetEquipmentType_Armor(human);
            if (armor != 0) { Queue(human, 73, armor, -1); return; }
        }
        // The native idle finish clears this target; callbacks may repopulate it.
        auto* secondary = static_cast<std::byte*>(human) + 0xA84;
        if (!InteractionTargetData_IsValid(secondary)) return;
        const auto target = Read<std::uint32_t>(secondary, 0);
        if (target == 7 || target == 8) Queue(human, 4, MapMigPoint_GetLong(static_cast<std::byte*>(human) + 0xA88), 0);
        else if (target == 6)
        {
            if (Human_IsAttachedToVehicle(human) && Read<std::uint32_t>(human, 0xB4C) == Read<std::uint32_t>(human, 0xA9C)) Queue(human, 40, Read<std::uint32_t>(human, 0xA98), static_cast<std::int32_t>(Read<std::uint32_t>(human, 0xA9C)));
        }
        else if (target >= 1 && target <= 5 && Read<std::uint32_t>(human, 0xAD8) != 0)
        {
            const auto mode = Read<std::uint32_t>(human, 0xAD8);
            std::memcpy(static_cast<std::byte*>(human) + 0xA68, secondary, 28);
            Write(human, 0xAD0, Read<std::uint32_t>(human, 0xA94));
            if (mode >= 1 && mode <= 4) Human_l_StartAtomicWalkAndFetch(human, static_cast<std::byte*>(human) + 0xA6C, static_cast<std::byte*>(human) + 0xA70, mode == 1);
            else if (mode == 5)
            {
                Human_l_StartAtomicWalk(human, static_cast<std::byte*>(human) + 0xA6C, static_cast<std::byte*>(human) + 0xA70);
                Write(human, 0xA68, Read<std::uint32_t>(secondary, 0));
                Write<std::uint32_t>(human, 0xACC, 5);
            }
        }
        Human_Target_ResetSecondaryTarget(human);
    }
    // Original function: FUN_0046c310.
    void Human_l_FailTask_Equip(void* human)
    {
        const auto failure = Read<std::uint32_t>(human, 0x374);
        if (failure == 10) { Human_CurrentTask_DoInterrupt(human); return; }
        if (failure == 3) Human_UserMessage_SendToUserMessage_CannotFindEquipment(human);
        else if (failure == 12) Human_UserMessage_SendToUserMessage_BackpackIsFull(human);
        const auto good = Read<std::uint32_t>(human, 0xAD0);
        if (good == 30) Human_Equipment_ResetShoeType(human);
        else if (good == 31 || good == 32) Human_Equipment_ResetToolType(human);
        Write<std::uint32_t>(human, 0xAD0, 0);
    }

    // Original function: FUN_0042d63f; both Windows armor/weapon leaves call 0042D6E8.
    bool Human_CanEquipGoodType(const void* human, std::uint32_t good)
    {
        if (good == 30) return Human_CanEquipShoes(human);
        if (good == 31 || good == 32) return Human_CanEquipToolIron(human);
        const auto& data = gStaticGoodTypeStorage.records[good];
        if (data.armorType != 0) return Human_CanEquipArmorType(human, good);
        if (data.weaponType != 0) return Human_CanEquipArmorType(human, good); // Same folded Windows permission entry.
        return false;
    }

    // Original function: FUN_0046c369.
    void Human_l_FailTask_Drop(void* human)
    {
        const auto good = Read<std::uint32_t>(human, 0xAD4);
        if (!Human_CanEquipGoodType(human, good)) return;
        Write<std::uint32_t>(human, 0x36C, 0);
        Write(human, 0xAD0, good);
        Write<std::uint32_t>(human, 0x368, 16);
    }

    // Original function: FUN_0042d31a.
    bool Human_HasEquippedWeapon(const void* human) { return static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(human, 0x14)])[Read<std::uint32_t>(human, 0xAE4)].goodType != 0; }
    // Original function: FUN_0042d33c.
    bool Human_HasEquippedArmor(const void* human) { return Read<std::uint32_t>(human, 0xAE8) != 0; }
    // Original function: FUN_0042d3ff; the native routine names the stored good without conversion.
    const char* Human_GetReallyProducedGoodType_GetGuiNamePtr(const void* human) { return StringAccess_GetGoodNamePtr(Read<std::uint32_t>(human, 0x1C), 1); }
    // Original function: FUN_0042d453.
    std::uint32_t Human_Equipment_GetArmorType(const void* human) { return Read<std::uint32_t>(human, 0xAE8); }
    // Original function: FUN_0042d461; condition is retained.
    void Human_Equipment_ResetShoeType(void* human) { Write<std::uint32_t>(human, 0xAEC, 0); }
    // Original function: FUN_0042d469; condition is retained.
    void Human_Equipment_ResetToolType(void* human) { Write<std::uint32_t>(human, 0xAF0, 0); }
    // Original function: FUN_0042d48b.
    void Human_Equipment_ResetArmorType(void* human) { Write<std::uint32_t>(human, 0xAE8, 0); CCallbackManager::sTheObjectPtr->ExecuteCallback(0x20, true, Read<std::uint32_t>(human, 4), 0, 0); }
    // Original function: FUN_0042d6e8; Windows folds the armor and weapon permission entries.
    bool Human_CanEquipArmorType(const void* human, std::uint32_t good) { (void)good; const auto job = Read<std::int32_t>(human, 0x18); return (job >= 31 && job <= 41) || (job >= 42 && job <= 47); }

}
