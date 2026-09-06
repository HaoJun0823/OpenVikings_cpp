#pragma once

#include <Windows.h>
#include <ddraw.h>

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
	class CDDDisplay
	{
	public:
		// Original function: FUN_0047ab9a
		explicit CDDDisplay(bool fullscreen);

		// Original function: FUN_0047abbc
		~CDDDisplay();

		// Original function: FUN_0047abdc
		bool StartUp(HWND windowHandle, std::int32_t width, std::int32_t height, std::int32_t depth);

		// Original function: FUN_0047b0b6
		void ShutDown();

		// Original function: FUN_0047b38e
		bool SetDisplayMode(std::int32_t width, std::int32_t height, std::int32_t depth);

		// Original function: FUN_0047b3d6
		bool ToggleFullscreen();

		// Original function: FUN_0047b44c
		void Update();

		std::int32_t GetWidth() const
		{
			return mWidth;
		}

		std::int32_t GetHeight() const
		{
			return mHeight;
		}

		std::int32_t GetDepth() const
		{
			return mDepth;
		}

		std::int32_t GetWindowOffsetX() const
		{
			return mFullscreen ? 0 : mWindowOffsetX;
		}

		std::int32_t GetWindowOffsetY() const
		{
			return mFullscreen ? 0 : mWindowOffsetY;
		}

		bool IsFullscreen() const
		{
			return mFullscreen;
		}

		HPALETTE GetWindowsPalette() const
		{
			return mWindowsPalette;
		}

		void SetWindowOffsets(std::int32_t x, std::int32_t y)
		{
			mWindowOffsetX = x;
			mWindowOffsetY = y;
		}

	private:
		// Original function: FUN_0047ac2d
		bool l_StartUp();

		// Original function: FUN_0047b057
		bool l_DoesInitializationHaveError(HRESULT result);

		// Original function: FUN_0047b198
		IDirectDrawSurface7* l_CreateSurface(std::int32_t width, std::int32_t height, DWORD surfaceCaps, bool specifyPixelFormat, bool useCurrentFullscreenState);

		// Original function: FUN_0047b29d
		HRESULT l_RestoreSurfaces();

		// Original function: FUN_0047b2b8
		std::uint32_t l_GetSurfaceDepth(IDirectDrawSurface7* surface);

		// Original function: FUN_0047b8db
		static std::uint32_t l_GetColorMask(IDirectDrawSurface7* surface, COLORREF color);

		// Original function: FUN_0047c21f
		void l_CreatePalettes();

		// Original function: FUN_0047c23e
		void l_DestroyPalettes();

		// Original function: FUN_0047c266
		bool l_SetPaletteEntries(const PALETTEENTRY* paletteEntries);

		// Original function: FUN_0047c588
		void l_Configure16BitColorMasks();

		// Original function: FUN_0047c5f1
		void l_Configure32BitColorMasks();

		HWND mWindowHandle;                         // +0x00
		std::int32_t mWidth;                       // +0x04
		std::int32_t mHeight;                      // +0x08
		std::int32_t mDepth;                       // +0x0C
		bool mInitialized;                         // +0x10
		std::byte mPadding11[3];                   // +0x11
		HMODULE mDirectDrawModule;                 // +0x14
		IDirectDraw7* mDirectDraw;                 // +0x18
		IDirectDrawSurface7* mPrimarySurface;      // +0x1C
		IDirectDrawSurface7* mBackSurface;         // +0x20
		IDirectDrawClipper* mClipper;              // +0x24
		IDirectDrawPalette* mDirectDrawPalette;    // +0x28
		HPALETTE mWindowsPalette;                  // +0x2C
		LOGPALETTE* mPaletteData;                  // +0x30
		bool mFullscreen;                          // +0x34
		std::byte mPadding35[3];                   // +0x35
		std::int32_t mWindowOffsetX;               // +0x38
		std::int32_t mWindowOffsetY;               // +0x3C
		std::int32_t mSavedWindowOffsetX;          // +0x40
		std::int32_t mSavedWindowOffsetY;          // +0x44
		std::uint32_t mTransparentColor;           // +0x48
		bool mFullscreenFlipState;                 // +0x4C
		std::byte mPadding4D[3];                   // +0x4D
		float mGamma;                              // +0x50
	};

	static_assert(sizeof(CDDDisplay) == 0x54);
}