#include "XCPU.h"

#include <cstdint>
#include <intrin.h>

// Original data; initial bytes remain unresolved.
extern std::uint8_t gMmxDetectionComplete;
extern std::uint8_t gMmxSupported;

namespace NXBasics
{
    // Original function: FUN_00473abf
    bool CPU_HasMMX()
    {
        if (gMmxDetectionComplete != 0)
            return gMmxSupported != 0;

        gMmxDetectionComplete = 1;

        int registers[4];
        __cpuid(registers, 1);

        if ((static_cast<std::uint32_t>(registers[3]) & 0x00800000u) != 0)
            gMmxSupported = 1;

        return gMmxSupported != 0;
    }
}