#include "C2GameEngineStartup.h"
#include "ApplicationGlobals.h"
#include "C2LifetimeAccess.h"
#include "C2LogicLifetimeAccess.h"
#include "C2EngineLifetimeAccess.h"
#include "C2EngineDisplayAccess.h"
#include "MasterControlProgram.h"
#include "XCrtFileAccess.h"
#include "XIniFileReader.h"
#include "C2CallbackManager.h"
#include "C2E2AnimalSoundManager.h"
#include "C2E2LevelOfDetailManager.h"
#include "C2E2StaticDataAnimalSounds.h"
#include "C2E2StaticDataSoundFX.h"
#include "C2EngineDisplayManager.h"
#include "C2LogicObjectArrays.h"
#include "C2CampaignStaticDataManager.h"
#include "C2StaticStringsManager.h"
#include "C2CleanMapsManager.h"
#include "C2SavegamesManager.h"
#include "C2StaticTrianglePatternTypeData.h"
#include "C2StaticLandscapeTypeData.h"
#include "C2StaticGoodTypeData.h"
#include "C2StaticHouseTypeData.h"
#include "C2StaticVehicleTypeData.h"
#include "C2StaticAtomicAnimationData.h"
#include "C2StaticTribeTypeData.h"

#include <Windows.h>
#include <mmsystem.h>
#include <cstring>
#include <new>

namespace
{
    template<class Constructor, class... Arguments> void ConstructOriginal(std::uint32_t bytes, Constructor constructor, Arguments... arguments)
    {
        if (void* allocation = NXBasics::Runtime_AllocateWithNewHandler(bytes, 1)) constructor(allocation, nullptr, arguments...);
    }

    template<class Object> void ConstructNative(std::uint32_t bytes)
    {
        if (void* allocation = NXBasics::Runtime_AllocateWithNewHandler(bytes, 1)) ::new (allocation) Object();
    }
}

namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Unreconstructed original subconstructors. No replacement bodies are supplied.
    extern void* __fastcall FUN_0041300a(void*, void* unusedEdx);
    extern void* __fastcall FUN_00412a5d(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041277c(void*, void* unusedEdx);
    extern void* __fastcall FUN_00412299(void*, void* unusedEdx);
    extern void* __fastcall FUN_004121a9(void*, void* unusedEdx);
    extern void* __fastcall FUN_00411e81(void*, void* unusedEdx);
    extern void* __fastcall FUN_00410d14(void*, void* unusedEdx);
    extern void* __fastcall FUN_00408fdd(void*, void* unusedEdx, NXBasics::CIniFileReader*);
    extern void* __fastcall FUN_004231d7(void*, void* unusedEdx, std::int16_t, std::int16_t);
    extern void* __fastcall FUN_0041cf58(void*, void* unusedEdx);
    extern void* __fastcall FUN_00407c8b(void*, void* unusedEdx);
    extern void* __fastcall FUN_0040bb10(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041cd2f(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041c3aa(void*, void* unusedEdx);
    extern void* __fastcall FUN_0040b94a(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041ae53(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041aa59(void*, void* unusedEdx);
    extern void* __fastcall FUN_00419f41(void*, void* unusedEdx);
    extern void* __fastcall FUN_00419ddb(void*, void* unusedEdx);
    extern void* __fastcall FUN_00419bc5(void*, void* unusedEdx);
    extern void* __fastcall FUN_004196cd(void*, void* unusedEdx);
    extern void* __fastcall FUN_00419032(void*, void* unusedEdx);
    extern void* __fastcall FUN_004175ec(void*, void* unusedEdx);
    extern void* __fastcall FUN_0041714b(void*, void* unusedEdx);

    extern void FUN_004079cf();

    // Original function: FUN_00411ba6
    static void* __fastcall GameMapModifyManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gGameMapModifyManager = manager;
        gGameMapModifyNestingDepth = 0;
        return manager;
    }

    // Original function: FUN_00406b70
    void* Game_Construct(void* game)
    {
        gGame = game;
        std::memset(&gGameStaticVars, 0, sizeof(gGameStaticVars));
        // Fixed Windows seed has thirteen set bits; neither adjustment branch runs.
        gGameRandomState.seed = 0x18273645;
        gGameRandomState.currentState = 0x18273645;
        gGameStaticVars.DAT_005108b4 = 1;
        ConstructNative<CStaticStringsManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CCampaignStaticDataManager>(0x218);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CCleanMapsManager>(0x30);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CSavegamesManager>(0x20);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticLandscapeTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticTrianglePatternTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticGoodTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticHouseTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticVehicleTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticAtomicAnimationDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructNative<CStaticTribeTypeDataManager>(0x1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0041300a);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_00412a5d);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0041277c);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_00412299);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_004121a9);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_00411e81);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        static_assert(sizeof(NC2Logic::CCallbackManager) == 0x1A0);
        ConstructNative<NC2Logic::CCallbackManager>(0x1A0);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(1, GameMapModifyManager_Construct);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x9A0, FUN_00410d14);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        return game;
    }

    // Original function: FUN_00406f7d
    void Game_StartUp(std::int16_t width, std::int16_t height, bool enableTime, NXBasics::CIniFileReader* iniFile, bool networkGame)
    {
        if (gGameInitialized != 0)
        {
            std::byte savedStartupData[0x120];
            std::memcpy(savedStartupData, &gGameControlStartupData, sizeof(savedStartupData));
            Game_ShutDown();
            std::memcpy(&gGameControlStartupData, savedStartupData, sizeof(savedStartupData));
        }
        gGameRandomState.seed = 0x18273645;
        gGameRandomState.currentState = 0x18273645;
        gGameStaticVars.DAT_005108b4 = 1;
        GameTime_Reset(false);
        gGameStaticVars.DAT_005109c4 = static_cast<std::uint8_t>(networkGame);
        ConstructOriginal(0x5D204, FUN_00408fdd, iniFile);
        ConstructOriginal(0x1, FUN_004231d7, width, height);
        ConstructOriginal(0x81CAC, FUN_0041cf58);
        ConstructOriginal(0xA0, FUN_00407c8b);
        ConstructOriginal(0x30, FUN_0040bb10);
        ConstructOriginal(0x30, FUN_0041cd2f);
        static_assert(sizeof(NC2Logic::CHumanArray) == 0x30);
        ConstructNative<NC2Logic::CHumanArray>(0x30);
        ConstructOriginal(0x5D8, FUN_0041c3aa);
        static_assert(sizeof(NC2Logic::CAnimalArray) == 0x30);
        ConstructNative<NC2Logic::CAnimalArray>(0x30);
        static_assert(sizeof(NC2Logic::CVehicleArray) == 0x30);
        ConstructNative<NC2Logic::CVehicleArray>(0x30);
        ConstructOriginal(0x30, FUN_0040b94a);
        static_assert(sizeof(NC2Logic::CHouseArray) == 0x30);
        ConstructNative<NC2Logic::CHouseArray>(0x30);
        ConstructOriginal(0x1368, FUN_0041ae53);
        ConstructOriginal(0xA8, FUN_0041aa59);
        ConstructOriginal(0x34, FUN_00419f41);
        ConstructOriginal(0x960, FUN_00419ddb);
        ConstructOriginal(0x8, FUN_00419bc5);
        ConstructOriginal(0xAA0, FUN_004196cd);
        ConstructOriginal(0x740, FUN_00419032);
        ConstructOriginal(0x460, FUN_004175ec);
        ConstructOriginal(0xFA0, FUN_0041714b);
        CCleanMapsManager::Tool_GenerateNewGuid(&gGameStaticVars.mapGuid);
        Write<std::uint32_t>(&gGameStaticVars, 0x18, 2);
        GameTime_Reset(enableTime);
        Write<std::uint32_t>(&gGameStaticVars, 0x88, 0);
        gGameInitialized = 1;
    }

    // Original function: FUN_004075ab; distinct tail-forwarding entry.
    void Misc_EngineIsInitialised() { CStaticGoodTypeDataManager::Misc_GetEngineLandscapeIds(); }

    // Original function: FUN_0040796b; Mac l_GameTime_Reset.
    void GameTime_Reset(bool enabled)
    {
        std::memset(reinterpret_cast<std::byte*>(&gGameStaticVars) + 0x20, 0, 0x68);
        DAT_00510838 = 1;
        Write<double>(&gGameStaticVars, 0x28, static_cast<double>(timeGetTime()));
        Write<double>(&gGameStaticVars, 0x40, 12.0);
        GameTime_SetSpeed(enabled ? 12.0 : 0.0);
        if (enabled) FUN_004079cf();
    }

    // Original function: FUN_00407be0
    void GameTime_SetSpeed(double speed)
    {
        // The original JAE branch treats an unordered old speed like a small speed.
        if (!(DAT_00510848 >= 0.1)) Write<double>(&gGameStaticVars, 0x28, static_cast<double>(timeGetTime()));
        if (DAT_00510848 > 1.0) Write<double>(&gGameStaticVars, 0x40, DAT_00510848);
        DAT_00510848 = speed;
        Write<double>(&gGameStaticVars, 0x38, speed > 0.1 ? 1000.0 / speed : 0.0);
        DAT_0051089d = 0;
    }

    // Original function: FUN_00411bcc
    void GameMapModifyManager_StartGameMapModify()
    {
        if (++gGameMapModifyNestingDepth == 1) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x31, false, 0, 0, 0);
    }

    // Original function: FUN_00411bef
    void GameMapModifyManager_StopGameMapModify()
    {
        if (gGameMapModifyNestingDepth == 0) return;
        if (gGameMapModifyNestingDepth == 1) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x32, true, 0, 0, 0);
        --gGameMapModifyNestingDepth;
    }

    // Original function: FUN_00411c18
    void GameMapModifyManager_ForceUpdate()
    {
        if (gGameMapModifyNestingDepth == 0) return;
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x32, true, 0, 0, 0);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x31, false, 0, 0, 0);
    }
}

