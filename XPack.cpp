#include "SwitchIdentifiers.h"
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
        case SwitchIds::PackingType::Rle8: return XB_Pack_Simple8BitRLEUnPack(packedBlock, destination, destinationBytes);
        case SwitchIds::PackingType::Rle16: return XB_Pack_Simple16BitRLEUnPack(packedBlock, destination, destinationBytes);
        case SwitchIds::PackingType::None:
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
    namespace
    {
        // Shared packet state, translated from the separate Windows byte/word packers.
        template<class T> std::uint32_t PackSimpleRLE(const void* source, std::uint32_t sourceBytes, void* destination, std::uint32_t destinationBytes, std::uint32_t packingType)
        {
            auto* output = static_cast<std::byte*>(destination);
            const auto* input = static_cast<const std::byte*>(source);
            const std::uint32_t header[3] = {0x5870636B, packingType, sourceBytes};
            std::memcpy(output, header, sizeof(header));
            std::uint32_t used = 16;
            T literals[127];
            T last{};
            std::uint32_t literalCount = 0, runCount = 0;
            const auto writeByte = [&](std::uint8_t value)
            {
                if (used == destinationBytes) return false;
                output[used++] = static_cast<std::byte>(value);
                return true;
            };
            const auto writeValue = [&](T value)
            {
                if constexpr (sizeof(T) == 1) { if (used == destinationBytes) return false; }
                else if (used >= destinationBytes - 1u) return false;
                std::memcpy(output + used, &value, sizeof(T));
                used += sizeof(T);
                return true;
            };
            const auto flushLiterals = [&]()
            {
                if (!writeByte(static_cast<std::uint8_t>(literalCount))) return false;
                for (std::uint32_t index = 0; index < literalCount; ++index) if (!writeValue(literals[index])) return false;
                literalCount = 0;
                return true;
            };
            const auto flushRun = [&]()
            {
                if (!writeByte(static_cast<std::uint8_t>(runCount | 0x80)) || !writeValue(last)) return false;
                runCount = 0;
                return true;
            };
            const auto appendLiteral = [&](T value)
            {
                literals[literalCount++] = value;
                return literalCount != 127 || flushLiterals();
            };
            for (std::uint32_t offset = 0; offset != sourceBytes; offset += sizeof(T))
            {
                T value;
                std::memcpy(&value, input + offset, sizeof(T));
                if (literalCount == 0)
                {
                    if (runCount == 0) { last = value; runCount = 1; }
                    else if (value == last) { if (++runCount == 127 && !flushRun()) return 0; }
                    else if (runCount > 2) { if (!flushRun()) return 0; last = value; runCount = 1; }
                    else
                    {
                        const std::uint32_t count = runCount;
                        for (std::uint32_t index = 0; index < count; ++index) if (!appendLiteral(last)) return 0;
                        runCount = 0;
                        if (!appendLiteral(value)) return 0;
                    }
                }
                else if (runCount != 0)
                {
                    if (value == last)
                    {
                        if (++runCount == 127 && (!flushLiterals() || !flushRun())) return 0;
                    }
                    else
                    {
                        if (!flushLiterals() || !flushRun()) return 0;
                        last = value;
                        runCount = 1;
                    }
                }
                else if (literals[literalCount - 1] == value) { --literalCount; last = value; runCount = 2; }
                else if (!appendLiteral(value)) return 0;
            }
            if (literalCount != 0 && !flushLiterals()) return 0;
            if (runCount != 0 && !flushRun()) return 0;
            std::memcpy(output + 12, &used, 4);
            return used;
        }
    }

    // Original function: FUN_004585c6.
    std::uint32_t XB_Pack_Simple8BitRLEPack(const void* source, std::uint32_t sourceBytes, void* destination, std::uint32_t destinationBytes)
    {
        return PackSimpleRLE<std::uint8_t>(source, sourceBytes, destination, destinationBytes, 0x726C6538);
    }

    // Original function: FUN_0045892a; odd source lengths are rejected before writing the header.
    std::uint32_t XB_Pack_Simple16BitRLEPack(const void* source, std::uint32_t sourceBytes, void* destination, std::uint32_t destinationBytes)
    {
        if ((sourceBytes & 1u) != 0) return 0;
        return PackSimpleRLE<std::uint16_t>(source, sourceBytes, destination, destinationBytes, 0x726C6536);
    }

    // Original function: FUN_00458ce2.
    std::uint32_t XB_Pack_Pack(std::uint32_t packingType, const void* source, std::uint32_t sourceBytes, void* destination, std::uint32_t destinationBytes)
    {
        switch (packingType)
        {
        case SwitchIds::PackingType::Rle8Signed: return XB_Pack_Simple8BitRLEPack(source, sourceBytes, destination, destinationBytes);
        case SwitchIds::PackingType::Rle16Signed: return XB_Pack_Simple16BitRLEPack(source, sourceBytes, destination, destinationBytes);
        case SwitchIds::PackingType::NoneSigned:
        {
            const std::uint32_t header[4] = {0x5870636B, packingType, sourceBytes, sourceBytes + 16u};
            std::memcpy(destination, header, sizeof(header));
            if (destinationBytes < sourceBytes + 16u) return 0;
            std::memcpy(static_cast<std::byte*>(destination) + 16, source, sourceBytes);
            return sourceBytes + 16u;
        }
        default: return 0;
        }
    }

}
