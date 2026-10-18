#include "StringInputHandler.h"
#include "SwitchIdentifiers.h"
#include "XCrtFileAccess.h"
#include "XFontMetrics.h"

#include <cstdint>
#include <cstring>

// Original data: 00507468..00507497; additional accepted characters.
const char kAdditionalAcceptedCharacters[] =
"\xE4\xC4\xF6\xD6\xFC\xDC\xDF\x21\xA7\x24\x26\x28\x29\x3D\x2B\x2D"
"\x5F\x20\x25\x2F\x3F\xB4\x60\x23\x2A\x27\x2C\x3B\x2E\x3A\x40\x3C"
"\x3E\x7C\x5C\x7E\x5E\x5E\xB0\xE7\xE2\xE1\xE0\xF4\xE9\xE8\xEA";

static_assert(sizeof(kAdditionalAcceptedCharacters) == 48);

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

	// Original function: FUN_004a3b10
	std::uint32_t CStringInputHandler::WorkOnKey(std::uint32_t key, char character)
	{
		if (mActive == 0)
		{
			return 0;
		}

		const auto characterByte = static_cast<std::uint8_t>(character);

		if (mEditing == 0)
		{
			const bool activationKeyMatches = mActivationKey != 0 && key == mActivationKey;
			const bool activationCharacterMatches = mActivationCharacter != 0 && characterByte == mActivationCharacter;

			if (!activationKeyMatches && !activationCharacterMatches)
			{
				return 1;
			}

			mCursorPosition = 0;
			mEditing = 1;
			mText[0] = '\0';

			return 2;
		}

		const auto length = static_cast<std::uint32_t>(std::strlen(mText));

		switch (key)
		{
			case SwitchIds::VirtualKey::Backspace:
				{
					// Backspace.
					if (length != 0 && mCursorPosition != 0)
					{
						const std::uint32_t moveBytes = length - mCursorPosition + 1u;

						std::memmove(mText + mCursorPosition - 1u, mText + mCursorPosition, moveBytes);
						--mCursorPosition;
					}

					return 2;
				}

			case SwitchIds::VirtualKey::Return:
				{
					// Return.
					if (mContinuousEditing == 0)
					{
						mEditing = 0;
					}

					return 3;
				}

			case SwitchIds::VirtualKey::Escape:
				{
					// Escape does not reset the cursor.
					if (mContinuousEditing != 0)
					{
						mText[0] = '\0';
					}
					else
					{
						mEditing = 0;
					}

					return 4;
				}

			case SwitchIds::VirtualKey::End:
				// End.
				mCursorPosition = length;
				return 2;

			case SwitchIds::VirtualKey::Home:
				// Home.
				mCursorPosition = 0;
				return 2;

			case SwitchIds::VirtualKey::Left:
				// Left.
				if (length != 0 && mCursorPosition != 0)
				{
					--mCursorPosition;
				}

				return 2;

			case SwitchIds::VirtualKey::Right:
				// Right.
				if (mCursorPosition < length)
				{
					++mCursorPosition;
				}

				return 2;

			case SwitchIds::VirtualKey::Delete:
				{
					// Delete preserves the original lack of a cursor-range check.
					if (length != 0)
					{
						const std::uint32_t moveBytes = length - mCursorPosition;

						std::memmove(mText + mCursorPosition, mText + mCursorPosition + 1u, moveBytes);
					}

					return 2;
				}

			default:
				break;
		}

		// Remaster: a zero-capacity input accepts no inserted characters.
		if (length >= mMaximumLength)
		{
			return 2;
		}

		if (mCursorPosition > length)
		{
			mCursorPosition = length;
		}

		if (characterByte == 0)
		{
			return 2;
		}

		if (!NXBasics::Runtime_IsLetterOrNumber(character))
		{
			const std::int32_t signedCharacter = characterByte < 0x80u ? static_cast<std::int32_t>(characterByte) : static_cast<std::int32_t>(characterByte) - 0x100;

			if (std::strchr(kAdditionalAcceptedCharacters, signedCharacter) == nullptr)
			{
				return 2;
			}
		}

		if (mCursorPosition == length)
		{
			mText[length] = character;
			mText[length + 1u] = '\0';
		}
		else
		{
			const std::uint32_t moveBytes = length - mCursorPosition + 1u;

			std::memmove(mText + mCursorPosition + 1u, mText + mCursorPosition, moveBytes);
			mText[mCursorPosition] = character;
		}

		++mCursorPosition;
		return 2;
	}

	// Original function: FUN_004a3cc2
	std::uint32_t CStringInputHandler::GetCursorPixelPosition(const NXBasics::CFont* font)
	{
		if (mActive == 0)
		{
			return 0;
		}

		const char savedCharacter = mText[mCursorPosition];
		mText[mCursorPosition] = '\0';

		const std::uint32_t width = static_cast<std::uint32_t>(NXBasics::Font_GetPixelWidth(font, mText));

		mText[mCursorPosition] = savedCharacter;

		return width;
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
