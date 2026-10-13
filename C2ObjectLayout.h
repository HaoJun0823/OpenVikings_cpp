#pragma once

// Established byte offsets and record sizes of original Windows game objects.
// Values and integer literal types are retained from the Windows reconstruction.
namespace NC2Layout
{
    namespace AnimalOffset
    {
        inline constexpr auto ConstructedState = 0x18;
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto AnimalId = 4;
        inline constexpr auto UniqueId = 8;
        inline constexpr auto MapMoveableId = 0x0C;
        inline constexpr auto PlayerId = 0x10;
        inline constexpr auto TribeType = 0x14;
        inline constexpr auto AnimalType = 0x18;
        inline constexpr auto CurrentTask = 0x1C;
        inline constexpr auto CurrentTaskStatus = 0x20;
        inline constexpr auto WorkCenter = 0x24;
        inline constexpr auto WorkCenterX = 0x24;
        inline constexpr auto WorkCenterY = 0x26;
        inline constexpr auto StayPoint = 0x28;
        inline constexpr auto StayPointX = 0x28;
        inline constexpr auto StayPointY = 0x2A;
        inline constexpr auto BirthPoint = 0x2C;
        inline constexpr auto BirthPointX = 0x2C;
        inline constexpr auto BirthPointY = 0x2E;
        inline constexpr auto LeaderId = 0x34;
        inline constexpr auto LeaderRevision = 0x38;
        inline constexpr auto GroupSize = 0x3C;
        inline constexpr auto EnvironmentalFoodValue = 0x40;
        inline constexpr auto MilitaryTarget = 0x44;
        inline constexpr auto MilitaryTargetUnsigned = 0x44u;
        inline constexpr auto AttachmentTarget = 0x50;
        inline constexpr auto AttachedObjectId = 0x54;
        inline constexpr auto AttachedObjectRevision = 0x58;
        inline constexpr auto AggressiveFlag = 0x5C;
        inline constexpr auto AggressionClearDelay = 0x60;
        inline constexpr auto Hitpoints = 0x64;
        inline constexpr auto Food = 0x68;
        inline constexpr auto AgeCounter = 0x6C;
        inline constexpr auto AtomicAction = 0x70;
        inline constexpr auto AtomicAnimationRunningFlag = 0x74;
        inline constexpr auto AtomicAnimationId = 0x78;
        inline constexpr auto AtomicAnimationFrame = 0x7C;
        inline constexpr auto AtomicAnimationStartCount = 0x80;
        inline constexpr auto PreviousSectorAnimalId = 0x84;
        inline constexpr auto NextSectorAnimalId = 0x88;
        inline constexpr auto MissionCheckId = 0x8C;
        inline constexpr auto MiscFlags = 0x90;
        inline constexpr auto InsideHouseFlag = 0x94;
    }

    namespace DisplayAtomInformationOffset
    {
        inline constexpr auto BodyBobId = 0;
        inline constexpr auto HeadBobId = 4;
        inline constexpr auto AnimationTribe = 8;
        inline constexpr auto AnimationJob = 0x0C;
        inline constexpr auto AnimationAction = 0x10;
        inline constexpr auto AnimationStartCount = 0x14;
        inline constexpr auto WalkTribe = 0x18;
        inline constexpr auto WalkJob = 0x1C;
        inline constexpr auto WalkGood = 0x20;
        inline constexpr auto WalkSpeed = 0x24;
        inline constexpr auto BodyBobManager = 0x28;
        inline constexpr auto HeadBobManager = 0x2C;
        inline constexpr auto ShadowBobManager = 0x30;
        inline constexpr auto Animation = 0x34;
        inline constexpr auto Walk = 0x38;
        inline constexpr auto ActorSpecificData = 0x3C;
    }

    namespace DynamicHumanOffset
    {
        inline constexpr auto BodyVariant = 0x3C;
        inline constexpr auto HeadVariant = 0x40;
        inline constexpr auto BodyPalette = 0x44;
        inline constexpr auto HeadPalette = 0x450;
        inline constexpr auto Name = 0x85C;
        inline constexpr auto NameUnsigned = 0x85Cu;
        inline constexpr auto NameTerminator = 0x8BF;
        inline constexpr auto HasUserMessageFlag = 0x8C0;
        inline constexpr auto SelectedFlag = 0x8C4;
        inline constexpr auto HighlightedFlag = 0x8C5;
    }

    namespace DynamicAnimalVehicleOffset
    {
        inline constexpr auto PaletteId = 0x3C;
        inline constexpr auto SelectedFlag = 0x40;
        inline constexpr auto HighlightedFlag = 0x41;
    }

    namespace DynamicHouseOffset
    {
        inline constexpr auto SelectedFlag = 0xCC;
    }

    namespace StaticMoveableDataOffset
    {
        inline constexpr auto AnimationCount = 0;
        inline constexpr auto Animations = 4;
        inline constexpr auto WalkCount = 8;
        inline constexpr auto Walks = 12;
        inline constexpr auto AnimalGraphicsCount = 0x10;
        inline constexpr auto AnimalGraphics = 0x14;
        inline constexpr auto VehicleGraphicsCount = 0x18;
        inline constexpr auto VehicleGraphics = 0x1C;
    }

    namespace StaticMoveableManagerOffset
    {
        inline constexpr auto SequenceLibraryCount = 0;
        inline constexpr auto SequenceLibraries = 4;
        inline constexpr auto AnimationAccess = 8;
        inline constexpr auto WalkAccess = 0xDD30;
        inline constexpr auto FirstAnimationList = 0x10;
        inline constexpr auto FirstWalkList = 0xDD38;
    }

    namespace FastMoveableTribeOffset
    {
        inline constexpr auto Count = 0;
        inline constexpr auto Jobs = 4;
        inline constexpr auto JobsUnsigned = 4u;
        inline constexpr auto ReverseDestructionEnd = 0xDD2C;
    }

    namespace StaticMoveableSortOffset
    {
        inline constexpr auto ActionOrGood = 8;
        inline constexpr auto WalkSpeed = 12;
    }

    namespace FastMoveableJobOffset
    {
        inline constexpr auto Count = 0;
        inline constexpr auto Entries = 4;
    }

    namespace StaticAnimalVehicleGraphicsOffset
    {
        inline constexpr auto Tribe = 0;
        inline constexpr auto Type = 4;
        inline constexpr auto BodyBobManager = 8;
        inline constexpr auto ShadowBobManager = 12;
        inline constexpr auto PaletteCount = 0x10;
        inline constexpr auto PaletteIds = 0x14;
    }

    namespace StaticMoveableAnimationOffset
    {
        inline constexpr auto DirectionFrames = 0x24;
        inline constexpr auto Mode = 0x10;
        inline constexpr auto PartArray = 0x18;
        inline constexpr auto FrameArray = 0x20;
        inline constexpr auto DirectionFramePointers = 0x28;
    }

    namespace StaticMoveableWalkOffset
    {
        inline constexpr auto WalkFrames = 0x18;
        inline constexpr auto TurnFrames = 0x58;
        inline constexpr auto WalkFramePointers = 0x1C;
        inline constexpr auto TurnFramePointers = 0x5C;
    }

    namespace WeatherSectorManagerOffset
    {
        inline constexpr auto SectorWidth = 0;
        inline constexpr auto SectorHeight = 4;
        inline constexpr auto Sectors = 8;
    }

    namespace WeatherSectorOffset
    {
        inline constexpr auto RainDensity = 0;
        inline constexpr auto SnowDensity = 4;
        inline constexpr auto SandDensity = 8;
    }

    namespace InputSelectionMemberOffset
    {
        inline constexpr auto LandscapeMapPointX = 0xA9C;
        inline constexpr auto LandscapeMapPointY = 0xA9E;
        inline constexpr auto RoadBuildPointX = 0xAA4;
        inline constexpr auto RoadBuildPointY = 0xAA6;
        inline constexpr auto WallBuildPointX = 0xAA8;
        inline constexpr auto WallBuildPointY = 0xAAA;
    }

    namespace InputRoadWallPointStateOffset
    {
        inline constexpr auto RoadStartPointX = 0;
        inline constexpr auto RoadStartPointY = 2;
        inline constexpr auto WallStartPointX = 4;
        inline constexpr auto WallStartPointY = 6;
        inline constexpr auto TargetPoints = 8;
    }

    namespace InputTargetPointListOffset
    {
        inline constexpr auto Count = 0;
        inline constexpr auto Points = 4u;
    }

    namespace InputManagerOffset
    {
        inline constexpr auto MapHighlightingEnabled = 0xAB4;
        inline constexpr auto CursorMapPoint = 0xAB6;
        inline constexpr auto CursorMapPointY = 0xAB8;
        inline constexpr auto AcceptedTargetPresent = 0xABA;
        inline constexpr auto AcceptedTargetMapPoint = 0xABC;
        inline constexpr auto AcceptedTargetMapPointY = 0xABE;
        inline constexpr auto RoadStartPoint = 0xAC0;
        inline constexpr auto WallStartPoint = 0xAC4;
        inline constexpr auto TargetPointCount = 0xAC8;
        inline constexpr auto TargetPointsUnsigned = 0xACCu;

