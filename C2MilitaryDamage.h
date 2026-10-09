#pragma once
#include <cstdint>

namespace NC2Logic
{
    bool Human_Item_IsAmuletEquipped_Strength(const void* human);
    bool Human_Item_IsAmuletEquipped_CriticalHit(const void* human);
    std::uint32_t Human_GetExperienceForEquippedWeapon(const void* human);
    std::int32_t Tool_Military_GetDamage(std::int32_t damage, const void* attacker);
    bool Tool_Misc_GetFleePointFarestAwayFromTargetPointRelativToSourcePoint(const void* source, const void* target, void* result, std::uint32_t radius);
    void Animal_l_Aggressive_GetAngry(void* animal);
    bool Animal_Military_StartGettingHitByWeapon(void* animal, std::uint32_t tribe, std::uint32_t weapon, std::uint32_t attackerId, std::uint32_t attackerRevision, std::uint32_t attackerType, std::uint32_t power, bool propagate);
}
