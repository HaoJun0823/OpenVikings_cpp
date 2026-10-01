#include "C2GameState.h"
#include "ApplicationGlobals.h"
#include "C2LogicLifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2EngineTerrainData.h"
#include "C2GameMapAccess.h"
#include "C2MapSectorAttachment.h"
#include "C2MapWalkSectorManager.h"
#include "XIniFileReader.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "C2CampaignStaticDataManager.h"
#include "C2StaticStringsManager.h"
#include "C2CleanMapsManager.h"
#include "C2SavegamesManager.h"
#include "C2StaticTrianglePatternTypeData.h"
#include "C2StaticLandscapeTypeData.h"
#include "C2StaticGoodTypeData.h"

namespace NC2Logic
{
    using namespace NC2Lifetime;
    SMapMoveableArrayStorage gMapMoveableArrayStorage{};
    extern bool __cdecl FUN_00408f7b(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0040a612(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00418d3b(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_004195a4(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00419a7f(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0041b936(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0041cbb8(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_004230ee(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00424ae9(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_004513c1(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_004511c5(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00450e8c(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_00450c1d(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0044eb1e(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    struct SUserCommandListTag {};

    // Original function: FUN_00411b6d.
    static void UserCommandList_Destruct(void* list) { static_cast<CRuntimeOwnedList<SUserCommandListTag>*>(list)->~CRuntimeOwnedList(); }

    // Original function: FUN_00410d8d. Destruction reverses the three member lists.
    static void UserCommandQueues_Destruct(void* queues)
    {
        UserCommandList_Destruct(static_cast<std::byte*>(queues) + 0x28);
        UserCommandList_Destruct(static_cast<std::byte*>(queues) + 0x14);
        UserCommandList_Destruct(queues);
    }

    // Original function: FUN_00410da9.
    void UserNetworkCommandManager_EmptyQueue(void* manager)
    {
        for (std::uint32_t offset : {0x964u, 0x978u, 0x98Cu})
        {
            DrainRuntimeList(static_cast<std::byte*>(manager) + offset);
        }
        std::memset(manager, 0, 0x964);
    }

    // Original function: FUN_00410d71.
    static void UserNetworkCommandManager_Destruct(void* manager)
    {
        UserNetworkCommandManager_EmptyQueue(manager);
        gUserNetworkCommandManager = nullptr;
        UserCommandQueues_Destruct(static_cast<std::byte*>(manager) + 0x964);
    }

    // Original function: FUN_00411bc4.
    static void GameMapModifyManager_Destruct(void*) { gGameMapModifyManager = nullptr; }

    // Original function: FUN_0041217b.
    static void StaticHumanJobExperienceTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticHumanJobExperienceTable00510A3C);
        gStaticHumanJobExperienceTable00510A3C = nullptr;
        NXBasics::Runtime_Free(gStaticHumanJobExperienceData);
        gStaticHumanJobExperienceData = nullptr;
        gStaticHumanJobExperienceTypeDataManager = nullptr;
    }

    // Original function: FUN_00412240.
    static void StaticDependenciesManager_Destruct(void*) { gStaticDependenciesManager = nullptr; }

    // Original function: FUN_00412761.
    static void StaticAnimalTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticAnimalTypeData);
        gStaticAnimalTypeData = nullptr;
        gStaticAnimalTypeDataManager = nullptr;
    }

    // Original function: FUN_00412a42.
    static void StaticArmorTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticArmorTypeData);
        gStaticArmorTypeData = nullptr;
        gStaticArmorTypeDataManager = nullptr;
    }

    // Original function: FUN_004134b6.
    static void StaticJobTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticJobTypeData);
        gStaticJobTypeData = nullptr;
        gStaticJobTypeDataManager = nullptr;
    }

    // Original function: FUN_00414173.
    static void StaticTribeTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticTribeTypeData);
        gStaticTribeTypeData = nullptr;
        gStaticTribeTypeDataManager = nullptr;
    }

    // Original function: FUN_00414409.
    static void StaticAtomicAnimationDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticAtomicAnimationData);
        gStaticAtomicAnimationData = nullptr;
        gStaticAtomicAnimationDataManager = nullptr;
    }

    // Original function: FUN_00414997.
    static void StaticVehicleTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticVehicleTypeData);
        gStaticVehicleTypeData = nullptr;
        gStaticVehicleTypeDataManager = nullptr;
    }

