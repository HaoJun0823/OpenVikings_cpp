#include "X2DDisplay.h"

#include "X2DApplication.h"
#include "XMMouseManager.h"
#include "XPixelFormatConverter.h"

#include <Windows.h>
#include <ddraw.h>

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <malloc.h>

#pragma comment(lib, "dxguid.lib")

namespace
{
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
		mPadding11{},
		mDirectDrawModule(nullptr),
		mDirectDraw(nullptr),
		mPrimarySurface(nullptr),
		mBackSurface(nullptr),
		mClipper(nullptr),
		mDirectDrawPalette(nullptr),
		mWindowsPalette(nullptr),
		mPaletteData(nullptr),
		mFullscreen(fullscreen),
		mPadding35{},
		mWindowOffsetX(0),
		mWindowOffsetY(0),
		mSavedWindowOffsetX(0),
		mSavedWindowOffsetY(0),
		mTransparentColor(0),
		mFullscreenFlipState(false),
		mPadding4D{},
		mGamma(1.0F)
	{}

	// Original function: FUN_0047abbc
	CDDDisplay::~CDDDisplay()
	{
		ShutDown();

		std::free(mPaletteData);
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

		if (mPaletteData == nullptr)
		{
			/*
			 * The original allocates 0x408 bytes, although a LOGPALETTE
			 * containing 256 PALETTEENTRY values only requires 0x404.
			 */
			mPaletteData = static_cast<LOGPALETTE*>(std::malloc(0x408));

			if (mPaletteData == nullptr)
			{
				mInitialized = false;
				return false;
			}
		}

		mPaletteData->palVersion = 0x0300;
		mPaletteData->palNumEntries = 0x0100;

		for (std::uint32_t index = 0; index < 256; ++index)
		{
			PALETTEENTRY& entry = mPaletteData->palPalEntry[index];

			entry.peRed = static_cast<BYTE>(index);
			entry.peGreen = static_cast<BYTE>(index);
			entry.peBlue = static_cast<BYTE>(index);
			entry.peFlags = 0;
		}

		mInitialized = l_StartUp();
		return mInitialized;
	}

	// Original function: FUN_0047b057
	bool CDDDisplay::l_DoesInitializationHaveError(HRESULT result)
	{
		if (result == DD_OK)
		{
			return false;
		}

		char errorMessage[200]{};

		wsprintfA(errorMessage, "NXSys2DApp::CDDDisplay::l_DoesInitializationHaveError\n DirectDraw error: 0x%03X\r\n", static_cast<unsigned int>(result) & 0x0FFF);
		OutputDebugStringA(errorMessage);

		ShutDown();
		return true;
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

			DDSURFACEDESC2 surfaceDescription{};
			surfaceDescription.dwSize = sizeof(surfaceDescription);
			surfaceDescription.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			surfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
			surfaceDescription.dwBackBufferCount = 1;

			result = mDirectDraw->CreateSurface(&surfaceDescription, &mPrimarySurface, nullptr);

			if (l_DoesInitializationHaveError(result))
			{
				return false;
			}

			DDSCAPS2 backBufferCaps{};
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
			windowStyle &= 0x7F37FFFF;
			windowStyle |= 0x00C80000;

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

			DDSURFACEDESC2 primaryDescription{};
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

		DDBLTFX blitEffects{};
		blitEffects.dwSize = sizeof(blitEffects);
		blitEffects.dwFillColor = mTransparentColor;

		mBackSurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &blitEffects);
		mPrimarySurface->Blt(nullptr, nullptr, nullptr, DDBLT_COLORFILL, &blitEffects);

		RECT windowRectangle{};

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

