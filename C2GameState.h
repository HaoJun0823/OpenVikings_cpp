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
    // Complete clock region 00510838..0051089F, cleared by GameTime_Reset.
    struct SGameTimeState
    {
        std::uint32_t tick;
        std::byte unknown004[4];
        double lastRealTime;
        double speed;
        double tickDuration;
        double retainedSpeed;
        std::uint32_t networkBaseTick;
        std::uint32_t networkBaseRealTime;
        std::uint32_t networkFrequency;
        std::uint32_t latencySampleTime;
        std::uint32_t latencySampleIndex;
        std::uint32_t latencySamples[10];
        std::uint8_t autoAdjustNetworkFrequency;
        std::uint8_t maximizeSpeed;
        std::byte unknown066[2];
    };
    static_assert(sizeof(SGameTimeState) == 0x68);
    static_assert(offsetof(SGameTimeState, lastRealTime) == 8);
    static_assert(offsetof(SGameTimeState, networkFrequency) == 0x30);
    static_assert(offsetof(SGameTimeState, latencySamples) == 0x3C);
    static_assert(offsetof(SGameTimeState, maximizeSpeed) == 0x65);

    // Complete Windows CGame::mStaticVars region, 00510818..005109CF.
    struct SGameStaticVars
    {
        unsigned char initialized;
        std::byte unknown001[7];
        SCleanMapGuid mapGuid;                   // +0x008; original 00510820
        std::byte unknown018[8];
        SGameTimeState gameTime;
        std::byte unknown088[0x0C];
        SGameRandomState random;
        std::uint32_t DAT_005108b4;
        std::byte unknown0A0[0x104];
        NXBasics::CIniFileReader* DAT_005109bc;
        void* DAT_005109c0;
        std::uint8_t DAT_005109c4;
        std::uint8_t networkWait;                    // +0x1AD; original 005109C5.
        std::byte unknown1AE[0x0A];
    };
#pragma pack(pop)

    inline SGameStaticVars gGameStaticVars{};
    inline unsigned char& gGameInitialized = gGameStaticVars.initialized;
    inline SGameRandomState& gGameRandomState = gGameStaticVars.random;
    inline SGameTimeState& gGameTime = gGameStaticVars.gameTime;
    static_assert(sizeof(SGameRandomState) == 8);
    static_assert(sizeof(SGameStaticVars) == 0x1B8);
    static_assert(offsetof(SGameStaticVars, mapGuid) == 0x08);
    static_assert(offsetof(SGameStaticVars, gameTime) == 0x20);
    static_assert(offsetof(SGameStaticVars, random) == 0x94);
    static_assert(offsetof(SGameStaticVars, DAT_005108b4) == 0x9C);
    static_assert(offsetof(SGameStaticVars, DAT_005109bc) == 0x1A4);
    static_assert(offsetof(SGameStaticVars, DAT_005109c0) == 0x1A8);
    static_assert(offsetof(SGameStaticVars, DAT_005109c4) == 0x1AC);
    static_assert(offsetof(SGameStaticVars, networkWait) == 0x1AD);
}

// Compatibility aliases for existing callers; the clock fields now have named storage.
// Remaining game-loader/network fields retain their original labels.
inline std::uint32_t& DAT_00510838 = NC2Logic::gGameTime.tick;
inline double& DAT_00510848 = NC2Logic::gGameTime.speed;
inline std::uint8_t& DAT_0051089d = NC2Logic::gGameTime.maximizeSpeed;
inline NXBasics::CIniFileReader*& DAT_005109bc = NC2Logic::gGameStaticVars.DAT_005109bc;
inline void*& DAT_005109c0 = NC2Logic::gGameStaticVars.DAT_005109c0;
