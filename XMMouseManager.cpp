#include "X2DApplication.h"
#include "XMMouseManager.h"
#include "XMDirectInputMouse.h"
#include "XSystemMouseCursor.h"

#include <Windows.h>

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include <new>

namespace NXSysMouseManager
{
    // Original function: FUN_00403014.
    bool CMouseManager::IsAnyButtonDown() const { return mLeftButton || mMiddleButton || mRightButton; }

    // Original function: FUN_004792ec
    CMouseManager::CMouseManager()
    {
        sTheObjectPtr = this;
        std::memset(this, 0, sizeof(*this));

        new (std::nothrow) CDirectInputMouse();

        mHideSystemMouseCursorWhenFullscreen = false;
        ResetWindowsMouseState();
    }

    // Original function: FUN_00479323
    CMouseManager::~CMouseManager()
    {
        if (CDirectInputMouse::sTheObjectPtr != nullptr)
        {
            delete CDirectInputMouse::sTheObjectPtr;
        }

        ClipCursor(nullptr);

        NXSysMisc::XWS_SystemMouseCursor_SetNormal();
        NXSysMisc::XWS_SystemMouseCursor_Show();

        sTheObjectPtr = nullptr;
    }

    // Original function: FUN_00479357
    void CMouseManager::UpdateMouseState()
    {
        l_UpdateMouseState();

        NXSys2DApp::CApplication* application = NXSys2DApp::CApplication::sTheObjectPtr;

        if (application != nullptr && application->IsApplicationActive())
        {
            l_UpdateMouseButtonState();
            return;
        }

        l_SetState_Total(false, false, false, mMouseX, mMouseY, 0, 0, 0);
    }

    // Original function: FUN_0047938d
    void CMouseManager::UpdateMouseWheelState(int wheelDelta)
    {
        l_SetState_Wheel(wheelDelta);
        l_UpdateMouseButtonState();
    }

    // Original function: FUN_004793a4
    void CMouseManager::UpdateMouseButtonState(bool leftButton, bool middleButton, bool rightButton)
    {
        l_SetState_Total(leftButton, middleButton, rightButton, mMouseX, mMouseY, 0, 0, 0);
    }

    // Original function: FUN_004793c2
    void CMouseManager::l_UpdateMouseState()
    {
        POINT cursorPosition{};

        if (!GetCursorPos(&cursorPosition))
        {
            return;
        }

        NXSys2DApp::CApplication* application = NXSys2DApp::CApplication::sTheObjectPtr;

        if (application == nullptr)
        {
            return;
        }

        const int mouseX = cursorPosition.x - application->GetWindowOffsetX();
        const int mouseY = cursorPosition.y - application->GetWindowOffsetY();

        l_SetState_Position(mouseX, mouseY);

        CDirectInputMouse* directInputMouse = CDirectInputMouse::sTheObjectPtr;

        if (directInputMouse != nullptr && directInputMouse->IsInitialised())
        {
            int deltaX = 0;
            int deltaY = 0;

            directInputMouse->GetMovement(deltaX, deltaY);

            if (mPositionCallback != nullptr && mPositionCallback())
            {
                const int fixedCursorX = mMouseX + application->GetWindowOffsetX();
                const int fixedCursorY = mMouseY + application->GetWindowOffsetY();

                SetCursorPos(fixedCursorX, fixedCursorY);

                RECT cursorRectangle{ fixedCursorX,	fixedCursorY, fixedCursorX + 1, fixedCursorY + 1 };

                ClipCursor(&cursorRectangle);

                mDirectInputCursorLocked = true;

                l_SetState_Total(mLeftButton, mMiddleButton, mRightButton, mMouseX, mMouseY, deltaX, deltaY, 0);
            }
            else if (mDirectInputCursorLocked)
            {
                l_UpdateWindowsMouseState();
                mDirectInputCursorLocked = false;
            }
        }
        else if (mPositionCallback != nullptr && mPositionCallback())
        {
            SetCursorPos(mMouseX + application->GetWindowOffsetX(), mMouseY + application->GetWindowOffsetY());
        }
    }

    // Original function: FUN_004794eb
    void CMouseManager::l_UpdateMouseButtonState()
    {
        const bool leftButton = (GetKeyState(VK_LBUTTON) & 0x8000) != 0;
        const bool middleButton = (GetKeyState(VK_MBUTTON) & 0x8000) != 0;
        const bool rightButton = (GetKeyState(VK_RBUTTON) & 0x8000) != 0;

        l_SetState_Total(leftButton, middleButton, rightButton, mMouseX, mMouseY, 0, 0, 0);
    }

