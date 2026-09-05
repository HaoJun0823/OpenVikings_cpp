#include "MasterControlProgram.h"

#include <cstring>
#include <cstdint>
#include <new>

struct SReceivedNetworkRecord
{
	std::byte unknown00[4];
	std::uint16_t type;
	char text[0xFE];
};

static_assert(sizeof(SReceivedNetworkRecord) == 0x104);


struct SQueuedNetworkMessage
{
	std::uint32_t playerId;
	wchar_t playerName[256];
	std::uint32_t unknown204;
	std::uint32_t type;
	SQueuedNetworkMessage* next;
};

static_assert(sizeof(SQueuedNetworkMessage) == 0x210);
static_assert(offsetof(SQueuedNetworkMessage, type) == 0x208);
static_assert(offsetof(SQueuedNetworkMessage, next) == 0x20C);


std::uint32_t GetNetworkMessageStringId(std::uint32_t messageType)
{
	switch (messageType)
	{
	case 1:
		return 0x59;

	case 2:
		return 0x58;

	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
		return 0x57;

	case 8:
		return 0x5A;

	case 9:
		return 0x5B;

	case 10:
		return 0x5C;

	case 11:
		return 0x5D;

	case 12:
		return 0x5E;

	case 13:
		return 0x5F;

	default:
		return 0;
	}
}


namespace NMasterControlProgram
{
	// Original function: FUN_00401c1d
	bool CMasterControlProgram::System_ProgramState_Game_StartUp()
	{
		mProgramState = 10;

		FUN_00402530(this, false);

		void* guiManagerMemory = ::operator new(0x1F0, std::nothrow);

		if (guiManagerMemory != nullptr)
		{
			FUN_0049fd79(guiManagerMemory);
		}

		FUN_00401985(this);

		FUN_004a0205(DAT_005690cc);

		const int displayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

		const int displayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();

		const int displayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();

		FUN_004a030c(DAT_005690cc, displayWidth, displayHeight, displayDepth);

		FUN_004a2653(DAT_005690cc);

		auto* guiManagerBytes = reinterpret_cast<const std::uint8_t*>(DAT_005690cc);

		if (guiManagerBytes[0xE5] != 0)
		{
			auto* message = FUN_004e1e35(0x56);

			FUN_004a25e0(DAT_005690cc, message);
		}

		FUN_004019da(this);
		FUN_00402620(this);

		mProgramState = static_cast<std::int32_t>(TProgramState::Game);

		return true;
	}

