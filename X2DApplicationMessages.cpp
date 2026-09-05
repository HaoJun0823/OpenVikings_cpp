#include "X2DApplication.h"
#include "XMMouseManager.h"

#include <Windows.h>

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
	// Original function: FUN_004799d8
	bool CApplication::DoMessages()
	{
		MSG message{};

		for (std::uint32_t processedMessageCount = 0; processedMessageCount < 51; ++processedMessageCount)
		{
			if (PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE) == FALSE)
			{
				break;
			}

			if (GetMessageA(&message, nullptr, 0, 0) == 0)
			{
				return false;
			}

			TranslateMessage(&message);
			DispatchMessageA(&message);
		}

		const auto* objectBytes = reinterpret_cast<const std::byte*>(this);
		const auto* messageControl = *reinterpret_cast<const std::uint8_t* const*>(objectBytes + 0x4C);

		if (messageControl[5] != 0)
		{
			NXSysMouseManager::CMouseManager::sTheObjectPtr->UpdateMouseState();
		}

		return true;
	}
}