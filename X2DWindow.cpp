#include "X2DWindow.h"

#include "X2DApplication.h"
#include "X2DDisplay.h"
#include "XMMouseManager.h"

#include <Windows.h>

#include <new>

namespace NXSys2DApp
{
    // Original function: FUN_0047b524
    CWindow::CWindow(HINSTANCE instance, int showCommand, std::int32_t width, std::int32_t height, std::int32_t depth, bool fullscreen) :
        mWindowHandle(nullptr),
        mApplicationActive(false),
        mWindowActive(false),
        mInitialized(false),
        mPadding07{},

        mDisplay(new (std::nothrow) CDDDisplay(fullscreen)),
        mInstance(instance),
        mShowCommand(showCommand)
    {
        WNDCLASSA windowClass{};
        windowClass.style = CS_HREDRAW | CS_VREDRAW;
        windowClass.lpfnWndProc = &CWindow::WindowProcedure;
        windowClass.hInstance = mInstance;
        windowClass.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
        windowClass.lpszClassName = WindowClassName;

        RegisterClassA(&windowClass);

        mWindowHandle = CreateWindowExA(WS_EX_TOPMOST, WindowClassName, WindowClassName, WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, mInstance, nullptr);

        if (mWindowHandle == nullptr)
        {
            return;
        }

        ShowWindow(mWindowHandle, mShowCommand);
        UpdateWindow(mWindowHandle);

        if (mDisplay == nullptr || !mDisplay->StartUp(mWindowHandle, width, height, depth))
        {
            OutputDebugStringA("NXSys2DApp::CWindow::CWindow\n Could not initialize the display.\r\n");

            PostMessageA(mWindowHandle, WM_CLOSE, 0, 0);
            return;
        }

        mApplicationActive = true;
        mWindowActive = true;
        mInitialized = true;
    }

    // Original function: FUN_0047b638
    LRESULT CALLBACK CWindow::WindowProcedure(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        CApplication* application = CApplication::sTheObjectPtr;

        if (application == nullptr)
        {
            return DefWindowProcA(windowHandle, message, wParam, lParam);
        }

        return application->HandleWindowMessage(windowHandle, message, wParam, lParam);
    }

    // Original function: FUN_0047b662
    LRESULT CWindow::HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        switch (message)
        {
        case WM_DESTROY:
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;

        case WM_MOVE:
            if (mDisplay != nullptr)
            {
                mDisplay->SetWindowOffsets(static_cast<std::int16_t>(LOWORD(lParam)), static_cast<std::int16_t>(HIWORD(lParam)));
            }

            return 0;

        case WM_ACTIVATE:
            mWindowActive = wParam != WA_INACTIVE;

            if (NXSysMouseManager::CMouseManager::sTheObjectPtr != nullptr)
            {
                NXSysMouseManager::CMouseManager::sTheObjectPtr->Callback_WindowActivationChanged(mWindowActive);
            }

            return 1;

        case WM_PAINT:
        {
            PAINTSTRUCT paint{};
            const HDC deviceContext = BeginPaint(mWindowHandle, &paint);

            if (deviceContext != nullptr)
            {
                RECT clientRectangle{};
                GetClientRect(mWindowHandle, &clientRectangle);
                EndPaint(mWindowHandle, &paint);
            }

            return 0;
        }

        case WM_ACTIVATEAPP:
        {
            mApplicationActive = wParam != FALSE;

            if (mApplicationActive && mDisplay != nullptr)
            {
                const HPALETTE palette = mDisplay->GetWindowsPalette();

                const HDC deviceContext = GetDC(windowHandle);

                if (deviceContext != nullptr)
                {
                    if (palette != nullptr)
                    {
                        const HPALETTE previousPalette = SelectPalette(deviceContext, palette, FALSE);
                        RealizePalette(deviceContext);
                        SelectPalette(deviceContext, previousPalette, FALSE);
                    }

                    ReleaseDC(windowHandle, deviceContext);
                }
            }
            else
            {
                ClipCursor(nullptr);
            }

            CApplication* application = CApplication::sTheObjectPtr;

            if (application != nullptr && application->applicationActivationCallback != nullptr)
            {
                application->applicationActivationCallback();
            }

            return 0;
        }

        case WM_SETCURSOR:
            if (NXSysMouseManager::CMouseManager::sTheObjectPtr != nullptr)
            {
                NXSysMouseManager::CMouseManager::sTheObjectPtr->ResetWindowsMouseState();
            }

            return 0;

        case WM_SYSKEYUP:
            if (wParam == VK_RETURN && windowHandle == mWindowHandle)
            {
                CApplication* application = CApplication::sTheObjectPtr;

                if (application != nullptr && application->fullscreenToggle)
                {
                    application->ToggleFullscreen();
                    return 0;
                }
            }

            return -1;

        case WM_QUERYNEWPALETTE:
        case WM_PALETTECHANGED:
        {
            if (message == WM_PALETTECHANGED && reinterpret_cast<HWND>(wParam) == windowHandle)
            {
                return 0;
            }

            if (mDisplay == nullptr || mDisplay->GetWindowsPalette() == nullptr)
            {
                return 0;
            }

            const HDC deviceContext = GetDC(windowHandle);

            if (deviceContext == nullptr)
            {
                return 0;
            }

            const HPALETTE previousPalette = SelectPalette(deviceContext, mDisplay->GetWindowsPalette(), FALSE);
            const UINT changedColors = RealizePalette(deviceContext);

            SelectPalette(deviceContext, previousPalette, FALSE);
            ReleaseDC(windowHandle, deviceContext);

            if (changedColors > 0)
            {
                InvalidateRect(windowHandle, nullptr, TRUE);
                return -1;
            }

            return 0;
        }

        default:
            return -1;
        }
    }

    // Original function: FUN_0047b868
    CWindow::~CWindow()
    {
        delete mDisplay;
        mDisplay = nullptr;
    }

    // Original function: FUN_0047b888
    void CWindow::Update()
    {
        if (mDisplay != nullptr)
        {
            mDisplay->Update();
        }
    }

    // Original function: FUN_0047b895
    void CWindow::FlipToGDISurface()
    {
        if (mDisplay != nullptr)
        {
            mDisplay->FlipToGDISurface();
        }
    }

    // Original function: FUN_0047b8a2
    void CWindow::ToggleFullscreen()
    {
        if (mDisplay == nullptr)
        {
            return;
        }

        if (!mDisplay->ToggleFullscreen())
        {
            OutputDebugStringA("NXSys2DApp::CWindow::ToggleFullscreen failed.\r\n");
            PostMessageA(mWindowHandle, WM_CLOSE, 0, 0);
        }

        if (NXSysMouseManager::CMouseManager::sTheObjectPtr != nullptr)
        {
            NXSysMouseManager::CMouseManager::sTheObjectPtr->ResetWindowsMouseState();
        }
    }
}