    // Original function: FUN_00414f1b.
    static void StaticHouseTypeDataManager_Destruct(void*)
    {
        NXBasics::Runtime_Free(gStaticHouseTypeData);
        gStaticHouseTypeData = nullptr;
        gStaticHouseTypeDataManager = nullptr;
    }

    // Original function: FUN_00412fe0.
    static void StaticWeaponTypeDataManager_Destruct(void*)
    {
        for (void*& table : gStaticWeaponTypeArrays)
        {
            if (table != nullptr)
            {
                NXBasics::Runtime_Free(table);
                table = nullptr;
            }
        }
        gStaticWeaponTypeDataManager = nullptr;
    }

    // Original function: FUN_00417168.
    static void MissionOnscreenInfoManager_Destruct(void*) { gMissionOnscreenInfoManager = nullptr; }
    // Original function: FUN_0041761f.
    static void GameAssistantManager_Destruct(void*)
    {
        Unregister(FUN_00418d3b, 0x0C, {1});
        gGameAssistantManager = nullptr;
    }
    // Original function: FUN_00419065.
    static void MultiPlayerGoalManager_Destruct(void*)
    {
        Unregister(FUN_004195a4, 0x6F, {1});
        gMultiPlayerGoalManager = nullptr;
    }
    // Original function: FUN_00419727.
    static void HappinessManager_Destruct(void*)
    {
        Unregister(FUN_00419a7f, 0x0C, {0x28, 0x27, 1});
        gHappinessManager = nullptr;
    }
    // Original function: FUN_00419bdf.
    static void EmbeddedSavegameManager_Destruct(void* manager)
    {
        if (Read<std::uint32_t>(manager, 0) != 0)
        {
            void* data = Read<void*>(manager, 4);
            Write<std::uint32_t>(manager, 0, 0);
            NXBasics::Runtime_Free(data);
            Write<void*>(manager, 4, nullptr);
        }
        gEmbeddedSavegameManager = nullptr;
    }
    // Original function: FUN_00419e06.
    static void TributeManager_Destruct(void*) { gTributeManager = nullptr; }

