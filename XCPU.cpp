#include "XCPU.h"

#include <cstdint>
#include <intrin.h>

// Original data; initial bytes remain unresolved.
extern std::uint8_t DAT_00554d58;
extern std::uint8_t DAT_00554d59;

namespace NXBasics
{
    // Original function: FUN_00473abf
    bool CPU_HasMMX()
    {
        if (DAT_00554d58 != 0)
            return DAT_00554d59 != 0;

        DAT_00554d58 = 1;

        int registers[4];
        __cpuid(registers, 1);

        if ((static_cast<std::uint32_t>(registers[3]) & 0x00800000u) != 0)
            DAT_00554d59 = 1;

        return DAT_00554d59 != 0;
    }
}