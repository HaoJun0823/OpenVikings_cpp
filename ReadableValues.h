#pragma once

// Readable constants retain the original Windows values and integer literal types.
// Byte offsets describe established fields without changing object layouts.
namespace ReadableValues
{
    namespace AIManagerToken
    {
        inline constexpr auto AIMainTaskBuildMilestone = 0x6169626D;
        inline constexpr auto AIMainTaskClearTributes = 0x6169636C;
        inline constexpr auto AISetTributeHireling = 0x61697468;
        inline constexpr auto AIAddTribute = 0x61697472;
    }

    namespace AnimalOffset
    {
        inline constexpr auto MapMoveableId = 0x0C;
        inline constexpr auto PlayerId = 0x10;
        inline constexpr auto TribeType = 0x14;
        inline constexpr auto AnimalType = 0x18;
    }

    namespace AnimalTypeToken
    {
        inline constexpr auto GetAngry = 0x67616E67u;
        inline constexpr auto IgnoreHouses = 0x6967686Fu;
        inline constexpr auto TribeType = 0x74797065u;
    }

    namespace ArmorTypeToken
    {
        inline constexpr auto Type = 0x74797065u;
    }

    namespace ArrayChunk
    {
        inline constexpr auto Header = 0x78616861u;
    }

    namespace Binary64
    {
        inline constexpr auto ExponentMaximum = 0x7FFu;
        inline constexpr auto ExponentHighWordMask = 0x7FF00000u;
        inline constexpr auto NegativeInfinityHighWord = 0xFFF00000u;
        inline constexpr auto FractionMask = 0x000FFFFFFFFFFFFFULL;
        inline constexpr auto ImplicitLeadingBit = 0x0010000000000000ULL;
        inline constexpr auto LargestFiniteBits = 0x7FEFFFFFFFFFFFFFULL;
        inline constexpr auto ExponentMask = 0x7FF0000000000000ULL;
        inline constexpr auto SignMask = 0x8000000000000000ULL;
    }

