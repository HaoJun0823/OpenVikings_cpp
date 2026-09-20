#include <windows.h>

#include "dplay8.h"
#include "MainMenuGuiManager.h"
#include "FileTransferManager.h"
#include "NetworkShutdown.h"
#include "XCrtFileAccess.h"
#include "XFile.h"
#include "XFileAccess.h"
#include "XIniFileReader.h"

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

namespace NC2MainMenuGuiManager
{
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
                    auto* configuration = DAT_005697ec;
                    const auto callsign = FUN_004d7389(this);

                    FUN_004e167d(configuration, "multiplayer_callsign", 0, 0, callsign);
                    FUN_004e1816(DAT_005697ec);
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

        // Original stack storage reused by the file loader and compressor.
        struct STemporaryFileData
        {
            std::uint32_t unknown00;
            std::uint32_t byteCount;
            void* data;
        };

        static_assert(sizeof(STemporaryFileData) == 0x0C);

        switch (screen)
        {
            case 9:
                if (!FUN_004d7092(this))
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
                        STemporaryFileData temporaryFileData;

                        if (screen == 17)
                        {
                            const auto* mapInformation = reinterpret_cast<const std::byte*>(FUN_00416418(DAT_005114b4, lobbyData + 4));

                            std::strcpy(sourceFileName, reinterpret_cast<const char*>(mapInformation + 0x386));
                            NXBasics::Runtime_PrintString(transferFileName, DAT_00500818, FUN_0044029d(), "transdat");

                            FUN_0042f88a(&temporaryFileData, sourceFileName);

                            {
                                NXBasics::CFile outputFile(transferFileName, false);

                                outputFile.OpenForWriting(true);
                                outputFile.Write(temporaryFileData.data, temporaryFileData.byteCount);
                            }

                            FUN_0042f8f4(&temporaryFileData);

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

                        FUN_004e0b3a(&temporaryFileData);

                        const std::int32_t compressionResult = FUN_004e0b4e(&temporaryFileData, transferFileName);

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
                    const auto session = FUN_004e1658(DAT_005697ec, "multiplayer_auto_join");

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
}