    static void FreeAllocatedRecords(void* manager, std::uint32_t stride)
    {
        auto* array = static_cast<CArrayHandler*>(manager);
        for (std::uint32_t index = static_cast<std::uint32_t>(array->mFirstScanIndex); index <= static_cast<std::uint32_t>(array->mLastScanIndex); ++index)
        {
            auto* entry = static_cast<std::byte*>(Read<void*>(manager, 0x30)) + index * stride;
            if (Read<std::uint8_t>(entry, 0) != 0 && array->Element_Free(NXBasics::Runtime_TimeSigned32(index), true))
            {
                entry = static_cast<std::byte*>(Read<void*>(manager, 0x30)) + index * stride;
                Write<std::uint8_t>(entry, 0, 0);
            }
        }
    }
    // Original function: FUN_0041a059.
    static void DelayedActionManager_FreeAll(void* manager) { FreeAllocatedRecords(manager, 0x30); }
    // Original function: FUN_00419f91.
    static void DelayedActionManager_Destruct(void* manager)
    {
        DelayedActionManager_FreeAll(manager);
        FreeAndClearField(manager, 0x30);
        gDelayedActionManager = nullptr;
        Write<std::uint8_t>(manager, 0, 0); // CArrayHandler::~CArrayHandler, 00430CA4.
    }
    // Original function: FUN_0041aaf1.
    static void PlayerStatisticManager_Destruct(void* manager)
    {
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            if (Read<void*>(manager, 8 + index * 4) != nullptr) FreeField(manager, 8 + index * 4);
            if (Read<void*>(manager, 0x58 + index * 4) != nullptr) FreeField(manager, 0x58 + index * 4);
        }
        gPlayerStatisticManager = nullptr;
    }
    // Original function: FUN_0041aedf.
    static void GuideManager_Destruct(void* manager)
    {
        FreeField(manager, 0x1364);
        Unregister(FUN_0041b936, 0x64, {1, 0x33, 0x31, 0x32});
        DestroyOwned(CGuideArray::sTheObjectPtr, [](CGuideArray* array) { array->~CGuideArray(); });
        gGuideManager = nullptr;
    }
    // Original function: FUN_0040b9af.
    static void MarkerArray_Destruct(void* array)
    {
        NXBasics::Runtime_Free(gMarkerEntries);
        gMarkerEntries = nullptr;
        gMarkerArray = nullptr;
        Write<std::uint8_t>(array, 0, 0);
    }
    // Original function: FUN_0041c428.
    static void HumanMerchantArray_Destruct(void* array)
    {
        Unregister(FUN_0041cbb8, 0xDE, {0x1A});
        if (Read<void*>(array, 0x30) != nullptr) FreeAndClearField(array, 0x30);
        gHumanMerchantArray = nullptr;
        Write<std::uint8_t>(array, 0, 0);
    }
    // Original function: FUN_0041cde1.
    static void MapPathfinderManager_Destruct(void* array)
    {
        NXBasics::Runtime_Free(gMapPathfinderEntries);
        gMapPathfinderEntries = nullptr;
        gMapPathfinderManager = nullptr;
        Write<std::uint8_t>(array, 0, 0);
    }
    // Original function: FUN_0040bbbd.
    static void MapMoveableArray_Destruct(CArrayHandler* array)
    {
        NXBasics::Runtime_Free(gMapMoveableArrayStorage.entries);
        gMapMoveableArrayStorage.entries = nullptr;
        gMapMoveableArrayStorage.manager = nullptr;
        array->mInitialized = 0;
    }
    // Original function: FUN_00429546; owning AI member type is not yet identified.
    static void AIMember00429546_Destruct(void* object)
    {
        if (Read<void*>(object, 0x20) != nullptr) FreeAndClearField(object, 0x20);
    }
    // Original function: FUN_00407f55.
    static void AIManager_Destruct(void* manager)
    {
        Unregister(FUN_00408f7b, 0x0B, {0x2A, 1});
        for (std::uint32_t index = 0; index < 20; ++index)
        {
            if (Read<void*>(manager, index * 4) != nullptr)
            {
                FreeAndClearField(manager, index * 4);
                void* player = Read<void*>(manager, 0x50 + index * 4);
                if (player != nullptr)
                {
                    AIMember00429546_Destruct(static_cast<std::byte*>(player) + 0xAA18);
                    NXBasics::Runtime_Free(player);
                    Write<void*>(manager, 0x50 + index * 4, nullptr);
                }
            }
        }
        gAIManager = nullptr;
    }
    // Original function: FUN_0041cff0.
    static void MissionManager_Destruct(void*)
    {
        Unregister(FUN_004230ee, 0x0A, {1});
        gMissionManager = nullptr;
    }
    // Original function: FUN_00409a9e.
    static void GameControl_Destruct(void*)
    {
        Unregister(FUN_0040a612, 0, {0x28});
        gGameControl = nullptr;
    }

    // Original function: FUN_00426a89. Shared by the fish and bio-regrow arrays.
    static void MapBioRegrowManager_FreeAll(void* manager) { FreeAllocatedRecords(manager, 0x14); }
    // Original function: FUN_004269a0.
    static void MapBioRegrowManager_Destruct(void* manager)
    {
        MapBioRegrowManager_FreeAll(manager);
        FreeAndClearField(manager, 0x30);
        gMapBioRegrowManager = nullptr;
        Write<std::uint8_t>(manager, 0, 0);
    }
    // Original function: FUN_00426e9b.
    static void MapFishManager_Destruct(void* manager)
    {
        MapBioRegrowManager_FreeAll(manager);
        FreeAndClearField(manager, 0x30);
        gMapFishManager = nullptr;
        Write<std::uint8_t>(manager, 0, 0);
    }
    // Original function: FUN_00427343.
    static void WallBuildManager_Destruct(void* manager)
    {
        for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0); ++index)
        {
            void* data = Read<void*>(Read<void*>(manager, 4), static_cast<std::uint32_t>(index) * 4);
            if (data != nullptr) NXBasics::Runtime_Free(data);
        }
        FreeField(manager, 4);
        gWallBuildManager = nullptr;
    }
    // Original function: FUN_00427fb8.
    static void RoadBuildManager_Destruct(void* manager)
    {
        for (std::int32_t index = 0; index < Read<std::int32_t>(manager, 0); ++index)
        {
            void* data = Read<void*>(Read<void*>(manager, 4), static_cast<std::uint32_t>(index) * 4);
            if (data != nullptr) NXBasics::Runtime_Free(data);
        }
        FreeField(manager, 4);
        gRoadBuildManager = nullptr;
    }
    // Original function: FUN_0044e65b.
    static void MapLandscapeAutoTransitionManager_Destruct(void*)
    {
        Unregister(FUN_0044eb1e, 0x65, {1, 0x33, 0x32, 0x31});
        gMapLandscapeAutoTransitionManager = nullptr;
    }
    // Original function: FUN_0042329f.
    void GameMap_Destruct(void*)
    {
        CCallbackManager::sTheObjectPtr->ExecuteCallback(2, false, 0, 0, 0);
        for (TCallbackFunction callback : {FUN_00424ae9, FUN_004513c1, FUN_004511c5, FUN_00450e8c, FUN_00450c1d})
        {
            Unregister(callback, 0, {0x33, 0x32, 0x31});
        }
        GameMap_ReleaseMapBuffers();
        DestroyOwned(gWallBuildManager, WallBuildManager_Destruct);
        DestroyOwned(gRoadBuildManager, RoadBuildManager_Destruct);
        DestroyOwned(gMapLandscapeAutoTransitionManager, MapLandscapeAutoTransitionManager_Destruct);
        DestroyOwned(gMapBioRegrowManager, MapBioRegrowManager_Destruct);
        DestroyOwned(gMapFishManager, MapFishManager_Destruct);
        DestroyOwned(CGameMapSectorAttachmentManager::sTheObjectPtr, [](CGameMapSectorAttachmentManager* manager) { manager->~CGameMapSectorAttachmentManager(); });
        DestroyOwned(CMapWalkSectorManager::sTheObjectPtr, [](CMapWalkSectorManager* manager) { manager->~CMapWalkSectorManager(); });
        std::memset(DAT_00511570, 0, sizeof(DAT_00511570));
        gMapPointDimensions.macroWidth = 0;
        gMapPointDimensions.macroHeight = 0;
        gMapPointDimensions.microWidth = 0;
        gMapPointDimensions.microHeight = 0;
        gGameMap = nullptr;
    }
    // Bridge for existing raw-record callers of SGameControlStartupData::Reset.
    void GameControlStartupData_Clear(void* data) { static_cast<SGameControlStartupData*>(data)->Reset(); }

    // Original function: FUN_004071c3.
    void Game_ShutDown()
    {
        if (gGameInitialized == 0) return;
        gGameInitialized = 0;
        DestroyOwned(gMissionOnscreenInfoManager, MissionOnscreenInfoManager_Destruct);
        DestroyOwned(gGameAssistantManager, GameAssistantManager_Destruct);
        DestroyOwned(gMultiPlayerGoalManager, MultiPlayerGoalManager_Destruct);
        DestroyOwned(gHappinessManager, HappinessManager_Destruct);
        DestroyOwned(gEmbeddedSavegameManager, EmbeddedSavegameManager_Destruct);
        DestroyOwned(gTributeManager, TributeManager_Destruct);
        DestroyOwned(gDelayedActionManager, DelayedActionManager_Destruct);
        DestroyOwned(gPlayerStatisticManager, PlayerStatisticManager_Destruct);
        DestroyOwned(gGuideManager, GuideManager_Destruct);
        DestroyOwned(CHouseArray::sTheObjectPtr, [](CHouseArray* array) { array->~CHouseArray(); });
        DestroyOwned(gMarkerArray, MarkerArray_Destruct);
        DestroyOwned(CVehicleArray::sTheObjectPtr, [](CVehicleArray* array) { array->~CVehicleArray(); });
        DestroyOwned(CAnimalArray::sTheObjectPtr, [](CAnimalArray* array) { array->~CAnimalArray(); });
        DestroyOwned(gHumanMerchantArray, HumanMerchantArray_Destruct);
        DestroyOwned(CHumanArray::sTheObjectPtr, [](CHumanArray* array) { array->~CHumanArray(); });
        DestroyOwned(gMapPathfinderManager, MapPathfinderManager_Destruct);
        DestroyOwned(gMapMoveableArrayStorage.manager, MapMoveableArray_Destruct);
        DestroyOwned(gAIManager, AIManager_Destruct);
        DestroyOwned(gMissionManager, MissionManager_Destruct);
        DestroyOwned(gGameMap, GameMap_Destruct);
        DestroyOwned(gGameControl, GameControl_Destruct);
        if (DAT_005109bc != nullptr)
        {
            DestroyOwned(DAT_005109bc, [](NXBasics::CIniFileReader* reader) { reader->~CIniFileReader(); });
            DAT_005109bc = nullptr;
        }
        if (DAT_005109c0 != nullptr)
        {
            NXBasics::CFile::FileSystem_FileLibrary_Remove(DAT_005109c0);
            DestroyOwned(DAT_005109c0, NXBasics::Archive_Destroy);
            DAT_005109c0 = nullptr;
        }
        gGameControlStartupData.Reset();
        UserNetworkCommandManager_EmptyQueue(gUserNetworkCommandManager);
    }

    // Original function: FUN_00406d85.
    void Game_Destruct(void*)
    {
        if (gGameInitialized != 0) Game_ShutDown();
        DestroyOwned(gUserNetworkCommandManager, UserNetworkCommandManager_Destruct);
        DestroyOwned(gGameMapModifyManager, GameMapModifyManager_Destruct);
        DestroyOwned(CCallbackManager::sTheObjectPtr, [](CCallbackManager* manager) { manager->~CCallbackManager(); });
        DestroyOwned(gStaticHumanJobExperienceTypeDataManager, StaticHumanJobExperienceTypeDataManager_Destruct);
        DestroyOwned(gStaticDependenciesManager, StaticDependenciesManager_Destruct);
        DestroyOwned(gStaticAnimalTypeDataManager, StaticAnimalTypeDataManager_Destruct);
        DestroyOwned(gStaticWeaponTypeDataManager, StaticWeaponTypeDataManager_Destruct);
        DestroyOwned(gStaticArmorTypeDataManager, StaticArmorTypeDataManager_Destruct);
        DestroyOwned(gStaticJobTypeDataManager, StaticJobTypeDataManager_Destruct);
        DestroyOwned(gStaticTribeTypeDataManager, StaticTribeTypeDataManager_Destruct);
        DestroyOwned(gStaticAtomicAnimationDataManager, StaticAtomicAnimationDataManager_Destruct);
        DestroyOwned(gStaticVehicleTypeDataManager, StaticVehicleTypeDataManager_Destruct);
        DestroyOwned(gStaticHouseTypeDataManager, StaticHouseTypeDataManager_Destruct);
        DestroyOwned(gStaticGoodTypeDataManager, [](CStaticGoodTypeDataManager* manager) { manager->~CStaticGoodTypeDataManager(); });
        DestroyOwned(gStaticTrianglePatternTypeDataManager, [](CStaticTrianglePatternTypeDataManager* manager) { manager->~CStaticTrianglePatternTypeDataManager(); });
        DestroyOwned(gStaticLandscapeTypeDataManager, [](CStaticLandscapeTypeDataManager* manager) { manager->~CStaticLandscapeTypeDataManager(); });
        DestroyOwned(gSavegamesManager, [](CSavegamesManager* manager) { manager->~CSavegamesManager(); });
        DestroyOwned(gCleanMapsManager, [](CCleanMapsManager* manager) { manager->~CCleanMapsManager(); });
        DestroyOwned(gCampaignStaticDataManager, [](CCampaignStaticDataManager* manager) { manager->~CCampaignStaticDataManager(); });
        DestroyOwned(gStaticStringsManager, [](CStaticStringsManager* manager) { manager->~CStaticStringsManager(); });
        gGame = nullptr;
    }
}
