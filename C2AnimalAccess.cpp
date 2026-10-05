#include "C2AnimalAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_0042e9e8.
    std::uint32_t Animal_GetCurrentAtomicAnimationLengthInTicks(const void* animal) { return NC2Lifetime::Read<std::uint32_t>(gStaticAtomicAnimationData, NC2Lifetime::Read<std::uint32_t>(animal, 0x78) * 0x138u + 0x34); }
    // Original function: FUN_0042f6aa; animal type IDs index the native biased table.
    std::uint32_t Animal_HitPoints_GetMaximum(const void* animal)
    {
        const auto offset = NC2Lifetime::Read<std::uint32_t>(animal, 0x14) * 0x40u - (NC2Lifetime::Read<std::uint32_t>(animal, 0x18) == 48 ? 0x1CCu : 0x1C8u);
        return NC2Lifetime::Read<std::uint32_t>(gStaticAnimalTypeData, offset);
    }
    // Original function: FUN_0042ea17
    const void* Animal_GetMapPositionRef(const void* animal)
    {
        std::uint32_t index;
        std::memcpy(&index, static_cast<const std::byte*>(animal) + 0x0C, sizeof(index));

        const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(gMapMoveableArrayStorage.entries));
        const std::uint32_t positionAddress = baseAddress + index * 0x58u + 0x12u;

        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(positionAddress));
    }
}
