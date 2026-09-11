#include "XDiagnostics.h"
#include "XCrtFileAccess.h"

#include <Windows.h>
#include <cstdarg>
#include <cstring>

namespace NXBasics
{
	// Original function: FUN_00406995
	void __cdecl ReportWarning(const char* format, ...)
	{
		char buffer[500];

		va_list arguments;
		va_start(arguments, format);

		Runtime_FormatString(buffer, 499, format, arguments);

		va_end(arguments);

		using WarningCallback = void(__cdecl*)(const char*);

		WarningCallback callback;
		std::memcpy(&callback, &DAT_005106ec, sizeof(callback));

		buffer[499] = '\0';

		if (callback != nullptr)
		{
			callback(buffer);
			return;
		}

		OutputDebugStringA("NXBasics: WARNING:  ");
		OutputDebugStringA(buffer);
		OutputDebugStringA("\n\n\n");
	}
}