    namespace BitMask
    {
        inline constexpr auto Bit1 = 0x02u;
        inline constexpr auto Bit1Signed = 0x02;
        inline constexpr auto Bit2 = 0x04u;
        inline constexpr auto Bit2Signed = 0x04;
        inline constexpr auto Bit3 = 0x08u;
        inline constexpr auto Bit1AndBit3 = 0x0Au;
        inline constexpr auto Bits2Through3 = 0x0Cu;
        inline constexpr auto Low4Bits = 0x0Fu;
        inline constexpr auto Bit4 = 0x10u;
        inline constexpr auto Bit4Signed = 0x10;
        inline constexpr auto Bits3Through4 = 0x18u;
        inline constexpr auto Low5Bits = 0x1Fu;
        inline constexpr auto Bit5 = 0x20u;
        inline constexpr auto Bit5Signed = 0x20;
        inline constexpr auto Bits2Through5 = 0x3C;
        inline constexpr auto Low6Bits = 0x3Fu;
        inline constexpr auto Bit6 = 0x40u;
        inline constexpr auto Bit6Signed = 0x40;
        inline constexpr auto Bit0AndBit6 = 0x41u;
        inline constexpr auto Bits0Through1AndBit6 = 0x43u;
        inline constexpr auto Bit3AndBit6 = 0x48u;
        inline constexpr auto Bit3AndBit6Signed = 0x48;
        inline constexpr auto Bit4AndBit6 = 0x50u;
        inline constexpr auto Bit7 = 0x80u;
        inline constexpr auto Bit7Signed = 0x80;
        inline constexpr auto Bit1AndBit7 = 0x82u;
        inline constexpr auto ByteExceptBits2Through6 = 0x83u;
        inline constexpr auto ByteExceptBits3Through6 = 0x87u;
        inline constexpr auto ByteExceptBits4Through6 = 0x8Fu;
        inline constexpr auto Bit4AndBit7 = 0x90u;
        inline constexpr auto ByteExceptBits0Through5 = 0xC0u;
        inline constexpr auto ByteExceptBits4Through5 = 0xCFu;
        inline constexpr auto ByteExceptBit5 = 0xDFu;
        inline constexpr auto ByteExceptBit4 = 0xEFu;
        inline constexpr auto ByteExceptBits2Through3 = 0xF3;
        inline constexpr auto ByteExceptBit3 = 0xF7u;
        inline constexpr auto ByteExceptBits0Through2 = 0xF8u;
        inline constexpr auto ByteExceptBit2 = 0xFB;
        inline constexpr auto ByteExceptBits0Through1 = 0xFCu;
        inline constexpr auto ByteExceptBits0Through1Signed = 0xFC;
        inline constexpr auto ByteExceptBit1 = 0xFDu;
        inline constexpr auto ByteExceptBit1Signed = 0xFD;
        inline constexpr auto ByteExceptBit0 = 0xFEu;
        inline constexpr auto Bit8 = 0x100u;
        inline constexpr auto Bit8Signed = 0x100;
        inline constexpr auto Bits0Through1AndBit8 = 0x103u;
        inline constexpr auto Bit2AndBit8 = 0x104u;
        inline constexpr auto Bits0Through2AndBit8 = 0x107u;
        inline constexpr auto Bit3AndBit8 = 0x108u;
        inline constexpr auto Bits2Through3AndBit8 = 0x10Cu;
        inline constexpr auto Bits1Through8 = 0x1FEu;
        inline constexpr auto Low9Bits = 0x1FFu;
        inline constexpr auto Low9BitsSigned = 0x1FF;
        inline constexpr auto Bit9 = 0x200;
        inline constexpr auto Bit9Unsigned = 0x200u;
        inline constexpr auto Bits8Through9 = 0x300u;
        inline constexpr auto Bit10 = 0x400u;
        inline constexpr auto Bits8Through10 = 0x700u;
        inline constexpr auto Bit11 = 0x800u;
        inline constexpr auto Bit4AndBit11 = 0x810u;
        inline constexpr auto Bits4Through5AndBit11 = 0x830u;
        inline constexpr auto Bit8AndBit11 = 0x900u;
        inline constexpr auto Bits10Through11 = 0xC00u;
        inline constexpr auto Low12Bits = 0x0FFF;
        inline constexpr auto Low12BitsUnsigned = 0xFFFu;
        inline constexpr auto Bit12 = 0x1000u;
        inline constexpr auto Bit0AndBit5AndBit12 = 0x1021u;
        inline constexpr auto Bit1AndBit8AndBit12 = 0x1102u;
        inline constexpr auto Low13Bits = 0x1FFFu;
        inline constexpr auto Bit13 = 0x2000u;
        inline constexpr auto Bit13Signed = 0x2000;
        inline constexpr auto Low14Bits = 0x3FFFu;
        inline constexpr auto Bit14 = 0x4000u;
        inline constexpr auto Bit8AndBit14 = 0x4100u;
        inline constexpr auto Bits8Through10AndBit14 = 0x4700u;
        inline constexpr auto Bits3Through14 = 0x7FF8u;
        inline constexpr auto Bit15 = 0x8000u;
        inline constexpr auto Bit15Signed = 0x8000;
        inline constexpr auto WordExceptBits4Through14 = 0x800Fu;
        inline constexpr auto WordExceptBits0Through13 = 0xC000u;
        inline constexpr auto WordExceptBits0Through10 = 0xF800u;
        inline constexpr auto WordExceptBits0Through9 = 0xFC00u;
        inline constexpr auto WordExceptBits0Through7 = 0xFF00u;
        inline constexpr auto WordExceptBits0Through1 = 0xFFFCu;
        inline constexpr auto WordExceptBit0 = 0xFFFEu;
        inline constexpr auto Bit16 = 0x10000u;
        inline constexpr auto Bit16Signed = 0x10000;
        inline constexpr auto Bits1Through16 = 0x1FFFEu;
        inline constexpr auto Low17Bits = 0x1FFFFu;
        inline constexpr auto Bit17 = 0x00020000u;
        inline constexpr auto Bit17Signed = 0x20000;
        inline constexpr auto Bits16Through17 = 0x30000u;
        inline constexpr auto Bit18 = 0x40000u;
        inline constexpr auto Low19Bits = 0x7FFFFu;
        inline constexpr auto Bit19 = 0x00080000u;
        inline constexpr auto Low20Bits = 0xFFFFFu;
        inline constexpr auto Low22Bits = 0x003FFFFFu;
        inline constexpr auto Bit23 = 0x00800000u;
        inline constexpr auto Bit19AndBits22Through23 = 0x00C80000;
        inline constexpr auto Bits16Through23 = 0xFF0000u;
        inline constexpr auto Low24Bits = 0x00FFFFFFu;
        inline constexpr auto Bit30 = 0x40000000u;
        inline constexpr auto Bits0Through18AndBits20Through21AndBits24Through30 = 0x7F37FFFF;
        inline constexpr auto DwordExceptBits0Through23 = 0xFF000000u;
        inline constexpr auto DwordExceptBits5Through16 = 0xFFFE001Fu;
        inline constexpr auto DwordExceptBits0Through15 = 0xFFFF0000u;
        inline constexpr auto DwordExceptBits0Through11 = 0xFFFFF000u;
        inline constexpr auto DwordExceptBits10Through11 = 0xFFFFF3FFu;
        inline constexpr auto DwordExceptBit11 = 0xFFFFF7FFu;
        inline constexpr auto DwordExceptBit10 = 0xFFFFFBFFu;
        inline constexpr auto DwordExceptBits0Through7 = 0xFFFFFF00u;
        inline constexpr auto DwordExceptBit5 = 0xFFFFFFDFu;
        inline constexpr auto DwordExceptBits2Through4 = 0xFFFFFFE3u;
        inline constexpr auto DwordExceptBits3Through4 = 0xFFFFFFE7u;
        inline constexpr auto Bits0Through1AndBit3AndBits5Through31 = 0xFFFFFFEBu;
        inline constexpr auto DwordExceptBit4 = 0xFFFFFFEFu;
        inline constexpr auto DwordExceptBits0Through3 = 0xFFFFFFF0u;
        inline constexpr auto Bit2AndBits4Through31 = 0xFFFFFFF4u;
        inline constexpr auto DwordExceptBit3 = 0xFFFFFFF7u;
        inline constexpr auto DwordExceptBit2 = 0xFFFFFFFBu;
        inline constexpr auto DwordExceptBits0Through1 = 0xFFFFFFFCu;
        inline constexpr auto DwordExceptBit0 = 0xFFFFFFFEu;
        inline constexpr auto Low48Bits = 0x0000FFFFFFFFFFFFULL;
        inline constexpr auto Bit63 = 0x8000000000000000ULL;
    }

