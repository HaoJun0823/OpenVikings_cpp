#include "XSystemMouseCursor.h"

#include <Windows.h>

namespace NXSysMisc
{
	namespace
	{
		// Original global: DAT_00554d60.
		HCURSOR gCustomCursor = nullptr;

		// Original global: DAT_00504e5c.
		bool gSystemMouseCursorShown = true;
	}

	// Original function: FUN_00478b1b
	void __stdcall XWS_SystemMouseCursor_SetNormal()
	{
		const HCURSOR cursor = LoadCursorA(nullptr, MAKEINTRESOURCEA(0x7F00));
		SetCursor(cursor);

		gCustomCursor = nullptr;
	}

	// Original function: FUN_00478b37
	void __stdcall XWS_SystemMouseCursor_SetWaitMode()
	{
		const HCURSOR cursor = LoadCursorA(nullptr, MAKEINTRESOURCEA(0x7F02));
		SetCursor(cursor);

		gCustomCursor = reinterpret_cast<HCURSOR>(static_cast<INT_PTR>(-1));
	}

	// Original function: FUN_00478b53
	void __cdecl XWS_SystemMouseCursor_SetCustom(HCURSOR cursor)
	{
		if (gCustomCursor != cursor)
		{
			gCustomCursor = cursor;

			if (cursor != nullptr)
			{
				SetCursor(cursor);
			}
		}

		if (cursor == nullptr)
		{
			XWS_SystemMouseCursor_SetNormal();
		}

		if (!gSystemMouseCursorShown)
		{
			ShowCursor(TRUE);
			gSystemMouseCursorShown = true;
		}
	}

	// Original function: FUN_00478b94
	void __stdcall XWS_SystemMouseCursor_Reset()
	{
		if (!gSystemMouseCursorShown)
		{
			SetCursor(nullptr);
			return;
		}

		if (gCustomCursor == nullptr)
		{
			XWS_SystemMouseCursor_SetNormal();
			return;
		}

		if (gCustomCursor == reinterpret_cast<HCURSOR>(static_cast<INT_PTR>(-1)))
		{
			XWS_SystemMouseCursor_SetWaitMode();
			return;
		}

		XWS_SystemMouseCursor_SetCustom(gCustomCursor);
	}

	// Original function: FUN_00478bcc
	void __stdcall XWS_SystemMouseCursor_Hide()
	{
		if (!gSystemMouseCursorShown)
		{
			return;
		}

		// Keep decrementing the display counter until it is negative.
		while (ShowCursor(FALSE) >= 0)
		{
		}

		gSystemMouseCursorShown = false;
	}

	// Original function: FUN_00478be9
	void __stdcall XWS_SystemMouseCursor_Show()
	{
		if (gSystemMouseCursorShown)
		{
			return;
		}

		// The original increments the display counter only once.
		ShowCursor(TRUE);
		gSystemMouseCursorShown = true;
	}
}