        inline constexpr auto InputMode = 0;
        inline constexpr auto SelectionType = 4;
        inline constexpr auto SingleHumanId = 8;
        inline constexpr auto SingleHumanUniqueId = 0xC;
        inline constexpr auto HumanGroupCount = 0x10;
        inline constexpr auto HumanGroup = 0x14;
        inline constexpr auto HumanGroupUniqueIds = 0x18;
        inline constexpr auto HumanGroupSecondId = 0x1C;
        inline constexpr auto HumanGroupSecondUniqueId = 0x20;
        inline constexpr auto SingleAnimalId = 0x974;
        inline constexpr auto SingleAnimalUniqueId = 0x978;
        inline constexpr auto SingleVehicleId = 0x97C;
        inline constexpr auto SingleVehicleUniqueId = 0x980;
        inline constexpr auto VehicleGroupCount = 0x984;
        inline constexpr auto VehicleGroup = 0x988;
        inline constexpr auto VehicleGroupUniqueIds = 0x98C;
        inline constexpr auto VehicleGroupSecondId = 0x990;
        inline constexpr auto VehicleGroupSecondUniqueId = 0x994;
        inline constexpr auto HouseId = 0xA78;
        inline constexpr auto HouseUniqueId = 0xA7C;
        inline constexpr auto HouseType = 0xA80;
        inline constexpr auto HouseDevelopmentStage = 0xA84;
        inline constexpr auto HouseSpecialItemType = 0xA88;
        inline constexpr auto HouseSpecialItem = 0xA8C;
        inline constexpr auto GuideId = 0xA90;
        inline constexpr auto GuideUniqueId = 0xA94;
        inline constexpr auto LandscapeType = 0xA98;
        inline constexpr auto LandscapeVariant = 0xA9C;
        inline constexpr auto LandscapeMapPoint = 0xAA0;
        inline constexpr auto LandscapePlayerId = 0xAA4;
        inline constexpr auto RoadBuildPoint = 0xAA8;
        inline constexpr auto WallBuildPoint = 0xAAC;
        inline constexpr auto SelectionChangeCount = 0xAB0;
        inline constexpr auto SingleHumanIdUnsigned = 8u;
        inline constexpr auto SingleHumanUniqueIdUnsigned = 0xCu;
        inline constexpr auto HumanGroupCountUnsigned = 0x10u;
        inline constexpr auto HumanGroupUnsigned = 0x14u;
        inline constexpr auto HumanGroupUniqueIdsUnsigned = 0x18u;
        inline constexpr auto SingleAnimalIdUnsigned = 0x974u;
        inline constexpr auto SingleAnimalUniqueIdUnsigned = 0x978u;
        inline constexpr auto SingleVehicleIdUnsigned = 0x97Cu;
        inline constexpr auto SingleVehicleUniqueIdUnsigned = 0x980u;
        inline constexpr auto VehicleGroupCountUnsigned = 0x984u;
        inline constexpr auto VehicleGroupUnsigned = 0x988u;
        inline constexpr auto VehicleGroupUniqueIdsUnsigned = 0x98Cu;
        inline constexpr auto HouseIdUnsigned = 0xA78u;
        inline constexpr auto HouseUniqueIdUnsigned = 0xA7Cu;
        inline constexpr auto HouseTypeUnsigned = 0xA80u;
        inline constexpr auto HouseDevelopmentStageUnsigned = 0xA84u;
        inline constexpr auto GuideIdUnsigned = 0xA90u;
        inline constexpr auto GuideUniqueIdUnsigned = 0xA94u;
        inline constexpr auto LandscapeTypeUnsigned = 0xA98u;
        inline constexpr auto LandscapeVariantUnsigned = 0xA9Cu;
    }

    namespace DisplayObjectReferenceOffset
    {
        inline constexpr auto HumanIdUnsigned = 8u;
        inline constexpr auto AnimalIdUnsigned = 0x10u;
        inline constexpr auto VehicleIdUnsigned = 0x18u;
        inline constexpr auto Type = 0;
        inline constexpr auto MapPosition = 4;
        inline constexpr auto MapPositionX = 4;
        inline constexpr auto MapPositionY = 6;
        inline constexpr auto HumanId = 8;
        inline constexpr auto HumanUniqueId = 12;
        inline constexpr auto AnimalId = 0x10;
        inline constexpr auto AnimalUniqueId = 0x14;
        inline constexpr auto VehicleId = 0x18;
        inline constexpr auto VehicleUniqueId = 0x1C;
        inline constexpr auto MapMoveableId = 0x20;
        inline constexpr auto MapMoveableUniqueId = 0x24;
        inline constexpr auto HouseId = 0x28;
        inline constexpr auto HouseUniqueId = 0x2C;
        inline constexpr auto GuideId = 0x30;
        inline constexpr auto GuideUniqueId = 0x34;
        inline constexpr auto LandscapeId = 0x38;
        inline constexpr auto LandscapeVariant = 0x3C;
        inline constexpr auto PlayerId = 0x40;
        inline constexpr auto MarkType = 0x44;
        inline constexpr auto BobData = 0x48;
    }

    namespace DisplayMarkingOffset
    {
        inline constexpr auto PrimaryReferenceEnabled = 0;
        inline constexpr auto PrimaryMarkedObject = 4;
        inline constexpr auto SecondaryMarkedObjectCount = 8;
        inline constexpr auto SecondaryMarkedObjects = 12;
        inline constexpr auto FirstSecondaryMapPositionX = 0x10;
        inline constexpr auto FirstSecondaryMapPositionY = 0x12;
    }

    namespace DisplayDescriptionOffset
    {
        inline constexpr auto VerticalSpacing = 0;
        inline constexpr auto HorizontalSpacing = 4;
        inline constexpr auto HeightScale = 8;
        inline constexpr auto MicroVerticalFixed = 0x0C;
        inline constexpr auto MicroHorizontalFixed = 0x10;
        inline constexpr auto MicroHeightFixed = 0x14;
        inline constexpr auto MicroHalfHorizontalFixed = 0x18;
        inline constexpr auto MicroHalfVerticalFixed = 0x1C;
        inline constexpr auto MicroVerticalPixels = 0x20;
        inline constexpr auto MicroHorizontalPixels = 0x24;
        inline constexpr auto MicroHeightPixels = 0x28;
        inline constexpr auto MicroHalfHorizontalPixels = 0x2C;
        inline constexpr auto MicroHalfVerticalPixels = 0x30;
        inline constexpr auto MacroVerticalFixed = 0x34;
        inline constexpr auto MacroHorizontalFixed = 0x38;
        inline constexpr auto MacroHeightFixed = 0x3C;
        inline constexpr auto MacroHalfHorizontalFixed = 0x40;
        inline constexpr auto MacroHalfVerticalFixed = 0x44;
        inline constexpr auto MacroVerticalPixels = 0x48;
        inline constexpr auto MacroHorizontalPixels = 0x4C;
        inline constexpr auto MacroHeightPixels = 0x50;
        inline constexpr auto MacroHalfHorizontalPixels = 0x54;
        inline constexpr auto MacroHalfVerticalPixels = 0x58;
        inline constexpr auto WorldWidthPixels = 0x5C;
        inline constexpr auto WorldHeightPixels = 0x60;
    }

    namespace EngineDisplayOffset
    {
        inline constexpr auto DrawCounters = 0x84;
        inline constexpr auto SeenLocalAdultMenUnsigned = 0x90u;
        inline constexpr auto SeenLocalAdultWomenUnsigned = 0x94u;
        inline constexpr auto SeenLocalChildrenUnsigned = 0x98u;
        inline constexpr auto VisitedMapPoints = 0x84;
        inline constexpr auto LandscapeDrawCount = 0x88;
        inline constexpr auto SeenHumans = 0x8C;
        inline constexpr auto SeenLocalAdultMen = 0x90;
        inline constexpr auto SeenLocalAdultWomen = 0x94;
        inline constexpr auto SeenLocalChildren = 0x98;
        inline constexpr auto SeenHouses = 0x9C;
        inline constexpr auto SeenDamagedHouses = 0xA0;

        inline constexpr auto PrimaryReferenceEnabled = 0xA4;
        inline constexpr auto PrimaryMarkedObject = 0xA8;
        inline constexpr auto SecondaryMarkedObjectCount = 0xAC;
        inline constexpr auto SecondaryMarkedObjects = 0xB0u;
        inline constexpr auto TooltipOverlaysEnabled = 0x920;

        inline constexpr auto ActiveDisplayFlag = 0x6C;
        inline constexpr auto ZoomCounter = 0x78;
        inline constexpr auto EarthquakeEndTime = 0x940;
        inline constexpr auto EarthquakeOffsetX = 0x944;
        inline constexpr auto EarthquakeOffsetY = 0x948;
        inline constexpr auto WantedWorldPixelX = 0x94C;
        inline constexpr auto WantedWorldPixelY = 0x950;
        inline constexpr auto BufferRectangle = 0x95C;
        inline constexpr auto StaticBitmap = 0x96C;
        inline constexpr auto StaticBitmapX = 0x970;
        inline constexpr auto StaticBitmapY = 0x974;
        inline constexpr auto StaticRebuildRequiredFlag = 0x978;
        inline constexpr auto MapRectangle = 0x97A;
        inline constexpr auto MapRectangleX = 0x97A;
        inline constexpr auto MapRectangleY = 0x97C;
        inline constexpr auto MapRectangleWidth = 0x97E;
        inline constexpr auto MapRectangleHeight = 0x980;
        inline constexpr auto MapMinimumX = 0x982;
        inline constexpr auto MapMaximumX = 0x984;
        inline constexpr auto MapMinimumY = 0x986;
        inline constexpr auto MapMaximumY = 0x988;
        inline constexpr auto MapRectanglePixelX = 0x98C;
        inline constexpr auto MapRectanglePixelY = 0x990;

        inline constexpr auto RainEnabled = 0x928;
        inline constexpr auto RainColorTable = 0x92C;
        inline constexpr auto SnowEnabled = 0x930;
        inline constexpr auto SnowColorTable = 0x934;
        inline constexpr auto SandEnabled = 0x938;
        inline constexpr auto SandColorTable = 0x93C;
        inline constexpr auto WorldPixelX = 0x954;
        inline constexpr auto WorldPixelY = 0x958;
        inline constexpr auto BufferWidth = 0x964;
        inline constexpr auto BufferHeight = 0x968;

