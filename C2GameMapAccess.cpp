#include "C2GameMapAccess.h"
#include "ApplicationGlobals.h"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace NC2Logic
{
    // Original function: FUN_004237d2
    const void* GameMap_GetConstMapMicroPointPtr(const void* packedMigPoint)
    {
        const auto* position = static_cast<const std::byte*>(packedMigPoint);

        std::int16_t y;
        std::int16_t width;
        std::int16_t x;

        std::memcpy(&y, position + 2, sizeof(y));
        std::memcpy(&width, DAT_00511570, sizeof(width));
        std::memcpy(&x, position, sizeof(x));

        const std::uint32_t index = static_cast<std::uint32_t>(static_cast<std::int32_t>(y)) * static_cast<std::uint32_t>(static_cast<std::int32_t>(width)) + static_cast<std::uint32_t>(static_cast<std::int32_t>(x));
        const std::uint32_t offset = index * 14u;

        std::uint32_t baseAddress;
        std::memcpy(&baseAddress, DAT_00511570 + 0x34, sizeof(baseAddress));

        const std::uint32_t address = baseAddress + offset;
        return reinterpret_cast<const void*>(static_cast<std::uintptr_t>(address));
    }
}