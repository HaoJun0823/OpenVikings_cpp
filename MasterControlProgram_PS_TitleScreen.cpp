#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "C2LanguageTools.h"
#include "C2LifetimeAccess.h"
#include "WindowsVideoPlayer.h"
#include "X2DApplication.h"
#include "X2DWindow.h"
#include "XBitmap.h"
#include "XPicture.h"
#include "XPalette.h"
#include "XFont.h"
#include "XStorableIO.h"
#include <mmsystem.h>
#include "XMKeyManager.h"
#include "XMMouseManager.h"
#include "XCrtFileAccess.h"
#include <cstring>
#include <new>

namespace
{
    using namespace NC2Lifetime;
    // Complete 0050F630..0050F637 region: cleared byte, padding, manager pointer.
    struct STitleScreenStaticVars { std::uint8_t state; std::byte padding[3]; void* manager; };
    static STitleScreenStaticVars gTitleScreen{};
    static const GUID gLegacyDrawInterface = {0x6C14DB80, 0xA733, 0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};
    static const GUID gLegacySurfaceInterface = {0x6C14DB81, 0xA733, 0x11CE, {0xA5, 0x21, 0x00, 0x20, 0xAF, 0x0B, 0xE5, 0x60}};

    // Verified application +4C, window +8, display +18/+20 COM ownership chain.
    static IUnknown* TitleScreen_DisplayInterface(std::uint32_t offset)
    {
        auto* window = Read<NXSys2DApp::CWindow*>(NXSys2DApp::CApplication::sTheObjectPtr, 0x4C);
        return Read<IUnknown*>(window->GetDisplay(), offset);
    }

    // Repeated original bitmap allocation sequence; callers retain their lifetimes.
    static NXBasics::CBitmap* TitleScreen_CreateBitmap(std::int32_t width, std::int32_t height, std::int32_t depth, void* memory)
    {
        return memory != nullptr ? ::new (memory) NXBasics::CBitmap(width, height, depth) : nullptr;
    }

    // Original function: FUN_00402e88.
    static void TitleScreen_GfxData_Load(void* manager)
    {
        char fileName[0x104];
        NC2Logic::LanguageTool_GetFilename("data\\gui\\lang\\%s\\pictures\\titlescreen.bmp", fileName, false);
        {
            NXBasics::CPicture picture(fileName);
            auto* memory = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
            NXBasics::CBitmap* bitmap = nullptr;
            if (memory != nullptr)
            {
                auto* widthBitmap = picture.GetBitmap();
                auto* heightBitmap = widthBitmap;
                const auto depth = NXBasics::ConvertBitsToTDepth(NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth());
                bitmap = TitleScreen_CreateBitmap(widthBitmap->GetRectangle().width, heightBitmap->GetRectangle().height, depth, memory);
            }
            Write(manager, 0xD8, bitmap);
            picture.GetBitmap()->CopyIntoBitmap(bitmap, 0, 0);
        }
        auto* memory = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
        NXBasics::CBitmap* bitmap = nullptr;
        if (memory != nullptr)
        {
            auto* heightApplication = NXSys2DApp::CApplication::sTheObjectPtr;
            auto* widthApplication = heightApplication;
            const auto depth = NXBasics::ConvertBitsToTDepth(heightApplication->GetDisplayDepth());
            const auto height = heightApplication->GetDisplayHeight();
            const auto width = widthApplication->GetDisplayWidth();
            bitmap = TitleScreen_CreateBitmap(width, height, depth, memory);
        }
        Write(manager, 0xDC, bitmap);
        bitmap->Fill(NXBasics::SColorRGB(0, 0, 0));
        auto* font = static_cast<NXBasics::CFont*>(NXBasics::XB_Storable_LoadObject("data\\gui\\fonts\\font10.fnt"));
        Write(manager, 0xE0, font);
        Write<std::uint32_t>(font, 0xC, 0xFFFFFFFFu);
        memory = NXBasics::Runtime_AllocateWithNewHandler(0x40C, 1);
        auto* palette = memory != nullptr ? ::new (memory) NXBasics::CPalette() : nullptr;
        Write(manager, 0xE4, palette);
        for (std::uint32_t index = 0; index < 256; ++index) reinterpret_cast<NXBasics::SColorRGB*>(reinterpret_cast<std::byte*>(palette) + 4 + index * 4)->SetColors(static_cast<std::uint8_t>(index), 0, 0);
    }