        inline constexpr auto DestinationBitmap = 0x64;
        inline constexpr auto DrawingBitmap = 0x68;
        inline constexpr auto DrawFlags = 0x70;
        inline constexpr auto HighlightingEnabled = 0x74;
        inline constexpr auto PlayerId = 0x7C;
        inline constexpr auto GouraudShader = 0x80;
        inline constexpr auto Font = 0x924;
    }

    namespace PaletteOffset
    {
        inline constexpr auto Colors = 4;
        inline constexpr auto PlayerColorSampleWithinRange = 0x2C;
    }

    namespace StaticHumanMiscOffset
    {
        inline constexpr auto RandomPaletteSetCount = 0;
        inline constexpr auto RandomPaletteSets = 4;
        inline constexpr auto JobBaseGraphicsCount = 8;
        inline constexpr auto JobBaseGraphics = 12;
        inline constexpr auto JobChangeGraphicsCount = 0x10;
        inline constexpr auto JobChangeGraphics = 0x14;
        inline constexpr auto TribeNameArrays = 0x18;
        inline constexpr auto TribeNameArraysUnsigned = 0x18u;
        inline constexpr auto PlayerColors = 0x58;
        inline constexpr auto PlayerColorsUnsigned = 0x58u;
    }

    namespace HumanJobGraphicsOffset
    {
        inline constexpr auto Tribe = 0;
        inline constexpr auto Job = 4;
        inline constexpr auto BodyCount = 8;
        inline constexpr auto BodyChoices = 12;
        inline constexpr auto BodyChoicesHex = 0xC;
        inline constexpr auto BodyBobManagers = 0x10;
        inline constexpr auto ShadowBobManagers = 0x14;
        inline constexpr auto HeadCount = 0x48;
        inline constexpr auto HeadChoices = 0x4C;
        inline constexpr auto HeadBobManagers = 0x50;
    }

    namespace HumanJobBaseGraphicsOffset
    {
        inline constexpr auto BodyPaletteCount = 0x74;
        inline constexpr auto BodyPaletteCountUnsigned = 0x74u;
        inline constexpr auto BodyPaletteIds = 0x78;
        inline constexpr auto HeadPaletteCount = 0x8C;
        inline constexpr auto HeadPaletteCountUnsigned = 0x8Cu;
        inline constexpr auto HeadPaletteIds = 0x90;
        inline constexpr auto RandomPatchCount = 0xA4;
        inline constexpr auto RandomPatchCountUnsigned = 0xA4u;
        inline constexpr auto RandomPatchIds = 0xA8;
    }

    namespace HumanJobChangeGraphicsOffset
    {
        inline constexpr auto RandomPatchId = 0x74;
    }

    namespace TribeHumanNameArraysOffset
    {
        inline constexpr auto Male = 0;
        inline constexpr auto Female = 4;
        inline constexpr auto MaleUnsigned = 0u;
        inline constexpr auto FemaleUnsigned = 4u;
    }

    namespace HumanBodyGraphicsOffset
    {
        inline constexpr auto VariantId = 0;
        inline constexpr auto BodyBobManager = 4;
        inline constexpr auto ShadowBobManager = 8;
    }

    namespace RandomPaletteSetOffset
    {
        inline constexpr auto Name = 0;
        inline constexpr auto PatchCount = 0x28;
        inline constexpr auto Patches = 0x2C;
        inline constexpr auto DestinationRangeCount = 0x30;
        inline constexpr auto DestinationRanges = 0x34;
    }

    namespace RandomPalettePatchOffset
    {
        inline constexpr auto DestinationRange = 0;
        inline constexpr auto Palette16Id = 4;
        inline constexpr auto SourceRange = 8;
        inline constexpr auto Weight = 12;
    }

    namespace StaticHumanColorOffset
    {
        inline constexpr auto Color = 0;
        inline constexpr auto MaleRandomPatchId = 4;
        inline constexpr auto MaleRandomPatchIdUnsigned = 4u;
        inline constexpr auto FemaleRandomPatchId = 8;
        inline constexpr auto FemaleRandomPatchIdUnsigned = 8u;
        inline constexpr auto InhabitantSigns = 0xC;
        inline constexpr auto SoldierSigns = 0x34;
        inline constexpr auto SoldierSignsUnsigned = 0x34u;
        inline constexpr auto PlayerPaletteId = 0x50;
        inline constexpr auto PlayerPaletteIdUnsigned = 0x50u;
        inline constexpr auto ShipPaletteId = 0x54;
        inline constexpr auto RoadBuildSignLandscapeId = 0x48;
        inline constexpr auto WallBuildSignLandscapeId = 0x4C;
    }

    namespace GameControlOffset
    {
        inline constexpr auto DifficultyLevel = 0x5D200;
    }

    namespace GameControlPlayerOffset
    {
        inline constexpr auto AreaSearchCount = 0x4A24u;
        inline constexpr auto PointPathfinderStartCount = 0x4A28u;
        inline constexpr auto AreaSearchWithoutEarlyExitCount = 0x4A2Cu;
    }

    namespace GameMapOffset
    {
        inline constexpr auto MicroHeight = 0x02;
        inline constexpr auto MacroWidth = 0x04;
        inline constexpr auto MacroHeight = 0x06;
        inline constexpr auto MacroPointStorage = 0x30;
        inline constexpr auto MicroPointStorage = 0x34;
        inline constexpr auto PackingScratchBuffer = 0x38;
    }

    namespace HouseAttachmentOffset
    {
        inline constexpr auto RoleMask = 0;
        inline constexpr auto HumanId = 4;
        inline constexpr auto TrainingJobType = 8;
        inline constexpr auto TrainingGoodType = 12;
    }

    namespace HouseConstructionGoodOffset
    {
        inline constexpr auto GoodType = 0;
        inline constexpr auto RequiredAmount = 4;
        inline constexpr auto DeliveredAmount = 8;
        inline constexpr auto IncomingAmount = 12;
        inline constexpr auto ConsumedAmount = 16;
    }

