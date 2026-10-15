#include "X2DDisplay.h"
#include "XNumericConstants.h"

#include "X2DApplication.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XMMessageDialogs.h"
#include "XMMouseManager.h"
#include "XPixelFormatConverter.h"

#include <ddraw.h>
#include <Windows.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

#pragma comment(lib, "dxguid.lib")

namespace
{
	// Original function: FUN_0047c309; four trailing allocation bytes remain untouched.
	LOGPALETTE* DDDisplay_AllocatePaletteData()
	{
		auto* palette = static_cast<LOGPALETTE*>(NXBasics::Runtime_Allocate(1032));
		palette->palVersion = 0x0300;
		palette->palNumEntries = 0x0100;
		return palette;
	}

	using DirectDrawCreateExFunction = HRESULT(WINAPI*)(GUID* driverGuid, void** directDraw, REFIID interfaceId, IUnknown* outerUnknown);

	/*
	 * Original globals:
	 * DAT_00505a28 = 1
	 * DAT_00505a29 = 1
	 */
	bool gWaitForFullscreenFlip = true;
	bool gSkipPaletteUpdateBeforePresent = true;

	constexpr DWORD GetFullscreenFlipFlags()
	{
		return DDFLIP_WAIT;
	}
}

namespace NXSys2DApp
{
	// Original function: FUN_0047ab9a
	CDDDisplay::CDDDisplay(bool fullscreen) :
		mWindowHandle(nullptr),
		mWidth(0),
		mHeight(0),
		mDepth(0),
		mInitialized(false),
		mPadding11{ },
		mDirectDrawModule(nullptr),
		mDirectDraw(nullptr),
		mPrimarySurface(nullptr),
		mBackSurface(nullptr),
		mClipper(nullptr),
		mDirectDrawPalette(nullptr),
		mWindowsPalette(nullptr),
		mPaletteData(nullptr),
		mFullscreen(fullscreen),
		mPadding35{ },
		mWindowOffsetX(0),
		mWindowOffsetY(0),
		mSavedWindowOffsetX(0),
		mSavedWindowOffsetY(0),
		mTransparentColor(0),
		mFullscreenFlipState(false),
		mPadding4D{ },

		mGamma(1.0F)
	{}

	// Original function: FUN_0047abbc
	CDDDisplay::~CDDDisplay()
	{
		ShutDown();

		NXBasics::Runtime_Free(mPaletteData);
		mPaletteData = nullptr;

		mInitialized = false;
	}

	// Original function: FUN_0047abdc
	bool CDDDisplay::StartUp(HWND windowHandle, std::int32_t width, std::int32_t height, std::int32_t depth)
	{
		mWindowHandle = windowHandle;
		mWidth = width;
		mHeight = height;
		mDepth = depth;

		mPaletteData = DDDisplay_AllocatePaletteData();

		for (std::uint32_t index = 0; index < 256; ++index)
		{
			PALETTEENTRY& entry = mPaletteData->palPalEntry[index];

			entry.peBlue = 0;
			entry.peGreen = 0;
			entry.peRed = 0;
			entry.peFlags = 0;
		}

		mInitialized = l_StartUp();
		return mInitialized;
	}

