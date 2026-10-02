#include "C2VehicleAccess.h"
#include "C2HumanIterator.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2MapGeometry.h"
#include "XCrtFileAccess.h"


namespace NC2Logic
{
    // Original function: FUN_0042c1d4.
    std::int32_t CHumanIterator::GetFirstHumanId()
    {
        flags = 0;
        currentAnimalId = l_GetNext(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(CHumanArray::sTheObjectPtr->mFirstScanIndex) - 1u));
        return currentAnimalId;
    }

    // Original function: FUN_0042c1ee.
    std::int32_t CHumanIterator::GetFirstHumanId(std::uint32_t player)
    {
        selection = player;
        flags = 1;
        currentAnimalId = l_GetNext(NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(CHumanArray::sTheObjectPtr->mFirstScanIndex) - 1u));
        return currentAnimalId;
    }

    // Original function: FUN_0042c392.
    std::int32_t CHumanIterator::GetNextHumanId()
    {
        if (currentAnimalId == -1) return -1;
        currentAnimalId = l_GetNext(currentAnimalId);
        return currentAnimalId;
    }

    // Original function: FUN_0042c3ae.
    std::int32_t CHumanIterator::l_GetNext(std::int32_t previous)
    {
        auto index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(previous) + 1u);
        for (; index <= CHumanArray::sTheObjectPtr->mLastScanIndex; index = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(index) + 1u))
        {
            const void* human = reinterpret_cast<const void*>(reinterpret_cast<std::uintptr_t>(CHumanArray::mStaticVars) + static_cast<std::uint32_t>(index) * 0xC58u);
            if (NC2Lifetime::Read<std::uint8_t>(human, 0) == 0) continue;
            if ((flags & 1u) != 0 && NC2Lifetime::Read<std::uint32_t>(human, 0x10) != selection) continue;
            if ((flags & 2u) != 0)
            {
                const auto* position = Human_GetMapPositionRef(human);
                if (static_cast<std::uint32_t>(VE_HexagonDirection_GetDistance(static_cast<std::int16_t>(position[0]), static_cast<std::int16_t>(position[1]), static_cast<std::int16_t>(packedCenter), static_cast<std::int16_t>(packedCenter >> 16))) > static_cast<std::uint32_t>(radius)) continue;
            }
            return index;
        }
        return -1;
    }
}