    namespace CampaignStaticDataManagerToken
    {
        inline constexpr auto V = 0x00766172;
        inline constexpr auto A = 0x616C6C6F;
    }

    namespace CleanMapsManagerToken
    {
        inline constexpr auto MapDescriptionStringId = 0x6D617064;
        inline constexpr auto MapNameStringId = 0x6D617073;
    }

    namespace ComResult
    {
        inline constexpr auto Failure = 0x80004005u;
        inline constexpr auto NotInitialized = 0x800401F0u;
    }

    namespace CompressedFileHeader
    {
        inline constexpr auto Signature = 0x2E433250u;
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

    namespace EngineLifetimeToken
    {
        inline constexpr auto Generic = 0x67656E65u;
        inline constexpr auto Respond = 0x72657370u;
        inline constexpr auto Scream = 0x73637265u;
        inline constexpr auto SetRainRectangle = 0x73727265u;
        inline constexpr auto SetSandRectangle = 0x7373616Eu;
        inline constexpr auto SetSnowRectangle = 0x73737265u;
        inline constexpr auto LogicTribe = 0x74726962u;
    }

    namespace EngineMacroMapsToken
    {
        inline constexpr auto Version = 0x766572;
        inline constexpr auto Animal = 0x616E696D;
        inline constexpr auto Center = 0x63656E74;
        inline constexpr auto FishSwarm = 0x66697368;
        inline constexpr auto Hgt = 0x68656967;
        inline constexpr auto House = 0x686F7573;
        inline constexpr auto Human = 0x68756D61;
        inline constexpr auto Land = 0x6C616E64;
        inline constexpr auto LandName = 0x6C616E6E;
        inline constexpr auto Pat = 0x70617464;
        inline constexpr auto Patname = 0x7061746E;
        inline constexpr auto Road = 0x726F6164;
        inline constexpr auto Size = 0x73697A65;
        inline constexpr auto Trans = 0x74726164;
        inline constexpr auto Transname = 0x7472616E;
        inline constexpr auto Veco = 0x7665636F;
    }

