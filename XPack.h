#pragma once

#include <cstdint>

namespace NXBasics
{
    bool XB_Pack_Simple8BitRLEUnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes);
    bool XB_Pack_Simple16BitRLEUnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes);
    bool XB_Pack_UnPack(const void* packedBlock, void* destination, std::uint32_t destinationBytes);
}