	// Original function: FUN_0047ac2d
	bool CDDDisplay::l_StartUp()
	{
		mDirectDrawModule = LoadLibraryA("DDRAW.DLL");

		if (mDirectDrawModule == nullptr)
		{
			return false;
		}

		const auto directDrawCreateEx = reinterpret_cast<DirectDrawCreateExFunction>(GetProcAddress(mDirectDrawModule, "DirectDrawCreateEx"));

		if (directDrawCreateEx == nullptr)
		{
			FreeLibrary(mDirectDrawModule);
			mDirectDrawModule = nullptr;
			return false;
		}

		HRESULT result = directDrawCreateEx(nullptr, reinterpret_cast<void**>(&mDirectDraw), IID_IDirectDraw7, nullptr);

		if (result != DD_OK || mDirectDraw == nullptr)
		{
			l_DoesInitializationHaveError(result);

			if (mDirectDrawModule != nullptr)
			{
				FreeLibrary(mDirectDrawModule);
				mDirectDrawModule = nullptr;
			}

			return false;
		}

		if (mFullscreen)
		{
			SetWindowLongA(mWindowHandle, GWL_STYLE, WS_POPUP | WS_VISIBLE);
			SetWindowPos(mWindowHandle, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);

			result = mDirectDraw->SetCooperativeLevel(mWindowHandle, DDSCL_FULLSCREEN | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			result = mDirectDraw->SetDisplayMode(static_cast<DWORD>(mWidth), static_cast<DWORD>(mHeight), static_cast<DWORD>(mDepth), 0, 0);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			mFullscreenFlipState = true;

			ShowWindow(mWindowHandle, SW_SHOW);

			DDSURFACEDESC2 surfaceDescription{ };
			surfaceDescription.dwSize = sizeof(surfaceDescription);
			surfaceDescription.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			surfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
			surfaceDescription.dwBackBufferCount = 1;

			result = mDirectDraw->CreateSurface(&surfaceDescription, &mPrimarySurface, nullptr);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			DDSCAPS2 backBufferCaps{ };
			backBufferCaps.dwCaps = DDSCAPS_BACKBUFFER;

			result = mPrimarySurface->GetAttachedSurface(&backBufferCaps, &mBackSurface);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}
		}
		else
		{
			RECT windowRectangle{ 0, 0,	mWidth,	mHeight };
			LONG windowStyle = GetWindowLongA(mWindowHandle, GWL_STYLE);

			/*
			 * Exact mask and flags used by the original binary.
			 * This removes popup/resizing flags and enables a caption
			 * plus system menu.
			 */
			windowStyle &= NXBasics::BitMask::Bits0Through18AndBits20Through21AndBits24Through30;
			windowStyle |= NXBasics::BitMask::Bit19AndBits22Through23;

			SetWindowLongA(mWindowHandle, GWL_STYLE, windowStyle);

			const DWORD extendedStyle = static_cast<DWORD>(GetWindowLongA(mWindowHandle, GWL_EXSTYLE));
			const BOOL hasMenu = GetMenu(mWindowHandle) != nullptr;

			AdjustWindowRectEx(&windowRectangle, static_cast<DWORD>(windowStyle), hasMenu, extendedStyle);
			SetWindowPos(mWindowHandle, nullptr, 0, 0, windowRectangle.right - windowRectangle.left, windowRectangle.bottom - windowRectangle.top, SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
			SetWindowPos(mWindowHandle, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

			result = mDirectDraw->SetCooperativeLevel(mWindowHandle, DDSCL_NORMAL);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			DDSURFACEDESC2 primaryDescription{ };
			primaryDescription.dwSize = sizeof(primaryDescription);
			primaryDescription.dwFlags = DDSD_CAPS;
			primaryDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			result = mDirectDraw->CreateSurface(&primaryDescription, &mPrimarySurface, nullptr);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			mBackSurface = l_CreateSurface(mWidth, mHeight, DDSCAPS_OFFSCREENPLAIN, true, mFullscreen);

			if (mBackSurface == nullptr)
			{
				l_DoesInitializationHaveError(DDERR_GENERIC);

				return false;
			}
		}

		result = mDirectDraw->CreateClipper(0, &mClipper, nullptr);

		if (l_DoesInitializationHaveError(result))
		{
			return false;
		}

		result = mClipper->SetHWnd(
			0,
			mWindowHandle);

		if (l_DoesInitializationHaveError(result))
		{
			return false;
		}

		result = mPrimarySurface->SetClipper(mClipper);

		if (l_DoesInitializationHaveError(result))
		{
			return false;
		}

		l_CreatePalettes();

		if (mWindowsPalette != nullptr && !mFullscreen)
		{
			const HDC deviceContext = GetDC(mWindowHandle);

			if (deviceContext != nullptr)
			{
				const HPALETTE previousPalette = SelectPalette(deviceContext, mWindowsPalette, FALSE);
				RealizePalette(deviceContext);
				SelectPalette(deviceContext, previousPalette, FALSE);
				ReleaseDC(mWindowHandle, deviceContext);
			}
		}

		mTransparentColor = l_GetColorMask(mBackSurface, RGB(0, 0, 0));

		DDBLTFX blitEffects{ };
		blitEffects.dwSize = sizeof(blitEffects);
		blitEffects.dwFillColor = mTransparentColor;

		mBackSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &blitEffects);
		mPrimarySurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &blitEffects);

		RECT windowRectangle{ };

		if (GetWindowRect(mWindowHandle, &windowRectangle))
		{
			POINT windowPosition{ windowRectangle.left,	windowRectangle.top };

			ScreenToClient(mWindowHandle, &windowPosition);
			mWindowOffsetX = windowRectangle.left - windowPosition.x;
			mWindowOffsetY = windowRectangle.top - windowPosition.y;
		}

		if (mDepth == 16)
		{
			l_Configure16BitColorMasks();
		}

		if (mDepth == 32)
		{
			l_Configure32BitColorMasks();
		}

		return true;
	}

	// Original function: FUN_0047b057
	bool CDDDisplay::l_DoesInitializationHaveError(HRESULT result)
	{
		if (result == DD_OK)
		{
			return false;
		}

		char errorMessage[200]{ };

		wsprintfA(errorMessage, "NXSys2DApp::CDDDisplay::l_DoesInitializationHaveError\n DirectDraw error: 0x%03X\r\n", static_cast<unsigned int>(result) & NXBasics::BitMask::Low12Bits);
		OutputDebugStringA(errorMessage);

		ShutDown();
		return true;
	}

	// Original function: FUN_0047b0b6
	void CDDDisplay::ShutDown()
	{
		if (mBackSurface != nullptr)
		{
			DDSURFACEDESC2 surfaceDescription{ };
			surfaceDescription.dwSize = sizeof(surfaceDescription);

			HRESULT result{ };

			do
			{
				result = mBackSurface->Lock(nullptr, &surfaceDescription, DDLOCK_WAIT, nullptr);
			} while (result == DDERR_WASSTILLDRAWING);

			if (result == DD_OK)
			{
				mBackSurface->Unlock(nullptr);
			}
		}

		l_DestroyPalettes();

		if (mDirectDraw == nullptr)
		{
			mInitialized = false;
			return;
		}

		if (mFullscreen)
		{
			if (!mFullscreenFlipState)
			{
				Update();
			}

			mDirectDraw->RestoreDisplayMode();
			mDirectDraw->SetCooperativeLevel(mWindowHandle, DDSCL_NORMAL);
		}

		if (mClipper != nullptr)
		{
			mClipper->Release();
			mClipper = nullptr;
		}

		/*
		 * The attached fullscreen backbuffer is owned through the
		 * primary complex surface in the original implementation.
		 */
		if (mBackSurface != nullptr && !mFullscreen)
		{
			mBackSurface->Release();
		}

		mBackSurface = nullptr;

		if (mPrimarySurface != nullptr)
		{
			mPrimarySurface->Release();
			mPrimarySurface = nullptr;
		}

		mDirectDraw->Release();
		mDirectDraw = nullptr;

		if (mDirectDrawModule != nullptr)
		{
			FreeLibrary(mDirectDrawModule);
			mDirectDrawModule = nullptr;
		}

		mInitialized = false;
	}

