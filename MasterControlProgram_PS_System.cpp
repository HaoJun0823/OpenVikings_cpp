#include "MasterControlProgram.h"
#include "X2DApplication.h"

namespace NMasterControlProgram
{
	// Original function: FUN_0040234f
	bool CMasterControlProgram::System_ProgramState_StartUpSystem()
	{
		return System_ProgramState_Start(TProgramState::TitleScreen);
	}


	// Original function: FUN_0040235c
	bool CMasterControlProgram::System_ProgramState_Start(TProgramState programState)
	{
		if (mProgramState != static_cast<std::int32_t>(TProgramState::None))
		{
			return false;
		}

		switch (programState)
		{
		case TProgramState::TitleScreen:
			return System_ProgramState_TitleScreen_StartUp();

		case TProgramState::MainMenu:
			return System_ProgramState_MainMenu_StartUp();

		case TProgramState::Game:
			return System_ProgramState_Game_StartUp();

		case TProgramState::PreIngameFmv:
			return System_ProgramState_PreIngameFmv_StartUp();

		default:
			return false;
		}
	}


	// Original function: FUN_0040241e
	bool CMasterControlProgram::System_Update()
	{
		switch (mProgramState)
		{
		case 5:
			System_ProgramState_TitleScreen_Update();
			break;

		case 8:
			System_ProgramState_MainMenu_Update();
			break;

		case 11:
		case 12:
			System_ProgramState_Game_Update();
			break;

		case 15:
			System_ProgramState_PreIngameFmv_Update();
			break;
		}

		if (DAT_0050f6a4 != nullptr)
		{
			FUN_00404797(DAT_0050f6a4);
		}

		if (DAT_0050f698 != nullptr)
		{
			FUN_00403858(DAT_0050f698);

			auto** errorMessage = reinterpret_cast<char**>(reinterpret_cast<std::byte*>(DAT_0050f698) + 0x220);

			if (*errorMessage != nullptr)
			{
				if (**errorMessage != '\0')
				{
					char buffer[300];

					DexterString::StringPrint(buffer, "DMManager reports error: %s\r\n", *errorMessage);

					OutputDebugStringA(buffer);
				}

				FUN_004e5e48(*errorMessage);
				*errorMessage = nullptr;
			}
		}

		if (DAT_0050f69c != nullptr)
		{
			FUN_004040eb(DAT_0050f69c);
		}

		NXSys2DApp::CApplication::sTheObjectPtr->UpdateDisplay();

		DisplayResolution_CheckChangeRequest();
		System_ProgramState_ExecuteWantedChange();

		if (!NXSys2DApp::CApplication::sTheObjectPtr->IsWindowActive() && !NXSys2DApp::CApplication::sTheObjectPtr->IsApplicationActive())
		{
			Sleep(10);
		}

		return reinterpret_cast<const std::uint8_t*>(this)[9] != 0;
	}

	// Original function: FUN_004023de
	void CMasterControlProgram::System_ProgramState_ChangeNextTimePossible(TProgramState programState, bool forceChange)
	{
		mWantedProgramState = static_cast<std::int32_t>(programState);
		mForceProgramStateChange = forceChange;
	}

	// Original function: FUN_004023ef
	void CMasterControlProgram::System_ProgramState_ExecuteWantedChange()
	{
		if (mWantedProgramState == 0)
		{
			return;
		}

		if (mWantedProgramState != mProgramState || mForceProgramStateChange)
		{
			System_ProgramState_EndCurrent();
			System_ProgramState_Start(static_cast<TProgramState>(mWantedProgramState));
		}

		mWantedProgramState = 0;
		mForceProgramStateChange = false;
	}

	// Original function: FUN_004023ae
	void CMasterControlProgram::System_ProgramState_EndCurrent()
	{
		switch (mProgramState)
		{
		case 5:
			System_ProgramState_TitleScreen_ShutDown();
			break;

		case 8:
			System_ProgramState_MainMenu_ShutDown();
			break;

		case 11:
		case 12:
			System_ProgramState_Game_ShutDown();
			break;

		case 15:
			System_ProgramState_PreIngameFmv_ShutDown();
			break;
		}
	}
}