#include "C2SavegameStateAccess.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2LogicObjectArrays.h"
#include "C2LogicLifetimeAccess.h"
#include "C2GameControlStartup.h"
#include "C2CallbackManager.h"
#include "C2MapSectorGridCursor.h"
#include "C2SavegamesManager.h"
#include "XFile.h"
#include <new>
namespace NC2Logic
{
    using namespace NC2Lifetime;
    // Windows member-construction tree: 0041CF8D and 0041CFB0.
    static void MissionManager_ConstructMembers(void* manager)
    {
        for (std::uint32_t index = 0; index < 150; ++index)
        {
            auto* record = static_cast<std::byte*>(manager) + 0x40 + index * 0xDD8;
            std::memset(record + 0x18, 0, 30 * 0x28);
            std::memset(record + 0x4CC, 0, 30 * 0x4C);
        }
    }
    extern bool __cdecl AIManager_l_AI_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl GameAssistantManager_ls_LogicCallback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl HappinessManager_l_Happiness_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl HumanMerchantArray_l_LogicCallback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl MissionManager_l_Mission_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    extern bool __cdecl MultiPlayerGoalManager_l_LogicCallback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);

    // Original function: FUN_00409aba.
    void __fastcall GameControl_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x7867636F, 1);
        io->IO_Memory_WriteBinary(manager, 0x5D204);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00409ae9.
    void* __fastcall GameControl_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gGameControl = manager;
        std::memset(manager, 0, 0x5D204);
        if (io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(manager, 0x5D204);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x28, GameControl_ls_LogicCallback, 0x0, 10);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_0041d00c.
    void __fastcall MissionManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646D78, 2);
        io->IO_Memory_WriteBinary(manager, 0x81CAC);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0041d03b.
    void* __fastcall MissionManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        // Windows constructs the mission records before clearing the entire object.
        MissionManager_ConstructMembers(manager);
        gMissionManager = manager;
        std::memset(manager, 0, 0x81CAC);
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x1, MissionManager_l_Mission_Callback, 0xA, 1);
        if (io->mReadHeader.version == 2)
        {
            io->IO_File_ReadBinary(manager, 0x81CAC);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_004171aa.
    void __fastcall MissionOnscreenInfoManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x786F6D69, 1);
        io->IO_Memory_WriteBinary(manager, 0xFA0);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00417170.
    void* __fastcall MissionOnscreenInfoManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gMissionOnscreenInfoManager = manager;
        std::memset(manager, 0, 0xFA0);
        if (io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(manager, 0xFA0);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_0041768b.
    void __fastcall GameAssistantManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x7832616D, 1);
        io->IO_Memory_WriteBinary(manager, 0x460);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0041763b.
    void* __fastcall GameAssistantManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gGameAssistantManager = manager;
        std::memset(manager, 0, 0x460);
        if (io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(manager, 0x460);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x1, GameAssistantManager_ls_LogicCallback, 0xC, 1);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_00419081.
    void __fastcall MultiPlayerGoalManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x786D7367, 1);
        io->IO_Memory_WriteBinary(manager, 0x740);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_004190b0.
    void* __fastcall MultiPlayerGoalManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gMultiPlayerGoalManager = manager;
        std::memset(manager, 0, 0x740);
        if (io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(manager, 0x740);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x1, MultiPlayerGoalManager_l_LogicCallback, 0x6F, 50);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_004197db.
    void __fastcall HappinessManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646870, 1);
        io->IO_Memory_WriteBinary(manager, 0xAA0);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00419766.
    void* __fastcall HappinessManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gHappinessManager = manager;
        std::memset(manager, 0, 0xAA0);
        if (io->mReadHeader.version == 1)
        {
            io->IO_File_ReadBinary(manager, 0xAA0);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x1, HappinessManager_l_Happiness_Callback, 0xC, 1);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x27, HappinessManager_l_Happiness_Callback, 0xC, 1);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x28, HappinessManager_l_Happiness_Callback, 0xC, 1);
        }
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_00419e0e.
    void __fastcall TributeManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78647478, 2);
        io->IO_Memory_WriteBinary(manager, 0x960);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00419e3d.
    void* __fastcall TributeManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        for (std::uint32_t index = 0; index < 40; ++index) Write<std::uint8_t>(manager, index * 0x3C, 0);
        gTributeManager = manager;
        std::memset(manager, 0, 0x960);
        if (io->mReadHeader.version == 2)
        {
            io->IO_File_ReadBinary(manager, 0x960);
        }
        else if (io->mReadHeader.version == 1) io->IO_File_ReadBinary(manager, 0x258);
        else io->IO_File_Chunk_Skip();
        return manager;
    }

    // Original function: FUN_0041cf58.
    void* __fastcall MissionManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        MissionManager_ConstructMembers(manager);
        gMissionManager = manager;
        std::memset(manager, 0, 0x81CAC);
        CCallbackManager::sTheObjectPtr->RegisterCallback(1, MissionManager_l_Mission_Callback, 10, 1);
        return manager;
    }

    // Original function: FUN_0040b9d5.
    void __fastcall MarkerArray_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646E61, 1);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(manager, 4));
        static_cast<CArrayHandler*>(manager)->IO_SaveArray(io);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0040ba09.
    void* __fastcall MarkerArray_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        auto* array = ::new (manager) CArrayHandler();
        gMarkerArray = manager;
        std::memset(manager, 0, 0x30);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        const std::uint32_t capacity = io->IO_File_ReadLong();
        const std::uint32_t bytes = capacity * 0x18u;
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        gMarkerEntries = records;
        std::memset(records, 0, bytes);
        array->IO_InitAndLoadArray(records, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        return manager;
    }

    // Original function: FUN_0040bbe3.
    void __fastcall MapMoveableArray_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646D6D, 1);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(manager, 4));
        static_cast<CArrayHandler*>(manager)->IO_SaveArray(io);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0040bc17.
    void* __fastcall MapMoveableArray_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        auto* array = ::new (manager) CArrayHandler();
        gMapMoveableArrayStorage.manager = array;
        std::memset(manager, 0, 0x30);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        const std::uint32_t capacity = io->IO_File_ReadLong();
        const std::uint32_t bytes = capacity * 0x58u;
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        if (records != nullptr)
            for (std::uint32_t index = 0; NXBasics::Runtime_TimeSigned32(index) < NXBasics::Runtime_TimeSigned32(capacity); ++index)
            {
                auto* record = records + index * 0x58 + 0x10;
                Write<std::uint16_t>(record, 0x14, 0);
                Write<std::uint16_t>(record, 0x12, 0);
                Write<std::uint32_t>(record, 0x18, 0);
                Write<std::uint32_t>(record, 0x1C, 0);
            }
        gMapMoveableArrayStorage.entries = records;
        std::memset(records, 0, bytes);
        array->IO_InitAndLoadArray(records, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        return manager;
    }

    // Original function: FUN_0041ce07.
    void __fastcall MapPathfinderManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x7864706D, 1);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(manager, 4));
        static_cast<CArrayHandler*>(manager)->IO_SaveArray(io);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0041ce3b.
    void* __fastcall MapPathfinderManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        auto* array = ::new (manager) CArrayHandler();
        gMapPathfinderManager = manager;
        std::memset(manager, 0, 0x30);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        const std::uint32_t capacity = io->IO_File_ReadLong();
        const std::uint32_t bytes = capacity * 0x84u;
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        if (records != nullptr)
            for (std::uint32_t index = 0; NXBasics::Runtime_TimeSigned32(index) < NXBasics::Runtime_TimeSigned32(capacity); ++index)
            {
                auto* record = records + index * 0x84;
                for (std::uint32_t offset : {0x42u, 0x40u, 0x46u, 0x44u}) Write<std::uint16_t>(record, offset, 0);
            }
        gMapPathfinderEntries = records;
        std::memset(records, 0, bytes);
        array->IO_InitAndLoadArray(records, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        return manager;
    }

    // Original function: FUN_00419fb6.
    void __fastcall DelayedActionManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x7864616D, 1);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(manager, 4));
        static_cast<CArrayHandler*>(manager)->IO_SaveArray(io);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00419fea.
    void* __fastcall DelayedActionManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        auto* array = ::new (manager) CArrayHandler();
        gDelayedActionManager = manager;
        std::memset(manager, 0, 0x34);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        const std::uint32_t capacity = io->IO_File_ReadLong();
        const std::uint32_t bytes = capacity * 0x30u;
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        Write(manager, 0x30, records);
        std::memset(records, 0, bytes);
        array->IO_InitAndLoadArray(records, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        return manager;
    }

    // Original function: FUN_0041c464.
    void __fastcall HumanMerchantArray_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646D61, 1);
        io->IO_Memory_WriteBinary(static_cast<std::byte*>(manager) + 0x34, 0x5A4);
        io->IO_Memory_WriteLong(Read<std::uint32_t>(manager, 4));
        static_cast<CArrayHandler*>(manager)->IO_SaveArray(io);
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0041c4a8.
    void* __fastcall HumanMerchantArray_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        auto* array = ::new (manager) CArrayHandler();
        gHumanMerchantArray = manager;
        std::memset(manager, 0, 0x34);
        CCallbackManager::sTheObjectPtr->RegisterCallback(0x1A, HumanMerchantArray_l_LogicCallback, 0xDE, 50);
        if (io->mReadHeader.version != 1)
        {
            auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(0x2FDA0, 1));
            Write(manager, 0x30, records);
            std::memset(records, 0, 0x2FDA0);
            std::memset(static_cast<std::byte*>(manager) + 0x34, 0, 0x5A4);
            array->System_InitArray(1000, records, 0xC4, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
            io->IO_File_Chunk_Skip();
            return manager;
        }
        io->IO_File_ReadBinary(static_cast<std::byte*>(manager) + 0x34, 0x5A4);
        const std::uint32_t capacity = io->IO_File_ReadLong();
        const std::uint32_t bytes = capacity * 0xC4u;
        auto* records = static_cast<std::byte*>(NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
        Write(manager, 0x30, records);
        std::memset(records, 0, bytes);
        array->IO_InitAndLoadArray(records, io, ArrayTool_IsObjectAllocated, nullptr, nullptr, nullptr);
        return manager;
    }

    // Original function: FUN_0041ab24.
    void __fastcall PlayerStatisticManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x7870736D, 1);
        io->IO_Memory_WriteBinary(manager, 8);
        for (std::uint32_t table = 0; table < 2; ++table)
            for (std::uint32_t index = 0; index < 20; ++index)
            {
                void* data = Read<void*>(manager, (table == 0 ? 8 : 0x58) + index * 4);
                io->IO_Memory_WriteByte(data != nullptr);
                if (data != nullptr) io->IO_Memory_WriteBinary(data, table == 0 ? 0x2DB40 : 0x138);
            }
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_0041abc1.
    void* __fastcall PlayerStatisticManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gPlayerStatisticManager = manager;
        std::memset(manager, 0, 0xA8);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        io->IO_File_ReadBinary(manager, 8);
        for (std::uint32_t table = 0; table < 2; ++table)
            for (std::uint32_t index = 0; index < 20; ++index)
                if (io->IO_File_ReadByte() != 0)
                {
                    const std::uint32_t bytes = table == 0 ? 0x2DB40 : 0x138;
                    void* data = NXBasics::Runtime_AllocateWithNewHandler(bytes, 1);
                    Write(manager, (table == 0 ? 8 : 0x58) + index * 4, data);
                    io->IO_File_ReadBinary(data, bytes);
                }
        return manager;
    }

    // Original function: FUN_00419c00.
    void __fastcall EmbeddedSavegameManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(0x78646573, 1);
        io->IO_Memory_WriteBinary(manager, 4);
        if (Read<std::uint32_t>(manager, 0) != 0) io->IO_Memory_WriteBinary(Read<void*>(manager, 4), Read<std::uint32_t>(manager, 0));
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00419c3d.
    void* __fastcall EmbeddedSavegameManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gEmbeddedSavegameManager = manager;
        std::memset(manager, 0, 8);
        if (io->mReadHeader.version != 1) { io->IO_File_Chunk_Skip(); return manager; }
        io->IO_File_ReadBinary(manager, 4);
        const auto bytes = Read<std::uint32_t>(manager, 0);
        if (bytes != 0)
        {
            void* data = NXBasics::Runtime_AllocateWithNewHandler(bytes, 1);
            Write(manager, 4, data);
            io->IO_File_ReadBinary(data, bytes);
        }
        return manager;
    }

    // Original function: FUN_00419cc5.
    bool EmbeddedSavegameManager_Savegame_DoEmbed(void* manager, const char* filename)
    {
        if (Read<std::uint32_t>(manager, 0) != 0)
        {
            Write<std::uint32_t>(manager, 0, 0);
            NXBasics::Runtime_Free(Read<void*>(manager, 4));
            Write<void*>(manager, 4, nullptr);
        }
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename(filename, fullFilename);
        if (!NXBasics::CFile::FileSystem_Tool_FileExists(fullFilename, false)) return false;
        NXBasics::CFile file(fullFilename, true);
        const auto bytes = file.GetSize();
        Write(manager, 0, bytes);
        void* data = NXBasics::Runtime_AllocateWithNewHandler(bytes, 1);
        Write(manager, 4, data);
        file.Read(data, bytes);
        return true;
    }

    // Original function: FUN_00419d5a.
    bool EmbeddedSavegameManager_Savegame_RestoreEmbedded(void* manager, const char* filename)
    {
        if (Read<std::uint32_t>(manager, 0) == 0) return false;
        char fullFilename[260];
        gSavegamesManager->Tool_CreateFullSavegameFilename(filename, fullFilename);
        NXBasics::CFile file(fullFilename, false);
        file.OpenForWriting(true);
        file.Write(Read<void*>(manager, 4), Read<std::uint32_t>(manager, 0));
        if (Read<std::uint32_t>(manager, 0) != 0)
        {
            Write<std::uint32_t>(manager, 0, 0);
            NXBasics::Runtime_Free(Read<void*>(manager, 4));
            Write<void*>(manager, 4, nullptr);
        }
        return true;
    }
}
