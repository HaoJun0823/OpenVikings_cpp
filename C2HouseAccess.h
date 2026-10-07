#pragma once
#include <cstdint>
namespace NC2Logic
{
    std::uint32_t House_Stock_GetMaximumAmountByStockId(const void* house, std::uint32_t stock);
    void House_Stock_SetMinimumValue(void* house, std::uint32_t good, std::uint32_t minimum);
    bool House_HomeQualityEnhancer_SetAllowedFlag(void* house, std::uint32_t enhancer, bool allowed);
    bool House_HomeQualityEnhancer_ModifyValue(void* house, std::uint32_t enhancer, std::int32_t direction);
    bool House_Religion_GetNextPrayPoint(const void* house, const void* source, void* point, bool inTemple, std::uint32_t humanId, std::uint32_t continent);
    void House_Production_SetStoppedFlag(void* house, bool stopped);
    void House_l_Production_CheckProductionCounter(void* house);
    void House_Production_ChangeCounter(void* house, std::int32_t delta);
    void House_Production_DidProduceOneGood(void* house);
    void House_ActionOverlay_Start(void* house, std::int32_t overlay, std::uint32_t duration);
    bool House_Construction_GetNextWorkPoint(const void* house, const void* source, void* point, bool avoidOtherBuilders, std::uint32_t humanId, std::uint32_t continent);
    void House_SetDefenceModeFlag(void* house, bool enabled);
    void House_DoNewUserCommand(void* house, const void* command);
    struct SStaticHouseTypeData;
    const SStaticHouseTypeData* House_GetHouseTypeDataPtr(const void* house);
    std::uint32_t House_GetHouseMainType(const void* house);
    std::int32_t House_Construction_GetPercentage(const void* house);
    std::uint32_t House_Stock_GetAmountOfGoods(const void* house, std::uint32_t goodId, bool stockOnly);
    std::uint32_t House_Stock_RequestAmountOfGoods(void* house, std::uint32_t goodId, bool stockOnly);
    bool House_Stock_ModifyAmount(void* house, std::uint32_t good, std::int32_t amount, std::int32_t fraction, bool construction);
    bool House_l_Construction_PutGoodIntoStock(void* house, std::uint32_t good);
    bool House_Construction_IsNeedingGood(const void* house, std::uint32_t good);
    bool House_Stock_IsFull(const void* house, std::uint32_t good);
    bool House_Stock_IsFullSoon(const void* house, std::uint32_t good);
    bool House_Construction_IsNeedingFutureGood(const void* house, std::uint32_t good);
    std::uint32_t House_Stock_GetDemandForGood(const void* house, std::uint32_t good);
    bool House_Stock_HasDemandForGood(const void* house, std::uint32_t good);
    float House_Stock_GetAmountOfGoodX(const void* house, std::uint32_t goodId, bool includeFutureStock);
    bool House_Stock_CanStoreGoodType(const void* house, std::uint32_t goodId, bool futureStock);
    bool House_Stock_ModifyFutureAmount(void* house, std::uint32_t goodId, std::int32_t delta, bool modifyProduction);
    std::int32_t House_l_Stock_GetStockSlotIdForGoodType(const void* house, std::uint32_t goodId);
    void* House_Misc_WalkBlockArea_GetPtr(const void* house);
    void House_Misc_Blocks_SetToMap(const void* house);
}
