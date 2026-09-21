#include "X2DApplication.h"
#include "MainMenuGuiManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include <new>

namespace NMasterControlProgram
{

    // Original function: FUN_0040212b
    bool CMasterControlProgram::System_ProgramState_MainMenu_StartUp()
    {
        mProgramState = 7;

        ProgressBar_Init(mEngineInitialized == 0);

        void* guiManagerMemory = ::operator new(0x21B4, std::nothrow);

        if (guiManagerMemory != nullptr)
        {
            FUN_004d2b2d(guiManagerMemory);
        }

        FUN_00401985(this);

        const int displayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

        const int displayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();

        const int displayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();

        FUN_004d2e77(DAT_005694dc, displayWidth, displayHeight, displayDepth);

        FUN_00402298(this);
        ProgressBar_Exit();

        if (DAT_0050f69c != nullptr)
        {
            FUN_00403f85(DAT_0050f69c, 2);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::MainMenu);

        constexpr const char* propertyName = "start_campaign_3_screen";

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists(propertyName))
        {
            reinterpret_cast<NC2MainMenuGuiManager::CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(5, false);
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove(propertyName);
        }

        return true;
    }

    // Original function: FUN_004021ea
    void CMasterControlProgram::System_ProgramState_MainMenu_ShutDown()
    {
        mProgramState = 9;

        if (DAT_0050f69c != nullptr)
        {
            if (FUN_00404070(DAT_0050f69c))
            {
                FUN_0040401e(DAT_0050f69c);
            }
        }

        FUN_004800c8(DAT_005550d4);
        FUN_004d2fdc(DAT_005694dc);
        FUN_00401bd5(this);

        auto* mainMenuGuiManager = DAT_005694dc;

        if (mainMenuGuiManager != nullptr)
        {
            FUN_004d2ba8(mainMenuGuiManager);
            ::operator delete(mainMenuGuiManager);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::None);
    }

    // Original function: FUN_00402250
    void CMasterControlProgram::System_ProgramState_MainMenu_Update()
    {
        const DWORD currentTime = timeGetTime();
        const auto* objectBytes = reinterpret_cast<const std::byte*>(this);
        const DWORD lastMapChangeTime = *reinterpret_cast<const DWORD*>(objectBytes + 0x164);

        if (lastMapChangeTime + 300000U <= currentTime)
        {
            FUN_00402298(this);
        }

        thunk_FUN_004077b8();
        reinterpret_cast<NC2MainMenuGuiManager::CGuiManager*>(DAT_005694dc)->System_UpdateGui();

        const auto* desktopBytes = reinterpret_cast<const std::byte*>(DAT_00554d5c);
        const std::uint32_t displayBitmap = *reinterpret_cast<const std::uint32_t*>(desktopBytes + 0x10);

        FUN_00479052(NXSys2DApp::CApplication::sTheObjectPtr, displayBitmap, nullptr, 0);
    }
}
