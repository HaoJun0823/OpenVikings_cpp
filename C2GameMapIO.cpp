#include "C2GameMapIO.h"
#include "C2CallbackManager.h"
#include "C2CleanMapsManager.h"
#include "C2GameEngineStartup.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "XFileAccess.h"
#include "XIniFileReader.h"

#include <cstring>
#include <new>

namespace NC2Logic
{
    using namespace NC2Lifetime;

    // Original map-chunk dispatcher and script-manager bodies remain unresolved.
    extern bool FUN_0042fbf4(CIoHelper*);
    extern void __fastcall FUN_0041d09b(void*, void* unusedEdx, NXBasics::CIniFileReader*);
    extern void __fastcall FUN_0040822e(void*, void* unusedEdx, NXBasics::CIniFileReader*);
    extern void __fastcall FUN_0041cb1e(void*, void* unusedEdx, NXBasics::CIniFileReader*);
    extern void __fastcall FUN_0041910b(void*, void* unusedEdx, NXBasics::CIniFileReader*);

    namespace
    {
        void ReleaseIni(NXBasics::CIniFileReader* reader)
        {
            if (reader != nullptr)
            {
                reader->~CIniFileReader();
                NXBasics::Runtime_Free(reader);
            }
        }

        void ReleaseLibrary(void* library)
        {
            if (library != nullptr)
            {
                NXBasics::Archive_Destroy(library);
                NXBasics::Runtime_Free(library);
            }
        }
    }

    // Original function: FUN_00407586.
    void CleanMapGuid_InitWhenNotSet()
    {
        if (!gGameStaticVars.mapGuid.IsValid())
        {
            CCleanMapsManager::Tool_GenerateNewGuid(&gGameStaticVars.mapGuid);
            gCleanMapsManager->CleanMapList_Update();
        }
    }

    // Original function: FUN_0040a68b; distinct directory-loading entry.
    void GameIO_Cleanmap_Load(const char* mapRoot, bool enableTime, bool retainIni, bool networkGame)
    {
        GameIO_Cleanmap_LoadX(mapRoot, nullptr, enableTime, retainIni, networkGame);
    }

    // Original function: FUN_0040a6a6.
    bool GameIO_Cleanmap_LoadX(const char* mapRoot, const char* libraryFilename, bool enableTime, bool retainIni, bool networkGame)
    {
        const bool libraryMode = libraryFilename != nullptr;
        const bool rootMode = !libraryMode && mapRoot != nullptr;
        if (!libraryMode && !rootMode) return false;

        NXBasics::CIniFileReader* reader = nullptr;
        void* library = nullptr;
        if (rootMode)
        {
            NXBasics::CFile::FileSystem_SystemPathName_Set("$maproot$", mapRoot);
            if (void* storage = NXBasics::Runtime_AllocateWithNewHandler(0x1890, 1))
                reader = ::new (storage) NXBasics::CIniFileReader("$maproot$\\map.ini", nullptr, false, nullptr, nullptr);
        }
        if (libraryMode)
        {
            if (void* storage = NXBasics::Runtime_AllocateWithNewHandler(0x14, 1))
                library = NXBasics::Archive_Initialize(storage, libraryFilename);
            NXBasics::CFile::FileSystem_FileLibrary_Add(library);
            if (void* storage = NXBasics::Runtime_AllocateWithNewHandler(0x1890, 1))
                reader = ::new (storage) NXBasics::CIniFileReader("currentusermap\\map.ini", nullptr, false, nullptr, nullptr);
        }

        // Windows dereferences the result even after a failed reader allocation.
        if (!reader->IsLoaded())
        {
            ReleaseIni(reader);
            ReleaseLibrary(library);
            return false;
        }

        if (reader->SearchGroup("logiccontrol") && reader->SearchCommand("version")) reader->GetInt();
        if (!reader->SearchGroup("logiccontrol") || !reader->SearchCommand("mapsize"))
        {
            ReleaseIni(reader);
            ReleaseLibrary(library);
            return false;
        }
        const std::int32_t width = reader->GetInt();
        const std::int32_t height = reader->GetInt();
        Game_StartUp(static_cast<std::int16_t>(width), static_cast<std::int16_t>(height), false, reader, networkGame);

        if (reader->SearchGroup("logiccontrol") && reader->SearchCommand("mapguid"))
            for (std::uint32_t index = 0; index < 16; ++index) gGameStaticVars.mapGuid.bytes[index] = static_cast<std::uint8_t>(reader->GetInt());
        CleanMapGuid_InitWhenNotSet();
        if (reader->SearchGroup("misc_maptype") && reader->SearchCommand("maptype")) Write<std::int32_t>(&gGameStaticVars, 0x18, reader->GetInt());
        if (retainIni)
        {
            if (mapRoot != nullptr) std::strcpy(reinterpret_cast<char*>(&gGameStaticVars) + 0xA0, mapRoot);
            DAT_005109bc = reader;
        }

        {
            char mapDataFilename[260];
            mapDataFilename[0] = '\0';
            std::strcpy(mapDataFilename, rootMode ? "$maproot$\\map.dat" : "currentusermap\\map.dat");
            NXBasics::CFile file(mapDataFilename, true);
            CIoHelper io;
            io.IO_File_Init(&file);
            bool loaded = io.IO_File_Chunk_CheckMarkerAndVersion(0x6C6F6769, 0) && GameMap_IO_Cleanmap_Load(&io);
            if (loaded)
            {
                CCallbackManager::sTheObjectPtr->ExecuteCallback(0x13, false, static_cast<std::uint32_t>(reinterpret_cast<std::uintptr_t>(&io)), 0, 0);
                loaded = io.IO_File_Chunk_CheckMarkerAndVersion(0x74656E64, 0);
            }
            if (!loaded)
            {
                // Free these before the stack I/O helper and file, as in Windows.
                ReleaseIni(reader);
                DAT_005109bc = nullptr;
                ReleaseLibrary(library);
                return false;
            }
        }

        if (reader != nullptr)
        {
            FUN_0041d09b(gMissionManager, nullptr, reader);
            FUN_0040822e(gAIManager, nullptr, reader);
            FUN_0041cb1e(gHumanMerchantArray, nullptr, reader);
            FUN_0041910b(gMultiPlayerGoalManager, nullptr, reader);
        }
        CCallbackManager::sTheObjectPtr->ExecuteCallback(0x16, false, 1, 0, 0);
        if (rootMode) NXBasics::CFile::FileSystem_SystemPathName_Clear("$maproot$");
        if (libraryMode) DAT_005109c0 = library;
        if (reader != DAT_005109bc) ReleaseIni(reader);
        if (enableTime) GameTime_Reset(true);
        return true;
    }

    // Original function: FUN_0042fbcc; validates the map envelope before dispatch.
    bool GameMap_IO_Cleanmap_Load(CIoHelper* io)
    {
        std::uint32_t version;
        if (!io->IO_File_Chunk_CheckMarkerAndGetVersion(0x6C676D6D, &version, nullptr)) return false;
        return FUN_0042fbf4(io);
    }
}
