#include "InGameGuiManager.h"
#include "InGameGuiSupportAccess.h"
#include "InGameGuiMissionInfoWindow.h"
#include "InGameGuiWindowState.h"
#include "C2EngineDisplayAccess.h"
#include "C2GameState.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "NetworkShutdown.h"

#include <cstring>

namespace NC2InGameGuiMiscWindows
{
    // Original function: FUN_004b1e0f.
    static void StatisticsWindow_IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x58737461, 1);
        io->IO_Memory_WriteLong(NC2Lifetime::Read<std::uint32_t>(gStatisticsWindowStatics, 0));
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004b1e38.
    static void StatisticsWindow_IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == 0x58737461 && io->mReadHeader.version == 1 && !NNetwork::Network_IsInSession()) NC2Lifetime::Write(gStatisticsWindowStatics, 0, io->IO_File_ReadLong());
        else { StatisticsWindow_Statics_Init(); io->IO_File_Chunk_Skip(); }
    }

    // Original function: FUN_004b00f0.
    static void HumanListWindow_IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x5868756C, 1);
        io->IO_Memory_WriteBinary(gHumanListWindowStatics, 0x18);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004b011a.
    static void HumanListWindow_IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == 0x5868756C && io->mReadHeader.version == 1 && !NNetwork::Network_IsInSession()) io->IO_File_ReadBinary(gHumanListWindowStatics, 0x18);
        else { HumanListWindow_Statics_Init(); io->IO_File_Chunk_Skip(); }
    }

    // Original function: FUN_004af070.
    static void DiplomacyWindow_IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x58646970, 1);
        io->IO_Memory_WriteLong(NC2Lifetime::Read<std::uint32_t>(gDiplomacyWindowStatics, 0));
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004af099.
    static void DiplomacyWindow_IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == 0x58646970 && io->mReadHeader.version == 1 && !NNetwork::Network_IsInSession()) NC2Lifetime::Write(gDiplomacyWindowStatics, 0, io->IO_File_ReadLong());
        else { std::memset(gDiplomacyWindowStatics, 0, 4); io->IO_File_Chunk_Skip(); }
    }
}

namespace NC2InGameGuiMessages
{
    using namespace NC2Lifetime;

    // Original function: FUN_004b3b2e; mini-button pointers are not serialized.
    void CMessageManager::IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x586D6D61, 2);
        io->IO_Memory_WriteBinary(this, 8);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(this, 0); ++index) io->IO_Memory_WriteBinary(Read<std::byte*>(this, 0x10) + index * 0x310u, 0x30C);
        io->IO_Memory_WriteBinary(Read<void*>(this, 0x14), 0x26160);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004b3b92; discard current mini-buttons before replacing records.
    void CMessageManager::IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        while (Read<std::uint32_t>(this, 0) != 0) Message_Remove(Read<std::uint32_t>(Read<void*>(this, 0x10), 4), false);
        if (io->mReadHeader.marker != 0x586D6D61 || io->mReadHeader.version != 2) { io->IO_File_Chunk_Skip(); return; }
        io->IO_File_ReadBinary(this, 8);
        for (std::uint32_t index = 0; index < Read<std::uint32_t>(this, 0); ++index)
        {
            auto* record = Read<std::byte*>(this, 0x10) + index * 0x310u;
            io->IO_File_ReadBinary(record, 0x30C);
            Write<void*>(record, 0x30C, nullptr);
        }
        io->IO_File_ReadBinary(Read<void*>(this, 0x14), 0x26160);
    }

    // Original function: FUN_004b4481.
    void CMessageManager::MessagePriority_Set(std::uint32_t priority)
    {
        Write(this, 8, priority);
        System_Update();
    }
}

namespace NC2InGameGuiGroups
{
    // Original function: FUN_004a3251.
    void CObjectGroupManager::IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x586F676D, 2);
        io->IO_Memory_WriteBinary(this, 0x2F0C);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004a3280; network loading deliberately discards saved groups.
    void CObjectGroupManager::IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == 0x586F676D && io->mReadHeader.version == 2)
        {
            if (NC2Logic::gGameStaticVars.networkGame == 0) { io->IO_File_ReadBinary(this, 0x2F0C); return; }
            std::memset(this, 0, 0x2F0C);
        }
        io->IO_File_Chunk_Skip();
    }
}

