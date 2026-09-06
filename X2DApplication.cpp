#include "X2DApplication.h"
#include "X2DDisplay.h"
#include "XMMouseManager.h"
#include "XMKeyManager.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <vector>

#pragma comment(lib, "Ole32.lib")
#pragma comment(lib, "Version.lib")
#pragma comment(lib, "dxguid.lib")

namespace NXBasics
{
	class CXBSystemManager
	{
	public:
		CXBSystemManager();
		~CXBSystemManager();

		static CXBSystemManager* sTheObjectPtr;
		static void* sFatalErrorFunctionPtr;
		static void* sWarningErrorFunctionPtr;
		static void* sAssertFunctionPtr;
	};
}

namespace NXSysMisc
{
	void __cdecl XWS_AbortBox();
	void __cdecl XWS_WarningBox(const char* message);
	void __cdecl XWS_AssertBox();
}

namespace
{
	using DirectDrawCreateFunction = HRESULT(WINAPI*)(GUID*, LPDIRECTDRAW*, IUnknown*);
	using DirectDrawCreateExFunction = HRESULT(WINAPI*)(GUID*, void**, REFIID, IUnknown*);

	// Original function: FUN_0047a5aa
	void GetDXVersion(DWORD* directXVersion, DWORD* directXPlatform)
	{
		*directXVersion = 0;
		*directXPlatform = 0;

		OSVERSIONINFOA versionInformation{};
		versionInformation.dwOSVersionInfoSize = sizeof(versionInformation);

		if (!GetVersionExA(&versionInformation))
		{
			return;
		}

		if (versionInformation.dwPlatformId == VER_PLATFORM_WIN32_NT)
		{
			*directXPlatform = VER_PLATFORM_WIN32_NT;

			if (versionInformation.dwMajorVersion < 4)
			{
				return;
			}

			if (versionInformation.dwMajorVersion == 4)
			{
				*directXVersion = 0x0200;

				HMODULE directInputModule = LoadLibraryA("DINPUT.DLL");

				if (directInputModule == nullptr)
				{
					OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");
					return;
				}

				const FARPROC directInputCreate = GetProcAddress(directInputModule, "DirectInputCreateA");

				FreeLibrary(directInputModule);

				if (directInputCreate == nullptr)
				{
					OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n");
					return;
				}

				*directXVersion = 0x0300;
				return;
			}

			// Windows NT 5 and newer continue with the DirectDraw tests below.
		}
		else
		{
			*directXPlatform = VER_PLATFORM_WIN32_WINDOWS;
		}

		HMODULE directDrawModule = LoadLibraryA("DDRAW.DLL");

		if (directDrawModule == nullptr)
		{
			*directXPlatform = 0;
			return;
		}

		const auto directDrawCreate = reinterpret_cast<DirectDrawCreateFunction>(GetProcAddress(directDrawModule, "DirectDrawCreate"));

		if (directDrawCreate == nullptr)
		{
			*directXPlatform = 0;
			FreeLibrary(directDrawModule);
			OutputDebugStringA("Couldn't LoadLibrary DDraw\r\n");
			return;
		}

		IDirectDraw* directDraw = nullptr;

		if (FAILED(directDrawCreate(nullptr, &directDraw, nullptr)) || directDraw == nullptr)
		{
			*directXPlatform = 0;
			FreeLibrary(directDrawModule);
			OutputDebugStringA("Couldn't create DDraw\r\n");
			return;
		}

		*directXVersion = 0x0100;

		IDirectDraw2* directDraw2 = nullptr;

		if (FAILED(directDraw->QueryInterface(IID_IDirectDraw2, reinterpret_cast<void**>(&directDraw2))))
		{
			directDraw->Release();
			FreeLibrary(directDrawModule);
			OutputDebugStringA("Couldn't QI DDraw2\r\n");
			return;
		}

		directDraw2->Release();
		*directXVersion = 0x0200;

		HMODULE directInputModule = LoadLibraryA("DINPUT.DLL");

		if (directInputModule == nullptr)
		{
			OutputDebugStringA("Couldn't LoadLibrary DInput\r\n");
			directDraw->Release();
			FreeLibrary(directDrawModule);
			return;
		}

		const FARPROC directInputCreate = GetProcAddress(directInputModule, "DirectInputCreateA");
		FreeLibrary(directInputModule);

		if (directInputCreate == nullptr)
		{
			directDraw->Release();
			FreeLibrary(directDrawModule);
			OutputDebugStringA("Couldn't GetProcAddress DInputCreate\r\n");
			return;
		}

		*directXVersion = 0x0300;

		DDSURFACEDESC surfaceDescription{};
		surfaceDescription.dwSize = sizeof(surfaceDescription);
		surfaceDescription.dwFlags = DDSD_CAPS;
		surfaceDescription.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if (FAILED(directDraw->SetCooperativeLevel(nullptr, DDSCL_NORMAL)))
		{
			directDraw->Release();
			FreeLibrary(directDrawModule);
			*directXVersion = 0;
			OutputDebugStringA("Couldn't Set coop level\r\n");
			return;
		}

		IDirectDrawSurface* surface = nullptr;

		if (FAILED(directDraw->CreateSurface(&surfaceDescription, &surface, nullptr)) || surface == nullptr)
		{
			directDraw->Release();
			FreeLibrary(directDrawModule);
			*directXVersion = 0;
			OutputDebugStringA("Couldn't CreateSurface\r\n");
			return;
		}

		IDirectDrawSurface3* surface3 = nullptr;

		if (FAILED(surface->QueryInterface(IID_IDirectDrawSurface3, reinterpret_cast<void**>(&surface3))))
		{
			surface->Release();
			directDraw->Release();
			FreeLibrary(directDrawModule);
			return;
		}

		*directXVersion = 0x0500;
		surface3->Release();

		IDirectDrawSurface4* surface4 = nullptr;

		if (FAILED(surface->QueryInterface(IID_IDirectDrawSurface4, reinterpret_cast<void**>(&surface4))))
		{
			surface->Release();
			directDraw->Release();
			FreeLibrary(directDrawModule);
			return;
		}

		*directXVersion = 0x0600;
		surface4->Release();
		surface->Release();
		directDraw->Release();

		// CLSID_DirectMusic and IID_IDirectMusic from the legacy DirectMusic API.
		constexpr GUID directMusicClassId = { 0x636B9F10, 0x0C7D, 0x11D1, { 0x95, 0xB2, 0x00, 0x20, 0xAF, 0xDC, 0x74, 0x21 } };
		constexpr GUID directMusicInterfaceId = { 0x6536115A, 0x7B2D, 0x11D2, { 0xBA, 0x18, 0x00, 0x00, 0xF8, 0x75, 0xAC, 0x12 } };

		const HRESULT initializeResult = CoInitialize(nullptr);
		IUnknown* directMusic = nullptr;

		if (SUCCEEDED(CoCreateInstance(directMusicClassId, nullptr, CLSCTX_INPROC_SERVER, directMusicInterfaceId, reinterpret_cast<void**>(&directMusic))))
		{
			*directXVersion = 0x0601;
			directMusic->Release();
		}
		else
		{
			OutputDebugStringA("Couldn't create CLSID_DirectMusic\r\n");

			if (SUCCEEDED(initializeResult))
			{
				CoUninitialize();
			}

			FreeLibrary(directDrawModule);
			return;
		}

		if (SUCCEEDED(initializeResult))
		{
			CoUninitialize();
		}

		DWORD versionHandle = 0;
		const DWORD versionInfoSize = GetFileVersionInfoSizeA("dplayx.dll", &versionHandle);

		if (versionInfoSize == 0)
		{
			FreeLibrary(directDrawModule);
			return;
		}

		std::vector<std::byte> versionInfo(versionInfoSize);

		if (!GetFileVersionInfoA("dplayx.dll", 0, versionInfoSize, versionInfo.data()))
		{
			FreeLibrary(directDrawModule);
			return;
		}

		VS_FIXEDFILEINFO* fixedVersionInfo = nullptr;
		UINT fixedVersionInfoSize = 0;

		if (!VerQueryValueA(versionInfo.data(), "\\", reinterpret_cast<void**>(&fixedVersionInfo), &fixedVersionInfoSize) || fixedVersionInfo == nullptr)
		{
			FreeLibrary(directDrawModule);
			return;
		}

		if (fixedVersionInfo->dwFileVersionMS > 0x00040006 || (fixedVersionInfo->dwFileVersionMS == 0x00040006 && fixedVersionInfo->dwFileVersionLS >= 0x00030206))
		{
			*directXVersion = 0x0602;
		}

		const auto directDrawCreateEx = reinterpret_cast<DirectDrawCreateExFunction>(GetProcAddress(directDrawModule, "DirectDrawCreateEx"));

		if (directDrawCreateEx != nullptr)
		{
			IDirectDraw7* directDraw7 = nullptr;

			if (SUCCEEDED(directDrawCreateEx(nullptr, reinterpret_cast<void**>(&directDraw7), IID_IDirectDraw7, nullptr)) && directDraw7 != nullptr)
			{
				*directXVersion = 0x0700;
				directDraw7->Release();
			}
		}

		FreeLibrary(directDrawModule);

		HMODULE direct3D8Module = LoadLibraryA("D3D8.DLL");

		if (direct3D8Module != nullptr)
		{
			*directXVersion = 0x0800;
			FreeLibrary(direct3D8Module);
		}
	}
}

