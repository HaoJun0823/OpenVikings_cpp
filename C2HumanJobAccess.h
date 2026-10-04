#pragma once

#include <cstdint>

namespace NC2Logic
{
    std::uint32_t Human_GetPlayerType(const void* human);
    bool Human_IsProducingVehicle(const void* human);
    bool Human_IsAgeBaby(const void* human);
    bool Human_IsJobBuilder(const void* human);
    bool Human_IsJobScout(const void* human);
    bool Human_IsJobHunter(const void* human);
    bool Human_IsJobBreeder(const void* human);
    bool Human_IsJobTrader(const void* human);
    bool Human_IsJobCarrier(const void* human);
    bool Human_IsJobFisher(const void* human);
    bool Human_CanEquipShoes(const void* human);
    bool Human_CanEquipToolIron(const void* human);
    bool Human_DoesExperienceAllowJobChange(const void* human, std::uint32_t job);
    bool Human_DoesExperienceAllowGoodProduction(const void* human, std::uint32_t good);
    bool Human_DoesExperienceAllowHouseBuilding(const void* human, std::uint32_t house);
    bool Human_DoesExperienceAllowEquipment(const void* human, std::uint32_t good);
    bool Human_DoesEducationAllowJobChange(const void* human, std::uint32_t job);
    bool Human_DoesEducationAllowGoodProduction(const void* human, std::uint32_t good);
    bool Human_IsCapableToProduceGood(const void* human, std::uint32_t good);
    bool Human_IsAbleToProduceGoodNow(const void* human, std::uint32_t good);
    std::uint32_t Human_GetHumanAttributeMaximum_Hitpoints(const void* human);
    void Human_Equipment_SetShoeType(void* human, std::uint32_t good);
    void Human_SetupCurrentJobType(void* human);
    void Human_SetJobType(void* human, std::uint32_t job);
    void Human_SetupPlayerSpecificStuff(void* human);
    void Human_Production_Setup(void* human);
    void Human_Production_Update(void* human);
    std::uint32_t Human_Production_GetCounter(const void* human, std::uint32_t good);
    void Human_ChangeProducedGoodType(void* human, std::uint32_t good);
    bool House_Production_IsAbleToProduceGood(const void* house, std::uint32_t good, bool primary);

    // Operations shared with human initialization and command handling.
    std::uint32_t Human_DoExecuteUserCommand_ChangeProducedGood(void* human, std::uint32_t good);
    bool Human_IsAttachedToLearnHouse(const void* human);
    void Human_WorkCenterMarker_Update(void* human);
    void Human_l_AssignWorkHouse(void* human, std::uint32_t house);
    std::int32_t HumanMerchantArray_AllocateMerchant(void* manager, std::uint32_t human);
    void HumanMerchantArray_FreeMerchant(void* manager, std::int32_t id);
}