    // Original function: FUN_0047953d
    void CMouseManager::ResetWindowsMouseState()
    {
        l_UpdateWindowsMouseState();

        NXSys2DApp::CApplication* application = NXSys2DApp::CApplication::sTheObjectPtr;

        if (application->IsFullscreen() && mHideSystemMouseCursorWhenFullscreen)
        {
            NXSysMisc::XWS_SystemMouseCursor_Hide();
        }
        else
        {
            NXSysMisc::XWS_SystemMouseCursor_Show();
        }

        NXSysMisc::XWS_SystemMouseCursor_Reset();
    }

    // Original function: FUN_0047956d
    void CMouseManager::l_UpdateWindowsMouseState()
    {
        NXSys2DApp::CApplication* application = NXSys2DApp::CApplication::sTheObjectPtr;

        if (mPositionCallback != nullptr && mPositionCallback())
        {
            const int cursorX = mMouseX + application->GetWindowOffsetX();
            const int cursorY = mMouseY + application->GetWindowOffsetY();

            SetCursorPos(cursorX, cursorY);

            RECT cursorRectangle{ cursorX, cursorY, cursorX + 1, cursorY + 1 };
            ClipCursor(&cursorRectangle);
            return;
        }

        if (mMouseAreaEnabled)
        {
            const int windowOffsetX = application->GetWindowOffsetX();
            const int windowOffsetY = application->GetWindowOffsetY();

            RECT mouseArea{};
            mouseArea.left = mMouseAreaX + windowOffsetX;
            mouseArea.top = mMouseAreaY + windowOffsetY;
            mouseArea.right = mMouseAreaX + mMouseAreaWidth + windowOffsetX - 1;
            mouseArea.bottom = mMouseAreaY + mMouseAreaHeight + windowOffsetY - 1;

            ClipCursor(&mouseArea);
            return;
        }

        if (application->IsFullscreen())
        {
            const int windowOffsetX = application->GetWindowOffsetX();
            const int windowOffsetY = application->GetWindowOffsetY();

            RECT displayArea{};
            displayArea.left = windowOffsetX;
            displayArea.top = windowOffsetY;
            displayArea.right = application->GetDisplayWidth() + windowOffsetX;
            displayArea.bottom = application->GetDisplayHeight() + windowOffsetY;

            ClipCursor(&displayArea);
            return;
        }

        ClipCursor(nullptr);
    }

    // Original function: FUN_0047969a
    void CMouseManager::Callback_WindowActivationChanged(bool active)
    {
        if (!active)
        {
            return;
        }

        CDirectInputMouse* directInputMouse = CDirectInputMouse::sTheObjectPtr;

        if (directInputMouse != nullptr)
        {
            directInputMouse->Restart();
        }
    }

    // Original function: FUN_004796af
    void CMouseManager::Callback_FullscreenToggled(bool fullscreen)
    {
        (void)fullscreen;

        l_UpdateWindowsMouseState();
    }

    // Original function: FUN_004796b7
    void CMouseManager::l_SetState_Position(int mouseX, int mouseY)
    {
        l_SetState_Total(mLeftButton, mMiddleButton, mRightButton, mouseX, mouseY, mouseX - mMouseX, mouseY - mMouseY, 0);
    }

    // Original function: FUN_004796ea
    void CMouseManager::l_SetState_Wheel(int wheelDelta)
    {
        l_SetState_Total(mLeftButton, mMiddleButton, mRightButton, mMouseX, mMouseY, 0, 0, wheelDelta);
    }

