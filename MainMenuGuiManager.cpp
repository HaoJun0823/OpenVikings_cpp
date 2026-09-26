#include <cstddef>
#include <cstdint>
#include <cstring>
#include <new>
#include <windows.h>

#include "dplay8.h"
#include "FileCompressor.h"
#include "FileTransferManager.h"
#include "GuiToolsBase.h"
#include "MainMenuGuiManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "NetworkShutdown.h"
#include "XBitmap.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "XFontMetrics.h"
#include "XIniFileReader.h"
#include "XMemory.h"
#include "XMKeyManager.h"
#include "XMMouseManager.h"

// Original data: DAT_004f5254.
// Keep unresolved until the Windows constant is recovered.
extern const float DAT_004f5254;

// Original data: DAT_00500818.
// Keep unresolved until the Windows format string is recovered.
extern const char DAT_00500818[];

// Original data: 004ff054; shared root-path string.
extern const char DAT_004ff054[];

// Original data: DAT_004fe238.
// Format used when a session event has no player name.
extern const char DAT_004fe238[];

// Original data: DAT_0050716c.
extern const char DAT_0050716c[];

// Original data: DAT_004f7e48.
// Service-provider GUID compared by the original code.
extern const GUID DAT_004f7e48;

// Original data: DAT_005694c0.
std::uint8_t DAT_005694c0 = 0;

// Original data: DAT_005694e0.
std::uint32_t DAT_005694e0 = 0;

// Original data: DAT_005694e4.
std::uint32_t DAT_005694e4 = 0;

namespace NC2MainMenuGuiManager
{

	// Original function: FUN_004d2bf2
	void CGuiManager::System_UpdateGui()
	{
		const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
			return value;
		};

		const auto writeWord = [] (void* object, std::size_t offset, std::uint32_t value)
		{
			std::memcpy(static_cast<std::byte*>(object) + offset, &value, sizeof(value));
		};

		const auto readPointer = [] (const void* object, std::size_t offset) -> void*
		{
			void* value;
			std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
			return value;
		};

		// Pending: l_ScreenGuiElement_FrameCall_Do.
		FUN_004d6b08(this);

		if (readWord(this, 0x58) == 0)
			writeWord(this, 0x58, timeGetTime());

		const std::uint32_t mouseActivity = readWord(NXSysMouseManager::CMouseManager::sTheObjectPtr, 0x24);
		const std::uint32_t keyboardActivity = readWord(NXSysKeyManager::CKeyManager::sTheObjectPtr, 4);
		const std::uint32_t latestInput = mouseActivity > keyboardActivity ? mouseActivity : keyboardActivity;
		const std::uint32_t previousActivity = readWord(this, 0x58);

		writeWord(this, 0x58, previousActivity > latestInput ? previousActivity : latestInput);

		// Preserve the original time-query sequence.
		timeGetTime();

		if (readWord(this, 0x60) == 0)
			writeWord(this, 0x60, timeGetTime());

		const std::uint32_t elapsedSeconds = (timeGetTime() - readWord(this, 0x60)) / 1000u;
		const std::uint32_t intervalSeconds = readWord(this, 0x64) != 0 ? 30u : 0u;

		if (elapsedSeconds > intervalSeconds)
		{
			writeWord(this, 0x60, timeGetTime());
			timeGetTime();

			// Storage covering the original CHumanIterator stack object.
			alignas(4) std::byte iterator[0x30];
			std::int32_t candidates[40];
			std::uint32_t candidateCount = 0;

			FUN_004402a3(iterator);

			std::int32_t humanId = FUN_0042c1d4(iterator);

			while (humanId != -1)
			{
				const std::uint32_t humanOffset = static_cast<std::uint32_t>(humanId) * 0xC58u;

				if (readWord(DAT_00510a00, humanOffset + 0xB80u) == 10000u)
				{
					candidates[candidateCount++] = humanId;

					if (candidateCount == 40u)
						break;
				}

				humanId = FUN_0042c392(iterator);
			}

			if (candidateCount != 0)
			{
				const std::int32_t selectedHuman = candidates[timeGetTime() % candidateCount];

				if (selectedHuman != -1)
					FUN_004dad1b(readPointer(this, 4), selectedHuman);
			}

			writeWord(this, 0x64, readWord(this, 0x64) + 1u);
		}

		l_Network_Update();

		if (readPointer(this, 0x30) != nullptr && !FUN_004d782f(this))
		{
			const std::uint32_t currentTime = timeGetTime();

			// Preserve the original unsigned comparison and addition.
			if (currentTime >= DAT_005694e0 + 500u)
			{
				DAT_005694e0 = currentTime;

				GetNetworkInstance();

				for (std::uint32_t index = 0; index < 10; ++index)
				{
					if (readPointer(this, 0x30 + index * 4u) != nullptr)
					{
						char text[0xA8];

						FUN_004d7db7(this, index, text);
						FUN_004b8fd0(readPointer(this, 0x30 + index * 4u), text);
					}
				}
			}
		}

