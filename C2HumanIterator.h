#pragma once

#include "C2AnimalIterator.h"

namespace NC2Logic
{
    // Windows shares the constructor and 2C-byte layout with CAnimalIterator.
    class CHumanIterator : private CAnimalIterator
    {
    public:
        CHumanIterator() = default;
        std::int32_t GetFirstHumanId();
        std::int32_t GetFirstHumanId(std::uint32_t player);
        std::int32_t GetFirstHumanId(std::uint32_t player, const void* point, std::int32_t radius);
        std::int32_t GetNextHumanId();
    private:
        std::int32_t l_GetNext(std::int32_t previous);
    };
    static_assert(sizeof(CHumanIterator) == 0x2C);
}
