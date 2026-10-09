#pragma once

#include "C2AnimalIterator.h"

namespace NC2Logic
{
    class CVehicleIterator : private CAnimalIterator
    {
    public:
        CVehicleIterator() = default;
        std::int32_t GetFirstSectorVehicleId(std::uint32_t player, std::uint32_t point, std::int32_t radius, bool useSecondList);
        std::int32_t GetNextSectorVehicleId();
    private:
        std::int32_t l_GetNextSectorVehicleId(std::int32_t previous);
    };
    static_assert(sizeof(CVehicleIterator) == 0x2C);
}