	// Original function: FUN_0047b198
	IDirectDrawSurface7* CDDDisplay::l_CreateSurface(std::int32_t width, std::int32_t height, DWORD surfaceCaps, bool specifyPixelFormat, bool useCurrentFullscreenState)
	{
		DDSURFACEDESC2 surfaceDescription{ };
		surfaceDescription.dwSize = sizeof(surfaceDescription);
		surfaceDescription.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		surfaceDescription.dwWidth = static_cast<DWORD>(width);
		surfaceDescription.dwHeight = static_cast<DWORD>(height);
		surfaceDescription.ddsCaps.dwCaps = surfaceCaps;

		if (specifyPixelFormat && useCurrentFullscreenState)
		{
			surfaceDescription.dwFlags |= DDSD_PIXELFORMAT;
			surfaceDescription.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
			surfaceDescription.ddpfPixelFormat.dwFlags = DDPF_RGB | DDPF_PALETTEINDEXED8;
			surfaceDescription.ddpfPixelFormat.dwRGBBitCount = 8;
		}

		IDirectDrawSurface7* surface = nullptr;

		HRESULT result = mDirectDraw->CreateSurface(&surfaceDescription, &surface, nullptr);

		if (result != DD_OK || surface == nullptr)
		{
			OutputDebugStringA("CDDDisplay::l_CreateSurface: CreateSurface failed.\r\n");

			return nullptr;
		}

		result = surface->Restore();

		if (result != DD_OK)
		{
			OutputDebugStringA("CDDDisplay::l_CreateSurface: Restore failed.\r\n");

			surface->Release();
			return nullptr;
		}

		DDCOLORKEY colorKey{ };

		if (mPrimarySurface != nullptr && mPrimarySurface->GetColorKey(DDCKEY_SRCBLT, &colorKey) == DD_OK)
		{
			surface->SetColorKey(DDCKEY_SRCBLT, &colorKey);
		}

		return surface;
	}

	// Original function: FUN_0047b29d
	HRESULT CDDDisplay::l_RestoreSurfaces()
	{
		const HRESULT result = mPrimarySurface->Restore();
		return result == DD_OK ? mBackSurface->Restore() : result;
	}

	// Original function: FUN_0047b2b8
	std::uint32_t CDDDisplay::l_GetSurfaceDepth(IDirectDrawSurface7* surface)
	{
		constexpr const char* sourceFile = R"(D:\\Dev\\Projects\\FunaticsOld\\ProjectD\\BaseLibs\\XSystem\\X2DApp\\X2DDDDisplay.cpp)";
		DDSURFACEDESC2 description{ };
		description.dwSize = sizeof(description);
		HRESULT result;
		do
		{
			result = surface->Lock(nullptr, &description, DDLOCK_WAIT, nullptr);
		} while (result == DDERR_WASSTILLDRAWING);
		if (result == DDERR_SURFACELOST)
		{
			result = l_RestoreSurfaces();
			if (result != DD_OK)
			{
				NXSysMisc::XWS_DirectDrawErrorBox(result, sourceFile, 0x33A);
				return 0;
			}
			result = surface->Lock(nullptr, &description, DDLOCK_WAIT, nullptr);
		}
		if (result != DD_OK)
		{
			NXSysMisc::XWS_DirectDrawErrorBox(result, sourceFile, 0x33A);
			return 0;
		}
		DDPIXELFORMAT format{ };
		format.dwSize = sizeof(format);
		result = surface->GetPixelFormat(&format);
		std::uint32_t depth = 0;
		if (result == DD_OK) depth = format.dwRGBBitCount;
		else NXSysMisc::XWS_DirectDrawErrorBox(result, sourceFile, 0x353);
		surface->Unlock(nullptr);
		return depth;
	}

	// Original function: FUN_0047b38e
	bool CDDDisplay::SetDisplayMode(std::int32_t width, std::int32_t height, std::int32_t depth)
	{
		if (mWidth == width && mHeight == height && mDepth == depth)
		{
			return true;
		}

		ShutDown();

		mWidth = width;
		mHeight = height;
		mDepth = depth;

		mInitialized = l_StartUp();
		return mInitialized;
	}

	// Original function: FUN_0047b3d6
	bool CDDDisplay::ToggleFullscreen()
	{
		ShutDown();

		mFullscreen = !mFullscreen;

		if (mFullscreen)
		{
			mSavedWindowOffsetX = mWindowOffsetX;
			mSavedWindowOffsetY = mWindowOffsetY;

			mWindowOffsetX = 0;
			mWindowOffsetY = 0;
		}
		else
		{
			mWindowOffsetX = mSavedWindowOffsetX;
			mWindowOffsetY = mSavedWindowOffsetY;
		}

		mInitialized = l_StartUp();

		CApplication* application = CApplication::sTheObjectPtr;

		if (application != nullptr && application->fullscreenToggledCallback != nullptr)
		{
			application->fullscreenToggledCallback();
		}

		if (NXSysMouseManager::CMouseManager::sTheObjectPtr != nullptr)
		{
			NXSysMouseManager::CMouseManager::sTheObjectPtr->Callback_FullscreenToggled(mFullscreen);
		}

		return mInitialized;
	}

