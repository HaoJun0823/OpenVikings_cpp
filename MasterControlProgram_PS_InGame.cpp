#include "X2DApplication.h"
#include "MasterPropertyManager.h"
#include "XCrtFileAccess.h"
#include "MasterControlProgram.h"
#include "InGameGuiManager.h"
#include "NetworkShutdown.h"

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

		ProgressBar_Init(false);

		new (std::nothrow) NInGameGuiManager::CInGameGuiManager();

		FUN_00401985(this);

		FUN_004a0205(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

		const int displayDepth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayDepth();

		const int displayHeight = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayHeight();

		const int displayWidth = NXSys2DApp::CApplication::sTheObjectPtr->GetDisplayWidth();

		FUN_004a030c(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, displayWidth, displayHeight, displayDepth);

		FUN_004a2653(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

		if (NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->AreCheatsAllowed())
		{
			auto* message = FUN_004e1e35(0x56);

			FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, message);
		}

		FUN_004019da(this);
		ProgressBar_Exit();

		mProgramState = static_cast<std::int32_t>(TProgramState::Game);

		return true;
	}

	// Original function: FUN_00401d38
	void CMasterControlProgram::System_ProgramState_Game_Update()
	{
		if (DAT_0050f6a8.active != 0)
		{
			if (DAT_0050f6a8.receivedRecordCount > 0)
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
						FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, receivedText);
						FUN_004e2bcf();
						break;

					case 2:
					{
						const int senderPlayerId = NXBasics::Runtime_StringToInteger(receivedText);

						int playerIndex = -1;
						int validationValue = 0;
						char* commandText = nullptr;

						char* firstSeparator = std::strchr(receivedText, ':');

						if (firstSeparator != nullptr)
						{
							++firstSeparator;
							playerIndex = NXBasics::Runtime_StringToInteger(firstSeparator);

							char* secondSeparator = std::strchr(firstSeparator, ':');

							if (secondSeparator != nullptr)
							{
								++secondSeparator;
								validationValue = NXBasics::Runtime_StringToInteger(secondSeparator);

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
						const int localPlayerId = NInGameGuiManager::CInGameGuiManager::sTheObjectPtr->GetLocalPlayerId();

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
					::operator delete(static_cast<void*>(receivedRecords));
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
				FUN_004a25e0(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr, outputText);
			}

			char diagnosticText[200]{};
			FUN_004db635(currentMessage, diagnosticText, sizeof(diagnosticText));

			currentMessage = currentMessage->next;
		}

		NNetwork::ClearEventQueue(reinterpret_cast<NNetwork::SEventQueue*>(messageCollection));

		if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_DoesExists("start_fmv"))
		{
			System_ProgramState_ChangeNextTimePossible(TProgramState::TitleScreen, false);
		}

		thunk_FUN_004077b8();
		FUN_004a0184(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);

		const auto* desktopBytes = reinterpret_cast<const std::byte*>(DAT_00554d5c);
		const std::uint32_t displayBitmap = *reinterpret_cast<const std::uint32_t*>(desktopBytes + 0x10);

		FUN_00479052(NXSys2DApp::CApplication::sTheObjectPtr, displayBitmap, nullptr, 0);
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

		NNetwork::Network_ShutDown();
		FUN_004a0aaf(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
		FUN_004a0295(NInGameGuiManager::CInGameGuiManager::sTheObjectPtr);
		FUN_00401bd5(this);

		delete NInGameGuiManager::CInGameGuiManager::sTheObjectPtr;

		mProgramState = static_cast<std::int32_t>(TProgramState::None);
	}
}