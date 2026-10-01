#pragma once

#include <cstddef>
#include <cstdint>
#include "C2MapGuid.h"

namespace NXBasics { class CIniFileReader; }

namespace NC2Logic
{
    struct SGameRandomState
    {
        std::uint32_t currentState;
        std::uint32_t seed;
    };

#pragma pack(push, 4)
    // Complete Windows CGame::mStaticVars region, 00510818..005109CF.
    struct SGameStaticVars
    {
        unsigned char initialized;
        std::byte unknown001[7];
        SCleanMapGuid mapGuid;                   // +0x008; original 00510820
        std::byte unknown018[8];
        std::uint32_t DAT_00510838;
        std::byte unknown024[0x0C];
        double DAT_00510848;
        std::byte unknown038[0x4D];
        std::uint8_t DAT_0051089d;
        std::byte unknown086[0x0E];
        SGameRandomState random;
        std::uint32_t DAT_005108b4;
        std::byte unknown0A0[0x104];
        NXBasics::CIniFileReader* DAT_005109bc;
        void* DAT_005109c0;
        std::uint8_t DAT_005109c4;
        std::uint8_t DAT_005109c5;
        std::byte unknown1AE[0x0A];
    };
#pragma pack(pop)

    inline SGameStaticVars gGameStaticVars{};
    inline unsigned char& gGameInitialized = gGameStaticVars.initialized;
    inline SGameRandomState& gGameRandomState = gGameStaticVars.random;
    static_assert(sizeof(SGameRandomState) == 8);
    static_assert(sizeof(SGameStaticVars) == 0x1B8);
    static_assert(offsetof(SGameStaticVars, mapGuid) == 0x08);
    static_assert(offsetof(SGameStaticVars, DAT_00510838) == 0x20);
    static_assert(offsetof(SGameStaticVars, DAT_00510848) == 0x30);
    static_assert(offsetof(SGameStaticVars, DAT_0051089d) == 0x85);
    static_assert(offsetof(SGameStaticVars, random) == 0x94);
    static_assert(offsetof(SGameStaticVars, DAT_005108b4) == 0x9C);
    static_assert(offsetof(SGameStaticVars, DAT_005109bc) == 0x1A4);
    static_assert(offsetof(SGameStaticVars, DAT_005109c0) == 0x1A8);
    static_assert(offsetof(SGameStaticVars, DAT_005109c4) == 0x1AC);
    static_assert(offsetof(SGameStaticVars, DAT_005109c5) == 0x1AD);
}

// Original labels retained until each field's semantic identity is established.
inline std::uint32_t& DAT_00510838 = NC2Logic::gGameStaticVars.DAT_00510838;
inline double& DAT_00510848 = NC2Logic::gGameStaticVars.DAT_00510848;
inline std::uint8_t& DAT_0051089d = NC2Logic::gGameStaticVars.DAT_0051089d;
inline NXBasics::CIniFileReader*& DAT_005109bc = NC2Logic::gGameStaticVars.DAT_005109bc;
inline void*& DAT_005109c0 = NC2Logic::gGameStaticVars.DAT_005109c0;
inline std::uint8_t& DAT_005109c5 = NC2Logic::gGameStaticVars.DAT_005109c5;