		if (NNetwork::Network_IsHost() && currentScreen == 14)
		{
			const std::uint32_t currentTime = timeGetTime();

			if (currentTime >= DAT_005694e4 + 1000u)
			{
				DAT_005694e4 = currentTime;

				std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
				message[0] = 6;

				NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, 0, message, sizeof(message));
			}
		}

		// Pending: CDesktop::Desktop_DoWork.
		FUN_00477104(DAT_00554d5c, false, DAT_005694c0);
		DAT_005694c0 = 0;

		l_Network_Draw();

		const auto* objectBytes = reinterpret_cast<const std::byte*>(this);

		if (currentScreen == 1 && objectBytes[0x5C] == std::byte{ 0 })
		{
			char text[0x15C];

			// Preserve the original localization lookup order.
			const char* suffix = FUN_004e1e19(0x1B);
			const char* versionLabel = FUN_004e1e19(0x1A);
			const char* title = FUN_004e1e19(0x19);

			NXBasics::Runtime_PrintString(text, "%s - %s %d.%2.2d - %s Nov 29 2003 17:37:45", title, versionLabel, 1, 0, suffix);

			void* bitmap = readPointer(DAT_00554d5c, 0x10);
			auto* font = DAT_005698f8;
			const auto* colour = reinterpret_cast<const std::byte*>(DAT_00569908) + 0x400;

			const std::uint32_t textHeight = static_cast<std::uint32_t>(NXBasics::Font_GetPixelHeight(font, text));
			const std::uint32_t y = readWord(bitmap, 0x14) - textHeight - 1u;

			const std::uint32_t textWidth = static_cast<std::uint32_t>(NXBasics::Font_GetPixelWidth(font, text));
			const std::uint32_t x = readWord(bitmap, 0x10) - textWidth - 1u;

			reinterpret_cast<NXBasics::CBitmap*>(bitmap)->Text_Print(font, reinterpret_cast<const NXBasics::SColorRGB*>(colour), NXBasics::Runtime_TimeSigned32(x), NXBasics::Runtime_TimeSigned32(y), text);
		}
	}

	// Original function: FUN_004d31bc
	bool CGuiManager::StringInput_HandleKeys(std::uint32_t key, char character)
	{
		if (textInput.mEditing == 0)
			return false;

		if (textInput.WorkOnKey(key, character) != 3)
			return true;

		if (currentScreen == 9 || currentScreen == 12)
		{
			using MessageHandler = std::uint32_t(__thiscall*)(void*, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

			const std::byte* vtable;
			std::memcpy(&vtable, unknown216C, sizeof(vtable));

			MessageHandler handleMessage;
			std::memcpy(&handleMessage, vtable + 8, sizeof(handleMessage));

			handleMessage(unknown216C, 0x800u, currentScreen == 9 ? 0x13A1u : 0x13A5u, 0, 0, 0);
		}
		else if (currentScreen == 14 || currentScreen == 16 || currentScreen == 17)
		{
			if (std::strlen(textInput.GetString()) != 0)
			{
				char formattedText[128];

				NXBasics::Runtime_PrintStringN(formattedText, 127, "%s> %s", NNetwork::Network_GetCallsign(), textInput.GetString());

				wchar_t message[128]{};
				formattedText[127] = '\0';

				NNetwork::CopyNetworkAnsiToWide(message + 1, formattedText, -1);

				message[0] = 1;
				NNetwork::SendDynamicNetworkChatMessage(&DAT_0050f6a8, 0, message);
			}

			textInput.SetString("");
		}

		return true;
	}

	// Original function: FUN_004d3308
	void CGuiManager::Debug_ErrorString_Add(const char* text)
	{
		char message[0x12C];

		NXBasics::Runtime_PrintString(message, "PROBLEM: <%s>!", text);
		WriteNetworkLog(GetNetworkLog(), message, 0);

		if (errorCount != 0)
		{
			const std::uint32_t movedCount = errorCount == 10u ? 9u : errorCount;

			std::memmove(errorTexts[1], errorTexts[0], movedCount * 0xCAu);
		}

		std::strncpy(errorTexts[0], text, 0xC8);
		errorTexts[0][0xC8] = '\0';

		if (errorCount != 10u)
			++errorCount;
	}

	// Original function: FUN_004d33c6
	void CGuiManager::Screen_ChangeTo(std::int32_t screen, bool shutDownNetwork)
	{
		if (screen == currentScreen)
			return;

		l_ScreenData_Exit(shutDownNetwork);
		l_ScreenData_Init(screen);

		// Pending: CGuiManager::Screen_UpdateGui.
		FUN_004d3a6d(this);
	}

	// Original function: FUN_004d33ef
	void CGuiManager::l_ScreenData_Exit(bool shutDownNetwork)
	{
		FUN_0040aebd(0.0f);

		switch (currentScreen)
		{
			case 9:
				if (shutDownNetwork)
				{
					NNetwork::Network_ShutDown();
				}
				else
				{
					auto* configuration = NMasterPropertyManager::CPropertyManager::sTheObjectPtr;
					const auto callsign = Network_GetCallsign();

					configuration->Property_Add("multiplayer_callsign", false, 0, callsign);
					FUN_004e1816(NMasterPropertyManager::CPropertyManager::sTheObjectPtr);
				}

				textInput.Exit();
				break;

			case 10:
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
			case 19:
			case 20:
				if (shutDownNetwork)
					NNetwork::Network_ShutDown();

				textInput.Exit();
				break;

			case 16:
			case 17:
				if (fileTransferManager != nullptr)
				{
					fileTransferManager->~CFileTransferManager();
					NXBasics::Runtime_Free(fileTransferManager);
					fileTransferManager = nullptr;
				}

				if (shutDownNetwork)
					NNetwork::Network_ShutDown();

				break;

			case 18:
				if (shutDownNetwork)
					NNetwork::Network_ShutDown();

				errorCount = 0;
				break;

			default:
				break;
		}

		currentScreen = 0;
	}

	// Original function: FUN_004d34d2
	void CGuiManager::l_ScreenData_Init(std::int32_t screen)
	{
		if (currentScreen != 0)
			return;

		currentScreen = screen;

		switch (screen)
		{
			case 9:
				if (!StartNetworkForMenu(this))
				{
					currentScreen = 18;
					Debug_ErrorString_Add("Could not init multi player module");
					break;
				}

				FUN_004d7b06(this);
				FUN_004d7c95(this);
				textInput.Init(NNetwork::Network_GetCallsignBufferSize() - 2, false, true, 0u, 0u);
				textInput.SetString(NNetwork::Network_GetCallsign());
				break;

			case 10:
				if (!FUN_004d70b6(this))
				{
					currentScreen = 18;
					Debug_ErrorString_Add("Could not enumerate service provider");
				}

				break;

			case 12:
				std::memset(sessionSearchData, 0, sizeof(sessionSearchData));
				textInput.Init(100u, false, true, 0u, 0u);
				break;

			case 14:
				{
					textInput.Init(60u, true, true, 0u, 0u);

					std::memset(lobbyData, 0, sizeof(lobbyData));
					lobbyValue = 0;

					if (!NNetwork::Network_IsHost())
					{
						std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
						message[0] = 2;

						const DPNID hostPlayerId = NNetwork::GetDynamicNetworkHostPlayerId(&DAT_0050f6a8);
						NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, hostPlayerId, message, sizeof(message));
					}

					if (!FUN_004d76cc(this))
					{
						currentScreen = 18;
						Debug_ErrorString_Add("Update player list failed");
						break;
					}

					if (NNetwork::Network_IsHost() && DAT_005114b4 != nullptr)
						FUN_00416413(DAT_005114b4);

					break;
				}

			case 15:
				std::memset(reinterpret_cast<std::byte*>(this) + 0xEEC, 0, 0x24);
				break;

			case 16:
			case 17:
				{
					std::memset(reinterpret_cast<std::byte*>(this) + 0xEEC, 0, 0x24);

					char sourceFileName[0x104];
					char transferFileName[0x104];
					char requestedFileName[0x104];

					if (NNetwork::Network_IsHost())
					{
						if (screen == 17)
						{
							const auto* mapInformation = reinterpret_cast<const std::byte*>(FUN_00416418(DAT_005114b4, lobbyData + 4));

							std::strcpy(sourceFileName, reinterpret_cast<const char*>(mapInformation + 0x386));
							NXBasics::Runtime_PrintString(transferFileName, DAT_00500818, FUN_0044029d(), "transdat");

							{
								NXBasics::CMemory temporaryFileData(sourceFileName);
								NXBasics::CFile outputFile(transferFileName, false);

								outputFile.OpenForWriting(true);
								outputFile.Write(temporaryFileData.mData, temporaryFileData.mSize);
							}

							transferFlags[4] = 1;

							NXBasics::Runtime_PrintString(transferFileName, DAT_00500818, FUN_0044029d(), "transdat");
						}
						else
						{
							std::uint32_t saveGameId;
							std::memcpy(&saveGameId, lobbyData + 0x14, sizeof(saveGameId));

							const auto saveGameName = FUN_00415e8d(DAT_005114ac, saveGameId);

							FUN_004161a2(DAT_005114ac, saveGameName, sourceFileName);
							FUN_004161a2(DAT_005114ac, "sys_continue", transferFileName);

							{
								NXBasics::CMemory temporaryFileData(sourceFileName);
								NXBasics::CFile outputFile(transferFileName, false);

								outputFile.OpenForWriting(true);
								outputFile.Write(temporaryFileData.mData, temporaryFileData.mSize);
							}

							transferFlags[1] = 1;

							FUN_004161a2(DAT_005114ac, "sys_continue", transferFileName);
						}

						NNetwork::CFileCompressor compressor;
						const std::int32_t compressionResult = compressor.CompressFile(transferFileName);

						if (compressionResult > 0 && compressionResult <= 2)
						{
							currentScreen = 18;

							if (screen == 17)
								Debug_ErrorString_Add("Map compression failed (I/O error)");
							else
								Debug_ErrorString_Add("Save game compression failed (I/O error)");
						}

						// The original starts the service even after this error.
						void* memory = NXBasics::Runtime_AllocateWithNewHandler(sizeof(NNetwork::CFileTransferManager), 1);

						fileTransferManager = memory != nullptr ? new (memory) NNetwork::CFileTransferManager : nullptr;
						fileTransferManager->StartService();
					}
					else
					{
						if (screen == 17)
						{
							transferFlags[6] = 0;
							transferFlags[5] = 1;
						}
						else
						{
							transferFlags[3] = 0;
							transferFlags[2] = 1;
						}

						void* memory = NXBasics::Runtime_AllocateWithNewHandler(sizeof(NNetwork::CFileTransferManager), 1);

						fileTransferManager = memory != nullptr ? new (memory) NNetwork::CFileTransferManager : nullptr;
						fileTransferManager->StartService();

						if (screen == 17)
						{
							FUN_004065e2(FUN_0044029d());

							NXBasics::Runtime_PrintString(requestedFileName, DAT_00500818, FUN_0044029d(), "transdat");
							std::strcat(requestedFileName, ".c2p");
						}
						else
						{
							FUN_00415e64(DAT_005114ac, 2, false);
							FUN_004161a2(DAT_005114ac, "sys_continue", requestedFileName);
							FUN_004065e2(FUN_004161c5(DAT_005114ac));

							std::strcat(requestedFileName, ".c2p");
						}

						fileTransferManager->StartTransfer(requestedFileName, &transferId);
					}

					transferStartTick = timeGetTime();
					transferIsHost = static_cast<std::uint8_t>(NNetwork::Network_IsHost());

					FUN_0040aebd(0.0f);
					return;
				}

			case 19:
				if (!FUN_004d7b0b(this))
				{
					currentScreen = 18;
					Debug_ErrorString_Add("Could not auto host session");
				}
				else
				{
					currentScreen = 0;
					l_ScreenData_Init(14);
				}

				break;

			case 20:
				{
					const auto session = NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetStringValuePtr("multiplayer_auto_join");

					if (!FUN_004d7ba7(this, session))
					{
						currentScreen = 18;
						Debug_ErrorString_Add("Could not auto join session");
					}
					else
					{
						currentScreen = 0;
						l_ScreenData_Init(14);
					}

					break;
				}

			default:
				break;
		}

		FUN_0040aebd(DAT_004f5254);
	}

	// Original function: FUN_004d6b4a
	bool CGuiManager::Screen_IsMultiPlayerScreen() const
	{
		return currentScreen >= 9 && currentScreen <= 20;
	}

	// Original function: FUN_004d7092
	bool StartNetworkForMenu(void* guiManager)
	{
		auto* objectBytes = static_cast<std::byte*>(guiManager);

		// Original helper FUN_00410b46 forwards to memset with a zero value.
		std::memset(objectBytes + 0x68, 0, 0x4B8);

		if (!NNetwork::Network_StartUp())
		{
			NNetwork::Network_ShutDown();
			return false;
		}

		return true;
	}

	// Original function: FUN_004d7389
	char* CGuiManager::Network_GetCallsign()
	{
		if (currentScreen == 9)
		{
			NNetwork::Network_SetCallsign(textInput.GetString());
		}

		return NNetwork::Network_GetCallsign();
	}

	// Original function: FUN_004d73b0
	char* CGuiManager::Network_GetSessionName()
	{
		char* sessionName = reinterpret_cast<char*>(sessionSearchData);

		if (currentScreen == 12)
			std::strcpy(sessionName, textInput.GetString());

		return sessionName;
	}

	// Original function: FUN_004d7ca9
	void CGuiManager::Chat_AddText(std::uint32_t type, const char* text)
	{
		std::uint32_t count;
		std::memcpy(&count, unknown1070, sizeof(count));

		if (count != 0)
		{
			const std::uint32_t movedCount = count == 20u ? 19u : count;

			std::memmove(unknown1070 + 4 + 0x74, unknown1070 + 4, movedCount * 0x74u);
		}

		// Each record contains a four-byte type followed by its text storage.
		std::memset(unknown1070 + 4, 0, 0x74);
		std::strncpy(reinterpret_cast<char*>(unknown1070 + 8), text, 0x6D);

		std::memcpy(unknown1070 + 4, &type, sizeof(type));

		// Reload the count after inserting the record, as the original does.
		std::memcpy(&count, unknown1070, sizeof(count));

		if (count != 20u)
		{
			++count;
			std::memcpy(unknown1070, &count, sizeof(count));
		}
	}

	// Original function: FUN_004d85cf
	void CGuiManager::l_Network_Update()
	{
		// Access unresolved fields without changing the established object layout.
		const auto readWord = [this] (std::size_t offset) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, reinterpret_cast<const std::byte*>(this) + offset, sizeof(value));
			return value;
		};

		const auto writeWord = [this] (std::size_t offset, std::uint32_t value)
		{
			std::memcpy(reinterpret_cast<std::byte*>(this) + offset, &value, sizeof(value));
		};

		const auto fail = [this] (const char* text)
		{
			Screen_ChangeTo(18, true);
			Debug_ErrorString_Add(text);
		};

		if (Screen_IsMultiPlayerScreen() && DAT_0050f6a8.active != 0)
		{
			NNetwork::UpdateDynamicNetwork(&DAT_0050f6a8, false);

			if (DAT_0050f6a8.receivedRecordCount != 0)
			{
				void* chatBuffer = nullptr;
				std::uint32_t chatCount = 0;

				NNetwork::TakeDynamicNetworkTextRecords(&DAT_0050f6a8, &chatBuffer, &chatCount);

				const auto* records = static_cast<const std::byte*>(chatBuffer);

				for (std::uint32_t index = 0; index < chatCount; ++index)
				{
					const std::byte* record = records + index * 0x104u;

					std::uint16_t textType;
					std::memcpy(&textType, record + 4, sizeof(textType));

					if (textType == 1)
					{
						char text[0x80] = {};

						NNetwork::ConvertNetworkWideToAnsi(text, reinterpret_cast<const wchar_t*>(record + 6), -1);
						Chat_AddText(0, text);
						NC2GuiToolsBase::FXTool_Chat();
					}
				}

				if (chatBuffer != nullptr)
					NXBasics::Runtime_Free(chatBuffer);
			}

			l_Network_ProcessDataMessages();

			// Processing a data message can shut down the network.
			if (DAT_0050f6a8.active != 0)
			{
				for (NNetwork::SEventNode* event = DAT_0050f6a8.events.first; event != nullptr; event = event->next)
				{
					char playerName[100] = {};
					char text[0x12C];
					text[0] = '\0';

					const auto* name = reinterpret_cast<const wchar_t*>(reinterpret_cast<const std::byte*>(event) + 4);

					if (*name != L'\0')
						NNetwork::CopyNetworkWideToAnsi(playerName, name, -1);
					else
						NXBasics::Runtime_PrintString(playerName, DAT_004fe238);

					std::uint32_t eventType;
					std::memcpy(&eventType, reinterpret_cast<const std::byte*>(event) + 0x208, sizeof(eventType));

					if (eventType >= 1u && eventType <= 13u)
					{
						static constexpr std::uint32_t textIds[13] =
						{
							0x59, 0x58, 0x57, 0x57, 0x57, 0x57, 0x57,
							0x5A, 0x5B, 0x5C, 0x5D, 0x5E, 0x5F
						};

						const char* description = FUN_004e1e35(textIds[eventType - 1u]);
						const char* format = eventType >= 2u && eventType <= 7u ? "%s %s!" : ">> %s %s!";

						NXBasics::Runtime_PrintString(text, format, playerName, description);
					}
					else
					{
						char description[0xC8] = {};

						FUN_004db635(&DAT_0050f6a8.events, event, description, sizeof(description));
						NXBasics::Runtime_PrintString(text, ">> NET: '%s'", description);
					}

					if (text[0] != '\0')
						Chat_AddText(1, text);
				}

				NNetwork::ClearDynamicNetworkEvents(&DAT_0050f6a8);
			}
		}

		const std::uint32_t networkFlags = NNetwork::Network_GetFlags();

		switch (currentScreen)
		{
			case 10:
				{
					if ((networkFlags & 1u) == 0)
					{
						fail("Wrong network state while selecting service provider");
						return;
					}

					std::uint32_t providerIndex = 0;

					for (; providerIndex < readWord(0x68); ++providerIndex)
					{
						const std::byte* providerGuid = reinterpret_cast<const std::byte*>(this) + 0xD4 + providerIndex * 0x78u;

						if (std::memcmp(providerGuid, &DAT_004f7e48, sizeof(GUID)) == 0)
							break;
					}

					if (providerIndex == readWord(0x68))
					{
						Debug_ErrorString_Add("No TCPIP service provider found");
						return;
					}

					writeWord(0x51C, providerIndex);

					reinterpret_cast<CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(11, false);
					return;
				}

			case 11:
				if ((networkFlags & 1u) == 0)
					fail("Wrong network state while selecting host or client");

				return;

			case 13:
				if ((networkFlags & 4u) == 0)
				{
					fail("Wrong network state while searching sessions");
					return;
				}

				if (FUN_004d74a4(this))
					FUN_004d3a6d(this);

				return;

			default:
				if (currentScreen < 14 || currentScreen > 17)
					return;

				break;
		}

		if (NNetwork::Network_IsHost())
		{
			if ((networkFlags & 2u) == 0)
			{
				fail("Wrong network state while waiting for game start while hosting");
				return;
			}
		}
		else
		{
			if ((networkFlags & 8u) == 0)
			{
				fail("Wrong network state while waiting for game start while connected");
				return;
			}
		}

		if (readWord(0xEE0) != GetNetworkInstance()->playerChangeCount)
		{
			if (currentScreen == 16 || currentScreen == 17)
			{
				fail("Player left session while transfering savegame");
				return;
			}

			if (!FUN_004d76cc(this))
			{
				fail("Update player list failed");
				return;
			}

			FUN_004d3a6d(this);
		}

		const auto hasPlayer = [&readWord] (std::uint32_t playerId) -> bool
		{
			const std::uint32_t playerCount = readWord(0xAA4);

			for (std::uint32_t index = 0; index < playerCount; ++index)
			{
				if (readWord(0xB10 + index * 0x6Cu) == playerId)
					return true;
			}

			return false;
		};

		const std::uint32_t selectedPlayerId = readWord(0xEE4);

		if (selectedPlayerId != 0 && !hasPlayer(selectedPlayerId))
			writeWord(0xEE4, 0);

		bool assignmentsChanged = false;

		for (std::uint32_t index = 0; index < 20; ++index)
		{
			const std::size_t offset = 0xF28 + index * 0x10u;
			const std::uint32_t playerId = readWord(offset);

			if (playerId != 0 && !hasPlayer(playerId))
			{
				writeWord(offset, 0);
				assignmentsChanged = true;
			}
		}

		if (assignmentsChanged)
		{
			FUN_004d3a6d(this);
			FUN_004d7aa7(this);
		}

		if (currentScreen == 14)
		{
			if (transferIsHost != static_cast<std::uint8_t>(NNetwork::Network_IsHost()))
			{
				if (NNetwork::Network_IsHost())
					FUN_004d8169(this);

				transferIsHost = static_cast<std::uint8_t>(NNetwork::Network_IsHost());
			}
		}

		if (currentScreen == 15 && NNetwork::Network_IsHost())
		{
			bool allPlayersAssigned = true;
			const std::uint32_t playerCount = readWord(0xAA4);

			for (std::uint32_t playerIndex = 0; playerIndex < playerCount; ++playerIndex)
			{
				const std::uint32_t playerId = readWord(0xB10 + playerIndex * 0x6Cu);
				bool assigned = false;

				for (std::uint32_t slot = 0; slot < 20; ++slot)
				{
					if (readWord(0xF28 + slot * 0x10u) == playerId)
					{
						assigned = true;
						break;
					}
				}

				if (!assigned)
				{
					allPlayersAssigned = false;
					break;
				}
			}

			if (allPlayersAssigned)
			{
				if (readWord(0xF10) == 2u)
				{
					reinterpret_cast<CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(16, false);

					std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
					message[0] = 4;

					NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, 0, message, sizeof(message));
				}
				else
				{
					const std::byte* mapInformation = nullptr;

					if (FUN_00416b63(lobbyData + 4))
						mapInformation = reinterpret_cast<const std::byte*>(FUN_00416418(DAT_005114b4, lobbyData + 4));

					if (mapInformation != nullptr && (std::to_integer<std::uint8_t>(mapInformation[0x120]) & 0x40u) != 0 && mapInformation[0x385] != std::byte{ 0 })
					{
						reinterpret_cast<CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(17, false);

						std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
						message[0] = 5;

						NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, 0, message, sizeof(message));
					}
					else if (transferFlags[0] == 0)
					{
						transferFlags[0] = 1;

						NNetwork::BroadcastControlMessage8(&DAT_0050f6a8);

						std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
						auto* messageBytes = reinterpret_cast<std::byte*>(message);

						message[0] = 1;

						const auto* menu = reinterpret_cast<const CGuiManager*>(DAT_005694dc);
						std::memcpy(messageBytes + 8, menu->lobbyData, sizeof(menu->lobbyData));

						messageBytes[4] = std::byte{ 1 };

						NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, 0, message, sizeof(message));
					}
				}
			}
		}

		// This deliberately observes screen changes made above.
		l_Transfer_Update();
	}

	// Original function: FUN_004d85cf
	// Extracted block: 004d86a7 through 004d8917.
	void CGuiManager::l_Network_ProcessDataMessages()
	{
		NNetwork::SDynamicDataMessage* messages = nullptr;
		std::uint32_t messageCount = 0;

		NNetwork::GetDynamicNetworkMessages(&DAT_0050f6a8, &messages, &messageCount);

		for (std::uint32_t index = 0; index < messageCount; ++index)
		{
			const NNetwork::SDynamicDataMessage& message = messages[index];

			if (message.checksumMismatch != 0)
				Debug_ErrorString_Add("CRC-32 check failed - message data is damaged");

			std::uint32_t messageType;
			std::memcpy(&messageType, message.payload, sizeof(messageType));

			switch (messageType)
			{
				case 1:
					{
						if (!NNetwork::Network_IsHost())
							std::memcpy(lobbyData, message.payload + 8, sizeof(lobbyData));

						// Pending: CGuiManager::Screen_UpdateGui.
						FUN_004d3a6d(this);

						if (message.payload[4] == 0)
							break;

						std::uint32_t gameMode;
						std::memcpy(&gameMode, lobbyData, sizeof(gameMode));

						if (gameMode == 1u || gameMode == 2u)
						{
							auto* master = NMasterControlProgram::CMasterControlProgram::sTheObjectPtr;
							auto* startupData = reinterpret_cast<std::byte*>(master) + 0x2C;

							// Pending: NC2Logic::SGameControlStartupData::Reset.
							FUN_00406b4a(startupData);

							const std::uint32_t startupType = gameMode == 1u ? 3u : 4u;
							std::memcpy(startupData, &startupType, sizeof(startupType));

							if (gameMode == 1u)
								std::memcpy(startupData + 0x18, lobbyData + 4, 0x10);

							const std::uint32_t localPlayerId = NNetwork::Network_GetLocalPlayerId();
							std::memcpy(startupData + 0x28, &localPlayerId, sizeof(localPlayerId));

							for (std::uint32_t playerIndex = 0; playerIndex < 20; ++playerIndex)
							{
								const std::byte* source = lobbyData + 0x18 + playerIndex * 0x10u;
								std::byte* destination = startupData + 0x2C + playerIndex * 0x0Cu;

								std::uint32_t firstValue;
								std::uint32_t secondValue;
								std::uint32_t thirdValue;

								std::memcpy(&firstValue, source + 4, sizeof(firstValue));
								std::memcpy(&thirdValue, source, sizeof(thirdValue));

								if (lobbyData[0x158] != std::byte{ 0 })
									secondValue = playerIndex < 10u ? playerIndex : 9u;
								else
									std::memcpy(&secondValue, source + 0x0C, sizeof(secondValue));

								std::memcpy(destination, &firstValue, sizeof(firstValue));
								std::memcpy(destination + 4, &secondValue, sizeof(secondValue));
								std::memcpy(destination + 8, &thirdValue, sizeof(thirdValue));
							}

							if (gameMode == 1u)
								std::memcpy(startupData + 0x11C, lobbyData + 0x15C, 4);

							NMasterControlProgram::CMasterControlProgram::sTheObjectPtr->System_ProgramState_ChangeNextTimePossible(NMasterControlProgram::TProgramState::Game, false);
						}

						// The original checks the music setting even for other game modes.
						if (NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_GetIntegerValue("music_mode") == 2)
							NMasterPropertyManager::CPropertyManager::sTheObjectPtr->Property_Add("music_mode", true, DAT_0050f69c != nullptr ? 3 : 1, nullptr);

						break;
					}

				case 2:
					{
						std::uint32_t response[0x168 / sizeof(std::uint32_t)] = {};
						auto* responseBytes = reinterpret_cast<std::byte*>(response);

						response[0] = 1;

						const auto* menu = reinterpret_cast<const CGuiManager*>(DAT_005694dc);
						std::memcpy(responseBytes + 8, menu->lobbyData, sizeof(menu->lobbyData));

						responseBytes[4] = std::byte{ 0 };

						NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, message.sender, response, sizeof(response));
						break;
					}

				case 3:
					Screen_ChangeTo(0x15, true);
					break;

				case 4:
					if (!NNetwork::Network_IsHost())
						Screen_ChangeTo(0x10, false);

					break;

				case 5:
					if (!NNetwork::Network_IsHost())
						Screen_ChangeTo(0x11, false);

					break;

				default:
					break;
			}
		}

		if (messages != nullptr)
			NXBasics::Runtime_Free(messages);
	}

	// Original function: FUN_004d85cf
	// Extracted transfer section beginning at 004d8d5e.
	void CGuiManager::l_Transfer_Update()
	{
		if (currentScreen == 16)
		{
			const std::uint8_t isHost = static_cast<std::uint8_t>(NNetwork::Network_IsHost());

			if (transferIsHost != isHost)
			{
				Screen_ChangeTo(18, true);
				Debug_ErrorString_Add("Host changed while transfering savegame");
				return;
			}

			// Preserve the second host query performed by the original.
			if (NNetwork::Network_IsHost())
				l_Transfer_UpdateUpload(false);
			else
				l_Transfer_UpdateDownload(false);
		}

		// Re-read the screen after the savegame branch.
		if (currentScreen == 17)
		{
			const std::uint8_t isHost = static_cast<std::uint8_t>(NNetwork::Network_IsHost());

			if (transferIsHost != isHost)
			{
				Screen_ChangeTo(18, true);
				Debug_ErrorString_Add("Host changed while transfering user map");
				return;
			}

			if (NNetwork::Network_IsHost())
				l_Transfer_UpdateUpload(true);
			else
				l_Transfer_UpdateDownload(true);
		}
	}

	// Original function: FUN_004d85cf
	// Extracted blocks: 004d8f13 through 004d9053 and 004d924e through 004d950f.
	void CGuiManager::l_Transfer_UpdateDownload(bool userMap)
	{
		const std::uint32_t requestedFlag = userMap ? 5u : 2u;
		const std::uint32_t completedFlag = userMap ? 6u : 3u;

		if (transferFlags[requestedFlag] == 0 || transferFlags[completedFlag] != 0)
			return;

		const auto reportFailure = [this] (const char* text)
		{
			// The original changes the singleton's screen and logs on this object.
			reinterpret_cast<CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(18, true);
			Debug_ErrorString_Add(text);
		};

		if (fileTransferManager == nullptr || fileTransferManager->serviceState != 1)
		{
			reportFailure(userMap ? "Clean map transfer failed! Transfer not started!" : "Savegame transfer failed! Transfer not started!");
			return;
		}

		const NNetwork::SFileTransferInfo* info = fileTransferManager->GetTransferInfo(&transferId);

		if (info == nullptr)
		{
			reportFailure(userMap ? "Clean map transfer failed! Could not get description!" : "Savegame transfer failed! Could not get description!");
			return;
		}

		if (info->fileSize == 0 || info->transferredBytes != info->fileSize)
			return;

		if (info->expectedChecksum != info->checksum)
		{
			reportFailure(userMap ? "Clean map transfer failed! Checksum do not match!" : "Savegame transfer failed! Checksum do not match!");
			return;
		}

		transferFlags[completedFlag] = 1;

		if (fileTransferManager != nullptr)
		{
			fileTransferManager->~CFileTransferManager();
			NXBasics::Runtime_Free(fileTransferManager);
		}

		fileTransferManager = nullptr;

		char compressedFileName[0x104];

		NNetwork::CFileCompressor compressor;

		if (!userMap)
		{
			FUN_004161a2(DAT_005114ac, "sys_continue", compressedFileName);
			std::strcat(compressedFileName, ".c2p");

			const std::int32_t result = compressor.DecompressFile(compressedFileName);

			if (result != 0)
				reportFailure("Savegame decompression failed! File I/O error!");

			// The original performs this call even after decompression fails.
			FUN_00415e64(DAT_005114ac, 2, true);
			return;
		}

		std::strcpy(compressedFileName, FUN_0044029d());
		std::strcat(compressedFileName, DAT_004ff054);
		std::strcat(compressedFileName, "transdat");
		std::strcat(compressedFileName, ".c2p");

		const std::int32_t result = compressor.DecompressFile(compressedFileName);

		if (result != 0)
		{
			reportFailure("Map decompression failed! File I/O error!");
		}
		else
		{
			char archiveFileName[0x104];
			char destinationFileName[0x104];

			NXBasics::Runtime_PrintString(archiveFileName, DAT_00500818, FUN_0044029d(), "transdat");

			NXBasics::SArchiveState archive;
			NXBasics::Archive_Initialize(&archive, archiveFileName);

			NXBasics::CFile::FileSystem_FileLibrary_Add(&archive);

			{
				NXBasics::CIniFileReader reader("currentusermap\\map.ini", nullptr, false, nullptr, nullptr);

				// The reader has already loaded its input at this point.
				NXBasics::CFile::FileSystem_FileLibrary_Remove(&archive);

				if (reader.IsLoaded() && reader.SearchGroup("logiccontrol") && reader.SearchCommand("mapguid"))
				{
					std::uint8_t mapGuid[16];

					for (std::uint32_t index = 0; index < 16; ++index)
						mapGuid[index] = static_cast<std::uint8_t>(reader.GetInt());

					NXBasics::Runtime_PrintString(destinationFileName, "%s\\usermap_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d_%d.c2m", FUN_0044029d(), static_cast<int>(mapGuid[0]), static_cast<int>(mapGuid[1]), static_cast<int>(mapGuid[2]), static_cast<int>(mapGuid[3]), static_cast<int>(mapGuid[4]), static_cast<int>(mapGuid[5]), static_cast<int>(mapGuid[6]), static_cast<int>(mapGuid[7]), static_cast<int>(mapGuid[8]), static_cast<int>(mapGuid[9]), static_cast<int>(mapGuid[10]), static_cast<int>(mapGuid[11]), static_cast<int>(mapGuid[12]), static_cast<int>(mapGuid[13]), static_cast<int>(mapGuid[14]), static_cast<int>(mapGuid[15]));
				}
				else
				{
					NXBasics::Runtime_PrintString(destinationFileName, "%s\\transfered.c2m", FUN_0044029d());
				}
			}

			NXBasics::Archive_Destroy(&archive);

			// The original ignores the rename result.
			NXBasics::Runtime_RenameFile(archiveFileName, destinationFileName);
		}

		// The original refreshes the map list after either decompression outcome.
		FUN_00416413(DAT_005114b4);
	}

	// Original function: FUN_004d85cf
	// Extracted host blocks: 004d8d98 through 004d8f13 and 004d908b through 004d924e.
	void CGuiManager::l_Transfer_UpdateUpload(bool userMap)
	{
		const std::uint32_t uploadFlag = userMap ? 4u : 1u;

		if (transferFlags[uploadFlag] == 0 || transferFlags[0] != 0 || fileTransferManager == nullptr)
			return;

		const auto* objectBytes = reinterpret_cast<const std::byte*>(this);

		std::uint32_t playerCount;
		std::memcpy(&playerCount, objectBytes + 0xAA4, sizeof(playerCount));

		std::uint32_t expectedTransfers = 0;

		// Preserve the original player-record accesses without guessing their types.
		const std::byte* playerFields = objectBytes + 0xB0E;

		for (std::uint32_t index = 0; index < playerCount; ++index)
		{
			std::uint32_t playerId;
			std::memcpy(&playerId, playerFields + 2, sizeof(playerId));

			if (playerId != 0 && playerFields[0] == std::byte{ 0 })
				++expectedTransfers;

			playerFields += 0x6C;
		}

		NNetwork::SFileTransferNode jobs[20];
		std::uint32_t jobCount = 20;

		fileTransferManager->GetTransferList(jobs, &jobCount);

		std::uint32_t completedTransfers = 0;

		for (std::uint32_t index = 0; index < jobCount; ++index)
		{
			if (jobs[index].state == 1)
			{
				++completedTransfers;
			}
			else if (jobs[index].state == 14)
			{
				reinterpret_cast<CGuiManager*>(DAT_005694dc)->Screen_ChangeTo(18, true);
				Debug_ErrorString_Add(userMap ? "Clean map transfer failed!" : "Savegame transfer failed!");

				// The original continues evaluating the copied job list.
			}
		}

		// The original destroys all twenty copied payloads in reverse order.
		for (std::uint32_t index = 20; index != 0;)
		{
			NNetwork::SFileTransferNode& job = jobs[--index];

			if (job.fileName != nullptr)
			{
				NXBasics::Runtime_Free(job.fileName);
				job.fileName = nullptr;
			}

			if (job.path24 != nullptr)
			{
				NXBasics::Runtime_Free(job.path24);
				job.path24 = nullptr;
			}
		}

		if (completedTransfers != expectedTransfers)
			return;

		if (fileTransferManager != nullptr)
		{
			fileTransferManager->~CFileTransferManager();
			NXBasics::Runtime_Free(fileTransferManager);
			fileTransferManager = nullptr;
		}

		if (transferFlags[0] != 0)
			return;

		transferFlags[0] = 1;

		NNetwork::BroadcastControlMessage8(&DAT_0050f6a8);

		std::uint32_t message[0x168 / sizeof(std::uint32_t)] = {};
		auto* messageBytes = reinterpret_cast<std::byte*>(message);

		message[0] = 1;
		messageBytes[4] = std::byte{ 1 };

		const auto* menu = reinterpret_cast<const CGuiManager*>(DAT_005694dc);
		std::memcpy(messageBytes + 8, menu->lobbyData, sizeof(menu->lobbyData));

		NNetwork::SendDynamicNetworkMessage(&DAT_0050f6a8, 0, message, sizeof(message));

		if (userMap)
		{
			char fileName[0x104];

			NXBasics::Runtime_PrintString(fileName, DAT_00500818, FUN_0044029d(), "transdat");
			NXBasics::Runtime_RemoveFile(fileName);

			std::strcat(fileName, ".c2p");
			NXBasics::Runtime_RemoveFile(fileName);
		}
	}

	// Original function: FUN_004d9605
	void CGuiManager::l_Network_Draw()
	{
		if (!Screen_IsMultiPlayerScreen())
			return;

		const auto readWord = [] (const void* object, std::size_t offset) -> std::uint32_t
		{
			std::uint32_t value;
			std::memcpy(&value, static_cast<const std::byte*>(object) + offset, sizeof(value));
			return value;
		};

		const auto signedWord = [] (std::uint32_t value) -> std::int32_t
		{
			return NXBasics::Runtime_TimeSigned32(value);
		};

		const auto desktop = [this] () -> NXBasics::CBitmap*
		{
			return reinterpret_cast<NXBasics::CBitmap*>(FUN_004d32ff(this));
		};

		const auto defaultColour = [] () -> const NXBasics::SColorRGB*
		{
			return reinterpret_cast<const NXBasics::SColorRGB*>(reinterpret_cast<const std::byte*>(DAT_00569908) + 0x400);
		};

		const auto drawText = [&] (NXBasics::CBitmap* bitmap, auto* font, std::int32_t x, std::int32_t y, const char* text)
		{
			bitmap->Text_Print(font, defaultColour(), x, y, text);
		};

		const auto drawFormatted = [&] (NXBasics::CBitmap* bitmap, auto* font, std::int32_t x, std::int32_t y, const char* format, auto... arguments)
		{
			bitmap->Text_PrintV(font, defaultColour(), x, y, format, arguments...);
		};

		const auto drawCursor = [&] (auto* font, std::uint32_t x, std::uint32_t y, std::int32_t length, NXBasics::CBitmap* bitmap)
		{
			const std::uint8_t intensity = ((::timeGetTime() / 200u) & 1u) != 0 ? 255 : 100;
			const NXBasics::SColorRGB colour(intensity, intensity, intensity);
			const std::uint32_t cursorX = x + textInput.GetCursorPixelPosition(font);

			if (bitmap == nullptr)
				bitmap = desktop();

			bitmap->Draw_VerticalLine(signedWord(cursorX), y, length, colour);
		};

		auto* inputFont = DAT_005698fc;

		if (currentScreen == 9)
		{
			std::int32_t y = signedWord(static_cast<std::uint32_t>(DAT_005694b8)) / 2 - 50;
			const auto* colour = defaultColour();
			const char* text = FUN_004e1e19(0xD1);

			desktop()->Text_Print(inputFont, colour, 56, y, text);

			y += 30;
			colour = defaultColour();
			text = Network_GetCallsign();

			desktop()->Text_Print(inputFont, colour, 66, y, text);
			drawCursor(inputFont, 66, static_cast<std::uint32_t>(y), 16, nullptr);
		}

		if (currentScreen == 12)
		{
			std::int32_t y = signedWord(static_cast<std::uint32_t>(DAT_005694b8)) / 2 - 50;
			const auto* colour = defaultColour();
			const char* text = FUN_004e1e19(0xD2);

			desktop()->Text_Print(inputFont, colour, 56, y, text);

			y += 16;
			colour = defaultColour();
			text = FUN_004e1e19(0xD3);

			desktop()->Text_Print(inputFont, colour, 56, y, text);

			y += 30;
			colour = defaultColour();
			text = Network_GetSessionName();

			desktop()->Text_Print(inputFont, colour, 66, y, text);
			drawCursor(inputFont, 66, static_cast<std::uint32_t>(y), 16, nullptr);
		}

		const std::int32_t chatY = signedWord(readWord(desktop(), 0x14)) / 2 + 80;
		const std::int32_t informationY = signedWord(readWord(desktop(), 0x14)) / 2 + 210;
		auto* informationFont = DAT_005698f8;

		const auto drawTransferProgress = [&] (bool userMap)
		{
			std::int32_t y = signedWord(static_cast<std::uint32_t>(DAT_005694b8)) / 2 - 70;
			NXBasics::CBitmap* bitmap = desktop();
			auto* font = DAT_005698f8;

			if (NNetwork::Network_IsHost())
			{
				const std::size_t startedFlag = userMap ? 4 : 1;

				if (transferFlags[startedFlag] != 0 && fileTransferManager != nullptr)
				{
					NNetwork::SFileTransferNode transfers[20];

					for (auto& transfer : transfers)
						std::memset(&transfer.fileName, 0, sizeof(NNetwork::SFileTransferInfo));

					std::uint32_t count = 20;
					fileTransferManager->GetTransferList(transfers, &count);

					const std::uint32_t now = ::timeGetTime();

					for (std::uint32_t index = 0; index < count; ++index)
					{
						char text[100];
						std::strcpy(text, FUN_004e1e19(0xC9));

						const auto& transfer = transfers[index];
						const char* playerName = FUN_004d7d56(this, transfer.peerId);
						const std::uint32_t transferred = transfer.transferredBytes;
						const std::uint32_t size = transfer.fileSize;

						// Preserve the original unsigned arithmetic and unguarded division.
						const std::uint32_t percent = (transferred * 100u) / size;
						const std::uint32_t elapsed = (now - transferStartTick) / 1000u;
						std::uint32_t estimatedTotal = percent != 0 ? (elapsed * 100u) / percent : 3600u;

						if (estimatedTotal >= 3600u)
							estimatedTotal = 3600u;

						if (transfer.state == 1)
						{
							NXBasics::Runtime_PrintString(text, FUN_004e1e19(0xCC), playerName);
						}
						else if (transfer.state == 13)
						{
							const std::uint32_t remaining = estimatedTotal - elapsed;

							NXBasics::Runtime_PrintString(text, FUN_004e1e19(0xCA), playerName, percent, transferred >> 10, size >> 10, elapsed / 60u, elapsed % 60u, remaining / 60u, remaining % 60u);
						}
						else if (transfer.state == 14)
						{
							NXBasics::Runtime_PrintString(text, FUN_004e1e19(0xCB), playerName);
						}

						drawText(bitmap, font, 56, y, text);
						y = signedWord(static_cast<std::uint32_t>(y) + 20u);
					}

					// Release all twenty copied subobjects in reverse construction order.
					for (std::uint32_t index = 20; index != 0; --index)
					{
						auto& transfer = transfers[index - 1];

						if (transfer.fileName != nullptr)
						{
							NXBasics::Runtime_Free(transfer.fileName);
							transfer.fileName = nullptr;
						}

						if (transfer.path24 != nullptr)
						{
							NXBasics::Runtime_Free(transfer.path24);
							transfer.path24 = nullptr;
						}
					}
				}
			}
			else
			{
				char text[100];
				std::strcpy(text, FUN_004e1e19(0xCD));

				const std::size_t receivingFlag = userMap ? 5 : 2;
				const std::size_t finishedFlag = userMap ? 6 : 3;

				if (transferFlags[receivingFlag] != 0 && transferFlags[finishedFlag] == 0 && fileTransferManager != nullptr && fileTransferManager->serviceState == 1)
				{
					NNetwork::SFileTransferInfo* info = fileTransferManager->GetTransferInfo(&transferId);

					if (info != nullptr)
					{
						if (info->fileSize == 0)
						{
							std::strcpy(text, FUN_004e1e19(0xCE));
						}
						else
						{
							const std::uint32_t percent = (info->transferredBytes * 100u) / info->fileSize;
							const std::uint32_t elapsed = (::timeGetTime() - transferStartTick) / 1000u;
							std::uint32_t estimatedTotal = percent != 0 ? (elapsed * 100u) / percent : 3600u;

							if (estimatedTotal >= 3600u)
								estimatedTotal = 3600u;

							const std::uint32_t remaining = estimatedTotal - elapsed;

							NXBasics::Runtime_PrintString(text, FUN_004e1e19(0xCF), percent, info->transferredBytes >> 10, info->fileSize >> 10, elapsed / 60u, elapsed % 60u, remaining / 60u, remaining % 60u);
						}
					}
				}

				if (transferFlags[finishedFlag] != 0)
					std::strcpy(text, FUN_004e1e19(0xD0));

				drawText(bitmap, font, 56, y, text);
			}

			::Sleep(50);
		};

		{
			const std::int32_t width = signedWord(readWord(desktop(), 0x10) - 370u);
			const NXBasics::SRectangle rectangle(360, informationY, width, 80);
			NXBasics::CBitmap informationBitmap(desktop(), rectangle);

			informationBitmap.Tool_Darken();
			NC2GuiToolsBase::DrawTool_DrawFrame(&informationBitmap, informationBitmap.GetRectangle(), 2);

			NNetwork::GetNetworkInstance();

			char statusText[100];
			statusText[0] = '\0';

			const std::uint32_t flags = NNetwork::Network_GetFlags();

			const auto appendStatus = [&] (std::uint32_t flag, std::uint32_t textId)
			{
				if ((flags & flag) == 0)
					return;

				if (statusText[0] != '\0')
					std::strcat(statusText, ", ");

				std::strcat(statusText, FUN_004e1e19(textId));
			};

			appendStatus(1, 0xD6);
			appendStatus(4, 0xD7);
			appendStatus(8, 0xD8);
			appendStatus(2, 0xD9);

			if (statusText[0] == '\0')
				std::strcat(statusText, FUN_004e1e19(0xDA));

			drawFormatted(&informationBitmap, informationFont, 4, 2, "%s: <%s>", FUN_004e1e19(0xBB), statusText);

			std::uint32_t y = 14;

			for (std::uint32_t index = 0; index < errorCount; ++index)
			{
				drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), "%s %d: <%s>", FUN_004e1e19(0xBC), index, errorTexts[index]);
				y += 12;
			}

			if (currentScreen == 13 && FUN_004d7849(this))
			{
				const char* sessionName = Network_GetSessionName();

				drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), "%s: <%s>", FUN_004e1e19(0xBD), sessionName);
				y += 12;
			}

			std::strcpy(statusText, FUN_004e1e19(0xBE));
			std::strcat(statusText, ": ");

			char address[32];
			std::uint32_t addressCount = 0;

			while (NNetwork::GetNetworkIPAddress(addressCount, address, 30))
			{
				if (addressCount != 0)
					std::strcat(statusText, ", ");

				std::strcat(statusText, address);
				++addressCount;
			}

			if (addressCount != 0)
			{
				drawText(&informationBitmap, informationFont, 4, signedWord(y), statusText);
				y += 12;
			}

			if (currentScreen == 14)
			{
				void* mapId = lobbyData + 4;

				if (FUN_00416b63(mapId))
				{
					auto* map = FUN_00416418(DAT_005114b4, mapId);

					if (map != nullptr)
					{
						char mapName[100];
						FUN_00416b98(map, mapName);

						drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), "%s: %s", FUN_004e1e19(0xC2), mapName);
					}
					else
					{
						drawText(&informationBitmap, informationFont, 4, signedWord(y), FUN_004e1e19(0xC3));
					}
				}
				else
				{
					const std::uint32_t mapIdValue = readWord(lobbyData, 0x14);

					if (mapIdValue != 0)
					{
						auto* map = FUN_00415e8d(DAT_005114ac, mapIdValue);

						if (map != nullptr)
						{
							const char* mapName = reinterpret_cast<const char*>(map) + 0x136;

							drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), "%s: %s", FUN_004e1e19(0xC4), mapName);
						}
						else
						{
							drawText(&informationBitmap, informationFont, 4, signedWord(y), FUN_004e1e19(0xC5));
						}
					}
					else
					{
						// The original passes this localized text as a format string.
						drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), FUN_004e1e19(0xC6));
					}
				}

				y += 12;

				if (FUN_00416b63(mapId))
				{
					const std::uint32_t mode = readWord(lobbyData, 0x15C);

					if (mode <= 2)
					{
						const char* modeText = FUN_004e1ea5(0x1B + mode * 2u);

						if (modeText != nullptr)
							drawFormatted(&informationBitmap, informationFont, 4, signedWord(y), "%s: %s", FUN_004e1ea5(0x1A), modeText);
					}
				}
			}

			if (currentScreen == 16)
				drawTransferProgress(false);

			if (currentScreen == 17)
				drawTransferProgress(true);
		}

		if (currentScreen == 14 || currentScreen == 16 || currentScreen == 17)
		{
			const std::int32_t width = signedWord(readWord(desktop(), 0x10) - 370u);
			const NXBasics::SRectangle rectangle(360, chatY, width, 120);
			NXBasics::CBitmap chatBitmap(desktop(), rectangle);

			chatBitmap.Tool_Darken();
			NC2GuiToolsBase::DrawTool_DrawFrame(&chatBitmap, chatBitmap.GetRectangle(), 2);

			auto* font = DAT_005698f8;

			drawText(&chatBitmap, font, 4, 2, DAT_0050716c);
			drawText(&chatBitmap, font, 14, 2, textInput.GetString());
			drawCursor(font, 14, 4, 12, &chatBitmap);

			font = DAT_005698f8;
			std::uint32_t y = 14;
			const std::byte* recordText = unknown1070 + 8;

			for (std::uint32_t index = 0; index < readWord(unknown1070, 0); ++index)
			{
				NXBasics::SColorRGB colour(0, 0, 0);
				std::memcpy(&colour, defaultColour(), sizeof(colour));

				const std::uint32_t count = readWord(unknown1070, 0);
				const std::uint32_t type = index < count ? readWord(recordText - 4, 0) : 0;

				if (type == 1)
					std::memcpy(&colour, reinterpret_cast<const std::byte*>(DAT_0056990c) + 0x400, sizeof(colour));
				else if (type == 2)
					std::memcpy(&colour, reinterpret_cast<const std::byte*>(DAT_00569910) + 0x400, sizeof(colour));

				const char* text = index < count ? reinterpret_cast<const char*>(recordText) : nullptr;

				chatBitmap.Text_Print(font, &colour, 4, signedWord(y), text);

				y += 10;
				recordText += 0x74;
			}
		}
	}
}