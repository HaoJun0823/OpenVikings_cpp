#include "C2GameState.h"

namespace NC2Logic
{
    // Original functions: FUN_00410ccd, FUN_00410cd2; NXBasics::CRandom::Get/l_GetLong.
    std::uint32_t Random_Get(SGameRandomState& random)
    {
        const std::uint32_t high = (random.currentState >> 16) * 0x41A7u;
        std::uint32_t value = (random.currentState & 0xFFFFu) * 0x41A7u + ((high & 0x7FFFu) << 16);
        if (value > 0x7FFFFFFFu) value = (value & 0x7FFFFFFFu) + 1u;
        value += high >> 15;
        if (value > 0x7FFFFFFFu) value = (value & 0x7FFFFFFFu) + 1u;
        random.currentState = value;
        return value;
    }

    // Original function: FUN_00407570; CGame::Random_GetNumber.
    std::uint32_t Game_Random_GetNumber() { return Random_Get(gGameRandomState); }

    // Original function: FUN_0040aed5.
    std::uint32_t Logic_EntryPoint_Random_GetNumber() { return Game_Random_GetNumber(); }
}
