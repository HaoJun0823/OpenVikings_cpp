#include "ApplicationStartup.h"
#include "MasterControlProgram.h"
#include "X2DApplication.h"
#include "XCrtStartup.h"

#include <cstdint>
#include <new>
#include <sal.h>
#include <Windows.h>

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

	// Original function: FUN_0047a9d9
	void ReleaseSingleInstanceGuard()
	{
		if (g_singleInstanceMutex != nullptr)
		{
			CloseHandle(g_singleInstanceMutex);
		}
	}

	// Original function: FUN_00403487; the native WinMain entry uses RET 0x10.
	int WINAPI WindowsApplicationEntry(HINSTANCE applicationInstance, HINSTANCE applicationPreviousInstance, LPSTR applicationCommandLine, int applicationShowCommand)
	{
		if (!InitializeSingleInstanceGuard("weltwunder"))
		{
			return 0;
		}

		void* allocation = NXBasics::Runtime_AllocateWithNewHandler(sizeof(NMasterControlProgram::CMasterControlProgram));
		if (allocation != nullptr) ::new(allocation) NMasterControlProgram::CMasterControlProgram(applicationInstance, applicationPreviousInstance, applicationCommandLine, applicationShowCommand);

		auto* masterControlProgram = NMasterControlProgram::CMasterControlProgram::sTheObjectPtr;

		if (*reinterpret_cast<const std::int32_t*>(masterControlProgram) == 2 && masterControlProgram->System_ProgramState_StartUpSystem())
		{
			do
			{
				if (!NXSys2DApp::CApplication::sTheObjectPtr->DoMessages())
				{
					break;
				}
			} while (!NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_Update());

			NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_ProgramState_EndCurrent();
		}

		auto* object = NMasterControlProgram::CMasterControlProgram::sTheObjectPtr;

		if (object != nullptr)
		{
			object->~CMasterControlProgram();
			NXBasics::Runtime_Free(object);
		}

		ReleaseSingleInstanceGuard();
		return 0;
	}
}

// Hosted entry for the reconstructed startup at FUN_004e709e.
int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE previousInstance, _In_ LPSTR commandLine, _In_ int showCommand)
{
	// The reconstructed startup obtains these values from the Win32 APIs.
	(void) instance;
	(void) previousInstance;
	(void) commandLine;
	(void) showCommand;

	return NXBasics::Runtime_RunApplication(WindowsApplicationEntry, NApplicationStartup::gApplicationInitializers, NApplicationStartup::gApplicationInitializers + NApplicationStartup::ApplicationInitializerCount);
}
