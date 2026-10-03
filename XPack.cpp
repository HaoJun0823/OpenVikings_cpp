#include "XPack.h"

#include <cstddef>
#include <cstring>

namespace NXBasics
{
    namespace
    {
        std::uint32_t ReadPackedLong(const void* block, std::uint32_t offset)
        {
            std::uint32_t value;
            std::memcpy(&value, static_cast<const std::byte*>(block) + offset, sizeof(value));
            return value;
        }
    }

    // Original function: FUN_00458890.
    bool XB_Pack_Simple8BitRLEUnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes)
    {
        if (ReadPackedLong(packedBlock, 0) != 0x5870636Bu || ReadPackedLong(packedBlock, 4) != 0x726C6538u || destinationBytes < ReadPackedLong(packedBlock, 8))
            return false;

        const auto* block = static_cast<const std::uint8_t*>(packedBlock);
        const auto* source = block + 0x10;
        const auto* end = block + ReadPackedLong(packedBlock, 0x0C);
        auto* output = static_cast<std::uint8_t*>(destination);
        // The original compares exact input positions; it does not bound each run.
        while (source != end)
        {
            const std::uint8_t descriptor = *source++;
            const std::uint32_t count = descriptor & 0x7Fu;
            if ((descriptor & 0x80u) != 0)
            {
                const std::uint8_t value = *source++;
                for (std::uint32_t index = 0; index < count; ++index)
                    *output++ = value;
            }
            else
            {
                for (std::uint32_t index = 0; index < count; ++index)
                    *output++ = *source++;
            }
        }
        return true;
    }

    // Original function: FUN_00458c53.
    bool XB_Pack_Simple16BitRLEUnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes)
    {
        if (ReadPackedLong(packedBlock, 0) != 0x5870636Bu || ReadPackedLong(packedBlock, 4) != 0x726C6536u || destinationBytes < ReadPackedLong(packedBlock, 8))
            return false;

        const auto* block = static_cast<const std::byte*>(packedBlock);
        const auto* source = block + 0x10;
        const auto* end = block + ReadPackedLong(packedBlock, 0x0C);
        auto* output = static_cast<std::byte*>(destination);
        while (source != end)
        {
            const auto descriptor = static_cast<std::uint8_t>(*source++);
            const std::uint32_t count = descriptor & 0x7Fu;
            if ((descriptor & 0x80u) != 0)
            {
                std::uint16_t value;
                std::memcpy(&value, source, sizeof(value));
                source += 2;
                for (std::uint32_t index = 0; index < count; ++index)
                {
                    std::memcpy(output, &value, sizeof(value));
                    output += 2;
                }
            }
            else
            {
                for (std::uint32_t index = 0; index < count; ++index)
                {
                    std::uint16_t value;
                    std::memcpy(&value, source, sizeof(value));
                    std::memcpy(output, &value, sizeof(value));
                    source += 2;
                    output += 2;
                }
            }
        }
        return true;
    }

    // Original function: FUN_00458d60.
    bool XB_Pack_UnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes)
    {
        if (ReadPackedLong(packedBlock, 0) != 0x5870636Bu)
            return false;
        switch (ReadPackedLong(packedBlock, 4))
        {
        case 0x726C6538u: return XB_Pack_Simple8BitRLEUnPack(packedBlock, destination, destinationBytes);
        case 0x726C6536u: return XB_Pack_Simple16BitRLEUnPack(packedBlock, destination, destinationBytes);
        case 0x6E6F6E65u:
        {
            const std::uint32_t unpackedBytes = ReadPackedLong(packedBlock, 8);
            if (destinationBytes < unpackedBytes)
                return false;
            std::memcpy(destination, static_cast<const std::byte*>(packedBlock) + 0x10, unpackedBytes);
            return true;
        }
        default: return false;
        }
    }
}
