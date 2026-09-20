#include "XSystemManager.h"
#include "XCrtFileAccess.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "NetworkShutdown.h"
#include "X2DApplication.h"
#include "XBasicTypes.h"
#include "XBitmap.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "XSystemMouseCursor.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>

namespace NC2LogicExternals
{
	// Original function: FUN_00401c12
	void __stdcall External_Misc_ProgressBar_Update()
	{
		NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->ProgressBar_Update();
	}
}

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
		NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_WorkOnCommandLine(commandLine);
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

			void* fileLibrary = NXBasics::Runtime_AllocateWithNewHandler(0x14u, 1);

			if (fileLibrary != nullptr)
			{
				NXBasics::Archive_Initialize(fileLibrary, configuredFile);
			}

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

				void* fileLibrary = NXBasics::Runtime_AllocateWithNewHandler(0x14u, 1);

				if (fileLibrary != nullptr)
				{
					NXBasics::Archive_Initialize(fileLibrary, fileName);
				}

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

	// Original function: FUN_00401b85
	void CMasterControlProgram::GameEngine_ShutDownEngine()
	{
		if (mEngineInitialized == 0)
		{
			return;
		}

		if (mGameInitialized != 0)
		{
			FUN_004071c3();
			mGameInitialized = 0;
		}

		auto* engineManager = DAT_00554da4;

		if (engineManager != nullptr)
		{
			FUN_0047e181(engineManager);
			NXBasics::Runtime_Free(engineManager);
		}

		auto* game = DAT_005109d0;

		if (game != nullptr)
		{
			FUN_00406d85(game);
			NXBasics::Runtime_Free(game);
		}

		mEngineInitialized = 0;
	}

	// Original function: FUN_00402530
	void CMasterControlProgram::ProgressBar_Init(bool useTitleScreen)
	{
		ProgressBar_Exit();

		const char* fileNamePattern = useTitleScreen ? "data\\gui\\lang\\%s\\pictures\\titlescreen.bmp" : "data\\gui\\lang\\%s\\pictures\\loadscreen.bmp";

		char fileName[0x104];
		FUN_00405adb(fileNamePattern, fileName, false);

		// Temporary x86 object storage until the corresponding classes are implemented.
		alignas(4) std::byte pictureStorage[0x0C];
		alignas(4) std::byte bitmapStorage[0x54];

		FUN_0040bd76(pictureStorage, fileName);

		void* sourceBitmap;
		std::memcpy(&sourceBitmap, pictureStorage, sizeof(sourceBitmap));

		const auto sourceDisplayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();
		const auto sourceBitmapFormat = NXBasics::ConvertBitsToTDepth(sourceDisplayDepth);

		std::uint32_t sourceHeight;
		std::uint32_t sourceWidth;

		std::memcpy(&sourceHeight, static_cast<const std::byte*>(sourceBitmap) + 0x14, sizeof(sourceHeight));
		std::memcpy(&sourceWidth, static_cast<const std::byte*>(sourceBitmap) + 0x10, sizeof(sourceWidth));

		auto* temporaryBitmap = ::new (static_cast<void*>(bitmapStorage)) NXBasics::CBitmap(sourceWidth, sourceHeight, sourceBitmapFormat);

		// Reload the picture's bitmap pointer at the original call site.
		std::memcpy(&sourceBitmap, pictureStorage, sizeof(sourceBitmap));
		static_cast<NXBasics::CBitmap*>(sourceBitmap)->CopyIntoBitmap(temporaryBitmap, 0, 0);

		void* progressBarMemory = ::operator new(0x54, std::nothrow);
		void* progressBar = nullptr;

		if (progressBarMemory != nullptr)
		{
			auto* application = NXSys2DApp::CApplication::sTheObjectPtr;

			const auto displayDepth = application->GetDisplayDepth();
			const auto bitmapFormat = NXBasics::ConvertBitsToTDepth(displayDepth);
			const auto displayHeight = application->GetDisplayHeight();
			const auto displayWidth = application->GetDisplayWidth();

			progressBar = ::new (progressBarMemory) NXBasics::CBitmap(displayWidth, displayHeight, bitmapFormat);
		}

		mProgressBar = progressBar;

		temporaryBitmap->CopyIntoBitmap_FitIn(static_cast<NXBasics::CBitmap*>(progressBar));
		temporaryBitmap->~CBitmap();
		FUN_0040be7d(pictureStorage);

		ProgressBar_PrintToScreen();

		mProgressBarValue = 0;
	}

	// Original function: FUN_0040263d
	void CMasterControlProgram::ProgressBar_Update()
	{
		void* progressBar = mProgressBar;

		if (progressBar == nullptr)
		{
			return;
		}

		++mProgressBarValue;

		std::uint32_t heightBits;
		std::uint32_t widthBits;

		std::memcpy(&heightBits, static_cast<const std::byte*>(progressBar) + 0x14, sizeof(heightBits));
		std::memcpy(&widthBits, static_cast<const std::byte*>(progressBar) + 0x10, sizeof(widthBits));

		// Preserve 32-bit subtraction before interpreting the signed coordinates.
		widthBits -= 84u;
		heightBits -= 6u;

		std::int32_t widthDifference;
		std::int32_t top;

		std::memcpy(&widthDifference, &widthBits, sizeof(widthDifference));
		std::memcpy(&top, &heightBits, sizeof(top));

		const std::int32_t left = widthDifference / 2;

		NXBasics::SRectangle rectangle(left, top, 84, 4);

		// Preserve the original explicit destruction point.
		alignas(NXBasics::CBitmap) std::byte bitmapStorage[sizeof(NXBasics::CBitmap)];
		auto* temporaryBitmap = ::new (static_cast<void*>(bitmapStorage)) NXBasics::CBitmap(static_cast<NXBasics::CBitmap*>(mProgressBar), rectangle);

		// Original static storage: DAT_0050f628 and DAT_0050f62c.
		static std::uint8_t backgroundInitializationFlags = 0;
		static NXBasics::SColorRGB backgroundColor(0, 0, 0);

		if ((backgroundInitializationFlags & 1u) == 0)
		{
			backgroundInitializationFlags |= 1u;

			backgroundColor = NXBasics::SColorRGB(108, 100, 83);

			// Original callback at 0x0040272B contains only RET.
			// Adapt to the reconstructed onexit signature; the result is ignored.
			NXBasics::Runtime_AtExit(+[] () -> int
			{
				return 0;
			});
		}

		temporaryBitmap->Fill(backgroundColor);

		const std::uint32_t doubledValue = mProgressBarValue << 1;
		const std::int32_t position = static_cast<std::int32_t>(doubledValue % 84u);

		for (std::int32_t offset = -7; offset <= 7; ++offset)
		{
			const std::int32_t distance = offset < 0 ? -offset : offset;
			const std::uint8_t intensity = static_cast<std::uint8_t>(255 - distance * 14);

			NXBasics::SColorRGB lineColor(intensity, intensity, intensity);
			temporaryBitmap->Draw_VerticalLine(position + offset, 0, 4, lineColor);
		}

		temporaryBitmap->~CBitmap();
		ProgressBar_PrintToScreen();
	}

	// Original function: FUN_0040272c
	void CMasterControlProgram::ProgressBar_PrintToScreen()
	{
		for (int iteration = 0; iteration < 4; ++iteration)
		{
			FUN_00479052(NXSys2DApp::CApplication::sTheObjectPtr, mProgressBar, nullptr, 0);
			NXSys2DApp::CApplication::sTheObjectPtr->UpdateDisplay();
			Sleep(10);
		}
	}

	// Original function: FUN_00402620
	void CMasterControlProgram::ProgressBar_Exit()
	{
		void* progressBar = mProgressBar;

		if (progressBar == nullptr)
		{
			return;
		}

		delete static_cast<NXBasics::CBitmap*>(progressBar);
		mProgressBar = nullptr;
	}

	// Original function: FUN_004015fb
	CMasterControlProgram::~CMasterControlProgram()
	{
		System_ProgramState_EndCurrent();
		NNetwork::Network_ShutDown();
		GameEngine_ShutDownEngine();
		ProgressBar_Exit();

		mProgramState = 3;

		NXSys2DApp::CApplication::sTheObjectPtr->ShutDown();

		auto* cdAudioManager = DAT_0050f69c;

		if (cdAudioManager != nullptr)
		{
			FUN_00403e62(cdAudioManager);
			NXBasics::Runtime_Free(cdAudioManager);
		}

		void* directMusicManager = DAT_0050f698;

		if (directMusicManager != nullptr)
		{
			// Invoke vtable slot 0 with the original deletion flag.
			using DeletingDestructor = void* (__thiscall*)(void*, unsigned int);

			auto* vtable = *reinterpret_cast<DeletingDestructor**>(directMusicManager);
			vtable[0](directMusicManager, 1);
		}

		auto* soundManager = DAT_0050f6a4;

		if (soundManager != nullptr)
		{
			FUN_00404762(soundManager);
			NXBasics::Runtime_Free(soundManager);
		}

		auto* application = NXSys2DApp::CApplication::sTheObjectPtr;

		if (application != nullptr)
		{
			delete application;
		}

		auto* propertyManager = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;

		if (propertyManager != nullptr)
		{
			delete propertyManager;
		}

		for (std::size_t index = 0; index < 5; ++index)
		{
			void* fileLibrary = NXBasics::CFile::mStaticVars.fileLibraries[index];

			if (fileLibrary != nullptr)
			{
				NXBasics::CFile::FileSystem_FileLibrary_Remove(fileLibrary);
				NXBasics::Archive_Destroy(fileLibrary);
				NXBasics::Runtime_Free(fileLibrary);
			}
		}

		auto* systemManager = NXBasics::CXBSystemManager::sTheObjectPtr;

		if (systemManager != nullptr)
		{
			FUN_00406ab5(systemManager);
			::operator delete(systemManager);
		}

		mProgramState = 0;
		sTheObjectPtr = nullptr;
	}
}