#include "MasterControlProgram.h"
#include "X2DApplication.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

namespace NMasterControlProgram
{
	CMasterControlProgram* CMasterControlProgram::sTheObjectPtr = nullptr;

	// Original function: FUN_00401000
	CMasterControlProgram::CMasterControlProgram(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
	{
		FUN_00406b33(reinterpret_cast<std::byte*>(this) + 0x2C);

		sTheObjectPtr = this;

		std::memset(this, 0, 0x168);
		std::memset(&DAT_0050f620, 0, 1);

		mProgramState = 3;
		mWantedProgramState = 0;

		new (std::nothrow) NXBasics::CXBSystemManager();

		FUN_004065e2(reinterpret_cast<std::uint32_t*>(&DAT_004fe208));

		new (std::nothrow) NMasterPropertyManager::CPropertyManager();

		NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Options_Load();
		NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_WorkOnIniFile("game.ini");

		FUN_004e1463(NMasterPropertyManager::CPropertyManager::sTheObjectPtr, commandLine);

		NMasterPropertyManager::CPropertyManager::sTheObjectPtr->System_AddDefaultProperties();

		bool customDataFileLoaded = false;
		char fileName[28];

		for (int index = 9; index >= 0; --index)
		{
			DexterString::StringPrint(fileName, "use_data_file_%d", index);

			const char* configuredFile = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetStringValuePtr(fileName);

			if (configuredFile == nullptr || std::strlen(configuredFile) == 0)
			{
				continue;
			}

			if (!NXBasics::CFile::FileSystem_Tool_FileExists(configuredFile, false))
			{
				continue;
			}

			auto* fileLibrary = new (std::nothrow) NXBasics::CSimpleFileLibrary(configuredFile);
			NXBasics::CFile::FileSystem_FileLibrary_Add(fileLibrary);
			customDataFileLoaded = true;
		}

		if (!customDataFileLoaded)
		{
			for (int index = 10; index >= 1; --index)
			{
				DexterString::StringPrint(fileName, "datax\\libs\\data%4.4d.lib", index);

				if (!NXBasics::CFile::FileSystem_Tool_FileExists(fileName, false))
				{
					continue;
				}

				auto* fileLibrary = new (std::nothrow) NXBasics::CSimpleFileLibrary(fileName);
				NXBasics::CFile::FileSystem_FileLibrary_Add(fileLibrary);
			}
		}

		NXSys2DApp::SApplicationControl applicationControl;

		applicationControl.instance = instance;
		applicationControl.previousInstance = previousInstance;
		applicationControl.commandLine = commandLine;
		applicationControl.showCommand = showCommand;
		applicationControl.fullscreen = true;
		applicationControl.fullscreenToggle = false;

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("gfx_fullscreen"))
		{
			applicationControl.fullscreen = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("gfx_fullscreen") != 0;
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("gfx_fullscreen_toggle") ||
			!applicationControl.fullscreen)
		{
			applicationControl.fullscreenToggle = true;
		}

		applicationControl.screenWidth = 800;
		applicationControl.screenHeight = 600;
		applicationControl.screenDepth = 16;

		applicationControl.minimumScreenWidth = 800;
		applicationControl.minimumScreenHeight = 600;
		applicationControl.minimumScreenDepth = 16;

		applicationControl.maximumScreenWidth = 10000;
		applicationControl.maximumScreenHeight = 10000;
		applicationControl.maximumScreenDepth = 16;

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("gfx_screen_width"))
		{
			applicationControl.screenWidth = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("gfx_screen_width");
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("gfx_screen_height"))
		{
			applicationControl.screenHeight = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("gfx_screen_height");
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("gfx_screen_depth"))
		{
			applicationControl.screenDepth = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("gfx_screen_depth");
		}

		new (std::nothrow) NXSys2DApp::CApplication(applicationControl);

		NXBasics::CFile::FileSystem_SystemPathName_Set("$gameroot$", ".");

		char resolvedPath[MAX_PATH];

		if (NXBasics::CFile::FileSystem_PathName_Resolve("datax\\mouse\\mousenormal.cur", resolvedPath))
		{
			mMouseCursors[0] = LoadCursorFromFileA(resolvedPath);
		}

		if (NXBasics::CFile::FileSystem_PathName_Resolve("datax\\mouse\\mousepressed.cur", resolvedPath))
		{
			mMouseCursors[1] = LoadCursorFromFileA(resolvedPath);
		}

		if (NXBasics::CFile::FileSystem_PathName_Resolve("datax\\mouse\\mouseright.cur", resolvedPath))
		{
			mMouseCursors[2] = LoadCursorFromFileA(resolvedPath);
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("set_language"))
		{
			const int language = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("set_language");

			NC2Logic::LanguageTool_SetCurrentLanguage(language);
		}

		FUN_00405a04();

		if (!NXSys2DApp::CApplication::sTheObjectPtr->StartUp())
		{
			return;
		}

		mDisplayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();
		mDisplayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();
		mDisplayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

		HINSTANCE applicationInstance = NXSys2DApp::CApplication::sTheObjectPtr->GetInstance();
		HWND window = NXSys2DApp::CApplication::sTheObjectPtr->GetWindowHandle();

		HICON smallIcon = LoadIconA(applicationInstance, MAKEINTRESOURCEA(101));
		SendMessageA(window, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(smallIcon));

		HICON largeIcon = LoadIconA(applicationInstance, MAKEINTRESOURCEA(102));
		SendMessageA(window, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(largeIcon));

		SendMessageA(window, WM_SETTEXT, 0, reinterpret_cast<LPARAM>("Weltwunder"));

		HardwareMouseCursor_Enable();

		if (mHardwareMouseCursorEnabled)
		{
			NXSysMisc::XWS_SystemMouseCursor_SetCustom(mMouseCursors[0]);
		}

		HardwareMouseCursor_ReSet();

		if (!NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("fx_off"))
		{
			new (std::nothrow) NDSManager::CManager(window, true);

			const int quality = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("fx_quality") ? NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("fx_quality") : 1;

			NDSManager::CManager::sTheObjectPtr->Quality_Set(quality);

			if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("fx_volume"))
			{
				const int volume = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("fx_volume");

				NDSManager::CManager::sTheObjectPtr->MainVolume_Set(volume);
			}
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("dm_off"))
		{
			const int musicMode = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode");

			if (musicMode == 2)
			{
				NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("music_mode", true, 1, nullptr);
			}
		}
		else
		{
			void* directMusicManagerMemory = ::operator new(0x224, std::nothrow);

			if (directMusicManagerMemory != nullptr)
			{
				FUN_00403524(directMusicManagerMemory);
			}

			char directMusicPath[MAX_PATH]{};

			GetCurrentDirectoryA(MAX_PATH - 1, directMusicPath);
			DexterString::StringAttach(directMusicPath, "\\datax\\dm2\\");

			FUN_004035b6(DAT_0050f698, 0, directMusicPath);

			if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("dm_volume"))
			{
				const int volume = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("dm_volume");

				FUN_00403932(DAT_0050f698, volume);
			}
		}

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("cda_off"))
		{
			const int musicMode = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode");

			if (musicMode == 3)
			{
				NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("music_mode", true, 1, nullptr);
			}
		}
		else
		{
			new (std::nothrow) NCDAudioManager::CCDAudioManager();

			const char initializationValue = FUN_00402761();
			NCDAudioManager::CCDAudioManager::sTheObjectPtr->Init(initializationValue);
		}

		mProgramState = 2;
	}
}