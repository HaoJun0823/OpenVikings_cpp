#pragma once

// Established byte offsets and record sizes of original Windows game objects.
// Values and integer literal types are retained from the Windows reconstruction.
namespace NC2Layout
{
    namespace AnimalOffset
    {
        inline constexpr auto MapMoveableId = 0x0C;
        inline constexpr auto PlayerId = 0x10;
        inline constexpr auto TribeType = 0x14;
        inline constexpr auto AnimalType = 0x18;
    }

    namespace EngineDisplayOffset
    {
        inline constexpr auto DestinationBitmap = 0x64;
        inline constexpr auto DrawingBitmap = 0x68;
        inline constexpr auto DrawFlags = 0x70;
        inline constexpr auto HighlightingEnabled = 0x74;
        inline constexpr auto PlayerId = 0x7C;
        inline constexpr auto GouraudShader = 0x80;
        inline constexpr auto Font = 0x924;
    }

    namespace GameControlOffset
    {
        inline constexpr auto DifficultyLevel = 0x5D200;
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
        inline constexpr auto MapMoveableId = 0xC;
        inline constexpr auto PlayerId = 0x10;
        inline constexpr auto TribeType = 0x14;
        inline constexpr auto JobType = 0x18;
        inline constexpr auto ProductionCounters = 0x21Cu;
        inline constexpr auto CurrentTask = 0x368;
        inline constexpr auto CurrentTaskStatus = 0x36C;
        inline constexpr auto CurrentTaskParameter = 0x370;
        inline constexpr auto CurrentTaskStep = 0x374;
        inline constexpr auto UserCommandQueue = 0x390u;
        inline constexpr auto InteractionTarget = 0xA68;
        inline constexpr auto TargetPosition = 0xA6C;
        inline constexpr auto TargetObjectId = 0xA7C;
        inline constexpr auto TargetObjectRevision = 0xA80;
        inline constexpr auto SecondaryInteractionTarget = 0xA84;
        inline constexpr auto Hitpoints = 0xAA0;
        inline constexpr auto Food = 0xAA8;
        inline constexpr auto Stamina = 0xAAC;
        inline constexpr auto Social = 0xAB0;
        inline constexpr auto Religion = 0xAB4;
        inline constexpr auto GoodCirculationMode = 0xACC;
        inline constexpr auto RequestedGood = 0xAD0;
        inline constexpr auto CarriedGood = 0xAD4;
        inline constexpr auto EquippedWeapon = 0xAE4;
        inline constexpr auto EquippedArmor = 0xAE8;
        inline constexpr auto EquippedShoes = 0xAEC;
        inline constexpr auto EquippedTools = 0xAF0;
        inline constexpr auto InventorySlots = 0xAF4u;
        inline constexpr auto InventorySlotsSigned = 0xAF4;
        inline constexpr auto HomeHouseId = 0xB28;
        inline constexpr auto WorkCenter = 0xB30;
        inline constexpr auto WorkAttachmentHouseId = 0xB34;
        inline constexpr auto AttachedVehicleId = 0xB48;
        inline constexpr auto AttachedVehicleRevision = 0xB4C;
        inline constexpr auto MiscFlags = 0xB84;
    }

    namespace MapMoveableOffset
    {
        inline constexpr auto MapPosition = 0x12;
        inline constexpr auto PathfinderId = 0x28;
        inline constexpr auto ObjectId = 0x54;
    }

    namespace ObjectSize
    {
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

    namespace ParticleManagerOffset
    {
        inline constexpr auto Particles = 0x9F70;
        inline constexpr auto ParticlesUnsigned = 0x9F70u;
        inline constexpr auto LastParticleIndex = 0x2F0270;
        inline constexpr auto Generators = 0x2F0278;
        inline constexpr auto GeneratorsUnsigned = 0x2F0278u;
        inline constexpr auto LastGeneratorIndex = 0x2F7F78;
    }

    namespace RecordSize
    {
        inline constexpr auto HouseQualityEnhancer = 8u;
        inline constexpr auto HouseAttachment = 16u;
        inline constexpr auto HouseConstructionGood = 20u;
        inline constexpr auto MapMoveable = 0x58u;
        inline constexpr auto MapMoveableSigned = 0x58;
        inline constexpr auto JobType = 0x88u;
        inline constexpr auto JobTypeSigned = 0x88;
        inline constexpr auto GameControlPlayer = 0x4A80u;
        inline constexpr auto GameControlPlayerSigned = 0x4A80;
        inline constexpr auto GameControlPlayerArray = 0x5D200u;
    }

    namespace VehicleOffset
    {
        inline constexpr auto MapMoveableId = 0x1DC;
        inline constexpr auto PlayerId = 0x1E0;
        inline constexpr auto TribeType = 0x1E4;
        inline constexpr auto LogicType = 0x1E8;
        inline constexpr auto JobType = 0x1EC;
        inline constexpr auto StockAmounts = 0x338u;
        inline constexpr auto StockAmountsSigned = 0x338;
        inline constexpr auto FutureStockAmounts = 0x37Au;
        inline constexpr auto ExplorationRadius = 0x518;
        inline constexpr auto CarrierVehicleId = 0x540;
        inline constexpr auto CarrierVehicleRevision = 0x544;
        inline constexpr auto InsideVehicleFlag = 0x548;
    }

}
