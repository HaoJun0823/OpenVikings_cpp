#include "X2DApplication.h"
#include "XMMouseManager.h"
#include "XMKeyManager.h"

#include <Windows.h>

#include <cstddef>
#include <cstdint>

namespace NXSys2DApp
{
    // Original function: FUN_004799d8
    bool CApplication::DoMessages()
    {
        MSG message{};

        // Keep queue order for raw movement, buttons and wheel, with room for high-rate mice.
        // The finite budget still guarantees that the game receives an update.
        for (std::uint32_t processedMessageCount = 0; processedMessageCount < 1024; ++processedMessageCount)
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

    // Original function: FUN_00479a61
    LRESULT CApplication::HandleWindowMessage(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        // Raw Input cleanup must run even when the application callback consumes other messages.
        if (message == WM_INPUT)
        {
            auto* mouseManager = NXSysMouseManager::CMouseManager::sTheObjectPtr;
            if (mouseManager != nullptr) mouseManager->HandleRawMouseInput(reinterpret_cast<HRAWINPUT>(lParam));
            return DefWindowProcA(windowHandle, message, wParam, lParam);
        }

        if (windowMessageCallback != nullptr)
        {
            if (windowMessageCallback(windowHandle, message, wParam, lParam) == 0)
            {
                return 0;
            }
        }

        switch (message)
        {
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            auto* keyManager = NXSysKeyManager::CKeyManager::sTheObjectPtr;

            if (keyManager != nullptr)
            {
                keyManager->PushKey(static_cast<std::uint8_t>(wParam), 0, message == WM_KEYDOWN);
            }

            return 0;
        }

        case WM_CHAR:
        {
            auto* keyManager = NXSysKeyManager::CKeyManager::sTheObjectPtr;

            if (keyManager != nullptr)
            {
                keyManager->PushKey(0, static_cast<std::uint8_t>(wParam), true);
            }

            return 0;
        }

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        {
            // Only F10 is intercepted by this branch.
            if (wParam != VK_F10)
            {
                break;
            }

            auto* keyManager = NXSysKeyManager::CKeyManager::sTheObjectPtr;

            if (keyManager != nullptr)
            {
                keyManager->PushKey(static_cast<std::uint8_t>(VK_F10), 0, message == WM_SYSKEYDOWN);
            }

            return 0;
        }

        case WM_MOUSEMOVE:
        case WM_LBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_MBUTTONDOWN:
        case WM_MBUTTONUP:
        {
            auto* mouseManager = NXSysMouseManager::CMouseManager::sTheObjectPtr;

            if (mouseManager != nullptr)
            {
                const bool leftButton = (wParam & MK_LBUTTON) != 0;
                const bool middleButton = (wParam & MK_MBUTTON) != 0;
                const bool rightButton = (wParam & MK_RBUTTON) != 0;

                mouseManager->UpdateMouseButtonState(leftButton, middleButton, rightButton);
            }

            return 0;
        }

        case WM_SYSCOMMAND:
            // The original compares the complete value without masking.
            if (wParam == SC_SCREENSAVE || wParam == SC_MONITORPOWER)
            {
                return 0;
            }

            break;

        default:
            break;
        }

        // The registered legacy wheel message uses the low word.
        if (message == mMouseWheelMessage)
        {
            auto* mouseManager = NXSysMouseManager::CMouseManager::sTheObjectPtr;

            if (mouseManager != nullptr)
            {
                const int wheelDelta = static_cast<std::int16_t>(LOWORD(wParam));
                mouseManager->UpdateMouseWheelState(wheelDelta);
            }

            return 0;
        }

        // WM_MOUSEWHEEL uses the high word.
        if (message == WM_MOUSEWHEEL)
        {
            auto* mouseManager = NXSysMouseManager::CMouseManager::sTheObjectPtr;

            if (mouseManager != nullptr)
            {
                const int wheelDelta = static_cast<std::int16_t>(HIWORD(wParam));
                mouseManager->UpdateMouseWheelState(wheelDelta);
            }

            return 0;
        }

        if (mWindow != nullptr && GetWindowHandle() == windowHandle)
        {
            const LRESULT result = mWindow->HandleWindowMessage(windowHandle, message, wParam, lParam);

            // Only zero suppresses default processing.
            if (result == 0)
            {
                return 0;
            }
        }

        return DefWindowProcA(windowHandle, message, wParam, lParam);
    }
}
