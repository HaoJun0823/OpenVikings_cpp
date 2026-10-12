#pragma once
#include <cstdint>
namespace NC2Logic
{
    bool AIAnimalTypeCounts_IsRequested(const void* counts, std::uint32_t type);
    bool AIAnimalTypeCounts_IsKnownOrSatisfied(const void* counts, std::uint32_t type);
    bool AIAnimalCatcher_IsType10Available(const void* catcher);
    bool AIAnimalCatcher_IsType19Available(const void* catcher);
    void AIAnimalCatcher_Init(void* catcher);
    void AIAnimalCatcher_DoOneLogicTick(void* catcher, std::uint32_t player, const void* center, void* military);
    void AIDiplomacy_DoOneLogicTick(void* diplomacy, std::uint32_t player, const void* villages);
}