    namespace HouseOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto HouseId = 4;
        inline constexpr auto UniqueId = 8;
        inline constexpr auto PlayerId = 0x0C;
        inline constexpr auto EngineType = 0x10;
        inline constexpr auto LogicType = 0x14;
        inline constexpr auto TribeType = 0x18;
        inline constexpr auto MapPosition = 0x1C;
        inline constexpr auto MapPositionX = 0x1C;
        inline constexpr auto MapPositionY = 0x1E;
        inline constexpr auto DevelopmentStage = 0x20;
        inline constexpr auto DoorEntryPoint = 0x24;
        inline constexpr auto DoorEntryPointX = 0x24;
        inline constexpr auto DoorEntryPointY = 0x26;
        inline constexpr auto MappedFlag = 0x28;
        inline constexpr auto ConstructionState = 0x2C;
        inline constexpr auto ConstructionStepsDone = 0x30;
        inline constexpr auto ConstructionStepsRequired = 0x34;
        inline constexpr auto ConstructionGoodsCount = 0x38;
        inline constexpr auto ConstructionGoodEntries = 0x3Cu;
        inline constexpr auto ConstructionGoodEntriesSigned = 0x3C;
        inline constexpr auto ConstructionGoodRequiredAmounts = 0x40u;
        inline constexpr auto ConstructionGoodsRequired = 0x104;
        inline constexpr auto ConstructionGoodsDelivered = 0x108;
        inline constexpr auto ConstructionGoodsConsumed = 0x10C;
        inline constexpr auto ConstructionStepLimit = 0x110;
        inline constexpr auto BuilderRecruitmentCounter = 0x114;
        inline constexpr auto StockAmounts = 0x118u;
        inline constexpr auto StockFractionalAmounts = 0x220u;
        inline constexpr auto FutureStockAmounts = 0x328u;
        inline constexpr auto StockRequestCounters = 0x430u;
        inline constexpr auto StockMinimumAmounts = 0x538u;
        inline constexpr auto Attachments = 0x640u;
        inline constexpr auto AttachmentHumanIds = 0x644u;
        inline constexpr auto HomeQualityEnhancers = 0x780;
        inline constexpr auto HomeQualityEnhancersUnsigned = 0x780u;
        inline constexpr auto HomeQualityEnhancer0Value = 0x784;
        inline constexpr auto HomeQualityEnhancer0ValueUnsigned = 0x784u;
        inline constexpr auto HomeQualityEnhancer1Allowed = 0x788;
        inline constexpr auto HomeQualityEnhancer1Value = 0x78C;
        inline constexpr auto HomeQualityEnhancer2Allowed = 0x790;
        inline constexpr auto HomeQualityEnhancer2Value = 0x794;
        inline constexpr auto ProductionStoppedFlag = 0x798;
        inline constexpr auto ProductionCounter = 0x79C;
        inline constexpr auto PreviousSectorHouseId = 0x7A0;
        inline constexpr auto NextSectorHouseId = 0x7A4;
        inline constexpr auto ActionOverlayCounters = 0x7A8u;
        inline constexpr auto LandscapeOverlayActiveFlags = 0x7BCu;
        inline constexpr auto LandscapeOverlayActiveFlagsSigned = 0x7BC;
        inline constexpr auto Hitpoints = 0x7C8;
        inline constexpr auto MissionCheckId = 0x7CC;
        inline constexpr auto DemolitionFlag = 0x7D0;
        inline constexpr auto DemolitionStepsDone = 0x7D2;
        inline constexpr auto DemolitionStepsRequired = 0x7D4;
        inline constexpr auto DefenceModeFlag = 0x7D8;
        inline constexpr auto BehaviourFlags = 0x7DC;
    }

    namespace HumanOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto HumanId = 4;
        inline constexpr auto UniqueId = 8;
        inline constexpr auto MapMoveableId = 0xC;
        inline constexpr auto PlayerId = 0x10;
        inline constexpr auto TribeType = 0x14;
        inline constexpr auto JobType = 0x18;
        inline constexpr auto ProducedGoodType = 28;
        inline constexpr auto CollectionGoodType = 0x20;
        inline constexpr auto JobExperience = 0x24u;
        inline constexpr auto JobExperienceSigned = 0x24;
        inline constexpr auto EducationLevel = 0x158;
        inline constexpr auto AllowedJobFlags = 0x15Cu;
        inline constexpr auto AllowedJobFlagsSigned = 0x15C;
        inline constexpr auto ProductionSelectionMode = 0x194;
        inline constexpr auto AllowedProductionGoodFlags = 0x195u;
        inline constexpr auto AllowedEquipmentFlags = 0x1E0u;
        inline constexpr auto AllowedEquipmentFlagsSigned = 0x1E0;
        inline constexpr auto ProductionRotationIndex = 0x218;
        inline constexpr auto ProductionCounters = 0x21Cu;
        inline constexpr auto CurrentTask = 0x368;
        inline constexpr auto CurrentTaskStatus = 0x36C;
        inline constexpr auto CurrentTaskParameter = 0x370;
        inline constexpr auto CurrentTaskStep = 0x374;
        inline constexpr auto WaitingHumanCount = 0x378;
        inline constexpr auto IdleStartCount = 0x37C;
        inline constexpr auto TaskFailureCount = 0x380;
        inline constexpr auto MapPositionReachedCount = 0x384;
        inline constexpr auto WorkRetriesRequired = 0x388;
        inline constexpr auto WorkRetryCount = 0x38C;
        inline constexpr auto UserCommandQueue = 0x390u;
        inline constexpr auto LastUserCommandIndex = 0xA5C;
        inline constexpr auto UserCommandActiveFlag = 0xA60;
        inline constexpr auto UrgentNeedsCheckedFlag = 0xA61;
        inline constexpr auto InteractionTarget = 0xA68;
        inline constexpr auto TargetPosition = 0xA6C;
        inline constexpr auto TargetPositionX = 0xA6C;
        inline constexpr auto TargetPositionY = 0xA6E;
        inline constexpr auto TargetWorkPosition = 0xA70;
        inline constexpr auto TargetWorkPositionX = 0xA70;
        inline constexpr auto TargetWorkPositionY = 0xA72;
        inline constexpr auto TargetLandscapeType = 0xA74;
        inline constexpr auto TargetGoodType = 0xA78;
        inline constexpr auto TargetObjectId = 0xA7C;
        inline constexpr auto TargetObjectRevision = 0xA80;
        inline constexpr auto SecondaryInteractionTarget = 0xA84;
        inline constexpr auto SecondaryTargetPosition = 0xA88;
        inline constexpr auto SecondaryTargetWorkPosition = 0xA8C;
        inline constexpr auto SecondaryTargetLandscapeType = 0xA90;
        inline constexpr auto SecondaryTargetGoodType = 0xA94;
        inline constexpr auto SecondaryTargetObjectId = 0xA98;
        inline constexpr auto SecondaryTargetObjectRevision = 0xA9C;
        inline constexpr auto Hitpoints = 0xAA0;
        inline constexpr auto AgeCounter = 0xAA4;
        inline constexpr auto Food = 0xAA8;
        inline constexpr auto Stamina = 0xAAC;
        inline constexpr auto Social = 0xAB0;
        inline constexpr auto Religion = 0xAB4;
        inline constexpr auto AtomicAction = 0xAB8;
        inline constexpr auto AtomicAnimationRunningFlag = 0xABC;
        inline constexpr auto AtomicAnimationId = 0xAC0;
        inline constexpr auto AtomicAnimationFrame = 0xAC4;
        inline constexpr auto AtomicAnimationStartCount = 0xAC8;
        inline constexpr auto GoodCirculationMode = 0xACC;
        inline constexpr auto RequestedGood = 0xAD0;
        inline constexpr auto CarriedGood = 0xAD4;
        inline constexpr auto SuspendedGoodCirculationMode = 0xAD8;
        inline constexpr auto ShoeCondition = 0xADC;
        inline constexpr auto ToolCondition = 0xAE0;
        inline constexpr auto EquippedWeapon = 0xAE4;
        inline constexpr auto EquippedArmor = 0xAE8;
        inline constexpr auto EquippedShoes = 0xAEC;
        inline constexpr auto EquippedTools = 0xAF0;
        inline constexpr auto InventorySlots = 0xAF4u;
        inline constexpr auto InventorySlotsSigned = 0xAF4;
        inline constexpr auto InventoryConditions = 0xAF8u;
        inline constexpr auto FatherId = 0xB14;
        inline constexpr auto MotherId = 0xB18;
        inline constexpr auto WaitingHumanId = 0xB1C;
        inline constexpr auto SpouseId = 0xB20;
        inline constexpr auto ChildId = 0xB24;
        inline constexpr auto HomeHouseId = 0xB28;
        inline constexpr auto HomeHouseRevision = 0xB2C;
        inline constexpr auto WorkCenter = 0xB30;
        inline constexpr auto WorkCenterX = 0xB30;
        inline constexpr auto WorkCenterY = 0xB32;
        inline constexpr auto WorkAttachmentHouseId = 0xB34;
        inline constexpr auto WorkAttachmentHouseRevision = 0xB38;
        inline constexpr auto VehicleConstructionHouseId = 0xB3C;
        inline constexpr auto VehicleConstructionWorkshopRevision = 0xB40;
        inline constexpr auto MerchantId = 0xB44;
        inline constexpr auto AttachedVehicleId = 0xB48;
        inline constexpr auto AttachedVehicleRevision = 0xB4C;
        inline constexpr auto InsideHouseFlag = 0xB50;
        inline constexpr auto InsideVehicleFlag = 0xB51;
        inline constexpr auto PreviousSectorHumanId = 0xB54;
        inline constexpr auto NextSectorHumanId = 0xB58;
        inline constexpr auto MilitaryMode = 0xB5C;
        inline constexpr auto MilitaryState = 0xB5C;
        inline constexpr auto MilitaryWorldRefreshmentFlag = 0xB60;
        inline constexpr auto PrimaryMilitaryTarget = 0xB64;
        inline constexpr auto PrimaryMilitaryTargetUnsigned = 0xB64u;
        inline constexpr auto PrimaryMilitaryTargetValue = 0xB68;
        inline constexpr auto PrimaryMilitaryTargetRevision = 0xB6C;
        inline constexpr auto SecondaryMilitaryTarget = 0xB70;
        inline constexpr auto SecondaryMilitaryTargetUnsigned = 0xB70u;
        inline constexpr auto ExplorationRadius = 0xB7C;
        inline constexpr auto MissionCheckId = 0xB80;
        inline constexpr auto MiscFlags = 0xB84;
        inline constexpr auto AssistantData = 0xB88;
        inline constexpr auto AssistantHasCommandHistoryFlag = 0xB8C;
    }

    namespace MapMoveableOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto MoveableId = 4;
        inline constexpr auto ObjectType = 8;
        inline constexpr auto RemovalRequestedFlag = 0x0C;
        inline constexpr auto AttachedToMapFlag = 0x10;
        inline constexpr auto MapPosition = 0x12;
        inline constexpr auto MapPositionY = 0x14;
        inline constexpr auto SectorPosition = 0x18;
        inline constexpr auto SectorPositionY = 0x1C;
        inline constexpr auto Direction = 0x20;
        inline constexpr auto NextCellMoveableId = 0x24;
        inline constexpr auto PathfinderId = 0x28;
        inline constexpr auto WalkingFlag = 0x2C;
        inline constexpr auto WalkingDirection = 0x30;
        inline constexpr auto WalkingProgress = 0x34;
        inline constexpr auto WalkingPositionAdvancedFlag = 0x38;
        inline constexpr auto TurningFlag = 0x39;
        inline constexpr auto TurningElapsedTicks = 0x3C;
        inline constexpr auto TurnTargetDirection = 0x40;
        inline constexpr auto WalkingTicksPerStep = 0x44;
        inline constexpr auto WalkingProgressPerTick = 0x48;
        inline constexpr auto TurningTicksPerDirection = 0x4C;
        inline constexpr auto IncludeVerticalDirectionsFlag = 0x50;
        inline constexpr auto ObjectId = 0x54;
    }

    namespace HumanInventorySlotOffset
    {
        inline constexpr auto GoodType = 0;
        inline constexpr auto PercentageCondition = 4;
    }

    namespace UserNetworkCommandOffset
    {
        inline constexpr auto Opcode = 0;
        inline constexpr auto PlayerId = 1;
        inline constexpr auto ObjectId = 4;
        inline constexpr auto ObjectRevision = 8;
        inline constexpr auto Parameter1 = 12;
        inline constexpr auto Parameter2 = 16;
    }

    namespace ObjectSize
    {
        inline constexpr auto InputManager = 3964;
        inline constexpr auto EmbeddedSavegameManager = 0x8;
        inline constexpr auto MacroMapManager = 0x8;
        inline constexpr auto E2MapWeatherManagerSectorManager = 0xC;
        inline constexpr auto E2StaticDataTexturesManager = 0x14;
        inline constexpr auto E2StaticDataBobManagersManager = 0x1C;
        inline constexpr auto MapMoveableArray = 0x30;
        inline constexpr auto MapPathfinderManager = 0x30;
        inline constexpr auto MarkerArray = 0x30;
        inline constexpr auto DelayedActionManager = 0x34;
        inline constexpr auto AmbientSoundManager = 0x64;
        inline constexpr auto AIManager = 0xA0;
        inline constexpr auto PlayerStatisticManager = 0xA8;
        inline constexpr auto GameAssistantManager = 0x460;
        inline constexpr auto HumanMerchantArray = 0x5D8;
        inline constexpr auto MultiPlayerGoalManager = 0x740;
        inline constexpr auto TributeManager = 0x960;
        inline constexpr auto UserNetworkCommandManager = 0x9A0;
        inline constexpr auto HappinessManager = 0xAA0;
        inline constexpr auto E2HumanSoundManager = 0xD24;
        inline constexpr auto MissionOnscreenInfoManager = 0xFA0;
        inline constexpr auto E2StaticDataLandscapesManager = 0xFA4;
        inline constexpr auto E2StaticDataPatternManager = 0xFA4;
        inline constexpr auto GuideManager = 0x1368;
        inline constexpr auto E2StaticDataMapMoveablesManager = 0x1BA58;
        inline constexpr auto GameControl = 0x5D204;
        inline constexpr auto MissionManager = 0x81CAC;
        inline constexpr auto E2ParticelManager = 0x2F7F80;
    }


    // Windows particle records and pool storage; offsets are relative to the indicated record.
    namespace ParticleOffset
    {
        inline constexpr auto SpeedMapX = 0x98;
        inline constexpr auto SpeedHeight = 0xA0;
        inline constexpr auto SpeedMapY = 0xA8;
        inline constexpr auto MovementMode = 0x90;
        inline constexpr auto UseWind = 0xF8;
        inline constexpr auto RemainingLifetimeTicks = 0x14;
        inline constexpr auto Active = 4;
        inline constexpr auto PendingMovementTicks = 0x110;
        inline constexpr auto ValencyIsDirection = 0x18;
        inline constexpr auto StaticParticleId = 0x128;
        inline constexpr auto FrameIndex = 0x124;
        inline constexpr auto Valency = 0x11C;
        inline constexpr auto Position = 0x28;
        inline constexpr auto PositionMapY = 0x30;
        inline constexpr auto UseMapHeights = 0x20;
        inline constexpr auto CachedGroundHeight = 0x80;
        inline constexpr auto PositionHeight = 0x38;
        inline constexpr auto PositionPixelX = 0x40;
        inline constexpr auto PositionPixelY = 0x48;
        inline constexpr auto MapPoint = 0x78;
        inline constexpr auto TargetPosition = 80;
        inline constexpr auto TargetHeight = 0x60;
        inline constexpr auto TargetPixelX = 0x68;
        inline constexpr auto TargetPixelY = 0x70;
        inline constexpr auto TargetMapY = 0x58;
        inline constexpr auto VectorMapX = 0xB0;
        inline constexpr auto VectorHeight = 0xB8;
        inline constexpr auto VectorMapY = 0xC0;
        inline constexpr auto ParabolaBaselineHeight = 0x108;
        inline constexpr auto TotalLifetimeTicks = 0x10;
        inline constexpr auto StepMapX = 0xC8;
        inline constexpr auto StepHeight = 0xD0;
        inline constexpr auto StepMapY = 0xD8;
        inline constexpr auto StepPixelX = 0xE0;
        inline constexpr auto StepPixelY = 0xE8;
        inline constexpr auto MaximumValency = 0x120;
        inline constexpr auto ValencyMode = 0x118;
        inline constexpr auto KillOnGroundCollision = 0xF9;
        inline constexpr auto FadingSpawn = 0x12D;
        inline constexpr auto DrawBottom = 0x12C;
        inline constexpr auto ParabolaHeight = 0x100;
        inline constexpr auto InitialDirectionValency = 0x1C;
        inline constexpr auto PlayerId = 0;
        inline constexpr auto StoredWindParameter = 0xF0;
        inline constexpr auto PreviousWorldPixelPoint = 136;
    }

    namespace ParticlePositionOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 8;
        inline constexpr auto Height = 0x10;
        inline constexpr auto PixelX = 0x18;
        inline constexpr auto PixelY = 0x20;
    }

    namespace ParticleWindOffset
    {
        inline constexpr auto TicksSinceChange = 0x54;
        inline constexpr auto AngleDegrees = 0x50;
        inline constexpr auto Cosine = 0;
        inline constexpr auto Sine = 8;
        inline constexpr auto HorizontalSpeed = 0x48;
        inline constexpr auto VerticalSpeed = 0x4C;
        inline constexpr auto HorizontalScale = 0x18;
        inline constexpr auto VerticalScale = 0x20;
        inline constexpr auto DeltaMapX = 0x30;
        inline constexpr auto DeltaMapY = 0x38;
    }

    namespace StaticParticleOffset
    {
        inline constexpr auto Graphics = 0x34;
        inline constexpr auto AnimationLoops = 0x30;
        inline constexpr auto MaximumValency = 0x38;
        inline constexpr auto SpawnParticleId = 0x24;
        inline constexpr auto ValencyIsDirection = 0x31;
        inline constexpr auto HasSpawnParticle = 0x20;
        inline constexpr auto BobManager = 0x28;
        inline constexpr auto PaletteId = 0x2C;
        inline constexpr auto SpecialType = 0x44;
        inline constexpr auto BobType = 0x40;
    }

    namespace ParticleFramesOffset
    {
        inline constexpr auto FrameCount = 4;
        inline constexpr auto FrameIds = 8;
        inline constexpr auto MinimumValency = 0;
    }

    namespace ParticleGeneratorOffset
    {
        inline constexpr auto AreaWidth = 0x5C;
        inline constexpr auto AreaHeight = 0x60;
        inline constexpr auto UseRandomArea = 0x58;
        inline constexpr auto ParticleId = 0x20;
        inline constexpr auto MovementMode = 0x30;
        inline constexpr auto SpeedMapX = 0x34;
        inline constexpr auto SpeedHeight = 0x38;
        inline constexpr auto SpeedMapY = 0x3C;
        inline constexpr auto UseMapHeights = 0x40;
        inline constexpr auto UseWind = 0x48;
        inline constexpr auto StoredWindParameter = 0x50;
        inline constexpr auto LifetimeTicks = 0x24;
        inline constexpr auto MaximumGoneLifetime = 0x28;
        inline constexpr auto ValencyMode = 0x2C;
        inline constexpr auto HeightOffset = 0x44;
        inline constexpr auto KillOnGroundCollision = 0x64;
    }

    namespace ParticleEffectOffset
    {
        inline constexpr auto ParticleId = 0;
        inline constexpr auto UseMapHeights = 0x1C;
        inline constexpr auto UseWind = 0x1D;
        inline constexpr auto MovementMode = 0xC;
        inline constexpr auto LifetimeTicks = 4;
        inline constexpr auto StartLifetimeTicks = 8;
        inline constexpr auto SpeedMapX = 0x10;
        inline constexpr auto SpeedHeight = 0x14;
        inline constexpr auto SpeedMapY = 0x18;
        inline constexpr auto MapOffsetX = 0x2E;
        inline constexpr auto MapOffsetY = 0x30;
        inline constexpr auto PixelPoint = 0x24;
        inline constexpr auto HeightOffset = 0x20;
        inline constexpr auto DrawBottom = 0x2C;
        inline constexpr auto PixelY = 0x28;
    }

    namespace ParticleGeneratorSlotOffset
    {
        inline constexpr auto ChancePercent = 0xC;
        inline constexpr auto PixelPoint = 4;
        inline constexpr auto PixelY = 8;
        inline constexpr auto Enabled = 0x14;
        inline constexpr auto GeneratorId = 0x10;
        inline constexpr auto Identifier = 0x18;
        inline constexpr auto MapPoint = 0;
        inline constexpr auto Active = 0x1C;
    }

    namespace ParticleGraphicsOffset
    {
        inline constexpr auto EntryCount = 0;
        inline constexpr auto Entries = 8;
    }

    namespace ParticleGeneratorManagerOffset
    {
        inline constexpr auto Count = 0;
        inline constexpr auto Entries = 4;
    }


    // Windows macro-map storage and section records.
    namespace MacroMapSectionLimits
    {
        inline constexpr auto Count = 13;
    }

    namespace MacroMapByteValueOffset
    {
        inline constexpr auto Value = 4;
    }

    namespace MacroMapOffset
    {
        inline constexpr auto Initialized = 0;
        inline constexpr auto Width = 0x106;
        inline constexpr auto Height = 0x108;
        inline constexpr auto CenterX = 0x10A;
        inline constexpr auto CenterY = 0x10C;
        inline constexpr auto PatternsCount = 0x118;
        inline constexpr auto Patterns = 0x11c;
        inline constexpr auto PatternNames = 0x114;
        inline constexpr auto TransitionsCount = 0x128;
        inline constexpr auto Transitions = 0x12c;
        inline constexpr auto TransitionNames = 0x124;
        inline constexpr auto LandscapesCount = 0x138;
        inline constexpr auto Landscapes = 0x13c;
        inline constexpr auto LandscapeNames = 0x134;
        inline constexpr auto HeightsCount = 0x140;
        inline constexpr auto Heights = 0x144;
        inline constexpr auto VertexColorsCount = 0x148;
        inline constexpr auto VertexColors = 0x14c;
        inline constexpr auto RoadsCount = 0x150;
        inline constexpr auto Roads = 0x154;
        inline constexpr auto HumansCount = 0x158;
        inline constexpr auto Humans = 0x15c;
        inline constexpr auto AnimalsCount = 0x160;
        inline constexpr auto Animals = 0x164;
        inline constexpr auto HousesCount = 0x168;
        inline constexpr auto Houses = 0x16c;
        inline constexpr auto FishSwarmsCount = 0x170;
        inline constexpr auto FishSwarms = 0x174;
        inline constexpr auto PatternNamesCount = 0x110;
        inline constexpr auto TransitionNamesCount = 0x120;
        inline constexpr auto LandscapeNamesCount = 0x130;
        inline constexpr auto Filename = 1;
    }

    namespace MacroMapManagerOffset
    {
        inline constexpr auto Count = 0;
        inline constexpr auto Entries = 4;
    }

    namespace MacroMapNameOffset
    {
        inline constexpr auto StaticDataId = 40;
    }

    namespace MacroMapHouseOffset
    {
        inline constexpr auto PlayerId = 0;
        inline constexpr auto MapX = 4;
        inline constexpr auto MapY = 6;
        inline constexpr auto HouseType = 8;
        inline constexpr auto DevelopmentStage = 12;
        inline constexpr auto Completed = 16;
    }

    namespace MacroMapCreatureOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
        inline constexpr auto Type = 4;
        inline constexpr auto TribeType = 8;
        inline constexpr auto PlayerId = 12;
    }

    namespace MacroMapPatternOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
        inline constexpr auto NameIndex = 8;
        inline constexpr auto TriangleA = 4;
    }

    namespace MacroMapTransitionOffset
    {
        inline constexpr auto Layer = 4;
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
        inline constexpr auto TriangleA = 8;
        inline constexpr auto NameIndex = 12;
    }

    namespace MacroMapLandscapeOffset
    {
        inline constexpr auto MapY = 2;
        inline constexpr auto NameIndex = 4;
        inline constexpr auto MapX = 0;
        inline constexpr auto Valency = 8;
        inline constexpr auto PlayerId = 12;
    }

    namespace MacroMapPointOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
    }

    namespace MacroMapFishSwarmOffset
    {
        inline constexpr auto FishCount = 4;
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
    }

    namespace MacroMapHeightOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
        inline constexpr auto Height = 4;
    }

    namespace MacroMapVertexColorOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
        inline constexpr auto ColorIndex = 4;
    }

    namespace MacroMapRoadOffset
    {
        inline constexpr auto MapX = 0;
        inline constexpr auto MapY = 2;
    }


    // Windows AI owners, task records and soldier/military storage.
    namespace AISinglePlayerOffset
    {
        inline constexpr auto SoldierCount = 0x6168;
        inline constexpr auto PlayerId = 0;
        inline constexpr auto CombinedTaskCount = 0x2A48;
        inline constexpr auto FirstCombinedTaskMainIndices = 0x2A64;
        inline constexpr auto CombinedTasks = 0x2A50;
        inline constexpr auto FirstCombinedTaskPhase = 0x2A98;
        inline constexpr auto VehicleCount = 0x5CB4;
        inline constexpr auto Vehicles = 0x5CB8;
        inline constexpr auto AllocatedSoldierCounts = 0xA500;
        inline constexpr auto SoldierAssignedFlags = 0xAC70;
        inline constexpr auto SoldierCandidateCount = 0xA62C;
        inline constexpr auto SoldierCandidateScores = 0xA634;
        inline constexpr auto SoldierCandidateSlots = 0xA630;
        inline constexpr auto TaskAllocationFinished = 0xA5FA;
        inline constexpr auto TaskAllocationWeighted = 0xA5C8;
        inline constexpr auto TaskAllocationEligible = 0xA338;
        inline constexpr auto TaskAllocationPriorityPercent = 0xA370;
        inline constexpr auto DefaultSoldierPositionEnabled = 0xA32C;
        inline constexpr auto DefaultSoldierPosition = 0xA32E;
        inline constexpr auto DefaultSoldierRadius = 0xA334;
        inline constexpr auto Soldiers = 0x616C;
        inline constexpr auto SoldierSelfDefenceOrigins = 0xAD48;
        inline constexpr auto WeaponCategoryCounts = 0xA30C;
        inline constexpr auto FirstMainTaskDescription = 0x1C38;
        inline constexpr auto TowerAttachmentCursor = 0xAD40;
        inline constexpr auto LastAttackedTick = 0xAD44;
        inline constexpr auto SoldierSelfDefenceOriginsUnsigned = 0xAD48u;
        inline constexpr auto LastMainTaskIndex = 0x1C2C;
        inline constexpr auto SoldiersUnsigned = 0x616Cu;
        inline constexpr auto CombinedTasksUnsigned = 0x2A50u;
        inline constexpr auto MainTasksUnsigned = 0x1C30u;
        inline constexpr auto VehiclesUnsigned = 0x5CB8u;
        inline constexpr auto ActiveMainTaskCount = 0x2978;
        inline constexpr auto ActiveMainTaskIndices = 0x297C;
        inline constexpr auto SecondActiveMainTaskIndex = 0x2980;
        inline constexpr auto BuildHouseCount = 0x3D60;
        inline constexpr auto BuildHouseTypes = 0x3D64;
        inline constexpr auto BuildHousePositions = 0x3D68;
        inline constexpr auto BuildHouseStages = 0x3D6C;
        inline constexpr auto BuildHouseMissionCheckIds = 0x3D70;
        inline constexpr auto FirstCombinedTaskPriority = 0x2A54;
        inline constexpr auto FirstCombinedTaskPriorityPercent = 0x2A58;
        inline constexpr auto FirstMainTaskDescriptionUnsigned = 0x1C38u;
        inline constexpr auto MainTasksChanged = 6;
        inline constexpr auto MainTasks = 0x1C30;
        inline constexpr auto FirstMainTaskPosition = 0x1C3C;
        inline constexpr auto BuildHousePositionsUnsigned = 0x3D68u;
        inline constexpr auto BuildHouseTypesUnsigned = 0x3D64u;
        inline constexpr auto BuildHouseStagesUnsigned = 0x3D6Cu;
        inline constexpr auto BuildHouseMissionCheckIdsUnsigned = 0x3D70u;
    }

    namespace AISoldierOffset
    {
        inline constexpr auto HumanId = 0;
        inline constexpr auto HumanUniqueId = 4;
        inline constexpr auto TaskDescription = 20;
        inline constexpr auto CurrentAction = 52;
        inline constexpr auto WeaponType = 8;
        inline constexpr auto ArmorType = 12;
        inline constexpr auto CombinedTaskIndex = 0x38;
        inline constexpr auto UsesDefaultTask = 0x10;
        inline constexpr auto MovementDestination = 60;
        inline constexpr auto MovementRadius = 64;
        inline constexpr auto TaskRadius = 28;
        inline constexpr auto TaskMilitaryMode = 44;
        inline constexpr auto TaskMapPoint = 24;
        inline constexpr auto AttackTarget = 0x48;
        inline constexpr auto SelfDefence = 0x44;
        inline constexpr auto AttackTargetUniqueId = 0x50;
        inline constexpr auto AttackTargetId = 0x4C;
    }

    namespace AIVehicleOffset
    {
        inline constexpr auto VehicleId = 0;
        inline constexpr auto VehicleUniqueId = 4;
        inline constexpr auto TaskIndex = 8;
        inline constexpr auto AttackActive = 12;
        inline constexpr auto AttackTargetUniqueId = 24;
        inline constexpr auto AttackTargetId = 20;
        inline constexpr auto AttackTarget = 16;
    }

    namespace AICombinedTaskOffset
    {
        inline constexpr auto MainTaskIndices = 0x14;
        inline constexpr auto TaskType = 0;
        inline constexpr auto AttackPhase = 0x48;
        inline constexpr auto Radius = 0x44;
        inline constexpr auto FormationRadius = 0x4C;
        inline constexpr auto MaximumSoldiers = 0x40;
        inline constexpr auto MinimumSoldiers = 0x3C;
        inline constexpr auto PriorityPercent = 8;
        inline constexpr auto FormationPosition = 0x50;
        inline constexpr auto Priority = 4;
        inline constexpr auto MainTaskCount = 0x10;
        inline constexpr auto FormationPositionY = 0x52;
        inline constexpr auto NextTaskPriority = 0x5C;
    }

    namespace AIMainTaskOffset
    {
        inline constexpr auto Description = 8;
        inline constexpr auto PrimaryPosition = 12;
        inline constexpr auto PrimaryPositionY = 14;
        inline constexpr auto Radius = 16;
        inline constexpr auto ConditionId = 0x2C;
        inline constexpr auto Disabled = 0;
        inline constexpr auto ConfiguredPriority = 40;
        inline constexpr auto ActivePriority = 4;
        inline constexpr auto CreatedTick = 0x30;
        inline constexpr auto SecondaryPosition = 28;
        inline constexpr auto SecondaryPositionY = 30;
        inline constexpr auto MilitaryMode = 32;
        inline constexpr auto MinimumSoldiers = 20;
        inline constexpr auto MaximumSoldiers = 24;
    }

    namespace AITaskDescriptionOffset
    {
        inline constexpr auto Type = 0;
        inline constexpr auto MapPoint = 4;
        inline constexpr auto MapPointY = 6;
        inline constexpr auto MinimumSoldiers = 0xC;
        inline constexpr auto MaximumSoldiers = 0x10;
        inline constexpr auto Radius = 8;
        inline constexpr auto FormationPosition = 0x14;
        inline constexpr auto FormationPositionY = 0x16;
    }

    namespace AIMilitaryManagerOffset
    {
        inline constexpr auto WeaponSeventeenGood = 0x1528;

        inline constexpr auto PlayerId = 0;
        inline constexpr auto ArmorGoods = 0x1C9C;
        inline constexpr auto ArmorPreferredWeapon = 0x1CA0;
        inline constexpr auto EnemyScanCursor = 0x1F54;
        inline constexpr auto TargetedEnemyPlayer = 0x1F58;
        inline constexpr auto TargetedEnemyMilitaryPointIndex = 0x1F5C;
        inline constexpr auto WeaponWeights = 0x1458;
        inline constexpr auto ArmorWeights = 0x1C98;
        inline constexpr auto ArmorTwoGood = 0x1CB4;
        inline constexpr auto ArmorOneGood = 0x1CA8;
        inline constexpr auto WeaponGoods = 0x145C;
        inline constexpr auto WeaponFourGood = 0x148C;
        inline constexpr auto WeaponPreferredArmor = 0x1460;
        inline constexpr auto AttackedPositionMilitaryPointIndex = 0x1F64;
        inline constexpr auto EnemyTargets = 0x1CD4;
        inline constexpr auto AttackedPosition = 0x1F60;
        inline constexpr auto AttackedPositionY = 0x1F62;
    }

    namespace AIHighLevelOffset
    {
        inline constexpr auto FirstVillageCenter = 52;
        inline constexpr auto Villages = 0x1C;
        inline constexpr auto AnimalCatcher = 0x94E8;
        inline constexpr auto FirstVillageActive = 36;
        inline constexpr auto Trade = 0x74D4;
        inline constexpr auto Military = 0x7578;
    }

    namespace AIMilitaryPointOffset
    {
        inline constexpr auto TargetUniqueId = 8;
        inline constexpr auto TargetIdOrMapPoint = 4;
        inline constexpr auto Target = 0;
        inline constexpr auto TargetPolicy = 12;
        inline constexpr auto MainTaskIndex = 20;
        inline constexpr auto LifetimePolicy = 16;
        inline constexpr auto PlayerId = 28;
        inline constexpr auto Radius = 24;
    }

    namespace AIMilitarySlotAccessOffset
    {
        inline constexpr auto Active = 4;
        inline constexpr auto Role = 8;
        inline constexpr auto TargetPolicy = 24;
        inline constexpr auto MaximumSoldiers = 48;
        inline constexpr auto MinimumSoldiers = 44;
        inline constexpr auto Radius = 36;
        inline constexpr auto MainTaskIndex = 32;
        inline constexpr auto Point = 12;
        inline constexpr auto SecondaryType = 52;
    }


    namespace AIBuildHouseTaskOffset
    {
        inline constexpr auto Position = 12;
        inline constexpr auto PositionY = 14;
        inline constexpr auto HouseType = 16;
        inline constexpr auto DevelopmentStage = 20;
        inline constexpr auto RequestedCount = 24;
    }

    namespace AIDiplomacyTaskOffset
    {
        inline constexpr auto OtherPlayerId = 16;
        inline constexpr auto DiplomacyState = 12;
    }

    namespace AISpawnHumansTaskOffset
    {
        inline constexpr auto HumanCount = 24;
        inline constexpr auto Position = 12;
        inline constexpr auto PositionY = 14;
        inline constexpr auto TribeType = 16;
        inline constexpr auto JobType = 20;
        inline constexpr auto Height = 32;
        inline constexpr auto DisableAfterSpawn = 28;
    }

    namespace ParticleManagerOffset
    {
        inline constexpr auto FirstParticleActiveUnsigned = 0x9F74u;
        inline constexpr auto FirstGeneratorActiveUnsigned = 0x2F0294u;
        inline constexpr auto Wind = 4;
        inline constexpr auto DrawRowCount = 8;
        inline constexpr auto DrawRows = 0xC;
        inline constexpr auto FirstDrawRowCount = 0x10;
        inline constexpr auto FirstDrawRowIndices = 0x14;
        inline constexpr auto GeneratorManager = 0;

        inline constexpr auto Particles = 0x9F70;
        inline constexpr auto ParticlesUnsigned = 0x9F70u;
        inline constexpr auto LastParticleIndex = 0x2F0270;
        inline constexpr auto Generators = 0x2F0278;
        inline constexpr auto GeneratorsUnsigned = 0x2F0278u;
        inline constexpr auto LastGeneratorIndex = 0x2F7F78;
    }

    namespace RecordSize
    {
        inline constexpr auto AITaskDescription = 32;
        inline constexpr auto AIMilitarySlot = 52;

        inline constexpr auto AISoldier = 84;
        inline constexpr auto AICombinedTask = 0x58;
        inline constexpr auto AIMainTask = 0x44;
        inline constexpr auto AIVehicle = 28;
        inline constexpr auto AISoldierUnsigned = 0x54u;
        inline constexpr auto AICombinedTaskUnsigned = 0x58u;
        inline constexpr auto AIMainTaskUnsigned = 0x44u;
        inline constexpr auto AIVehicleUnsigned = 0x1Cu;

        inline constexpr auto MacroMapPatternUnsigned = 12u;
        inline constexpr auto MacroMapTransitionUnsigned = 16u;
        inline constexpr auto MacroMapLandscapeUnsigned = 16u;
        inline constexpr auto MacroMapHeightUnsigned = 6u;
        inline constexpr auto MacroMapVertexColorUnsigned = 6u;
        inline constexpr auto MacroMapRoadUnsigned = 4u;
        inline constexpr auto MacroMapCreatureUnsigned = 16u;
        inline constexpr auto MacroMapHouseUnsigned = 20u;
        inline constexpr auto MacroMapFishSwarmUnsigned = 8u;

        inline constexpr auto MacroMapNameUnsigned = 44u;
        inline constexpr auto MacroMapName = 44;
        inline constexpr auto MacroMapPattern = 12;
        inline constexpr auto MacroMapTransition = 16;
        inline constexpr auto MacroMapLandscape = 16;
        inline constexpr auto MacroMapHeight = 6;
        inline constexpr auto MacroMapVertexColor = 6;
        inline constexpr auto MacroMapRoad = 4;
        inline constexpr auto MacroMapCreature = 16;
        inline constexpr auto MacroMapHouse = 20;
        inline constexpr auto MacroMapFishSwarm = 8;
        inline constexpr auto MacroMap = 376;

        inline constexpr auto Particle = 304;
        inline constexpr auto ParticlePosition = 40;
        inline constexpr auto ParticlePositionStateCopy = 112;
        inline constexpr auto ParticleUnsigned = 0x130u;
        inline constexpr auto StaticParticleUnsigned = 0x48u;
        inline constexpr auto ParticleGeneratorUnsigned = 0x68u;
        inline constexpr auto ParticleEffectUnsigned = 0x34u;
        inline constexpr auto ParticleGeneratorSlotUnsigned = 0x20u;
        inline constexpr auto ParticleDrawRowUnsigned = 0x198u;
        inline constexpr auto ParticlePoolClearBytes = 3040008;
        inline constexpr auto ParticleGeneratorPoolBytes = 0x7D04;
        inline constexpr auto ParticleDrawPrefixBytes = 40812;

        inline constexpr auto DisplayDrawCounters = 32;
        inline constexpr auto DisplayObjectReference = 68;
        inline constexpr auto DisplayMarkedObject = 0x48u;
        inline constexpr auto DisplaySearchObject = 396;
        inline constexpr auto DisplayInitializedTail = 2280;
        inline constexpr auto DisplayScrollState = 72;
        inline constexpr auto WeatherSector = 12u;
        inline constexpr auto WeatherSectorDimensions = 8;
        inline constexpr auto WeatherDensity = 4u;
        inline constexpr auto StaticMoveableManager = 113240;
        inline constexpr auto StaticMoveableData = 32;
        inline constexpr auto FastMoveableTribeUnsigned = 0x544u;
        inline constexpr auto FastMoveableTribe = 0x544;
        inline constexpr auto FastMoveableJobUnsigned = 0x18u;
        inline constexpr auto FastMoveableJob = 0x18;
        inline constexpr auto StaticMoveableAnimation = 0x64;
        inline constexpr auto StaticMoveableWalk = 0x98;
        inline constexpr auto FrameList = 8;
        inline constexpr auto StaticAnimalVehicleGraphics = 0x28;
        inline constexpr auto StaticAnimalVehicleGraphicsUnsigned = 0x28u;

        inline constexpr auto StaticGoodGraphics = 264;
        inline constexpr auto StaticGoodGraphicsHex = 0x108;
        inline constexpr auto HumanJobBaseGraphics = 0xBC;
        inline constexpr auto HumanJobBaseGraphicsUnsigned = 0xBCu;
        inline constexpr auto HumanJobChangeGraphics = 0x78;
        inline constexpr auto HumanJobChangeGraphicsUnsigned = 0x78u;
        inline constexpr auto HumanBodyGraphicsUnsigned = 12u;
        inline constexpr auto HumanHeadGraphicsUnsigned = 8u;
        inline constexpr auto RandomPaletteSet = 0xB4;
        inline constexpr auto RandomPaletteSetUnsigned = 0xB4u;
        inline constexpr auto RandomPalettePatch = 16;
        inline constexpr auto RandomPalettePatchUnsigned = 16u;
        inline constexpr auto RandomPalettePatchHexUnsigned = 0x10u;
        inline constexpr auto TribeHumanNameArrays = 8;
        inline constexpr auto TribeHumanNameArraysUnsigned = 8u;
        inline constexpr auto StaticHumanColor = 0x58;
        inline constexpr auto StaticHumanColorUnsigned = 0x58u;

        inline constexpr auto MoveableAnimationPartUnsigned = 28u;
        inline constexpr auto DisplayAtomInformation = 60;
        inline constexpr auto DynamicAnimalVehicle = 0x44;
        inline constexpr auto DynamicAnimalVehicleUnsigned = 0x44u;
        inline constexpr auto DynamicHuman = 0x8C8;
        inline constexpr auto DynamicHumanUnsigned = 0x8C8u;
        inline constexpr auto DynamicHumanSavegamePayload = 0x888;

        inline constexpr auto InteractionTargetData = 28;
        inline constexpr auto HumanAssistantData = 0xD0;
        inline constexpr auto HumanAssistantCommandHistory = 0x14u;
        inline constexpr auto HumanInventorySlot = 8u;
        inline constexpr auto UserNetworkCommand = 20u;
        inline constexpr auto VehiclePassenger = 12u;
        inline constexpr auto VehiclePassengerSigned = 12;
        inline constexpr auto VehicleStockReservation = 12u;
        inline constexpr auto AtomicAnimation = 0x138u;
        inline constexpr auto StaticVehicleType = 0x1DCu;
        inline constexpr auto TargetData = 12u;
        inline constexpr auto TargetDataSigned = 12;
        inline constexpr auto HouseQualityEnhancer = 8u;
        inline constexpr auto HouseAttachment = 16u;
        inline constexpr auto HouseConstructionGood = 20u;
        inline constexpr auto MapMoveable = 0x58u;
        inline constexpr auto MapMoveableSigned = 0x58;
        inline constexpr auto MapPathfinder = 0x84u;
        inline constexpr auto MapPathfinderSigned = 0x84;
        inline constexpr auto SectorPathfinder = 0x60u;
        inline constexpr auto SectorPathfinderSigned = 0x60;
        inline constexpr auto PathfinderDirectionRun = 2u;
        inline constexpr auto JobType = 0x88u;
        inline constexpr auto JobTypeSigned = 0x88;
        inline constexpr auto GameControlPlayer = 0x4A80u;
        inline constexpr auto GameControlPlayerSigned = 0x4A80;
        inline constexpr auto GameControlPlayerArray = 0x5D200u;
    }

    namespace VehicleOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto VehicleId = 4;
        inline constexpr auto UniqueId = 8;
        inline constexpr auto Hitpoints = 0xC;
        inline constexpr auto UserCommandQueue = 0x10u;
        inline constexpr auto UserCommandQueueSigned = 0x10;
        inline constexpr auto CurrentTask = 0x1C8;
        inline constexpr auto DoorEntryPoint = 0x1CC;
        inline constexpr auto DockedFlag = 0x1D0;
        inline constexpr auto TargetWorkPosition = 0x1D2;
        inline constexpr auto MilitaryCenter = 0x1D6;
        inline constexpr auto MapMoveableId = 0x1DC;
        inline constexpr auto PlayerId = 0x1E0;
        inline constexpr auto TribeType = 0x1E4;
        inline constexpr auto LogicType = 0x1E8;
        inline constexpr auto JobType = 0x1EC;
        inline constexpr auto HumanPassengers = 0x1F0u;
        inline constexpr auto HumanPassengersSigned = 0x1F0;
        inline constexpr auto HumanPassengerInsideFlags = 0x1F8u;
        inline constexpr auto CommanderAttachedFlag = 0x2EC;
        inline constexpr auto DraggingAnimalInsideFlag = 0x2F0;
        inline constexpr auto DraggingAnimalId = 0x2F4;
        inline constexpr auto DraggingAnimalRevision = 0x2F8;
        inline constexpr auto VehiclePassengers = 0x2FCu;
        inline constexpr auto VehiclePassengersSigned = 0x2FC;
        inline constexpr auto VehiclePassengerInsideFlags = 0x304u;
        inline constexpr auto VehiclePassengerInsideFlagsSigned = 0x304;
        inline constexpr auto StockAmounts = 0x338u;
        inline constexpr auto StockAmountsSigned = 0x338;
        inline constexpr auto FutureStockAmounts = 0x37Au;
        inline constexpr auto WantedStockAmounts = 0x3BCu;
        inline constexpr auto AtomicAction = 0x4FC;
        inline constexpr auto AtomicAnimationRunningFlag = 0x500;
        inline constexpr auto AtomicAnimationId = 0x504;
        inline constexpr auto AtomicAnimationFrame = 0x508;
        inline constexpr auto AtomicAnimationStartCount = 0x50C;
        inline constexpr auto PreviousSectorVehicleId = 0x510;
        inline constexpr auto NextSectorVehicleId = 0x514;
        inline constexpr auto ExplorationRadius = 0x518;
        inline constexpr auto MissionCheckId = 0x51C;
        inline constexpr auto PrimaryMilitaryTarget = 0x520;
        inline constexpr auto PrimaryMilitaryTargetValue = 0x524;
        inline constexpr auto PrimaryMilitaryTargetRevision = 0x528;
        inline constexpr auto SecondaryMilitaryTarget = 0x52C;
        inline constexpr auto SecondaryMilitaryTargetValue = 0x530;
        inline constexpr auto SecondaryMilitaryTargetRevision = 0x534;
        inline constexpr auto ActiveMilitaryTargetIndex = 0x538;
        inline constexpr auto MilitaryMode = 0x53C;
        inline constexpr auto CarrierVehicleId = 0x540;
        inline constexpr auto CarrierVehicleRevision = 0x544;
        inline constexpr auto InsideVehicleFlag = 0x548;
    }

    namespace VehiclePassengerOffset
    {
        inline constexpr auto ObjectId = 0;
        inline constexpr auto ObjectRevision = 4u;
        inline constexpr auto ObjectRevisionSigned = 4;
        inline constexpr auto InsideFlag = 8u;
        inline constexpr auto InsideFlagSigned = 8;
    }

    namespace VehicleStockOffset
    {
        inline constexpr auto HumanReservations = 0xC8u;
        inline constexpr auto ReservationHumanRevisions = 0xCCu;
    }

    namespace VehicleStockReservationOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto HumanRevision = 4;
        inline constexpr auto RetainedAmount = 8;
    }

    namespace AtomicAnimationOffset
    {
        inline constexpr auto LengthInTicks = 0x34;
    }

    namespace AnimalStateOffset
    {
        inline constexpr auto WorkCenterX = 0x0C;
        inline constexpr auto WorkCenterY = 0x0E;
        inline constexpr auto StayPointX = 0x10;
        inline constexpr auto StayPointY = 0x12;
        inline constexpr auto BirthPointX = 0x14;
        inline constexpr auto BirthPointY = 0x16;
        inline constexpr auto MilitaryTarget = 0x2C;
        inline constexpr auto AttachmentTarget = 0x38;
    }

    namespace VehicleVarsOffset
    {
        inline constexpr auto DoorEntryPointX = 0x1BC;
        inline constexpr auto DoorEntryPointY = 0x1BE;
        inline constexpr auto TargetWorkPositionX = 0x1C2;
        inline constexpr auto TargetWorkPositionY = 0x1C4;
        inline constexpr auto MilitaryCenterX = 0x1C6;
        inline constexpr auto MilitaryCenterY = 0x1C8;
    }

    namespace HumanTaskOffset
    {
        inline constexpr auto UserCommandQueue = 0x28u;
    }

    namespace MapPathfinderOffset
    {
        inline constexpr auto AllocatedFlag = 0;
        inline constexpr auto SearchState = 4;
        inline constexpr auto TargetMode = 8;
        inline constexpr auto SearchRadius = 0x0C;
        inline constexpr auto RequiredClearance = 0x10;
        inline constexpr auto UseResistanceFlag = 0x14;
        inline constexpr auto UseSectorsFlag = 0x15;
        inline constexpr auto AllowVerticalDirectionsFlag = 0x16;
        inline constexpr auto UseGuidesFlag = 0x17;
        inline constexpr auto DeterministicDirectionChoiceFlag = 0x18;
        inline constexpr auto SourceGuideId = 0x1C;
        inline constexpr auto SourceGuideRevision = 0x20;
        inline constexpr auto CurrentGuideId = 0x24;
        inline constexpr auto CurrentGuideRevision = 0x28;
        inline constexpr auto PlayerId = 0x2C;
        inline constexpr auto PreferredDirection1 = 0x30;
        inline constexpr auto PreferredDirection2 = 0x34;
        inline constexpr auto PreferredDirectionWeight1 = 0x38;
        inline constexpr auto PreferredDirectionWeight2 = 0x3C;
        inline constexpr auto CurrentPosition = 0x40;
        inline constexpr auto DestinationPosition = 0x44;
        inline constexpr auto TargetMoveableId = 0x48;
        inline constexpr auto TargetObjectRevision = 0x4C;
        inline constexpr auto TargetDistance = 0x50;
        inline constexpr auto DirectionRunIndex = 0x54;
        inline constexpr auto BlockedAreaSteps = 0x58;
        inline constexpr auto DirectionRuns = 0x5C;
        inline constexpr auto DirectionRunCounts = 0x5D;
        inline constexpr auto DirectionRunCountsUnsigned = 0x5Du;
    }

    namespace TargetDataOffset
    {
        inline constexpr auto Type = 0;
        inline constexpr auto ObjectIdOrMapPosition = 4;
        inline constexpr auto MapPositionX = 4;
        inline constexpr auto MapPositionY = 6;
        inline constexpr auto ObjectRevision = 8;
    }

    namespace InteractionTargetDataOffset
    {
        inline constexpr auto Type = 0;
        inline constexpr auto RealPosition = 4;
        inline constexpr auto WorkPosition = 8;
        inline constexpr auto LandscapeType = 12;
        inline constexpr auto GoodType = 16;
        inline constexpr auto ObjectId = 0x14;
        inline constexpr auto ObjectRevision = 0x18;
    }

    namespace HumanAssistantDataOffset
    {
        inline constexpr auto InitializedFlag = 0;
        inline constexpr auto HasCommandHistoryFlag = 4;
        inline constexpr auto CommandHistory = 8u;
    }

    namespace HumanAssistantCommandHistoryOffset
    {
        inline constexpr auto Command = 0u;
        inline constexpr auto Parameter1 = 4u;
        inline constexpr auto Parameter2 = 8u;
        inline constexpr auto StartTick = 0x0C;
        inline constexpr auto StartTickUnsigned = 0x0Cu;
        inline constexpr auto DurationTicks = 0x10;
        inline constexpr auto DurationTicksUnsigned = 0x10u;
    }

    namespace HumanMilitaryStateOffset
    {
        inline constexpr auto Targets = 8u;
    }

    namespace SectorPathfinderOffset
    {
        inline constexpr auto SearchState = 0;
        inline constexpr auto SearchRadius = 4;
        inline constexpr auto RequiredClearance = 8;
        inline constexpr auto UseResistanceFlag = 0x0C;
        inline constexpr auto AllowVerticalDirectionsFlag = 0x0D;
        inline constexpr auto PreferredDirection1 = 0x10;
        inline constexpr auto PreferredDirection2 = 0x14;
        inline constexpr auto CurrentPosition = 0x20;
        inline constexpr auto DestinationPosition = 0x24;
        inline constexpr auto SearchRectangle = 0x28;
        inline constexpr auto DirectionRunIndex = 0x30;
        inline constexpr auto BlockedAreaSteps = 0x34;
        inline constexpr auto DirectionRuns = 0x38;
        inline constexpr auto DirectionRunCounts = 0x39;
        inline constexpr auto DirectionRunCountsUnsigned = 0x39u;
    }

}
