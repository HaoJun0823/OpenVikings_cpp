#pragma once
#include <cstdint>
namespace NC2Logic
{
    struct SStaticVehicleTypeData;
    bool Vehicle_DoAddNewUserCommand(void* vehicle, const void* command);
    bool Vehicle_l_UserCommands_TryToExecuteRightAway(void* vehicle, const void* command);
    std::uint32_t Vehicle_Stock_GetTotalWantedAmount(const void* vehicle);
    bool Vehicle_Stock_SetWantedAmount(void* vehicle, std::uint32_t good, std::uint32_t wanted);
    void* Vehicle_Passengers_GetAttachedHumanPtr(const void* vehicle, std::uint32_t slot);
    void* Vehicle_Passengers_GetAttachedCommanderPtr(const void* vehicle);
    bool Vehicle_Passengers_IsCarrierAttached(const void* vehicle);
    void Vehicle_l_Passengers_StopAllIdleTasks(void* vehicle);
    const SStaticVehicleTypeData* Vehicle_GetVehicleTypeDataPtr(const void* vehicle);
    const void* Vehicle_GetConstMapMicroPointPtr(const void* vehicle);
    const void* Vehicle_GetMapPositionRef(const void* vehicle);
    const std::uint16_t* Human_GetMapPositionRef(const void* human);
    std::uint32_t Vehicle_Stock_GetAmountOfGoods(const void* vehicle, std::uint32_t goodId);
    bool Vehicle_Stock_CanStoreGoodType(const void* vehicle, std::uint32_t goodId);
    std::int32_t Vehicle_l_Stock_GetStockSlotIdForGoodType(const void* vehicle, std::uint32_t goodId);
    std::int32_t Vehicle_Passengers_GetAttachedHumanSlot(const void* vehicle, std::uint32_t humanId);
    std::int32_t Vehicle_Passengers_GetAttachedCommanderId(const void* vehicle);
    std::int32_t Vehicle_Passengers_GetAttachedVehicleSlot(const void* vehicle, std::uint32_t vehicleId);
}
