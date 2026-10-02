#pragma once
#include <cstddef>
#include <cstdint>
namespace NC2Logic
{
    struct SPlayerStatisticSampleData
    {
        std::uint16_t humans[10];
        std::uint16_t houses[60];
        std::uint32_t goodTotal;
        std::uint16_t goods[66];
        std::uint32_t values114[3];
        std::byte unknown120[16];
        std::uint32_t values130[2];
    };
    static_assert(sizeof(SPlayerStatisticSampleData) == 0x138);
    static_assert(offsetof(SPlayerStatisticSampleData, goodTotal) == 0x8C);
    static_assert(offsetof(SPlayerStatisticSampleData, values114) == 0x114);
    void Tool_PlayerStatistics_CountHumans(std::uint32_t player, std::uint16_t* counts);
    void Tool_PlayerStatistics_CountHouses(std::uint32_t player, std::uint16_t* counts);
    void Tool_PlayerStatistics_CountGoods(std::uint32_t player, void* counts);
    void PlayerStatisticManager_CreateStatisticSamples(std::uint32_t player, SPlayerStatisticSampleData* sample);
    void PlayerStatisticManager_CreateStatistics(void* manager);
    void* __fastcall PlayerStatisticManager_Construct(void* manager, void* unusedEdx);
    void __fastcall PlayerStatisticManager_UpdateStatistics(void* manager, void* unusedEdx, std::uint32_t time);
}
