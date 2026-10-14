#pragma once

// Windows game random seeds, state sentinels and caller-defined sound instance tags.
// Values and integer literal types are retained from the Windows reconstruction.
namespace NC2Runtime
{
    namespace GuideRoutingLimits
    {
        inline constexpr auto DistanceBuckets = 40;
        inline constexpr auto ConnectionsPerGuide = 8;
    }

    namespace GoodSearchLimits
    {
        inline constexpr auto WorkCenterPileRadius = 20;

        inline constexpr auto QuarryPileRadius = 10;

        inline constexpr auto SearchRadiusUnsigned = 40u;

        inline constexpr auto SearchRadius = 40;

        inline constexpr auto LandscapeCandidates = 10;
        inline constexpr auto HouseCandidates = 2;
        inline constexpr auto GuideCandidates = 3;
        inline constexpr auto DistanceSlackUnsigned = 10u;
        inline constexpr auto Goods = 66;
    }

    namespace ProductionCounter
    {
        inline constexpr auto ImpossibleUnsigned = 14u;
        inline constexpr auto Impossible = 14;
        inline constexpr auto Incapable = 13;
        inline constexpr auto Forbidden = 12;
        inline constexpr auto Infinite = 11;
    }


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

    namespace InputTargetPointLimits
    {
        inline constexpr auto Points = 300;
    }

    namespace InputSelectionLimits
    {
        inline constexpr auto Humans = 300;
        inline constexpr auto Vehicles = 30;
    }

    namespace DisplaySelectionLimits
    {
        inline constexpr auto SecondaryMarks = 30;
        inline constexpr auto BobsPerHit = 20;
    }

    namespace DisplayObjectMarkType
    {
        inline constexpr auto LocalOrUnowned = 1u;
        inline constexpr auto OtherPlayer = 3u;
    }

    namespace DisplayProjection
    {
        inline constexpr auto DefaultVerticalSpacing = 19.0f;
        inline constexpr auto DefaultHorizontalSpacing = 34.0f;
        inline constexpr auto DefaultHeightScale = 1.2f;
        inline constexpr auto MinimumVerticalSpacing = 1.0f;
        inline constexpr auto MinimumHorizontalSpacing = 2.0f;
        inline constexpr auto MinimumHeightScale = 0.0f;
        inline constexpr auto FixedPointScale = 256.0f;
        inline constexpr auto DoubledFixedPointScale = 512.0f;
        inline constexpr auto FractionBits = 8;
        inline constexpr auto RoundingBias = 127u;
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
        inline constexpr auto FullStepSigned = 10000;
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

    namespace BuildLimits
    {
        inline constexpr auto Players = 20;
        inline constexpr auto WorkerSearchRadius = 50;
        inline constexpr auto RoadWorkers = 3;
        inline constexpr auto WallWorkers = 5;
        inline constexpr auto RoadPoints = 200;
        inline constexpr auto WallPoints = 100;
    }

    namespace MapTerrainFlags
    {
        inline constexpr auto RoadBuildPending = 4;
        inline constexpr auto WallBuildPending = 8;
        inline constexpr auto WallBuildPendingUnsigned = 8u;
        inline constexpr auto RoadBuildPendingUnsigned = 4u;
        inline constexpr auto RoadUnsigned = 2u;
    }

    namespace PlayerAllowanceStride
    {
        inline constexpr auto VehiclesUnsigned = 7u;
        inline constexpr auto HousesUnsigned = 55u;
        inline constexpr auto JobsAndEquipmentUnsigned = 56u;
        inline constexpr auto GoodsUnsigned = 66u;
    }

    namespace PlayerAllowanceSlots
    {
        inline constexpr auto Tribes = 42;
    }

    namespace GuiMessageLimits
    {
        inline constexpr auto Capacity = 200;

        inline constexpr auto LifetimeTicksUnsigned = 3600u;
    }

    namespace MissionCapacity
    {
        inline constexpr auto Missions = 150;

        inline constexpr auto GoalsAndResults = 30;
    }

    namespace AIConditionReference
    {
        inline constexpr auto UnresolvedOperand = 999999;

        inline constexpr auto AlwaysInactive = 100001;

        inline constexpr auto AlwaysActive = 100000;
    }

    namespace AIConditionLimits
    {
        inline constexpr auto TicksPerScriptSecondUnsigned = 12u;

        inline constexpr auto TicksPerScriptMinuteUnsigned = 720u;

        inline constexpr auto Operands = 10;

        inline constexpr auto PlayerDeathGraceTicks = 720;

        inline constexpr auto RecheckPasses = 10;

        inline constexpr auto Slots = 100;
    }

    namespace MerchantLimits
    {
        inline constexpr auto DefaultCapacity = 1000;

        inline constexpr auto Goods = 66;

        inline constexpr auto TradeAgreements = 60;

        inline constexpr auto Houses = 2;
    }

    namespace AIEquipmentLimits
    {
        inline constexpr auto RecruitmentLimit = 190;

        inline constexpr auto Armors = 5;

        inline constexpr auto Tribes = 8;

        inline constexpr auto Weapons = 22;
    }

    namespace AttachedVehicleSelectionLimits
    {
        inline constexpr auto Rows = 30;
    }

    namespace NetworkCommandLimits
    {
        inline constexpr auto MaximumFrequency = 12;

        inline constexpr auto MinimumFrequency = 1;

        inline constexpr auto TicksPerSecondUnsigned = 12u;

        inline constexpr auto ChecksumExpiryTicksUnsigned = 1440u;

        inline constexpr auto ChecksumExpiryTicks = 1440;

        inline constexpr auto Players = 20;

        inline constexpr auto SamplesPerPlayer = 10;

        inline constexpr auto PacketCommandsUnsigned = 9u;
    }

    namespace MissionTiming
    {
        inline constexpr auto TicksPerSecondUnsigned = 12u;

        inline constexpr auto EvaluationIntervalTicksUnsigned = 36u;
    }

    namespace VehicleCommandQueue
    {
        inline constexpr auto Capacity = 22;
    }

    namespace VehicleWeaponSlot
    {
        inline constexpr auto CatapultUnsigned = 21u;
    }

    namespace VehicleAtomicAction
    {
        inline constexpr auto Dock = 0x54;

        inline constexpr auto Attack = 0x51;

        inline constexpr auto Walk = 1;

        inline constexpr auto None = 0;
    }

    namespace VehicleWalkTiming
    {
        inline constexpr auto TurningTicks = 2;

        inline constexpr auto ProgressRoundingBiasUnsigned = 9999u;

        inline constexpr auto MinimumTicks = 3;

        inline constexpr auto CatapultMultiplierUnsigned = 2u;

        inline constexpr auto BaseTicksUnsigned = 4u;

        inline constexpr auto TerrainMultiplierUnsigned = 2u;
    }

    namespace MapWalkingFlags
    {
        inline constexpr auto SpeedMaskUnsigned = 15u;

        inline constexpr auto SpeedShift = 3;

        inline constexpr auto VehicleSizeMaskUnsigned = 7u;
    }

}
