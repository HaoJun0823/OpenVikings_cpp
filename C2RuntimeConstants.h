#pragma once

// Windows game random seeds, state sentinels and caller-defined sound instance tags.
// Values and integer literal types are retained from the Windows reconstruction.
namespace NC2Runtime
{
    namespace HumanAssistantCommandHistory
    {
        inline constexpr auto Capacity = 10;
        inline constexpr auto TicksPerDurationUnit = 0x2D0u;
    }

    namespace MoveableAnimationMode
    {
        inline constexpr auto Unspecified = -1;
        inline constexpr auto ProgressFrames = 0;
        inline constexpr auto CyclicFrames = 1;
        inline constexpr auto Compound = 2;
    }

    namespace MoveableAnimationPartType
    {
        inline constexpr auto Walk = 1;
        inline constexpr auto WalkUnsigned = 1u;
        inline constexpr auto AtomicAnimation = 2;
        inline constexpr auto AtomicAnimationUnsigned = 2u;
        inline constexpr auto HouseBobOverlay = 3;
        inline constexpr auto HouseBobOverlayUnsigned = 3u;
        inline constexpr auto LandscapeOverlay = 4;
        inline constexpr auto LandscapeOverlayUnsigned = 4u;
    }

    namespace MoveableAnimationLimits
    {
        inline constexpr auto FramesPerList = 500;
        inline constexpr auto CompoundParts = 50;
        inline constexpr auto OverlaysPerLayer = 5;
    }

    namespace HumanAppearanceLimits
    {
        inline constexpr auto Goods = 66;
        inline constexpr auto GraphicsChoices = 5;
        inline constexpr auto Tribes = 8;
        inline constexpr auto PlayerColors = 10;
        inline constexpr auto InhabitantSigns = 10;
        inline constexpr auto SoldierSigns = 5;
        inline constexpr auto DestinationRanges = 32;
    }

    namespace HumanPaletteRange
    {
        inline constexpr auto FirstHeadRange = 16;
        inline constexpr auto LocalRangeMask = 15u;
        inline constexpr auto Bytes = 64;
        inline constexpr auto BytesUnsigned = 64u;
    }

    namespace FastMoveableAccessLimits
    {
        inline constexpr auto TribeSlots = 42;
        inline constexpr auto JobSlots = 56;
        inline constexpr auto JobSlotsHex = 0x38;
        inline constexpr auto RandomAnimationChoices = 10;
        inline constexpr auto FramesPerWalkList = 200;
    }

    namespace WeatherGrid
    {
        inline constexpr auto SectorWidthInMapPoints = 10;
        inline constexpr auto SectorCenterInMapPoints = 5;
        inline constexpr auto SectorRoundUp = 9;
        inline constexpr auto FullPercentage = 100;
        inline constexpr auto DensityMaximum = 10000;
        inline constexpr auto DensityUnitsPerPercent = 100u;
        inline constexpr auto ColorTableBytes = 131072;
        inline constexpr auto ColorTableEntries = 0x10000;
    }

    namespace WeatherType
    {
        inline constexpr auto None = 0;
        inline constexpr auto Rain = 1;
        inline constexpr auto Snow = 2;
        inline constexpr auto Sand = 3;
    }

    namespace DisplayScrolling
    {
        inline constexpr auto MinimumWorldX = -100;
        inline constexpr auto MinimumWorldY = -200;
        inline constexpr auto MaximumWorldMargin = 100u;
        inline constexpr auto FullRebuildAreaPercent = 75u;
    }

    namespace EarthquakeDisplacement
    {
        inline constexpr auto HorizontalRange = 41u;
        inline constexpr auto HorizontalCenter = 20u;
        inline constexpr auto VerticalRange = 21u;
        inline constexpr auto VerticalCenter = 10u;
    }

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

    namespace PathfinderCalculationValue
    {
        inline constexpr auto BlockedStartAreaFlag = 0x200;
        inline constexpr auto DirectionMaskBase = 0x400u;
    }

    namespace PathfinderSearchState
    {
        inline constexpr auto Stopped = 0;
        inline constexpr auto SearchRequired = 1;
        inline constexpr auto SearchRequiredUnsigned = 1u;
        inline constexpr auto RouteAvailable = 2;
        inline constexpr auto RouteAvailableUnsigned = 2u;
        inline constexpr auto Arrived = 3;
        inline constexpr auto ArrivedUnsigned = 3u;
    }

    namespace PathfinderTargetMode
    {
        inline constexpr auto Position = 1;
        inline constexpr auto PositionUnsigned = 1u;
        inline constexpr auto Moveable = 2;
        inline constexpr auto MoveableUnsigned = 2u;
    }

}
