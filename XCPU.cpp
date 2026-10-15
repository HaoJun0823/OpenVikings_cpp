#include "XCPU.h"
#include "XNumericConstants.h"

#include <cstdint>
#include <intrin.h>

namespace
{
	// Complete cache region 00554D58..00554D5B: two flags and two unused bytes.
	// The PE places it in zero-initialized virtual data; all flag xrefs are in CPU_HasMMX.
	struct SMmxDetectionState
	{
		std::uint8_t complete;
		std::uint8_t supported;
		std::uint8_t unused[2];
	};
	SMmxDetectionState gMmxDetectionState{ };
	static_assert(sizeof(SMmxDetectionState) == 4);
}

namespace NXBasics
{
	// Original function: FUN_00473abf
	bool CPU_HasMMX()
	{
		if (gMmxDetectionState.complete != 0)
			return gMmxDetectionState.supported != 0;

		gMmxDetectionState.complete = 1;

		int registers[4];
		__cpuid(registers, 1);

		if ((static_cast<std::uint32_t>(registers[3]) & NXBasics::BitMask::Bit23) != 0)
			gMmxDetectionState.supported = 1;

		return gMmxDetectionState.supported != 0;
	}
}
