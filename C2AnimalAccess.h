#pragma once
#include <cstdint>

namespace NC2Logic
{
    const void* Animal_GetMapPositionRef(const void* animal);
    std::uint32_t Animal_GetCurrentAtomicAnimationLengthInTicks(const void* animal);
    std::uint32_t Animal_HitPoints_GetMaximum(const void* animal);
}