	// Original function: FUN_0047b43c
	void CDDDisplay::FlipToGDISurface()
	{
		if (mFullscreen)
		{
			mDirectDraw->FlipToGDISurface();
		}
	}

	// Original function: FUN_0047b44c
	void CDDDisplay::Update()
	{
		CApplication* application = CApplication::sTheObjectPtr;

		if (application == nullptr || mPrimarySurface == nullptr || mBackSurface == nullptr)
		{
			return;
		}

		if (mFullscreen)
		{
			if (!application->IsWindowActive())
			{
				return;
			}

			for (;;)
			{
				if (!gSkipPaletteUpdateBeforePresent && mDirectDrawPalette != nullptr)
				{
					mPrimarySurface->SetPalette(mDirectDrawPalette);
				}

				const DWORD flipFlags = gWaitForFullscreenFlip ? GetFullscreenFlipFlags() : DDFLIP_NOVSYNC;
				const HRESULT result = mPrimarySurface->Flip(nullptr, flipFlags);

				mFullscreenFlipState = !mFullscreenFlipState;

				if (result == DD_OK)
				{
					return;
				}

				if (result == DDERR_SURFACELOST)
				{
					if (l_RestoreSurfaces() != DD_OK)
					{
						return;
					}

					return;
				}

				if (result != DDERR_WASSTILLDRAWING)
				{
					return;
				}
			}
		}

		RECT destinationRectangle{ mWindowOffsetX, mWindowOffsetY, mWindowOffsetX + mWidth, mWindowOffsetY + mHeight };

		for (;;)
		{
			if (!gSkipPaletteUpdateBeforePresent && mDirectDrawPalette != nullptr)
			{
				mBackSurface->SetPalette(mDirectDrawPalette);
			}

			const HRESULT result = mPrimarySurface->Blt(&destinationRectangle, mBackSurface, nullptr, DDBLT_WAIT, nullptr);

			if (result == DD_OK)
			{
				return;
			}

			if (result == DDERR_SURFACELOST)
			{
				if (l_RestoreSurfaces() != DD_OK)
				{
					return;
				}

				return;
			}

			if (result != DDERR_WASSTILLDRAWING)
			{
				return;
			}
		}
	}

	// Original function: FUN_0047b8db
	std::uint32_t CDDDisplay::l_GetColorMask(IDirectDrawSurface7* surface, COLORREF color)
	{
		if (color == CLR_INVALID)
		{
			return UINT32_MAX;
		}

		HDC deviceContext = nullptr;

		if (surface->GetDC(&deviceContext) != DD_OK)
		{
			return UINT32_MAX;
		}

		const COLORREF previousColor = GetPixel(deviceContext, 0, 0);

		SetPixel(deviceContext, 0, 0, color);
		surface->ReleaseDC(deviceContext);

		DDSURFACEDESC2 surfaceDescription{ };
		surfaceDescription.dwSize = sizeof(surfaceDescription);

		HRESULT result{ };

		do
		{
			result = surface->Lock(nullptr, &surfaceDescription, 0, nullptr);
		} while (result == DDERR_WASSTILLDRAWING);

		if (result != DD_OK)
		{
			return UINT32_MAX;
		}

		std::uint32_t colorMask = *static_cast<const std::uint32_t*>(surfaceDescription.lpSurface);

		const DWORD bitCount = surfaceDescription.ddpfPixelFormat.dwRGBBitCount;

		if (bitCount < 32)
		{
			colorMask &= (std::uint32_t{ 1 } << bitCount) - 1;
		}

		surface->Unlock(nullptr);
		deviceContext = nullptr;

		if (surface->GetDC(&deviceContext) == DD_OK)
		{
			SetPixel(deviceContext, 0, 0, previousColor);
			surface->ReleaseDC(deviceContext);
		}

		return colorMask;
	}

	struct CDDDisplay::SBitmapSurface
	{
		DDSURFACEDESC2 description;
		IDirectDrawSurface7* surface;
		NXBasics::SRectangle rectangle{ 0, 0, 0, 0 };
	};

	// Original function: FUN_0047b9b4.
	std::int32_t CDDDisplay::GetWindowOffsetX() const
	{
		return mFullscreen ? 0 : mWindowOffsetX;
	}

	// Original function: FUN_0047b9c1.
	std::int32_t CDDDisplay::GetWindowOffsetY() const
	{
		return mFullscreen ? 0 : mWindowOffsetY;
	}

