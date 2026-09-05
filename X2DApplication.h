#pragma once

#include <Windows.h>

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
	struct SApplicationControl
	{
		HINSTANCE instance;
		HINSTANCE previousInstance;
		LPSTR commandLine;
		int showCommand;

		bool fullscreen;
		std::byte padding11[3];

		std::int32_t screenWidth;
		std::int32_t screenHeight;
		std::int32_t screenDepth;

		std::int32_t minimumScreenWidth;
		std::int32_t minimumScreenHeight;
		std::int32_t minimumScreenDepth;

		std::int32_t maximumScreenWidth;
		std::int32_t maximumScreenHeight;
		std::int32_t maximumScreenDepth;

		bool fullscreenToggle;
		std::byte padding39[3];
	};

	static_assert(sizeof(SApplicationControl) == 0x3C);

	class CApplication
	{
	public:
		// Original function: FUN_004790cd
		explicit CApplication(const SApplicationControl& applicationControl);

		bool StartUp();

		// Original function: FUN_00478c02
		bool SetDisplayMode(std::int32_t width, std::int32_t height, std::uint32_t depth, bool skipDisplayModeValidation);

		// Original function: FUN_00478c98
		bool ToggleFullscreen();

		// Original function: FUN_00478d16
		int GetDisplayWidth() const;

		// Original function: FUN_00478d27
		int GetDisplayHeight() const;

		// Original function: FUN_00478d38
		int GetWindowOffsetX() const;

		// Original function: FUN_00478d4b
		int GetWindowOffsetY() const;

		// Original function: FUN_00478d5e
		int GetDisplayDepth() const;

		// Original function: FUN_00478d6f
		bool IsFullscreen() const;

		// Original function: FUN_00478efa
		bool CanDisplayModeBeActivated(bool fullscreen, std::uint32_t width, std::uint32_t height, std::uint32_t depth) const;

		HINSTANCE GetInstance() const;
		HWND GetWindowHandle() const;

		// Original function: FUN_004799d8
		bool DoMessages();

		static CApplication* sTheObjectPtr;

	private:
		std::byte mUnknown00[0x42C];
	};

	static_assert(sizeof(CApplication) == 0x42C);
}