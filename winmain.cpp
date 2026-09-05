#include "MasterControlProgram.h"
#include "X2DApplication.h"

#include <Windows.h>
#include <cstdint>
#include <cstddef>
#include <new>

namespace
{
	HANDLE g_singleInstanceMutex = nullptr;

	// Original function: FUN_0047a9a4
	bool InitializeSingleInstanceGuard(LPCSTR mutexName)
	{
		if (g_singleInstanceMutex != nullptr)
		{
			return false;
		}

		g_singleInstanceMutex = CreateMutexA(nullptr, FALSE, mutexName);

		// The original intentionally allows startup if mutex creation fails.
		if (g_singleInstanceMutex == nullptr)
		{
			return true;
		}

		return GetLastError() != ERROR_ALREADY_EXISTS;
	}
}

// Original function: FUN_00403487
int WINAPI WinMain(HINSTANCE instance, HINSTANCE previousInstance, LPSTR commandLine, int showCommand)
{
	// Original function: FUN_0047a9a4
	if (!InitializeSingleInstanceGuard("weltwunder"))
	{
		return 0;
	}

	new (std::nothrow) NMasterControlProgram::CMasterControlProgram(instance, previousInstance, commandLine, showCommand);

	auto* masterControlProgram = NMasterControlProgram::CMasterControlProgram::sTheObjectPtr;

	if (*reinterpret_cast<const std::int32_t*>(masterControlProgram) == 2 &&
		masterControlProgram->System_ProgramState_StartUpSystem())
	{
		do
		{
			if (!NXSys2DApp::CApplication::sTheObjectPtr->DoMessages())
			{
				break;
			}
		} while (!masterControlProgram->System_Update());

		masterControlProgram->System_ProgramState_EndCurrent();
	}

	int* object = DAT_0050f624;

	if (object != nullptr)
	{
		FUN_004015fb(object);
		FUN_004e5e48(object);
	}

	FUN_0047a9d9();
	return 0;
}