#pragma once

#include <Windows.h>
#include <ddraw.h>

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
	using TApplicationCallback = void(__cdecl*)();

	struct SApplicationControl
	{
		// Original function: FUN_00479076
		SApplicationControl();

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

		std::byte mUnknown3C[8];
		TApplicationCallback callback;
	};

	static_assert(sizeof(SApplicationControl) == 0x48);

	struct SDisplayMode
	{
		std::uint32_t width;
		std::uint32_t height;
		std::uint32_t depth;
	};

	static_assert(sizeof(SDisplayMode) == 0x0C);

	struct SDisplayModeData
	{
		std::uint32_t desktopWidth;
		std::uint32_t desktopHeight;
		std::uint32_t desktopDepth;
		std::uint32_t fullscreenDisplayModeCount;
		SDisplayMode fullscreenDisplayModes[80];
	};

	static_assert(sizeof(SDisplayModeData) == 0x3D0);

	class CApplication : public SApplicationControl
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
		// Original function: FUN_00478d8a
		void l_GetDesktopDisplayMode();

		// Original function: FUN_00478ddf
		void l_GetFullscreenDisplayModes();

		// Original function: FUN_00478e4e
		static HRESULT CALLBACK l_EnumDisplayModesCallback(LPDDSURFACEDESC2 surfaceDescription, LPVOID context);

		bool mOwnsSystemManager;             // +0x48
		std::byte mPadding49[3];             // +0x49
		void* mDisplaySystem;                // +0x4C
		UINT mMouseWheelMessage;             // +0x50
		DWORD mDirectXVersion;               // +0x54
		DWORD mDirectXPlatform;              // +0x58
		SDisplayModeData mDisplayModeData;   // +0x5C
	};

	static_assert(sizeof(CApplication) == 0x42C);
}