    // Original function: FUN_00402fcb; resource fields remain unchanged.
    static void TitleScreen_GfxData_Free(void* manager)
    {
        if (auto* palette = Read<NXBasics::CPalette*>(manager, 0xE4)) delete palette;
        if (auto* font = Read<NXBasics::CFont*>(manager, 0xE0)) delete font;
        if (auto* bitmap = Read<NXBasics::CBitmap*>(manager, 0xDC)) delete bitmap;
        if (auto* bitmap = Read<NXBasics::CBitmap*>(manager, 0xD8)) delete bitmap;
    }

    // Original function: FUN_00402af2.
    static bool TitleScreen_FMV_Start(void* manager)
    {
        if (Read<std::uint8_t>(manager, 0) == 0) return false;
        char pattern[0x104], fileName[0x104];
        NXBasics::Runtime_PrintString(pattern, "data\\\\FMV\\%%s\\%s.mpg", static_cast<const char*>(manager) + 1);
        if (!NC2Logic::LanguageTool_GetFilename(pattern, fileName, false)) return false;
        Read<NXBasics::CBitmap*>(manager, 0xDC)->Fill(NXBasics::SColorRGB(0, 0, 0));
        if (NWindowsVideo::CVideoPlayer::sTheObjectPtr == nullptr)
        {
            if (auto* memory = NXBasics::Runtime_AllocateWithNewHandler(0x2C, 1)) new (memory) NWindowsVideo::CVideoPlayer();
        }
        IDirectDraw* directDraw = nullptr;
        TitleScreen_DisplayInterface(0x18)->QueryInterface(gLegacyDrawInterface, reinterpret_cast<void**>(&directDraw));
        if (directDraw != nullptr)
        {
            auto* video = NWindowsVideo::CVideoPlayer::sTheObjectPtr;
            const bool enableAudio = !NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("fx_off");
            if (video->Open(fileName, directDraw, enableAudio))
            {
                NWindowsVideo::CVideoPlayer::sTheObjectPtr->Start();
                Write<std::uint8_t>(manager, 0xC9, 1);
            }
            directDraw->Release();
        }
        if (Read<std::uint8_t>(manager, 0xC9) == 0) return false;
        NXSysMouseManager::CMouseManager::sTheObjectPtr->SetHideSystemMouseCursorWhenFullscreenState(true);
        Write<std::uint8_t>(manager, 0xCA, 1);
        return true;
    }

    // Original function: FUN_00402c08; the force argument is unused by Windows.
    static void TitleScreen_UpdateDisplay(void* manager, bool /*force*/)
    {
        if (Read<std::uint8_t>(manager, 0xC9) != 0)
        {
            auto* video = NWindowsVideo::CVideoPlayer::sTheObjectPtr;
            if (video != nullptr && video->IsPlaying())
            {
                IDirectDrawSurface* surface = nullptr;
                TitleScreen_DisplayInterface(0x20)->QueryInterface(gLegacySurfaceInterface, reinterpret_cast<void**>(&surface));
                if (surface != nullptr)
                {
                    NWindowsVideo::CVideoPlayer::sTheObjectPtr->Update(surface, nullptr);
                    surface->Release();
                    return;
                }
            }
        }
        else Read<NXBasics::CBitmap*>(manager, 0xD8)->CopyIntoBitmap_FitIn(Read<NXBasics::CBitmap*>(manager, 0xDC));
        NXSys2DApp::CApplication::sTheObjectPtr->BitmapToScreen(Read<NXBasics::CBitmap*>(manager, 0xDC), 0, 0);
    }