    // Original function: FUN_00479711
    void CMouseManager::l_SetState_Total(bool leftButton, bool middleButton, bool rightButton, int mouseX, int mouseY, int deltaX, int deltaY, int wheelDelta)
    {
        mAccumulatedDeltaX += deltaX;
        mAccumulatedDeltaY += deltaY;
        mAccumulatedWheelDelta += wheelDelta;

        bool newLeftButtonPress = false;

        if (mLeftButton != leftButton)
        {
            if (leftButton)
            {
                if (!mLeftButton)
                {
                    newLeftButtonPress = true;
                }

                mLeftButtonPressed = true;
            }
            else
            {
                mLeftButtonReleased = true;
            }

            mLeftButton = leftButton;
            mStateChanged = true;
            mLastStateChangeTime = timeGetTime();
        }

        if (mMiddleButton != middleButton)
        {
            if (middleButton)
            {
                mMiddleButtonPressed = true;
            }
            else
            {
                mMiddleButtonReleased = true;
            }

            mMiddleButton = middleButton;
            mStateChanged = true;
            mLastStateChangeTime = timeGetTime();
        }

        if (mRightButton != rightButton)
        {
            if (rightButton)
            {
                mRightButtonPressed = true;
            }
            else
            {
                mRightButtonReleased = true;
            }

            mRightButton = rightButton;
            mStateChanged = true;
            mLastStateChangeTime = timeGetTime();
        }

        if (mPositionCallback != nullptr && mPositionCallback())
        {
            if (deltaX != 0 || deltaY != 0)
            {
                mPositionChanged = true;
                mStateChanged = true;
                mLastStateChangeTime = timeGetTime();
            }
        }
        else if (mMouseX != mouseX || mMouseY != mouseY)
        {
            mPositionChanged = true;
            mMouseX = mouseX;
            mMouseY = mouseY;

            if (mMouseAreaEnabled)
            {
                mMouseX = (std::max)(mMouseX, mMouseAreaX);
                mMouseY = (std::max)(mMouseY, mMouseAreaY);
                mMouseX = (std::min)(mMouseX, mMouseAreaX + mMouseAreaWidth - 1);
                mMouseY = (std::min)(mMouseY, mMouseAreaY + mMouseAreaHeight - 1);
            }

            mStateChanged = true;
            mLastStateChangeTime = timeGetTime();
        }

        if (mPositionChanged && mPreviousLeftButtonClickTime != 0)
        {
            if (std::abs(mPreviousLeftButtonClickX - mMouseX) > 4 || std::abs(mPreviousLeftButtonClickY - mMouseY) > 4)
            {
                mPreviousLeftButtonClickTime = 0;
            }
        }

        if (newLeftButtonPress)
        {
            const std::uint32_t currentTime = timeGetTime();

            if (currentTime - mPreviousLeftButtonClickTime < 400)
            {
                mPreviousLeftButtonClickTime = 0;
                mLeftButtonDoubleClicked = true;
                mStateChanged = true;
                mLastStateChangeTime = timeGetTime();
            }
            else
            {
                mPreviousLeftButtonClickTime = currentTime;
                mPreviousLeftButtonClickX = mMouseX;
                mPreviousLeftButtonClickY = mMouseY;
            }
        }

        if (wheelDelta != 0)
        {
            mStateChanged = true;
            mLastStateChangeTime = timeGetTime();
        }
    }

    // Original function: FUN_00479880
    void CMouseManager::GetNewMouseMessage(SMouseMessage& mouseMessage)
    {
        mouseMessage = {};

        if (!mStateChanged)
        {
            return;
        }

        mStateChanged = false;

        mouseMessage.mouseX = mMouseX;
        mouseMessage.mouseY = mMouseY;
        mouseMessage.deltaX = mAccumulatedDeltaX;
        mouseMessage.deltaY = mAccumulatedDeltaY;
        mouseMessage.wheelDelta = mAccumulatedWheelDelta;

        mAccumulatedDeltaX = 0;
        mAccumulatedDeltaY = 0;
        mAccumulatedWheelDelta = 0;

        if (mLeftButtonReleased)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::LeftButtonReleased);

            mLeftButtonReleased = false;
        }

        if (mLeftButtonPressed)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::LeftButtonPressed);

            mLeftButtonPressed = false;
        }

        if (mMiddleButtonReleased)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::MiddleButtonReleased);

            mMiddleButtonReleased = false;
        }

        if (mMiddleButtonPressed)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::MiddleButtonPressed);

            mMiddleButtonPressed = false;
        }

        if (mRightButtonReleased)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::RightButtonReleased);

            mRightButtonReleased = false;
        }

        if (mRightButtonPressed)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::RightButtonPressed);

            mRightButtonPressed = false;
        }

        if (mLeftButtonDoubleClicked)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::LeftButtonDoubleClicked);

            mLeftButtonDoubleClicked = false;
        }

        if (mPositionChanged)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::PositionChanged);

            mPositionChanged = false;
        }

        if (mouseMessage.wheelDelta != 0)
        {
            mouseMessage.flags |= static_cast<std::uint32_t>(EMouseMessageFlags::WheelChanged);
        }
    }

    // Original function: FUN_00479923
    void CMouseManager::SetHideSystemMouseCursorWhenFullscreenState(bool hideSystemMouseCursor)
    {
        mHideSystemMouseCursorWhenFullscreen = hideSystemMouseCursor;

        if (hideSystemMouseCursor)
        {
            NXSysMisc::XWS_SystemMouseCursor_Hide();
        }
        else
        {
            NXSysMisc::XWS_SystemMouseCursor_Show();
        }
    }
}
