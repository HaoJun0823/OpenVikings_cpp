#include "C2RuntimeConstants.h"
#include "C2ObjectLayout.h"
#include "XNumericConstants.h"
#include "SwitchIdentifiers.h"
#include "C2AISavegameAccess.h"
#include "C2IoHelper.h"
#include "C2LifetimeAccess.h"
#include "C2LifetimeGlobals.h"
#include "C2CallbackManager.h"
#include "C2MapSectorGridCursor.h"
#include "C2OverviewObjectIterators.h"
#include "C2LogicObjectArrays.h"
#include "C2AIConstruction.h"
#include "C2AIGuideAccess.h"
#include "XFile.h"
namespace NC2Logic
{
    using namespace NC2Lifetime;
    extern bool __cdecl AIManager_l_AI_Callback(TCallbackTypes, std::uint32_t, std::uint32_t, std::uint32_t, std::uint32_t);
    // Original function: FUN_0042a5de; an absent player handler leaves the stored array untouched.
    void AIManager_EntryPoint_RebuildHouseArray(void* manager, std::int32_t player)
    {
        void* handler = Read<void*>(manager, static_cast<std::uint32_t>(player) * 4u);
        if (handler != nullptr) AISinglePlayerHandler_EntryPoint_RecalculateHouseArray(handler);
    }