	// Original function: FUN_0047b9ce. Bitmap origin fields do not affect placement.
	void CDDDisplay::BitmapToScreen(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		std::uint32_t depth;
		if (mFullscreen)
		{
			if (!CApplication::sTheObjectPtr->IsWindowActive()) return;
			depth = static_cast<std::uint32_t>(mDepth);
			if (depth == 8)
			{
				if (bitmap->mBytesPerPixel != 1) NXSysMisc::XWS_AbortBox("NXSys2DApp::CDDDisplay::BitmapToScreen\n\nCan only work with 8 or 16 bit bitmaps in window mode when desktop bitdepth is 16 bit");
				l_MemoryToScreen(bitmap->mRawMemory, static_cast<std::uint32_t>(bitmap->mRectangle.width), static_cast<std::uint32_t>(bitmap->mRectangle.height), bitmap->mRowPixelCount, static_cast<std::uint32_t>(x), static_cast<std::uint32_t>(y), bitmap->mBytesPerPixel);
				return;
			}
			if (depth != 16 && depth != 32)
			{
				NXSysMisc::XWS_AbortBox("NXSys2DApp::CDDDisplay::BitmapToScreen\n\nunknown fullscreen display depth");
				return;
			}
		}
		else
		{
			depth = CApplication::sTheObjectPtr->GetDesktopDepth();
			if (depth != 16 && depth != 32)
			{
				NXSysMisc::XWS_AbortBox("NXSys2DApp::CDDDisplay::BitmapToScreen\n\ncan only run in window mode when desktop bitdepth is 16 or 32 bit");
				return;
			}
		}
		if (depth == 16)
		{
			if (bitmap->mBytesPerPixel != 1 && bitmap->mBytesPerPixel != 2) NXSysMisc::XWS_AbortBox("NXSys2DApp::CDDDisplay::BitmapToScreen\n\ncan only work with 8 or 16 bit bitmaps in window mode when desktop bitdepth is 16 bit");
			if (bitmap->mBytesPerPixel == 1) l_BitmapToScreen8To16(bitmap, x, y);
			else if (bitmap->mBytesPerPixel == 2) l_BitmapToScreen16To16(bitmap, x, y);
		}
		else
		{
			if (bitmap->mBytesPerPixel == 1) l_BitmapToScreen8To32(bitmap, x, y);
			else if (bitmap->mBytesPerPixel == 2) l_BitmapToScreen16To32(bitmap, x, y);
			else if (bitmap->mBytesPerPixel == 4) l_BitmapToScreen32To32(bitmap, x, y);
		}
	}

	// Original function: FUN_0047baf4. The indexed fullscreen path uses unsigned bounds.
	void CDDDisplay::l_MemoryToScreen(const std::uint8_t* pixels, std::uint32_t width, std::uint32_t height, std::uint32_t rowPixelCount, std::uint32_t x, std::uint32_t y, std::uint32_t bytesPerPixel)
	{
		if (CApplication::sTheObjectPtr->mWindow == nullptr) return;
		if (mFullscreen && !CApplication::sTheObjectPtr->IsWindowActive()) return;
		IDirectDrawSurface7* surface = mBackSurface;
		DDSURFACEDESC2 description{ };
		description.dwSize = sizeof(description);
		HRESULT result;
		do
		{
			result = surface->Lock(nullptr, &description, DDLOCK_WAIT, nullptr);
		} while (result == DDERR_WASSTILLDRAWING);
		if (result == DDERR_SURFACELOST)
		{
			if (l_RestoreSurfaces() != DD_OK) return;
			result = surface->Lock(nullptr, &description, DDLOCK_WAIT, nullptr);
		}
		if (result != DD_OK) return;
		if (x < description.dwWidth && y < description.dwHeight)
		{
			if (width + x >= description.dwWidth) width = description.dwWidth - x;
			if (height + y >= description.dwHeight) height = description.dwHeight - y;
			auto destination = reinterpret_cast<std::uintptr_t>(description.lpSurface) + static_cast<std::uint32_t>(description.lPitch) * y + x;
			auto source = reinterpret_cast<std::uintptr_t>(pixels);
			for (std::uint32_t row = 0; row < height; ++row)
			{
				std::memmove(reinterpret_cast<void*>(destination), reinterpret_cast<const void*>(source), width * bytesPerPixel);
				destination += static_cast<std::uint32_t>(description.lPitch);
				source += rowPixelCount * bytesPerPixel;
			}
		}
		surface->Unlock(nullptr);
	}

	// Original function: FUN_0047bc01. The restore retry is a single Lock call.
	bool CDDDisplay::l_BitmapSurface_Lock(SBitmapSurface& target, IDirectDrawSurface7* surface, bool fullscreen)
	{
		static_assert(offsetof(SBitmapSurface, surface) == 0x7C);
		static_assert(offsetof(SBitmapSurface, rectangle) == 0x80);
		static_assert(sizeof(SBitmapSurface) == 144);
		if (CApplication::sTheObjectPtr->mWindow == nullptr) return false;
		if (fullscreen && !CApplication::sTheObjectPtr->IsWindowActive()) return false;
		target.surface = surface;
		std::memset(&target.description, 0, sizeof(target.description));
		target.description.dwSize = sizeof(target.description);
		HRESULT result;
		do
		{
			result = target.surface->Lock(nullptr, &target.description, DDLOCK_WAIT, nullptr);
		} while (result == DDERR_WASSTILLDRAWING);
		if (result == DDERR_SURFACELOST)
		{
			if (l_RestoreSurfaces() != DD_OK) return false;
			result = target.surface->Lock(nullptr, &target.description, DDLOCK_WAIT, nullptr);
		}
		return result == DD_OK;
	}