namespace NXSys2DApp
{
	CApplication* CApplication::sTheObjectPtr = nullptr;

	// Original function: FUN_00479076
	SApplicationControl::SApplicationControl()
	{
		std::memset(this, 0, sizeof(*this));

		screenWidth = 800;
		screenHeight = 600;
		screenDepth = 16;

		minimumScreenWidth = 640;
		minimumScreenHeight = 400;
		minimumScreenDepth = 8;

		maximumScreenWidth = 1600;
		maximumScreenHeight = 1200;
		maximumScreenDepth = 32;

		fullscreenToggle = true;
	}

	// Original function: FUN_004790cd
	CApplication::CApplication(const SApplicationControl& applicationControl) : SApplicationControl(applicationControl),
		mOwnsSystemManager(false),
		mPadding49{},
		mWindow(nullptr),
		mMouseWheelMessage(0),
		mDirectXVersion(0),
		mDirectXPlatform(0),
		mDisplayModeData{}
	{
		sTheObjectPtr = this;

		if (NXBasics::CXBSystemManager::sTheObjectPtr == nullptr)
		{
			new (std::nothrow) NXBasics::CXBSystemManager();
			mOwnsSystemManager = true;
		}

		NXBasics::CXBSystemManager::sFatalErrorFunctionPtr = reinterpret_cast<void*>(&NXSysMisc::XWS_AbortBox);
		NXBasics::CXBSystemManager::sWarningErrorFunctionPtr = reinterpret_cast<void*>(&NXSysMisc::XWS_WarningBox);
		NXBasics::CXBSystemManager::sAssertFunctionPtr = reinterpret_cast<void*>(&NXSysMisc::XWS_AssertBox);

		mMouseWheelMessage = RegisterWindowMessageA("MSWHEEL_ROLLMSG");

		GetDXVersion(&mDirectXVersion, &mDirectXPlatform);
		l_GetDesktopDisplayMode();
		l_GetFullscreenDisplayModes();
	}

