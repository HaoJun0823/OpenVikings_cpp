#include <new>

#include "MasterControlProgram.h"

namespace NMasterControlProgram
{
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
		FUN_004d2bf2(DAT_005694dc);

		const auto* desktopBytes = reinterpret_cast<const std::byte*>(DAT_00554d5c);
		const std::uint32_t displayBitmap = *reinterpret_cast<const std::uint32_t*>(desktopBytes + 0x10);

		FUN_00479052(DAT_00554d74, displayBitmap, nullptr, 0);
	}

	// Original function: FUN_0040212b
	bool CMasterControlProgram::System_ProgramState_MainMenu_StartUp()
	{
		mProgramState = 7;

		const auto* objectBytes = reinterpret_cast<const std::uint8_t*>(this);

		FUN_00402530(this, objectBytes[0x28] == 0);

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

		if (FUN_004e166b(DAT_005697ec, propertyName))
		{
			FUN_004d33c6(DAT_005694dc, 5, false);
			FUN_004e16fb(DAT_005697ec, propertyName);
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
			FUN_004e5e48(mainMenuGuiManager);
		}

		mProgramState = static_cast<std::int32_t>(TProgramState::None);
	}
}