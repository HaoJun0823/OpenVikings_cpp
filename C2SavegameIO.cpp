#include "C2SavegameIO.h"
#include "C2SavegamesManager.h"
#include "C2LifetimeGlobals.h"
#include "C2IoHelper.h"
#include "XFile.h"
#include "XMemory.h"

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
                    player.unknown30 = io.IO_File_ReadLong();
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
}
