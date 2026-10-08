#pragma once
#include <cstdint>
namespace NXBasics { class CIniFileReader; }
namespace NC2Logic
{
    bool MerchantData_AddHouse(void* merchant, std::int32_t house, std::uint32_t revision, bool foreign);
    void MerchantData_ClearHouse(void* merchant, std::int32_t house, std::uint32_t revision);
    void MerchantData_NextHouse(void* merchant);
    bool MerchantData_IsHouseInArray(const void* merchant, std::int32_t house, std::uint32_t revision);
    void* MerchantData_GetHouseDataPtr(void* merchant, std::int32_t house, std::uint32_t revision);
    void* MerchantData_GetCurrentHouseDataPtr(void* merchant);
    void* MerchantData_GetNextHouseDataPtr(void* merchant);
    bool MerchantData_IsTradeAgreementValid(void* merchant, std::int32_t player);
    bool MerchantData_Tool_CanHouseBeAdded(const void* merchant, std::int32_t human, std::int32_t house);
    void MerchantHouseData_Init(void* data, const void* house, bool foreign);
    void MerchantHouseData_SetGoodTypeImportFlag(void* data, std::uint32_t good, bool enabled);
    bool MerchantHouseData_GetGoodTypeImportFlag(const void* data, std::uint32_t good);
    bool TradeAgreement_IsValid(const void* agreement);
    std::uint32_t HumanMerchantArray_GetNextTradeAgreementId(const void* manager, const void* house, std::uint32_t previous);
    void HumanMerchantArray_AddTradeAgreement(void* manager, std::uint32_t missionCheck, std::uint32_t firstGood, std::uint32_t firstAmount, std::uint32_t secondGood, std::uint32_t secondAmount);
    void __fastcall HumanMerchantArray_CleanMap_LoadTradeAgreements(void* manager, void* unusedEdx, NXBasics::CIniFileReader* reader);
    bool __cdecl HumanMerchantArray_l_LogicCallback(std::uint32_t type, std::uint32_t callbackId, std::uint32_t firstPlayer, std::uint32_t secondPlayer, std::uint32_t state);
    void Human_l_Trader_ResetImportFlags(void* human);
    std::uint32_t Human_DoExecuteUserCommand_AttachTradeHouse(void* human, std::int32_t house, std::uint32_t revision);
    std::uint32_t Human_DoExecuteUserCommand_DetachTradeHouse(void* human, std::int32_t house, std::uint32_t revision);
}