	// Original function: FUN_0047bc80. Signed intersection precedes unsigned clipping.
	bool CDDDisplay::l_BitmapSurface_SetRectangle(SBitmapSurface& target, std::int32_t x, std::int32_t y, std::int32_t width, std::int32_t height)
	{
		const NXBasics::SRectangle bounds(0, 0, NXBasics::Runtime_TimeSigned32(target.description.dwWidth), NXBasics::Runtime_TimeSigned32(target.description.dwHeight));
		target.rectangle.SetVariables(x, y, width, height);
		auto& rectangle = target.rectangle;
		if (!rectangle.IsTouching(bounds)) return false;
		if (rectangle.x < 0)
		{
			rectangle.width = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.width) + static_cast<std::uint32_t>(rectangle.x));
			rectangle.x = 0;
		}
		if (static_cast<std::uint32_t>(rectangle.width) + static_cast<std::uint32_t>(rectangle.x) > target.description.dwWidth) rectangle.width = NXBasics::Runtime_TimeSigned32(target.description.dwWidth - static_cast<std::uint32_t>(rectangle.x));
		if (rectangle.y < 0)
		{
			rectangle.height = NXBasics::Runtime_TimeSigned32(static_cast<std::uint32_t>(rectangle.height) + static_cast<std::uint32_t>(rectangle.y));
			rectangle.y = 0;
		}
		if (static_cast<std::uint32_t>(rectangle.height) + static_cast<std::uint32_t>(rectangle.y) > target.description.dwHeight) rectangle.height = NXBasics::Runtime_TimeSigned32(target.description.dwHeight - static_cast<std::uint32_t>(rectangle.y));
		return true;
	}

	// Original function: FUN_0047bd2a.
	void CDDDisplay::l_BitmapToScreen8To16(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		if (!NXBasics::g16BitPixelConverter->IsInitialized()) l_Configure16BitColorMasks();
		const auto* table = bitmap->GetHighColorTablePtr();
		if (table == nullptr) return;
		SBitmapSurface target;
		if (!l_BitmapSurface_Lock(target, mBackSurface, mFullscreen)) return;
		if (!l_BitmapSurface_SetRectangle(target, x, y, bitmap->mRectangle.width, bitmap->mRectangle.height))
		{
			target.surface->Unlock(nullptr);
			return;
		}
		const auto& area = target.rectangle;
		const std::uint32_t pitchPixels = static_cast<std::uint32_t>(target.description.lPitch >> 1);
		const std::uint32_t lastRow = static_cast<std::uint32_t>(area.y) + static_cast<std::uint32_t>(area.height) - 1u;
		std::uint32_t destinationRow = static_cast<std::uint32_t>(area.y) * pitchPixels;
		for (std::uint32_t row = static_cast<std::uint32_t>(area.y); row <= lastRow; ++row)
		{
			const std::uint32_t sourceIndex = (row - static_cast<std::uint32_t>(y)) * bitmap->mRowPixelCount - static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(area.x);
			auto source = reinterpret_cast<std::uintptr_t>(bitmap->mRawMemory) + sourceIndex * 1u;
			auto destination = reinterpret_cast<std::uintptr_t>(target.description.lpSurface) + (destinationRow + static_cast<std::uint32_t>(area.x)) * 2u;
			const std::uint32_t lastColumn = static_cast<std::uint32_t>(area.x) + static_cast<std::uint32_t>(area.width) - 1u;
			for (std::uint32_t column = static_cast<std::uint32_t>(area.x); column <= lastColumn; ++column)
			{
				std::uint8_t pixel;
				std::memcpy(&pixel, reinterpret_cast<const void*>(source), sizeof(pixel));
				const std::uint16_t color = table[pixel];
				std::memcpy(reinterpret_cast<void*>(destination), &color, sizeof(color));
				source += 1;
				destination += 2;
			}
			destinationRow += pitchPixels;
		}
		target.surface->Unlock(nullptr);
	}

	// Original function: FUN_0047be41.
	void CDDDisplay::l_BitmapToScreen8To32(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		if (!NXBasics::g32BitPixelConverter->IsInitialized()) l_Configure32BitColorMasks();
		const auto* table = bitmap->GetTrueColorTablePtr();
		if (table == nullptr) return;
		SBitmapSurface target;
		if (!l_BitmapSurface_Lock(target, mBackSurface, mFullscreen)) return;
		if (!l_BitmapSurface_SetRectangle(target, x, y, bitmap->mRectangle.width, bitmap->mRectangle.height))
		{
			target.surface->Unlock(nullptr);
			return;
		}
		const auto& area = target.rectangle;
		const std::uint32_t pitchPixels = static_cast<std::uint32_t>(target.description.lPitch >> 2);
		const std::uint32_t lastRow = static_cast<std::uint32_t>(area.y) + static_cast<std::uint32_t>(area.height) - 1u;
		std::uint32_t destinationRow = static_cast<std::uint32_t>(area.y) * pitchPixels;
		for (std::uint32_t row = static_cast<std::uint32_t>(area.y); row <= lastRow; ++row)
		{
			const std::uint32_t sourceIndex = (row - static_cast<std::uint32_t>(y)) * bitmap->mRowPixelCount - static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(area.x);
			auto source = reinterpret_cast<std::uintptr_t>(bitmap->mRawMemory) + sourceIndex * 1u;
			auto destination = reinterpret_cast<std::uintptr_t>(target.description.lpSurface) + (destinationRow + static_cast<std::uint32_t>(area.x)) * 4u;
			const std::uint32_t lastColumn = static_cast<std::uint32_t>(area.x) + static_cast<std::uint32_t>(area.width) - 1u;
			for (std::uint32_t column = static_cast<std::uint32_t>(area.x); column <= lastColumn; ++column)
			{
				std::uint8_t pixel;
				std::memcpy(&pixel, reinterpret_cast<const void*>(source), sizeof(pixel));
				const std::uint32_t color = table[pixel];
				std::memcpy(reinterpret_cast<void*>(destination), &color, sizeof(color));
				source += 1;
				destination += 4;
			}
			destinationRow += pitchPixels;
		}
		target.surface->Unlock(nullptr);
	}

	// Original function: FUN_0047bf58.
	void CDDDisplay::l_BitmapToScreen16To16(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		SBitmapSurface target;
		if (!l_BitmapSurface_Lock(target, mBackSurface, mFullscreen)) return;
		if (!l_BitmapSurface_SetRectangle(target, x, y, bitmap->mRectangle.width, bitmap->mRectangle.height))
		{
			target.surface->Unlock(nullptr);
			return;
		}
		const auto& area = target.rectangle;
		const std::uint32_t pitchPixels = static_cast<std::uint32_t>(target.description.lPitch >> 1);
		const std::uint32_t lastRow = static_cast<std::uint32_t>(area.y) + static_cast<std::uint32_t>(area.height) - 1u;
		std::uint32_t destinationRow = static_cast<std::uint32_t>(area.y) * pitchPixels;
		for (std::uint32_t row = static_cast<std::uint32_t>(area.y); row <= lastRow; ++row)
		{
			const std::uint32_t sourceIndex = (row - static_cast<std::uint32_t>(y)) * bitmap->mRowPixelCount - static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(area.x);
			auto source = reinterpret_cast<std::uintptr_t>(bitmap->mHighColorMemory) + sourceIndex * 2u;
			auto destination = reinterpret_cast<std::uintptr_t>(target.description.lpSurface) + (destinationRow + static_cast<std::uint32_t>(area.x)) * 2u;
			std::memmove(reinterpret_cast<void*>(destination), reinterpret_cast<const void*>(source), static_cast<std::uint32_t>(area.width) * 2u);
			destinationRow += pitchPixels;
		}
		target.surface->Unlock(nullptr);
	}

	// Original function: FUN_0047c029.
	void CDDDisplay::l_BitmapToScreen16To32(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		if (!NXBasics::g16BitPixelConverter->IsInitialized()) l_Configure16BitColorMasks();
		const auto* table = NXBasics::g16BitPixelConverter->GetTrueColorConvertTablePtr();
		if (table == nullptr) return;
		SBitmapSurface target;
		if (!l_BitmapSurface_Lock(target, mBackSurface, mFullscreen)) return;
		if (!l_BitmapSurface_SetRectangle(target, x, y, bitmap->mRectangle.width, bitmap->mRectangle.height))
		{
			target.surface->Unlock(nullptr);
			return;
		}
		const auto& area = target.rectangle;
		const std::uint32_t pitchPixels = static_cast<std::uint32_t>(target.description.lPitch >> 2);
		const std::uint32_t lastRow = static_cast<std::uint32_t>(area.y) + static_cast<std::uint32_t>(area.height) - 1u;
		std::uint32_t destinationRow = static_cast<std::uint32_t>(area.y) * pitchPixels;
		for (std::uint32_t row = static_cast<std::uint32_t>(area.y); row <= lastRow; ++row)
		{
			const std::uint32_t sourceIndex = (row - static_cast<std::uint32_t>(y)) * bitmap->mRowPixelCount - static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(area.x);
			auto source = reinterpret_cast<std::uintptr_t>(bitmap->mHighColorMemory) + sourceIndex * 2u;
			auto destination = reinterpret_cast<std::uintptr_t>(target.description.lpSurface) + (destinationRow + static_cast<std::uint32_t>(area.x)) * 4u;
			const std::uint32_t lastColumn = static_cast<std::uint32_t>(area.x) + static_cast<std::uint32_t>(area.width) - 1u;
			for (std::uint32_t column = static_cast<std::uint32_t>(area.x); column <= lastColumn; ++column)
			{
				std::uint16_t pixel;
				std::memcpy(&pixel, reinterpret_cast<const void*>(source), sizeof(pixel));
				const std::uint32_t color = table[pixel];
				std::memcpy(reinterpret_cast<void*>(destination), &color, sizeof(color));
				source += 2;
				destination += 4;
			}
			destinationRow += pitchPixels;
		}
		target.surface->Unlock(nullptr);
	}

	// Original function: FUN_0047c14c.
	void CDDDisplay::l_BitmapToScreen32To32(NXBasics::CBitmap* bitmap, std::int32_t x, std::int32_t y)
	{
		SBitmapSurface target;
		if (!l_BitmapSurface_Lock(target, mBackSurface, mFullscreen)) return;
		if (!l_BitmapSurface_SetRectangle(target, x, y, bitmap->mRectangle.width, bitmap->mRectangle.height))
		{
			target.surface->Unlock(nullptr);
			return;
		}
		const auto& area = target.rectangle;
		const std::uint32_t pitchPixels = static_cast<std::uint32_t>(target.description.lPitch >> 2);
		const std::uint32_t lastRow = static_cast<std::uint32_t>(area.y) + static_cast<std::uint32_t>(area.height) - 1u;
		std::uint32_t destinationRow = static_cast<std::uint32_t>(area.y) * pitchPixels;
		for (std::uint32_t row = static_cast<std::uint32_t>(area.y); row <= lastRow; ++row)
		{
			const std::uint32_t sourceIndex = (row - static_cast<std::uint32_t>(y)) * bitmap->mRowPixelCount - static_cast<std::uint32_t>(x) + static_cast<std::uint32_t>(area.x);
			auto source = reinterpret_cast<std::uintptr_t>(bitmap->mTrueColorMemory) + sourceIndex * 4u;
			auto destination = reinterpret_cast<std::uintptr_t>(target.description.lpSurface) + (destinationRow + static_cast<std::uint32_t>(area.x)) * 4u;
			std::memmove(reinterpret_cast<void*>(destination), reinterpret_cast<const void*>(source), static_cast<std::uint32_t>(area.width) * 4u);
			destinationRow += pitchPixels;
		}
		target.surface->Unlock(nullptr);
	}

	// Original function: FUN_0047c21f
	void CDDDisplay::l_CreatePalettes()
	{
		if (mPaletteData == nullptr)
		{
			return;
		}

		mWindowsPalette = CreatePalette(mPaletteData);
		l_SetPaletteEntries(mPaletteData->palPalEntry);
	}

	// Original function: FUN_0047c23e
	void CDDDisplay::l_DestroyPalettes()
	{
		if (mDirectDrawPalette != nullptr)
		{
			mDirectDrawPalette->Release();
			mDirectDrawPalette = nullptr;
		}

		if (mWindowsPalette != nullptr)
		{
			DeleteObject(mWindowsPalette);
			mWindowsPalette = nullptr;
		}
	}

	// Original function: FUN_0047c266
	bool CDDDisplay::l_SetPaletteEntries(const PALETTEENTRY* paletteEntries)
	{
		if (mPrimarySurface == nullptr || mDirectDraw == nullptr || paletteEntries == nullptr)
		{
			return false;
		}

		DDSURFACEDESC2 surfaceDescription{ };
		surfaceDescription.dwSize = sizeof(surfaceDescription);

		mPrimarySurface->GetSurfaceDesc(&surfaceDescription);

		if ((surfaceDescription.ddpfPixelFormat.dwFlags & DDPF_PALETTEINDEXED8) == 0)
		{
			return false;
		}

		if (mDirectDrawPalette != nullptr)
		{
			mDirectDrawPalette->Release();
			mDirectDrawPalette = nullptr;
		}

		HRESULT result = mDirectDraw->CreatePalette(DDPCAPS_8BIT, const_cast<PALETTEENTRY*>(paletteEntries), &mDirectDrawPalette, nullptr);

		if (result != DD_OK)
		{
			OutputDebugStringA("CDDDisplay::l_SetPaletteEntries: CreatePalette failed.\r\n");

			return false;
		}

		result = mPrimarySurface->SetPalette(mDirectDrawPalette);

		if (result == DDERR_SURFACELOST)
		{
			l_RestoreSurfaces();
		}

		if (result != DD_OK)
		{
			mDirectDrawPalette->Release();
			mDirectDrawPalette = nullptr;
			return false;
		}

		return true;
	}

	// Original function: FUN_0047c588
	void CDDDisplay::l_Configure16BitColorMasks()
	{
		if (l_GetSurfaceDepth(mBackSurface) == 16)
		{
			auto* converter = NXBasics::g16BitPixelConverter;
			auto* surface = mBackSurface;
			DDPIXELFORMAT format{ };
			format.dwSize = sizeof(format);
			// Requested fullscreen colour correction: use the surface's RGB
			// masks directly. GDI colour probes may fail on an exclusive surface.
			if (surface->GetPixelFormat(&format) == DD_OK && (format.dwFlags & DDPF_RGB) != 0 && format.dwRGBBitCount == 16 && format.dwRBitMask != 0 && format.dwGBitMask != 0 && format.dwBBitMask != 0 && ((format.dwRBitMask | format.dwGBitMask | format.dwBBitMask) & NXBasics::BitMask::DwordExceptBits0Through15) == 0 && (format.dwRBitMask & format.dwGBitMask) == 0 && (format.dwRBitMask & format.dwBBitMask) == 0 && (format.dwGBitMask & format.dwBBitMask) == 0)
			{
				converter->SetColorMasks(static_cast<std::uint16_t>(format.dwRBitMask), static_cast<std::uint16_t>(format.dwGBitMask), static_cast<std::uint16_t>(format.dwBBitMask));
				return;
			}
			const auto blue = static_cast<std::uint16_t>(l_GetColorMask(surface, RGB(0, 0, 255)));
			const auto green = static_cast<std::uint16_t>(l_GetColorMask(surface, RGB(0, 255, 0)));
			const auto red = static_cast<std::uint16_t>(l_GetColorMask(surface, RGB(255, 0, 0)));
			converter->SetColorMasks(red, green, blue);
			return;
		}
		NXBasics::g16BitPixelConverter->SetColorMasks(NXBasics::Rgb565::RedMask, NXBasics::Rgb565::GreenMask, 0x001F);
	}

	// Original function: FUN_0047c5f1
	void CDDDisplay::l_Configure32BitColorMasks()
	{
		if (l_GetSurfaceDepth(mBackSurface) == 32)
		{
			auto* converter = NXBasics::g32BitPixelConverter;
			auto* surface = mBackSurface;
			DDPIXELFORMAT format{ };
			format.dwSize = sizeof(format);
			// Keep the native probe as fallback for surfaces without RGB masks.
			if (surface->GetPixelFormat(&format) == DD_OK && (format.dwFlags & DDPF_RGB) != 0 && format.dwRGBBitCount == 32 && format.dwRBitMask != 0 && format.dwGBitMask != 0 && format.dwBBitMask != 0 && (format.dwRBitMask & format.dwGBitMask) == 0 && (format.dwRBitMask & format.dwBBitMask) == 0 && (format.dwGBitMask & format.dwBBitMask) == 0)
			{
				converter->SetColorMasks(format.dwRBitMask, format.dwGBitMask, format.dwBBitMask);
				return;
			}
			const auto blue = l_GetColorMask(surface, RGB(0, 0, 255));
			const auto green = l_GetColorMask(surface, RGB(0, 255, 0));
			const auto red = l_GetColorMask(surface, RGB(255, 0, 0));
			converter->SetColorMasks(red, green, blue);
			return;
		}
		NXBasics::g32BitPixelConverter->SetColorMasks(NXBasics::Rgb888::RedMask, NXBasics::Rgb888::GreenMask, NXBasics::IntegerRange::UInt8MaximumSigned);
	}
}
