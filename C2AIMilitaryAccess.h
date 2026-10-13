#pragma once
#include <cstdint>
#include <cstddef>
namespace NC2Logic
{
    // The existing slot accessors use a pointer four bytes before this physical record.
    struct SAIMilitaryPointStorage { std::uint32_t target[3], targetPolicy, lifetimePolicy, mainTaskIndex, radius, playerId; };
    struct SAIMilitarySlotStorage { std::uint8_t active; std::byte padding[3]; std::uint32_t role; SAIMilitaryPointStorage point; std::uint32_t minimumSoldiers, maximumSoldiers, secondaryType; };
    static_assert(sizeof(SAIMilitaryPointStorage) == 32);
    static_assert(sizeof(SAIMilitarySlotStorage) == 52);

    void AIMilitaryManager_Init(void* manager, std::uint32_t player, const void* center, std::uint32_t radius);
    void AIMilitaryManager_DoOneLogicGameTick(void* manager, void* high, std::uint32_t tick);
    void AIMilitaryManager_l_Soldier_Create(void* manager, const void* high);
    void AIMilitaryManager_l_Soldier_CheckOthersEquipment(void* manager);
    std::uint32_t AIMilitaryManager_l_Soldier_GetBestPossibleArmorType(void* manager, std::uint32_t armor, void* high);
    std::uint32_t AIMilitaryManager_l_Soldier_GetBestPossibleWeaponType(void* manager, std::uint32_t weapon, void* high);
    void AIMilitaryManager_l_Soldier_SetupCorrectEquipment(void* manager, void* high);
    void AIMilitaryManager_l_MilitaryPoints_Update(void* manager, void* high);
    void AIMilitaryManager_l_MilitaryPoints_Villages_Refresh(void* manager, const void* high);
    void AIMilitaryManager_l_EnemyTarget_CheckNext(void* manager, const void* center);
    bool AIMilitaryManager_l_EnemyTarget_GetBestTarget(const void* manager, std::uint32_t enemy, const void* point, void* target);
    bool AIMilitaryManager_Military_IsAnyTargetInRange(std::uint32_t player, const void* point, std::uint32_t radius);
    void AIMilitaryPoint_Clear(void* point);
    bool AIMilitaryPoint_l_IsValid(const void* point);
    bool AIMilitaryPoint_IsValid(const void* point);
    void AIMilitaryPoint_SetTarget(void* point, std::uint32_t player, const void* target, std::uint32_t radius, std::uint32_t type, std::uint32_t lifetime);
    std::int32_t AIMilitaryManager_MilitaryPoint_Add(void* manager, std::uint32_t type, std::uint32_t secondaryType, const void* target, std::uint32_t radius, std::uint32_t targetType, std::uint32_t lifetime);
    void AIMilitaryManager_MilitaryPoint_Clear(void* manager, std::int32_t index);
    void AIMilitaryManager_l_EnemyTarget_ClearCurrentTargetedEnemyPlayer(void* manager);
    void AIMilitaryManager_l_EnemyTarget_SelectCurrentTargetedEnemyPlayer(void* manager, const void* center);
    void AIMilitaryManager_l_EnemyTarget_CheckForAttack(void* manager);
    void AIMilitaryManager_Message_Soldier_Attacked(void* manager, const void* target);
    void AIMilitaryManager_l_MilitaryPoints_Tower_Refresh(void* manager, const void* high);
    void __fastcall AIHighLevelHandler_Message_Soldier_Attacked(void* handler, void* unusedEdx, const void* target);
}
