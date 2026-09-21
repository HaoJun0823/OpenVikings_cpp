#include "StringInputHandler.h"
#include "XCrtFileAccess.h"

#include <cstring>

namespace NC2InGameGuiBaseTextInputElement
{
    // Original function: FUN_004a3a8e
    void CStringInputHandler::Init(std::uint32_t maximumLength, bool continuousEditing, bool startEditing, std::uint32_t activationKey, std::uint8_t activationCharacter)
    {
        NXBasics::Runtime_Free(mText);

        mContinuousEditing = static_cast<std::uint8_t>(continuousEditing);
        mActivationKey = activationKey;
        mActivationCharacter = activationCharacter;
        mMaximumLength = maximumLength;
        mText = nullptr;
        mActive = 1;

        mText = static_cast<char*>(NXBasics::Runtime_AllocateWithNewHandler(maximumLength + 2u, 1));

        if ((startEditing || continuousEditing) && mActive != 0)
        {
            mEditing = 1;
            mCursorPosition = 0;
            mText[0] = '\0';
        }
    }

    // Original function: FUN_004a3ae8
    void CStringInputHandler::Exit()
    {
        NXBasics::Runtime_Free(mText);

        mText = nullptr;
        mActive = 0;
    }

    // Original function: FUN_004a3afd
    void CStringInputHandler::StartEditing()
    {
        if (mActive != 0)
        {
            mEditing = 1;
            mCursorPosition = 0;
            mText[0] = '\0';
        }
    }

    // Original function: FUN_004a3cf6
    void CStringInputHandler::SetString(const char* text)
    {
        if (mActive == 0)
        {
            return;
        }

        const auto length = static_cast<std::uint32_t>(std::strlen(text));

        if (length > mMaximumLength)
        {
            std::strncpy(mText, text, mMaximumLength);
            mText[mMaximumLength] = '\0';
            mCursorPosition = mMaximumLength;
        }
        else
        {
            std::strcpy(mText, text);
            mCursorPosition = length;
        }
    }

    // Original function: FUN_004dffa4
    CStringInputHandler::CStringInputHandler()
    {
        std::memset(this, 0, sizeof(*this));
    }
}