#include "C2HouseAccess.h"
#include "C2AnimalAccess.h"
#include "C2DelayedActionManager.h"
#include "C2EngineDisplayAccess.h"
#include "C2HappinessEvents.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanInitialization.h"
#include "C2HumanIterator.h"
#include "C2HumanJobAccess.h"
#include "C2HumanNeeds.h"
#include "C2HumanTaskAccess.h"
#include "C2HumanWalking.h"
#include "C2GameMapAccess.h"
#include "C2GameState.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "C2MissionStartObjects.h"
#include "C2MilitaryTargetSearch.h"
#include "C2StaticActorTypeDataAccess.h"
#include "C2StaticHouseTypeData.h"
#include "C2VehicleAccess.h"
#include "XCrtFileAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;

    void __fastcall House_Misc_Destroy_House(void* house, void* unusedEdx);

    // Original function: FUN_0040ace5.
    void __fastcall HouseArray_DoOneGameTick(void* array, void*)
    {
        auto* handler = static_cast<CArrayHandler*>(array);
        for (auto id = handler->mFirstScanIndex; id <= handler->mLastScanIndex; ++id)
        {
            void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) != 0) House_DoOneGameTick(house);
        }
    }

    // Original function: FUN_00433694; target searches also run when the current tick has no arrows available.
    void House_DoOneGameTick(void* house)
    {
        if (Read<std::uint8_t>(house, 0x7D0) == 0 && Read<std::int32_t>(house, 0x7C8) <= 0)
        {
            HappinessManager_Messages_HouseDestroyedTroughAttack(gHappinessManager, house);
            House_Misc_Destroy_House(house, nullptr);
        }
        if (Read<std::uint8_t>(house, 0x7D0) != 0)
        {
            const auto tick = static_cast<std::uint16_t>(Read<std::uint16_t>(house, 0x7D2) + 1u);
            Write(house, 0x7D2, tick);
            if (static_cast<std::int16_t>(tick) == Read<std::int16_t>(house, 0x7D4) - 12) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x1D, true, Read<std::uint32_t>(house, 4), 0, 0);
            if (static_cast<std::int16_t>(tick) >= Read<std::int16_t>(house, 0x7D4)) { CHouseArray::sTheObjectPtr->FreeHouse(Read<std::int32_t>(house, 4)); return; }
        }
        if (House_GetHouseMainType(house) == 6)
        {
            if (Read<std::uint32_t>(house, 0x2C) == 3)
            {
                // Windows frees the site first and reads its retained fields for vehicle allocation.
                CHouseArray::sTheObjectPtr->FreeHouse(Read<std::int32_t>(house, 4));
                Logic_EntryPoint_Add_Vehicle(Read<std::uint32_t>(house, 0xC), Read<std::uint32_t>(house, 0x18), gStaticHouseTypeStorage.records[Read<std::uint32_t>(house, 0x14)].vehicleType, static_cast<const std::byte*>(house) + 0x1C, 0);
                return;
            }
        }
        else if (Read<std::uint32_t>(house, 0x2C) == 3) Tool_Map_CheckUserInformationAboutEnemies(Read<std::uint32_t>(house, 0xC), static_cast<const std::byte*>(house) + 0x1C);
        if (Read<std::uint8_t>(house, 0x7D8) == 0 || Read<std::uint32_t>(house, 0x2C) != 3 || Read<std::uint8_t>(house, 0x7D0) != 0) return;
        CHumanIterator iterator;
        std::uint32_t defenders = 0;
        const auto* door = static_cast<const std::byte*>(house) + 0x24;
        const auto player = Read<std::uint32_t>(house, 0xC);
        for (auto id = iterator.GetFirstSectorHumanId(player, Read<std::uint32_t>(door, 0), 0, false); id != -1; id = iterator.GetNextSectorHumanId()) ++defenders;
        auto remaining = defenders / 24u;
        if (defenders % 24u > gGameTime.tick % 24u) ++remaining;
        const auto& weapon = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(house, 0x18)])[20];
        bool fired;
        do
        {
            fired = false;
            const auto humanFirst = Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, door, weapon.minimumRange, weapon.maximumRange, true, 5, 10, true, true);
            if (humanFirst != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanFirst) * CHumanArray::ElementSize), 0)); --remaining; fired = true; }
            const auto vehicleFirst = Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, door, weapon.minimumRange, weapon.maximumRange, true, 5, 10, false, false);
            if (vehicleFirst != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(vehicleFirst) * CVehicleArray::ElementSize), 0)); --remaining; fired = true; }
            const auto animal = Tool_Animals_GetOneOfNextEnemyInRange(player, door, weapon.minimumRange, weapon.maximumRange, true, false, 5, 10);
            if (animal != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(Animal_GetMapPositionRef(CAnimalArray::mStaticVars + static_cast<std::uint32_t>(animal) * CAnimalArray::ElementSize), 0)); --remaining; fired = true; }
            const auto houseFirst = Tool_GetOneOfNextEnemyHousesInRange(player, door, weapon.minimumRange, weapon.maximumRange, true, 5, 10);
            if (houseFirst != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseFirst) * CHouseArray::ElementSize, 0x1C)); --remaining; fired = true; }
            const auto humanSecond = Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, door, weapon.minimumRange, weapon.maximumRange, false, 5, 10, true, true);
            if (humanSecond != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(Human_GetMapPositionRef(CHumanArray::mStaticVars + static_cast<std::uint32_t>(humanSecond) * CHumanArray::ElementSize), 0)); --remaining; fired = true; }
            const auto vehicleSecond = Tool_ObjectId_GetOneOfNextEnemyMapMoveableInRange(player, door, weapon.minimumRange, weapon.maximumRange, false, 5, 10, false, false);
            if (vehicleSecond != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(Vehicle_GetMapPositionRef(CVehicleArray::mStaticVars + static_cast<std::uint32_t>(vehicleSecond) * CVehicleArray::ElementSize), 0)); --remaining; fired = true; }
            const auto houseSecond = Tool_GetOneOfNextEnemyHousesInRange(player, door, weapon.minimumRange, weapon.maximumRange, false, 5, 10);
            if (houseSecond != -1 && remaining != 0) { House_l_Military_StartArrow(house, Read<std::uint32_t>(CHouseArray::mStaticVars + static_cast<std::uint32_t>(houseSecond) * CHouseArray::ElementSize, 0x1C)); --remaining; fired = true; }
        } while (fired && remaining != 0);
    }

    // Original function: FUN_00433a9d; the target is passed by value and scatter consumes y before x.
    void House_l_Military_StartArrow(const void* house, std::uint32_t point)
    {
        const auto& weapon = static_cast<const SStaticWeaponTypeData*>(gStaticWeaponTypeArrays[Read<std::uint32_t>(house, 0x18)])[20];
        const auto source = Read<std::uint32_t>(house, 0x1C);
        const auto* graphics = NC2E2::Engine2d_EntryPoint_GetHouseDataPtr(Read<std::uint32_t>(house, 0x10));
        std::uint32_t height = 0, offsetX = 20, offsetY = 20;
        const auto launchPoints = Read<std::uint32_t>(graphics, 0x1B0);
        if (launchPoints != 0)
        {
            const auto selected = Game_Random_GetNumber() % launchPoints;
            height = Read<std::uint32_t>(graphics, 0x1BCu + selected * 12u);
            offsetX = Read<std::uint32_t>(graphics, 0x1B4u + selected * 12u);
            offsetY = Read<std::uint32_t>(graphics, 0x1B8u + selected * 12u);
        }
        const auto divisor = Game_Random_GetNumber() % 30u + 1u;
        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(&source, 0), Read<std::int16_t>(&source, 2)));
        const auto spread = (distance >> 2) / divisor;
        const auto scatterDivisor = NXBasics::Runtime_TimeSigned32(spread + 1u), half = NXBasics::Runtime_TimeSigned32(spread) / 2;
        const auto y = static_cast<std::int16_t>(Game_Random_GetNumber()) % scatterDivisor - half;
        const auto x = static_cast<std::int16_t>(Game_Random_GetNumber()) % scatterDivisor - half;
        MapMigPoint_AddOffset(&point, static_cast<std::int16_t>(x), static_cast<std::int16_t>(y));
        const auto flightTicks = (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(&point, 0), Read<std::int16_t>(&point, 2), Read<std::int16_t>(&source, 0), Read<std::int16_t>(&source, 2))) << 3) / static_cast<std::uint32_t>(weapon.speed);
        // Complete native 0x24-byte stack callback record; its original type name remains unresolved.
        const std::uint32_t arrow[9] = {source, point, height, offsetX, offsetY, 20, 20, static_cast<std::uint32_t>(weapon.munitionType), flightTicks};
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x23, true, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(arrow)), 0, 0);
        DelayedActionManager_DelayedAction_AddWeaponHit(gDelayedActionManager, flightTicks, 4, Read<std::uint32_t>(house, 4), &point, 20);
    }

    // Original function: FUN_0040ad1d; the upper occupied-range bound is read each iteration.
    void __fastcall HouseArray_DoOneGameSecond(void* array, void*)
    {
        auto* handler = static_cast<CArrayHandler*>(array);
        for (auto id = handler->mFirstScanIndex; id <= handler->mLastScanIndex; ++id)
        {
            void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            if (Read<std::uint8_t>(house, 0) != 0) House_DoOneGameSecond(house);
        }
    }

    // Original function: FUN_00433337; the native radius query includes both religion and hitpoint effects.
    void House_Religion_Send(const void* house)
    {
        CHumanIterator iterator;
        for (auto id = iterator.GetFirstHumanId(Read<std::uint32_t>(house, 0x0C), static_cast<const std::byte*>(house) + 0x1C, 50); id != -1; id = iterator.GetNextHumanId())
        {
            void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (Read<std::int32_t>(human, 0xAB4) <= 8000) Write(human, 0xAB4, Read<std::uint32_t>(human, 0xAB4) + 250u);
            Write(human, 0xAA0, Read<std::uint32_t>(human, 0xAA0) + 50u);
        }
    }

    // Original function: FUN_00433bf9; builder recruitment, stock upkeep and civilian defence stay in native order.
    void House_DoOneGameSecond(void* house)
    {
        if (Read<std::uint8_t>(house, 0x7D0) != 0) return;
        if (House_GetHouseMainType(house) != 6 && (Read<std::uint32_t>(house, 0x2C) != 3 || !House_Repair_IsFinished(house)))
        {
            const auto builders = House_Inhabitants_GetNumberOfAttachedHumans(house, 4);
            const auto maximum = House_Construction_GetTotalNumberOfBuildersAllowed(house);
            if (NXBasics::Runtime_TimeSigned32(builders) >= maximum) Write<std::uint32_t>(house, 0x114, 0);
            else
            {
                const auto wait = Read<std::uint32_t>(house, 0x114) + 1u;
                Write(house, 0x114, wait);
                if (wait >= 5)
                {
                    Write<std::uint32_t>(house, 0x114, 5);
                    CHumanIterator iterator;
                    auto* base = static_cast<std::byte*>(house) + 0x1C;
                    const auto continent = Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(base), 6);
                    void* selected = nullptr;
                    std::uint32_t bestDistance = 20000;
                    for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(house, 0x0C), Read<std::uint32_t>(house, 0x24), 10000, false); id != -1; id = iterator.GetNextSectorHumanId())
                    {
                        void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
                        if (Read<std::uint32_t>(human, 0x18) != 7 || Human_UserControl_GetNextJobType(human) != 7 || Read<std::uint32_t>(human, 0xACC) != 0 || Human_IsAttachedToBuildSite(human) || Human_IsAttachedToVehicle(human)) continue;
                        const void* position = Human_GetMapPositionRef(human);
                        if (Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(position), 6) != continent) continue;
                        const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2), Read<std::int16_t>(base, 0), Read<std::int16_t>(base, 2)));
                        if (selected != nullptr && distance >= bestDistance) continue;
                        selected = human;
                        bestDistance = distance;
                        if (distance < 15) break;
                    }
                    if (selected != nullptr && House_Inhabitants_AttachHuman(house, Read<std::uint32_t>(selected, 4), 4))
                    {
                        Human_l_AssignWorkHouse(selected, Read<std::uint32_t>(house, 4));
                        Write<std::uint32_t>(selected, 0x38C, 0);
                        Write(selected, 0xB30, Read<std::uint32_t>(house, 0x24));
                        Human_WorkCenterMarker_Update(selected);
                    }
                }
            }
        }
        else
        {
            const auto type = Read<std::uint32_t>(house, 0x14);
            if (type == 37) House_Religion_Send(house);
            else if (type == 11)
            {
                if (!House_Stock_IsFull(house, 12)) House_Stock_ModifyAmount(house, 12, 1, 0, false);
            }
            else if (type == 10)
            {
                if (!House_Stock_IsFull(house, 1)) House_Stock_ModifyAmount(house, 1, 1, 0, false);
            }
            if (Read<std::uint32_t>(house, 0x14) != 1 && Read<std::uint32_t>(house, 0x14) != 37) House_HomeQualityEnhancer_ModifyValue(house, 2, -1);
        }
        for (std::uint32_t index = 0; index < 66; ++index)
        {
            const auto stock = Read<std::uint32_t>(house, 0x118u + index * 4u);
            const auto recent = Read<std::uint32_t>(house, 0x328u + index * 4u);
            if (stock > recent) Write(house, 0x328u + index * 4u, recent + 1u);
            else if (stock < recent) Write(house, 0x328u + index * 4u, recent - 1u);
        }
        for (std::uint32_t index = 0; index < 66; ++index)
        {
            const auto offset = 0x430u + index * 4u;
            if (Read<std::int32_t>(house, offset) > 0) Write(house, offset, Read<std::uint32_t>(house, offset) - 1u);
        }
        House_l_ActionOverlay_Update(house);
        if (Read<std::uint8_t>(house, 0x7D8) == 0) return;
        CHumanIterator iterator;
        const auto type = Read<std::uint32_t>(house, 0x14);
        const std::uint32_t maximum = type == 1 ? 30u : type == 41 ? 20u : 10u;
        std::uint32_t count = 0;
        const auto* door = static_cast<const std::byte*>(house) + 0x24;
        auto outside = Read<std::uint32_t>(house, 0x24);
        Tool_Map_GetNotBlockedPosition(&outside, true);
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(house, 0x0C), Read<std::uint32_t>(house, 0x24), 0, false); id != -1; id = iterator.GetNextSectorHumanId())
        {
            if (count != maximum) { ++count; continue; }
            void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            if (Human_IsDefendingHouse(human, Read<std::int32_t>(house, 4))) Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(Read<std::uint32_t>(house, 0x0C)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), MapMigPoint_GetLong(&outside), 0);
        }
        for (auto id = iterator.GetFirstSectorHumanId(Read<std::uint32_t>(house, 0x0C), Read<std::uint32_t>(house, 0x24), 40, false); id != -1 && count < maximum; id = iterator.GetNextSectorHumanId())
        {
            void* human = CHumanArray::mStaticVars + static_cast<std::uint32_t>(id) * CHumanArray::ElementSize;
            const void* position = Human_GetMapPositionRef(human);
            if (Read<std::uint32_t>(position, 0) == Read<std::uint32_t>(door, 0) || Read<std::uint32_t>(human, 0xAD4) != 0) continue;
            if (Human_IsDefendingHouse(human, Read<std::int32_t>(house, 4))) { ++count; continue; }
            if (Human_IsDefendingHouse(human, -1))
            {
                const auto oldId = Human_GetDefenseHouseId(human);
                const void* oldHouse = oldId == -1 ? nullptr : CHouseArray::mStaticVars + static_cast<std::uint32_t>(oldId) * CHouseArray::ElementSize;
                const auto distance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2), Read<std::int16_t>(door, 0), Read<std::int16_t>(door, 2)));
                const auto previousDistance = static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(Read<std::int16_t>(position, 0), Read<std::int16_t>(position, 2), Read<std::int16_t>(oldHouse, 0x24), Read<std::int16_t>(oldHouse, 0x26)));
                if (distance >= previousDistance) continue;
            }
            else if (Human_UserControl_IsExecutingUserCommand(human)) continue;
            Human_DoAddNewUserCommand(human, 4, static_cast<std::uint8_t>(Read<std::uint32_t>(house, 0x0C)), Read<std::uint32_t>(human, 4), 0, MapMigPoint_GetLong(door), 1);
            ++count;
        }
    }

    // Original function: FUN_0043420f; only nonzero counters decrement, then notify on zero.
    void House_l_ActionOverlay_Update(void* house)
    {
        for (std::int32_t overlay = 0; overlay < 5; ++overlay)
        {
            const auto offset = 0x7A8u + static_cast<std::uint32_t>(overlay) * 4u;
            const auto value = Read<std::uint32_t>(house, offset);
            if (value == 0) continue;
            Write(house, offset, value - 1u);
            if (value == 1) House_ActionOverlay_Stop(house, overlay);
        }
    }
}