	// Original function: FUN_0047b0b6
	void CDDDisplay::ShutDown()
	{
		if (mBackSurface != nullptr)
		{
			DDSURFACEDESC2 surfaceDescription{};
			surfaceDescription.dwSize = sizeof(surfaceDescription);

			HRESULT result{};

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
		DDSURFACEDESC2 surfaceDescription{};
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

		DDCOLORKEY colorKey{};

		if (mPrimarySurface != nullptr && mPrimarySurface->GetColorKey(DDCKEY_SRCBLT, &colorKey) == DD_OK)
		{
			surface->SetColorKey(DDCKEY_SRCBLT, &colorKey);
		}

		return surface;
	}

	// Original function: FUN_0047b29d
	HRESULT CDDDisplay::l_RestoreSurfaces()
	{
		if (mPrimarySurface == nullptr)
		{
			return DDERR_INVALIDOBJECT;
		}

		HRESULT result = mPrimarySurface->Restore();

		if (result != DD_OK)
		{
			return result;
		}

		if (mBackSurface != nullptr)
		{
			result = mBackSurface->Restore();
		}

		return result;
	}

	// Original function: FUN_0047b2b8
	std::uint32_t CDDDisplay::l_GetSurfaceDepth(
		IDirectDrawSurface7* surface)
	{
		if (surface == nullptr)
		{
			return 0;
		}

		DDSURFACEDESC2 surfaceDescription{};
		surfaceDescription.dwSize = sizeof(surfaceDescription);

		HRESULT result{};

		for (;;)
		{
			result = surface->Lock(nullptr, &surfaceDescription, DDLOCK_WAIT, nullptr);

			if (result == DDERR_WASSTILLDRAWING)
			{
				continue;
			}

			if (result == DDERR_SURFACELOST)
			{
				if (l_RestoreSurfaces() == DD_OK)
				{
					continue;
				}
			}

			break;
		}

		if (result != DD_OK)
		{
			OutputDebugStringA("CDDDisplay::l_GetSurfaceDepth: Lock failed.\r\n");

			return 0;
		}

		DDPIXELFORMAT pixelFormat{};
		pixelFormat.dwSize = sizeof(pixelFormat);

		const HRESULT pixelFormatResult = surface->GetPixelFormat(&pixelFormat);
		const std::uint32_t depth = pixelFormatResult == DD_OK ? pixelFormat.dwRGBBitCount : 0;

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
		if (surface == nullptr || color == CLR_INVALID)
		{
			return UINT32_MAX;
		}

		HDC deviceContext = nullptr;

		if (surface->GetDC(&deviceContext) != DD_OK || deviceContext == nullptr)
		{
			return UINT32_MAX;
		}

		const COLORREF previousColor = GetPixel(deviceContext, 0, 0);

		SetPixel(deviceContext, 0, 0, color);
		surface->ReleaseDC(deviceContext);

		DDSURFACEDESC2 surfaceDescription{};
		surfaceDescription.dwSize = sizeof(surfaceDescription);

		HRESULT result{};

		do
		{
			result = surface->Lock(nullptr, &surfaceDescription, DDLOCK_WAIT, nullptr);
		} while (result == DDERR_WASSTILLDRAWING);

		if (result != DD_OK || surfaceDescription.lpSurface == nullptr)
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

		if (surface->GetDC(&deviceContext) == DD_OK && deviceContext != nullptr)
		{
			SetPixel(deviceContext, 0, 0, previousColor);
			surface->ReleaseDC(deviceContext);
		}

		return colorMask;
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

		DDSURFACEDESC2 surfaceDescription{};
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
		if (NXBasics::g16BitPixelConverter == nullptr)
		{
			return;
		}

		if (l_GetSurfaceDepth(mBackSurface) == 16)
		{
			NXBasics::g16BitPixelConverter->SetColorMasks(
				static_cast<std::uint16_t>(l_GetColorMask(mBackSurface, RGB(255, 0, 0))),
				static_cast<std::uint16_t>(l_GetColorMask(mBackSurface, RGB(0, 255, 0))),
				static_cast<std::uint16_t>(l_GetColorMask(mBackSurface, RGB(0, 0, 255))));

			return;
		}

		NXBasics::g16BitPixelConverter->SetColorMasks(0xF800, 0x07E0, 0x001F);
	}

	// Original function: FUN_0047c5f1
	void CDDDisplay::l_Configure32BitColorMasks()
	{
		if (NXBasics::g32BitPixelConverter == nullptr)
		{
			return;
		}

		if (l_GetSurfaceDepth(mBackSurface) == 32)
		{
			NXBasics::g32BitPixelConverter->SetColorMasks(l_GetColorMask(mBackSurface, RGB(255, 0, 0)), l_GetColorMask(mBackSurface, RGB(0, 255, 0)), l_GetColorMask(mBackSurface, RGB(0, 0, 255)));

			return;
		}

		NXBasics::g32BitPixelConverter->SetColorMasks(0x00FF0000, 0x0000FF00, 0x000000FF);
	}
}