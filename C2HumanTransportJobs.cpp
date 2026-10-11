#include "C2HumanTransportJobs.h"
#include "C2Debug.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanMessageAccess.h"
#include "C2HumanProduction.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MissionStartObjects.h"
#include "C2UserMessageAccess.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* Vehicle(const void* human) { return CVehicleArray::mStaticVars + Read<std::uint32_t>(human, 0xB48) * CVehicleArray::ElementSize; }
        void* WorkHouse(const void* human) { return CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize; }
        bool IsUsable(const void* vehicle) { const auto type = Read<std::uint32_t>(vehicle, 0x1E8); return (type != 3 && type != 4) || Read<std::uint8_t>(vehicle, 0x1D0) != 0; }
        void TargetVehicle(void* human) { Write<std::uint32_t>(human, 0xA68, 6); Write(human, 0xA7C, Read<std::uint32_t>(human, 0xB48)); Write(human, 0xA80, Read<std::uint32_t>(human, 0xB4C)); }
        void TargetHouse(void* human, const void* house) { Write<std::uint32_t>(human, 0xA68, 5); Write(human, 0xA7C, Read<std::uint32_t>(house, 4)); Write(human, 0xA80, Read<std::uint32_t>(house, 8)); }
        void Start(void* human, std::uint32_t animation) { Human_l_StartAtomicAnimation(human, animation, 6); Write<std::uint32_t>(human, 0x36C, 1); }
        std::uint32_t PickupAnimation(const void* house) { const auto type = Read<std::uint32_t>(house, 0x14); return type == 10 ? 44u : type == 11 ? 45u : 22u; }
        bool WalkToConsumer(void* human) { std::uint32_t real = 0, work = 0; return Human_FindConsumerNearWorkCenter(human, Read<std::uint32_t>(human, 0xAD4), &real, &work) && Human_l_StartAtomicWalkAndBring(human, &real, &work); }
        bool WalkToPile(void* human, const void* source) { std::uint32_t real = 0, work = 0; return Human_FindPilePointNearPosition(human, Read<std::uint32_t>(human, 0xAD4), source, &real, &work) && Human_l_StartAtomicWalk(human, &real, &work); }
        bool SameContinent(const void* human, const void* point) { return Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(point), 6) == Read<std::uint8_t>(GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human)), 6); }
    }

    // Original function: FUN_00445e98.
    void Human_l_StartTask_MoveInsideVehicle(void* human)
    {
        auto* vehicle = Vehicle(human);
        if (Read<std::uint32_t>(vehicle, 8) != Read<std::uint32_t>(human, 0xB4C))
        {
            Human_DetachFromVehicle(human);
            Write<std::uint32_t>(human, 0x36C, 3);
            return;
        }
        const auto cargo = Read<std::uint32_t>(human, 0xAD4);
        if (cargo != 0 && Vehicle_Stock_CanStoreGoodType(vehicle, cargo) && Vehicle_Stock_IsFull(vehicle))
        {
            Human_StopCurrentGoodCirculationMode(human);
            Write<std::uint32_t>(human, 0xAD0, 0);
            Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human);
            if (Read<std::uint32_t>(human, 0x36C) != 3) Human_DoAddNewUserCommand(human, 40, static_cast<std::uint8_t>(Read<std::uint32_t>(human, 0x10)), Read<std::uint32_t>(human, 4), Read<std::uint32_t>(human, 8), Read<std::uint32_t>(human, 0xB48), Read<std::uint32_t>(human, 0xB4C));
            return;
        }
        std::uint32_t door = 0;
        if (!IsUsable(vehicle))
        {
            std::byte message[0x304];
            ToUserMessage_Construct(message);
            Write(message, 0x10, Read<std::uint32_t>(human, 4));
            Write(message, 8, Read<std::uint32_t>(human, 0x10));
            Write<std::uint32_t>(message, 0, 43);
            Write(message, 0x2E0, Read<std::uint32_t>(vehicle, 0x1E8));
            ToUserMessage_FinishMessage(message);
            NC2LogicExternals::External_Misc_SendToUserMessage(message);
            Write<std::uint32_t>(human, 0x36C, 3);
            Write<std::uint32_t>(human, 0x374, 1);
            return;
        }
        Vehicle_Door_GetEntryPoint(vehicle, &door, 0);
        if (!SameContinent(human, &door)) { Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 1); return; }
        if (Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == door)
        {
            if (!Human_VehicleMisc_Enter(human)) { Write<std::uint32_t>(human, 0x36C, 3); return; }
            const auto circulation = Read<std::uint32_t>(human, 0xACC);
            const bool bringing = Human_IsBringingCarriedGood(human);
            if (!bringing && circulation != 5 && circulation != 2) { Write<std::uint32_t>(human, 0x36C, 2); return; }
            if (!Human_l_StartAtomicAnimation(human, bringing ? 23 : 22, -1)) { Write<std::uint32_t>(human, 0x36C, 3); return; }
            TargetVehicle(human);
            Write<std::uint32_t>(human, 0x368, 8);
            Write<std::uint32_t>(human, 0x36C, 1);
        }
        else if (Human_l_StartAtomicWalk(human, Vehicle_GetMapPositionRef(vehicle), &door)) TargetVehicle(human);
        else Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_0044611b.
    void Human_l_FinishTask_MoveInsideVehicle(void* human)
    {
        if (Read<std::uint32_t>(human, 0xACC) == 0) Human_Target_Reset(human);
        Write<std::uint32_t>(human, 0x36C, 0);
        Write<std::uint32_t>(human, 0x368, 8);
    }

    // Original function: FUN_0046d3bd.
    void Human_l_StartTask_ExecuteJob_Carrier(void* human)
    {
        if (Human_IsAttachedToVehicle(human))
        {
            auto* vehicle = Vehicle(human);
            if (IsUsable(vehicle))
            {
                if ((Vehicle_IsPriorityCommandInQueue(vehicle) || Read<std::uint32_t>(vehicle, 0x4FC) == 1) && Read<std::uint8_t>(human, 0xB51) != 0)
                {
                    Write<std::uint32_t>(human, 0x368, 1);
                    Write<std::uint32_t>(human, 0x36C, 0);
                    return;
                }
                std::uint32_t door = 0;
                Vehicle_Door_GetEntryPoint(vehicle, &door, 0);
                if (Read<std::uint8_t>(human, 0xB51) != 0 || SameContinent(human, &door))
                {
                    const auto circulation = Read<std::uint32_t>(human, 0xACC);
                    if (circulation == 2) { Human_l_StartTask_ExecuteJob_Carrier_Collect(human); return; }
                    if (circulation == 5)
                    {
                        const bool started = Read<std::uint32_t>(human, 0xAD4) == 0 ? Human_l_StartTask_ExecuteJob_Carrier_FlushVehicle(human) : Human_l_StartTask_ExecuteJob_Carrier_Flush_Bring(human);
                        if (!started) { Write<std::uint32_t>(human, 0xACC, 0); Write<std::uint32_t>(human, 0xAD0, 0); Write<std::uint32_t>(human, 0x36C, 3); Write<std::uint32_t>(human, 0x374, 2); }
                        return;
                    }
                    if (circulation == 4)
                    {
                        const auto cargo = Read<std::uint32_t>(human, 0xAD4);
                        if (Human_IsAtTargetPoint(human))
                        {
                            if (Read<std::uint32_t>(human, 0xA68) == 6)
                            {
                                if (Vehicle_Stock_CanStoreGoodType(vehicle, cargo) && Vehicle_Stock_GetAmountOfWantedGoods(vehicle, cargo) >= Vehicle_Stock_GetFutureAmount(vehicle, cargo) && !Vehicle_Stock_IsFull(vehicle))
                                {
                                    TargetVehicle(human);
                                    Write<std::uint32_t>(human, 0x368, 21);
                                    Write<std::uint32_t>(human, 0x36C, 0);
                                }
                                else Write<std::uint32_t>(human, 0x36C, 3);
                                return;
                            }
                        }
                        else if (Vehicle_Stock_CanStoreGoodType(vehicle, cargo) && Vehicle_Stock_GetFutureAmount(vehicle, cargo) < Vehicle_Stock_GetAmountOfWantedGoods(vehicle, cargo) && !Vehicle_Stock_IsFullSoon(vehicle) && Human_l_StartAtomicWalkAndBring(human, Vehicle_GetMapPositionRef(vehicle), &door))
                        {
                            TargetVehicle(human);
                            Vehicle_Stock_ModifyFutureAmount(vehicle, Read<std::uint32_t>(human, 0xAD4), 1, Read<std::uint32_t>(human, 8));
                            return;
                        }
                        Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(human);
                        return;
                    }
                    if (Human_l_StartTask_ExecuteJob_Carrier_Vehicle(human)) return;
                }
            }
        }
        switch (Read<std::uint32_t>(human, 0xACC))
        {
        case 2: Human_l_StartTask_ExecuteJob_Carrier_Collect(human); break;
        case 4: Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(human); break;
        case 5:
            if (Read<std::uint32_t>(human, 0xAD4) == 0) Human_l_StartTask_ExecuteJob_Carrier_FlushStock(human);
            else Human_l_StartTask_ExecuteJob_Tool_FlushStock_BringAway(human);
            break;
        default: Human_l_StartTask_ExecuteJob_Carrier_TryToCollect(human); break;
        }
    }

    // Original function: FUN_0046d66f.
    bool Human_l_StartTask_ExecuteJob_Carrier_Flush_Bring(void* human)
    {
        auto* vehicle = Vehicle(human);
        if (IsUsable(vehicle))
        {
            std::uint32_t door = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &door, 0);
            if (Human_IsAtTargetPoint(human))
            {
                const auto id = Logic_EntryPoint_House_GetOnPosition(static_cast<std::byte*>(human) + 0xA6C);
                if (id != -1)
                {
                    auto* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
                    const auto cargo = Read<std::uint32_t>(human, 0xAD4);
                    if (House_Stock_CanStoreGoodType(house, cargo, false) && House_Stock_HasDemandForGood(house, cargo)) Start(human, 23);
                    else Human_Target_Reset(human);
                    return true;
                }
                if (WalkToConsumer(human)) return true;
                if (Map_GoodTool_CanBePutOnMap(Read<std::uint32_t>(human, 0xAD4), static_cast<std::byte*>(human) + 0xA6C)) { Start(human, 23); return true; }
                if (WalkToPile(human, &door)) return true;
            }
            else
            {
                if ((!Human_IsAttachedToWorkHouse(human) || House_GetHouseMainType(WorkHouse(human)) != 1) && WalkToConsumer(human)) return true;
                if (WalkToPile(human, static_cast<std::byte*>(human) + 0xB30) || WalkToPile(human, &door)) return true;
            }
        }
        Write<std::uint32_t>(human, 0x36C, 3);
        return false;
    }

    // Original function: FUN_0046d8e0; reservation changes precede walking, even if walking fails.
    bool Human_l_StartTask_ExecuteJob_Carrier_FlushVehicle(void* human)
    {
        auto* vehicle = Vehicle(human);
        if (!IsUsable(vehicle)) return false;
        for (std::uint32_t good = 0; good < 66; ++good)
        {
            if (!Vehicle_Stock_CanStoreGoodType(vehicle, good) || Vehicle_Stock_GetAmountOfWantedGoods(vehicle, good) >= Vehicle_Stock_GetFutureAmount(vehicle, good)) continue;
            Vehicle_Stock_ModifyFutureAmount(vehicle, good, -1, Read<std::uint32_t>(human, 8));
            Write(human, 0xAD0, good);
            std::uint32_t door = 0;
            Vehicle_Door_GetEntryPoint(vehicle, &door, 0);
            if (Human_l_StartAtomicWalk(human, Vehicle_GetMapPositionRef(vehicle), &door)) { TargetVehicle(human); return true; }
        }
        return false;
    }

    // Original function: FUN_0046d9ba.
    bool Human_l_StartTask_ExecuteJob_Carrier_Vehicle(void* human)
    {
        auto* vehicle = Vehicle(human);
        if (!IsUsable(vehicle)) return false;
        std::uint32_t door = 0, real = 0, work = 0, good = 0;
        Vehicle_Door_GetEntryPoint(vehicle, &door, 0);
        if (Human_FindGoodsDemandedForVehicle(human, &real, &work, &good, &door))
        {
            Write(human, 0xAD0, good);
            if (Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return true;
        }
        for (good = 0; good < 66; ++good)
        {
            if (!Vehicle_Stock_CanStoreGoodType(vehicle, good) || Vehicle_Stock_GetAmountOfWantedGoods(vehicle, good) >= Vehicle_Stock_GetFutureAmount(vehicle, good)) continue;
            Write<std::uint32_t>(human, 0x36C, 0);
            Write<std::uint32_t>(human, 0xACC, 5);
            Human_Target_Reset(human);
            return true;
        }
        return false;
    }

    // Original function: FUN_0046daa8; no replacement-good search on failed pickup.
    void Human_l_StartTask_ExecuteJob_Carrier_Collect(void* human)
    {
        if (!Human_IsAtTargetPoint(human)) { Human_StopCurrentGoodCirculationMode(human); Human_l_StartTask_ExecuteJob_Carrier_TryToCollect(human); return; }
        auto* house = Human_GetHousePtrAtTargetPosition(human);
        if (house != nullptr)
        {
            if (Read<std::uint32_t>(human, 0xA70) != Read<std::uint32_t>(house, 0x24)) C2Debug_WarningBox("standing at invalid position for interaction with house stock!");
            const auto good = Read<std::uint32_t>(human, 0xAD0);
            if (House_Stock_CanStoreGoodType(house, good, true) && House_Stock_RequestAmountOfGoods(house, good, true) > 0) { Start(human, PickupAnimation(house)); return; }
        }
        else if (Human_GetLandscapeTypeAtTargetPosition(human) == Read<std::uint32_t>(human, 0xA74)) { Start(human, 22); return; }
        Human_Target_Reset(human);
        Human_StopCurrentGoodCirculationMode(human);
    }

    // Original function: FUN_0046db75.
    void Human_l_StartTask_ExecuteJob_Carrier_TryToCollect(void* human)
    {
        Human_Target_Reset(human);
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_Tool_DropCarriedGoodNearby_Forget(human); return; }
        Write<std::uint32_t>(human, 0x36C, 3);
        Write<std::uint32_t>(human, 0x374, 2);
        if (Human_IsAttachedToWorkHouse(human))
        {
            std::uint32_t real = 0, work = 0, good = 0;
            if (Human_FindGoodsDemandedForStockHouse(human, &real, &work, &good, static_cast<std::byte*>(human) + 0xB30))
            {
                Write(human, 0xAD0, good);
                if (Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) return;
            }
        }
        if (Human_IsAttachedToWorkHouse(human))
        {
            auto* house = WorkHouse(human);
            if (Read<std::uint32_t>(house, 0x2C) == 3 && House_GetHouseMainType(house) == 3)
            {
                std::uint32_t fullest = 0, largest = 0, amount = 0;
                for (std::uint32_t good = 1; good <= 55; ++good)
                {
                    if (!House_Stock_CanStoreGoodType(house, good, true)) continue;
                    if (House_Stock_IsFullSoon(house, good)) fullest = good;
                    else
                    {
                        const auto current = House_Stock_GetAmountOfGoods(house, good, true);
                        if (current > amount) { largest = good; amount = current; }
                    }
                }
                if (fullest != 0)
                {
                    Write<std::uint32_t>(human, 0xACC, 5);
                    if (Human_FindConsumerNearWorkCenter(human, fullest)) { Write(human, 0xAD0, fullest); Human_l_StartTask_ExecuteJob_Carrier_FlushStock(human); return; }
                }
                if (largest != 0)
                {
                    Write<std::uint32_t>(human, 0xACC, 5);
                    if (Human_FindConsumerNearWorkCenter(human, largest)) { Write(human, 0xAD0, largest); Human_l_StartTask_ExecuteJob_Carrier_FlushStock(human); return; }
                }
            }
        }
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_0046dce0.
    void Human_l_StartTask_ExecuteJob_Carrier_FlushStock(void* human)
    {
        if (Human_IsAttachedToWorkHouse(human))
        {
            auto* house = WorkHouse(human);
            const auto good = Read<std::uint32_t>(human, 0xAD0);
            if (Human_Target_HasTarget(human) && Human_IsAtTargetPoint(human))
            {
                if (Read<std::uint32_t>(human, 0xA68) != 5) Start(human, 22);
                else if (House_Stock_GetAmountOfGoods(house, good, true) > 0) Start(human, PickupAnimation(house));
                else Human_Target_Reset(human);
                return;
            }
            if (House_Stock_GetAmountOfGoods(house, good, true) > 0)
            {
                auto* work = static_cast<std::byte*>(house) + 0x24;
                auto* real = static_cast<std::byte*>(house) + 0x1C;
                if (Read<std::uint32_t>(Human_GetMapPositionRef(human), 0) == Read<std::uint32_t>(work, 0))
                {
                    Human_Target_SetTargetPositions(human, real, work);
                    TargetHouse(human, house);
                    Start(human, PickupAnimation(house));
                    return;
                }
                if (Human_l_StartAtomicWalk(human, real, work)) { TargetHouse(human, house); return; }
            }
        }
        Write<std::uint32_t>(human, 0x36C, 3);
    }

    // Original function: FUN_004464a9; Windows outlined delivery predicate, name inferred from callers.
    bool Human_IsBringingCarriedGood(const void* human) { const auto mode = Read<std::uint32_t>(human, 0xACC); return mode == 4 || (mode == 5 && Read<std::uint32_t>(human, 0xAD4) != 0); }

}