    namespace EngineMapChunk
    {
        inline constexpr auto MapHeader = 0x656D6D6D;
    }

    namespace EngineSavegameChunk
    {
        inline constexpr auto EngineHeader = 0x58453244;
    }

    namespace GameControlOffset
    {
        inline constexpr auto DifficultyLevel = 0x5D200;
    }

    namespace GameControlToken
    {
        inline constexpr auto Add = 0x61646469;
        inline constexpr auto AllowGood = 0x61676F6F;
        inline constexpr auto AllowHouse = 0x61686F75;
        inline constexpr auto AllowJob = 0x616A6F62;
        inline constexpr auto Diplomacy = 0x6469706C;
        inline constexpr auto ForbidGood = 0x66676F6F;
        inline constexpr auto ForbidHouse = 0x66686F75;
        inline constexpr auto ForbidJob = 0x666A6F62;
        inline constexpr auto NameTribe = 0x6E616D74;
        inline constexpr auto NameTribeShort = 0x6E616D75;
        inline constexpr auto NoSeenFirstMessage = 0x6E73666D;
        inline constexpr auto PlayerFixColors = 0x7066636F;
        inline constexpr auto PlayerHideInMenu = 0x7068696D;
        inline constexpr auto Player = 0x706C6179;
        inline constexpr auto PlayerOption = 0x706C6F70;
        inline constexpr auto PlayerNeverDies = 0x706E6469;
        inline constexpr auto RelationHide = 0x72656C68;
        inline constexpr auto RelationNotChangeable = 0x72656C6E;
        inline constexpr auto RelationHideDetails = 0x72656C73;
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

    namespace GoodToken
    {
        inline constexpr auto Type = 0x74797065;
    }

    namespace GuiSavegameChunk
    {
        inline constexpr auto GuiHeader = 0x58475549;
    }

    namespace HouseOffset
    {
        inline constexpr auto PlayerId = 0x0C;
        inline constexpr auto EngineType = 0x10;
        inline constexpr auto LogicType = 0x14;
        inline constexpr auto MapPosition = 0x1C;
        inline constexpr auto MapPositionX = 0x1C;
        inline constexpr auto MapPositionY = 0x1E;
        inline constexpr auto DoorEntryPoint = 0x24;
        inline constexpr auto DoorEntryPointX = 0x24;
        inline constexpr auto DoorEntryPointY = 0x26;
        inline constexpr auto ConstructionState = 0x2C;
        inline constexpr auto LandscapeOverlayActiveFlags = 0x7BCu;
        inline constexpr auto LandscapeOverlayActiveFlagsSigned = 0x7BC;
    }

    namespace HouseToken
    {
        inline constexpr auto LogicType = 0x6C6F7479;
    }

    namespace HumanJobExperienceTypeToken
    {
        inline constexpr auto Type = 0x74797065u;
    }

    namespace HumanMerchantToken
    {
        inline constexpr auto TradeAgreement = 0x706E616D;
    }

    namespace HumanMiscToken
    {
        inline constexpr auto GraphicsHumanRandomPalette = 0x67726872;
        inline constexpr auto LogicGood = 0x6C6F6764;
        inline constexpr auto Patch = 0x70617463;
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

    namespace HyperLinkTextSystemToken
    {
        inline constexpr auto Rem = 0x72656D;
        inline constexpr auto Block = 0x626C636B;
        inline constexpr auto GlobalJump = 0x676A6D70;
        inline constexpr auto LocalJump = 0x6C6A6D70;
        inline constexpr auto OnScreenCallback = 0x6F736362;
        inline constexpr auto UserIcon = 0x7569636F;
        inline constexpr auto UserPicture = 0x75706963;
        inline constexpr auto TextXEnd = 0x78656E64;
        inline constexpr auto TextXStart = 0x78737461;
    }

