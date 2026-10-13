#pragma once

// Windows game random seeds, state sentinels and caller-defined sound instance tags.
// Values and integer literal types are retained from the Windows reconstruction.
namespace NC2Runtime
{
    namespace RandomSeed
    {
        inline constexpr auto Game = 0x18273645;
        inline constexpr auto DisplayEffects = 0x5432ACE0u;
    }

    namespace SoundTag
    {
        inline constexpr auto MissionBriefingVoice = 0x65655656;
    }

    namespace AnimalFoodValue
    {
        inline constexpr auto Slaughtered = -100000;
        inline constexpr auto SlaughteredBits = 0xFFFE7960u;
    }

    namespace WalkingProgress
    {
        inline constexpr auto RelocationThreshold = 5000u;
        inline constexpr auto FullStep = 10000u;
    }

    namespace PathfinderDirectionResult
    {
        inline constexpr auto Deferred = -4;
        inline constexpr auto Stopped = -3;
        inline constexpr auto Arrived = -2;
        inline constexpr auto Failed = -1;
    }

}
