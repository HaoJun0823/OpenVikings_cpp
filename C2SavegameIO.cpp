#include "C2SavegameIO.h"
#include "C2SavegamesManager.h"
#include "C2CleanMapsManager.h"
#include "C2LifetimeGlobals.h"
#include "C2IoHelper.h"
#include "XFile.h"
#include "XMemory.h"
#include "C2SavegameStateAccess.h"
#include "C2AISavegameAccess.h"
#include "C2GameMapIO.h"
#include "C2LogicLifetimeAccess.h"
#include "C2LogicObjectArrays.h"
#include "C2CallbackManager.h"
#include "XFileAccess.h"
#include "NetworkShutdown.h"
#include <Windows.h>
#include <mmsystem.h>
#include <new>

namespace NC2Logic
{
    // Original function: FUN_00425bfb.
    std::int32_t GameIO_Savegame_GetInfoHeader(const char* filename, SSavegameInfoHeaderData* header)
    {
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename(filename, fullFilename);
        NXBasics::CFile file(fullFilename, false);
        file.OpenForReading(true, true);
        CIoHelper io;
        io.IO_File_Init(&file);
        NXBasics::CMemory memory(1000u);
        io.IO_Memory_Init(&memory);
        return l_GameIO_Savegame_DoLoad_CheckHeaderAndGetInfos(io, header);
    }

    // Original function: FUN_004264a7.
    std::int32_t l_GameIO_Savegame_DoLoad_CheckHeaderAndGetInfos(CIoHelper& io, SSavegameInfoHeaderData* header)
    {
        std::uint32_t version = 0;
        if (!io.IO_File_Chunk_CheckMarkerAndGetVersion(0x78736776, &version, nullptr))
            return -1;
        io.IO_File_Chunk_Skip();
        std::uint32_t infoVersion = 0;
        if (!io.IO_File_Chunk_CheckMarkerAndGetVersion(0x78736769, &infoVersion, nullptr))
            return -1;
        if (header == nullptr)
        {
            io.IO_File_Chunk_Skip();
            return static_cast<std::int32_t>(version);
        }
        std::memset(header, 0, sizeof(*header));
        header->version = version;
        header->slotId = 10000;
        if (infoVersion < 2)
        {
            std::strcpy(header->info, "(no infos found)");
            io.IO_File_Chunk_Skip();
            return static_cast<std::int32_t>(version);
        }
        while (io.IO_File_Chunk_ReadHeader())
        {
            switch (io.mReadHeader.marker)
            {
            case 0x49436973:
                std::strncpy(header->info, io.IO_File_ReadString(), 200);
                break;
            case 0x49436D6E:
                std::strncpy(header->mapName, io.IO_File_ReadString(), 200);
                break;
            case 0x49436367:
                io.IO_File_ReadBinary(&header->mapGuid, 16);
                break;
            case 0x49436D70:
            {
                header->multiplayer = 1;
                // Original entries end at a negative index, without an upper-bound check.
                for (std::int32_t index = static_cast<std::int32_t>(io.IO_File_ReadLong()); index >= 0; index = static_cast<std::int32_t>(io.IO_File_ReadLong()))
                {
                    auto& player = header->players[index];
                    player.value = io.IO_File_ReadLong();
                    std::strncpy(player.name, io.IO_File_ReadString(), 40);
                    player.colorId = io.IO_File_ReadLong();
                }
                break;
            }
            case 0x49437469:
                for (auto& field : header->creationTimeFields)
                    field = io.IO_File_ReadLong();
                break;
            case 0x49437369:
                header->slotId = static_cast<std::int32_t>(io.IO_File_ReadLong());
                break;
            case 0x49437878:
                return static_cast<std::int32_t>(version);
            default:
                io.IO_File_Chunk_Skip();
                break;
            }
        }
        return static_cast<std::int32_t>(version);
    }
    // Original function: FUN_004075b0.
    static void Misc_GetInternalNameAndVersionString(char* destination)
    {
        NXBasics::Runtime_PrintString(destination, "%s - Version %d.%2.2d - Build date Nov 29 2003 17:36:55", "Weltwunder", 1, 0);
    }

