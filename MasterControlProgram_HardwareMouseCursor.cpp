#include "MasterControlProgram.h"

namespace NMasterControlProgram
{
	// Original function: FUN_004016ee
	void CMasterControlProgram::HardwareMouseCursor_Enable()
	{
		mHardwareMouseCursorEnabled = true;
		FUN_00479923(DAT_00554d78, false);
		FUN_00478b1b();
		FUN_00478be9();
	}

	// Original function: FUN_0040170c
	void CMasterControlProgram::HardwareMouseCursor_Disable()
	{
		mHardwareMouseCursorEnabled = false;
		FUN_00479923(DAT_00554d78, true);
		FUN_00478b1b();
		FUN_00478bcc();
	}

	// Original function: FUN_0040172a
	void CMasterControlProgram::HardwareMouseCursor_SetMode(TMouseCursorModes mouseCursorMode)
	{
		if (!mHardwareMouseCursorEnabled)
		{
			return;
		}

		const int mouseCursorIndex = static_cast<int>(mouseCursorMode);

		if (mouseCursorIndex == -1)
		{
			FUN_00478b53(nullptr);
			return;
		}

		FUN_00478b53(mMouseCursors[mouseCursorIndex]);
	}

	// Original function: FUN_00401750
	void CMasterControlProgram::HardwareMouseCursor_ReSet()
	{
		if (mHardwareMouseCursorEnabled)
		{
			FUN_00479923(DAT_00554d78, false);
			FUN_00478b1b();
			FUN_00478be9();

			if (mHardwareMouseCursorEnabled)
			{
				FUN_00478b53(mMouseCursors[0]);
			}
		}
		else
		{
			FUN_00479923(DAT_00554d78, true);
			FUN_00478b1b();
			FUN_00478bcc();
		}
	}
}