    // Original function: FUN_0044e35e; append missing positions, retaining existing entries and count.
    void AISinglePlayerHandler_EntryPoint_RecalculateHouseArray(void* handler)
    {
        CHouseIterator iterator;
        for (auto id = iterator.GetFirstHouseId(Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::PlayerId)); id != -1; id = iterator.GetNextHouseId())
        {
            const auto count = Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::BuildHouseCount);
            if (count == 100) break;
            const void* house = CHouseArray::mStaticVars + static_cast<std::uint32_t>(id) * CHouseArray::ElementSize;
            const auto point = Read<std::uint32_t>(house, NC2Layout::HouseOffset::MapPosition);
            std::uint32_t index = 0;
            while (index < count && Read<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::BuildHousePositionsUnsigned + index * 16u) != point) ++index;
            if (index != count) continue;
            Write(handler, NC2Layout::AISinglePlayerOffset::BuildHouseTypesUnsigned + count * 16u, Read<std::uint32_t>(house, NC2Layout::HouseOffset::EngineType));
            Write(handler, NC2Layout::AISinglePlayerOffset::BuildHousePositionsUnsigned + count * 16u, point);
            Write(handler, NC2Layout::AISinglePlayerOffset::BuildHouseStagesUnsigned + count * 16u, Read<std::uint32_t>(house, NC2Layout::HouseOffset::DevelopmentStage));
            Write(handler, NC2Layout::AISinglePlayerOffset::BuildHouseMissionCheckIdsUnsigned + count * 16u, Read<std::uint32_t>(house, NC2Layout::HouseOffset::MissionCheckId));
            Write(handler, NC2Layout::AISinglePlayerOffset::BuildHouseCount, count + 1u);
        }
    }

    namespace
    {
        // Original function: FUN_0042955d; serialized fields exclude the owned calculation grid.
        void AIGuideManager_Init(void* guides, CIoHelper* io)
        {
            io->IO_File_ReadBinary(guides, NC2Runtime::AIGuideLimits::SerializedBytes);
            std::memset(static_cast<std::byte*>(guides) + NC2Layout::AIGuideManagerOffset::CalculationGrid, 0, NC2Layout::RecordSize::AIGuideGrid);
        }

        // Original function: FUN_00429580.
        void AIGuideManager_IO_Savegame_WriteBinary(void* guides, CIoHelper* io)
        {
            io->IO_Memory_WriteBinary(guides, NC2Runtime::AIGuideLimits::SerializedBytes);
        }

        // Original function: FUN_00428a97.
        void* C2AIHighLevelHandler_ConstructFromSavegame(void* handler, CIoHelper* io)
        {
            AIHighLevelHandler_Vars_Construct(handler);
            AIGuideManager_Construct(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager);
            std::memset(handler, 0, 43544);
            auto* guides = static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager;
            std::memset(guides, 0, NC2Layout::RecordSize::AIGuideManager);
            io->IO_File_ReadBinary(handler, NC2Layout::AIHighLevelOffset::GuideManager);
            AIGuideManager_Init(guides, io);
            return handler;
        }

        // Original function: FUN_0042894d.
        void* C2AIHighLevelHandler_Construct(void* handler)
        {
            AIHighLevelHandler_Vars_Construct(handler);
            AIGuideManager_Construct(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager);
            std::memset(handler, 0, 43544);
            std::memset(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager, 0, NC2Layout::RecordSize::AIGuideManager);
            return handler;
        }

    }
    // Original function: FUN_004295c2.
    void AIGuideManager_CalcVars_CreateGuideCalcArray(void* grid)
    {
        if (Read<std::int32_t>(grid, NC2Layout::AIGuideGridOffset::Width) == SSectorGridCursor::sGridWidth && Read<std::int32_t>(grid, NC2Layout::AIGuideGridOffset::Height) == SSectorGridCursor::sGridHeight) return;
        if (Read<void*>(grid, NC2Layout::AIGuideGridOffset::Cells) != nullptr)
        {
            NXBasics::Runtime_Free(Read<void*>(grid, NC2Layout::AIGuideGridOffset::Cells));
            Write<void*>(grid, NC2Layout::AIGuideGridOffset::Cells, nullptr);
        }
        Write(grid, NC2Layout::AIGuideGridOffset::Height, SSectorGridCursor::sGridHeight);
        Write(grid, NC2Layout::AIGuideGridOffset::Width, SSectorGridCursor::sGridWidth);
        const auto bytes = static_cast<std::uint32_t>(SSectorGridCursor::sGridWidth) * static_cast<std::uint32_t>(SSectorGridCursor::sGridHeight) * NC2Layout::RecordSize::AIGuideGridCellUnsigned;
        Write(grid, NC2Layout::AIGuideGridOffset::Cells, NXBasics::Runtime_AllocateWithNewHandler(bytes, 1));
    }

    namespace
    {
        // Original function: FUN_0042958f.
        void AIGuideManager_Init(void* guides)
        {
            Write<std::uint32_t>(guides, NC2Layout::AIGuideManagerOffset::ScoutHumanId, NXBasics::IntegerRange::UInt32Maximum);
            Write<std::uint8_t>(guides, NC2Layout::AIGuideManagerOffset::ScoutRequestedFlag, 0);
            Write<std::uint8_t>(guides, NC2Layout::AIGuideManagerOffset::GuidePairSearchExhaustedFlag, 0);
            AIGuideManager_CalcVars_CreateGuideCalcArray(static_cast<std::byte*>(guides) + NC2Layout::AIGuideManagerOffset::CalculationGrid);
            Write<std::uint16_t>(guides, NC2Layout::AIGuideManagerOffset::Destination, NXBasics::IntegerRange::UInt16MaximumSigned);
            Write<std::uint16_t>(guides, NC2Layout::AIGuideManagerOffset::DestinationY, NXBasics::IntegerRange::UInt16MaximumSigned);
            Write<std::uint32_t>(guides, NC2Layout::AIGuideManagerOffset::MilitaryPointIndex, NXBasics::IntegerRange::UInt32Maximum);
        }

        // Original function: FUN_00428ae7.
        void C2AIHighLevelHandler_Init(void* handler, std::uint32_t player, bool enabled)
        {
            Write(handler, 0, player);
            Write<std::uint8_t>(handler, 4, enabled);
            AIGuideManager_Init(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager);
            Write<std::uint8_t>(handler, 0x1A, 1);
        }

        // Original function: FUN_004294cf.
        void C2AISinglePlayerHandler_Init(void* handler, std::uint32_t player, bool enabled)
        {
            AISinglePlayerHandler_Construct(handler);
            std::memset(handler, 0, 44360);
            std::memset(static_cast<std::byte*>(handler) + NC2Layout::AISinglePlayerOffset::SoldierSelfDefenceOrigins, 0, 800);
            Write<std::uint32_t>(handler, NC2Layout::AISinglePlayerOffset::TowerAttachmentCursor, NXBasics::IntegerRange::UInt32Maximum);
            Write(handler, NC2Layout::AISinglePlayerOffset::PlayerId, player);
            Write<std::uint8_t>(handler, 4, enabled);
            Write<std::uint8_t>(handler, 5, 1);
            Write<std::uint32_t>(handler, 0xAD38, 0x50);
        }

        // Original function: FUN_00407cdc.
        void AIManager_l_InitPlayerArrays(void* manager, bool enableHighLevel)
        {
            for (std::uint32_t player = 0; player < 20; ++player)
            {
                const std::uint32_t offset = player * NC2Layout::RecordSize::GameControlPlayerSigned;
                if (Read<std::uint8_t>(gGameControl, offset) == 0) continue;
                void* single = NXBasics::Runtime_AllocateWithNewHandler(45160, 1);
                Write(manager, player * 4, single);
                C2AISinglePlayerHandler_Init(single, player, Read<std::uint32_t>(gGameControl, offset + 4) == 2);
                void* high = NXBasics::Runtime_AllocateWithNewHandler(43580, 1);
                if (high != nullptr) C2AIHighLevelHandler_Construct(high);
                Write(manager, 0x50 + player * 4, high);
                const bool enabled = enableHighLevel && Read<std::uint32_t>(gGameControl, offset + 4) == 2 && Read<std::uint32_t>(gGameControl, offset + 12) != 5 && Read<std::uint32_t>(gGameControl, offset + 12) != 6;
                C2AIHighLevelHandler_Init(high, player, enabled);
            }
        }

        // Original function: FUN_00428b0a.
        void C2AIHighLevelHandler_IO_Savegame_WriteBinary(void* handler, CIoHelper* io)
        {
            io->IO_Memory_WriteBinary(handler, NC2Layout::AIHighLevelOffset::GuideManager);
            AIGuideManager_IO_Savegame_WriteBinary(static_cast<std::byte*>(handler) + NC2Layout::AIHighLevelOffset::GuideManager, io);
        }

        void RegisterAICallbacks()
        {
            CCallbackManager::sTheObjectPtr->RegisterCallback(1, AIManager_l_AI_Callback, 11, 1);
            CCallbackManager::sTheObjectPtr->RegisterCallback(0x2A, AIManager_l_AI_Callback, 11, 1);
        }
    }

    // Original function: FUN_00407c8b.
    void* __fastcall AIManager_Construct(void* manager, void* /*unusedEdx*/)
    {
        gAIManager = manager;
        std::memset(manager, 0, 160);
        AIManager_l_InitPlayerArrays(manager, true);
        RegisterAICallbacks();
        return manager;
    }

    // Original function: FUN_00407fc0.
    void __fastcall AIManager_IO_Savegame_DoSave(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        io->IO_Tool_Chunk_StartCreation(SwitchIds::SavegameChunk::AI, 5);
        io->IO_Memory_WriteLong(20);
        for (std::uint32_t player = 0; player < 20; ++player)
        {
            void* handler = Read<void*>(manager, player * 4);
            io->IO_Memory_WriteByte(handler != nullptr);
            if (handler != nullptr)
            {
                io->IO_Memory_WriteBinary(handler, 0xB068);
                C2AIHighLevelHandler_IO_Savegame_WriteBinary(Read<void*>(manager, 0x50 + player * 4), io);
            }
        }
        io->IO_Tool_Chunk_FinishAndWrite();
    }

    // Original function: FUN_00408025; Windows accepts versions 3, 4 and 5.
    void* __fastcall AIManager_ConstructFromSavegame(void* manager, void* /*unusedEdx*/, CIoHelper* io)
    {
        gAIManager = manager;
        std::memset(manager, 0, 160);
        const std::uint32_t version = io->mReadHeader.version;
        if (version == 3 || version == 5)
        {
            const std::uint32_t players = io->IO_File_ReadLong();
            // No original upper-bound check on the serialized player count.
            for (std::uint32_t player = 0; player < players; ++player)
                if (io->IO_File_ReadByte() != 0)
                {
                    void* single = NXBasics::Runtime_AllocateWithNewHandler(45160, 1);
                    if (single != nullptr) AISinglePlayerHandler_Construct(single);
                    Write(manager, player * 4, single);
                    io->IO_File_ReadBinary(single, version == 3 ? 0xA5B8 : 0xB068);
                    if (version == 3) io->IO_File_Skip(0x3C08);
                    void* high = NXBasics::Runtime_AllocateWithNewHandler(43580, 1);
                    if (high != nullptr)
                    {
                        if (version == 3) C2AIHighLevelHandler_Construct(high);
                        else C2AIHighLevelHandler_ConstructFromSavegame(high, io);
                    }
                    Write(manager, 0x50 + player * 4, high);
                    if (version == 3) C2AIHighLevelHandler_Init(high, player, false);
                }
        }
        else if (version == 4)
        {
            AIManager_l_InitPlayerArrays(manager, false);
            io->IO_File_Chunk_Skip();
        }
        RegisterAICallbacks();
        return manager;
    }
}
