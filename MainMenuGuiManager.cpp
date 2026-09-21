#include <windows.h>

#include "dplay8.h"
#include "MainMenuGuiManager.h"
#include "MasterControlProgram.h"
#include "MasterPropertyManager.h"
#include "XMMouseManager.h"
#include "XMKeyManager.h"
#include "FileTransferManager.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "XIniFileReader.h"
#include "XMemory.h"

#include <cstring>
#include <cstdint>
#include <cstddef>
#include <new>

// Original data: DAT_004f5254.
// Keep unresolved until the Windows constant is recovered.
extern const float DAT_004f5254;

// Original data: DAT_00500818.
// Keep unresolved until the Windows format string is recovered.
extern const char DAT_00500818[];

// Original data: DAT_004ff054.
// Preserve the original path fragment until its bytes are recovered.
extern const char DAT_004ff054[];

// Original data: DAT_004fe238.
// Format used when a session event has no player name.
extern const char DAT_004fe238[];

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

        if (FUN_004059a1() && currentScreen == 14)
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

        FUN_004d9605(this);

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

            const std::uint32_t textHeight = FUN_004397a6(font, text);
            const std::uint32_t y = readWord(bitmap, 0x14) - textHeight - 1u;

            const std::uint32_t textWidth = FUN_0043976c(font, text);
            const std::uint32_t x = readWord(bitmap, 0x10) - textWidth - 1u;

            FUN_0040ed72(bitmap, font, colour, NXBasics::Runtime_TimeSigned32(x), NXBasics::Runtime_TimeSigned32(y), text);
        }
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
                    const auto callsign = FUN_004d7389(this);

                    configuration->Property_Add("multiplayer_callsign", false, 0, callsign);
                    FUN_004e1816(NMasterPropertyManager::CPropertyManager::sTheObjectPtr);
                }

                FUN_004a3ae8(screenTimer);
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

                FUN_004a3ae8(screenTimer);
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
                FUN_004a3a8e(screenTimer, FUN_00405a87() - 2, false, true, false, false);
                FUN_004a3cf6(screenTimer, FUN_004059e3());
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
                FUN_004a3a8e(screenTimer, 100, false, true, false, false);
                break;

            case 14:
                {
                    FUN_004a3a8e(screenTimer, 60, true, true, false, false);

                    std::memset(lobbyData, 0, sizeof(lobbyData));
                    lobbyValue = 0;

                    if (!FUN_004059a1())
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

                    if (FUN_004059a1() && DAT_005114b4 != nullptr)
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

                    if (FUN_004059a1())
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

                            FUN_0042f88a(&temporaryFileData, sourceFileName);

                            {
                                NXBasics::CFile outputFile(transferFileName, false);

                                outputFile.OpenForWriting(true);
                                outputFile.Write(temporaryFileData.data, temporaryFileData.byteCount);
                            }

                            FUN_0042f8f4(&temporaryFileData);

                            transferFlags[1] = 1;

                            FUN_004161a2(DAT_005114ac, "sys_continue", transferFileName);
                        }

                        // Keep compressor storage separate from the completed CMemory lifetime.
                        std::uint32_t compressor[3];
                        static_assert(sizeof(compressor) == 0x0C);

                        FUN_004e0b3a(compressor);

                        const std::int32_t compressionResult = FUN_004e0b4e(compressor, transferFileName);

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
                    transferIsHost = static_cast<std::uint8_t>(FUN_004059a1());

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

                FUN_004dec60(&DAT_0050f6a8, &chatBuffer, &chatCount);

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
                        FUN_004d7ca9(this, 0, text);
                        FUN_004e2bcf();
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
                        FUN_004d7ca9(this, 1, text);
                }

                NNetwork::ClearDynamicNetworkEvents(&DAT_0050f6a8);
            }
        }

        const std::uint32_t networkFlags = FUN_00405999();

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

        if (FUN_004059a1())
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
            if (transferIsHost != static_cast<std::uint8_t>(FUN_004059a1()))
            {
                if (FUN_004059a1())
                    FUN_004d8169(this);

                transferIsHost = static_cast<std::uint8_t>(FUN_004059a1());
            }
        }

        if (currentScreen == 15 && FUN_004059a1())
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
                        if (!FUN_004059a1())
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

                            const std::uint32_t localPlayerId = FUN_004059cb();
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
                    if (!FUN_004059a1())
                        Screen_ChangeTo(0x10, false);

                    break;

                case 5:
                    if (!FUN_004059a1())
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
            const std::uint8_t isHost = static_cast<std::uint8_t>(FUN_004059a1());

            if (transferIsHost != isHost)
            {
                Screen_ChangeTo(18, true);
                Debug_ErrorString_Add("Host changed while transfering savegame");
                return;
            }

            // Preserve the second host query performed by the original.
            if (FUN_004059a1())
                l_Transfer_UpdateUpload(false);
            else
                l_Transfer_UpdateDownload(false);
        }

        // Re-read the screen after the savegame branch.
        if (currentScreen == 17)
        {
            const std::uint8_t isHost = static_cast<std::uint8_t>(FUN_004059a1());

            if (transferIsHost != isHost)
            {
                Screen_ChangeTo(18, true);
                Debug_ErrorString_Add("Host changed while transfering user map");
                return;
            }

            if (FUN_004059a1())
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

        // Original function: FUN_004e0b3a.
        // The compressor constructor clears exactly twelve bytes.
        std::uint32_t compressor[3] = {};

        if (!userMap)
        {
            FUN_004161a2(DAT_005114ac, "sys_continue", compressedFileName);
            std::strcat(compressedFileName, ".c2p");

            const std::int32_t result = FUN_004e0d5c(compressor, compressedFileName);

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

        const std::int32_t result = FUN_004e0d5c(compressor, compressedFileName);

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
}