    namespace IntegerRange
    {
        inline constexpr auto Int8Maximum = 0x7Fu;
        inline constexpr auto Int8MaximumSigned = 0x7F;
        inline constexpr auto UInt8Maximum = 0xFFu;
        inline constexpr auto UInt8MaximumSigned = 0xFF;
        inline constexpr auto Int16Maximum = 0x7FFFu;
        inline constexpr auto Int16MaximumSigned = 0x7fff;
        inline constexpr auto UInt16Maximum = 0xFFFFu;
        inline constexpr auto UInt16MaximumSigned = 0xFFFF;
        inline constexpr auto Int32Maximum = 0x7FFFFFFFu;
        inline constexpr auto Int32MaximumSigned = 0x7FFFFFFF;
        inline constexpr auto Bit31 = 0x80000000u;
        inline constexpr auto UInt32Maximum = 0xFFFFFFFF;
        inline constexpr auto UInt32MaximumUnsignedLongLong = 0xFFFFFFFFULL;
        inline constexpr auto Int64Maximum = 0x7FFFFFFFFFFFFFFFULL;
    }

    namespace IoChunk
    {
        inline constexpr auto End = 0x78656E64u;
        inline constexpr auto EndSigned = 0x78656E64;
        inline constexpr auto HeaderSignature = 0x78696F68u;
        inline constexpr auto HeaderSignatureSigned = 0x78696F68;
    }

    namespace JobTypeToken
    {
        inline constexpr auto Type = 0x74797065u;
    }

    namespace LandscapeToken
    {
        inline constexpr auto Type = 0x74797065;
    }

    namespace LogicMapChunk
    {
        inline constexpr auto Continents = 0x6C61636F;
        inline constexpr auto MapData = 0x6C676D6D;
        inline constexpr auto AttachedObjects = 0x6C6D616F;
        inline constexpr auto MicroBuildingBlocked = 0x6C6D6262;
        inline constexpr auto MicroContinent = 0x6C6D636F;
        inline constexpr auto MacroHeight = 0x6C6D6865;
        inline constexpr auto MicroHouseBuildForbidden = 0x6C6D6866;
        inline constexpr auto LandscapePlayer = 0x6C6D6C70;
        inline constexpr auto LandscapeType = 0x6C6D6C74;
        inline constexpr auto LandscapeValency = 0x6C6D6C76;
        inline constexpr auto MicroMaximumMoveableSize = 0x6C6D6D73;
        inline constexpr auto MacroPatternA = 0x6C6D7061;
        inline constexpr auto MacroPatternB = 0x6C6D7062;
        inline constexpr auto MicroPathResistance = 0x6C6D7072;
        inline constexpr auto MicroRoad = 0x6C6D726F;
        inline constexpr auto MicroSectorBasePoint = 0x6C6D7362;
        inline constexpr auto MicroWalkConnections = 0x6C6D7477;
        inline constexpr auto MicroWalkingBlocked = 0x6C6D7762;
        inline constexpr auto LogicFileStart = 0x6C6F6769;
        inline constexpr auto MapSize = 0x6C73697A;
        inline constexpr auto SectorAttachments = 0x7361616D;
        inline constexpr auto BioRegrowth = 0x73616772;
        inline constexpr auto AutoTransitions = 0x7361746D;
        inline constexpr auto FishManager = 0x7366696D;
        inline constexpr auto Exploration = 0x736D6578;
        inline constexpr auto Fog = 0x736D666D;
        inline constexpr auto Guides = 0x736D6775;
        inline constexpr auto Houses = 0x736D686F;
        inline constexpr auto RoadBuildManager = 0x73726F64;
        inline constexpr auto WallBuildManager = 0x73726F77;
        inline constexpr auto LogicFileEnd = 0x74656E64;
    }

    namespace MapMoveableOffset
    {
        inline constexpr auto MapPosition = 0x12;
        inline constexpr auto PathfinderId = 0x28;
        inline constexpr auto ObjectId = 0x54;
    }

    namespace MapMoveablesToken
    {
        inline constexpr auto Seq = 0x736571;
        inline constexpr auto GraphicsAnimationFrameList = 0x67667866;
        inline constexpr auto GraphicsWalkFrameList = 0x67667866;
    }

    namespace MissionStartObjectsToken
    {
        inline constexpr auto AllowJob = 0x616A6F62;
        inline constexpr auto ForbidJob = 0x666A6F62;
        inline constexpr auto SetName = 0x7365746E;
        inline constexpr auto SetPalette = 0x73657470;
    }

