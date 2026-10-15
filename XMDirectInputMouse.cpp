#include "X2DApplication.h"
#include "XMDirectInputMouse.h"

#include <dinput.h>
#include <Windows.h>

#include <cstring>
#include <new>
#include <string>

namespace NXSysMouseManager
{
	bool CDirectInputMouse::IsInitialised() const
	{
		return mInitialised;
	}

	// Original function: FUN_0047c320
	CDirectInputMouse::CDirectInputMouse()
	{
		sTheObjectPtr = this;
		std::memset(this, 0, sizeof(*this));
		StartUp();
	}

	// Original function: FUN_0047c341
	CDirectInputMouse::~CDirectInputMouse()
	{
		ShutDown();
		sTheObjectPtr = nullptr;
	}

	// Original function: FUN_0047c34e
	void CDirectInputMouse::Restart()
	{
		ShutDown();
		StartUp();
	}

	// Original function: FUN_0047c35f
	void CDirectInputMouse::StartUp()
	{
		NXSys2DApp::CApplication* application = NXSys2DApp::CApplication::sTheObjectPtr;
		HRESULT result = DirectInputCreateA(application->GetInstance(), DIRECTINPUT_VERSION, &mDirectInput, nullptr);

		if (result != DI_OK)
		{
			HandleError("DirectInputCreate", result);
			return;
		}

		result = mDirectInput->CreateDevice(GUID_SysMouse, &mMouseDevice, nullptr);

		if (result != DI_OK)
		{
			HandleError("CreateDevice", result);
			return;
		}

		result = mMouseDevice->SetDataFormat(&c_dfDIMouse);

		if (result != DI_OK)
		{
			HandleError("SetDataFormat", result);
			return;
		}

		result = mMouseDevice->SetCooperativeLevel(application->GetWindowHandle(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);

		if (result != DI_OK)
		{
			HandleError("SetCooperativeLevel", result);
			return;
		}

		result = mMouseDevice->Acquire();

		if (result == DI_OK)
		{
			mAcquired = true;
			delete[] mLastError;
			mLastError = nullptr;
		}
		else
		{
			HandleError("Acquire", result);
		}

		mInitialised = true;
	}

	// Original function: FUN_0047c432
	void CDirectInputMouse::ShutDown()
	{
		mInitialised = false;

		if (mMouseDevice != nullptr)
		{
			if (mAcquired)
			{
				mMouseDevice->Unacquire();
				mAcquired = false;
			}

			mMouseDevice->Release();
			mMouseDevice = nullptr;
		}

		if (mDirectInput != nullptr)
		{
			mDirectInput->Release();
			mDirectInput = nullptr;
		}

		delete[] mLastError;
		mLastError = nullptr;
	}

	// Original function: FUN_0047c47e
	bool CDirectInputMouse::GetMovement(int& deltaX, int& deltaY)
	{
		deltaX = 0;
		deltaY = 0;

		if (!mAcquired)
		{
			ShutDown();
			StartUp();

			if (!mAcquired)
			{
				return false;
			}
		}

		DIMOUSESTATE mouseState{ };
		const HRESULT result = mMouseDevice->GetDeviceState(sizeof(mouseState), &mouseState);

		if (result != DI_OK)
		{
			HandleError("GetDeviceState", result);
			ShutDown();
			StartUp();
			return false;
		}

		deltaX = mouseState.lX;
		deltaY = mouseState.lY;
		return true;
	}

	// Original function: FUN_0047c4f3
	void CDirectInputMouse::HandleError(const char* operation, HRESULT result)
	{
		const char* errorName = "UNKNOWN";

		if (result == DIERR_OTHERAPPHASPRIO)
		{
			errorName = "DIERR_OTHERAPPHASPRIO";
		}
		else if (result == DIERR_NOTINITIALIZED)
		{
			errorName = "DIERR_NOTINITIALIZED";
		}
		else if (result == DIERR_INVALIDPARAM)
		{
			errorName = "DIERR_INVALIDPARAM";
		}

		const std::string errorMessage = std::string(operation) + ":" + errorName;

		delete[] mLastError;
		mLastError = new (std::nothrow) char[errorMessage.size() + 1];

		if (mLastError != nullptr)
		{
			std::memcpy(mLastError, errorMessage.c_str(), errorMessage.size() + 1);
		}
	}
}