namespace NC2InGameGuiDeadHumanDataManager
{
    // Original function: FUN_004b3818; owned buffer is serialized separately from its pointer.
    void CDeadHumanDataManager::IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x5864686D, 1);
        io->IO_Memory_WriteBinary(this, 8);
        io->IO_Memory_WriteBinary(NC2Lifetime::Read<void*>(this, 8), 0xBB8);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004b3853.
    void CDeadHumanDataManager::IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        if (io->mReadHeader.marker == 0x5864686D && io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(this, 8);
            io->IO_File_ReadBinary(NC2Lifetime::Read<void*>(this, 8), 0xBB8);
        }
        else { io->IO_File_Chunk_Skip(); std::memset(this, 0, 8); }
    }
}

namespace NInGameGuiManager
{
    using namespace NC2Lifetime;
    using namespace NC2InGameGuiMiscWindows;

    // Original function: FUN_004c9c3b; preserve child-chunk order inside XGUI.
    void CInGameGuiManager::l_IO_Savegame_Save(NC2Logic::CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x58475549, 1);
        io->IO_Tool_Chunk_StartCreation(0x58647372, 1);
        std::uint32_t mapPoint = 0;
        NC2E2::DE_Misc_GetDisplayedCenterMapPosition(Read<void*>(sTheObjectPtr->GetDesktop(), 0x30), &mapPoint);
        io->IO_Memory_WriteLong(mapPoint);
        io->IO_Tool_Chunk_FinishAndWrite();
        StatisticsWindow_IO_Savegame_Save(io);
        HumanListWindow_IO_Savegame_Save(io);
        DiplomacyWindow_IO_Savegame_Save(io);
        static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->IO_Savegame_Save(io);
        static_cast<NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager)->IO_Savegame_Save(io);
        static_cast<NC2InGameGuiDeadHumanDataManager::CDeadHumanDataManager*>(NC2InGameGuiDeadHumanDataManager::gDeadHumanDataManager)->IO_Savegame_Save(io);
        MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Save(io);
        io->IO_Tool_Chunk_StartCreation(0x586D6C65, 1);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(NC2InGameGuiMessages::gMessageManager, 8));
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_FinishAndWrite();
        io->IO_Tool_Chunk_WriteEndChunk();
    }

    // Original function: FUN_004c9d08; skip unrecognized children without losing following chunks.
    void CInGameGuiManager::l_IO_Savegame_Load(NC2Logic::CIoHelper* io)
    {
        std::uint32_t version = 0;
        if (!io->IO_File_Chunk_CheckMarkerAndGetVersion(0x58475549, &version, nullptr)) { io->IO_File_Chunk_Skip(); return; }
        while (io->IO_File_Chunk_ReadHeader())
        {
            switch (io->mReadHeader.marker)
            {
            case 0x58647372:
            {
                const auto point = io->IO_File_ReadLong();
                NC2E2::DE_SetWantedPositionToMapPoint(Read<void*>(sTheObjectPtr->GetDesktop(), 0x30), &point);
                break;
            }
            case 0x58737461: StatisticsWindow_IO_Savegame_Load(io); break;
            case 0x5868756C: HumanListWindow_IO_Savegame_Load(io); break;
            case 0x58646970: DiplomacyWindow_IO_Savegame_Load(io); break;
            case 0x586D6D61: static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->IO_Savegame_Load(io); break;
            case 0x586F676D: static_cast<NC2InGameGuiGroups::CObjectGroupManager*>(NC2InGameGuiGroups::gObjectGroupManager)->IO_Savegame_Load(io); break;
            case 0x5864686D: static_cast<NC2InGameGuiDeadHumanDataManager::CDeadHumanDataManager*>(NC2InGameGuiDeadHumanDataManager::gDeadHumanDataManager)->IO_Savegame_Load(io); break;
            case 0x586D6268: MissionInfoWindow_MissionBriefingHistory_IO_Savegame_Load(io); break;
            case 0x586D6C65: static_cast<NC2InGameGuiMessages::CMessageManager*>(NC2InGameGuiMessages::gMessageManager)->MessagePriority_Set(io->IO_File_ReadLong()); break;
            default: io->IO_File_Chunk_Skip(); break;
            }
        }
    }
}