	// Original function: FUN_00401d38
	void CMasterControlProgram::System_ProgramState_Game_Update()
	{
		if (DAT_0050f6a8 != 0)
		{
			if (DAT_0050f6cc > 0)
			{
				SReceivedNetworkRecord* receivedRecords = nullptr;
				std::uint32_t receivedRecordCount = 0;

				FUN_004dec60(&DAT_0050f6a8, reinterpret_cast<void**>(&receivedRecords), &receivedRecordCount);

				for (std::uint32_t index = 0; index < receivedRecordCount; ++index)
				{
					const SReceivedNetworkRecord& record = receivedRecords[index];

					char receivedText[128]{};
					FUN_004db7cc(receivedText, record.text, -1);

					switch (record.type)
					{
					case 1:
						FUN_004a25e0(DAT_005690cc, receivedText);
						FUN_004e2bcf();
						break;

					case 2:
					{
						const int senderPlayerId = FUN_004e5fb7(receivedText);

						int playerIndex = -1;
						int validationValue = 0;
						char* commandText = nullptr;

						char* firstSeparator = std::strchr(receivedText, ':');

						if (firstSeparator != nullptr)
						{
							++firstSeparator;
							playerIndex = FUN_004e5fb7(firstSeparator);

							char* secondSeparator = std::strchr(firstSeparator, ':');

							if (secondSeparator != nullptr)
							{
								++secondSeparator;
								validationValue = FUN_004e5fb7(secondSeparator);

								char* thirdSeparator = std::strchr(secondSeparator, ':');

								if (thirdSeparator != nullptr)
								{
									commandText = thirdSeparator + 1;
								}
							}
						}

						auto* playerStateBytes = reinterpret_cast<std::byte*>(DAT_00510a00) + playerIndex * 0xC58;
						const bool playerStateActive = playerStateBytes[0] != std::byte{ 0 };
						const int storedValidationValue = *reinterpret_cast<const int*>(playerStateBytes + 0x08);
						const int localPlayerId = *reinterpret_cast<const int*>(DAT_005690cc);

						if (playerStateActive &&
							storedValidationValue == validationValue &&
							senderPlayerId != localPlayerId &&
							commandText != nullptr)
						{
							auto* playerObject = reinterpret_cast<std::byte*>(DAT_005550d8) + playerIndex * 0x8C8;
							FUN_004806c5(playerObject, commandText);
						}

						break;
					}
					}
				}

				if (receivedRecords != nullptr)
				{
					FUN_004e5e48(receivedRecords);
				}
			}
		}

		alignas(4) std::byte messageCollection[0x10]{};
		FUN_004db43a(messageCollection, &DAT_0050f6bc);

		auto* currentMessage = *reinterpret_cast<SQueuedNetworkMessage**>(messageCollection + 0x08);

		while (currentMessage != nullptr)
		{
			switch (currentMessage->type)
			{
			case 1:
				System_ProgramState_ChangeNextTimePossible(TProgramState::MainMenu, false);
				break;

			case 3:
			case 4:
			case 5:
			case 6:
			case 7:
			{
				const std::uint32_t playerId = currentMessage->playerId;
				const std::uint32_t currentPlayerId = FUN_004059cb();

				if (playerId == currentPlayerId || currentPlayerId == 0)
				{
					System_ProgramState_ChangeNextTimePossible(TProgramState::MainMenu, false);
				}
				else
				{
					FUN_0040a4fd(DAT_005109ec, playerId, true);
					FUN_0040b246(false);
				}

				break;
			}

			case 9:
			case 13:
				FUN_0040b246(true);
				break;
			}

			char playerName[100]{};

			if (currentMessage->playerName[0] != L'\0')
			{
				FUN_004db833(playerName, currentMessage->playerName, -1);
			}
			else
			{
				FUN_004e5d8b(playerName, "?");
			}

			char outputText[328]{};
			const std::uint32_t stringId = GetNetworkMessageStringId(currentMessage->type);

			if (stringId != 0)
			{
				const char* messageText = FUN_004e1e35(stringId);
				FUN_004e5d8b(outputText, ">> %s %s!", playerName, messageText);
			}
			else
			{
				char networkEventText[200]{};
				FUN_004db635(currentMessage, networkEventText, sizeof(networkEventText));
				FUN_004e5d8b(outputText, ">> NET: '%s'", networkEventText);
			}

			if (outputText[0] != '\0')
			{
				FUN_004a25e0(DAT_005690cc, outputText);
			}

			char diagnosticText[200]{};
			FUN_004db635(currentMessage, diagnosticText, sizeof(diagnosticText));

			currentMessage = currentMessage->next;
		}

		thunk_FUN_004db528(messageCollection);

		if (FUN_004e166b(DAT_005697ec, "start_fmv"))
		{
			System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
		}

		thunk_FUN_004077b8();
		FUN_004a0184(DAT_005690cc);

		const auto* desktopBytes = reinterpret_cast<const std::byte*>(DAT_00554d5c);
		const std::uint32_t displayBitmap = *reinterpret_cast<const std::uint32_t*>(desktopBytes + 0x10);

		FUN_00479052(DAT_00554d74, displayBitmap, nullptr, 0);
	}

	// Original function: FUN_00401ccb
	void CMasterControlProgram::System_ProgramState_Game_ShutDown()
	{
		mProgramState = 13;

		FUN_004800c8(DAT_005550d4);

		if (DAT_0050f698 != nullptr)
		{
			FUN_004038bd(DAT_0050f698, 0, 0, 1);
		}

		FUN_00405949();
		FUN_004a0aaf(DAT_005690cc);
		FUN_004a0295(DAT_005690cc);
		FUN_00401bd5(this);

		auto* inGameGuiManager = DAT_005690cc;

		if (inGameGuiManager != nullptr)
		{
			FUN_004a00c1(inGameGuiManager);
			FUN_004e5e48(inGameGuiManager);
		}

		mProgramState = static_cast<std::int32_t>(TProgramState::None);
	}
}