    // Original function: FUN_00426229.
    static void l_GameIO_Savegame_DoSave_WriteHeaderAndInfos(CIoHelper* io, const char* info, std::int32_t slotId)
    {
        io->IO_Tool_Chunk_StartCreation(0x78736776, 1);
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x78736769, 2);
        io->IO_Tool_Chunk_StartCreation(0x49436973, 1);
        io->IO_Memory_WriteString(info != nullptr && std::strlen(info) != 0 ? info : "(no infos)");
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x49436D6E, 1);
        char mapName[220];
        mapName[0] = '\0';
        if (auto* map = gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(gGameStaticVars.mapGuid)) map->Tool_GetMapName(mapName);
        io->IO_Memory_WriteString(std::strlen(mapName) != 0 ? mapName : "");
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x49437369, 1);
        io->IO_Memory_WriteLong(static_cast<std::uint32_t>(slotId));
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x4943656E, 1);
        char version[96];
        Misc_GetInternalNameAndVersionString(version);
        io->IO_Memory_WriteString(version);
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x49437469, 1);
        SYSTEMTIME time;
        ::GetLocalTime(&time);
        for (auto value : {time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds}) io->IO_Memory_WriteLong(value);
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_StartCreation(0x49436367, 1);
        io->IO_Memory_WriteBinary(&gGameStaticVars.mapGuid, 16);
        io->IO_Tool_Chunk_FinishAndWrite();
        if (gGameStaticVars.networkGame != 0)
        {
            io->IO_Tool_Chunk_StartCreation(0x49436D70, 1);
            for (std::uint32_t player = 0; player < 20; ++player)
            {
                const auto offset = player * 0x4A80;
                if (NC2Lifetime::Read<std::uint8_t>(gGameControl, offset) == 0) break;
                const auto type = NC2Lifetime::Read<std::uint32_t>(gGameControl, offset + 4);
                const auto color = NC2Lifetime::Read<std::uint32_t>(gGameControl, offset + 0x10);
                char name[216];
                name[0] = '\0';
                if (type == 1 && !NNetwork::Network_GetPlayerName(NC2Lifetime::Read<std::uint32_t>(gGameControl, offset + 0x14), name)) name[0] = '\0';
                io->IO_Memory_WriteLong(player);
                io->IO_Memory_WriteLong(type);
                io->IO_Memory_WriteString(name);
                io->IO_Memory_WriteLong(color);
            }
            io->IO_Memory_WriteLong(0xFFFFFFFF);
            io->IO_Tool_Chunk_FinishAndWrite();
        }
        io->IO_Tool_Chunk_StartCreation(0x49437878, 1);
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00425c9d.
    static void l_GameIO_Savegame_DoSave(CIoHelper* io, const char* info, std::int32_t slotId)
    {
        l_GameIO_Savegame_DoSave_WriteHeaderAndInfos(io, info, slotId);
        io->IO_Tool_Chunk_StartCreation(0x7867616D, 1);
        io->IO_Memory_WriteBinary(&gGameStaticVars.mapGuid, 0x98);
        io->IO_Tool_Chunk_FinishAndWrite();
        GameControl_IO_Savegame_DoSave(gGameControl, nullptr, io);
        GameMap_IO_Savegame_DoSave(gGameMap, nullptr, io);
        MissionManager_IO_Savegame_DoSave(gMissionManager, nullptr, io);
        AIManager_IO_Savegame_DoSave(gAIManager, nullptr, io);
        MapMoveableArray_IO_Savegame_DoSave(gMapMoveableArrayStorage.manager, nullptr, io);
        MapPathfinderManager_IO_Savegame_DoSave(gMapPathfinderManager, nullptr, io);
        CHumanArray::sTheObjectPtr->IO_Savegame_DoSave(io);
        HumanMerchantArray_IO_Savegame_DoSave(gHumanMerchantArray, nullptr, io);
        CAnimalArray::sTheObjectPtr->IO_Savegame_DoSave(io);
        CVehicleArray::sTheObjectPtr->IO_Savegame_DoSave(io);
        MarkerArray_IO_Savegame_DoSave(gMarkerArray, nullptr, io);
        CHouseArray::sTheObjectPtr->IO_Savegame_DoSave(io);
        GuideManager_IO_Savegame_DoSave(gGuideManager, nullptr, io);
        PlayerStatisticManager_IO_Savegame_DoSave(gPlayerStatisticManager, nullptr, io);
        DelayedActionManager_IO_Savegame_DoSave(gDelayedActionManager, nullptr, io);
        TributeManager_IO_Savegame_DoSave(gTributeManager, nullptr, io);
        EmbeddedSavegameManager_IO_Savegame_DoSave(gEmbeddedSavegameManager, nullptr, io);
        HappinessManager_IO_Savegame_DoSave(gHappinessManager, nullptr, io);
        MultiPlayerGoalManager_IO_Savegame_DoSave(gMultiPlayerGoalManager, nullptr, io);
        GameAssistantManager_IO_Savegame_DoSave(gGameAssistantManager, nullptr, io);
        MissionOnscreenInfoManager_IO_Savegame_DoSave(gMissionOnscreenInfoManager, nullptr, io);
        io->IO_Tool_Chunk_WriteEndChunk();
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x14, false, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(io)), 0, 0);
    }

    // Original function: FUN_00425a7b; stack owners destruct in the Windows order.
    static void l_GameIO_Savegame_DoSave(const char* fullFilename, const char* info, std::int32_t slotId)
    {
        CIoHelper io;
        NXBasics::CFile file(fullFilename, false);
        file.OpenForWriting(true);
        io.IO_File_Init(&file);
        NXBasics::CMemory memory(10000);
        io.IO_Memory_Init(&memory);
        NXBasics::CMemory packing(10000);
        io.mPackingBuffer = &packing;
        l_GameIO_Savegame_DoSave(&io, info, slotId);
    }

    // Original function: FUN_00425a35.
    void GameIO_Savegame_Save(const char* filename, const char* info, std::int32_t slotId)
    {
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename(filename, fullFilename);
        NXBasics::CFile::FileSystem_Tool_CreateDirectory(gSavegamesManager->Tool_GetCurrentPlayerSavegameDirectoryNamePtr());
        l_GameIO_Savegame_DoSave(fullFilename, info, slotId);
    }

    namespace
    {
        template<class T> void LoadNativeArray(CIoHelper* io)
        {
            if (void* storage = NXBasics::Runtime_AllocateWithNewHandler(sizeof(T), 1)) ::new (storage) T(io);
        }
        void LoadOriginalManager(std::uint32_t size, void* (__fastcall* constructor)(void*, void*, CIoHelper*), CIoHelper* io)
        {
            if (void* storage = NXBasics::Runtime_AllocateWithNewHandler(size, 1)) constructor(storage, nullptr, io);
        }
    }

    // Original function: FUN_00425df1.
    static void l_GameIO_Savegame_DoLoad(CIoHelper* io, bool network)
    {
        if (l_GameIO_Savegame_DoLoad_CheckHeaderAndGetInfos(*io, nullptr) == -1) return;
        Game_ShutDown();
        while (io->IO_File_Chunk_ReadHeader())
        {
            switch (io->mReadHeader.marker)
            {
            case 0x7832616D: LoadOriginalManager(0x460, GameAssistantManager_ConstructFromSavegame, io); break;
            case 0x78646161: LoadNativeArray<CAnimalArray>(io); break;
            case 0x7864616D: LoadOriginalManager(0x34, DelayedActionManager_ConstructFromSavegame, io); break;
            case 0x78646178: LoadOriginalManager(0xA0, AIManager_ConstructFromSavegame, io); break;
            case 0x78646573: LoadOriginalManager(8, EmbeddedSavegameManager_ConstructFromSavegame, io); break;
            case 0x78646778: LoadOriginalManager(0x1368, GuideManager_ConstructFromSavegame, io); break;
            case 0x78646861: LoadNativeArray<CHouseArray>(io); break;
            case 0x78646870: LoadOriginalManager(0xAA0, HappinessManager_ConstructFromSavegame, io); break;
            case 0x78646875: LoadNativeArray<CHumanArray>(io); break;
            case 0x78646D61: LoadOriginalManager(0x5D8, HumanMerchantArray_ConstructFromSavegame, io); break;
            case 0x78646D6D: LoadOriginalManager(0x30, MapMoveableArray_ConstructFromSavegame, io); break;
            case 0x78646D78: LoadOriginalManager(0x81CAC, MissionManager_ConstructFromSavegame, io); break;
            case 0x78646E61: LoadOriginalManager(0x30, MarkerArray_ConstructFromSavegame, io); break;
            case 0x7864706D: LoadOriginalManager(0x30, MapPathfinderManager_ConstructFromSavegame, io); break;
            case 0x78647478: LoadOriginalManager(0x960, TributeManager_ConstructFromSavegame, io); break;
            case 0x78647661: LoadNativeArray<CVehicleArray>(io); break;
            case 0x7867616D:
            {
                io->IO_File_ReadBinary(&gGameStaticVars.mapGuid, 0x98);
                gGameStaticVars.networkGame = static_cast<std::uint8_t>(network);
                auto* map = gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(gGameStaticVars.mapGuid);
                if (map != nullptr && map->userMap != 0)
                {
                    void* library = NXBasics::Runtime_AllocateWithNewHandler(0x14, 1);
                    if (library != nullptr) library = NXBasics::Archive_Initialize(library, map->archiveFilename);
                    gGameStaticVars.mapFileLibrary = library;
                    NXBasics::CFile::FileSystem_FileLibrary_Add(library);
                }
                break;
            }
            case 0x7867636F: LoadOriginalManager(0x5D204, GameControl_ConstructFromSavegame, io); break;
            case 0x78676D61: LoadOriginalManager(1, GameMap_ConstructFromSavegame, io); break;
            case 0x786D7367: LoadOriginalManager(0x740, MultiPlayerGoalManager_ConstructFromSavegame, io); break;
            case 0x786F6D69: LoadOriginalManager(0xFA0, MissionOnscreenInfoManager_ConstructFromSavegame, io); break;
            case 0x7870736D: LoadOriginalManager(0xA8, PlayerStatisticManager_ConstructFromSavegame, io); break;
            default: io->IO_File_Chunk_Skip(); break;
            }
        }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x15, false, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(io)), 0, 0);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x16, false, 0, 0, 0);
        const auto now = ::timeGetTime();
        gGameInitialized = 1;
        gGameTime.lastRealTime = static_cast<double>(now);
    }

    // Original function: FUN_00425b68.
    static void GameIO_Savegame_Load_Do(const char* fullFilename, bool network)
    {
        if (!NXBasics::CFile::FileSystem_Tool_FileExists(fullFilename, false)) return;
        NXBasics::CFile file(fullFilename, false);
        file.OpenForReading(true, true);
        CIoHelper io;
        io.IO_File_Init(&file);
        NXBasics::CMemory memory(10000);
        io.IO_Memory_Init(&memory);
        l_GameIO_Savegame_DoLoad(&io, network);
    }

    // Original function: FUN_00425b1a.
    void GameIO_Savegame_Load(const char* filename, bool network)
    {
        gGameStaticVars.unknown0A0[0] = std::byte{0};
        gSavegamesManager->SavegameList_Update(network ? 2 : 1, false);
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename(filename, fullFilename);
        GameIO_Savegame_Load_Do(fullFilename, network);
    }

    // Original function: FUN_004075cb.
    void System_RestartCurrentMap()
    {
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2E, true, 0, 0, 0);
        const bool embedded = EmbeddedSavegameManager_Savegame_RestoreEmbedded(gEmbeddedSavegameManager, "sys_temp");
        if (auto* map = gCleanMapsManager->CleanMapList_GetInfoPtrByGuid(gGameStaticVars.mapGuid))
        {
            if (map->archiveFilename[0] == '\0') GameIO_Cleanmap_Load(map->mapRoot, true, true, false);
            else GameIO_Cleanmap_LoadX(nullptr, map->archiveFilename, true, true, false);
        }
        if (embedded)
        {
            EmbeddedSavegameManager_Savegame_DoEmbed(gEmbeddedSavegameManager, "sys_temp");
            char fullFilename[260];
            gSavegamesManager->Tool_CreateFullSavegameFilename("sys_temp", fullFilename);
            NXBasics::Runtime_RemoveFile(fullFilename);
        }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2D, true, 0, 0, 0);
    }

    // Original function: FUN_0040768a; save/load return values are not checked by Windows.
    bool SubMap_StartSubMap(std::uint32_t campaignId, std::uint32_t mapId, std::uint32_t pathfinderLimitOverride)
    {
        auto* map = gCleanMapsManager->CleanMapList_GetCampaignMapInfoPtr(campaignId, mapId);
        if (map == nullptr) return false;
        GameIO_Savegame_Save("sys_temp", nullptr, 8000);
        gGameControlRuntimeStorage.pathfinderLimitOverridePending = 1;
        gGameControlRuntimeStorage.pathfinderLimitOverride = pathfinderLimitOverride;
        GameIO_Cleanmap_Load(map->mapRoot, true, true, false);
        // Loading replaces the embedded-savegame manager; obtain its current owner now.
        const bool embedded = EmbeddedSavegameManager_Savegame_DoEmbed(gEmbeddedSavegameManager, "sys_temp");
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename("sys_temp", fullFilename);
        NXBasics::Runtime_RemoveFile(fullFilename);
        if (embedded) CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2B, true, 0, 0, 0);
        return embedded;
    }

    // Original function: FUN_0040773a; only an absent embedded save returns false.
    bool SubMap_BackToParent()
    {
        if (!EmbeddedSavegameManager_Savegame_RestoreEmbedded(gEmbeddedSavegameManager, "sys_temp")) return false;
        GameIO_Savegame_Load("sys_temp", false);
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename("sys_temp", fullFilename);
        NXBasics::Runtime_RemoveFile(fullFilename);
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x2C, true, 0, 0, 0);
        return true;
    }

}
