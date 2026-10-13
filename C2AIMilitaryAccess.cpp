#include "SwitchIdentifiers.h"
#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "C2AIMilitaryAccess.h"
#include "C2AIBuildingAccess.h"
#include "C2AITaskAccess.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanWalking.h"
#include "C2InteractionTargetAccess.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MapSectorAttachment.h"
#include "C2MilitaryTargetSearch.h"
#include "C2OverviewObjectIterators.h"
#include "C2AIAnimalDiplomacy.h"
#include "C2AIGlobalGoods.h"
#include "C2AIVillageAccess.h"
#include "C2AIResourceAccess.h"
#include "C2GameControlNetwork.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanEquipment.h"
#include "C2HumanInitialization.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2MapWalkSectorManager.h"
#include "C2PlayerCenterPosition.h"
#include "C2PlayerStatisticAccess.h"
#include "C2VehicleAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Original function: FUN_00453efb; equipment tables retain tribe-specific records.
    void AIMilitaryManager_Init(void* manager, std::uint32_t player, const void* center, std::uint32_t radius)
    {
        for (std::uint32_t i = 0; i < 100; ++i) Write<std::uint8_t>(manager, NC2Layout::AIMilitarySlotAccessOffset::Active + i * 52, 0);
        Write<std::int32_t>(manager, 0x1454, -1);
        Write(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId, player);
        const auto tribe = static_cast<std::int32_t>(player) < 20 ? Read<std::uint32_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 12) : 1;
        for (std::uint32_t t = 0; t < 8; ++t)
            for (std::uint32_t weapon = 0; weapon < 22; ++weapon)
            {
                const auto* data = static_cast<const std::byte*>(gStaticWeaponTypeArrays[t]) + weapon * 0xC4;
                const auto offset = NC2Layout::AIMilitaryManagerOffset::WeaponGoods + (t * 22 + weapon) * 12;
                if (Read<std::uint32_t>(data, 48) == 0) { Write<std::uint32_t>(manager, offset, 0); continue; }
                std::uint32_t best = 0; std::int32_t damage = 100000;
                for (std::uint32_t armor = 0; armor < 5; ++armor)
                {
                    const auto* armorData = static_cast<const std::byte*>(gStaticArmorTypeData) + armor * 52;
                    const auto candidate = Read<std::int32_t>(data, 56 + Read<std::uint32_t>(armorData, 40) * 4);
                    if (candidate >= damage || Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 56 + Read<std::uint32_t>(armorData, 36) + 0x3C78) == 0) continue;
                    best = armor; damage = candidate;
                }
                Write(manager, offset + 4, best);
                if (weapon == 4 || weapon == 5 || weapon == 7 || weapon == 8 || weapon == 16 || weapon == 17) Write(manager, offset, Read<std::uint32_t>(data, 36));
            }
        for (std::uint32_t armor = 0; armor < 5; ++armor)
        {
            const auto* data = static_cast<const std::byte*>(gStaticArmorTypeData) + armor * 52;
            Write(manager, NC2Layout::AIMilitaryManagerOffset::ArmorGoods + armor * 12, Read<std::uint32_t>(data, 36));
            std::uint32_t best = 0; std::int32_t damage = 0;
            for (std::uint32_t weapon = 0; weapon < 22; ++weapon)
            {
                if (weapon != 4 && weapon != 5 && weapon != 7 && weapon != 8 && weapon != 16 && weapon != 17) continue;
                const auto* weaponData = static_cast<const std::byte*>(gStaticWeaponTypeArrays[tribe]) + weapon * 0xC4;
                if (Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 56 + Read<std::uint32_t>(weaponData, 36) + 0x3C78) == 0) continue;
                const auto candidate = Read<std::int32_t>(weaponData, 56 + Read<std::uint32_t>(data, 40) * 4);
                if (candidate > damage) { damage = candidate; best = weapon; }
            }
            Write(manager, NC2Layout::AIMilitaryManagerOffset::ArmorPreferredWeapon + armor * 12, best);
        }
        auto* single = Read<void*>(gAIManager, player * 4);
        if (Read<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::FirstMainTaskDescription) == 1 && Read<std::uint16_t>(single, NC2Layout::AISinglePlayerOffset::FirstMainTaskPosition) == Read<std::uint16_t>(center, 0)) { Write<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::FirstMainTaskDescription, 0);
        AISinglePlayerHandler_MainTask_RecheckAll(single); }
        AISinglePlayerHandler_EntryPoint_SingleSoldier_SetDefaultPosition(single, center, radius * 2);
        for (std::uint32_t i = 0; i < 20; ++i) AIMilitaryPoint_Clear(static_cast<std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::EnemyTargets + i * 32);
        Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor, -1);
        Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyPlayer, -1);
        Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex, -1);
    }
    // Original function: FUN_004545d4.
    void AIMilitaryManager_DoOneLogicGameTick(void* manager, void* high, std::uint32_t tick)
    {
        auto* villages = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages;
        if (AIVillagesData_Good_IsProducingGood(villages, 19, false))
        {
            if (AIVillagesData_Military_IsBarrackBuild(villages))
            {
                AIMilitaryManager_l_Soldier_Create(manager, high);
                AIMilitaryManager_l_Soldier_CheckOthersEquipment(manager);
                AIMilitaryManager_l_Soldier_SetupCorrectEquipment(manager, high);
                AIMilitaryManager_l_EnemyTarget_CheckNext(manager, static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::FirstVillageCenter);
                if (tick % 10 == 0) { AIMilitaryManager_l_EnemyTarget_SelectCurrentTargetedEnemyPlayer(manager, static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::FirstVillageCenter);
                AIMilitaryManager_l_EnemyTarget_CheckForAttack(manager); }
            }
            else AIVillagesData_House_BuildHouse(villages, 39, true, nullptr, 0);
        }
        if (tick % 10 == 0) { AIMilitaryManager_l_MilitaryPoints_Tower_Refresh(manager, high);
        AIMilitaryManager_l_MilitaryPoints_Villages_Refresh(manager, high); }
        if (tick % 5 == 0) AIMilitaryManager_l_MilitaryPoints_Update(manager, high);
    }
    // Original function: FUN_00454687; reservations remain queued until the human changes job.
    void AIMilitaryManager_l_Soldier_Create(void* manager, const void* high)
    {
        const auto* villages = static_cast<const std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages;
        auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(AIVillagesData_Military_GetBarrackHouseId(villages)) * CHouseArray::ElementSize;
        if (Read<std::uint32_t>(house, NC2Layout::HouseOffset::ConstructionState) != 3) return;
        const auto player = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId);
        auto count = Read<std::uint16_t>(PlayerStatisticManager_GetCurrentStatistics(gPlayerStatisticManager, player), 18);
        if (count > 190) return;
        auto needed = static_cast<std::int32_t>(AIVillagesData_GetNumberOfUsedHumans(villages) * 120) / 100;
        CHumanIterator humans;
        for (auto id = humans.GetFirstSectorHumanId(player, false); id != -1; id = humans.GetNextSectorHumanId())
        {
            const auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (Human_IsJobBuilder(human) && Human_IsAttachedToBuildSite(human)) continue;
            if (Human_IsAgeAdult(human) && Human_IsSexMale(human)) --needed;
        }
        while (needed < 0)
        {
            const auto id = AIHighLevelHandler_Tool_GetUnusedHuman(high, house + NC2Layout::HouseOffset::DoorEntryPoint);
            if (id == -1) return;
            auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            Human_DoAddNewUserCommand(human, 32, static_cast<std::uint8_t>(player), id, Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId), Read<std::uint32_t>(house, NC2Layout::HouseOffset::HouseId), Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
            if (++count > 190) return;
            ++needed;
        }
    }
    // Original function: FUN_0045479c.
    void AIMilitaryManager_l_Soldier_CheckOthersEquipment(void* manager)
    {
        for (std::uint32_t i = 0; i < 8 * 22; ++i) Write<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponWeights + i * 12, 0);
        for (std::uint32_t i = 0; i < 5; ++i) Write<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorWeights + i * 12, 0);
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            if (Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned) == 0 || player == Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId)) continue;
            const auto state = GameControl_Diplomacy_GetState(gGameControl, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId), player);
            if (state == 1) continue;
            const std::uint32_t weight = state == 3 ? 5 : 1;
            CHumanIterator humans;
            for (auto id = humans.GetFirstSectorHumanId(player, true); id != -1; id = humans.GetNextSectorHumanId())
            {
                const auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                const auto weapon = NC2Layout::AIMilitaryManagerOffset::WeaponWeights + (Read<std::uint32_t>(human, NC2Layout::HumanOffset::TribeType) * 22 + Human_Equipment_GetWeaponType(human)) * 12;
                Write(manager, weapon, Read<std::uint32_t>(manager, weapon) + weight);
                const auto armor = NC2Layout::AIMilitaryManagerOffset::ArmorWeights + Human_Equipment_GetArmorType(human) * 12;
                Write(manager, armor, Read<std::uint32_t>(manager, armor) + weight);
            }
        }
    }
    // Original function: FUN_004548aa.
    std::uint32_t AIMilitaryManager_l_Soldier_GetBestPossibleArmorType(void* manager, std::uint32_t armor, void* high)
    {
        auto* villages = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages; const auto* catcher = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::AnimalCatcher;
        if ((armor == 4 || armor == 3) && AIVillagesData_Good_IsProducingGood(villages, 32, true) && AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorGoods + armor * 12), true)) return armor;
        if ((!AIAnimalCatcher_IsType19Available(catcher) || armor == 2) && AIAnimalCatcher_IsType10Available(catcher) && AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorTwoGood), true)) return 2;
        if (AIAnimalCatcher_IsType19Available(catcher) && AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorOneGood), true)) return 1;
        return 0;
    }
    // Original function: FUN_00454958.
    std::uint32_t AIMilitaryManager_l_Soldier_GetBestPossibleWeaponType(void* manager, std::uint32_t weapon, void* high)
    {
        const auto player = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId);
        const auto tribe = static_cast<std::int32_t>(player) < 20 ? Read<std::uint32_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 12) : 1;
        auto* villages = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages;
        if ((weapon == 5 || weapon == 7 || weapon == 8) && AIVillagesData_Good_IsProducingGood(villages, 32, true) && AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponGoods + (tribe * 22 + weapon) * 12), true)) return weapon;
        if ((weapon == 16 || weapon == 17) && AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponGoods + (tribe * 22 + weapon) * 12), true)) return weapon;
        const auto good = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponFourGood + tribe * 264);
        if (Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 56 + good + 0x3C78) != 0 && AIVillagesData_Good_IsProducingGood(villages, good, true)) return 4;
        return 0;
    }
    // Original function: FUN_004557ed.
    void AIMilitaryManager_l_MilitaryPoints_Villages_Refresh(void* manager, const void* high)
    {
        for (std::uint32_t i = 0; i < 8; ++i)
        {
            const auto* entry = static_cast<const std::byte*>(high) + NC2Layout::AIHighLevelOffset::FirstVillageActive + i * 0xCF0;
            if (Read<std::uint8_t>(entry, 0) == 0) continue;
            std::byte target[12]{};
            Write<std::uint32_t>(target, 0, 5);
            Write(target, 4, Read<std::uint32_t>(entry, 16));
            Write<std::uint16_t>(target, 10, 0);
            AIMilitaryManager_MilitaryPoint_Add(manager, 1, 0, target, Read<std::uint32_t>(entry, 20) + 30, 1, 5);
        }
    }
    // Original function: FUN_00455860; process one eligible enemy per invocation.
    void AIMilitaryManager_l_EnemyTarget_CheckNext(void* manager, const void* center)
    {
        Write(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor) + 1);
        for (;;)
        {
            const auto enemy = Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor);
            if (enemy == 20) { Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor, -1); return; }
            const auto player = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId);
            if (static_cast<std::uint32_t>(enemy) != player && enemy < 20 && Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(enemy) * NC2Layout::RecordSize::GameControlPlayerSigned) != 0 && Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + enemy + 0x45F8) != 0 && Read<std::uint8_t>(gGameControl, static_cast<std::uint32_t>(enemy) * NC2Layout::RecordSize::GameControlPlayerSigned + 8) == 0)
            {
                auto* target = static_cast<std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::EnemyTargets + static_cast<std::uint32_t>(enemy) * 32;
                if (GameControl_Diplomacy_GetState(gGameControl, player, enemy) != 3) AIMilitaryPoint_Clear(target);
                else
                {
                    std::uint32_t enemyCenter = 0; Tool_Misc_GetPlayerCenterPosition(enemy, &enemyCenter);
                    if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(center), 6) == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&enemyCenter), 6))
                    {
                        std::uint32_t form = 0;
                        if (!CMapWalkSectorManager::AI_Military_GetFormPoint(player, center, &enemyCenter, &form)) form = Read<std::uint32_t>(center, 0);
                        std::uint32_t candidate[3]{};
                        if (AIMilitaryManager_l_EnemyTarget_GetBestTarget(manager, enemy, &form, candidate)) AIMilitaryPoint_SetTarget(target, player, candidate, 60, 1, 3);
                        else AIMilitaryPoint_Clear(target);
                        return;
                    }
                }
            }
            Write(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::EnemyScanCursor) + 1);
        }
    }
    // Original function: FUN_004559de; Windows repeats its first house-sector query on failure.
    bool AIMilitaryManager_l_EnemyTarget_GetBestTarget(const void* manager, std::uint32_t enemy, const void* point, void* target)
    {
        Write<std::uint32_t>(target, 0, 0); std::int32_t best = 100000;
        const auto player = Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId); CHouseIterator houses;
        const auto searchHouses = [&]()
        {
            for (auto id = houses.GetFirstSectorHouseId(enemy, point, 10000, false); id != -1; id = houses.GetNextSectorHouseId())
            {
                const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                const auto distance = VE_HexagonDirection_GetDistance(Read<std::uint16_t>(house, NC2Layout::HouseOffset::DoorEntryPointX), Read<std::uint16_t>(house, NC2Layout::HouseOffset::DoorEntryPointY), Read<std::uint16_t>(point, 0), Read<std::uint16_t>(point, 2));
                const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(house + NC2Layout::HouseOffset::DoorEntryPoint, player, 5);
                const auto score = static_cast<std::int32_t>(static_cast<std::uint32_t>(distance) + static_cast<std::uint32_t>(danger));
                if (score >= best) continue;
                best = score;
                Write(target, 4, id);
                Write(target, 8, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                Write<std::uint32_t>(target, 0, 4);
            }
        };
        searchHouses();
        if (!TargetData_IsValid(target)) searchHouses();
        if (!TargetData_IsValid(target))
        {
            CHumanIterator humans;
            for (auto id = humans.GetFirstSectorHumanId(enemy, false); id != -1; id = humans.GetNextSectorHumanId())
            {
                const auto* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                if (!Human_IsAgeAdult(human) || !Human_IsSexMale(human)) continue;
                const auto* position = Human_GetMapPositionRef(human);
                const auto distance = VE_HexagonDirection_GetDistance(Read<std::uint16_t>(position, 0), Read<std::uint16_t>(position, 2), Read<std::uint16_t>(point, 0), Read<std::uint16_t>(point, 2));
                const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(Human_GetMapPositionRef(human), player, 5);
                const auto score = static_cast<std::int32_t>(static_cast<std::uint32_t>(distance) + static_cast<std::uint32_t>(danger));
                if (score >= best) continue;
                best = score;
                Write(target, 4, id);
                Write(target, 8, Read<std::uint32_t>(human, NC2Layout::HumanOffset::UniqueId));
                Write<std::uint32_t>(target, 0, 1);
            }
        }
        return TargetData_IsValid(target);
    }
    // Original function: FUN_00455449.
    void AIMilitaryManager_l_MilitaryPoints_Update(void* manager, void* high)
    {
        const auto player = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId); auto* single = Read<void*>(gAIManager, player * 4);
        const auto tribe = static_cast<std::int32_t>(player) < 20 ? Read<std::uint32_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 12) : 1;
        auto* villages = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages;
        for (std::uint32_t index = 0; index < 100; ++index)
        {
            auto* entry = static_cast<std::byte*>(manager) + index * NC2Layout::RecordSize::AIMilitarySlot;
            if (Read<std::uint8_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Active) == 0) continue;
            if (!AIMilitaryPoint_IsValid(entry + NC2Layout::AIMilitarySlotAccessOffset::Point)) { AIMilitaryManager_MilitaryPoint_Clear(manager, index); continue; }
            std::uint32_t point = 0; TargetData_GetMapPosition(entry + NC2Layout::AIMilitarySlotAccessOffset::Point, &point); Tool_Map_GetNotBlockedPosition(&point, true);
            std::byte task[68]{}; std::uint32_t maximum = 0, minimum = 0, priority = 0;
            const auto attack = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Role) == 2;
            if (attack)
            {
                auto danger = AIMilitaryManager_Military_GetDangerValueForPosition(&point, player);
                if (danger > 0)
                {
                    maximum = static_cast<std::int32_t>(static_cast<std::uint32_t>(danger) + 5) / 10;
                    if (AIVillagesData_Good_IsProducingGood(villages, 6, false) && maximum > 1) maximum >>= 1;
                    if (AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, tribe * 264 + NC2Layout::AIMilitaryManagerOffset::WeaponSeventeenGood), false) || AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, tribe * 264 + NC2Layout::AIMilitaryManagerOffset::WeaponFourGood), false) || AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorTwoGood), false) || AIVillagesData_Good_IsProducingGood(villages, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorOneGood), false)) if (maximum > 1) maximum >>= 1;
                    minimum = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::TargetPolicy) == 3 ? maximum : (maximum >> 1) + (maximum & 1);
                    if (Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MaximumSoldiers) != 0)
                    {
                        if (Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MaximumSoldiers) > maximum) maximum = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MaximumSoldiers);
                        if (minimum > Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MinimumSoldiers)) minimum = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MinimumSoldiers) + 1;
                    }
                    Write(entry, NC2Layout::AIMilitarySlotAccessOffset::MaximumSoldiers, maximum);
                    Write(entry, NC2Layout::AIMilitarySlotAccessOffset::MinimumSoldiers, minimum);
                    const auto count = Read<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::SoldierCount);
                    if (count != 0) priority = maximum * 100 / count;
                }
                AIMainTask_Init(task, 2, priority, 100000); Write(task, NC2Layout::AIMainTaskOffset::PrimaryPosition, point);
                std::uint32_t form = 0;
                if (!CMapWalkSectorManager::AI_Military_GetFormPoint(player, static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::FirstVillageCenter, &point, &form)) form = Read<std::uint32_t>(high, NC2Layout::AIHighLevelOffset::FirstVillageCenter);
                Write(task, NC2Layout::AIMainTaskOffset::SecondaryPosition, form);
                Write<std::uint32_t>(task, NC2Layout::AIMainTaskOffset::MilitaryMode, 1);
            }
            else
            {
                const auto type = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::TargetPolicy);
                if (type == 2) { maximum = 2; priority = 5; }
                else if (type == 1 || type == 4)
                {
                    const auto danger = AIMilitaryManager_Military_GetDangerValueForPosition(&point, player);
                    if (danger > 5)
                    {
                        maximum = static_cast<std::int32_t>(static_cast<std::uint32_t>(danger) + 5) / 10; minimum = (maximum >> 1) + (maximum & 1);
                        const auto count = Read<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::SoldierCount); if (count != 0) priority = maximum * 50 / count;
                    }
                }
                AIMainTask_Init(task, 1, priority, 100000); Write(task, NC2Layout::AIMainTaskOffset::PrimaryPosition, point);
            }
            Write(task, NC2Layout::AIMainTaskOffset::Radius, Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Radius));
            Write(task, NC2Layout::AIMainTaskOffset::MinimumSoldiers, minimum);
            Write(task, NC2Layout::AIMainTaskOffset::MaximumSoldiers, maximum);
            const auto taskId = Read<std::int32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MainTaskIndex);
            if (taskId == -1)
            {
                const auto added = AISinglePlayerHandler_MainTask_Add(single, task); if (added != NXBasics::IntegerRange::UInt32Maximum) Write(entry, NC2Layout::AIMilitarySlotAccessOffset::MainTaskIndex, added);
                continue;
            }
            auto* old = static_cast<std::byte*>(single) + NC2Layout::AISinglePlayerOffset::MainTasks + static_cast<std::uint32_t>(taskId) * 68;
            if (Read<std::uint32_t>(old, NC2Layout::AIMainTaskOffset::ConfiguredPriority) == priority && AITaskSimpleDescription_IsEqualTo(old + NC2Layout::AIMainTaskOffset::Description, task + 8)) continue;
            if (attack && Read<std::uint32_t>(old, NC2Layout::AIMainTaskOffset::ConfiguredPriority) != 0)
            {
                const auto formDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(old, NC2Layout::AIMainTaskOffset::SecondaryPosition), Read<std::uint16_t>(old, NC2Layout::AIMainTaskOffset::SecondaryPositionY), Read<std::uint16_t>(task, NC2Layout::AIMainTaskOffset::SecondaryPosition), Read<std::uint16_t>(task, NC2Layout::AIMainTaskOffset::SecondaryPositionY)));
                if (formDistance <= 60)
                {
                    const auto targetDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::uint16_t>(old, NC2Layout::AIMainTaskOffset::PrimaryPosition), Read<std::uint16_t>(old, NC2Layout::AIMainTaskOffset::PrimaryPositionY), Read<std::uint16_t>(task, NC2Layout::AIMainTaskOffset::PrimaryPosition), Read<std::uint16_t>(task, NC2Layout::AIMainTaskOffset::PrimaryPositionY)));
                    if (targetDistance <= 60) continue;
                }
            }
            for (std::uint32_t offset = 0; offset < 68; offset += 4) Write(old, offset, Read<std::uint32_t>(task, offset));
            AISinglePlayerHandler_MainTask_RecheckAll(single);
        }
    }
    // Original function: FUN_00454a3e; adjust queued soldiers before choosing equipment quotas.
    void AIMilitaryManager_l_Soldier_SetupCorrectEquipment(void* manager, void* high)
    {
        const auto player = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId); auto* single = Read<void*>(gAIManager, player * 4);
        const auto tribe = static_cast<std::int32_t>(player) < 20 ? Read<std::uint32_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + 12) : 1;
        auto count = Read<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::SoldierCount); if (count == 0) return;
        auto remaining = count; auto* villages = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::Villages;
        const auto available = [&](std::uint32_t good) { return Read<std::uint8_t>(gGameControl, player * NC2Layout::RecordSize::GameControlPlayerSigned + tribe * 56 + good + 0x3C78) != 0; };
        const auto weaponGood = [&](std::uint32_t weapon) { return Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponGoods + (tribe * 22 + weapon) * 12); };
        const auto armorGood = [&](std::uint32_t armor) { return Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorGoods + armor * 12); };
        const auto soldierAt = [&](std::uint32_t i) { return static_cast<std::byte*>(single) + NC2Layout::AISinglePlayerOffset::Soldiers + i * 84; };
        const auto humanAt = [&](const void* soldier) { return CHumanArray::mStaticVars + Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::HumanId) * CHumanArray::ElementSize; };
        const auto inVillage = [&](const void* human) { return AIVillagesData_GetVillageId(villages, Human_GetMapPositionRef(human)) != -1; };
        const auto equip = [&](void* human, std::uint8_t opcode, std::uint32_t good) { Human_DoAddNewUserCommand(human, opcode, static_cast<std::uint8_t>(player), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), good, 0); };
        std::int32_t percentage = 0; std::uint32_t ranged = 0;
        if (available(weaponGood(17)) && AIVillagesData_Good_IsProducingGood(villages, weaponGood(17), true)) { percentage = 30; ranged = 17; }
        else if (available(weaponGood(16)) && AIVillagesData_Good_IsProducingGood(villages, weaponGood(16), true)) { percentage = 40; ranged = 16; }
        std::uint32_t weaponWeight = 0, armorWeight = 0;
        for (std::uint32_t i = 0; i < 8 * 22; ++i) weaponWeight += Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponWeights + i * 12);
        for (std::uint32_t i = 0; i < 5; ++i) armorWeight += Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorWeights + i * 12);
        std::int32_t weapons[22]{}, armors[5]{};
        for (std::uint32_t i = 0; i < count; ++i)
        {
            auto* soldier = soldierAt(i); const auto state = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction);
            if (state == SwitchIds::AISoldierAction::ChangingEquipment || state == SwitchIds::AISoldierAction::Attacking) { --remaining; continue; }
            auto* human = humanAt(soldier); const auto weapon = Human_Equipment_GetWeaponType(human);
            Write(soldier, NC2Layout::AISoldierOffset::WeaponType, weapon); ++weapons[weapon];
            const auto armor = Human_Equipment_GetArmorType(human);
            Write(soldier, NC2Layout::AISoldierOffset::ArmorType, armor); ++armors[armor];
        }
        const auto wantedRanged = static_cast<std::int32_t>(static_cast<std::uint32_t>(percentage) * remaining) / 100;
        if (weapons[ranged] < wantedRanged)
        {
            auto needed = wantedRanged - weapons[ranged]; if (ranged == 16) needed -= weapons[17];
            if (needed > 0)
            {
                auto reserved = ranged == 17 ? (weapons[16] <= needed ? weapons[16] : needed) : 0;
                auto fresh = reserved < needed ? needed - reserved : 0;
                for (std::uint32_t i = 0; i < count && needed != 0; ++i)
                {
                    auto* soldier = soldierAt(i); const auto state = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction);
                    if (state == SwitchIds::AISoldierAction::ChangingEquipment || state == SwitchIds::AISoldierAction::Attacking) continue;
                    const auto current = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::WeaponType);
                    if (current == 16)
                    {
                        if (ranged == 17)
                        {
                            auto* human = humanAt(soldier);
                            if (inVillage(human))
                            {
                                equip(human, 81, weaponGood(17)); --weapons[current]; --remaining; --needed;
                                Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
                                Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::WeaponType, 17);
                            }
                            else { ++reserved; if (weapons[16] < reserved) ++fresh; }
                        }
                        continue;
                    }
                    if (current == 17 || fresh == 0) continue;
                    auto* human = humanAt(soldier); if (!inVillage(human)) continue;
                    equip(human, 81, weaponGood(ranged)); --weapons[current]; --remaining; --needed; --fresh;
                    Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
                    Write(soldier, NC2Layout::AISoldierOffset::WeaponType, ranged);
                }
            }
        }
        else remaining -= static_cast<std::uint32_t>(wantedRanged);
        if (remaining != 0)
        {
            std::uint32_t wantedWeapons[22]{}, wantedArmors[5]{};
            std::uint32_t possibleWeapons[22], possibleArmors[5];
            if (weaponWeight != 0)
                for (std::uint32_t i = 0; i < 8 * 22; ++i)
                {
                    const auto amount = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponWeights + i * 12) * remaining / weaponWeight;
                    if (amount != 0) wantedArmors[Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::WeaponPreferredArmor + i * 12)] += amount;
                }
            if (armorWeight != 0)
                for (std::uint32_t i = 0; i < 5; ++i)
                {
                    const auto amount = Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorWeights + i * 12) * remaining / armorWeight;
                    if (amount != 0) wantedWeapons[Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::ArmorPreferredWeapon + i * 12)] += amount;
                }
            for (std::uint32_t i = 0; i < 22; ++i) if (wantedWeapons[i] != 0) possibleWeapons[i] = AIMilitaryManager_l_Soldier_GetBestPossibleWeaponType(manager, i, high);
            for (std::uint32_t i = 0; i < 5; ++i) if (wantedArmors[i] != 0) possibleArmors[i] = AIMilitaryManager_l_Soldier_GetBestPossibleArmorType(manager, i, high);
            for (std::uint32_t i = 0; i < count; ++i)
            {
                auto* soldier = soldierAt(i); const auto state = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction);
                if (state == SwitchIds::AISoldierAction::ChangingEquipment || state == SwitchIds::AISoldierAction::Attacking) continue;
                auto* human = humanAt(soldier); if (!inVillage(human)) continue;
                const auto weapon = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::WeaponType);
                if (weapon != 17 && weapon != 16 && weapons[weapon] > static_cast<std::int32_t>(wantedWeapons[weapon]) && weaponWeight != 0)
                    for (std::uint32_t next = 1; next < 22; ++next)
                    {
                        if (weapons[next] >= static_cast<std::int32_t>(wantedWeapons[next]) || possibleWeapons[next] == 0) continue;
                        equip(human, 81, weaponGood(possibleWeapons[next])); ++weapons[next]; --weapons[weapon];
                        Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
                        Write(soldier, NC2Layout::AISoldierOffset::WeaponType, next); break;
                    }
                const auto armor = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::ArmorType);
                if (armors[armor] > static_cast<std::int32_t>(wantedArmors[armor]) && armorWeight != 0)
                    for (std::uint32_t next = 1; next < 5; ++next)
                    {
                        if (armors[next] >= static_cast<std::int32_t>(wantedArmors[next]) || possibleArmors[next] == 0) continue;
                        equip(human, 82, armorGood(possibleArmors[next])); ++armors[next]; --armors[armor];
                        Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
                        Write(soldier, NC2Layout::AISoldierOffset::ArmorType, next); break;
                    }
            }
        }
        std::uint32_t defaultWeapon = 1, defaultArmor = 0;
        if (available(weaponGood(4))) defaultWeapon = AIVillagesData_Good_IsProducingGood(villages, weaponGood(4), true) ? 4 : 1;
        else if (available(weaponGood(7)) && AIVillagesData_Good_IsProducingGood(villages, weaponGood(7), true) && AIVillagesData_Good_IsProducingGood(villages, 32, true)) defaultWeapon = 7;
        const auto* catcher = static_cast<std::byte*>(high) + NC2Layout::AIHighLevelOffset::AnimalCatcher;
        if (AIAnimalCatcher_IsType10Available(catcher)) { if (AIVillagesData_Good_IsProducingGood(villages, armorGood(2), true)) defaultArmor = 2; }
        else if (AIAnimalCatcher_IsType19Available(catcher) && AIVillagesData_Good_IsProducingGood(villages, armorGood(1), true)) defaultArmor = 1;
        for (std::uint32_t i = 0; i < count; ++i)
        {
            auto* soldier = soldierAt(i); const auto state = Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction);
            if (state == SwitchIds::AISoldierAction::ChangingEquipment || state == SwitchIds::AISoldierAction::Attacking) continue;
            if ((Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::WeaponType) != 1 || defaultWeapon == 1) && (Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::ArmorType) != 0 || defaultArmor == 0)) continue;
            auto* human = humanAt(soldier); if (!inVillage(human)) continue;
            if (Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::WeaponType) == 1 && defaultWeapon != 1) { equip(human, 81, weaponGood(defaultWeapon));
            Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
            Write(soldier, NC2Layout::AISoldierOffset::WeaponType, defaultWeapon); }
            if (Read<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::ArmorType) == 0 && defaultArmor != 0) { equip(human, 82, armorGood(defaultArmor));
            Write<std::uint32_t>(soldier, NC2Layout::AISoldierOffset::CurrentAction, SwitchIds::AISoldierAction::ChangingEquipment);
            Write(soldier, NC2Layout::AISoldierOffset::ArmorType, defaultArmor); }
        }
    }
    namespace
    {
        // Original function: FUN_00455398; only the first four DWORDs participate.
        bool AIMilitaryPoint_IsEqualTo(const void* point, const void* other)
        {
            return Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::TargetUniqueId) == Read<std::uint32_t>(other, NC2Layout::AIMilitaryPointOffset::TargetUniqueId) && Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::TargetIdOrMapPoint) == Read<std::uint32_t>(other, NC2Layout::AIMilitaryPointOffset::TargetIdOrMapPoint) && Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::Target) == Read<std::uint32_t>(other, NC2Layout::AIMilitaryPointOffset::Target) && Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::TargetPolicy) == Read<std::uint32_t>(other, NC2Layout::AIMilitaryPointOffset::TargetPolicy);
        }
    }
    // Original function: FUN_00454578.
    bool AIMilitaryManager_Military_IsAnyTargetInRange(std::uint32_t player, const void* point, std::uint32_t radius)
    {
        if (AI_IsEnemyObjectAround(player, point, radius)) return true;
        if (Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, point, 0, radius, false, 1, 0, true, false) != -1) return true;
        return Tool_GetOneOfNextEnemyHousesInRange(player, point, 0, radius, false, 1, 0) != -1;
    }
    // Original function: FUN_00472a8e.
    void AIMilitaryPoint_Clear(void* point) { Write<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::TargetPolicy, 0);
    Write<std::int32_t>(point, NC2Layout::AIMilitaryPointOffset::MainTaskIndex, -1); }
    // Original function: FUN_00472a97.
    bool AIMilitaryPoint_l_IsValid(const void* point)
    {
        const auto type = Read<std::int32_t>(point, NC2Layout::AIMilitaryPointOffset::TargetPolicy);
        if (type <= 0) return false;
        if (type <= 3) return TargetData_IsValid(point);
        if (type != 4) return false;
        std::uint32_t position = 0;
        TargetData_GetMapPosition(point, &position);
        return Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(&position), 0) != 3;
    }
    // Original function: FUN_00472ae7.
    bool AIMilitaryPoint_IsValid(const void* point)
    {
        if (!AIMilitaryPoint_l_IsValid(point)) return false;
        const auto lifetime = Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::LifetimePolicy);
        if (lifetime == 0) return false;
        if (lifetime != 2 && lifetime != 4 && lifetime != 5) return true;
        std::uint32_t position = 0;
        TargetData_GetMapPosition(point, &position);
        const auto player = Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::PlayerId);
        const auto radius = Read<std::uint32_t>(point, NC2Layout::AIMilitaryPointOffset::Radius);
        if (lifetime == 2) return AI_IsEnemyObjectAround(player, &position, radius);
        if (lifetime == 4) return AIMilitaryManager_Military_IsAnyTargetInRange(player, &position, radius);
        CHouseIterator iterator;
        if (iterator.GetFirstSectorHouseId(player, &position, radius, false) != -1) return true;
        return iterator.GetFirstSectorHouseId(player, &position, radius, true) != -1;
    }
    // Original function: FUN_00472bd7.
    void AIMilitaryPoint_SetTarget(void* point, std::uint32_t player, const void* target, std::uint32_t radius, std::uint32_t type, std::uint32_t lifetime)
    {
        Write(point, NC2Layout::AIMilitaryPointOffset::PlayerId, player);
        Write(point, NC2Layout::AIMilitaryPointOffset::Radius, radius);
        for (std::uint32_t i = 0; i < 12; i += 4) Write(point, i, Read<std::uint32_t>(target, i));
        Write(point, NC2Layout::AIMilitaryPointOffset::TargetPolicy, type);
        Write(point, NC2Layout::AIMilitaryPointOffset::LifetimePolicy, lifetime);
    }
    // Original function: FUN_004552f1; slot accesses use a base four bytes before the physical 52-byte record.
    std::int32_t AIMilitaryManager_MilitaryPoint_Add(void* manager, std::uint32_t type, std::uint32_t secondaryType, const void* target, std::uint32_t radius, std::uint32_t targetType, std::uint32_t lifetime)
    {
        std::byte point[32]{};
        AIMilitaryPoint_Clear(point);
        AIMilitaryPoint_SetTarget(point, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId), target, radius, targetType, lifetime);
        std::int32_t free = -1;
        for (std::int32_t i = 0; i < 100; ++i)
        {
            const auto* entry = static_cast<const std::byte*>(manager) + static_cast<std::uint32_t>(i) * 52;
            if (Read<std::uint8_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Active) != 0)
            {
                if (AIMilitaryPoint_IsEqualTo(point, entry + NC2Layout::AIMilitarySlotAccessOffset::Point)) return i;
            }
            else if (free == -1) free = i;
        }
        if (free == -1) return free;
        auto* entry = static_cast<std::byte*>(manager) + static_cast<std::uint32_t>(free) * 52;
        for (std::uint32_t i = 0; i < 32; i += 4) Write(entry, NC2Layout::AIMilitarySlotAccessOffset::Point + i, Read<std::uint32_t>(point, i));
        Write(entry, NC2Layout::AIMilitarySlotAccessOffset::Role, type);
        Write<std::uint8_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Active, 1);
        Write(entry, NC2Layout::AIMilitarySlotAccessOffset::SecondaryType, secondaryType);
        Write<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MinimumSoldiers, 0);
        Write<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MaximumSoldiers, 0);
        return free;
    }
    // Original function: FUN_004553c4.
    void AIMilitaryManager_MilitaryPoint_Clear(void* manager, std::int32_t index)
    {
        auto* entry = static_cast<std::byte*>(manager) + static_cast<std::uint32_t>(index) * 52;
        if (Read<std::uint8_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Active) != 0)
        {
            const auto task = Read<std::int32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::MainTaskIndex);
            if (task != -1)
            {
                void* single = Read<void*>(gAIManager, Read<std::uint32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId) * 4);
                Write<std::uint32_t>(single, NC2Layout::AISinglePlayerOffset::FirstMainTaskDescriptionUnsigned + static_cast<std::uint32_t>(task) * 68, 0);
                AISinglePlayerHandler_MainTask_RecheckAll(single);
                AIMilitaryPoint_Clear(entry + NC2Layout::AIMilitarySlotAccessOffset::Point);
            }
            Write<std::uint8_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::Active, 0);
        }
        const auto secondary = Read<std::uint32_t>(entry, NC2Layout::AIMilitarySlotAccessOffset::SecondaryType);
        if (secondary == 1 && Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex) == index) AIMilitaryManager_l_EnemyTarget_ClearCurrentTargetedEnemyPlayer(manager);
        else if (secondary == 2)
        {
            MapMigPoint_Construct(static_cast<std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::AttackedPosition, -1, -1, false);
            Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::AttackedPositionMilitaryPointIndex, -1);
        }
    }
    // Original function: FUN_00455cc2; clear the reference only after the original nested clear.
    void AIMilitaryManager_l_EnemyTarget_ClearCurrentTargetedEnemyPlayer(void* manager)
    {
        const auto index = Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex);
        if (index != -1 && Read<std::uint8_t>(manager, static_cast<std::uint32_t>(index) * NC2Layout::RecordSize::AIMilitarySlot + 4) != 0) AIMilitaryManager_MilitaryPoint_Clear(manager, index);
        Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex, -1);
        Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyPlayer, -1);
    }
    // Original function: FUN_00455c0c.
    void AIMilitaryManager_l_EnemyTarget_SelectCurrentTargetedEnemyPlayer(void* manager, const void* center)
    {
        if (Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyPlayer) != -1) return;
        std::int32_t best = -1;
        std::uint32_t score = 100000;
        for (std::int32_t i = 0; i < 20; ++i)
        {
            const auto* target = static_cast<const std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::EnemyTargets + static_cast<std::uint32_t>(i) * 32;
            if (!AIMilitaryPoint_IsValid(target)) continue;
            std::uint32_t position = 0;
            TargetData_GetMapPosition(target, &position);
            Tool_Map_GetNotBlockedPosition(&position, true);
            const auto danger = AIMilitaryManager_Military_GetCivilianDangerValueForPosition(&position, Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::PlayerId), 5);
            const auto distance = VE_HexagonDirection_GetDistance(Read<std::int16_t>(center, 0), Read<std::int16_t>(center, 2), Read<std::int16_t>(&position, 0), Read<std::int16_t>(&position, 2));
            const auto candidate = static_cast<std::uint32_t>(danger) + static_cast<std::uint32_t>(distance);
            if (static_cast<std::int32_t>(candidate) >= static_cast<std::int32_t>(score)) continue;
            score = candidate; best = i;
        }
        Write(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyPlayer, best);
    }
    // Original function: FUN_00455cf4.
    void AIMilitaryManager_l_EnemyTarget_CheckForAttack(void* manager)
    {
        const auto enemy = Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyPlayer);
        if (enemy == -1) return;
        const auto* target = static_cast<const std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::EnemyTargets + static_cast<std::uint32_t>(enemy) * 32;
        if (!AIMilitaryPoint_IsValid(target)) { AIMilitaryManager_l_EnemyTarget_ClearCurrentTargetedEnemyPlayer(manager); return; }
        const auto index = Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex);
        if (index != -1)
        {
            const auto* previous = static_cast<const std::byte*>(manager) + static_cast<std::uint32_t>(enemy) * NC2Layout::RecordSize::AIMilitarySlot + 12;
            // Windows compares the current enemy index here, rather than the military-point index.
            if (Read<std::uint32_t>(previous, NC2Layout::AIMilitaryPointOffset::TargetUniqueId) == Read<std::uint32_t>(target, 8) && Read<std::uint32_t>(previous, NC2Layout::AIMilitaryPointOffset::Target) == Read<std::uint32_t>(target, 0)) return;
            Write<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex, -1);
            AIMilitaryManager_MilitaryPoint_Clear(manager, index);
        }
        Write(manager, NC2Layout::AIMilitaryManagerOffset::TargetedEnemyMilitaryPointIndex, AIMilitaryManager_MilitaryPoint_Add(manager, 2, 1, target, 60, 3, 3));
    }
    // Original function: FUN_00455da5; the Windows branch handles an animal target (type 3).
    void AIMilitaryManager_Message_Soldier_Attacked(void* manager, const void* target)
    {
        if (TargetData_GetTargetType(target) != 3 || Read<std::int32_t>(manager, NC2Layout::AIMilitaryManagerOffset::AttackedPositionMilitaryPointIndex) != -1) return;
        auto* position = static_cast<std::byte*>(manager) + NC2Layout::AIMilitaryManagerOffset::AttackedPosition;
        TargetData_GetMapPosition(target, position);
        std::byte mapTarget[12]{};
        Write<std::uint32_t>(mapTarget, 0, 5);
        Write(mapTarget, 4, Read<std::uint16_t>(position, 0)); Write(mapTarget, 6, Read<std::uint16_t>(position, 2));
        Write(manager, NC2Layout::AIMilitaryManagerOffset::AttackedPositionMilitaryPointIndex, AIMilitaryManager_MilitaryPoint_Add(manager, 2, 2, mapTarget, 30, 1, 2));
    }
    // Original function: FUN_0045522f.
    void AIMilitaryManager_l_MilitaryPoints_Tower_Refresh(void* manager, const void* high)
    {
        for (std::uint32_t village = 0; village < 8; ++village)
        {
            const auto* entry = static_cast<const std::byte*>(high) + village * 0xCF0;
            if (Read<std::uint8_t>(entry, 0x24) == 0) continue;
            std::byte target[12]{};
            for (std::uint32_t i = 0; i < Read<std::uint32_t>(entry, 0x104); ++i)
            {
                const auto* micro = GameMap_GetConstMapMicroPointPtr(entry + 0x108 + i * 4);
                const auto houseId = Read<std::uint8_t>(micro, 0) == 2 ? static_cast<std::int32_t>(Read<std::uint16_t>(micro, 4)) : -1;
                const auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseId) * CHouseArray::ElementSize;
                if (House_GetHouseMainType(house) != 5) continue;
                Write<std::uint32_t>(target, 0, 4);
                Write(target, 4, houseId);
                Write(target, 8, Read<std::uint32_t>(house, NC2Layout::HouseOffset::UniqueId));
                AIMilitaryManager_MilitaryPoint_Add(manager, 1, 0, target, 30, 2, 3);
            }
        }
    }
    // Original function: FUN_004293fc.
    void __fastcall AIHighLevelHandler_Message_Soldier_Attacked(void* handler, void* /*unusedEdx*/, const void* target)
    {
        AIMilitaryManager_Message_Soldier_Attacked(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::Military, target);
    }
}
