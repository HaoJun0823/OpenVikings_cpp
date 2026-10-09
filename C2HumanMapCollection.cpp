#include "C2HumanMapCollection.h"
#include "C2GameMapAccess.h"
#include "C2HouseAccess.h"
#include "C2HumanCommandAccess.h"
#include "C2HumanGoodieTasks.h"
#include "C2HumanGoods.h"
#include "C2HumanInitialization.h"
#include "C2HumanJobAccess.h"
#include "C2HumanLandscapeSearch.h"
#include "C2HumanProduction.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2StaticGoodTypeData.h"
#include "C2VehicleAccess.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    namespace
    {
        void* WorkHouse(const void* human) { return CHouseArray::mStaticVars + Read<std::uint32_t>(human, 0xB34) * CHouseArray::ElementSize; }
        const void* Real(const void* human) { return static_cast<const std::byte*>(human) + 0xA6C; }
        const void* Work(const void* human) { return static_cast<const std::byte*>(human) + 0xA70; }
        const void* Center(const void* human) { return static_cast<const std::byte*>(human) + 0xB30; }
        void Status(void* human, std::uint32_t status) { Write(human, 0x36C, status); }
        void Fail(void* human, std::uint32_t reason) { Status(human, 3); Write(human, 0x374, reason); }
        void Start(void* human, std::uint32_t animation, std::int32_t direction) { Human_l_StartAtomicAnimation(human, animation, direction); Status(human, 1); }
        void Reserve(void* human) { House_Stock_ModifyFutureAmount(WorkHouse(human), Read<std::uint32_t>(human, 0xAD0), 1, true); }
        void TargetCurrentPosition(void* human, std::uint32_t landscape)
        {
            Human_Target_SetTargetPositions(human, Human_GetMapPositionRef(human), Human_GetMapPositionRef(human));
            Write<std::uint32_t>(human, 0xA68, 2);
            Write(human, 0xA74, landscape);
            Write<std::uint32_t>(human, 0xACC, 2);
        }
        bool CanPickupTarget(const void* human, std::uint32_t type, std::uint32_t pickup, std::uint32_t store)
        {
            if (type == pickup) return true;
            if (type != store) return false;
            if (type == Read<std::uint32_t>(human, 0xA74) && !Human_IsAttachedToWorkHouse(human)) return true;
            return Human_IsAttachedToWorkHouse(human) && House_Stock_HasDemandForGood(WorkHouse(human), Read<std::uint32_t>(human, 0xAD0));
        }
    }

    // Original function: FUN_0042d159.
    std::uint32_t Human_GetLandscapeTypeAtActiveMapPosition(const void* human)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
        return Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0u;
    }
    // Original function: FUN_0042d172.
    std::uint32_t Human_GetLandscapeValencyAtActiveMapPosition(const void* human)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(Human_GetMapPositionRef(human));
        return Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0u;
    }
    // Original function: FUN_0042d1a5.
    std::uint32_t Human_GetLandscapeValencyAtTargetPosition(const void* human)
    {
        const auto* micro = GameMap_GetConstMapMicroPointPtr(Real(human));
        return Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 3) : 0u;
    }

    // Original function: FUN_0046c876.
    void Human_l_StartTask_ExecuteJob_Collector(void* human)
    {
        if (Read<std::uint32_t>(human, 0xAD4) != 0) { Human_l_StartTask_ExecuteJob_Tool_FillStock_BringAway(human); return; }
        const auto good = Read<std::uint32_t>(human, 0x1C);
        if (good == 0) { Fail(human, 2); return; }
        if (Human_Production_GetCounter(human, good) == 0) { Human_Production_Rotate(human); return; }
        Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect(human);
    }

    // Original function: FUN_0047603f.
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect(void* human)
    {
        if (Human_Target_HasTarget(human) && Read<std::uint32_t>(human, 0xA68) != 2)
        {
            Human_StopCurrentGoodCirculationMode(human);
            Human_Target_Reset(human);
        }
        auto good = Read<std::uint32_t>(human, 0x1C);
        if (good != 65)
        {
            if (Human_IsAttachedToWorkHouse(human) && House_Stock_IsFull(WorkHouse(human), good)) { Fail(human, 11); return; }
        }
        else if (Read<std::uint32_t>(human, 0xAD0) != 0) good = Read<std::uint32_t>(human, 0xAD0);
        else
        {
            SHumanSearchLandscapesData data;
            std::uint32_t real = 0, work = 0;
            for (std::uint32_t candidate = 1; candidate < 55; ++candidate)
            {
                if (!Human_IsAbleToProduceGoodNow(human, candidate)) continue;
                const auto& type = gStaticGoodTypeStorage.records[candidate];
                if (type.isBioLandscape != 0) data.AddBio(type.landscapeToHarvest);
                else data.Add(type.landscapeToHarvest);
                data.Add(type.landscapeToPickup);
                if (Human_IsAttachedToWorkHouse(human)) data.Add(type.landscapeToStore);
            }
            for (std::uint32_t index = 0; index < data.typeCount; ++index) data.entries[index].enabled = 1;
            bool found = false;
            for (std::uint32_t retry = 0; retry < data.typeCount; ++retry)
            {
                if (!Human_FindLandscapesNearPos_Random(human, data, &real, &work, Center(human))) continue;
                const auto* micro = GameMap_GetConstMapMicroPointPtr(&real);
                good = gStaticDependencyGoodByLandscape[Read<std::uint8_t>(micro, 0) == 1 ? Read<std::uint8_t>(micro, 2) : 0u];
                found = true;
                break;
            }
            if (!found) { Fail(human, 3); return; }
            Write<std::uint32_t>(human, 0x38C, 0);
            Write(human, 0x20, good);
        }
        Write(human, 0xAD0, good);
        const auto& type = gStaticGoodTypeStorage.records[good];
        if (type.isBioLandscape != 0) Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_Bio(human, type.landscapeToHarvest, type.landscapeToPickup, type.landscapeToStore, type.atomicForHarvesting);
        else Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_NonBio(human, type.landscapeToHarvest, type.landscapeToPickup, type.landscapeToStore, type.atomicForHarvesting);
    }

    // Original function: FUN_0047624a.
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_NonBio(void* human, std::uint32_t harvest, std::uint32_t pickup, std::uint32_t store, std::uint32_t animation)
    {
        if (Human_Target_HasTarget(human) && !Human_Target_IsValid(human)) { Human_StopCurrentGoodCirculationMode(human); return; }
        if (Human_IsAtTargetPoint(human))
        {
            const auto type = Human_GetLandscapeTypeAtTargetPosition(human);
            if (harvest != 0 && type == harvest)
            {
                if (Read<std::uint32_t>(human, 0xACC) == 0)
                {
                    Write<std::uint32_t>(human, 0xACC, 2);
                    if (Human_IsAttachedToWorkHouse(human)) Reserve(human);
                }
                Start(human, animation, Human_Target_GetDirectionTo(human));
                return;
            }
            if (CanPickupTarget(human, type, pickup, store)) { Start(human, 22, 6); return; }
            Human_Target_Reset(human);
            Status(human, 3);
            return;
        }
        if (Human_Target_HasTarget(human) && Human_l_StartAtomicWalkAndFetch(human, Real(human), Work(human), false)) return;
        if (harvest != 0 && Human_GetLandscapeTypeAtActiveMapPosition(human) == harvest)
        {
            TargetCurrentPosition(human, harvest);
            if (Human_IsAttachedToWorkHouse(human)) Reserve(human);
            Start(human, animation, -1);
            return;
        }
        SHumanSearchLandscapesData data;
        std::uint32_t real = 0, work = 0;
        if (Human_IsAttachedToWorkHouse(human))
        {
            if (House_Stock_HasDemandForGood(WorkHouse(human), Read<std::uint32_t>(human, 0xAD0)))
            {
                data.Add(store);
                data.Add(pickup);
                if (harvest != 0) data.Add(harvest);
                data.SetCurrent(store);
                if (Human_FindLandscapesNearPos_Sorted(human, data, &real, &work, Center(human)))
                {
                    if (!Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) Status(human, 3);
                    return;
                }
            }
        }
        else
        {
            data.Add(pickup);
            if (harvest != 0) data.Add(harvest);
        }
        data.SetCurrent(pickup);
        if (Human_GetLandscapeTypeAtActiveMapPosition(human) == pickup)
        {
            TargetCurrentPosition(human, pickup);
            if (Human_IsAttachedToWorkHouse(human)) Reserve(human);
            Start(human, 22, 6);
            return;
        }
        if (!Human_FindLandscapesNearPos_Sorted(human, data, &real, &work, Center(human)))
        {
            if (harvest == 0) { Status(human, 3); return; }
            data.SetCurrent(harvest);
            if (!Human_FindLandscapesNearPos_Sorted(human, data, &real, &work, Center(human))) { Fail(human, 3); return; }
        }
        if (!Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) Status(human, 3);
    }

    // Original function: FUN_004765cc.
    void Human_l_StartTask_ExecuteJob_Tool_MapCollector_Collect_Bio(void* human, std::uint32_t harvest, std::uint32_t pickup, std::uint32_t store, std::uint32_t animation)
    {
        if (Human_Target_HasTarget(human) && !Human_Target_IsValid(human))
        {
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            return;
        }
        if (Human_IsAtTargetPoint(human))
        {
            const auto type = Human_GetLandscapeTypeAtTargetPosition(human);
            if (harvest != 0 && type == harvest)
            {
                if (harvest == 4 && Human_GetLandscapeValencyAtTargetPosition(human) < 3)
                {
                    Human_Target_Reset(human);
                    Human_StopCurrentGoodCirculationMode(human);
                    return;
                }
                Start(human, animation, Human_Target_GetDirectionTo(human));
                return;
            }
            if (CanPickupTarget(human, type, pickup, store)) { Start(human, 22, 6); return; }
            Human_Target_Reset(human);
            Human_StopCurrentGoodCirculationMode(human);
            Status(human, 3);
            return;
        }
        if (Human_Target_HasTarget(human) && Human_l_StartAtomicWalkAndFetch(human, Real(human), Work(human), false)) return;
        if (harvest != 0 && Human_GetLandscapeTypeAtActiveMapPosition(human) == harvest && Human_GetLandscapeValencyAtActiveMapPosition(human) >= 3)
        {
            TargetCurrentPosition(human, harvest);
            Reserve(human); // Native current-position bio branch requires a work house.
            Start(human, animation, -1);
            return;
        }
        SHumanSearchLandscapesData data;
        std::uint32_t real = 0, work = 0;
        if (Human_IsAttachedToWorkHouse(human))
        {
            if (House_Stock_HasDemandForGood(WorkHouse(human), Read<std::uint32_t>(human, 0xAD0)))
            {
                if (Human_GetLandscapeTypeAtActiveMapPosition(human) == store)
                {
                    TargetCurrentPosition(human, store);
                    Reserve(human);
                    Start(human, 22, 6);
                    return;
                }
                data.Add(store);
                data.Add(pickup);
                data.SetCurrent(store);
                if (Human_FindLandscapesNearPos_Sorted(human, data, &real, &work, Center(human)))
                {
                    if (!Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) Status(human, 3);
                    return;
                }
            }
        }
        else data.Add(pickup);
        data.SetCurrent(pickup);
        if (Human_GetLandscapeTypeAtActiveMapPosition(human) == pickup)
        {
            TargetCurrentPosition(human, pickup);
            if (Human_IsAttachedToWorkHouse(human)) Reserve(human);
            Start(human, 22, 6);
            return;
        }
        if (!Human_FindLandscapesNearPos_Sorted(human, data, &real, &work, Center(human)))
        {
            if (harvest == 0 || !Human_FindBioLandscapeNearWorkCenter(human, harvest, &real, &work)) { Fail(human, 3); return; }
        }
        if (!Human_l_StartAtomicWalkAndFetch(human, &real, &work, false)) Status(human, 3);
    }
}
