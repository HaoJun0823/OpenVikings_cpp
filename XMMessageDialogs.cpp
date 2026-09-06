#include "XMMessageDialogs.h"
#include "X2DApplication.h"
#include "XSystemMouseCursor.h"

#include <Windows.h>
#include <cstdarg>
#include <cstdio>
#include <intrin.h>

namespace NXSysMisc
{
	// Original function: FUN_0047a9ea
	void __cdecl XWS_WarningBoxV(const char* format, ...)
	{
		char buffer[1000];
		va_list arguments;
		va_start(arguments, format);
		// Match the legacy _vsnprintf limit and explicit final terminator.
#pragma warning(suppress: 4996)
		_vsnprintf(buffer, sizeof(buffer) - 1, format, arguments);
		va_end(arguments);
		buffer[sizeof(buffer) - 1] = '\0';
		XWS_WarningBox(buffer);
	}

	// Original function: FUN_0047aa20
	void __cdecl XWS_WarningBox(const char* message)
	{
		const bool restoreHiddenCursor = !XWS_SystemMouseCursor_IsShown();
		if (restoreHiddenCursor)
		{
			XWS_SystemMouseCursor_Show();
		}

		ClipCursor(nullptr);
		auto* application = NXSys2DApp::CApplication::sTheObjectPtr;
		application->FlipToGDISurface();
		const HWND windowHandle = application->GetWindowHandle();

		// The original builds a suffixed copy but displays the original message.
		// Omit the unused, unbounded copy while preserving the displayed text.
		if (MessageBoxA(windowHandle, message, "Warning!", MB_OKCANCEL | MB_ICONWARNING) == IDCANCEL)
		{
			__debugbreak();
		}

		if (restoreHiddenCursor)
		{
			XWS_SystemMouseCursor_Hide();
		}
	}
}