	// Original function: FUN_00479169
	CApplication::~CApplication()
	{
		ShutDown();

		NXBasics::CXBSystemManager::sAssertFunctionPtr = nullptr;
		NXBasics::CXBSystemManager::sWarningErrorFunctionPtr = nullptr;

		if (mOwnsSystemManager && NXBasics::CXBSystemManager::sTheObjectPtr != nullptr)
		{
			delete NXBasics::CXBSystemManager::sTheObjectPtr;
		}

		sTheObjectPtr = nullptr;
	}

	// Original function: FUN_00478d8a
	void CApplication::l_GetDesktopDisplayMode()
	{
		HDC deviceContext = GetDC(nullptr);

		if (deviceContext != nullptr)
		{
			mDisplayModeData.desktopWidth = static_cast<std::uint32_t>(GetDeviceCaps(deviceContext, HORZRES));
			mDisplayModeData.desktopHeight = static_cast<std::uint32_t>(GetDeviceCaps(deviceContext, VERTRES));
			mDisplayModeData.desktopDepth = static_cast<std::uint32_t>(GetDeviceCaps(deviceContext, BITSPIXEL));

			ReleaseDC(nullptr, deviceContext);
			return;
		}

		mDisplayModeData.desktopWidth = 1024;
		mDisplayModeData.desktopHeight = 768;
		mDisplayModeData.desktopDepth = 16;
	}

	// Original function: FUN_00478ddf
	void CApplication::l_GetFullscreenDisplayModes()
	{
		mDisplayModeData.fullscreenDisplayModeCount = 0;

		HMODULE directDrawModule = LoadLibraryA("DDRAW.DLL");

		if (directDrawModule == nullptr)
		{
			return;
		}

		const auto directDrawCreateEx = reinterpret_cast<DirectDrawCreateExFunction>(GetProcAddress(directDrawModule, "DirectDrawCreateEx"));

		if (directDrawCreateEx != nullptr)
		{
			IDirectDraw7* directDraw = nullptr;

			if (SUCCEEDED(directDrawCreateEx(nullptr, reinterpret_cast<void**>(&directDraw), IID_IDirectDraw7, nullptr)) && directDraw != nullptr)
			{
				directDraw->EnumDisplayModes(0, nullptr, &mDisplayModeData, &CApplication::l_EnumDisplayModesCallback);
				directDraw->Release();
			}
		}

		FreeLibrary(directDrawModule);
	}

