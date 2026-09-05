#include "X2DApplication.h"

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
	CApplication* CApplication::sTheObjectPtr = nullptr;

	// Original function: FUN_00478d16
	int CApplication::GetDisplayWidth() const
	{
		const auto* displaySystem = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);

		const auto* displayData = *reinterpret_cast<std::byte* const*>(displaySystem + 0x08);

		if (displayData == nullptr)
		{
			return 0;
		}

		return *reinterpret_cast<const std::int32_t*>(displayData + 0x04);
	}

	// Original function: FUN_00478d27
	int CApplication::GetDisplayHeight() const
	{
		const auto* displaySystem = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);
		const auto* displayData = *reinterpret_cast<std::byte* const*>(displaySystem + 0x08);

		if (displayData == nullptr)
		{
			return 0;
		}

		return *reinterpret_cast<const std::int32_t*>(displayData + 0x08);
	}

	// Original function: FUN_00478d5e
	int CApplication::GetDisplayDepth() const
	{
		const auto* displaySystem = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);
		const auto* displayData = *reinterpret_cast<std::byte* const*>(displaySystem + 0x08);

		if (displayData == nullptr)
		{
			return 0;
		}

		return *reinterpret_cast<const std::int32_t*>(displayData + 0x0C);
	}

	// Original function: FUN_00478c02
	bool CApplication::SetDisplayMode(std::int32_t width, std::int32_t height, std::uint32_t depth, bool skipDisplayModeValidation)
	{
		auto* displaySystem = *reinterpret_cast<std::byte**>(reinterpret_cast<std::byte*>(this) + 0x4C);

		auto* displayData = *reinterpret_cast<std::byte**>(displaySystem + 0x08);

		if (displayData == nullptr)
		{
			return false;
		}

		if (!skipDisplayModeValidation)
		{
			const bool fullscreen = IsFullscreen();

			if (!CanDisplayModeBeActivated(fullscreen, static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height), depth))
			{
				FUN_0047a9ea("NXSys2DApp::CApplication::SetDisplayMode\n\n display mode not available (%d*%d-%d)", width, height, depth);

				return false;
			}
		}

		if (!FUN_0047b38e(displayData, width, height, static_cast<std::int32_t>(depth)))
		{
			FUN_0047a9ea("NXSys2DApp::CApplication::SetDisplayMode\n\n could not set display mode (%d*%d-%d)", width, height, depth);

			const HWND window = *reinterpret_cast<HWND*>(displaySystem);

			PostMessageA(window, WM_CLOSE, 0, 0);

			return false;
		}

		FUN_0047953d(DAT_00554d78);

		return true;
	}

	// Original function: FUN_00478d6f
	bool CApplication::IsFullscreen() const
	{
		const auto* displaySystem = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);

		if (displaySystem == nullptr)
		{
			return false;
		}

		const auto* displayData = *reinterpret_cast<std::byte* const*>(displaySystem + 0x08);

		if (displayData == nullptr)
		{
			return false;
		}

		return *reinterpret_cast<const bool*>(displayData + 0x34);
	}

	// Original function: FUN_00478efa
	bool CApplication::CanDisplayModeBeActivated(bool fullscreen, std::uint32_t width, std::uint32_t height, std::uint32_t depth) const
	{
		const auto* applicationBytes = reinterpret_cast<const std::byte*>(this);

		if (!fullscreen)
		{
			const std::uint32_t maximumWindowWidth = *reinterpret_cast<const std::uint32_t*>(applicationBytes + 0x5C);

			if (width > maximumWindowWidth)
			{
				return false;
			}

			const std::uint32_t maximumWindowHeight = *reinterpret_cast<const std::uint32_t*>(applicationBytes + 0x60);

			if (height > maximumWindowHeight)
			{
				return false;
			}

			return true;
		}

		const std::uint32_t displayModeCount = *reinterpret_cast<const std::uint32_t*>(applicationBytes + 0x68);

		if (displayModeCount == 0)
		{
			return true;
		}

		const auto* displayMode = reinterpret_cast<const std::uint32_t*>(applicationBytes + 0x6C);

		for (std::uint32_t index = 0; index < displayModeCount; ++index)
		{
			const std::uint32_t modeWidth = displayMode[0];
			const std::uint32_t modeHeight = displayMode[1];
			const std::uint32_t modeDepth = displayMode[2];

			if (modeWidth == width && modeHeight == height && modeDepth == depth)
			{
				return true;
			}

			displayMode += 3;
		}

		return false;
	}
}