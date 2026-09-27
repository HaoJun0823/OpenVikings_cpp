#include "C2AnimalAccess.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

extern void* DAT_00510a18;

namespace NC2Logic
{
    // Original function: FUN_0042ea17
    const void* Animal_GetMapPositionRef(const void* animal)
    {
        std::uint32_t index;
        std::memcpy(&index, static_cast<const std::byte*>(animal) + 0x0C, sizeof(index));

        const std::uint32_t baseAddress = static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(DAT_00510a18));
        const std::uint32_t positionAddress = baseAddress + index * 0x58u + 0x12u;

        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(positionAddress));
    }
}