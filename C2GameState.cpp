#include "XNumericConstants.h"
#include "C2GameState.h"

namespace NC2Logic
{
    // Original functions: FUN_00410ccd, FUN_00410cd2; NXBasics::CRandom::Get/l_GetLong.
    std::uint32_t Random_Get(SGameRandomState& random)
    {
        const std::uint32_t high = (random.currentState >> 16) * 0x41A7u;
        std::uint32_t value = (random.currentState & NXBasics::IntegerRange::UInt16Maximum) * 0x41A7u + ((high & NXBasics::IntegerRange::Int16Maximum) << 16);
        if (value > NXBasics::IntegerRange::Int32Maximum) value = (value & NXBasics::IntegerRange::Int32Maximum) + 1u;
        value += high >> 15;
        if (value > NXBasics::IntegerRange::Int32Maximum) value = (value & NXBasics::IntegerRange::Int32Maximum) + 1u;
        random.currentState = value;
        return value;
    }

    // Original function: FUN_00407570; CGame::Random_GetNumber.
    std::uint32_t Game_Random_GetNumber() { return Random_Get(gGameRandomState); }

    // Original function: FUN_0040aed5.
    std::uint32_t Logic_EntryPoint_Random_GetNumber() { return Game_Random_GetNumber(); }
    // Original function: FUN_0040757a; CGame::UniqueId_Generate.
    std::uint32_t Game_UniqueId_Generate() { return ++gGameStaticVars.uniqueIdCounter; }
}
