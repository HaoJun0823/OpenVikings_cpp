#pragma once
#include <cstdint>
namespace NC2Logic
{
    std::uint32_t House_Stock_GetMaximumAmountByStockId(const void* house, std::uint32_t stock);
    void House_Stock_SetMinimumValue(void* house, std::uint32_t good, std::uint32_t minimum);
    bool House_HomeQualityEnhancer_SetAllowedFlag(void* house, std::uint32_t enhancer, bool allowed);
    bool House_HomeQualityEnhancer_ModifyValue(void* house, std::uint32_t enhancer, std::int32_t direction);
    void House_Production_SetStoppedFlag(void* house, bool stopped);
    void House_l_Production_CheckProductionCounter(void* house);
    void House_Production_ChangeCounter(void* house, std::int32_t delta);
    void House_SetDefenceModeFlag(void* house, bool enabled);
    void House_DoNewUserCommand(void* house, const void* command);
    struct SStaticHouseTypeData;
    const SStaticHouseTypeData* House_GetHouseTypeDataPtr(const void* house);
    std::uint32_t House_GetHouseMainType(const void* house);
    std::int32_t House_Construction_GetPercentage(const void* house);
    std::uint32_t House_Stock_GetAmountOfGoods(const void* house, std::uint32_t goodId, bool stockOnly);
    float House_Stock_GetAmountOfGoodX(const void* house, std::uint32_t goodId, bool includeFutureStock);
    bool House_Stock_CanStoreGoodType(const void* house, std::uint32_t goodId, bool futureStock);
    bool House_Stock_ModifyFutureAmount(void* house, std::uint32_t goodId, std::int32_t delta, bool modifyProduction);
    std::int32_t House_l_Stock_GetStockSlotIdForGoodType(const void* house, std::uint32_t goodId);
    void* House_Misc_WalkBlockArea_GetPtr(const void* house);
}
