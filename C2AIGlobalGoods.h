#pragma once
#include <cstdint>

namespace NC2Logic
{
    void AIGlobalGoods_Init(void* manager);
    void AIGlobalGoods_DoOneLogicGameTick(void* manager, void* villages);
    std::uint32_t AIGlobalGoods_l_AddProducer(void* manager, std::int32_t village, std::uint32_t good, void* villages);
    std::uint32_t AIGlobalGoods_l_AddImporting(void* manager, std::int32_t village, std::uint32_t good, void* villages);
    std::uint32_t AIGlobalGoods_l_AddNeed(void* manager, std::int32_t village, std::uint32_t good, void* villages);
    bool AIGlobalGoods_IsNeedingVillage(const void* manager, std::uint32_t good);
    std::uint32_t AIVillagesData_GetNumberOfUsedHumans(const void* villages);
    bool AIVillagesData_Good_IsProducingGood(void* villages, std::uint32_t good, bool includeInputs);
    bool AIVillagesData_House_CanBuildAnyFurther(const void* villages, std::uint32_t house);
}