	// Original function: FUN_00478e4e
	HRESULT CALLBACK CApplication::l_EnumDisplayModesCallback(LPDDSURFACEDESC2 surfaceDescription, LPVOID context)
	{
		auto* displayModeData = static_cast<SDisplayModeData*>(context);

		if (displayModeData->fullscreenDisplayModeCount == 80)
		{
			return DDENUMRET_CANCEL;
		}

		constexpr DWORD requiredFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;

		if ((surfaceDescription->dwFlags & requiredFlags) != requiredFlags)
		{
			return DDENUMRET_OK;
		}

		DWORD displayDepth = 0;
		const DDPIXELFORMAT& pixelFormat = surfaceDescription->ddpfPixelFormat;

		if ((pixelFormat.dwFlags & DDPF_PALETTEINDEXED8) != 0)
		{
			displayDepth = 8;
		}
		else if ((pixelFormat.dwFlags & DDPF_RGB) != 0 && pixelFormat.dwRGBBitCount == 16)
		{
			displayDepth = 16;
		}
		else if ((pixelFormat.dwFlags & DDPF_RGB) != 0 && pixelFormat.dwRGBBitCount == 32)
		{
			displayDepth = 32;
		}
		else
		{
			return DDENUMRET_OK;
		}

		const DWORD displayWidth = surfaceDescription->dwWidth;
		const DWORD displayHeight = surfaceDescription->dwHeight;
		const CApplication* application = sTheObjectPtr;

		if (displayWidth < static_cast<DWORD>(application->minimumScreenWidth) ||
			displayHeight < static_cast<DWORD>(application->minimumScreenHeight) ||
			displayDepth < static_cast<DWORD>(application->minimumScreenDepth) ||
			displayWidth > static_cast<DWORD>(application->maximumScreenWidth) ||
			displayHeight > static_cast<DWORD>(application->maximumScreenHeight) ||
			displayDepth > static_cast<DWORD>(application->maximumScreenDepth))
		{
			return DDENUMRET_OK;
		}

		SDisplayMode& displayMode = displayModeData->fullscreenDisplayModes[displayModeData->fullscreenDisplayModeCount];

		displayMode.width = displayWidth;
		displayMode.height = displayHeight;
		displayMode.depth = displayDepth;

		++displayModeData->fullscreenDisplayModeCount;

		return DDENUMRET_OK;
	}

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
		CDDDisplay* display = mWindow->GetDisplay();

		if (display == nullptr)
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

		if (!display->SetDisplayMode(width, height, static_cast<std::int32_t>(depth)))
		{
			FUN_0047a9ea("NXSys2DApp::CApplication::SetDisplayMode\n\n could not set display mode (%d*%d-%d)", width, height, depth);
			PostMessageA(mWindow->GetWindowHandle(), WM_CLOSE, 0, 0);
			return false;
		}

		NXSysMouseManager::CMouseManager::sTheObjectPtr->ResetWindowsMouseState();

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

	// Original function: FUN_00478cfa
	bool CApplication::IsWindowActive() const
	{
		if (mWindow == nullptr)
		{
			return false;
		}

		return mWindow->IsWindowActive();
	}

	// Original function: FUN_00478d08
	bool CApplication::IsApplicationActive() const
	{
		if (mWindow == nullptr)
		{
			return false;
		}

		return mWindow->IsApplicationActive();
	}

	// Original function: FUN_00478ced
	HWND CApplication::GetWindowHandle() const
	{
		if (mWindow == nullptr)
		{
			return nullptr;
		}

		return mWindow->GetWindowHandle();
	}

	HINSTANCE CApplication::GetInstance() const
	{
		return instance;
	}