namespace NC2E2
{
    using namespace NC2Lifetime;
    // Unreconstructed original subconstructors. No replacement bodies are supplied.
    extern void* __fastcall FUN_004905b9(void*, void* unusedEdx);
    extern void* __fastcall FUN_0049039e(void*, void* unusedEdx);
    extern void* __fastcall FUN_00480a20(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048f79f(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048e651(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048e24e(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048d714(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048d194(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048bd66(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048acfb(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048ab5c(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048a4ba(void*, void* unusedEdx);
    extern void* __fastcall FUN_0048a028(void*, void* unusedEdx);
    extern void* __fastcall FUN_00489de0(void*, void* unusedEdx);
    extern void* __fastcall FUN_00486336(void*, void* unusedEdx);
    extern void* __fastcall FUN_0047f3c7(void*, void* unusedEdx);
    extern void* __fastcall FUN_00485aa1(void*, void* unusedEdx);
    extern void* __fastcall FUN_00485453(void*, void* unusedEdx);
    extern void* __fastcall FUN_00484bd1(void*, void* unusedEdx);

    // Original callbacks remain unresolved; their verified signatures match the callback manager.
    extern bool __cdecl FUN_0047e936(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047ea9f(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047eaf3(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl FUN_0047ec5b(NC2Logic::TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    // Original function: FUN_0047dcc4
    void* E2Manager_Construct(void* manager)
    {
        gE2Manager = manager;
        std::memset(manager, 0, 1);
        std::memset(DAT_00554d98, 0, sizeof(DAT_00554d98));
        gDesktopOwnedBobBitmap = nullptr;
        ConstructOriginal(0x1C, FUN_004905b9);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x14, FUN_0049039e);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_00480a20);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0xFA4, FUN_0048f79f);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048e651);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0xFA4, FUN_0048e24e);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        static_assert(sizeof(NC2E2::CE2StaticDataSoundFXManager) == 0x518);
        ConstructNative<NC2E2::CE2StaticDataSoundFXManager>(0x518);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048d714);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048d194);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1BA58, FUN_0048bd66);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048acfb);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048ab5c);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        static_assert(sizeof(NC2E2::CE2StaticDataAnimalSoundManager) == 0x8);
        ConstructNative<NC2E2::CE2StaticDataAnimalSoundManager>(0x8);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048a4ba);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_0048a028);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        static_assert(sizeof(NC2E2::CLevelOfDetailManager) == 0x1);
        ConstructNative<NC2E2::CLevelOfDetailManager>(0x1);
        ConstructOriginal(0x8, FUN_00489de0);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x2, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x3, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x4, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x5, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x6, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x7, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x8, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x9, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0xA, E2Manager_CallbackSomethingInLogicXtructed, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0xE, FUN_0047e936, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0xF, FUN_0047e936, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x10, FUN_0047e936, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x11, FUN_0047e936, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x12, FUN_0047ea9f, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x13, FUN_0047ea9f, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x14, FUN_0047ea9f, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x15, FUN_0047ea9f, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x16, FUN_0047ea9f, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x31, FUN_0047eaf3, 0, 0x2);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x32, FUN_0047eaf3, 0, 0x2);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x33, FUN_0047eaf3, 0, 0x2);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0xD, FUN_0047eaf3, 0, 0x2);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x17, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x1C, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x21, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x22, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x1F, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x20, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x23, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x27, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x24, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x1D, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x18, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x25, FUN_0047ec5b, 0, 0xA);
        NC2Logic::CCallbackManager::sTheObjectPtr->RegisterCallback(0x26, FUN_0047ec5b, 0, 0xA);
        static_assert(sizeof(NC2E2::C2DEngineDisplayManager) == 0x18);
        ConstructNative<NC2E2::C2DEngineDisplayManager>(0x18);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x2F7F80, FUN_00486336);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x64, FUN_0047f3c7);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        static_assert(sizeof(NC2E2::CE2AnimalSoundManager) == 0x8C);
        ConstructNative<NC2E2::CE2AnimalSoundManager>(0x8C);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0x1, FUN_00485aa1);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0xD24, FUN_00485453);
        NC2LogicExternals::External_Misc_ProgressBar_Update();
        ConstructOriginal(0xC, FUN_00484bd1);
        return manager;
    }
}

