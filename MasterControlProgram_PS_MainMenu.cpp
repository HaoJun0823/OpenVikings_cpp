#include "MainMenuGuiManager.h"
#include "CDAudioManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "X2DApplication.h"

#include <cstddef>
#include <cstdint>
#include <new>
#include <Windows.h>

namespace NMasterControlProgram
{

    // Original function: FUN_0040212b
    bool CMasterControlProgram::System_ProgramState_MainMenu_StartUp()
    {
        mProgramState = 7;

        ProgressBar_Init(mEngineInitialized == 0);

        new (std::nothrow) NC2MainMenuGuiManager::CGuiManager();

        FUN_00401985(this);

        const int displayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

        const int displayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();

        const int displayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();

        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->Desktop_Open(displayWidth, displayHeight, static_cast<std::uint32_t>(displayDepth));

        FUN_00402298(this);
        ProgressBar_Exit();

        if (NCDAudioManager::CCDAudioManager::sTheObjectPtr != nullptr)
        {
            NCDAudioManager::CCDAudioManager::sTheObjectPtr->StartTrack(2);
        }

        mProgramState = static_cast<std::int32_t>(TProgramState::MainMenu);

        constexpr const char* propertyName = "start_campaign_3_screen";

        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists(propertyName))
        {
            NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->Screen_ChangeTo(5, false);
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove(propertyName);
        }

        return true;
    }

    // Original function: FUN_004021ea
    void CMasterControlProgram::System_ProgramState_MainMenu_ShutDown()
    {
        mProgramState = 9;

        if (NCDAudioManager::CCDAudioManager::sTheObjectPtr != nullptr)
        {
            if (NCDAudioManager::CCDAudioManager::sTheObjectPtr->IsStillPlaying())
            {
                NCDAudioManager::CCDAudioManager::sTheObjectPtr->StopTrack();
            }
        }

        FUN_004800c8(DAT_005550d4);
        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->Desktop_Close();
        GameEngine_ShutDownGame();

        auto* mainMenuGuiManager = NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr;

        if (mainMenuGuiManager != nullptr)
        {
            delete mainMenuGuiManager;
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
        NC2MainMenuGuiManager::CGuiManager::sTheObjectPtr->System_UpdateGui();

        auto* displayBitmap = NXBaseGui::CDesktop::sTheObjectPtr->GetDisplayBitmap();

        NXSys2DApp::CApplication::sTheObjectPtr->BitmapToScreen(static_cast<NXBasics::CBitmap*>(displayBitmap), 0, 0);
    }
}
