#pragma once

#define DIRECTINPUT_VERSION 0x0700
#include <dinput.h>

#include <cstddef>

namespace NXSysMouseManager
{
	class CDirectInputMouse
	{
	public:
		// Original function: FUN_0047c320
		CDirectInputMouse();

		// Original function: FUN_0047c341
		~CDirectInputMouse();

		// Original function: FUN_0047c47e
		bool GetMovement(int& deltaX, int& deltaY);

		bool IsInitialised() const;

		// Original function: FUN_0047c34e
		void Restart();

		inline static CDirectInputMouse* sTheObjectPtr = nullptr;

	private:
		// Original function: FUN_0047c35f
		void StartUp();

		// Original function: FUN_0047c432
		void ShutDown();

		// Original function: FUN_0047c4f3
		void HandleError(const char* operation, HRESULT result);

		IDirectInputA* mDirectInput;        // +0x00
		IDirectInputDeviceA* mMouseDevice; // +0x04
		bool mInitialised;                 // +0x08
		bool mAcquired;                    // +0x09
		std::byte mPadding0A[2];           // +0x0A
		char* mLastError;                  // +0x0C
	};

	static_assert(sizeof(CDirectInputMouse) == 16);
}