    // Original function: FUN_00402c90.
    static void TitleScreen_ToNextMode(void* manager)
    {
        if (Read<std::uint8_t>(manager, 0xC9) != 0)
        {
            NWindowsVideo::CVideoPlayer::sTheObjectPtr->Stop();
            Write<std::uint8_t>(manager, 0xC9, 0);
        }
        if (Read<std::uint8_t>(manager, 0xCB) != 0)
        {
            Write<std::uint8_t>(manager, 0xCB, 0);
            std::strcpy(static_cast<char*>(manager) + 1, "intro");
            if (TitleScreen_FMV_Start(manager)) return;
        }
        if (Read<std::uint8_t>(manager, 0) != 0)
        {
            NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->DisplayResolution_ResetToUserResolution();
            TitleScreen_GfxData_Free(manager);
            TitleScreen_GfxData_Load(manager);
        }
        bool forceChange = false;
        if (Read<std::uint8_t>(manager, 0xCC) != 0 || Read<std::uint8_t>(manager, 0xCD) != 0)
        {
            const auto index = Read<std::uint32_t>(manager, 0xD0) + 1u;
            Write(manager, 0xD0, index);
            const char* prefix = Read<std::uint8_t>(manager, 0xCC) != 0 ? "pre" : "post";
            char fileName[0xC8];
            NXBasics::Runtime_PrintString(fileName, "data\\\\pictures\\%s%2.2d.bmp", prefix, NXBasics::Runtime_TimeSigned32(index));
            if (NXBasics::CFile::FileSystem_Tool_FileExists(fileName, true))
            {
                if (auto* bitmap = Read<NXBasics::CBitmap*>(manager, 0xD8)) delete bitmap;
                Write<void*>(manager, 0xD8, nullptr);
                NXBasics::CPicture picture(fileName);
                auto* memory = NXBasics::Runtime_AllocateWithNewHandler(0x54, 1);
                NXBasics::CBitmap* bitmap = nullptr;
                if (memory != nullptr)
                {
                    auto* heightBitmap = picture.GetBitmap();
                    auto* widthBitmap = heightBitmap;
                    const auto depth = NXBasics::ConvertBitsToTDepth(NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth());
                    bitmap = TitleScreen_CreateBitmap(widthBitmap->GetRectangle().width, heightBitmap->GetRectangle().height, depth, memory);
                }
                Write(manager, 0xD8, bitmap);
                picture.GetBitmap()->CopyIntoBitmap(bitmap, 0, 0);
                Write<std::uint32_t>(manager, 0xD4, timeGetTime());
                return;
            }
            if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("game_start_pre_intro"))
            {
                NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove("game_start_pre_intro");
                NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_ProgramState_ChangeNextTimePossible(NMasterControlProgram::TProgramState::TitleScreen, true);
                return;
            }
            if (Read<std::uint8_t>(manager, 0xCD) != 0)
            {
                Write<std::uint8_t>(NMasterControlProgram::CMasterControlProgram::sTheObjectPtr, 9, 1);
                return;
            }
            forceChange = true;
        }
        for (std::uint32_t frame = 0; frame < 5; ++frame)
        {
            TitleScreen_UpdateDisplay(manager, forceChange);
            NXSys2DApp::CApplication::sTheObjectPtr->UpdateDisplay();
        }
        const bool startGame = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("start_map") || NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("skip_main_menu");
        NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_ProgramState_ChangeNextTimePossible(startGame ? NMasterControlProgram::TProgramState::Game : NMasterControlProgram::TProgramState::MainMenu, false);
    }

    // Original function: FUN_004027d2.
    static void* TitleScreen_Construct(void* manager)
    {
        gTitleScreen.manager = manager;
        std::memset(manager, 0, 0xE8);
        gTitleScreen.state = 0;
        Write<std::uint8_t>(manager, 0, 0);
        if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("game_start_pre_picture_sequence"))
        {
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove("game_start_pre_picture_sequence");
            if (NXBasics::CFile::FileSystem_Tool_FileExists("data\\\\pictures\\pre00.bmp", true))
            {
                Write<std::uint32_t>(manager, 0xD0, 0xFFFFFFFFu);
                Write<std::uint8_t>(manager, 0xCC, 1);
                NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("game_start_pre_intro", false, 0, nullptr);
            }
        }
        if (Read<std::uint8_t>(manager, 0xCC) == 0)
        {
            if (!NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("intro_skip"))
            {
                Write<std::uint8_t>(manager, 0, 1);
                std::strcpy(static_cast<char*>(manager) + 1, "intro_00");
                Write<std::uint8_t>(manager, 0xCB, 1);
            }
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("intro_skip", false, 0, nullptr);
            if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("start_fmv"))
            {
                const char* caption = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetStringValuePtr("start_fmv");
                Write<std::uint8_t>(manager, 0, 1);
                std::strcpy(static_cast<char*>(manager) + 1, caption);
                NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove("start_fmv");
            }
        }
        if (Read<std::uint8_t>(manager, 0) == 0 && NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("game_start_post_picture_sequence"))
        {
            NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Remove("game_start_post_picture_sequence");
            // Requested missing-asset fallback: the native sequence end already
            // requests program exit when the next post picture does not exist.
            Write<std::uint32_t>(manager, 0xD0, 0xFFFFFFFFu);
            Write<std::uint8_t>(manager, 0xCD, 1);
        }
        if (Read<std::uint8_t>(manager, 0) != 0)
        {
            std::uint32_t depth = 32;
            if (!(NXSys2DApp::CApplication::sTheObjectPtr->IsFullscreen() || NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth() == 32) || (NXSys2DApp::CApplication::sTheObjectPtr->IsFullscreen() && !NXSys2DApp::CApplication::sTheObjectPtr->CheckFullscreenDisplayMode(640, 480, 32))) depth = 16;
            if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("intro_screen_depth")) depth = static_cast<std::uint32_t>(NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("intro_screen_depth"));
            NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->DisplayResolution_SetDirectly(640, 480, depth, false, false, true);
        }
        TitleScreen_GfxData_Load(manager);
        if (Read<std::uint8_t>(manager, 0) != 0) TitleScreen_FMV_Start(manager);
        else if (Read<std::uint8_t>(manager, 0xCC) != 0 || Read<std::uint8_t>(manager, 0xCD) != 0 || NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("tincat") || NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("game_pre_stuff_skip")) TitleScreen_ToNextMode(manager);
        return manager;
    }

    // Original function: FUN_00402a15.
    static void TitleScreen_Destruct(void* manager)
    {
        if (Read<std::uint8_t>(manager, 0xCA) != 0) NXSysMouseManager::CMouseManager::sTheObjectPtr->SetHideSystemMouseCursorWhenFullscreenState(false);
        if (auto* video = NWindowsVideo::CVideoPlayer::sTheObjectPtr)
        {
            video->~CVideoPlayer();
            NXBasics::Runtime_Free(video);
        }
        TitleScreen_GfxData_Free(manager);
        gTitleScreen.manager = nullptr;
    }

    // Original function: FUN_00402a58.
    static void TitleScreen_System_Update(void* manager)
    {
        bool advance = Read<std::uint8_t>(manager, 0) != 0 && (Read<std::uint8_t>(manager, 0xC9) == 0 || !NWindowsVideo::CVideoPlayer::sTheObjectPtr->IsPlaying());
        if (!advance && (Read<std::uint8_t>(manager, 0xCD) != 0 || Read<std::uint8_t>(manager, 0xCC) != 0)) advance = timeGetTime() > Read<std::uint32_t>(manager, 0xD4) + 20000u;
        if (advance) { TitleScreen_ToNextMode(manager); return; }
        NXSysKeyManager::SKeyMessage message;
        if ((NXSysKeyManager::CKeyManager::sTheObjectPtr->GetKey(message) && message.state != 0 && (message.auxiliaryCode == 0x20 || message.keyCode == 0x1B)) || NXSysMouseManager::CMouseManager::sTheObjectPtr->IsAnyButtonDown())
        {
            NXSysKeyManager::CKeyManager::sTheObjectPtr->Init();
            TitleScreen_ToNextMode(manager);
        }
        else TitleScreen_UpdateDisplay(manager, false);
    }
}

namespace NMasterControlProgram
{
    // Original function: FUN_00402773.
    bool CMasterControlProgram::System_ProgramState_TitleScreen_StartUp()
    {
        mProgramState = 4;
        if (auto* memory = NXBasics::Runtime_AllocateWithNewHandler(0xE8, 1)) TitleScreen_Construct(memory);
        mProgramState = static_cast<std::int32_t>(TProgramState::TitleScreen);
        return true;
    }

    // Original function: FUN_0040279c.
    void CMasterControlProgram::System_ProgramState_TitleScreen_ShutDown()
    {
        mProgramState = 6;
        if (void* manager = gTitleScreen.manager)
        {
            TitleScreen_Destruct(manager);
            NXBasics::Runtime_Free(manager);
        }
        mProgramState = static_cast<std::int32_t>(TProgramState::None);
    }

    // Original function: FUN_004027c7.
    void CMasterControlProgram::System_ProgramState_TitleScreen_Update()
    {
        TitleScreen_System_Update(gTitleScreen.manager);
    }
}