    namespace MultiPlayerGoalToken
    {
        inline constexpr auto GoalWonWhenGoods = 0x6777676F;
        inline constexpr auto GoalWonWhenInhabitants = 0x6777696E;
        inline constexpr auto GoalLostByMission = 0x67776C6D;
        inline constexpr auto GoalWonByMission = 0x6777776D;
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

    namespace PackingHeader
    {
        inline constexpr auto Signature = 0x5870636Bu;
        inline constexpr auto SignatureSigned = 0x5870636B;
    }

    namespace PackingType
    {
        inline constexpr auto None = 0x6E6F6E65;
        inline constexpr auto Rle16 = 0x726C6536;
        inline constexpr auto Rle16Unsigned = 0x726C6536u;
        inline constexpr auto Rle8 = 0x726C6538;
        inline constexpr auto Rle8Unsigned = 0x726C6538u;
    }

    namespace ParticleGeneratorToken
    {
        inline constexpr auto Generate = 0x67656E65u;
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

    namespace ParticlesToken
    {
        inline constexpr auto LogicType = 0x6C747970;
    }

    namespace PlayerCenterPositionToken
    {
        inline constexpr auto StartPosition = 0x73746170;
    }

    namespace RandomSeed
    {
        inline constexpr auto Game = 0x18273645;
        inline constexpr auto DisplayEffects = 0x5432ACE0u;
    }

    namespace RecordSize
    {
        inline constexpr auto MapMoveable = 0x58u;
        inline constexpr auto MapMoveableSigned = 0x58;
        inline constexpr auto JobType = 0x88u;
        inline constexpr auto JobTypeSigned = 0x88;
        inline constexpr auto GameControlPlayer = 0x4A80u;
        inline constexpr auto GameControlPlayerSigned = 0x4A80;
        inline constexpr auto GameControlPlayerArray = 0x5D200u;
    }

    namespace Rgb555
    {
        inline constexpr auto GreenMask = 0x3E0u;
        inline constexpr auto GreenMaskSigned = 0x03E0;
        inline constexpr auto RedMask = 0x7C00;
        inline constexpr auto PairedGreenMask = 0x03E003E0u;
    }

    namespace Rgb565
    {
        inline constexpr auto GreenMask = 0x07E0;
        inline constexpr auto GreenMaskUnsigned = 0x7E0u;
        inline constexpr auto RedMask = 0xF800;
        inline constexpr auto RedMaskUnsigned = 0xF800u;
        inline constexpr auto PairedGreenMask = 0x07E007E0u;
        inline constexpr auto RedProductMask = 0xFFFFF800u;
    }

    namespace Rgb888
    {
        inline constexpr auto GreenMask = 0x0000FF00;
        inline constexpr auto GreenMaskUnsigned = 0xFF00u;
        inline constexpr auto RedMask = 0x00FF0000;
        inline constexpr auto RedMaskUnsigned = 0xFF0000u;
    }

    namespace SavegameChunk
    {
        inline constexpr auto Info = 0x78736769;
        inline constexpr auto Header = 0x78736776;
    }

    namespace SavegameInfoChunk
    {
        inline constexpr auto EndMetadata = 0x4943656E;
    }

    namespace SoundTag
    {
        inline constexpr auto MissionBriefingVoice = 0x65655656;
    }

    namespace TrianglePatternToken
    {
        inline constexpr auto BioCanGrowOn = 0x62696F63;
        inline constexpr auto Type = 0x74797065;
    }

    namespace TribeToken
    {
        inline constexpr auto Type = 0x74797065;
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

    namespace VehicleToken
    {
        inline constexpr auto LogicTransformVehicleType = 0x6C747674;
        inline constexpr auto Type = 0x74797065;
    }

    namespace WaveTag
    {
        inline constexpr auto Format = 0x20746D66u;
        inline constexpr auto Wave = 0x45564157u;
        inline constexpr auto Riff = 0x46464952u;
        inline constexpr auto Data = 0x61746164u;
    }

    namespace WeaponTypeToken
    {
        inline constexpr auto TribeType = 0x74727479u;
        inline constexpr auto Type = 0x74797065u;
    }

    namespace ZeroByteDetection
    {
        inline constexpr auto AdditionBias = 0x7EFEFEFFu;
        inline constexpr auto CandidateMask = 0x81010100u;
    }

}