	// Original function: FUN_004791a6
	bool CApplication::StartUp()
	{
		if (mDirectXVersion < 0x0700)
		{
			NXSysMisc::XWS_WarningBox("NXSysMisc::CApplication::StartUp\n\n DirectX 7.0 required");
			return false;
		}

		HMODULE directDrawModule = LoadLibraryA("DDRAW.DLL");

		if (directDrawModule == nullptr)
		{
			return false;
		}

		const auto directDrawCreateEx = reinterpret_cast<DirectDrawCreateExFunction>(GetProcAddress(directDrawModule, "DirectDrawCreateEx"));

		if (directDrawCreateEx == nullptr)
		{
			FreeLibrary(directDrawModule);
			return false;
		}

		IDirectDraw7* directDraw = nullptr;
		const HRESULT result = directDrawCreateEx(nullptr, reinterpret_cast<void**>(&directDraw), IID_IDirectDraw7, nullptr);

		if (FAILED(result) || directDraw == nullptr)
		{
			FreeLibrary(directDrawModule);
			return false;
		}

		directDraw->Release();
		FreeLibrary(directDrawModule);

		mWindow = new (std::nothrow) CWindow(instance, showCommand, screenWidth, screenHeight, screenDepth, fullscreen);

		if (mWindow == nullptr || !mWindow->IsInitialized())
		{
			delete mWindow;
			mWindow = nullptr;
			return false;
		}

		new (std::nothrow) NXSysKeyManager::CKeyManager();
		new (std::nothrow) NXSysMouseManager::CMouseManager();

		return true;
	}

	// Original function: FUN_0047929c
	void CApplication::ShutDown()
	{
		delete mWindow;
		mWindow = nullptr;

		delete NXSysMouseManager::CMouseManager::sTheObjectPtr;
		delete NXSysKeyManager::CKeyManager::sTheObjectPtr;
	}

	// Original function: FUN_00479a54
	void CApplication::UpdateDisplay()
	{
		if (mWindow != nullptr)
		{
			mWindow->Update();
		}
	}

	// Original function: FUN_00478efa
	bool CApplication::CanDisplayModeBeActivated(bool fullscreen, std::uint32_t width, std::uint32_t height, std::uint32_t depth) const
	{
		if (!fullscreen)
		{
			if (width > mDisplayModeData.desktopWidth)
			{
				return false;
			}

			if (height > mDisplayModeData.desktopHeight)
			{
				return false;
			}

			return true;
		}

		if (mDisplayModeData.fullscreenDisplayModeCount == 0)
		{
			return true;
		}

		for (std::uint32_t index = 0; index < mDisplayModeData.fullscreenDisplayModeCount; ++index)
		{
			const SDisplayMode& displayMode = mDisplayModeData.fullscreenDisplayModes[index];

			if (displayMode.width == width && displayMode.height == height && displayMode.depth == depth)
			{
				return true;
			}
		}

		return false;
	}

	// Original function: FUN_00478c98
	bool CApplication::ToggleFullscreen()
	{
		if (mWindow == nullptr)
		{
			return false;
		}

		const std::uint32_t depth = static_cast<std::uint32_t>(GetDisplayDepth());
		const std::uint32_t height = static_cast<std::uint32_t>(GetDisplayHeight());
		const std::uint32_t width = static_cast<std::uint32_t>(GetDisplayWidth());
		const bool targetFullscreen = !IsFullscreen();

		if (!CanDisplayModeBeActivated(targetFullscreen, width, height, depth))
		{
			return false;
		}

		mWindow->ToggleFullscreen();

		return true;
	}

	// Original function: FUN_00478d38
	int CApplication::GetWindowOffsetX() const
	{
		const auto* window = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);

		if (window == nullptr)
		{
			return 0;
		}

		const auto* displayBackend = *reinterpret_cast<std::byte* const*>(window + 0x08);

		if (displayBackend == nullptr)
		{
			return 0;
		}

		const bool fullscreen = *reinterpret_cast<const bool*>(displayBackend + 0x34);

		if (fullscreen)
		{
			return 0;
		}

		return *reinterpret_cast<const std::int32_t*>(displayBackend + 0x38);
	}

	// Original function: FUN_00478d4b
	int CApplication::GetWindowOffsetY() const
	{
		const auto* window = *reinterpret_cast<std::byte* const*>(reinterpret_cast<const std::byte*>(this) + 0x4C);

		if (window == nullptr)
		{
			return 0;
		}

		const auto* displayBackend = *reinterpret_cast<std::byte* const*>(window + 0x08);

		if (displayBackend == nullptr)
		{
			return 0;
		}

		const bool fullscreen = *reinterpret_cast<const bool*>(displayBackend + 0x34);

		if (fullscreen)
		{
			return 0;
		}

		return *reinterpret_cast<const std::int32_t*>(displayBackend + 0x3C);
	}
}